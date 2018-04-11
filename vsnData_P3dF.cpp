//
// vsnData_P3dF
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

#include <algorithm>
#include "vsnData_P3dF.h"
#include "vsnError.h"

using namespace std;
using namespace CES;


/* constructors / destructor */

vsnData_P3dF::vsnData_P3dF(const string& name, const size_t csz)
  : vsnData_Sv(name), vsnDataCacheMF(csz), m_crdUpdStp(-1), m_nanWarned(false)
{
}

vsnData_P3dF::~vsnData_P3dF() {
}


/* methods */

bool vsnData_P3dF::init(const deque<string>& path_lst) {
  if ( path_lst.size() < 1 ) return false;
  m_ready = false;
  m_crdUpdStp = -1;

  // setup cache : setupMF() will be called
  if ( ! setupCachePool(path_lst) ) return false;

  // set min/max
  vsnApp* pApp = vsnApp::GetApp();
  if ( ! checkMinMax(true, pApp->isChkProgress()) )
    return false;

  // set bbox
  if ( ! updateBbox() )
    return false;

  m_ready = true;
  return true;
}

bool vsnData_P3dF::getOrig(Vec3<float>& orig, const size_t stp) {
  if ( ! isCacheReady() || stp >= m_numStps )
    return false;
  orig = m_origList[stp];
  return true;
}

bool vsnData_P3dF::getPitch(Vec3<float>& pitch, const size_t stp) {
  if ( ! isCacheReady() || stp >= m_numStps )
    return false;
  pitch = m_pitchList[stp];
  return true;
}


/* from vsnData_Sv */

bool vsnData_P3dF::isUniformCoord() const {
  return true;
}

const float* const vsnData_P3dF::getCoord(const size_t stp) {
  if ( ! updateBbox(stp) ) return NULL;

  register size_t dimSz = m_dims[0]*m_dims[1]*m_dims[2];
  if ( dimSz < 1 ) return NULL;
  if ( dimSz != nVerts ) {
    if ( ! alcVerts(dimSz) ) return NULL;
    m_crdUpdStp = -1;
  }

  if ( m_crdUpdStp != (int)stp ) {
    register int i, j, k, idx = 0;
    Vec3<float> pit; getPitch(pit, stp);
    for ( k = 0; k < m_dims.m_v[2]; k++ )
      for ( j = 0; j < m_dims.m_v[1]; j++ )
	for ( i = 0; i < m_dims.m_v[0]; i++ ) {
	  _verts[idx][0] = _bbox[0].m_v[0] + pit.m_v[0] * i;
	  _verts[idx][1] = _bbox[0].m_v[1] + pit.m_v[1] * j;
	  _verts[idx][2] = _bbox[0].m_v[2] + pit.m_v[2] * k;
	  idx ++;
	} // end of for(i)
    m_crdUpdStp = (int)stp;
  }
  return (float*)_verts;
}

const float* const vsnData_P3dF::getData(const size_t stp) {
  const unsigned char* const dptr = vsnDataCache::getData(stp);
  if ( ! dptr ) return NULL;
  const float* const fptr = (const float* const)dptr;
  return fptr;
}

deque<string> vsnData_P3dF::getSeqFilePathes() const {
  return m_cachePathes;
}

bool vsnData_P3dF::updateBbox(const int stp) {
  Vec3<float> org, pit;
  if ( ! getOrig(org, stp) ) return false;
  if ( ! getPitch(pit, stp) ) return false;
  pit.m_v[0] *= (float)(m_dims[0] -1);
  pit.m_v[1] *= (float)(m_dims[1] -1);
  pit.m_v[2] *= (float)(m_dims[2] -1);
  _bbox[0] = org;
  _bbox[1] = org + pit;
  notice();
  return true;
}


/* from vsnDataObj */

string vsnData_P3dF::getFilePath() const {
  if ( ! isCacheReady() ) return string("");
  if ( m_cachePathes.size() == 1 )
    return m_cachePathes[0];
  return string(VSN::vsn_seqfile);
}

