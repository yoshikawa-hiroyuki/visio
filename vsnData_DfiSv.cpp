//
// vsnData_DfiSv
//
#ifdef __GNUG__
#pragma implementation
#pragma interface
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif
#include "wx/progdlg.h"

#include "vsnData_DfiSv.h"
#include "vsnMethod_Dfi.h"
#include "vsnData_P3dF.h"
#include "vsnApp.h"
#include "vsnError.h"
#include "utilEndian.h"

using namespace std;
using namespace CES;
using namespace VSN;


/* constructors / destructor */

vsnData_DfiSv::vsnData_DfiSv(const string& name)
  : vsnData_Sv(name), p_refDataDfi(NULL), m_pData(NULL), m_checkingMinMax(false)
{
  m_currentStepIdx --;
}

vsnData_DfiSv::~vsnData_DfiSv() {
  if ( m_pData )
    DeAllocate(m_pData);
}


/* methods */

bool vsnData_DfiSv::init(vsnData_Dfi* pddfi, const Vec3<size_t>* regionIdx) {
  register int i, j;
  if ( ! pddfi || ! regionIdx ) return false;
  m_ready = false;

  p_refDataDfi = pddfi;
  Vec3<size_t> gDiv = p_refDataDfi->m_globalDiv;
  if ( regionIdx[0][0] >= gDiv[0] || regionIdx[1][0] >= gDiv[0] ||
       regionIdx[0][1] >= gDiv[1] || regionIdx[1][1] >= gDiv[1] ||
       regionIdx[0][2] >= gDiv[2] || regionIdx[1][2] >= gDiv[2] ) return false;
  if ( regionIdx[0][0] > regionIdx[1][0] ||
       regionIdx[0][1] > regionIdx[1][1] ||
       regionIdx[0][2] > regionIdx[1][2] ) return false;
  m_regionIdx[0] = regionIdx[0]; m_regionIdx[1] = regionIdx[1];

  // setup stpList
  m_numStps = p_refDataDfi->m_timeSliceList.size();
  if ( m_numStps < 1 ) return false;
  m_stpList.resize(m_numStps);
  for ( i = 0; i < m_numStps; i++ ) {
    m_stpList[i].step = p_refDataDfi->m_timeSliceList[i].step;
    m_stpList[i].time = p_refDataDfi->m_timeSliceList[i].time;
  } // end of for(i)

  // component
  m_dataLen = p_refDataDfi->m_dataLen;
  if ( m_dataLen < 1 ) return false;

  // get total dims of region as m_dims
  m_dims[0] = m_dims[1] = m_dims[2] = 0;
#if 0
  for ( i = m_regionIdx[0][0]; i <= m_regionIdx[1][0]; i++ ) {
    m_dims[0] += p_refDataDfi->m_brickList[i].voxelSize[0];
  }
  for ( i = m_regionIdx[0][1]; i <= m_regionIdx[1][1]; i++ ) {
    j = p_refDataDfi->m_globalDiv[0] * i;
    m_dims[1] += p_refDataDfi->m_brickList[j].voxelSize[1];
  }
  for ( i = m_regionIdx[0][2]; i <= m_regionIdx[1][2]; i++ ) {
    j = p_refDataDfi->m_globalDiv[0] * p_refDataDfi->m_globalDiv[1] * i;
    m_dims[2] += p_refDataDfi->m_brickList[j].voxelSize[2];
  }
#else
  Vec3<size_t> en_max;
  for ( i = 0; i < p_refDataDfi->m_brickList.size(); i++ ) {
    Vec3<size_t> en = p_refDataDfi->m_brickList[i].headIdx
      + p_refDataDfi->m_brickList[i].voxelSize;
    if ( i == 0 ) {
      en_max = en;
      continue;
    }
    for ( j = 0; j < 3; j++ )
      if ( en_max[j] < en[j] ) en_max[j] = en[j];
  } // end of for(i)
  m_dims = en_max;
#endif

  // set min/max
  if ( ! checkMinMax(true, vsnApp::GetApp()->isChkProgress()) )
    return false;

  // load the first file
  if ( ! setCurrentStepIdx(0) )
    return false;

  // set bbox
  if ( ! updateBbox(0) )
    return false;

  m_ready = true;
  return true;
}

