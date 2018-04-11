//
// vsnData_Sv
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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <zlib.h>

// need to include before vsnData (by reason of glew)
#include "vsnMethod_Sv_shaderVolren.h"

#include "vsnData_Sv.h"
#include "vsnDataCache.h"
#include "vsnError.h"
#include "vsnMethod_Sv_bounds.h"
#include "vsnMethod_Sv_crop.h"
#include "vsnMethod_Sv_extractHistory.h"
#include "vsnMethod_Sv_graphPlot.h"
#include "vsnMethod_Sv_histogram.h"
#include "vsnMethod_Sv_info.h"
#include "vsnMethod_Sv_isosurf.h"
#include "vsnMethod_Sv_minmaxGlyph.h"
#include "vsnMethod_Sv_orthoContour.h"
#include "vsnMethod_Sv_orthoProbe.h"
#include "vsnMethod_Sv_orthoScalar.h"
#include "vsnMethod_Sv_orthoSlicer.h"
#include "vsnMethod_Sv_orthoVector.h"
#include "vsnMethod_Sv_plotContour.h"
#include "vsnMethod_Sv_plotScalar.h"
#include "vsnMethod_Sv_plotVector.h"
#include "vsnMethod_Sv_probe.h"
#include "vsnMethod_Sv_setMask.h"
#include "vsnMethod_Sv_setRectCoord.h"
#include "vsnMethod_Sv_staggeredVector.h"
#include "vsnMethod_Sv_streamLines.h"
#include "vsnMethod_Sv_volren.h"
#include "vsnMethod_keyFrameAnim.h"
#include "vsnMethod_label.h"
#include "vsnMethod_sampler.h"
#include "vsnMethod_timeStep.h"
#include "vsnMethod_timeStepSync.h"

//sgi edit
#include "vsnMethod_Sv_VLD.h"
//end of sgi edit

using namespace std;
using namespace CES;
using namespace VSN;


/* static members */

deque<string> vsnData_Sv::s_methodList;


/* constructors / destructor */

vsnData_Sv::vsnData_Sv(const string& name)
  : vsnDataObj(name), m_numStps(0),
    m_mask(NULL), m_maskWk(NULL), m_minMaxMask(false)
{
}

vsnData_Sv::~vsnData_Sv() {
  if ( m_mask ) {
    CES::DeAllocate(m_mask);
    m_mask = NULL;
  }
  if ( m_maskWk ) {
    CES::DeAllocate(m_maskWk);
    m_maskWk = NULL;
  }
}


/* from vsnDataObj */

std::deque<std::string> vsnData_Sv::getSupportMethodList() const {
  if ( s_methodList.empty() ) {
    s_methodList.push_back(string("bounds"));
    s_methodList.push_back(string("crop"));
    if ( getDataType() == string("Sph") || getDataType() == string("P3dF")
	 || getDataType() == string("DfiSv") )
      s_methodList.push_back(string("extractHistory"));
    s_methodList.push_back(string("graphPlot"));
    s_methodList.push_back(string("histogram"));
    s_methodList.push_back(string("info"));
    s_methodList.push_back(string("isosurf"));
    s_methodList.push_back(string("keyFrameAnim"));
    s_methodList.push_back(string("label"));
    s_methodList.push_back(string("minmaxGlyph"));
    s_methodList.push_back(string("orthoContour"));
    s_methodList.push_back(string("orthoProbe"));
    s_methodList.push_back(string("orthoScalar"));
    s_methodList.push_back(string("orthoSlicer"));
    s_methodList.push_back(string("orthoVector"));
    s_methodList.push_back(string("plotContour"));
    s_methodList.push_back(string("plotScalar"));
    s_methodList.push_back(string("plotVector"));
    s_methodList.push_back(string("probe"));
    s_methodList.push_back(string("sampler"));
    s_methodList.push_back(string("setMask"));
    s_methodList.push_back(string("setRectCoord"));
    s_methodList.push_back(string("shaderVolren"));
    s_methodList.push_back(string("staggeredVector"));
    s_methodList.push_back(string("streamLines"));
    s_methodList.push_back(string("timeStep"));
    s_methodList.push_back(string("timeStepSync"));
    s_methodList.push_back(string("volren"));
    // sgi edit
    s_methodList.push_back(string("VLD"));
    // end of sgi edit
  }
  return s_methodList;
}

