//
// vsnData_Sph
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
#include "vsnData_Sph.h"
#include "vsnError.h"
#include "utilEndian.h"

using namespace std;
using namespace CES;


/* constructors / destructor */

vsnData_Sph::vsnData_Sph(const std::string& name, const size_t csz)
  : vsnData_Sv(name), vsnDataCacheMF(csz),
    m_crdUpdStp(-1), m_nanWarned(false), m_hasRectCrd(false)
{
  m_rectCoord[0] = m_rectCoord[1] = m_rectCoord[2] = NULL;
  m_rectCrdOfst[0] = m_rectCrdOfst[1] = m_rectCrdOfst[2] = 0;
}

vsnData_Sph::~vsnData_Sph() {
  if ( m_rectCoord[0] ) DeAllocate(m_rectCoord[0]);
  if ( m_rectCoord[1] ) DeAllocate(m_rectCoord[1]);
  if ( m_rectCoord[2] ) DeAllocate(m_rectCoord[2]);
}


/* methods */

bool vsnData_Sph::init(const std::deque<std::string>& path_lst) {
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

bool vsnData_Sph::getOrig(CES::Vec3<float>& orig, const size_t stp) {
  if ( ! isCacheReady() || stp >= m_numStps )
    return false;
  orig = m_origList[stp];
  return true;
}

bool vsnData_Sph::getPitch(CES::Vec3<float>& pitch, const size_t stp) {
  if ( ! isCacheReady() || stp >= m_numStps )
    return false;
  pitch = m_pitchList[stp];
  return true;
}

bool vsnData_Sph::loadRectCoord(const std::string& path, const size_t* ofst) {
  string msgHdr("Data_Sph: loadRectCoord: ");
  register int i;
  bool ret = true;
  size_t numMtds = getNumMethod();
  if ( path.empty() ) {
    m_hasRectCrd = false;
    m_rectCrdPath = "";
    m_crdUpdStp = -1;
	updateBbox(this->getCurrentStepIdx());
    for ( i = 0; i < numMtds; i++ ) {
      vsnMethodObj* pMtd = getMethod(i);
      if ( ! pMtd ) continue;
      if ( ! pMtd->update() )
	ret = false;
    } // end of for(i)
    return ret;
  }

  FILE* fp = fopen(path.c_str(), "rb");
  if ( ! fp ) {
    ErrMsg(MsgERR, msgHdr + string("can't open file: ") + path);
    return false;
  }

  bool ivConv = false;
  CES::EMatchType emt = CES::MatchEndian(fp, 8);
  if ( emt == CES::UnKnown ) {
    ErrMsg(MsgERR, msgHdr + string("invalid file: ") + path);
    fclose(fp); return false;
  }
  else if ( emt == CES::UnMatch ) ivConv = true;

  int ibuff[5];
  float* fbuff = (float*)ibuff;
  bool dbl = false;
  CES::Vec3<size_t> dims, xdims;

  // read header
  if ( fread(ibuff, 4, 4, fp) != 4 ) {fclose(fp); return false;}
  if ( ivConv ) BSWAPVEC(ibuff, 4);
  if ( ibuff[1] == 2 ) dbl = true;

  // read dims
  size_t ofstDims[3] = {0, 0, 0};
  if ( ofst ) {
    ofstDims[0] = ofst[0]; ofstDims[1] = ofst[1]; ofstDims[2] = ofst[2]; 
  }
  if ( fread(ibuff, 4, 5, fp) != 5 ) {fclose(fp); return false;}
  if ( ivConv ) BSWAPVEC(ibuff, 5);
  xdims[0] = ibuff[1]; xdims[1] = ibuff[2]; xdims[2] = ibuff[3];
  dims = xdims;
  if ( xdims[0] == 0 ) dims[0] = 1;
  if ( xdims[1] == 0 ) dims[1] = 1;
  if ( xdims[2] == 0 ) dims[2] = 1;
  if ( dims[0] < m_dims[0] + ofstDims[0] ||
       dims[1] < m_dims[1] + ofstDims[1] ||
       dims[2] < m_dims[2] + ofstDims[2] ) {
    ErrMsg(MsgERR, msgHdr
	   + string("dimension size of rect coord is too small."));
    fclose(fp);
    return false;
  }

  // point of no return
  m_hasRectCrd = false;
  m_rectCrdPath = "";
  m_rectCrdOfst[0] = ofstDims[0];
  m_rectCrdOfst[1] = ofstDims[1];
  m_rectCrdOfst[2] = ofstDims[2];

  // allocate
  m_rectCoord[0] = (float*)ReAllocate(m_rectCoord[0], dims[0]*sizeof(float));
  m_rectCoord[1] = (float*)ReAllocate(m_rectCoord[1], dims[1]*sizeof(float));
  m_rectCoord[2] = (float*)ReAllocate(m_rectCoord[2], dims[2]*sizeof(float));
  if ( ! m_rectCoord[0] || ! m_rectCoord[1] || ! m_rectCoord[2] ) {
    ErrMsg(MsgERR, msgHdr + string("memory allocation failed"));
    fclose(fp);
    return false;
  }

  // skip time
  if ( fread(ibuff, 4, 4, fp) != 4 ) {
    ErrMsg(MsgERR, msgHdr + string("invalid file format: ") + path);
    fclose(fp); return false;
  }

  // alloc buff to read
  size_t maxDim = dims[0];
  if ( maxDim < dims[1] ) maxDim = dims[1];
  if ( maxDim < dims[2] ) maxDim = dims[2];
  float* rcbuff = new float[maxDim];
  if ( ! rcbuff ) {
    ErrMsg(MsgERR, msgHdr + string("memory allocation failed"));
    fclose(fp);
    return false;
  }

  // read x coords
  if ( xdims[0] > 0 ) {
    fread(ibuff, 4, 1, fp);
    if ( fread(rcbuff, 4, xdims[0], fp) != xdims[0] ) {
      ErrMsg(MsgERR, msgHdr + string("invalid file format: ") + path);
      fclose(fp); delete [] rcbuff; return false;
    }
    memcpy(m_rectCoord[0], &rcbuff[m_rectCrdOfst[0]], m_dims[0]*sizeof(float));
    if ( ivConv ) BSWAPVEC(m_rectCoord[0], m_dims[0]);
    fread(ibuff, 4, 1, fp);
  } else {
    m_rectCoord[0][0] = _bbox[0][0];
  }

  // read y coords
  if ( xdims[1] > 0 ) {
    fread(ibuff, 4, 1, fp);
    if ( fread(rcbuff, 4, xdims[1], fp) != xdims[1] ) {
      ErrMsg(MsgERR, msgHdr + string("invalid file format: ") + path);
      fclose(fp); delete [] rcbuff; return false;
    }
    memcpy(m_rectCoord[1], &rcbuff[m_rectCrdOfst[1]], m_dims[1]*sizeof(float));
    if ( ivConv ) BSWAPVEC(m_rectCoord[1], m_dims[1]);
    fread(ibuff, 4, 1, fp);
  } else {
    m_rectCoord[1][0] = _bbox[0][1];
  }

  // read z coords
  if ( xdims[2] > 0 ) {
    fread(ibuff, 4, 1, fp);
    if ( fread(rcbuff, 4, xdims[2], fp) != xdims[2] ) {
      ErrMsg(MsgERR, msgHdr + string("invalid file format: ") + path);
      fclose(fp); delete [] rcbuff; return false;
    }
    memcpy(m_rectCoord[2], &rcbuff[m_rectCrdOfst[2]], m_dims[2]*sizeof(float));
    if ( ivConv ) BSWAPVEC(m_rectCoord[2], m_dims[2]);
    fread(ibuff, 4, 1, fp);
  } else {
    m_rectCoord[2][0] = _bbox[0][2];
  }

  fclose(fp);
  delete [] rcbuff;
  m_rectCrdPath = path;
  m_hasRectCrd = true;
  m_crdUpdStp = -1;
  updateBbox(this->getCurrentStepIdx());

  ret = true;
  numMtds = getNumMethod();
  for ( i = 0; i < numMtds; i++ ) {
    vsnMethodObj* pMtd = getMethod(i);
    if ( ! pMtd ) continue;
    if ( ! pMtd->update() )
      ret = false;
  } // end of for(i)
  return ret;
}


/* from vsnData_Sv */

bool vsnData_Sph::isUniformCoord() const {
  return !hasRectCoord();
}

const float* const vsnData_Sph::getCoord(const size_t stp) {
  if ( ! updateBbox(stp) ) return NULL;

  register size_t dimSz = m_dims[0]*m_dims[1]*m_dims[2];
  if ( dimSz < 1 ) return NULL;
  if ( dimSz != nVerts ) {
    if ( ! alcVerts(dimSz) ) return NULL;
    m_crdUpdStp = -1;
  }

  if ( m_crdUpdStp != (int)stp ) {
    register int i, j, k, idx = 0;
    if ( hasRectCoord() ) {
      for ( k = 0; k < m_dims.m_v[2]; k++ )
	for ( j = 0; j < m_dims.m_v[1]; j++ )
	  for ( i = 0; i < m_dims.m_v[0]; i++ ) {
	    _verts[idx][0] = m_rectCoord[0][i];
	    _verts[idx][1] = m_rectCoord[1][j];
	    _verts[idx][2] = m_rectCoord[2][k];
	    idx ++;
	  } // end of for(i)
    }
    else {
      Vec3<float> pit; getPitch(pit, stp);
      for ( k = 0; k < m_dims.m_v[2]; k++ )
	for ( j = 0; j < m_dims.m_v[1]; j++ )
	  for ( i = 0; i < m_dims.m_v[0]; i++ ) {
	    _verts[idx][0] = _bbox[0].m_v[0] + pit.m_v[0] * i;
	    _verts[idx][1] = _bbox[0].m_v[1] + pit.m_v[1] * j;
	    _verts[idx][2] = _bbox[0].m_v[2] + pit.m_v[2] * k;
	    idx ++;
	  } // end of for(i)
    }
    m_crdUpdStp = (int)stp;
  }
  return (float*)_verts;
}

const float* const vsnData_Sph::getData(const size_t stp) {
  const unsigned char* const dptr = vsnDataCache::getData(stp);
  if ( ! dptr ) return NULL;
  const float* const fptr = (const float* const)dptr;
  return fptr;
}

std::deque<std::string> vsnData_Sph::getSeqFilePathes() const {
  return m_cachePathes;
}

bool vsnData_Sph::updateBbox(const int stp) {
  if ( hasRectCoord() ) {
    _bbox[0][0] = m_rectCoord[0][0];
    _bbox[0][1] = m_rectCoord[1][0];
    _bbox[0][2] = m_rectCoord[2][0];
    _bbox[1][0] = m_rectCoord[0][m_dims[0] -1];
    _bbox[1][1] = m_rectCoord[1][m_dims[1] -1];
    _bbox[1][2] = m_rectCoord[2][m_dims[2] -1];
  }
  else {
    Vec3<float> org, pit;
    if ( ! getOrig(org, stp) ) return false;
    if ( ! getPitch(pit, stp) ) return false;
    pit.m_v[0] *= (float)(m_dims[0] -1);
    pit.m_v[1] *= (float)(m_dims[1] -1);
    pit.m_v[2] *= (float)(m_dims[2] -1);
    _bbox[0] = org;
    _bbox[1] = org + pit;
  }
  notice();
  return true;
}


/* from vsnDataObj */

std::string vsnData_Sph::getFilePath() const {
  if ( ! isCacheReady() ) return string("");
  if ( m_cachePathes.size() == 1 )
    return m_cachePathes[0];
  return string(VSN::vsn_seqfile);
}

bool vsnData_Sph::init(const std::string& path, xmlNodePtr xnp) {
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

bool vsnData_Sph::readin(const size_t stp, unsigned char* pdata) {
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

  unsigned char buff[32];
  int* pib = (int*)(&buff[4]);
  long long* plb = (long long*)(&buff[4]);
  float* pfb = (float*)(&buff[4]);
  double* pdb = (double*)(&buff[4]);

  // open the file
  FILE* fp = fopen(m_cachePathes[stp].c_str(), "rb");
  if ( ! fp ) {
    m_readinErrMsg = "can't open the file";
    return false;
  }
  EMatchType emt = MatchEndian(fp, 8);
  if ( emt == UnKnown ) {
    m_readinErrMsg = "can't figure out endian-type of the file";
    fclose(fp); return false;
  }

  // read headers
  if ( fread(buff, 1, 16, fp) < 16 ) {
    m_readinErrMsg = "can't read header record";
    fclose(fp); return false;
  }
  if ( emt == UnMatch ) BSWAPVEC(pib, 2);
  switch ( pib[0] ) {
  case 1: // scalar
    if ( m_dataLen != 1 ) {
      m_readinErrMsg = "number of data is not equal to the first step";
      fclose(fp); return false;
    }
    break;
  case 2: // vector
    if ( m_dataLen != 3 ) {
      m_readinErrMsg = "number of data is not equal to the first step";
      fclose(fp); return false;
    }
    break;
  default:
    m_readinErrMsg = "invalid data type";
    fclose(fp); return false;
  }
  
  bool dblPrec = false;
  switch ( pib[1] ) {
  case 1: break; // single precision
  case 2: dblPrec = true; break; // double precision
  default:
    m_readinErrMsg = "invalid data spec type";
    fclose(fp); return false;
  }

  // read dims, org, pitch, time
  if ( dblPrec ) {
    if ( fread(buff, 1, 32, fp) < 32 ) {
      m_readinErrMsg = "can't read dims/org/pitch record";
      fclose(fp); return false;
    }
    if ( emt == UnMatch ) LBSWAPVEC(plb, 3);
    if ( m_dims[0] != (size_t)plb[0] ||
	 m_dims[1] != (size_t)plb[1] ||
	 m_dims[2] != (size_t)plb[2] ) {
      m_readinErrMsg = "dims not equal to the first step";
      fclose(fp); return false;
    }
    if ( fseek(fp, 88 + 4, SEEK_CUR) != 0 ) {
      m_readinErrMsg = "file seek error has occurred";
      fclose(fp); return false;
    }
  } else {
    if ( fread(buff, 1, 20, fp) < 20 ) {
      m_readinErrMsg = "can't read dims/org/pitch record";
      fclose(fp); return false;
    }
    if ( emt == UnMatch ) BSWAPVEC(pib, 3);
    if ( m_dims[0] != (size_t)pib[0] ||
	 m_dims[1] != (size_t)pib[1] ||
	 m_dims[2] != (size_t)pib[2] ) {
      m_readinErrMsg = "dims not equal to the first step";
      fclose(fp); return false;
    }
    if ( fseek(fp, 56 + 4, SEEK_CUR) != 0 ) {
      m_readinErrMsg = "file seek error has occurred";
      fclose(fp); return false;
    }
  }

  // read data
  bool nanFound = false;
  if ( dblPrec ) {
    register size_t k = m_dims[0] * m_dataLen;
    double* pDblData = new double[k];
    if ( ! pDblData ) {
      m_readinErrMsg = "memory allocation failed";
      fclose(fp); return false;
    }
    register size_t dimSzJK = m_dims[1] * m_dims[2];
    register size_t i, j, idx = 0;
    for ( i = 0; i < dimSzJK; i++ ) {
      if ( fread(pDblData, sizeof(double), k, fp) != k ) {
	m_readinErrMsg = "can't read data record";
	delete [] pDblData;
	fclose(fp); return false;
      }
      if ( emt == UnMatch ) LBSWAPVEC(pDblData, k);
      for ( j = 0; j < k; j++ ) {
#ifndef WINDOWS
	if ( isnan(pDblData[j]) ) nanFound = true;
#endif // !WINDOWS
	pfData[idx++] = (float)pDblData[j];
      } // end of for(j)
    } // end of for(i)
    delete [] pDblData;
  }
  else {
    size_t dimSz = m_dims[0] * m_dims[1] * m_dims[2] * m_dataLen;
    if ( fread(pfData, 4, dimSz, fp) < dimSz ) {
      m_readinErrMsg = "can't read data record";
      fclose(fp); return false;
    }
    if ( emt == UnMatch ) BSWAPVEC(pfData, dimSz);
#ifndef WINDOWS
    register size_t i;
    for ( i = 0; i < dimSz; i++ )
      if ( isnan(pfData[i]) ) nanFound = true;
#endif // !WINDOWS
  }

  if ( nanFound && ! m_nanWarned ) {
    ErrMsg(MsgWARN, string("invalid data(NaN) found in file: ")
	   + m_cachePathes[stp]);
    m_nanWarned = true;
  }

  fclose(fp);
  return true;
}


/* from vsnDataCacheMF */

bool vsnData_Sph::setupMF(const std::deque<std::string>& path_lst,
			  std::deque<std::string>& new_path_lst)
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

  unsigned char buff[32];
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
  EMatchType emt = MatchEndian(fp, 8);
  if ( emt == UnKnown ) {
    ErrMsg(MsgERR, msgHdr + string("not a Sph data file: ") + path);
    return false;
  }

  // read header
  if ( fread(buff, 1, 16, fp) < 16 ) {
    ErrMsg(MsgERR, msgHdr + string("can't read header record: ") + path);
    fclose(fp);
    return false;
  }
  if ( emt == UnMatch ) BSWAPVEC(pib, 2);

  switch ( pib[0] ) {
  case 1: m_dataLen = 1; break; // scalar
  case 2: m_dataLen = 3; break; // vector
  default:
    ErrMsg(MsgERR, msgHdr + string("invalid svType: ") + path);
    fclose(fp);
    return false;
  }

  bool dblPrec = false;
  switch ( pib[1] ) {
  case 1: break; // single precision
  case 2: dblPrec = true; break; // double precision
  default:
    ErrMsg(MsgERR, msgHdr + string("invalid dType: ") + path);
    fclose(fp);
    return false;
  }

  // read dims
  if ( dblPrec ) {
    if ( fread(buff, 1, 32, fp) < 32 ) {
      ErrMsg(MsgERR, msgHdr + string("can't read dims record: ") + path);
      fclose(fp);
      return false;
    }
    if ( emt == UnMatch ) LBSWAPVEC(plb, 3);
    m_dims[0] = (size_t)plb[0];
    m_dims[1] = (size_t)plb[1];
    m_dims[2] = (size_t)plb[2];
  } else {
    if ( fread(buff, 1, 20, fp) < 20 ) {
      ErrMsg(MsgERR, msgHdr + string("can't read dims record: ") + path);
      fclose(fp);
      return false;
    }
    if ( emt == UnMatch ) BSWAPVEC(pib, 3);
    m_dims[0] = (size_t)pib[0];
    m_dims[1] = (size_t)pib[1];
    m_dims[2] = (size_t)pib[2];
  }
  size_t dimSz = m_dims[0] * m_dims[1] * m_dims[2];
  if ( dimSz < 1 ) {
    ErrMsg(MsgERR, msgHdr + string("invalid dims: ") + path);
    fclose(fp);
    return false;
  }
  fclose(fp);

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

std::deque<std::string>
vsnData_Sph::setupLists(const std::deque<std::string>& path_lst) {
  deque<std::string> pathLst;

  register size_t numPath = path_lst.size();
  if ( numPath < 1 ) return pathLst;

  m_stpList.resize(numPath);
  m_origList.resize(numPath);
  m_pitchList.resize(numPath);

  unsigned char buff[32];
  int* pib = (int*)(&buff[4]);
  long long* plb = (long long*)(&buff[4]);
  float* pfb = (float*)(&buff[4]);
  double* pdb = (double*)(&buff[4]);

  register size_t i, idx = 0;
  for ( i = 0; i < numPath; i++ ) {
    FILE* fp = fopen(path_lst[i].c_str(), "rb");
    if ( ! fp ) continue;
    EMatchType emt = MatchEndian(fp, 8);
    if ( emt == UnKnown ) {fclose(fp); continue;}
    
    // read header
    if ( fread(buff, 1, 16, fp) < 16 ) {fclose(fp); continue;}
    if ( emt == UnMatch ) BSWAPVEC(pib, 2);
    bool dblPrec = false;
    switch ( pib[1] ) {
    case 1: break; // single precision
    case 2: dblPrec = true; break; // double precision
    default: fclose(fp); continue;
    }

    // read dims (skip)
    if ( dblPrec ) {
      if ( fread(buff, 1, 32, fp) < 32 ) {fclose(fp); continue;}
    } else {
      if ( fread(buff, 1, 20, fp) < 20 ) {fclose(fp); continue;}
    }

    // read org
    if ( dblPrec ) {
      if ( fread(buff, 1, 32, fp) < 32 ) {fclose(fp); continue;}
      if ( emt == UnMatch ) LBSWAPVEC(pdb, 3);
      m_origList[idx].m_v[0] = (float)pdb[0];
      m_origList[idx].m_v[1] = (float)pdb[1];
      m_origList[idx].m_v[2] = (float)pdb[2];
    } else {
      if ( fread(buff, 1, 20, fp) < 20 ) {fclose(fp); continue;}
      if ( emt == UnMatch ) BSWAPVEC(pfb, 3);
      m_origList[idx].m_v[0] = pfb[0];
      m_origList[idx].m_v[1] = pfb[1];
      m_origList[idx].m_v[2] = pfb[2];
    }

    // read pitch
    if ( dblPrec ) {
      if ( fread(buff, 1, 32, fp) < 32 ) {fclose(fp); continue;}
      if ( emt == UnMatch ) LBSWAPVEC(pdb, 3);
      m_pitchList[idx].m_v[0] = (float)pdb[0];
      m_pitchList[idx].m_v[1] = (float)pdb[1];
      m_pitchList[idx].m_v[2] = (float)pdb[2];
    } else {
      if ( fread(buff, 1, 20, fp) < 20 ) {fclose(fp); continue;}
      if ( emt == UnMatch ) BSWAPVEC(pfb, 3);
      m_pitchList[idx].m_v[0] = pfb[0];
      m_pitchList[idx].m_v[1] = pfb[1];
      m_pitchList[idx].m_v[2] = pfb[2];
    }

    // read time
    if ( dblPrec ) {
      if ( fread(buff, 1, 24, fp) < 24 ) {fclose(fp); continue;}
      if ( emt == UnMatch ) LBSWAPVEC(plb, 2);
      m_stpList[idx].step = (int)plb[0];
      m_stpList[idx].time = (float)pdb[1];
    } else {
      if ( fread(buff, 1, 16, fp) < 16 ) {fclose(fp); continue;}
      if ( emt == UnMatch ) BSWAPVEC(pib, 2);
      m_stpList[idx].step = pib[0];
      m_stpList[idx].time = pfb[1];
    }
    if ( idx != 0 ) {
      if ( m_stpList[idx].step <= m_stpList[idx -1].step )
	m_stpList[idx].step = m_stpList[idx -1].step + 1;
    }

    pathLst.push_back(path_lst[i]);
    fclose(fp);
    idx ++;
  } // end of for(i)

  m_stpList.resize(idx);
  m_origList.resize(idx);
  m_pitchList.resize(idx);
  return pathLst;
}

bool vsnData_Sph::adjustStepList(const std::deque<std::string>& path_lst,
				 const std::string& baseDir, xmlNodePtr xnp)
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


// MHIR append begin
#include <stdexcept>

void 
vsnData_Sph::getCellIndex(const CES::Vec3<float>& point,
			  CES::Vec3<int>* idx,
			  CES::Vec3<float>* rate)
{
  // ピッチの取得
  //
  CES::Vec3<float> pitch;
  getPitch(pitch);

  // インデックス
  //
  const int newi = (point[0] - _bbox[0][0]) / pitch[0];
  const int newj = (point[1] - _bbox[0][1]) / pitch[1];
  const int newk = (point[2] - _bbox[0][2]) / pitch[2];
  (*idx)[0] = newi;
  (*idx)[1] = newj;
  (*idx)[2] = newk;

  // pointが含まれるボクセルの原点の位置
  //
  const float x = _bbox[0][0] + pitch[0] * (float)(newi);
  const float y = _bbox[0][1] + pitch[1] * (float)(newj);
  const float z = _bbox[0][2] + pitch[2] * (float)(newk);

  // 位置の割合
  //
  const float u = (point[0] - x) / pitch[0];
  const float v = (point[1] - y) / pitch[1];
  const float w = (point[2] - z) / pitch[2];

  (*rate)[0] = u;
  (*rate)[1] = v;
  (*rate)[2] = w;
}

CES::Vec3<float>
vsnData_Sph::getValue(const CES::Vec3<int>& idx, const std::vector<bool>& didx)
{
  if (didx.size() > m_dataLen) throw std::runtime_error("Over DataLen");
  
  //  CES::Vec3<float> pitch;
  //  getPitch(pitch);

  //  const int DIM_X = (_bbox[1][0] - _bbox[0][0]) / pitch[0];
  //  const int DIM_Y = (_bbox[1][1] - _bbox[0][1]) / pitch[1];
  //  const int DIM_Z = (_bbox[1][2] - _bbox[0][2]) / pitch[2];

  const int DIM_X = m_dims[0];
  const int DIM_Y = m_dims[1];
  const int DIM_Z = m_dims[2];

  const int X = (idx[0] < 0) ? 0 : (idx[0] >= DIM_X) ? DIM_X - 1 : idx[0];
  const int Y = (idx[1] < 0) ? 0 : (idx[1] >= DIM_Y) ? DIM_Y - 1 : idx[1];
  const int Z = (idx[2] < 0) ? 0 : (idx[2] >= DIM_Z) ? DIM_Z - 1 : idx[2];

  const int IDX = (Z * DIM_X * DIM_Y + Y * DIM_X + X) * m_dataLen;
  
  const float* const data = getData();
  if (! data) throw std::runtime_error("can't get data ptr");

  CES::Vec3<float> result;
  for (size_t i = 0; i < didx.size(); ++i) {
    result[i] = (didx[i]) ? data[IDX + i] : 0.0;
  }

  return result;
}

float
vsnData_Sph::getValue(const CES::Vec3<int>& idx2, int vecIdx)
{
  if (vecIdx > m_dataLen - 1) throw std::runtime_error("Over DataLen");

  CES::Vec3<int> idx = idx2;
  //  CES::Vec3<float> pitch;
  //  getPitch(pitch);

  //  const int DIM_X = (_bbox[1][0] - _bbox[0][0]) / pitch[0];
  //  const int DIM_Y = (_bbox[1][1] - _bbox[0][1]) / pitch[1];
  //  const int DIM_Z = (_bbox[1][2] - _bbox[0][2]) / pitch[2];

  const int DIM_X = m_dims[0];
  const int DIM_Y = m_dims[1];
  const int DIM_Z = m_dims[2];

  if (idx[0] < 0) idx[0] = 0;
  if (idx[0] >= DIM_X) idx[0] = DIM_X -1;
  if (idx[1] < 0) idx[1] = 0;
  if (idx[1] >= DIM_Y) idx[1] = DIM_Y - 1;
  if (idx[2] < 0) idx[2] = 0;
  if (idx[2] >= DIM_Z) idx[2] = DIM_Z - 1;

  //  if (idx[0] < 0 || idx[0] >= DIM_X) throw std::runtime_error("range over");
  //  if (idx[1] < 0 || idx[1] >= DIM_Y) throw std::runtime_error("range over");
  //  if (idx[2] < 0 || idx[2] >= DIM_Z) throw std::runtime_error("range over");

  const int IDX = (idx[0] + idx[1] * DIM_X + idx[2] * DIM_X * DIM_Y)*m_dataLen;
  
  const float* const data = getData();
  if (! data) throw std::runtime_error("can't get data ptr");

  float result = data[IDX + vecIdx];

  return result;
}

#ifdef OLD
static float
bilinear1DLocalFloat(const std::vector<float>& f, float x)
{
  return (1 - x) * f[0] + x * f[1];
}

static float
bilinear2DLocalFloat(const std::vector<std::vector<float> >& f,
		     float x, float y)
{
  return (1 - x) * bilinear1DLocalFloat(f[0], y)
             + x * bilinear1DLocalFloat(f[1], y);
}

static float
bilinear3DLocalFloat(const std::vector<std::vector<std::vector<float> > >& f,
		     const CES::Vec3<float>& rate)
{
  const float x(rate[0]);
  const float y(rate[1]);
  const float z(rate[2]);

  return (1 - x) * bilinear2DLocalFloat(f[0], y, z) +
    x * bilinear2DLocalFloat(f[1], y, z);
}
#endif // OLD

// TriLinear_Interp : tri-linear interpolation
bool 
TriLinear_Interp(const Vec3<float>& x0,
		 const Vec3<float>& sz,
		 const Vec3<float>& pos,
		 const std::vector<bool>& didx,
		 CES::Vec3<float> data[8],
		 CES::Vec3<float>& dval)
{
  if ( sz.m_v[0] == 0.f || sz.m_v[1] == 0.f || sz.m_v[2] == 0.f )
    return false;

  Vec3<float> u[2];
  u[1].m_v[0] = (pos.m_v[0] - x0.m_v[0]) / sz.m_v[0];
  u[1].m_v[1] = (pos.m_v[1] - x0.m_v[1]) / sz.m_v[1];
  u[1].m_v[2] = (pos.m_v[2] - x0.m_v[2]) / sz.m_v[2];
  u[0] = Vec3<float>(1.f, 1.f, 1.f) - u[1];

  for (size_t l = 0; l < didx.size(); l++ ) {
    dval[l] = 0.f;
    if (! didx[l]) continue;

    dval[l] += // (0,0,0)
      u[0].m_v[0] * u[0].m_v[1] * u[0].m_v[2] * data[0][l];
    dval[l] += // (1,0,0)
      u[1].m_v[0] * u[0].m_v[1] * u[0].m_v[2] * data[1][l];
    dval[l] += // (0,1,0)
      u[0].m_v[0] * u[1].m_v[1] * u[0].m_v[2] * data[2][l];
    dval[l] += // (1,1,0)
      u[1].m_v[0] * u[1].m_v[1] * u[0].m_v[2] * data[3][l];
    dval[l] += // (0,0,1)
      u[0].m_v[0] * u[0].m_v[1] * u[1].m_v[2] * data[4][l];
    dval[l] += // (1,0,1)
      u[1].m_v[0] * u[0].m_v[1] * u[1].m_v[2] * data[5][l];
    dval[l] += // (0,1,1)
      u[0].m_v[0] * u[1].m_v[1] * u[1].m_v[2] * data[6][l];
    dval[l] += // (1,1,1)
      u[1].m_v[0] * u[1].m_v[1] * u[1].m_v[2] * data[7][l];
  } // end of for(l)
  return true;
}

/**
 * @param interpolateMode (=0) ゼロ次
 *                        (=1) TriLinear
 *                        (=2) 外挿１次
 *                        (=3) 外挿２次
 */
std::vector<float>
vsnData_Sph::interpolateData(const CES::Vec3<float>& pos,
			     const std::vector<bool>& didx,
			     int interpolateMode, const CES::Vec3<float>& n,
			     bool exUseTrilinear)
{
  std::vector<float> result;

  // データの長さを超えていないこと
  //
  if (didx.size() > m_dataLen) {
    std::cout << "datalen invalid" << std::endl;
    return result;
  }

  // データの領域を超えないこと
  //
  if (!((_bbox[0][0] <= pos[0] && pos[0] <= _bbox[1][0]) && 
	(_bbox[0][1] <= pos[1] && pos[1] <= _bbox[1][1]) &&
	(_bbox[0][2] <= pos[2] && pos[2] <= _bbox[1][2]))) {
    // 全て0を返す
    //
    //    std::cout << "range over" << std::endl;
    for (int i = 0; i < didx.size(); ++i) result.push_back(0.f);
    return result;
  }
  
  // 指定した座標値のボクセルのインデックスとボクセル内の位置を求める
  //
  CES::Vec3<int> newIdx;
  CES::Vec3<float> rate;
  getCellIndex(pos, &newIdx, &rate);

  if (interpolateMode == 0) {
    // ゼロ次補間
    //
    // ボクセルの中心の座標値
    //
    CES::Vec3<float> dval = getValue(newIdx, didx);
    if (didx.size() == 1) {
      result.push_back(dval[0]);
    } else {
      result.push_back(dval[0]);
      result.push_back(dval[1]);
      result.push_back(dval[2]);
    }
    return result;
  } else if (interpolateMode == 2) {
    // 外挿１次
    //
    const CES::Vec3<float>* bbox = getBbox();
    const CES::Vec3<size_t>& dims = getDims();
    CES::Vec3<float> pitch;
    getPitch(pitch);

    // ベクトル方向にピッチ相当離れた位置
    //
    CES::Vec3<float> newPos;
    for (size_t i = 0; i < 3; ++i) newPos[i] = pos[i] + n[i] * pitch[i];

    //    return interpolateData(newPos, didx, 0, n);
    return interpolateData(newPos, didx, (exUseTrilinear) ? 1 : 0, n, false);

  } else if (interpolateMode == 3) {
    // 外挿２次
    //
    const CES::Vec3<float>* bbox = getBbox();
    const CES::Vec3<size_t>& dims = getDims();
    CES::Vec3<float> pitch;
    getPitch(pitch);

    // ベクトル方向にピッチ相当離れた位置
    // ベクトル方向にピッチ相当の２倍離れた位置    
    //
    CES::Vec3<float> newPos1, newPos2;
    for (size_t i = 0; i < 3; ++i) {
      newPos1[i] = pos[i] + n[i] * pitch[i];
      newPos2[i] = pos[i] + n[i] * pitch[i] * 2;
    }

    std::vector<float> result1
      = interpolateData(newPos1, didx, (exUseTrilinear) ? 1 : 0, n, false);
    std::vector<float> result2
      = interpolateData(newPos2, didx, (exUseTrilinear) ? 1 : 0, n, false);

    std::vector<float> result;
    if (result1.size() == result2.size()) {
      for (size_t i = 0; i < result1.size(); ++i) {
	result.push_back(2 * result1[i] - result2[i]);
      }
    }
    return result;
  }

  if (rate[0] == 0.5 && rate[1] == 0.5 && rate[2] == 0.5) {
    // ボクセルの中心の座標値
    //
    CES::Vec3<float> dval = getValue(newIdx, didx);
    if (didx.size() == 1) {
      result.push_back(dval[0]);
    } else {
      result.push_back(dval[0]);
      result.push_back(dval[1]);
      result.push_back(dval[2]);
    }
    return result;
  }

  // rate < 0.5 の時は手前のボクセルから対象にする
  //
  if (rate[0] < 0.5) newIdx[0] -= 1; if (newIdx[0] < 0) newIdx[0] = 0;
  if (rate[1] < 0.5) newIdx[1] -= 1; if (newIdx[1] < 0) newIdx[1] = 0;
  if (rate[2] < 0.5) newIdx[2] -= 1; if (newIdx[2] < 0) newIdx[2] = 0;

  // インデックスの最大値
  //
  CES::Vec3<float> pitch;
  getPitch(pitch);

  const int DIM_X = m_dims[0];
  const int DIM_Y = m_dims[1];
  const int DIM_Z = m_dims[2];

  CES::Vec3<float> data[8];
  int lIdx = 0;
  for (int k = 0; k < 2; ++k) {
    for (int j = 0; j < 2; ++j) {
      for (int i = 0; i < 2; ++i) {
	int ii = newIdx[0] + i; if (ii >= DIM_X) ii = DIM_X - 1;
	int jj = newIdx[1] + j; if (jj >= DIM_Y) jj = DIM_Y - 1;
	int kk = newIdx[2] + k; if (kk >= DIM_Z) kk = DIM_Z - 1;

	data[lIdx++] = getValue(CES::Vec3<int>(ii, jj, kk), didx);
      }
    }
  }

  CES::Vec3<float> x0;
  x0[0] = _bbox[0][0] + pitch[0] * (float)(newIdx[0]) + pitch[0] * 0.5;
  x0[1] = _bbox[0][1] + pitch[1] * (float)(newIdx[1]) + pitch[1] * 0.5;
  x0[2] = _bbox[0][2] + pitch[2] * (float)(newIdx[2]) + pitch[2] * 0.5;
  CES::Vec3<float> dval;

  TriLinear_Interp(x0, pitch, pos, didx, data, dval);

  if (didx.size() == 1) {
    result.push_back(dval[0]);
  } else {
    result.push_back(dval[0]);
    result.push_back(dval[1]);
    result.push_back(dval[2]);
  }

  return result;
}

// MHIR append end
