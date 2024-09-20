//
// vsnData_Dfi
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

#include "vsnData_Dfi.h"
#include "vsnMethod_Dfi.h"
#include "vsnMethod_label.h"
#include "vsnData_DfiSv.h"
#include "vsnApp.h"
#include "vsnError.h"
#include "utilEndian.h"
#include "TextParser.h"

using namespace std;
using namespace CES;
using namespace VSN;


/* constructors / destructor */

vsnData_Dfi::vsnData_Dfi(const string& name)
  : vsnDataObj(name), m_brickType(vsnData_Dfi::Brick_NONE),
    m_useTsDir(false), m_filenameStepRank(false), m_numGc(0),
    m_activeDomainFlag(NULL)
{
}

vsnData_Dfi::~vsnData_Dfi() {
  reset();
}


/* methods */

void vsnData_Dfi::reset() {
  m_path = "";
  m_baseDir = "";
  m_brickType = Brick_NONE;
  m_useTsDir = false;
  m_dirPath = "";
  m_filePrefix = "";
  m_filenameStepRank = false;
  m_numGc = 0;
  m_procPath = "";
  m_timeSliceList.clear();
  m_globalDims = Vec3<size_t>();
  m_globalDiv = Vec3<size_t>();
  m_globalOrig = Vec3<float>();
  m_globalRegn = Vec3<float>();
  m_activeDomainPath = "";
  if ( m_activeDomainFlag ) {
    delete [] m_activeDomainFlag; m_activeDomainFlag = NULL;
  }
  m_brickList.clear();
}

Vec3<float> vsnData_Dfi::getPitch() const {
  /* DFIのGlobalRegionは、仮想セルを除いた計算範囲で、セル数×ピッチなので、
     SPHの場合のピッチ計算(bboxLen / (dims-1))と異なり、GlobalRegion/GlobalDims
     となる
   */
  Vec3<float> pit;
  if ( m_globalDims[0] > 0 )
    pit[0] = m_globalRegn[0] / m_globalDims[0];
  if ( m_globalDims[1] > 0 )
    pit[1] = m_globalRegn[1] / m_globalDims[1];
  if ( m_globalDims[2] > 0 )
    pit[2] = m_globalRegn[2] / m_globalDims[2];
  return pit;
}

vsnMethodObj* vsnData_Dfi::getBrickLoader() {
  register int i, n = getNumChildren();
  for ( i = 0; i < n; i++ ) {
    vsnMethodObj* pm = dynamic_cast<vsnMethodObj*>(getChild(i));
    if ( ! pm ) continue;
    if ( pm->getMethodType() == string("brickLoader") )
      return pm;
  } // end of for(i)
  return NULL;
}


/* from vsnDataObj */

std::deque<std::string> vsnData_Dfi::getSupportMethodList() const {
  deque<string> retStrs;
  retStrs.push_back(string("brickLoader"));
  retStrs.push_back(string("info"));
  retStrs.push_back(string("label"));
  return retStrs;
}

vsnMethodObj* vsnData_Dfi::addNewMethod(const std::string& mtype,
					const std::string& mname) {
  string modType(mtype);
  vsnMethodObj* pMethod = NULL;
  string msgHdr
    = string("DataDfi[") + getName() + string("]: addNewMethod: ");

  // instance method
  if ( modType == string("info") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_Dfi_info());
  } // end of info
  else if ( modType == string("label") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_label());
  } // end of label
  else if ( modType == string("brickLoader") ) {
    if ( getBrickLoader() ) {
      ErrMsg(MsgERR, msgHdr + string("can't add brickLoader:")
	     + string("another brickLoader has already registed"));
      return NULL;
    }
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_Dfi_brickLoader());
  } // end of brickLoader

  if ( ! pMethod ) {
    ErrMsg(MsgERR, msgHdr + string("can't create ")
	   + modType + string(" method"));
    return NULL;
  }

  if ( ! mname.empty() )
    pMethod->setName(mname);

  if ( ! addMethod(pMethod) ) {
    ErrMsg(MsgERR, msgHdr + string("can't regist ")
           + modType + string(" method"));
    delete pMethod;
    return NULL;
  }

  return pMethod;
}

// STATIC
int vsnData_Dfi::searchStrVector(const vector<string>& list, const string& x) {
  vector<string>::const_iterator it;
  register int idx;
  for ( it = list.begin(), idx = 0; it != list.end(); it++, idx++ )
    if ( *it == x ) return idx;
  return -1;
}