vsnMethodObj* vsnData_Sv::addNewMethod(const std::string& mtype,
				       const std::string& mname) {
  string modType(mtype);
  vsnMethodObj* pMethod = NULL;
  string msgHdr
    = string("DataSv[") + getName() + string("]: addNewMethod: ");

  // instance method
  if ( modType == string("info") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_Sv_info());
  } // end of info
  else if ( modType == string("bounds") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_Sv_bounds());
  } // end of bounds
  else if ( modType == string("crop") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_Sv_crop());
  } // end of crop
  else if ( modType == string("extractHistory") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_Sv_extractHistory());
  } // end of extractHistory
  else if ( modType == string("graphPlot") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_Sv_graphPlot());
  } // end of graphPlot
  else if ( modType == string("histogram") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_Sv_histogram());
  } // end of histogram
  else if ( modType == string("isosurf") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_Sv_isosurf());
  } // end of isosurf
  else if ( modType == string("keyFrameAnim") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_keyFrameAnim());
  } // end of keyFrameAnim
  else if ( modType == string("label") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_label());
  } // end of label
  else if ( modType == string("minmaxGlyph") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_Sv_minmaxGlyph());
  } // end of minmaxGlyph
  else if ( modType == string("orthoContour") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_Sv_orthoContour());
  } // end of orthoContour
  else if ( modType == string("orthoProbe") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_Sv_orthoProbe());
  } // end of orthoProbe
  else if ( modType == string("orthoScalar") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_Sv_orthoScalar());
  } // end of orthoScalar
  else if ( modType == string("orthoSlicer") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_Sv_orthoSlicer());
  } // end of orthoSlicer
  else if ( modType == string("orthoVector") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_Sv_orthoVector());
  } // end of orthoVector
  else if ( modType == string("plotContour") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_Sv_plotContour());
  } // end of plotContour
  else if ( modType == string("plotScalar") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_Sv_plotScalar());
  } // end of plotScalar
  else if ( modType == string("plotVector") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_Sv_plotVector());
  } // end of plotVector
  else if ( modType == string("probe") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_Sv_probe());
  } // end of probe
  else if ( modType == string("sampler") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_sampler());
  } // end of sampler
  else if ( modType == string("setMask") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_Sv_setMask());
  } // end of setMask
  else if ( modType == string("setRectCoord") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_Sv_setRectCoord());
  } // end of setRectCoord
  else if ( modType == string("staggeredVector") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_Sv_staggeredVector());
  } // end of staggeredVector
  else if ( modType == string("shaderVolren") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_Sv_shaderVolren());
  } // end of shaderVolren
  else if ( modType == string("streamLines") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_Sv_streamLines());
  } // end of streamLines
  else if ( modType == string("timeStep") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_timeStep());
  } // end of timeStep
  else if ( modType == string("timeStepSync") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_timeStepSync());
  } // end of timeStepSync
  else if ( modType == string("volren") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_Sv_volren());
  } // end of volren
  // sgi edit
  else if ( modType == string("VLD") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_Sv_VLD());
  } // end of VLD
  // end of sgi edit 

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