bool vsnData_P3dF::init(const string& path, xmlNodePtr xnp) {
  static xmlChar* xs;
  deque<string> path_lst;
  string baseDir;

  if ( BaseName(path, string(""), vsnPath_getDelimChar())
       != string(VSN::vsn_seqfile) ) {
    path_lst.push_back(path);
    if ( ! init(path_lst) ) return false;
    setXmlNode(xnp);
    return true;
  }

  //---- sequential files ----
  vsnApp* pApp = vsnApp::GetApp();
  if ( ! xnp || ! pApp ) return false;

  // is 'data' node?
  if ( strcmp((const char*)xnp->name, "data") ) return false;

  // 'base_dir' prop and 'seq' child node
  vsnDataSeqFiles dsf;
  if ( ! dsf.parseXML_SeqFiles(xnp, baseDir, path_lst) ) return false;

  // local initialize
  if ( ! init(path_lst) ) return false;
  (void)adjustStepList(path_lst, baseDir, xnp);

  setXmlNode(xnp);
  return true;
}


/* from vsnDataCache */

bool vsnData_P3dF::readin(const size_t stp, unsigned char* pdata) {
  m_readinErrMsg = "";
  if ( ! isCacheReady() ) {
    m_readinErrMsg = "data cache is not ready";
    return false;
  }
  if ( ! pdata ) {
    m_readinErrMsg = "bad data cache has prepared";
    return false;
  }
  if ( stp >= m_totalSteps ) {
    m_readinErrMsg = "file sequential number overrun";
    return false;
  }
  float* pfData = (float*)pdata;

  unsigned char buff[24];
  int* pib = (int*)(&buff[4]);
  float* pfb = (float*)(&buff[4]);

  // open the file
  FILE* fp = fopen(m_cachePathes[stp].c_str(), "rb");
  if ( ! fp ) {
    m_readinErrMsg = "can't open the file";
    return false;
  }
  GridType grt;
  EMatchType emt;
  if ( ! CheckFileType(fp, grt, emt) ) {
    m_readinErrMsg = "can't figure out grid-type or endian-type of the file";
    fclose(fp); return false;
  }

  // read headers ((ngrid), dims, veclen)
  int ngrid = 1;
  if ( grt == P3dF_SingleGrid ) {
    if ( fread(buff, 1, 24, fp) < 24 ) {
      m_readinErrMsg = "can't read header record";
      fclose(fp); return false;
    }

    if ( fseek(fp, 4, SEEK_CUR) != 0 ) {
      m_readinErrMsg = "file seek error has occurred";
      fclose(fp); return false;
    }
  } else {
    if ( fread(buff, 1, 12, fp) < 12 ) {
      m_readinErrMsg = "can't read header record";
      fclose(fp); return false;
    }
    ngrid = pib[0];
    if ( emt == UnMatch ) BSWAP32(ngrid);
    if ( ngrid < 1 ) {
      m_readinErrMsg = "invalid number of grid, must be 1 or grater";
      fclose(fp); return false;
    }

    if ( fread(buff, 1, 24, fp) < 24 ) {
      m_readinErrMsg = "can't read header record";
      fclose(fp); return false;
    }

    if ( fseek(fp, (ngrid-1)*24 + 4, SEEK_CUR) != 0 ) {
      m_readinErrMsg = "file seek error has occurred";
      fclose(fp); return false;
    }
  }
  if ( emt == UnMatch ) BSWAPVEC(pib, 4);
  if ( m_dims[0] != (size_t)pib[0] ||
       m_dims[1] != (size_t)pib[1] ||
       m_dims[2] != (size_t)pib[2] ) {
    m_readinErrMsg = "dims not equal to the first step";
    fclose(fp); return false;
  }
  if ( m_dataLen != pib[3] ) {
    m_readinErrMsg = "number of data is not equal to the first step";
    fclose(fp); return false;
  }

  // read data
  bool nanFound = false;
  size_t dimSz = m_dims[0] * m_dims[1] * m_dims[2];
  size_t dataSz = dimSz * m_dataLen;

  float* dtbuf = (float*)Allocate(dimSz * sizeof(float));
  if ( ! dtbuf ) {
    m_readinErrMsg = "buffer for readin allocation failed";
    fclose(fp); return false;
  }
  register int n;
  for ( n = 0; n < m_dataLen; n++ ) {
    if ( fread(dtbuf, 4, dimSz, fp) < dimSz ) {
      m_readinErrMsg = "can't read data record";
      fclose(fp); return false;
    }
    if ( emt == UnMatch ) BSWAPVEC(dtbuf, dimSz);

    register size_t i;
    for ( i = 0; i < dimSz; i++ ) {
#ifndef WINDOWS
      if ( isnan(dtbuf[i]) ) nanFound = true;
#endif // !WINDOWS
      pfData[i*m_dataLen + n] = dtbuf[i];
    } // end of for(i)
  } //  end of for(n)
  DeAllocate(dtbuf);

  if ( nanFound && ! m_nanWarned ) {
    ErrMsg(MsgWARN, string("invalid data(NaN) found in file: ")
	   + m_cachePathes[stp]);
    m_nanWarned = true;
  }

  fclose(fp);
  return true;
}


