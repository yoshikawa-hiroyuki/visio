//
// vsnMethod_Sv_graphPlot
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

#include "vsnMethod_Sv_graphPlot.h"
#include "vsnError.h"

using namespace std;
using namespace CES;
using namespace VSN;


//----------------------------------------------------------------
// class vsnMethod_Sv_graphPlot
//----------------------------------------------------------------

/* constructors / destructor */

vsnMethod_Sv_graphPlot::vsnMethod_Sv_graphPlot(const string& name)
  : vsnMethod_graphPlot(name)
{
  _values = NULL;
}

vsnMethod_Sv_graphPlot::~vsnMethod_Sv_graphPlot() {
  if ( _values )
    DeAllocate(_values);
}


/* from vsnMethod_graphPlot */

bool vsnMethod_Sv_graphPlot::exportCsv(const std::string& path) {
  if ( path.empty() ) return false;
  if ( m_updatedStp < 0 ) return false;
  if ( ! _values ) return false;

  // check 1D/2D data
  register int i, j;
  size_t validDims = 3;
  for ( j = 0, i = 0; i < 3; i++ ) if ( _gus.m_dims[i] == 1 ) j++;
  switch ( j ) {
  case 1: validDims = 2; break;
  case 2: case 3: validDims = 1; break;
  default: break;
  } // end of switch(j)
  
  // check sampler
  Point2 sampleSize, dim2Idx;
  register size_t sampleSz;
  const vector3* samplePts;
  if ( validDims == 1 ) { // 1D
    for ( i = 0; i < 3; i++ ) if ( _gus.m_dims[i] != 1 ) break;
    switch ( i ) {
    case 0:  sampleSize.x = _gus.m_dims[0]; break; // MxN is Xx1
    case 1:  sampleSize.x = _gus.m_dims[1]; break; // MxN is Yx1
    default: sampleSize.x = _gus.m_dims[2]; break; // MxN is Zx1
    } // end of switch(i)
    sampleSize.y = 1; sampleSz = sampleSize.x;
    samplePts = (const vector3*)_gus.p_grid;
  } // end of if(1D)
  else if ( validDims == 2 ) { // 2D
    for ( i = 0; i < 3; i++ ) if ( _gus.m_dims[i] == 1 ) break;
    switch ( i ) {
    case 0:  dim2Idx.x = 1; dim2Idx.y = 2; break; // MxN is YxZ
    case 1:  dim2Idx.x = 0; dim2Idx.y = 2; break; // MxN is XxZ
    default: dim2Idx.x = 0; dim2Idx.y = 1; break; // MxN is XxY
    } // end of switch(i)
    sampleSize.x = _gus.m_dims[dim2Idx.x];
    sampleSize.y = _gus.m_dims[dim2Idx.y];
    sampleSz = sampleSize.x * sampleSize.y;
    samplePts = (const vector3*)_gus.p_grid;
  } // end of if(2D)
  else { // 3D
    if ( ! p_splr ) return true;
    sampleSize = p_splr->getSampleNumber();
    sampleSz = sampleSize.x * sampleSize.y;
    if ( sampleSz < 1 ) return true;
    samplePts = p_splr->getSamplePoints();
    if ( ! samplePts ) {
      ErrMsg(MsgERR, getMethodType() + string("[") + getName()
	     + string("]: can't get sampling points data"));
      return false;
    }
  } // end of if(3D)

  if ( ! ExportCsv(path, sampleSize, samplePts, _values) )
    return false;

  // ok
  return true;
}


/* vsnMethodObj methods */

bool vsnMethod_Sv_graphPlot::update(const bool force) {
  return updateStep(m_requestedStp, force);
}


/* vsnTimeSeriesMethodIF methods */