bool vsnData_Sv::setMaskSVX(const std::string& mskPath, const MaskType mskType,
			    const float* vrRange, const int medId,
			    const bool chkMinMax, const int* pOfst) {
  if ( ! m_ready ) return false;
  vsnApp* pApp = vsnApp::GetApp();

  register int idx, i, j, k;
  if ( mskType == MASK_None ) {
    bool ret = true;
    if ( m_mask ) {
      CES::DeAllocate(m_mask);
      m_mask = NULL;
#if 0
      if ( m_minMaxMask ) {
	m_minMaxMask = false;
	if ( ! checkMinMax(true, pApp->isChkProgress()) ) return false;
      }
#endif
      size_t numMtds = getNumMethod();
      for ( i = 0; i < numMtds; i++ ) {
	vsnTimeSeriesMethodIF* pMtdTS
	  = dynamic_cast<vsnTimeSeriesMethodIF*>(getMethod(i));
	if ( ! pMtdTS ) continue;
	if ( ! pMtdTS->updateStep(m_currentStepIdx) )
	  ret = false;
      } // end of for(i)
    }
    return ret;
  }

  // load svx file
  enum {
    VDT_VolRate   = (1<<0),
    VDT_OpenRate  = (1<<1),
    VDT_VoxMedium = (1<<2),
    VDT_VoxBC     = (1<<3),
    VDT_FaceBC    = (1<<4),
    VDT_All = (VDT_VolRate|VDT_OpenRate|VDT_VoxMedium|VDT_VoxBC|VDT_FaceBC)
  };

  string errMsg = string("DataSv[") + getName() + string("]: setMaskSVX: ");
  FILE* fp = fopen(mskPath.c_str(), "rb");
  if ( ! fp ) {
    ErrMsg(MsgERR, errMsg + string("can't open file: ") + mskPath);
    return false;
  }
  bool ivConv = false;
  EMatchType emt = MatchEndian(fp, 12);
  if ( emt == UnKnown ) {
    ErrMsg(MsgERR, errMsg + string("invalid file: ") + mskPath);
    fclose(fp); return false;
  }
  else if ( emt == UnMatch ) ivConv = true;
  int ibuff[5];
  float* fbuff = (float*)ibuff;

  // read dims
  if ( fread(ibuff, 4, 5, fp) != 5 ) {fclose(fp); return false;}
  if ( ivConv ) BSWAPVEC(ibuff, 5);
  size_t mskDims[3];
  mskDims[0] = ibuff[1]; mskDims[1] = ibuff[2]; mskDims[2] = ibuff[3];
  size_t mskDimSz = mskDims[0] * mskDims[1] * mskDims[2];
  if ( mskDimSz < 1 ) {
    ErrMsg(MsgERR, errMsg + string("invalid dims of svx file: ") + mskPath);
    fclose(fp); return false;
  }

  // skip org, pitch
  if ( fread(ibuff, 4, 5, fp) != 5 ) {
    ErrMsg(MsgERR, errMsg + string("invalid file: ") + mskPath);
    fclose(fp); return false;
  }
  if ( fread(ibuff, 4, 5, fp) != 5 ) {
    ErrMsg(MsgERR, errMsg + string("invalid file: ") + mskPath);
    fclose(fp); return false;
  }

  // read type
  if ( fread(ibuff, 4, 3, fp) != 3 ) {
    ErrMsg(MsgERR, errMsg + string("invalid file: ") + mskPath);
    fclose(fp); return false;
  }
  if ( ivConv ) BSWAPVEC(ibuff, 3);
  int type = ibuff[1];
  if ( type == 0 ) type = VDT_All;
  if ( (mskType == MASK_VolRate && !(type & VDT_VolRate)) ||
       (mskType == MASK_Medium && !(type & VDT_VoxMedium)) ) {
    ErrMsg(MsgERR, errMsg + string("no VolRate | VoxMedium in svx") + mskPath);
    fclose(fp); return false;
  }

  // allocate
  size_t dimSz = m_dims[0] * m_dims[1] * m_dims[2];
  m_mask = (unsigned char*)CES::ReAllocate(m_mask, dimSz);
  if ( ! m_mask ) {
    ErrMsg(MsgERR, errMsg + string("can't allocate memory"));
    fclose(fp); return false;
  }
  memset(m_mask, 0, dimSz);
  CES::Vec3<int> ofstDims;
  if ( pOfst ) ofstDims = pOfst;


  // read VolRate
  if ( mskType == MASK_VolRate ) {
    fread(ibuff, 4, 1, fp);
    float* buff = new float[mskDims[0]];
    if ( ! buff ) {
      ErrMsg(MsgERR, errMsg + string("can't allocate memory"));
      delete [] buff;
      CES::DeAllocate(m_mask); m_mask = NULL;
      fclose(fp); return false;
    }

    for ( k = 0; k < mskDims[2]; k++ ) {
      for ( j = 0; j < mskDims[1]; j++ ) {
	if ( fread(buff, 4, mskDims[0], fp) != mskDims[0] ) {
	  ErrMsg(MsgERR, errMsg + string("can't read VolRate data"));
	  delete [] buff;
	  CES::DeAllocate(m_mask); m_mask = NULL;
	  fclose(fp); return false;
	}
	if ( k -ofstDims[2] < 0 || j -ofstDims[1] < 0 ) continue;
	if ( k -ofstDims[2] >= m_dims[2] || j -ofstDims[1] >= m_dims[1] )
	  continue;
	idx = m_dims[0]*m_dims[1]*(k-ofstDims[2]) + m_dims[0]*(j-ofstDims[1]);
	for ( i = 0; i < mskDims[0]; i++ ) {
	  if ( i -ofstDims[0] < 0 ) continue;
	  if ( i -ofstDims[0] >= m_dims[0] ) break;
	  if ( buff[i] >= vrRange[0] && buff[i] <= vrRange[1] )
	    m_mask[idx +i -ofstDims[0]] = 1;
	} // end of for(i)
      } // end of for(j)
    } // end of for(k)

    delete [] buff;
    goto _SUCCEED;
  }
  else if ( type & VDT_VolRate ) {
    if ( fseek(fp, (mskDimSz + 2)*4, SEEK_CUR) != 0 ) {
      CES::DeAllocate(m_mask); m_mask = NULL;
      fclose(fp); return false;
    }
  }

  // skip OpenRate
  if ( type & VDT_OpenRate ) {
    size_t dSz = (mskDims[0]+1) * mskDims[1] * mskDims[2];
    if ( fseek(fp, (dSz + 2)*4, SEEK_CUR) != 0 ) {
      CES::DeAllocate(m_mask); m_mask = NULL;
      fclose(fp); return false;
    }
    dSz = mskDims[0] * (mskDims[1]+1) * mskDims[2];
    if ( fseek(fp, (dSz + 2)*4, SEEK_CUR) != 0 ) {
      CES::DeAllocate(m_mask); m_mask = NULL;
      fclose(fp); return false;
    }
    dSz = mskDims[0] * mskDims[1] * (mskDims[2]+1);
    if ( fseek(fp, (dSz + 2)*4, SEEK_CUR) != 0 ) {
      CES::DeAllocate(m_mask); m_mask = NULL;
      fclose(fp); return false;
    }
  }

  // read VoxMedium
  if ( mskType == MASK_Medium ) {
    fread(ibuff, 4, 1, fp);
    int* buff = new int[mskDims[0]];
    if ( ! buff ) {
      ErrMsg(MsgERR, errMsg + string("can't allocate memory"));
      delete [] buff;
      CES::DeAllocate(m_mask); m_mask = NULL;
      fclose(fp); return false;
    }

    for ( k = 0; k < mskDims[2]; k++ ) {
      for ( j = 0; j < mskDims[1]; j++ ) {
	if ( fread(buff, 4, mskDims[0], fp) != mskDims[0] ) {
	  ErrMsg(MsgERR, errMsg + string("can't read VoxMedium data"));
	  delete [] buff;
	  CES::DeAllocate(m_mask); m_mask = NULL;
	  fclose(fp); return false;
	}
	if ( k -ofstDims[2] < 0 || j -ofstDims[1] < 0 ) continue;
	if ( k -ofstDims[2] >= m_dims[2] || j -ofstDims[1] >= m_dims[1] )
	  continue;
	idx = m_dims[0]*m_dims[1]*(k-ofstDims[2]) + m_dims[0]*(j-ofstDims[1]);
	for ( i = 0; i < mskDims[0]; i++ ) {
	  if ( i -ofstDims[0] < 0 ) continue;
	  if ( i -ofstDims[0] >= m_dims[0] ) break;
	  if ( buff[i] == medId )
	    m_mask[idx +i -ofstDims[0]] = 1;
	} // end of for(i)
      } // end of for(j)
    } // end of for(k)

    delete [] buff;
    goto _SUCCEED;
  }

  // something wrong ...
  CES::DeAllocate(m_mask); m_mask = NULL;
  fclose(fp); return false;

 _SUCCEED:
  fclose(fp);
  bool ret = true;
#if 0
  m_minMaxMask = chkMinMax;
  ret = checkMinMax(true, pApp->isChkProgress());
#endif
  size_t numMtds = getNumMethod();
  for ( i = 0; i < numMtds; i++ ) {
    vsnTimeSeriesMethodIF* pMtdTS
      = dynamic_cast<vsnTimeSeriesMethodIF*>(getMethod(i));
    if ( ! pMtdTS ) continue;
    if ( ! pMtdTS->updateStep(m_currentStepIdx) )
      ret = false;
  } // end of for(i)

  return ret;
}