/* from vsnDataCacheMF */

bool vsnData_P3dF::setupMF(const deque<string>& path_lst,
			  deque<string>& new_path_lst)
{
  string msgHdr
    = getDataType() + string("[") + getName() + string("]: setupMF: ");

  deque<string> pathLst = setupLists(path_lst);
  if ( pathLst.size() < 1 ) {
    ErrMsg(MsgERR, msgHdr + string("no valid file exists"));
    return false;
  }
  if ( pathLst.size() != path_lst.size() ) {
    string badLst;
    deque<string>::const_iterator it0 = path_lst.begin();
    deque<string>::const_iterator it1 = pathLst.begin();
    for ( ; it0 != path_lst.end(); it0++ ) {
      if ( it1 == pathLst.end() || (*it1) != (*it0) ) {
	badLst += string("  + ")
	  + BaseName(*it0, string(""), vsnPath_getDelimChar()) + "\n";
	continue;
      }
      it1++;
    } // end of for(it0)
    ErrMsg(MsgWARN, msgHdr + string("file list contains bad file(s)\n")
	   + badLst);
  }

  unsigned char buff[24];
  int* pib = (int*)(&buff[4]);
  long long* plb = (long long*)(&buff[4]);

  // open the first file
  string path = pathLst[0];
  if ( path.size() < 1 ) return false;
  FILE* fp = fopen(path.c_str(), "rb");
  if ( ! fp ) {
    ErrMsg(MsgERR, msgHdr + string("can't open file: ") + path);
    return false;
  }
  GridType grt;
  EMatchType emt;
  if ( ! CheckFileType(fp, grt, emt) ) {
    m_readinErrMsg = "can't figure out grid-type or endian-type of the file";
    fclose(fp); return false;
  }

  // read header ((ngrid), dims, veclen)
  int ngrid = 1;
  if ( grt == P3dF_SingleGrid ) {
    if ( fread(buff, 1, 24, fp) < 24 ) {
      m_readinErrMsg = "can't read header record";
      fclose(fp); return false;
    }

    if ( fseek(fp, 4, SEEK_CUR) != 0 ) {
      m_readinErrMsg = "file seek error has occurred";
      fclose(fp); return false;
    }
  } else {
    if ( fread(buff, 1, 12, fp) < 12 ) {
      m_readinErrMsg = "can't read header record";
      fclose(fp); return false;
    }
    ngrid = pib[0];
    if ( emt == UnMatch ) BSWAP32(ngrid);
    if ( ngrid < 1 ) {
      m_readinErrMsg = "invalid number of grid, must be 1 or grater";
      fclose(fp); return false;
    }

    if ( fread(buff, 1, 24, fp) < 24 ) {
      m_readinErrMsg = "can't read header record";
      fclose(fp); return false;
    }

    if ( fseek(fp, (ngrid-1)*24 + 4, SEEK_CUR) != 0 ) {
      m_readinErrMsg = "file seek error has occurred";
      fclose(fp); return false;
    }
  }
  fclose(fp);

  if ( emt == UnMatch ) BSWAPVEC(pib, 4);
  m_dims[0] = (size_t)pib[0];
  m_dims[1] = (size_t)pib[1];
  m_dims[2] = (size_t)pib[2];
  m_dataLen = pib[3];
  size_t dimSz = m_dims[0] * m_dims[1] * m_dims[2];
  if ( dimSz < 1 ) {
    ErrMsg(MsgERR, msgHdr + string("invalid dims: ") + path);
    return false;
  }
  if ( m_dataLen < 1 ) {
    ErrMsg(MsgERR, msgHdr + string("invalid dataLen: ") + path);
    return false;
  }

  // set cache params
  m_numStps = m_totalSteps = pathLst.size();
  m_cacheUnitSize = dimSz * m_dataLen * sizeof(float);
  if ( m_cacheUnitSize >= 1024*1024*512 ) // over 512M
    m_cacheSteps = 1;
  else if ( m_cacheUnitSize >= 1024*1024*256 ) // over 256M
    m_cacheSteps = 2;
  else if ( m_cacheUnitSize >= 1024*1024*128 ) // over 128M
    m_cacheSteps = 4;
  else
    m_cacheSteps = 5;
  if ( m_cacheSteps > m_totalSteps )
    m_cacheSteps = m_totalSteps;

  new_path_lst = pathLst;
  return true;
}