const float* const vsnData_DfiSv::getCoord(const size_t stp) {
  if ( ! m_ready || ! p_refDataDfi ) return NULL;
  register size_t dimSz = m_dims[0]*m_dims[1]*m_dims[2];
  if ( dimSz < 1 ) return NULL;
  if ( dimSz != nVerts ) {
    if ( ! alcVerts(dimSz) ) return NULL;
  }

  register int i, j, k, idx = 0;
  Vec3<float> pit = p_refDataDfi->getPitch();
  for ( k = 0; k < m_dims.m_v[2]; k++ )
    for ( j = 0; j < m_dims.m_v[1]; j++ )
      for ( i = 0; i < m_dims.m_v[0]; i++ ) {
	_verts[idx][0] = _bbox[0].m_v[0] + pit.m_v[0] * i;
	_verts[idx][1] = _bbox[0].m_v[1] + pit.m_v[1] * j;
	_verts[idx][2] = _bbox[0].m_v[2] + pit.m_v[2] * k;
	idx ++;
      } // end of for(i)

  return (float*)_verts;  
}

bool vsnData_DfiSv::setCurrentStepIdx(const size_t stpIdx) {
  if ( stpIdx == m_currentStepIdx && m_ready ) return true;
  if ( ! vsnTimeSeriesDataIF::setCurrentStepIdx(stpIdx) )
    return false;

  if ( ! p_refDataDfi ) return false;
  register size_t stp = stpIdx;
  if ( stp >= m_numStps ) return false;
  if ( p_refDataDfi->m_brickType == vsnData_Dfi::Brick_NONE ||
       p_refDataDfi->m_filePrefix.empty() ) return false;
  size_t dimSz = m_dims[0] * m_dims[1] * m_dims[2];
  size_t datSz = dimSz * m_dataLen;
  if ( datSz < 1 ) return false;
  m_pData = (float*)ReAllocate(m_pData, sizeof(float)*datSz);
  if ( ! m_pData ) return false;

  // set mask
  if ( p_refDataDfi->m_activeDomainFlag ) {
    bool needSetZero = (m_mask != NULL);
    m_mask = (unsigned char*)ReAllocate(m_mask, dimSz);
    if ( ! m_mask ) return false;
    if ( needSetZero )
      memset(m_mask, 0, dimSz);
  }

  // read in
  string base_dir = p_refDataDfi->getBaseDir();
  register size_t ii, jj, kk, ll, pi;
  register size_t i, j, k, idx;
  Vec3<size_t> index;
  Vec3<size_t> gDiv = p_refDataDfi->m_globalDiv;
  size_t gDivSz = gDiv[0] * gDiv[1] * gDiv[2];
  if ( gDivSz < 1 ) return false;
  idx = gDiv[0]*gDiv[1]*m_regionIdx[0][2] + gDiv[0]*m_regionIdx[0][1]
    + m_regionIdx[0][0];
  vsnData_Dfi::BrickInfo& bi0 = p_refDataDfi->m_brickList[idx];

  float* pfDat = NULL;
  for ( k = m_regionIdx[0][2]; k <= m_regionIdx[1][2]; k++ )
    for ( j = m_regionIdx[0][1]; j <= m_regionIdx[1][1]; j++ )
      for ( i = m_regionIdx[0][0]; i <= m_regionIdx[1][0]; i++ ) {
	idx = gDiv[0]*gDiv[1]*k + gDiv[0]*j + i;
	vsnData_Dfi::BrickInfo& bi = p_refDataDfi->m_brickList[idx];
	if ( bi.ID < 0 && m_mask ) {
	  for ( kk = 0; kk < bi.voxelSize[2]; kk++ )
	    for ( jj = 0; jj < bi.voxelSize[1]; jj++ )
	      for ( ii = 0; ii < bi.voxelSize[0]; ii++ ) {
		index = Vec3<size_t>(ii, jj, kk) - bi0.headIdx;
		pi = m_dims[0]*m_dims[1]*index[2] +m_dims[1]*index[1] +index[0];
		m_mask[pi] = 1;
	      } // end of for(ii)
	}
	else {
	  string path = vsnPath_concat(base_dir, p_refDataDfi->m_dirPath);
	  path = vsnPath_concat(path, p_refDataDfi->m_filePrefix);
	  char buff[32];
	  size_t tstp = m_stpList[stp].step;
	  if ( gDivSz == 1 ) {
	    sprintf(buff, "_%010lu", tstp);
	  } else {
	    if ( p_refDataDfi->m_filenameStepRank )
	      sprintf(buff, "_%010lu_id%06lu", tstp, idx);
	    else
	      sprintf(buff, "_id%06lu_%010lu", idx, tstp);
	  }
	  path += string(buff);
	  if ( p_refDataDfi->m_brickType == vsnData_Dfi::Brick_Sph )
	    path += string(".sph");
	  else
	    path += string(".fun");
	  path = vsnPath_normalize(path);

	  size_t brkSz = bi.voxelSize[0] * bi.voxelSize[1] * bi.voxelSize[2]
	    * m_dataLen;
	  pfDat = (float*)ReAllocate(pfDat, sizeof(float)*brkSz);
	  if ( ! pfDat ) return false;

	  bool rret;
	  if ( p_refDataDfi->m_brickType == vsnData_Dfi::Brick_Sph )
	    rret = ReadinSph(path, pfDat, p_refDataDfi->m_numGc);
	  else
	    rret = ReadinP3dF(path, pfDat, p_refDataDfi->m_numGc);
	  if ( ! rret ) return false;

	  for ( kk = 0; kk < bi.voxelSize[2]; kk++ )
	    for ( jj = 0; jj < bi.voxelSize[1]; jj++ )
	      for ( ii = 0; ii < bi.voxelSize[0]; ii++ ) {
		index = bi.headIdx + Vec3<size_t>(ii, jj, kk);
		index = index - bi0.headIdx;
		pi = m_dims[0]*m_dims[1]*index[2] +m_dims[0]*index[1] +index[0];
		idx = bi.voxelSize[0]*bi.voxelSize[1]*kk
		  + bi.voxelSize[0]*jj + ii;
		for ( ll = 0; ll < m_dataLen; ll++ ) {
		  m_pData[pi*m_dataLen + ll] = pfDat[idx*m_dataLen + ll];
		} // end of for(ll)
	      } // end of for(ii)

	}
      } // end of for(i)

  if ( pfDat ) {
    DeAllocate(pfDat);
  }

  // update all TimeSeriesMethods
  bool ret = true;
  if ( m_checkingMinMax ) return ret;
  size_t numMtds = getNumMethod();
  for ( register size_t i = 0; i < numMtds; i++ ) {
    vsnTimeSeriesMethodIF* pMtdTS
      = dynamic_cast<vsnTimeSeriesMethodIF*>(getMethod(i));
    if ( ! pMtdTS ) continue;
    if ( ! pMtdTS->updateStep(m_currentStepIdx, /* force */ false) )
      ret = false;
  } // end of for(i)

  return ret;
}