bool vsnData_Sv::setMaskSBX(const std::string& mskPath, const MaskType mskType,
			    const float* vrRange, const int medId,
			    const bool chkMinMax, const int* pOfst) {
  if ( ! m_ready ) return false;
  vsnApp* pApp = vsnApp::GetApp();

  // select None or Medium
  register int idx, i, j, k;
  if ( mskType == MASK_None || mskType == MASK_Medium ) {
    bool ret = true;
    if ( m_mask ) {
      CES::DeAllocate(m_mask);
      m_mask = NULL;
#if 0
      if ( chkMinMax ) {
	m_minMaxMask = false;
	if ( ! checkMinMax(true, pApp->isChkProgress()) ) return false;
      }
#endif
      size_t numMtds = getNumMethod();
      for ( i = 0; i < numMtds; i++ ) {
        vsnTimeSeriesMethodIF* pMtdTS
          = dynamic_cast<vsnTimeSeriesMethodIF*>(getMethod(i));
        if ( ! pMtdTS ) continue;
        if ( ! pMtdTS->updateStep(m_currentStepIdx) )
          ret = false;
      } // end of for(i)
    }
    if ( m_maskWk ) {
      CES::DeAllocate(m_maskWk);
      m_maskWk = NULL;
    }
    return ret;
  }

  // load sbx file
  int buff[10];
  float* pfb = (float*)buff;
  double* pdb = (double*)buff;
  unsigned long long llbuf[3];
  unsigned long long* plb = llbuf;
  const unsigned int mgk = 0x584253;

  string errMsg = string("DataSv[") + getName() + string("]: setMaskSBX: ");
  FILE* fp = fopen(mskPath.c_str(), "rb");
  if ( ! fp ) {
    ErrMsg(MsgERR, errMsg + string("can't open file: ") + mskPath);
    return false;
  }
  EMatchType emt = MatchEndian(fp, mgk);
  if ( emt == UnKnown ) {fclose(fp); return false;}

  // read headers
  if ( fread(buff, 4, 8, fp) < 8 ) {fclose(fp); return false;}
  if ( emt == UnMatch ) BSWAPVEC(buff, 7);
  if ( fread(llbuf, 8, 1, fp) < 1 ) {fclose(fp); return false;}
  if ( emt == UnMatch ) LBSWAPVEC(llbuf, 1);

  int ndim = buff[1];
  if ( ndim < 1 || ndim > 3 ) {fclose(fp); return false;}
  SbxDataType sbxType;
  switch ( buff[3] ) {
  case 1: case 2: case 4:
    sbxType = (SbxDataType)buff[2]; break;
  default:
    fclose(fp); return false;
  }
  int ngc = buff[4];
  bool dblPrec = false;
  switch ( buff[5] ) {
  case 4: break; // single precision
  case 8: dblPrec = true; break; // double precision
  default:
    fclose(fp); return false;
  }
  SbxGridType gridType;
  switch ( buff[6] ) {
  case 1: case 2: case 3: case 4:
    gridType = (SbxGridType)buff[6]; break;
  default:
    fclose(fp); return false;
  }
  unsigned long long blkSz = plb[0];

  // read dims
  if ( fread(plb, 8, 3, fp) < 3 ) {fclose(fp); return false;}
  if ( emt == UnMatch ) BSWAPVEC(buff, 3);
  size_t mskDims[3];
  mskDims[0] = (size_t)plb[0];
  mskDims[1] = (size_t)plb[1];
  mskDims[2] = (size_t)plb[2];
  unsigned long long dimSz = mskDims[0] * mskDims[1] * mskDims[2];
  if ( dimSz < 1 ) {fclose(fp); return false;}

  // skip org, pitch
  if ( dblPrec ) {
    if ( fread(pdb, 8, 3, fp) < 3 ) {fclose(fp); return false;}
    if ( fread(pdb, 8, 3, fp) < 3 ) {fclose(fp); return false;}
  } else {
    if ( fread(pfb, 4, 3, fp) < 3 ) {fclose(fp); return false;}
    if ( fread(pfb, 4, 3, fp) < 3 ) {fclose(fp); return false;}
  }

  // allocate
  size_t sz = dimSz;
  switch ( sbxType ) {
  case SBX_SHORT:
    sz = sz * 2; break;
  case SBX_INT:
    sz = sz * 4; break;
  } // end of for(sdt)
  if ( sz < 1 ) return false;

  m_maskWk = CES::ReAllocate(m_maskWk, sz);
  if ( ! m_maskWk ) {
    ErrMsg(MsgERR, errMsg + string("can't allocate memory"));
    fclose(fp); return false;
  }
  memset(m_maskWk, 0, sz);
  m_mask = (unsigned char*)CES::ReAllocate(m_mask, dimSz);
  if ( ! m_mask ) {
    ErrMsg(MsgERR, errMsg + string("can't allocate memory"));
    fclose(fp); return false;
  }
  memset(m_mask, 0, dimSz);

  // offset
  CES::Vec3<int> ofstDims;
  if ( pOfst ) ofstDims = pOfst;

  // read data
  size_t elmSz = (size_t)sbxType;
  unsigned long long zblkSz;
  if ( blkSz == 0 ) {
    if( fread(&zblkSz, 8, 1, fp) != 1 ) {fclose(fp); return false;}

    size_t rdSz = fread(m_maskWk, elmSz, dimSz, fp);
    if ( rdSz != dimSz ) {fclose(fp); return false;}

    if( fread(&zblkSz, 8, 1, fp) != 1 ) {fclose(fp); return false;}
  }
  else {
    unsigned char* pd = (unsigned char*)m_maskWk;
    unsigned long long readSz = 0;
    unsigned long long dataSz = dimSz * elmSz;
    blkSz *= elmSz;
    unsigned char* zbuff = new unsigned char[blkSz];
    if ( ! zbuff ) {fclose(fp); return false;}

    while ( 1 ) {
      // read compsz
      if( fread(&zblkSz, 8, 1, fp) != 1 ) {
        delete [] zbuff; fclose(fp); return false;
      }
      if ( zblkSz < 1 ) break;

      // read data block
      if ( fread(zbuff, zblkSz, 1, fp) != 1 ) {
        delete [] zbuff; fclose(fp); return false;
      }

      // uncompress
      uLongf tmpBlkSz = (uLongf)blkSz;      
      if ( uncompress(&pd[readSz], &tmpBlkSz, zbuff, zblkSz) != Z_OK) {
        delete [] zbuff; fclose(fp); return false;
      }
      readSz += blkSz;
    } // end of while(1)

    delete [] zbuff;
    if ( readSz < dataSz ) {
      fclose(fp); return false;
    }
  }

  // endian conversion
  if ( emt == UnMatch ) {
    unsigned short* pSD = (unsigned short*)m_maskWk;
    unsigned int* pID = (unsigned int*)m_maskWk;
    switch ( sbxType ) {
    case SBX_BYTE: break;
    case SBX_SHORT: SBSWAPVEC(pSD, dimSz); break;
    case SBX_INT: BSWAPVEC(pID, dimSz); break;
    }
  }

  // set mask
  unsigned char* pd2 = (unsigned char*)m_maskWk;
  float vol;
  for ( k = 0; k < mskDims[2]; k++ ) {
    for ( j = 0; j < mskDims[1]; j++ ) {
      if ( k -ofstDims[2] < 0 || j -ofstDims[1] < 0 ) continue;
      if ( k -ofstDims[2] >= m_dims[2] || j -ofstDims[1] >= m_dims[1] )
        continue;
      idx = m_dims[0]*m_dims[1]*(k-ofstDims[2]) + m_dims[0]*(j-ofstDims[1]);
      for ( i = 0; i < mskDims[0]; i++ ) {
        if ( i -ofstDims[0] < 0 ) continue;
        if ( i -ofstDims[0] >= m_dims[0] ) break;

        if ( (int)pd2[idx+i] == 0 ) vol = 0.0f;
        else if ( (int)pd2[idx+i] == 1 ) vol = 0.5f;
        else if ( (int)pd2[idx+i] == 2 ) vol = 1.0f;
	else vol = -1.0f;
        if ( vol >= vrRange[0] && vol <= vrRange[1] )
          m_mask[idx +i -ofstDims[0]] = 1;
      } // end of for(i)
    } // end of for(j)
  } // end of for(k)

  fclose(fp);
  bool ret = true;
#if 0
  m_minMaxMask = chkMinMax;
  ret = checkMinMax(true, pApp->isChkProgress());
#endif
  size_t numMtds = getNumMethod();
  for ( i = 0; i < numMtds; i++ ) {
    vsnTimeSeriesMethodIF* pMtdTS
      = dynamic_cast<vsnTimeSeriesMethodIF*>(getMethod(i));
    if ( ! pMtdTS ) continue;
    if ( ! pMtdTS->updateStep(m_currentStepIdx) )
      ret = false;
  } // end of for(i)

  return ret;
}