bool vsnData_Dfi::readActiveSubdomainFile() {
  const char mgkc[] = "SBDM";
  if ( m_activeDomainFlag ) {
    delete [] m_activeDomainFlag; m_activeDomainFlag = NULL;
  }
  if ( m_activeDomainPath.empty() ) return false;
  size_t divSz = m_globalDiv[0] * m_globalDiv[1] * m_globalDiv[2];
  if ( divSz < 1 ) return false;

  string xpath = vsnPath_concat(m_baseDir, m_activeDomainPath);
  if ( xpath.empty() ) return false;
  FILE *fp = fopen(xpath.c_str(), "rb");
  if ( ! fp ) return false;

  EMatchType emt = UnKnown;
  int ident;
  if( fread( &ident, sizeof(int), 1, fp ) != 1 ) {
    fclose(fp); return false;
  }
  if ( ident == (mgkc[3]<<24) + (mgkc[2]<<16) + (mgkc[1]<<8) + mgkc[0] )
    emt = Match;
  else if ( ident == (mgkc[0]<<24) + (mgkc[1]<<16) + (mgkc[2]<<8) + mgkc[3] )
    emt = UnMatch;
  else {
    fclose(fp); return false;
  }

  int div[3];
  if( fread( div, sizeof(int), 3, fp ) != 3 ) {
    fclose(fp); return false;
  }
  if ( emt == UnMatch ) BSWAPVEC(div,3);
  if ( div[0] != m_globalDiv[0] ||
       div[1] != m_globalDiv[1] || div[2] != m_globalDiv[2] ) {
    fclose(fp); return false;
  }

  m_activeDomainFlag = new unsigned char(divSz);
  if ( ! m_activeDomainFlag ) {
    fclose(fp); return false;
  }
  if( fread(m_activeDomainFlag, sizeof(unsigned char), divSz, fp ) != divSz ) {
    delete [] m_activeDomainFlag; m_activeDomainFlag = NULL;
    fclose(fp); return false;
  }

  fclose(fp);
  return true;
}