bool vsnMethod_Sv_graphPlot::updateStep(const int stp,
					const bool force, const bool cascade)
{
  vsnData_Sv* pData = dynamic_cast<vsnData_Sv*>(p_refData);
  if ( ! pData ) return false;

  m_requestedStp = stp;
  if ( m_requestedStp < 0 )
    m_requestedStp = pData->getCurrentStepIdx();

  if ( force ) m_updatedStp = -1;
  if ( m_updatedStp == m_requestedStp ) return true;
  m_updatedStp = -1;

  // gnuplot interface
  if ( ! m_pGnuPlotIF ) return false;
  if ( ! m_pGnuPlotIF->IsConnected() ) {
    ErrMsg(MsgWARN, getMethodType() + string("[") + getName()
	   + string("]: can't connect to Gnuplot, check program path"));
    return true;
  }

  // get Sv datas
  (void)_gus.setup(pData->getDims(), pData->getDataLen(),
		   (float*)pData->getCoord(m_requestedStp),
		   (float*)pData->getData(m_requestedStp));
  size_t dimSz = _gus.m_dims[0] * _gus.m_dims[1] * _gus.m_dims[2];
  if ( dimSz < 1 || _gus.m_vecLen < 1 ) return true;
  if ( ! _gus.p_grid ) {
    ErrMsg(MsgERR, getMethodType() + string("[") + getName()
             + string("]: can't get grid data"));
    return false;
  }
  const Vec3<float>* pbb = pData->getBbox();

  // check selected data
  float minmax[2] = {0.f, 1.f};
  if ( m_selectedData == DATA_Veclen ) {
    if ( ! isValidVecData() ) return true;
    pData->getVectorMaxLen(m_vecDataIdx, minmax[1]);
  }
  else if ( m_selectedData > 0 && m_selectedData <= _gus.m_vecLen ) {
    pData->getMinMax(m_selectedData -1, minmax);
  }
  else return true;

  // check 1D/2D data
  register int i, j;
  size_t validDims = 3;
  for ( j = 0, i = 0; i < 3; i++ ) if ( _gus.m_dims[i] == 1 ) j++;
  switch ( j ) {
  case 1: validDims = 2; break;
  case 2: case 3: validDims = 1; break;
  default: break;
  } // end of switch(j)
  
  // check sampler
  Point2 sampleSize, dim2Idx;
  register size_t sampleSz;
  const vector3* samplePts;
  if ( validDims == 1 ) { // 1D
    for ( i = 0; i < 3; i++ ) if ( _gus.m_dims[i] != 1 ) break;
    switch ( i ) {
    case 0:  sampleSize.x = _gus.m_dims[0]; break; // MxN is Xx1
    case 1:  sampleSize.x = _gus.m_dims[1]; break; // MxN is Yx1
    default: sampleSize.x = _gus.m_dims[2]; break; // MxN is Zx1
    } // end of switch(i)
    sampleSize.y = 1; sampleSz = sampleSize.x;
    samplePts = (const vector3*)_gus.p_grid;
  } // end of if(1D)
  else if ( validDims == 2 ) { // 2D
    for ( i = 0; i < 3; i++ ) if ( _gus.m_dims[i] == 1 ) break;
    switch ( i ) {
    case 0:  dim2Idx.x = 1; dim2Idx.y = 2; break; // MxN is YxZ
    case 1:  dim2Idx.x = 0; dim2Idx.y = 2; break; // MxN is XxZ
    default: dim2Idx.x = 0; dim2Idx.y = 1; break; // MxN is XxY
    } // end of switch(i)
    sampleSize.x = _gus.m_dims[dim2Idx.x];
    sampleSize.y = _gus.m_dims[dim2Idx.y];
    sampleSz = sampleSize.x * sampleSize.y;
    samplePts = (const vector3*)_gus.p_grid;
  } // end of if(2D)
  else { // 3D
    if ( ! p_splr ) return true;
    sampleSize = p_splr->getSampleNumber();
    sampleSz = sampleSize.x * sampleSize.y;
    if ( sampleSz < 1 ) return true;
    samplePts = p_splr->getSamplePoints();
    if ( ! samplePts ) {
      ErrMsg(MsgERR, getMethodType() + string("[") + getName()
	     + string("]: can't get sampling points data"));
      return false;
    }
  } // end of if(3D)

  // allocate data-values buff
  _values = (float*)ReAllocate(_values, sizeof(float)*sampleSz);
  if ( ! _values ) {
    ErrMsg(MsgERR, getMethodType() + string("[") + getName()
	   + string("]: memory allocation failed"));
    return false;
  }

  // do sampling
  Vec3<float> pos, val;
  register int idx;
  if ( validDims == 1 || validDims == 2 ) { // 1D/2D
    if ( m_selectedData == DATA_Veclen ) {
      for ( i = 0; i < sampleSz; i++ ) {
	val = Vec3<float>(&_gus.p_vecd[_gus.m_vecLen * i]);
	_values[i] = val.Length();
      } // end of for(i)
    }
    else if ( m_selectedData > 0 && m_selectedData <= _gus.m_vecLen ) {
      for ( i = 0; i < sampleSz; i++ )
	_values[i] = _gus.p_vecd[_gus.m_vecLen * i + m_selectedData - 1];
    }
    else
      memset(_values, 0, sizeof(float)*sampleSz);
  } // end of if(2D/3D)
  else { // 3D
    DVec3 xp; DVec4 x_i;
    idx = 0;
    if ( m_selectedData == DATA_Veclen ) {
      for ( j = 0; j < sampleSize.y; j++ ) {
	for ( i = 0; i < sampleSize.x; i++, idx++ ) {
	  pos = samplePts[idx];
	  xp[0]=(double)pos[0]; xp[1]=(double)pos[1]; xp[2]=(double)pos[2];
	  // pre-check using bbox
	  if ( pos.m_v[0] < pbb[0].m_v[0] || pos.m_v[0] > pbb[1].m_v[0] ||
	       pos.m_v[1] < pbb[0].m_v[1] || pos.m_v[1] > pbb[1].m_v[1] ||
	       pos.m_v[2] < pbb[0].m_v[2] || pos.m_v[2] > pbb[1].m_v[2] ) {
	    _values[idx] = minmax[0]; continue;
	  }
	  // get contravariant components
	  if ( _gus.ConvXtoI(xp, x_i) < 0 ) {
	    _values[idx] = minmax[0]; continue;
	  }
	  // interpolate data
	  _gus.InterpolateData(x_i, m_vecDataIdx, val.m_v);
	  _values[idx] = val.Length();
	} // end of for(i)
      } // end of for(j)
    } // end of if(DATA_Veclen)
    else if ( m_selectedData > 0 && m_selectedData <= _gus.m_vecLen ) {
      for ( j = 0; j < sampleSize.y; j++ ) {
	for ( i = 0; i < sampleSize.x; i++, idx++ ) {
	  pos = samplePts[idx];
	  xp[0]=(double)pos[0]; xp[1]=(double)pos[1]; xp[2]=(double)pos[2];
	  // pre-check using bbox
	  if ( pos.m_v[0] < pbb[0].m_v[0] || pos.m_v[0] > pbb[1].m_v[0] ||
	       pos.m_v[1] < pbb[0].m_v[1] || pos.m_v[1] > pbb[1].m_v[1] ||
	       pos.m_v[2] < pbb[0].m_v[2] || pos.m_v[2] > pbb[1].m_v[2] ) {
	    _values[idx] = minmax[0]; continue;
	  }
	  // get contravariant components
	  if ( _gus.ConvXtoI(xp, x_i) < 0 ) {
	    _values[idx] = minmax[0]; continue;
	  }
	  // interpolate data
	  _gus.InterpolateData(x_i, m_selectedData -1, val.m_v[0]);
	  _values[idx] = val.m_v[0];
	} // end of for(i)
      } // end of for(j)
    } // end of if(DATA_n)
    else
      memset(_values, 0, sizeof(float)*sampleSz);
  } // end of if(3D)

  // send commands to gnuplot
  char txt[64];
  string titleStr;
  if ( m_showTitle ) {
    if ( m_selectedData == DATA_Veclen )
      titleStr += string("vector length : ");
    else {
      sprintf(txt, "data%d : ", m_selectedData -1);
      titleStr += txt;
    }
    if ( p_splr ) {
      titleStr += string("sampler[") + p_splr->getName() + string("], ");
    }
    titleStr += string("Data[") + pData->getName() + string("]");
  } // end of if(m_showTitle)

  if ( ! DoPlot(sampleSize, samplePts, _values, minmax, titleStr) )
    return false;

  // ok
  m_updatedStp = m_requestedStp;
  return true;
}