const float* const vsnData_DfiSv::getData(const size_t stp) {
  if ( stp != m_currentStepIdx )
    return NULL;
  return m_pData;
}

bool vsnData_DfiSv::getVectorMaxLen(const Vec3<int>& vidx, float& vml) {
  if ( ! m_ready || ! p_refDataDfi ) return false;
  return p_refDataDfi->getVectorMaxLen(vidx, vml);
}

bool vsnData_DfiSv::checkMinMax(const bool wholeStp, const bool progress) {
  if ( ! p_refDataDfi ) return false;
#if 0 // using Dfi MinMax
  if ( ! p_refDataDfi->checkMinMax(wholeStp, progress) )
    return false;
  m_hasMinMax = true;
  m_minVals = p_refDataDfi->m_minVals;
  m_maxVals = p_refDataDfi->m_maxVals;
  m_minVecLen012 = p_refDataDfi->m_minVecLen012;
  m_maxVecLen012 = p_refDataDfi->m_maxVecLen012;
#else // check local MinMax
  if ( m_hasMinMax && m_minVals.size() == m_dataLen )
    return true;

  register size_t i, j, k, idx, dimSz = m_dims[0]*m_dims[1]*m_dims[2];
  string errMsg = string("Data_DfiSv[") + getName() + string("]: ");

  // progressive dialog
  vsnViewFrame* pw = vsnApp::GetApp()->getViewFrame(0);
  wxProgressDialog* progDlg = NULL;
  char msgBuff[512];
  wxString prgMsg;
  if ( progress ) {
    sprintf(msgBuff, "Data_Sv: checking min/max of file #1 of %lu", m_numStps);
    prgMsg = vsnApp::ConvSysToWx(msgBuff);
    progDlg = new wxProgressDialog(wxT("check min/max value"),
                                   prgMsg, m_numStps, pw,
                                   wxPD_CAN_ABORT | wxPD_APP_MODAL |
                                   wxPD_AUTO_HIDE | wxPD_SMOOTH);
    progDlg->Update(0, prgMsg);
  }
  m_checkingMinMax = true;

  // data of the first step
  if ( ! setCurrentStepIdx(0) ) {
    if ( progDlg ) progDlg->Destroy();
    errMsg += string("can't load the first step data : ");
    errMsg += m_readinErrMsg;
    ErrMsg(MsgERR, errMsg);
    m_checkingMinMax = false;
    return false;
  }
  float* pd = m_pData;
  m_minVals.resize(m_dataLen);
  m_maxVals.resize(m_dataLen);

  for ( k = 0; k < m_dataLen; k++ )
    m_minVals[k] = m_maxVals[k] = pd[k];
  if ( m_dataLen >= 3 )
    m_maxVecLen012 = CES::Vec3<float>(pd).Length();

  for ( j = 1; j < dimSz; j++ ) {
    for ( k = 0; k < m_dataLen; k++ ) {
      idx = m_dataLen * j + k;
      if ( m_minVals[k] > pd[idx] ) m_minVals[k] = pd[idx];
      if ( m_maxVals[k] < pd[idx] ) m_maxVals[k] = pd[idx];
    } // end of for(k)
    if ( m_dataLen >= 3 ) {
      float vl = CES::Vec3<float>(&pd[m_dataLen * j]).Length();
      if ( m_maxVecLen012 < vl ) m_maxVecLen012 = vl;
    }
  } // end of for(j)
  if ( ! wholeStp ) {
    if ( progDlg ) progDlg->Destroy();
    m_checkingMinMax = false;
    return true;
  }

  // follow steps
  bool badFileExists = false;
  errMsg += string("can't load data of the step(s) below:\n");
  for ( i = 1; i < m_numStps; i++ ) {
    sprintf(msgBuff, "Data_Sv: checking min/max of file #%lu of %lu",
                  i+1, m_numStps);
    prgMsg = vsnApp::ConvSysToWx(msgBuff);
    if ( progDlg && ! progDlg->Update(i, prgMsg) ) break;

    if ( ! setCurrentStepIdx(i) ) {
      char txt[64]; sprintf(txt, "  step#%lu ", i);
      errMsg += txt;
      errMsg += string(" : ") + m_readinErrMsg + string("\n");
      badFileExists = true;
      continue;
    }
    pd = m_pData;
    for ( j = 0; j < dimSz; j++ ) {
      for ( k = 0; k < m_dataLen; k++ ) {
        idx = m_dataLen * j + k;
        if ( m_minVals[k] > pd[idx] ) m_minVals[k] = pd[idx];
        if ( m_maxVals[k] < pd[idx] ) m_maxVals[k] = pd[idx];
      } // end of for(k)
      if ( m_dataLen >= 3 ) {
        idx = m_dataLen * j;
        CES::Vec3<float> vv(pd[idx], pd[idx +1], pd[idx +2]);
        float vl = vv.Length();
        if ( m_maxVecLen012 < vl ) m_maxVecLen012 = vl;
      }
    } // end of for(j)
  } // end of for(i)

  if ( badFileExists ) {
    ErrMsg(MsgWARN, errMsg);
  }

  if ( progDlg ) progDlg->Destroy();
  m_hasMinMax = true;

  m_checkingMinMax = false;
#endif
  return true;
}

