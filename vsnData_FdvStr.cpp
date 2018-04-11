//
// vsnData_FdvStr
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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "vsnData_FdvStr.h"
#include "vsnError.h"

using namespace std;
using namespace CES;
using namespace VSN;


/* constructors / destructor */

vsnData_FdvStr::vsnData_FdvStr(const string& name)
  : vsnData_Sv(name), vsnDataCacheUF(), m_endianFlag(UnKnown),
    m_dtUpdStp(-1), m_bbUpdStp(-1), m_pWkData(NULL)
{
  m_dataMode[0] = m_dataMode[1] = m_dataMode[2] = NODATA;
  m_pStaticData[0] = m_pStaticData[1] = m_pStaticData[2] = NULL;
}

vsnData_FdvStr::~vsnData_FdvStr() {
  register int i;
  for ( i = 0; i < 3; i++ ) {
    if ( m_pStaticData[i] )
      DeAllocate((void*)m_pStaticData[i]);
  } // end of for(i)

  if ( m_pWkData )
    DeAllocate((void*)m_pWkData);
}


/* from vsnData_Sv */

bool vsnData_FdvStr::isUniformCoord() const {
  return (getDataMode(GRID) == NODATA);
}

const float* const vsnData_FdvStr::getCoord(const size_t stp) {
  ModeType dmode = getDataMode(GRID);
  if ( dmode == NODATA || dmode == STATIC || stp == 0 )
    return &m_pStaticData[GRID][1];

  const unsigned char* const dptr = vsnDataCache::getData(stp -1);
  if ( ! dptr ) return NULL;
  const float* const fptr = (const float* const)dptr;
  return &fptr[1];
}

const float* const vsnData_FdvStr::getData(const size_t stp) {
  if ( m_dtUpdStp == (int)stp && m_pWkData )
    return m_pWkData;

  m_dtUpdStp = -1;
  register size_t dimSz = m_dims[0] * m_dims[1] * m_dims[2];
  if ( dimSz < 1 ) return NULL;
  if ( m_dataLen < 1 || m_dataLen == 2 || m_dataLen > 4 ) return NULL;
  register size_t dataSz = dimSz * sizeof(float);
  m_pWkData = (float*)ReAllocate(m_pWkData, dataSz * m_dataLen);
  if ( ! m_pWkData ) return NULL;
  
  register size_t i;
  float *pwk = m_pWkData;
  const float *pvec, *psca;

  if ( stp == 0 ) {
    if ( m_dataLen == 1 ) { // only SCALAR data exists
      memcpy(m_pWkData, &m_pStaticData[SCALAR][1], dataSz);
    }
    else if ( m_dataLen == 3 ) { // only VECTOR data exists
      memcpy(m_pWkData, &m_pStaticData[VECTOR][1], dataSz*3);
    }
    else if ( m_dataLen == 4 ) { // both VECTOR and SCALAR data exists
      pvec = &m_pStaticData[VECTOR][1];
      psca = &m_pStaticData[SCALAR][1];
      for ( i = 0; i < dimSz; i++ ) {
	*pwk++ = *pvec++; *pwk++ = *pvec++; *pwk++ = *pvec++;
	*pwk++ = *psca++;
      } // end of for(i)
    }

    m_dtUpdStp = (int)stp;
    return m_pWkData;
  }

  const unsigned char* const dptr = vsnDataCache::getData(stp -1);
  if ( ! dptr ) return NULL;
  const float* const fptr = (const float* const)dptr;

  size_t vecOffset = 1, scaOffset = 1;
  if ( m_dataMode[GRID] == DYNAMIC ) {
    vecOffset += (dimSz * 3 + 2);
    scaOffset += (dimSz * 3 + 2);
  }
  if ( m_dataMode[SCALAR] == DYNAMIC )
    vecOffset += (dimSz + 2);

  // copy datas
  if ( m_dataLen == 1 ) { // only SCALAR data exists
    if ( m_dataMode[SCALAR] == DYNAMIC  )
      memcpy(m_pWkData, &fptr[scaOffset], dataSz);
    else if ( m_dataMode[SCALAR] == STATIC )
      memcpy(m_pWkData, &m_pStaticData[SCALAR][1], dataSz);
  }
  else if ( m_dataLen == 3 ) { // only VECTOR data exists
    if ( m_dataMode[VECTOR] == DYNAMIC  )
      memcpy(m_pWkData, &fptr[vecOffset], dataSz*3);
    else if ( m_dataMode[VECTOR] == STATIC )
      memcpy(m_pWkData, &m_pStaticData[VECTOR][1], dataSz*3);
  }
  else if ( m_dataLen == 4 ) { // both VECTOR and SCALAR data exists
    if ( m_dataMode[VECTOR] == DYNAMIC )
      pvec = &fptr[vecOffset];
    else
      pvec = &m_pStaticData[VECTOR][1];
    if ( m_dataMode[SCALAR] == DYNAMIC  )
      psca = &fptr[scaOffset];
    else
      psca = &m_pStaticData[SCALAR][1];

    for ( i = 0; i < dimSz; i++ ) {
      *pwk++ = *pvec++; *pwk++ = *pvec++; *pwk++ = *pvec++;
      *pwk++ = *psca++;
    } // end of for(i)
  }

  m_dtUpdStp = (int)stp;
  return m_pWkData;
}