bool vsnData_Sv::setMask(const bool apply,
			 const int x1, const int x2,
			 const int y1, const int y2,
			 const int z1, const int z2, const bool chkMinMax)
{
  if ( ! m_ready ) return false;
  vsnApp* pApp = vsnApp::GetApp();

  if ( x1 < 0 || x2 >= m_dims[0] || (x1 >= x2 && m_dims[0] > 1) ) return false;
  if ( y1 < 0 || y2 >= m_dims[1] || (y1 >= y2 && m_dims[1] > 1) ) return false;
  if ( z1 < 0 || z2 >= m_dims[2] || (z1 >= z2 && m_dims[2] > 1) ) return false;

  register int idx, i, j, k;
  if ( ! apply ) {
    bool ret = true;
    if ( m_mask ) {
      CES::DeAllocate(m_mask);
      m_mask = NULL;
#if 0
      if ( m_minMaxMask ) {
	m_minMaxMask = false;
	if ( ! checkMinMax(true, pApp->isChkProgress()) ) return false;
      }
#endif
      size_t numMtds = getNumMethod();
      for ( i = 0; i < numMtds; i++ ) {
        vsnTimeSeriesMethodIF* pMtdTS
          = dynamic_cast<vsnTimeSeriesMethodIF*>(getMethod(i));
	if ( ! pMtdTS ) continue;
	if ( ! pMtdTS->updateStep(m_currentStepIdx) )
	  ret = false;
      } // end of for(i)
    }
    return ret;
  }

  // allocate
  size_t dimSz = m_dims[0] * m_dims[1] * m_dims[2];
  m_mask = (unsigned char*)CES::ReAllocate(m_mask, dimSz);
  string errMsg = string("DataSv[") + getName() + string("]: setMask: ");
  if ( ! m_mask ) {
    ErrMsg(MsgERR, errMsg + string("can't allocate memory"));
    return false;
  }
  memset(m_mask, 0, dimSz);

  // m_mask
  idx = 0;
  for ( k = 0; k < m_dims[2]; k++ ) {
    for ( j = 0; j < m_dims[1]; j++ ) {
      for ( i = 0; i < m_dims[0]; i++ ) {
        bool bmask = false;
        if ( k < z1 || k > z2 ) bmask = true;
        if ( j < y1 || j > y2 ) bmask = true;
        if ( i < x1 || i > x2 ) bmask = true;
        if ( bmask ) m_mask[idx] = 1;
	idx++;
      } // end of for(i)
    } // end of for(j)
  } // end of for(k)

  // checkMinMax
  bool ret = true;
#if 0
  m_minMaxMask = chkMinMax;
  ret = checkMinMax(true, pApp->isChkProgress());
#endif
  size_t numMtds = getNumMethod();
  for ( i = 0; i < numMtds; i++ ) {
    vsnTimeSeriesMethodIF* pMtdTS
      = dynamic_cast<vsnTimeSeriesMethodIF*>(getMethod(i));
    if ( ! pMtdTS ) continue;
    if ( ! pMtdTS->updateStep(m_currentStepIdx) )
      ret = false;
  } // end of for(i)
  return ret;
}