bool vsnData_DfiSv::updateBbox(const int stp) {
  if ( ! p_refDataDfi ) return false;
  Vec3<float> pit = p_refDataDfi->getPitch();
  Vec3<size_t> gDiv = p_refDataDfi->m_globalDiv;
  size_t idx = gDiv[0]*gDiv[1]*m_regionIdx[0][2] + gDiv[0]*m_regionIdx[0][1]
    + m_regionIdx[0][0];
  vsnData_Dfi::BrickInfo& bi0 = p_refDataDfi->m_brickList[idx];

  const Vec3<float>* pbb = p_refDataDfi->getBbox();
  _bbox[0] = pbb[0];
  _bbox[0][0] += pit[0] * bi0.headIdx[0];
  _bbox[0][1] += pit[1] * bi0.headIdx[1];
  _bbox[0][2] += pit[2] * bi0.headIdx[2];

  _bbox[1] = _bbox[0];
  _bbox[1][0] += pit[0] * (m_dims[0] - 1);
  _bbox[1][1] += pit[1] * (m_dims[1] - 1);
  _bbox[1][2] += pit[2] * (m_dims[2] - 1);

  return true;
}


/* from vsnDataObj */

bool vsnData_DfiSv::init(const std::string& path, xmlNodePtr xnp) {
  static xmlChar* xs;
  if ( ! xnp ) return false;
  
  // is 'data' node?
  if ( strcmp((const char*)xnp->name, "data") ) return false;

  // check data type
  if ( xs ) xmlFree(xs);
  xs = xmlGetProp(xnp, (const xmlChar*)"type");
  if ( ! xs || strlen((const char*)xs) < 1 || strcmp((const char*)xs, "DfiSv") )
    return false;

  // get children node
  xmlNodePtr cur = xnp->xmlChildrenNode;

  // tarverse
  string dfiName;
  Vec3<size_t> regIdx[2];
  while ( cur ) {
    if ( (cur = vsnIoObject::SkipCommentXML(cur)) == NULL )
      break;

    // search "brick"
    if ( !strcmp((const char*)cur->name, "brick") ) {
      if ( xs ) xmlFree(xs);
      xs = xmlGetProp(cur, (const xmlChar*)"dfi");
      if ( ! xs || strlen((const char*)xs) < 1 )
	goto _NEXT_XML_NODE;
      dfiName = (const char*)xs;

      xmlNodePtr cur2 = cur->xmlChildrenNode;
      while ( cur2 ) {
	if ( (cur2 = vsnIoObject::SkipCommentXML(cur2)) == NULL )
	  break;

	if ( !strcmp((const char*)cur2->name, "head") ) {
	  regIdx[0] = ParseAttrXYZ(cur2);
	}
	else if ( !strcmp((const char*)cur2->name, "tail") ) {
	  regIdx[1] = ParseAttrXYZ(cur2);
	}

	cur2 = cur2->next;
      } // end of while(cur2)
    } // end of "brick"

  _NEXT_XML_NODE:
    cur = cur->next;
  } // end of while(cur)

  // get DataDfi
  if ( dfiName.empty() ) return false;
  vsnScene* psc = getAncestorScene();
  if ( ! psc ) return false;
  vsnData_Dfi* pddfi = dynamic_cast<vsnData_Dfi*>(psc->getNode(dfiName));
  if ( ! pddfi ) return false;
  
  if ( ! init(pddfi, regIdx) ) return false;
  vsnMethod_Dfi_brickLoader* ploader
    = dynamic_cast<vsnMethod_Dfi_brickLoader*>(pddfi->getBrickLoader());
  if ( ! ploader ) return false;
  if ( ! ploader->addloadedDataRef(this) )
    return false;

  setXmlNode(xnp);
  return true;
}