bool vsnData_FdvStr::updateBbox(const int stp) {
  if ( stp < 0 || stp >= m_numStps ) return false;
  if ( m_bbUpdStp == stp ) return true;

  register size_t idx, iidx, dimSz = m_dims[0]*m_dims[1]*m_dims[2];
  const float *pd;

  pd = getCoord(stp);
  if ( getDataMode(GRID) == NODATA || ! pd ) {
    _bbox[0] = Vec3<float>(0.f, 0.f, 0.f);
    _bbox[1] = Vec3<float>(m_dims[0]-1.f, m_dims[1]-1.f, m_dims[2]-1.f);
    m_bbUpdStp = stp;
    return true;
  }
  else {
    _bbox[0] = Vec3<float>(pd[0], pd[1], pd[2]);
    _bbox[1] = Vec3<float>(pd[3*dimSz-3], pd[3*dimSz-2], pd[3*dimSz-1]);
    for ( idx = 0; idx < dimSz; idx++ ) {
      iidx = dimSz -1 -idx;
      if ( _bbox[0].m_v[0] > pd[idx*3  ] ) _bbox[0].m_v[0] = pd[idx*3  ];
      if ( _bbox[0].m_v[1] > pd[idx*3+1] ) _bbox[0].m_v[1] = pd[idx*3+1];
      if ( _bbox[0].m_v[2] > pd[idx*3+2] ) _bbox[0].m_v[2] = pd[idx*3+2];

      if ( _bbox[1].m_v[0] < pd[iidx*3  ] ) _bbox[1].m_v[0] = pd[iidx*3  ];
      if ( _bbox[1].m_v[1] < pd[iidx*3+1] ) _bbox[1].m_v[1] = pd[iidx*3+1];
      if ( _bbox[1].m_v[2] < pd[iidx*3+2] ) _bbox[1].m_v[2] = pd[iidx*3+2];
    }
  }
  checkBbox();
  notice();

  m_bbUpdStp = stp;
  return true;
}


/* from vsnDataObj */

bool vsnData_FdvStr::init(const string& path, xmlNodePtr xnp) {
  if ( path.empty() ) return false;

  //if ( m_ready ) return false;
  m_ready = false;

  // setup cache : setupUF() will be called
  if ( ! setupCachePool(path) ) {
    if ( m_numStps != 1 )
      return false;
  }

  // set min/max
  vsnApp* pApp = vsnApp::GetApp();
  if ( ! checkMinMax(true, pApp->isChkProgress()) ) {
    return false;
  }

  // set bbox
  //generateBbox();
  updateBbox();

  m_ready = true;
  setXmlNode(xnp);
  return true;
}


/* from vsnDataCacheUF */