/* from vsnTimeSeriesDataIF */

bool vsnData_Sv::getStepIdx(const int NSTEP, size_t& stp) const {
  if ( m_numStps < 1 || m_stpList.empty() )
    return false;
  if ( m_numStps < 2 || NSTEP <= m_stpList[0].step ) {
    stp = 0;
    return true;
  }
  if ( NSTEP >= m_stpList[m_numStps -1].step ) {
    stp = m_numStps -1;
    return true;
  }

  // find step
  register int i;
  for ( i = 0; i < m_numStps -1; i++ ) {
    if ( m_stpList[i].step <= NSTEP && NSTEP < m_stpList[i+1].step )
      break;
  } // end of for(i)
  stp = (size_t)i;

  return true;
}

bool vsnData_Sv::getTimeStepNo(const size_t stp, int& tsNo) const {
  if ( m_numStps < 1 || m_stpList.empty() ) return false;
  if ( stp >= m_numStps ) {
    tsNo = m_stpList[m_numStps -1].step;
    return false;
  }
  tsNo = m_stpList[stp].step;
  return true;
}

float vsnData_Sv::getTime(const size_t stp) const {
  if ( m_numStps < 1 || m_stpList.empty() ) return 0.f;
  if ( stp >= m_numStps ) return m_stpList[m_numStps -1].time;
  return m_stpList[stp].time;
}