bool vsnData_Dfi::init(const string& path, xmlNodePtr xnp) {
  static xmlChar* xs;

  m_ready = false;
  m_path = string("");
  m_baseDir = string("");

  if ( BaseName(path, string(""), vsnPath_getDelimChar())
       == string(VSN::vsn_seqfile) ) {
    // not supported
    return false;
  }

  register int idx, i, j;
  string targPath, valStr;
  int ival; float fval;
  TextParser tp;
  vector<string> labels;

  reset();

  // load MAIN.dfi file
  if ( tp.read(path) != 0 )
    return false;
  if ( tp.getAllLabels(labels) != 0 )
    return false;
  m_baseDir = DirName(path, vsnPath_getDelimChar());

  targPath = "/FileInfo/FileFormat";
  if ( (idx = searchStrVector(labels, targPath)) < 0 )
    return false;
  if ( ! tp.getInspectedValue(targPath, valStr) )
    return false;
  if ( valStr == "sph" )
    m_brickType = Brick_Sph;
  else if ( valStr == "plot3d" )
    m_brickType = Brick_P3dF;
  else
    return false;
  
  targPath = "/FileInfo/TimeSliceDirectory";
  if ( (idx = searchStrVector(labels, targPath)) >= 0 ) {
    if ( tp.getInspectedValue(targPath, valStr) )
      m_useTsDir = (valStr == "on");
  }

  targPath = "/FileInfo/DirectoryPath";
  if ( (idx = searchStrVector(labels, targPath)) >= 0 ) {
    if ( tp.getInspectedValue(targPath, valStr) && ! valStr.empty() )
      m_dirPath = valStr;
  }

  targPath = "/FileInfo/Prefix";
  if ( (idx = searchStrVector(labels, targPath)) >= 0 ) {
    if ( tp.getInspectedValue(targPath, valStr) && ! valStr.empty() )
      m_filePrefix = valStr;
  }

  targPath = "/FileInfo/FieldFilenameFormat";
  if ( (idx = searchStrVector(labels, targPath)) >= 0 ) {
    if ( tp.getInspectedValue(targPath, valStr) )
      m_filenameStepRank = (valStr == "step_rank");
  }

  targPath = "/FileInfo/GuideCell";
  if ( (idx = searchStrVector(labels, targPath)) >= 0 ) {
    if ( tp.getInspectedValue(targPath, ival) )
      m_numGc = ival;
  }

  targPath = "/FileInfo/Component"; // FFVC-1.x
  if ( (idx = searchStrVector(labels, targPath)) >= 0 ) {
    if ( tp.getInspectedValue(targPath, ival) && ival > 0 )
      m_dataLen = ival;
    else
      return false;
  } else {
    targPath = "/FileInfo/NumVariables"; // FFVC-2.x
    if ( (idx = searchStrVector(labels, targPath)) >= 0 ) {
      if ( tp.getInspectedValue(targPath, ival) && ival > 0 )
	m_dataLen = ival;
      else
	return false;
    } else
      return false;
  }

  targPath = "/FilePath/Process";
  if ( (idx = searchStrVector(labels, targPath)) >= 0 ) {
    if ( tp.getInspectedValue(targPath, valStr) && ! valStr.empty() )
      m_procPath = valStr;
    else
      return false;
  } else
    return false;

  targPath = "/TimeSlice";
  int nts = tp.countLabels(targPath);
  if ( nts < 1 ) return false;
  for ( i = 0; i < nts; i++ ) {
    TimeSlice ts(0, 0.f, m_dataLen);
    char buff[64];
    sprintf(buff, "/TimeSlice/slice[%d]/", i);

    targPath = string(buff) + "Step";
    if ( (idx = searchStrVector(labels, targPath)) >= 0 ) {
      if ( tp.getInspectedValue(targPath, ival) )
	ts.step = ival;
    }

    targPath = string(buff) + "Time";
    if ( (idx = searchStrVector(labels, targPath)) >= 0 ) {
      if ( tp.getInspectedValue(targPath, fval) )
	ts.time = fval;
    }

    for ( j = 0; j < m_dataLen; j++ ) {
      char buff2[32];
      sprintf(buff2, "minmax[%d]/", j);

      targPath = string(buff) + string(buff2) + "Min";
      if ( (idx = searchStrVector(labels, targPath)) >= 0 ) {
	if ( tp.getInspectedValue(targPath, fval) )
	  ts.datMin[j] = fval;
      }

      targPath = string(buff) + string(buff2) + "Max";
      if ( (idx = searchStrVector(labels, targPath)) >= 0 ) {
	if ( tp.getInspectedValue(targPath, fval) )
	  ts.datMax[j] = fval;
      }
    } // end of for(j:dataLen)

    if ( m_dataLen > 1 ) {
      targPath = string(buff) + "VectorMinMax/Min";
      if ( (idx = searchStrVector(labels, targPath)) >= 0 ) {
	if ( tp.getInspectedValue(targPath, fval) )
	  ts.vecMinMax[0] = fval;
      }

      targPath = string(buff) + "VectorMinMax/Max";
      if ( (idx = searchStrVector(labels, targPath)) >= 0 ) {
	if ( tp.getInspectedValue(targPath, fval) )
	  ts.vecMinMax[1] = fval;
      }
    }

    m_timeSliceList.push_back(ts);
  } // end of for(i:TimeSlice)

  // load PROC.dfi file
  tp.remove();
  labels.clear();

  string xpath = vsnPath_concat(m_baseDir, m_procPath);
  if ( tp.read(xpath) != 0 )
    return false;
  if ( tp.getAllLabels(labels) != 0 )
    return false;

  int ivals[3]; float fvals[3];

  targPath = "/Domain/GlobalOrigin";
  if ( (idx = searchStrVector(labels, targPath)) < 0 )
    return false;
  if ( tp.getInspectedVector(targPath, fvals, 3) )
    m_globalOrig = Vec3<float>(fvals);
  else
    return false;

  targPath = "/Domain/GlobalRegion";
  if ( (idx = searchStrVector(labels, targPath)) < 0 )
    return false;
  if ( tp.getInspectedVector(targPath, fvals, 3) )
    m_globalRegn = Vec3<float>(fvals);
  else
    return false;

  targPath = "/Domain/GlobalVoxel";
  if ( (idx = searchStrVector(labels, targPath)) < 0 )
    return false;
  if ( tp.getInspectedVector(targPath, ivals, 3) ) {
    m_globalDims[0] = (size_t)ivals[0];
    m_globalDims[1] = (size_t)ivals[1];
    m_globalDims[2] = (size_t)ivals[2];
  } else
    return false;
  size_t dimSz = m_globalDims[0] * m_globalDims[1] * m_globalDims[2];
  if ( dimSz < 1 ) return false;

  targPath = "/Domain/GlobalDivision";
  if ( (idx = searchStrVector(labels, targPath)) < 0 )
    return false;
  if ( tp.getInspectedVector(targPath, ivals, 3) ) {
    m_globalDiv[0] = (size_t)ivals[0];
    m_globalDiv[1] = (size_t)ivals[1];
    m_globalDiv[2] = (size_t)ivals[2];
  } else
    return false;
  size_t divSz = m_globalDiv[0] * m_globalDiv[1] * m_globalDiv[2];
  if ( divSz < 1 ) return false;

  targPath = "/Domain/ActiveSubdomainFile";
  if ( (idx = searchStrVector(labels, targPath)) >= 0 ) {
    if ( tp.getInspectedValue(targPath, valStr) ) {
      m_activeDomainPath = valStr;
      if ( ! readActiveSubdomainFile() )
	m_activeDomainPath = "";
    }
  }

  targPath = "/Process";
  int nbks = tp.countLabels(targPath);
  if ( nbks < 1 ) return false;
  for ( i = 0; i < nbks; i++ ) {
    BrickInfo b;
    char buff[64];
    sprintf(buff, "/Process/rank[%d]/", i);

    targPath = string(buff) + "ID";
    if ( (idx = searchStrVector(labels, targPath)) >= 0 )
      if ( tp.getInspectedValue(targPath, ival) )
	b.ID = ival;

    targPath = string(buff) + "VoxelSize";
    if ( (idx = searchStrVector(labels, targPath)) >= 0 ) {
      if ( tp.getInspectedVector(targPath, ivals, 3) ) {
	b.voxelSize[0] = (size_t)ivals[0];
	b.voxelSize[1] = (size_t)ivals[1];
	b.voxelSize[2] = (size_t)ivals[2];
      }
    }

    targPath = string(buff) + "HeadIndex";
    if ( (idx = searchStrVector(labels, targPath)) >= 0 ) {
      if ( tp.getInspectedVector(targPath, ivals, 3) ) {
	b.headIdx[0] = (size_t)ivals[0] -1;
	b.headIdx[1] = (size_t)ivals[1] -1;
	b.headIdx[2] = (size_t)ivals[2] -1;
      }
    }

    m_brickList.push_back(b);
  } // end of for(i)

  // adjust non-active subdomain with dummy brick
  if ( divSz != nbks ) {
    if ( ! m_activeDomainFlag ) return false;
    set<size_t> xhead, yhead, zhead;
    deque<BrickInfo>::iterator bit = m_brickList.begin();
    for ( ; bit != m_brickList.end(); bit++ ) {
      xhead.insert(bit->headIdx[0]);
      xhead.insert(bit->headIdx[0] + bit->voxelSize[0]);
      yhead.insert(bit->headIdx[1]);
      yhead.insert(bit->headIdx[1] + bit->voxelSize[1]);
      zhead.insert(bit->headIdx[2]);
      zhead.insert(bit->headIdx[2] + bit->voxelSize[2]);
    } // end of for(bit)
    if ( xhead.size() != m_globalDiv[0] + 1 ||
	 yhead.size() != m_globalDiv[1] + 1 ||
	 zhead.size() != m_globalDiv[2] + 1 ) return false;
    register int k;
    set<size_t>::iterator xit, yit, zit, sit;
    bit = m_brickList.begin();
    for ( zit = zhead.begin(), k = 0; k < m_globalDiv[2]; zit++, k++ )
      for ( yit = yhead.begin(), j = 0; j < m_globalDiv[1]; yit++, j++ )
	for ( xit = xhead.begin(), i = 0; i < m_globalDiv[0]; xit++, i++ ) {
	  idx = m_globalDiv[0]*m_globalDiv[1]*k + m_globalDiv[0]*j + i;
	  if ( m_activeDomainFlag[idx] ) {bit++; continue;}
	  BrickInfo b;
	  b.headIdx[0] = *xit; b.headIdx[1] = *yit; b.headIdx[2] = *zit;
	  sit = xit; sit++; b.voxelSize[0] = *sit;
	  sit = yit; sit++; b.voxelSize[1] = *sit;
	  sit = zit; sit++; b.voxelSize[2] = *sit;
	  bit = m_brickList.insert(bit, b);
	  bit++;
	} // end of for(i)
  }

  if ( ! checkMinMax(true) )
    return false;

  m_ready = true;
  m_path = path;
  setXmlNode(xnp);
  return true;
}