bool vsnData_FdvStr::setupUF(const string& path) {
  string msgHdr
    = getDataType() + string("[") + getName() + string("]: setup: ");

  // check data type
  int strType = CheckStrDataType(path);
  if ( strType == 0 ) {
    ErrMsg(MsgERR, msgHdr + string("not FdvStr data file: ") + path);
    return false;
  }
  if ( strType != 1 && strType != -1 ) {
    ErrMsg(MsgERR, msgHdr
	   + string("not float type FdvStr data file: ") + path);
    return false;
  }

  // endian match type
  if ( strType == -1 ) m_endianFlag = UnMatch;
  else m_endianFlag = Match;

  //// do setup ////
  int headBuff[11];

  FILE* fp = fopen(path.c_str(), "rb");
  if ( ! fp ) {
    ErrMsg(MsgERR, msgHdr + string("can't open file: ") + path);
    return false;
  }
#ifdef WINDOWS
  struct _stat st_buf;
  if ( _fstat(fileno(fp), &st_buf) < 0 ) {
#else
  struct stat st_buf;
  if ( fstat(fileno(fp), &st_buf) < 0 ) {
#endif
    ErrMsg(MsgERR, msgHdr + string("can't get file status: ") + path);
    fclose(fp);
    return false;
  }
  // length of file(in words)
  size_t iwlen = st_buf.st_size / 4;

  // read header
  if ( fread(headBuff, sizeof(int), 11, fp) < 11 ) {
    ErrMsg(MsgERR, msgHdr + string("can't read header record: ") + path);
    fclose(fp);
    return false;
  }
  if ( m_endianFlag == UnMatch ) BSWAPVEC(headBuff, 11);

  // check MODE
  if ( headBuff[1] == 0 ) {
    ErrMsg(MsgERR, msgHdr + string("invalid MODE: ") + path);
    fclose(fp);
    return false;
  }
  m_dataMode[GRID]   = (ModeType)(headBuff[1] % 10);
  m_dataMode[SCALAR] = (ModeType)((headBuff[1] % 100) / 10);
  m_dataMode[VECTOR] = (ModeType)((headBuff[1] % 1000) / 100);

  // check DIMS
  m_dims[0] = headBuff[7];
  m_dims[1] = headBuff[8];
  m_dims[2] = headBuff[9];
  if ( m_dims[0] < 1 || m_dims[1] < 1 || m_dims[2] < 1 ) {
    ErrMsg(MsgERR, msgHdr + string("invalid DIMS: ") + path);
    fclose(fp);
    return false;
  }
  size_t dimSize = m_dims[0] * m_dims[1] * m_dims[2];
  register size_t Sz3 = dimSize * 3 + 2;
  register size_t Sz1 = dimSize + 2;

  // get #of steps
  m_numStps = 0;
  register size_t iindex = 11; // offset header record
  while ( 1 ) {
    iindex += 4; // offset time record
    if ( iindex > iwlen ) break;

    // grid record
    if ( m_dataMode[GRID] == DYNAMIC ||
         (m_dataMode[GRID] == STATIC && m_numStps == 0) )
      iindex += Sz3;

    // scalar record
    if ( m_dataMode[SCALAR] == DYNAMIC ||
         (m_dataMode[SCALAR] == STATIC && m_numStps == 0) )
      iindex += Sz1;

    // vector record
    if ( m_dataMode[VECTOR] == DYNAMIC ||
         (m_dataMode[VECTOR] == STATIC && m_numStps == 0) )
      iindex += Sz3;

    if ( iindex > iwlen ) break;
    m_numStps ++;
  } // end of while ( 1 )

  if ( m_numStps < 1 ) {
    ErrMsg(MsgERR, msgHdr + string("no data exists in file: ") + path);
    fclose(fp);
    return false;
  }

  // allocate stpList
  m_stpList.resize(m_numStps);

  // allocate memory for the data of the 1st step
  m_pStaticData[GRID]
    = (float*)ReAllocate(m_pStaticData[GRID], Sz3 * sizeof(float));
  if ( ! m_pStaticData[GRID] ) {
    ErrMsg(MsgERR, msgHdr
	   + string("can't allocate memory for grid: ") + path);
    fclose(fp);
    return false;
  }
  if ( m_dataMode[SCALAR] != NODATA ) {
    m_pStaticData[SCALAR]
      = (float*)ReAllocate(m_pStaticData[SCALAR], Sz1 * sizeof(float));
    if ( ! m_pStaticData[SCALAR] ) {
      ErrMsg(MsgERR, msgHdr
	     + string("can't allocate memory for scalar: ") + path);
      fclose(fp);
      return false;
    }
  }
  if ( m_dataMode[VECTOR] != NODATA ) {
    m_pStaticData[VECTOR]
      = (float*)ReAllocate(m_pStaticData[VECTOR], Sz3*sizeof(float));
    if ( ! m_pStaticData[VECTOR] ) {
      ErrMsg(MsgERR, msgHdr
	     + string("can't allocate memory for vector: ") + path);
      fclose(fp);
      return false;
    }
  }

  // step size of dynamic data
  m_cacheUnitSize = 0;
  if ( m_dataMode[GRID] == DYNAMIC )
    m_cacheUnitSize += Sz3 * sizeof(float);
  if ( m_dataMode[SCALAR] == DYNAMIC )
    m_cacheUnitSize += Sz1 * sizeof(float);
  if ( m_dataMode[VECTOR] == DYNAMIC )
    m_cacheUnitSize += Sz3 * sizeof(float);
  if ( m_cacheUnitSize > 0 ) {
    m_totalSteps = m_numStps - 1; // the 1st step is not in cache
    m_fposList.resize(m_totalSteps);
  }
  // adjust cacheSteps
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

  // set stpList, fposList
  register int istep = 0;
  while ( istep < m_numStps ) {
    int ibuff[4];
    float* fbuff = (float*)ibuff;

    // read time record
    if ( fread(ibuff, sizeof(int), 4, fp) < 4 ) {
      ErrMsg(MsgERR, msgHdr + string("can't read time record: ") + path);
      fclose(fp);
      return false;
    }
    if ( m_endianFlag == UnMatch ) BSWAPVEC(ibuff, 4);
    m_stpList[istep].step = ibuff[1];
    m_stpList[istep].time = fbuff[2];
    if ( istep != 0 ) {
      if ( m_stpList[istep].step <= m_stpList[istep -1].step )
	m_stpList[istep].step = m_stpList[istep -1].step + 1;
    }

    // fpos list of DataCacheUF
    if ( istep != 0 && m_cacheUnitSize > 0 )
      fgetpos(fp, &m_fposList[istep -1]);

    // grid record
    if ( m_dataMode[GRID] != NODATA ) {
      if ( istep == 0 ) {
	if ( fread(m_pStaticData[GRID], sizeof(int), Sz3, fp) < Sz3 ) {
	  ErrMsg(MsgERR, msgHdr + string("can't read grid data: ") + path);
	  fclose(fp);
	  return false;
	}
	if ( m_endianFlag == UnMatch )
	  BSWAPVEC(m_pStaticData[GRID], Sz3);
      }
      else if ( m_dataMode[GRID] == DYNAMIC )
	if ( fseek(fp, Sz3*sizeof(int), SEEK_CUR) != 0 ) {
	  ErrMsg(MsgERR, msgHdr + string("can't seek grid data: ") + path);
	  fclose(fp);
	  return false;
	}
    }

    // scalar record
    if ( m_dataMode[SCALAR] != NODATA ) {
      if ( istep == 0 ) {
	if ( fread(m_pStaticData[SCALAR], sizeof(int), Sz1, fp) < Sz1 ) {
	  ErrMsg(MsgERR, msgHdr + string("can't read scalar data: ") + path);
	  fclose(fp);
	  return false;
	}
	if ( m_endianFlag == UnMatch )
	  BSWAPVEC(m_pStaticData[SCALAR], Sz1);
      }
      else if ( m_dataMode[SCALAR] == DYNAMIC )
	if ( fseek(fp, Sz1*sizeof(int), SEEK_CUR) != 0 ) {
	  ErrMsg(MsgERR, msgHdr + string("can't seek scalar data: ") + path);
	  fclose(fp);
	  return false;
	}
    }

    // vector record
    if ( m_dataMode[VECTOR] != NODATA ) {
      if ( istep == 0 ) {
	if ( fread(m_pStaticData[VECTOR], sizeof(int), Sz3, fp) < Sz3 ) {
	  ErrMsg(MsgERR, msgHdr + string("can't read vector data: ") + path);
	  fclose(fp);
	  return false;
	}
	if ( m_endianFlag == UnMatch )
	  BSWAPVEC(m_pStaticData[VECTOR], Sz3);
      }
      else if ( m_dataMode[VECTOR] == DYNAMIC )
	if ( fseek(fp, Sz3*sizeof(int), SEEK_CUR) != 0 ) {
	  ErrMsg(MsgERR, msgHdr + string("can't seek vector data: ") + path);
	  fclose(fp);
	  return false;
	}
    }

    istep ++;   
  } // end of while ( istep < m_numStps )

  // store FILE*
  m_cacheFILE = fp;

  // indexed-grid data
  if ( m_dataMode[GRID] == NODATA ) {
    register size_t i, j, k;
    iindex = 1;
    for ( k = 0; k < m_dims[2]; k++ )
      for ( j = 0; j < m_dims[1]; j++ )
	for ( i = 0; i < m_dims[0]; i++ ) {
	  m_pStaticData[GRID][iindex++] = (float)i;
	  m_pStaticData[GRID][iindex++] = (float)j;
	  m_pStaticData[GRID][iindex++] = (float)k;
	} // end of for(i)
  } // end of if(GRID==NODATA)

  // set dataLen
  m_dataLen = 0;
  if ( m_dataMode[VECTOR] != NODATA ) m_dataLen += 3;
  if ( m_dataMode[SCALAR] != NODATA ) m_dataLen += 1;

  return true;
}


/* from vsnDataCache */

bool vsnData_FdvStr::readin(const size_t stp, unsigned char* pdata) {
  if ( ! isCacheReady() ) return false;
  if ( ! pdata ) return false;
  if ( stp >= m_totalSteps ) return false;

  // set file position
  if ( fsetpos(m_cacheFILE, &m_fposList[stp]) != 0 ) return false;

  // read m_cacheUnitSize bytes data from m_cacheFILE into 'pdata'
  size_t numElm = m_cacheUnitSize / 4;
  if ( fread(pdata, 4, numElm, m_cacheFILE) < numElm ) {
    return false;
  }

  // convert endian if m_endianFlag equals to UnMatch
  int* pint = (int*)pdata;
  if ( m_endianFlag == UnMatch )
    BSWAPVEC(pint, numElm);

  return true;
}


/* static methods */

// CheckStrDataType - returns StrDataFile type as follows
//  1/-1 : Standard StrData (endian unmatched)
//  2/-2 : Double precision StrData (endian unmatched)
//  3    : Ascii StrData
//  0    : Invalid data(not StrData)
int vsnData_FdvStr::CheckStrDataType(const string& path) {
  // open the file
  if ( path.empty() ) return 0;
  FILE* fp = fopen(path.c_str(), "rb");
  if ( ! fp ) return 0;

  // read the first 4bytes, and close
  int idat, xidat;
  if ( fread(&idat, 4, 1, fp) < 1 ) {fclose(fp); return 0;}
  fclose(fp);

  // check type
  xidat = BSWAP_X_32(idat);
  if ( idat == 4 ) return 1; // Standard
  else if ( xidat == 4 ) return -1; // Standard(endian unmatched)
  else if ( idat == 8 ) return 2; // Double precision
  else if ( xidat == 8 ) return -2; // Double precision(endian unmatched)
  else if ( idat == 0x23344456 || xidat == 0x23344456 ) return 3; // Ascii
  else return 0;
}