// STATIC
bool vsnData_DfiSv::ReadinSph(const string& path, float* pd, const size_t gc) {
  unsigned char buff[32];
  int* pib = (int*)(&buff[4]);
  long long* plb = (long long*)(&buff[4]);
  float* pfb = (float*)(&buff[4]);
  double* pdb = (double*)(&buff[4]);
  Vec3<size_t> dims;
  size_t dataLen;
  if ( ! pd ) return false;

  // open the file
  FILE* fp = fopen(path.c_str(), "rb");
  if ( ! fp ) {
    return false;
  }
  EMatchType emt = MatchEndian(fp, 8);
  if ( emt == UnKnown ) {
    fclose(fp); return false;
  }

  // read headers
  if ( fread(buff, 1, 16, fp) < 16 ) {
    fclose(fp); return false;
  }
  if ( emt == UnMatch ) BSWAPVEC(pib, 2);
  switch ( pib[0] ) {
  case 1: // scalar
    dataLen = 1;
    break;
  case 2: // vector
    dataLen = 3;
    break;
  default:
    fclose(fp); return false;
  }
  
  bool dblPrec = false;
  switch ( pib[1] ) {
  case 1: break; // single precision
  case 2: dblPrec = true; break; // double precision
  default:
    fclose(fp); return false;
  }

  // read dims, org, pitch, time
  if ( dblPrec ) {
    if ( fread(buff, 1, 32, fp) < 32 ) {
      fclose(fp); return false;
    }
    if ( emt == UnMatch ) LBSWAPVEC(plb, 3);
    dims[0] = (size_t)plb[0];
    dims[1] = (size_t)plb[1];
    dims[2] = (size_t)plb[2];
    if ( dims[0]*dims[1]*dims[2] < 1 ) {
      fclose(fp); return false;
    }
    if ( fseek(fp, 88 + 4, SEEK_CUR) != 0 ) {
      fclose(fp); return false;
    }
  } else {
    if ( fread(buff, 1, 20, fp) < 20 ) {
      fclose(fp); return false;
    }
    if ( emt == UnMatch ) BSWAPVEC(pib, 3);
    dims[0] = (size_t)pib[0];
    dims[1] = (size_t)pib[1];
    dims[2] = (size_t)pib[2];
    if ( dims[0]*dims[1]*dims[2] < 1 ) {
      fclose(fp); return false;
    }
    if ( fseek(fp, 56 + 4, SEEK_CUR) != 0 ) {
      fclose(fp); return false;
    }
  }

  // alloc data
  if ( dims[0] <= 2*gc || dims[1] <= 2*gc || dims[2] <= 2*gc ) {
    fclose(fp); return false;
  }
  size_t dimSz = dims[0] * dims[1] * dims[2] * dataLen;
  float* pfData = new float[dimSz];
  if ( ! pfData ) {
    fclose(fp); return false;
  }

  // read data
  if ( dblPrec ) {
    register size_t k = dims[0] * dataLen;
    double* pDblData = new double[k];
    if ( ! pDblData ) {
      fclose(fp); return false;
    }
    register size_t dimSzJK = dims[1] * dims[2];
    register size_t i, j, idx = 0;
    for ( i = 0; i < dimSzJK; i++ ) {
      if ( fread(pDblData, sizeof(double), k, fp) != k ) {
        delete [] pDblData;
        fclose(fp); return false;
      }
      if ( emt == UnMatch ) LBSWAPVEC(pDblData, k);
      for ( j = 0; j < k; j++ ) {
        pfData[idx++] = (float)pDblData[j];
      } // end of for(j)
    } // end of for(i)
    delete [] pDblData;
  }
  else {
    if ( fread(pfData, 4, dimSz, fp) < dimSz ) {
      fclose(fp); return false;
    }
    if ( emt == UnMatch ) BSWAPVEC(pfData, dimSz);
  }
  fclose(fp);

  if ( gc == 0 ) {
    memcpy(pd, pfData, 4 * dimSz);
    delete [] pfData;
    return true;
  }

  // omit gc
  Vec3<size_t> xdims = dims - Vec3<size_t>(2*gc, 2*gc, 2*gc);
  size_t xdimSz = xdims[0] * xdims[1] * xdims[2] * dataLen;
  float* xpfData = pd;

  register size_t i, j, k, l, idx0, idx = 0;
  for ( k = gc; k < dims[2] - gc; k++ )
    for ( j = gc; j < dims[1] - gc; j++ )
      for ( i = gc; i < dims[0] - gc; i++ ) {
	idx0 = dims[0]*dims[1]*k + dims[0]*j + i;
	for ( l = 0; l < dataLen; l++, idx++ )
	  xpfData[idx] = pfData[idx0*dataLen + l];
      } // end of for(i)

  delete [] pfData;
  return true;
}