bool vsnData_Sv::setCurrentStepIdx(const size_t stpIdx) {
  if ( ! vsnTimeSeriesDataIF::setCurrentStepIdx(stpIdx) )
    return false;

  // update bbox
  (void)updateBbox(m_currentStepIdx);

  // update all TimeSeriesMethods
  bool ret = true;
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


/* Bbox methods */

bool vsnData_Sv::updateBbox(const int stp) {
  if ( stp < 0 || stp >= m_numStps ) return false;
  _bbox[0] = CES::Vec3<float>(0.f, 0.f, 0.f);
  _bbox[1] = CES::Vec3<float>(m_dims[0]-1.f, m_dims[1]-1.f, m_dims[2]-1.f);
  notice();
  return true;
}


/* methods */

bool vsnData_Sv::getVectorMaxLen(const CES::Vec3<int>& vidx, float& vml) {
  if ( vsnNumericalDataIF::getVectorMaxLen(vidx, vml) )
    return true;

  if ( vidx.m_v[0] < 0 || vidx.m_v[0] >= m_dataLen ||
       vidx.m_v[1] < 0 || vidx.m_v[1] >= m_dataLen ||
       vidx.m_v[2] < 0 || vidx.m_v[2] >= m_dataLen )
    return false;

  register size_t i, j, idx, dimSz = m_dims[0]*m_dims[1]*m_dims[2];
  const float *pd;

  // data of the first step
  pd = getData(0);
  if ( ! pd ) return false;
  vml = CES::Vec3<float>(pd[vidx.m_v[0]],
			 pd[vidx.m_v[1]], pd[vidx.m_v[2]]).Length();
  for ( j = 1; j < dimSz; j++ ) {
    idx = m_dataLen * j;
    float vl = CES::Vec3<float>(pd[idx+vidx.m_v[0]], pd[idx+vidx.m_v[1]],
				pd[idx+vidx.m_v[2]]).Length();
    if ( vml < vl ) vml = vl;
  } // end of for(j)

  // follow steps
  for ( i = 1; i < m_numStps; i++ ) {
    pd = getData(i);
    if ( ! pd ) break;
    for ( j = 0; j < dimSz; j++ ) {
      idx = m_dataLen * j;
      float vl = CES::Vec3<float>(pd[idx+vidx.m_v[0]], pd[idx+vidx.m_v[1]],
				  pd[idx+vidx.m_v[2]]).Length();
      if ( vml < vl ) vml = vl;
    } // end of for(j)
  } // end of for(i)

  return true;
}

bool vsnData_Sv::checkMinMax(const bool wholeStp, const bool progress) {
  if ( m_hasMinMax && m_minVals.size() == m_dataLen )
    return true;

  register size_t i, j, k, idx, dimSz = m_dims[0]*m_dims[1]*m_dims[2];
  const float *pd;

  vsnDataCacheMF* pdcMF = dynamic_cast<vsnDataCacheMF*>(this);
  string errMsg = string("DataSv[") + getName() + string("]: ");

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

  // data of the first step
  pd = getData(0);
  if ( ! pd ) {
    if ( progDlg ) progDlg->Destroy();
    if ( pdcMF ) {
      errMsg += string("can't read the first file:\n  ");
      errMsg += BaseName(pdcMF->getCachePath(0),
			 string(""), vsnPath_getDelimChar());
      errMsg += string(" : ") + m_readinErrMsg;
    } else {
      errMsg += string("can't load the first step data : ");
      errMsg += m_readinErrMsg;
    }
    ErrMsg(MsgERR, errMsg);
    return false;
  }

  m_minVals.resize(m_dataLen);
  m_maxVals.resize(m_dataLen);
#if 0
  if ( m_mask && m_minMaxMask ) {
    for ( j = 0; j < dimSz; j++ ) {
      if ( m_mask[j] ) continue;
      break;
    } // end of for(j)
    if ( j == dimSz ) {
      if ( progDlg ) progDlg->Destroy();
      for ( k = 0; k < m_dataLen; k++ )
	m_minVals[k] = m_maxVals[k] = 0.f;
      m_maxVecLen012 = 0.f;
      return true;
    }
    for ( k = 0; k < m_dataLen; k++ ) {
      idx = m_dataLen * j + k;
      m_minVals[k] = pd[idx];
      m_maxVals[k] = pd[idx];
    } // end of for(k)
    j ++;
  } else {
#endif
    for ( k = 0; k < m_dataLen; k++ )
      m_minVals[k] = m_maxVals[k] = pd[k];
    j = 1;
#if 0
  }
#endif
  if ( m_dataLen >= 3 )
    m_maxVecLen012 = CES::Vec3<float>(pd).Length();

  for ( ; j < dimSz; j++ ) {
#if 0
    if ( m_mask && m_minMaxMask && m_mask[j] ) continue;
#endif
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
    return true;
  }

  // follow steps
  bool badFileExists = false;
  if ( pdcMF )
    errMsg += string("can't read the file(s) below:\n");
  else
    errMsg += string("can't load data of the step(s) below:\n");
  for ( i = 1; i < m_numStps; i++ ) {
    sprintf(msgBuff, "Data_Sv: checking min/max of file #%lu of %lu",
		  i+1, m_numStps);
    prgMsg = vsnApp::ConvSysToWx(msgBuff);
    if ( progDlg && ! progDlg->Update(i, prgMsg) ) break;

    pd = getData(i);
    if ( ! pd ) {
      char txt[64]; sprintf(txt, "  step#%lu ", i);
      errMsg += txt;
      if ( pdcMF )
        errMsg += BaseName(pdcMF->getCachePath(i),
			   string(""), vsnPath_getDelimChar());
      errMsg += string(" : ") + m_readinErrMsg + string("\n");
      badFileExists = true;
      continue;
    }
    for ( j = 0; j < dimSz; j++ ) {
#if 0
      if ( m_mask && m_minMaxMask && m_mask[j] ) continue;
#endif
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
  return true;
}

const float* const vsnData_Sv::getCoord(const size_t stp) {
  if ( ! updateBbox(stp) ) return NULL;

  register size_t dimSz = m_dims[0]*m_dims[1]*m_dims[2];
  if ( dimSz < 1 ) return NULL;
  if ( dimSz != nVerts ) {
    if ( ! alcVerts(dimSz) ) return NULL;
    register int i, j, k, idx = 0;
    for ( k = 0; k < m_dims.m_v[2]; k++ )
      for ( j = 0; j < m_dims.m_v[1]; j++ )
	for ( i = 0; i < m_dims.m_v[0]; i++ ) {
	  _verts[idx][0] = (float)i;
	  _verts[idx][1] = (float)j;
	  _verts[idx][2] = (float)k;
	  idx ++;
	}
  }
  return (float*)_verts;
}