// from vsnNumericalDataIF

bool vsnData_Dfi::getVectorMaxLen(const Vec3<int>& vidx, float& vml) {
  if ( vidx[0] < 0 || vidx[0] >= m_dataLen ||
       vidx[1] < 0 || vidx[1] >= m_dataLen ||
       vidx[2] < 0 || vidx[2] >= m_dataLen ) return false;
  size_t i, j, nts = m_timeSliceList.size();
  if ( nts < 1 ) return false;
  Vec3<float> v;
  vector2 mm;
  for ( j = 0; j < 3; j++ ) {
    mm[0] = (float)fabs(m_timeSliceList[0].datMin[vidx[j]]);
    mm[1] = (float)fabs(m_timeSliceList[0].datMax[vidx[j]]);
    v[j] = (mm[0] > mm[1]) ? mm[0] : mm[1];
    vml = v.Length();
  } // end of for(j)
  for ( i = 1; i < nts; i++ ) {
    for ( j = 0; j < 3; j++ ) {
      mm[0] = (float)fabs(m_timeSliceList[i].datMin[vidx[j]]);
      mm[1] = (float)fabs(m_timeSliceList[i].datMax[vidx[j]]);
      v[j] = (mm[0] > mm[1]) ? mm[0] : mm[1];
      float vl = v.Length();
      if ( vl > vml ) vml = vl;
    } // end of for(j)
  } // end of for(i)
  return true;
}