/* util methods */

deque<string>
vsnData_P3dF::setupLists(const deque<string>& path_lst) {
  deque<string> pathLst;

  register size_t numPath = path_lst.size();
  if ( numPath < 1 ) return pathLst;

  m_stpList.resize(numPath);
  m_origList.resize(numPath);
  m_pitchList.resize(numPath);

  unsigned char buff[24];
  int* pib = (int*)(&buff[4]);
  float* pfb = (float*)(&buff[4]);

  register size_t i, idx = 0;
  for ( i = 0; i < numPath; i++ ) {
    FILE* fp = fopen(path_lst[i].c_str(), "rb");
    if ( ! fp ) continue;
    GridType grt;
    EMatchType emt;
    if ( ! CheckFileType(fp, grt, emt) ) {
      fclose(fp); continue;
    }
    
    // read header
    int ngrid = 1;
    if ( grt == P3dF_SingleGrid ) {
      if ( fread(buff, 1, 24, fp) < 24 ) {fclose(fp); continue;}
    } else {
      if ( fread(buff, 1, 12, fp) < 12 ) {fclose(fp); continue;}
      ngrid = pib[0];
      if ( emt == UnMatch ) BSWAP32(ngrid);
      if ( ngrid < 1 ) {fclose(fp); continue;}
      if ( fread(buff, 1, 24, fp) < 24 ) {fclose(fp); continue;}
    }
    if ( emt == UnMatch ) BSWAPVEC(pib, 4);
    int dims[3] = {pib[0], pib[1], pib[2]};
    int dataLen = pib[3];
    if ( dims[0] * dims[1] * dims[2] < 1 ) {fclose(fp); continue;}
    if ( dataLen < 1 ) {fclose(fp); continue;}

    // set org
    m_origList[idx].m_v[0] = 0.f;
    m_origList[idx].m_v[1] = 0.f;
    m_origList[idx].m_v[2] = 0.f;

    // set pitch
    m_pitchList[idx].m_v[0] = 1.f;
    m_pitchList[idx].m_v[1] = 1.f;
    m_pitchList[idx].m_v[2] = 1.f;

    // set time
    m_stpList[idx].step = idx;
    m_stpList[idx].time = (float)idx;

    pathLst.push_back(path_lst[i]);
    fclose(fp);

    idx ++;
  } // end of for(i)

  m_stpList.resize(idx);
  m_origList.resize(idx);
  m_pitchList.resize(idx);
  return pathLst;
}