// STATIC
bool vsnData_DfiSv::ReadinP3dF(const string& path, float* pd, const size_t gc) {
  unsigned char buff[24];
  int* pib = (int*)(&buff[4]);
  float* pfb = (float*)(&buff[4]);
  Vec3<size_t> dims;
  size_t dataLen;
  if ( ! pd ) return false;

  // open the file
  FILE* fp = fopen(path.c_str(), "rb");
  if ( ! fp ) {
    return false;
  }
  vsnData_P3dF::GridType grt;
  EMatchType emt;
  if ( ! vsnData_P3dF::CheckFileType(fp, grt, emt) ) {
    fclose(fp); return false;
  }

  // read headers ((ngrid), dims, veclen)
  int ngrid = 1;
  if ( grt == vsnData_P3dF::P3dF_SingleGrid ) {
    if ( fread(buff, 1, 24, fp) < 24 ) {
      fclose(fp); return false;
    }

    if ( fseek(fp, 4, SEEK_CUR) != 0 ) {
      fclose(fp); return false;
    }
  } else {
    if ( fread(buff, 1, 12, fp) < 12 ) {
      fclose(fp); return false;
    }
    ngrid = pib[0];
    if ( emt == UnMatch ) BSWAP32(ngrid);
    if ( ngrid < 1 ) {
      fclose(fp); return false;
    }

    if ( fread(buff, 1, 24, fp) < 24 ) {
      fclose(fp); return false;
    }

    if ( fseek(fp, (ngrid-1)*24 + 4, SEEK_CUR) != 0 ) {
      fclose(fp); return false;
    }
  }
  if ( emt == UnMatch ) BSWAPVEC(pib, 4);
  dims[0] = (size_t)pib[0];
  dims[1] = (size_t)pib[1];
  dims[2] = (size_t)pib[2];
  dataLen = (size_t)pib[3];

  // alloc data
  if ( dims[0] <= 2*gc || dims[1] <= 2*gc || dims[2] <= 2*gc ) {
    fclose(fp); return false;
  }
  size_t dimSz = dims[0] * dims[1] * dims[2];
  size_t dataSz = dimSz * dataLen;
  if ( dimSz < 1 || dataLen < 1 ) {
    fclose(fp); return false;
  }
  float* pfData = new float[dataSz];
  if ( ! pfData ) {
    fclose(fp); return false;
  }
  float* dtbuf = new float[dimSz];
  if ( ! dtbuf ) {
    fclose(fp); return false;
  }

  // read data
  register int n;
  for ( n = 0; n < dataLen; n++ ) {
    if ( fread(dtbuf, 4, dimSz, fp) < dimSz ) {
      fclose(fp); return false;
    }
    if ( emt == UnMatch ) BSWAPVEC(dtbuf, dimSz);

    register size_t i;
    for ( i = 0; i < dimSz; i++ ) {
      pfData[i*dataLen + n] = dtbuf[i];
    } // end of for(i)
  } //  end of for(n)
  fclose(fp);
  delete [] dtbuf;

  if ( gc == 0 ) {
    memcpy(pd, pfData, 4 * dataSz);
    delete [] pfData;
    return true;
  }

  // omit gc
  Vec3<size_t> xdims = dims - Vec3<size_t>(2*gc, 2*gc, 2*gc);
  size_t xdimSz = xdims[0] * xdims[1] * xdims[2] * dataLen;
  float* xpfData = pd;

  register size_t i, j, k, l, idx0, idx = 0;
  for ( k = gc; k < dims[2] - gc; k++ )
    for ( j = gc; j < dims[1] - gc; j++ )
      for ( i = gc; i < dims[0] - gc; i++ ) {
	idx0 = dims[0]*dims[1]*k + dims[0]*j + i;
	for ( l = 0; l < dataLen; l++, idx++ )
	  xpfData[idx] = pfData[idx0*dataLen + l];
      } // end of for(i)

  delete [] pfData;
  return true;
}

// STATIC
Vec3<size_t> vsnData_DfiSv::ParseAttrXYZ(xmlNodePtr xnp) {
  static xmlChar* xs;
  Vec3<size_t> v;
  if ( ! xnp ) return v;

  if ( xs ) xmlFree(xs);
  xs = xmlGetProp(xnp, (const xmlChar*)"x");
  if ( xs && strlen((const char*)xs) > 0 ) {
    long x = atol((const char*)xs);
    if ( x >= 0 ) v[0] = (size_t)x;
  }
  if ( xs ) xmlFree(xs);
  xs = xmlGetProp(xnp, (const xmlChar*)"y");
  if ( xs && strlen((const char*)xs) > 0 ) {
    long y = atol((const char*)xs);
    if ( y >= 0 ) v[1] = (size_t)y;
  }
  if ( xs ) xmlFree(xs);
  xs = xmlGetProp(xnp, (const xmlChar*)"z");
  if ( xs && strlen((const char*)xs) > 0 ) {
    long z = atol((const char*)xs);
    if ( z >= 0 ) v[2] = (size_t)z;
  }

  return v;
}
