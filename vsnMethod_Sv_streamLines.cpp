//
// vsnMethod_Sv_streamLines
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

#include "vsnMethod_Sv_streamLines.h"
#include "vsnOctTree.h" // for decomp vecIdx
#include "vsnError.h"

using namespace std;
using namespace CES;
using namespace VSN;


//----------------------------------------------------------------
// class vsnMethod_Sv_streamLines
//----------------------------------------------------------------

/* constructors / destructor */

vsnMethod_Sv_streamLines::vsnMethod_Sv_streamLines(const string& name)
  : vsnMethod_streamLines(name)
{
  _bound[0] = _bound[1] = _bound[2] = false;
  _x0 = NULL;
  _array = NULL;
}

vsnMethod_Sv_streamLines::~vsnMethod_Sv_streamLines() {
  if ( _x0 ) DeAllocate(_x0);
  if ( _array ) delete [] _array;
}


/* vsnTimeSeriesMethodIF method */

bool vsnMethod_Sv_streamLines::updateStep(const int stp,
					  const bool force, const bool cascade)
{
  vsnData_Sv* pData = dynamic_cast<vsnData_Sv*>(p_refData);
  if ( ! pData ) return false;
  m_requestedStp = stp;
  if ( m_requestedStp < 0 )
    m_requestedStp = pData->getCurrentStepIdx();

  // check needs
  //  don't calculate when time-step changed if !force ...
  if ( m_always ) m_needUpd = true;
  if ( ! m_needUpd && 
       (m_updatedStp == m_requestedStp || ! force) ) return true;
  m_updatedStp = -1;
  m_needUpd = false;

  // check vecData
  if ( ! isValidVecData() ) return false;

  // lines
  if ( ! m_lines ) {
    m_lines = new vsnPtSet();
    if ( ! m_lines ) {
      ErrMsg(MsgERR, getMethodType() + string("[") + getName()
             + string("]: memory allocation failed"));
      return false;
    }
    m_lines->alcMaterial();
    addChild(m_lines);
  }
  m_lines->getPrivateMaterial()->setRenderMode(RT_NONE);
  if ( ! m_show ) {
    return true;
  }

  register int i, l;

  // check sampler
  if ( ! p_splr ) return true;
  Point2 sampleSize = p_splr->getSampleNumber();
  size_t sampleSz = sampleSize.x * sampleSize.y;
  if ( sampleSz < 1 ) return true;
  const vector3* const samplePts = p_splr->getSamplePoints();
  if ( ! samplePts ) {
    ErrMsg(MsgERR, getMethodType() + string("[") + getName()
           + string("]: can't get sample points data"));
    return false;
  }

  // alloc _x0
  _x0 = (DVec3*)ReAllocate(_x0, sizeof(DVec3)*sampleSz);
  if ( ! _x0 ) {
    ErrMsg(MsgERR, getMethodType() + string("[") + getName()
	   + string("]: memory allocation failed"));
    return false;
  }
  for ( i = 0; i < sampleSz; i++ ) {
    _x0[i][0] = (double)samplePts[i][0];
    _x0[i][1] = (double)samplePts[i][1];
    _x0[i][2] = (double)samplePts[i][2];
  }

  // alloc _array
  if ( _array ) delete [] _array;
  _array = new PTarray[sampleSz];
  if ( ! _array ) {
    ErrMsg(MsgERR, getMethodType() + string("[") + getName()
	   + string("]: memory allocation failed"));
    return false;
  }
  for ( i = 0; i < sampleSz; i++ ) {
    _array[i].x = (vector3*)Allocate(sizeof(vector3)*m_maxPts);
    memset(_array[i].x, 0, sizeof(vector3)*m_maxPts);
  }

  // get Sv data
  register size_t dlen = pData->getDataLen();
  float dr[2];
  std::deque<float> minVals;
  for ( i = 0; i < dlen; i++ ) {
    pData->getMinMax(i, dr);
    minVals.push_back(dr[0]);
  } // end of for(i)
  if ( ! _gus.setup(pData->getDims(), dlen,
		    (float*)pData->getCoord(m_requestedStp),
		    (float*)pData->getData(m_requestedStp), -1,
		    (unsigned char*)pData->getMask(), &minVals) ) {
    return false;
  }
  register size_t dimSz = _gus.m_dims[0]*_gus.m_dims[1]*_gus.m_dims[2];

  // get bbox from Sv data
  const Vec3<float>* pbb = pData->getBbox();

  // calculate streamlines
  DVec4 x_i;
  register size_t num_pts = 0;
  for ( l = 0; l < sampleSz; l++ ) {
    // pre-check using bbox
    if ( ! _gus.is2D() &&
	 (_x0[l][0] < pbb[0].m_v[0] || _x0[l][0] > pbb[1].m_v[0] ||
	  _x0[l][1] < pbb[0].m_v[1] || _x0[l][1] > pbb[1].m_v[1] ||
	  _x0[l][2] < pbb[0].m_v[2] || _x0[l][2] > pbb[1].m_v[2]) )
      continue;

    if ( _gus.ConvXtoI(_x0[l], x_i) < 0 ) continue;

    _gus.InterpolateCoord(x_i, _array[l].x[0]);
    _array[l].cnt++;
      
    x_i[3] = 0.0;
    StreamLine(x_i, l);
    num_pts += _array[l].cnt;
  } // end of for(l)

  // alloc lines data
  if ( ! m_lines->alcVerts(num_pts) ||
       ! m_lines->alcIndices(sampleSz) ) {
     ErrMsg(MsgERR, getMethodType() + string("[") + getName()
	   + string("]: memory allocation failed"));
    return false;
  }
  if ( m_colored ) {
    if ( ! m_lines->alcColors(sampleSz) ) {
      ErrMsg(MsgERR, getMethodType() + string("[") + getName()
	     + string("]: memory allocation failed"));
      return false;
    }
    m_lines->setColorMode(AT_PER_FACE);
  } else {
    m_lines->setColorMode(AT_WHOLE);
    m_lines->setColor4(0, m_colour);
  }

  // set lines
  vector3* vl = m_lines->getVerts();
  int* il = m_lines->getIndices();

  register int idx = 0;
  for ( l = 0; l < sampleSz; l++ ) {
    // set vertex
    for ( i = 0; i < _array[l].cnt; i++ ) {
      vl[idx][0] = _array[l].x[i][0];
      vl[idx][1] = _array[l].x[i][1];
      vl[idx][2] = _array[l].x[i][2];
      idx++;
    } // end of for(i)

    // indices
    il[l] = _array[l].cnt;
  } // end of for(l)

  // colors
  if ( m_colored ) {
    vfrDatamap lrgb((float)(sampleSize.x -1), 0.f);
    if ( sampleSize.x == 1 ) lrgb.setHue(0, 0.f);
    vector4* cl = m_lines->getColors();
    vector4 wkc;
    for ( l = 0; l < sampleSz; l++ ) {
      lrgb.datamap((float)(l % sampleSize.x), wkc);
      cl[l][0] = wkc[0]; cl[l][1] = wkc[1]; cl[l][2] = wkc[2];
      cl[l][3] = 1.f;
    } // end of for(l)
  } // end of if(m_colored)

  m_lines->getPrivateMaterial()->setLineWidth(m_lineWidth);
  m_lines->setTransparency(m_antiAlias);
  m_lines->generateBbox();

  // ok
  m_lines->getPrivateMaterial()->setRenderMode(RT_WIRE);
  m_updatedStp = m_requestedStp;

  if ( m_autoExport && ! m_exportPath.empty() ) {
    if ( ! exportLines(m_exportPath) )
      setAutoExport(false);
  }

  return true;
}