bool vsnData_P3dF::adjustStepList(const deque<string>& path_lst,
				  const string& baseDir, xmlNodePtr xnp)
{
  static xmlChar* xs;
  StpUnit stpUnit;
  bool hasStep, hasTime, hasChanged = false;
  register size_t i, npl;

 vsnApp* pApp = vsnApp::GetApp();
  if ( ! xnp || ! pApp ) return false;
  npl = path_lst.size();
  if ( npl < 1 ) return false;
  if ( npl != m_stpList.size() ) return false;

  // 'seq' child node
  xmlNodePtr cur = xnp->xmlChildrenNode;
  while ( cur ) {
    if ( (cur = vsnIoObject::SkipCommentXML(cur)) == NULL ) break;
    if ( !strcmp((const char*)cur->name, "seq") ) {
      hasStep = hasTime = false;
      if ( xs ) xmlFree(xs);
      xs = xmlGetProp(cur, (const xmlChar*)"step");
      if ( xs && strlen((const char*)xs) > 0 ) {
	stpUnit.step = atoi((const char*)xs);
	hasStep = true;
      }
      if ( xs ) xmlFree(xs);
      xs = xmlGetProp(cur, (const xmlChar*)"time");
      if ( xs && strlen((const char*)xs) > 0 ) {
	stpUnit.time = (float)atof((const char*)xs);
	hasTime = true;
      }
      if ( ! hasStep && ! hasTime )
	goto _NEXT_XML_NODE;

      if ( xs ) xmlFree(xs);
      xs = xmlGetProp(cur, (const xmlChar*)"file");
      if ( ! xs || strlen((const char*)xs) < 1 )
	goto _NEXT_XML_NODE;

      string seqpath;
      if ( pApp->needPathEncode() ) {
	wxString cvtPath = wxString::FromUTF8((const char*)xs);
	seqpath = vsnApp::ConvWxToSys(cvtPath);
      } else
	seqpath = (const char*)xs;
      if ( ! vsnPath_isAbsolute(seqpath) )
	seqpath = vsnPath_concat(baseDir, seqpath);
      seqpath = vsnPath_normalize(seqpath);

      for ( i = 0; i < npl; i++ )
	if ( seqpath == path_lst[i] ) break;
      if ( i >= npl )
	goto _NEXT_XML_NODE;

      if ( hasStep ) m_stpList[i].step = stpUnit.step;
      if ( hasTime ) m_stpList[i].time = stpUnit.time;
      hasChanged = true;
    } // end of 'seq'

  _NEXT_XML_NODE:
    cur = cur->next;
  } // end of while(cur)

  if ( hasChanged ) {
    for ( i = 1; i < npl; i++ ) {
      if ( m_stpList[i].step <= m_stpList[i -1].step )
	m_stpList[i].step = m_stpList[i -1].step + 1;
    } // end of for(i)
  }
  return true;
}

//STATIC
bool vsnData_P3dF::CheckFileType(FILE* fp, GridType& gt, EMatchType& et) {
  if ( ! fp ) return false;
  long here = ftell(fp);
  if ( here < 0 ) return false;

  gt = P3df_Invalid;
  et = UnKnown;
  unsigned int sz;
  if ( fread(&sz, 4, 1, fp) < 1 ) return false;
  (void)fseek(fp, here, SEEK_SET);

  // endian matched
  if ( sz == 16 ) {
    gt = P3dF_SingleGrid;
    et = Match;
    return true;
  }
  else if ( sz == 4 ) {
    gt = P3dF_MultiGrid;
    et = Match;
    return true;
  }

  // endian unmatched
  BSWAP32(sz);
  if ( sz == 16 ) {
    gt = P3dF_SingleGrid;
    et = UnMatch;
    return true;
  }
  else if ( sz == 4 ) {
    gt = P3dF_MultiGrid;
    et = UnMatch;
    return true;
  }

  // invalid file?
  return false;
}