bool vsnData_Dfi::checkMinMax(const bool wholeStp, const bool progress) {
  register size_t i, k;
  m_hasMinMax = false;
  string errMsg = string("DataDfi[") + getName() + string("]: ");

  m_minVals.resize(m_dataLen);
  m_maxVals.resize(m_dataLen);
  if ( m_dataLen < 1 ) return false;
  size_t numStps = m_timeSliceList.size();
  if ( numStps < 1 ) return false;

  // the first step
  for ( k = 0; k < m_dataLen; k++ ) {
    m_minVals[k] = m_timeSliceList[0].datMin[k];
    m_maxVals[k] = m_timeSliceList[0].datMax[k];
  } // end of for(k)
  if ( m_dataLen >= 3 ) {
    m_minVecLen012 = m_timeSliceList[0].vecMinMax[0];
    m_maxVecLen012 = m_timeSliceList[0].vecMinMax[1];
  }
  m_hasMinMax = true;

  if ( ! wholeStp ) {
    return true;
  }

  // follow steps
  for ( i = 1; i < numStps; i++ ) {
    for ( k = 0; k < m_dataLen; k++ ) {
      if ( m_minVals[k] > m_timeSliceList[i].datMin[k] )
	m_minVals[k] = m_timeSliceList[i].datMin[k];
      if ( m_maxVals[k] < m_timeSliceList[i].datMax[k] )
	m_maxVals[k] = m_timeSliceList[i].datMax[k];
    } // end of for(k)
    if ( m_dataLen >= 3 ) {
      if ( m_minVecLen012 > m_timeSliceList[i].vecMinMax[0] )
	m_minVecLen012 = m_timeSliceList[i].vecMinMax[0];
      if ( m_maxVecLen012 < m_timeSliceList[i].vecMinMax[1] )
	m_maxVecLen012 = m_timeSliceList[i].vecMinMax[1];
    }
  } // end of for(i)

  return true;
}


// from vfrGroup

void vsnData_Dfi::generateBbox() {
  /* DFIに記述されたGlobalOriginは、ガイドセルを除いた計算領域に対して、
     SPHのデータ定義点(格子点)をセル中心とした場合の、セルの左下隅の座標値
     (_bboxはSPHのデータ定義点を基点に考えるため、半セルサイズずれる)
   */
  Vec3<float> pit = getPitch();
  Vec3<float> half_pit = pit * 0.5f;
  _bbox[0] = m_globalOrig + half_pit;
  _bbox[1] = m_globalOrig + m_globalRegn - half_pit;
}