/* internal method for make stream lines */

double vsnMethod_Sv_streamLines::GetIntegrand(const DVec4 x_i, DVec3 func) {
  double t_step, max = 0.0;

  DMat3 m; DVec3 v;
  _gus.InterpolateData(x_i, m_vecDataIdx, v);

  double J = _gus.MatrixInverse(x_i, m);
  if ( ! J ) return 0.0;

  register int n;
  for ( n = 0; n < 3; n++ ) {
    func[n] = m[n][0]*v[0] + m[n][1]*v[1] + m[n][2]*v[2];
    if ( fabs(func[n]) > max ) max = fabs(func[n]);
  }

  if ( ! max )
    t_step = 0.0; // in this case, particle of Stream Line never move.
  else
    t_step = 1.0 / max / m_divTime;

  if ( t_step > 1.0 )
    t_step = 1.0; // avoid a stagnation point.

  return t_step;
}

int vsnMethod_Sv_streamLines::RKG(const double t_step, DVec4 x_i) {
  DVec3 q = {0.0, 0.0, 0.0};
  static DVec4 ct = {0.0, 0.5, 0.0, 0.5};
  static DVec4 ck = {2.0, 1.0, 1.0, 2.0};
  static DVec4 cx, cq;
  cq[0] = 0.5; cq[1] = 1.0 - sqrt(0.5); cq[2] = 1.0 + sqrt(0.5); cq[3] = 0.5;
  cx[0] = cq[0]; cx[1] = cq[1]; cx[2] = cq[2]; cx[3] = cq[3]/3 ;

  double eps = 1.0e-8;
  DVec3 i_pre = {x_i[0], x_i[1], x_i[2]};

  const int t = int(x_i[3]);

  register int l, n;
  for ( l = 0; l < 4; l++ ) {
    // get right-hand part of equation
    DVec3 func;
    if ( ! GetIntegrand(x_i, func) )
      return -1; // it is caused by J = 0 or all func = 0.

    for ( n = 0; n < 3; n++ ) {
      int i_n = int(x_i[n]);

      double k = t_step * func[n];
      double r = cx[l] * (k - ck[l] * q[n]); 
      x_i[n] += r;
      q[n] += 3.0 * r - cq[l] * k;

      int sign = int(floor(x_i[n])) - i_n;
      if ( x_i[n] > double(_gus.m_dims[n] - 1) ) {
	// for the case limit to overlimit
	sign = 1; i_n = _gus.m_dims[n] - 2;
      }
      if ( sign ) {
	if ( x_i[n] < 0.0 || x_i[n] > double(_gus.m_dims[n] - 1) ) {
	  if ( _bound[n] ) {
	    // the particle is saved by periodic boundary.
	    if ( x_i[n] < 0.0 )
	      x_i[n] += double(_gus.m_dims[n] - 1);
	    else if ( x_i[n] > double(_gus.m_dims[n] - 1) )
	      x_i[n] -= double(_gus.m_dims[n] - 1);
	  }
	  else
	    return -1; // if it's true, the particle is gone.
	}
      } // end of if(sign)
    } // end of for(n)
  } // end of for(l)

  if ( fabs(x_i[0] - i_pre[0]) < eps &&
       fabs(x_i[1] - i_pre[1]) < eps && fabs(x_i[2] - i_pre[2]) < eps )
    return -2; // drop into a stagnation point.

  return 1;
}

int vsnMethod_Sv_streamLines::StreamLine(DVec4 x_i, const int l) {
  register int it, end = (m_maxPts - 1) * m_numSkip;
  for ( it = 0; it < end; it++ ) {
    DVec3 func;
    double t_step = GetIntegrand(x_i, func);
    if ( ! t_step ) return 0;

    if ( RKG(t_step, x_i) < 0 )
      return it; // if return value is negative, the particle is out of range.

    if ( ! ((it+1)%m_numSkip) ) {
      _gus.InterpolateCoord(x_i, _array[l].x[_array[l].cnt]);
      _array[l].cnt++;
    }
  }
  return it;
}
