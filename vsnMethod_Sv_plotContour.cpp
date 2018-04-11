//
// vsnMethod_Sv_plotContour
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

#include "vsnMethod_Sv_plotContour.h"
#include "vsnError.h"

using namespace std;
using namespace CES;
using namespace VSN;


//----------------------------------------------------------------
// class vsnMethod_Sv_plotContour
//----------------------------------------------------------------

/* constructors / destructor */

vsnMethod_Sv_plotContour::vsnMethod_Sv_plotContour(const std::string& name)
  : vsnMethod_plotContour(name)
{
}

vsnMethod_Sv_plotContour::~vsnMethod_Sv_plotContour() {
}


/* vsnTimeSeriesMethodIF methods */

#define IDX(i, j) (sampleSize.x*(j)+(i))


bool vsnMethod_Sv_plotContour::updateStep(const int stp,
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

  // lines data
  register int i, j, k, idx;
  int n = getNumChildren();
  if ( m_numLines > n ) {
    for ( i = n; i < m_numLines; i++ ) {
      vfrLines* plns = new vfrLines("contour_line");
      if ( ! plns ) {
        ErrMsg(MsgERR, getMethodType() + string("[") + getName()
               + string("]: memory allocation failed"));
        return false;
      }
      addChild(plns);
    } // end of for(i)
  }

  // invalidate all lines
  m_numValidLines = 0;
  for ( i = 0; i < n; i++ ) {
    vfrNode* p = getChild(i);
    if ( ! p ) continue;
    p->alcVerts(0);
    p->notice();
  } // end of for(i)

  getPrivateMaterial()->setRenderMode(RT_NONE);
  if ( ! m_show ) {
    return true;
  }

  // check selected data
  size_t dlen = pData->getDataLen();
  if ( m_selectedData == DATA_None ) return true;
  else if ( m_selectedData == DATA_Veclen ) {
    if ( ! isValidVecData() ) return true;
  }
  else if ( m_selectedData > dlen ) return true;

  // check sampler
  if ( ! p_splr ) return true;
  Point2 sampleSize = p_splr->getSampleNumber();
  size_t sampleSz = sampleSize.x * sampleSize.y;
  if ( sampleSz < 2 ) return true;
  const vector3* const samplePts = p_splr->getSamplePoints();
  if ( ! samplePts ) {
    ErrMsg(MsgERR, getMethodType() + string("[") + getName()
           + string("]: can't get sampling points data"));
    return false;
  }

  // alloc lines' data
  //   needs M * N * 2(trias) * 2(verts) vertices for one contour
  size_t maxLineSegs = (sampleSize.x -1) * (sampleSize.y -1) * 2;
  for ( i = 0; i < m_numLines; i++ ) {
    vfrNode* p = getChild(i);
    if ( ! p->alcPools(maxLineSegs * 2, -1, -1, -1) ) {
      ErrMsg(MsgERR, getMethodType() + string("[") + getName()
             + string("]: memory allocation failed"));
      return false;
    }
  } // end of for(i)

  // get GridUtil of Sv data
  if ( ! _gus.setup(pData->getDims(), pData->getDataLen(),
		    (float*)pData->getCoord(m_requestedStp),
		    (float*)pData->getData(m_requestedStp)) ) {
    return false;
  }

  // get bbox
  const Vec3<float>* pbb = pData->getBbox();

  // get datalist on samplePts
  float* sampleData = NULL;
  if ( m_selectedData == DATA_Veclen )
    sampleData = (float*)Allocate(sizeof(vector3)*sampleSz);
  else
    sampleData = (float*)Allocate(sizeof(float)*sampleSz);
  if ( ! sampleData ) {
    ErrMsg(MsgERR, getMethodType() + string("[") + getName()
	   + string("]: memory allocation failed"));
    return false;
  }
  unsigned char* sampleFlgs
    = (unsigned char*)Allocate(sizeof(unsigned char)*sampleSz);
  if ( ! sampleFlgs ) {
    ErrMsg(MsgERR, getMethodType() + string("[") + getName()
           + string("]: memory allocation failed"));
    DeAllocate(sampleData);
    return false;
  }
  memset(sampleFlgs, 0, sizeof(unsigned char)*sampleSz);

  DVec3 xp; DVec4 x_i;
  if ( m_selectedData == DATA_Veclen ) {
    Vec3<float> dval;
    for ( i = 0; i < sampleSz; i++ ) {
      if ( ! _gus.is2D() &&
	   (samplePts[i][0] < pbb[0].m_v[0] ||
	    samplePts[i][0] > pbb[1].m_v[0] ||
	    samplePts[i][1] < pbb[0].m_v[1] ||
	    samplePts[i][1] > pbb[1].m_v[1] ||
	    samplePts[i][2] < pbb[0].m_v[2] ||
	    samplePts[i][2] > pbb[1].m_v[2]) ) continue;
      xp[0] = (double)samplePts[i][0];
      xp[1] = (double)samplePts[i][1];
      xp[2] = (double)samplePts[i][2];
      if ( _gus.ConvXtoI(xp, x_i) >= 0 &&
           _gus.InterpolateData(x_i, m_vecDataIdx, dval.m_v) ) {
	memcpy(&sampleData[i*3], dval.m_v, sizeof(vector3));
	sampleFlgs[i] = 1;
      }
    } // end of for(i)
  } else {
    float dval;
    for ( i = 0; i < sampleSz; i++ ) {
      if ( ! _gus.is2D() &&
	   (samplePts[i][0] < pbb[0].m_v[0] ||
	    samplePts[i][0] > pbb[1].m_v[0] ||
	    samplePts[i][1] < pbb[0].m_v[1] ||
	    samplePts[i][1] > pbb[1].m_v[1] ||
	    samplePts[i][2] < pbb[0].m_v[2] ||
	    samplePts[i][2] > pbb[1].m_v[2]) ) continue;
      xp[0] = (double)samplePts[i][0];
      xp[1] = (double)samplePts[i][1];
      xp[2] = (double)samplePts[i][2];
      if ( _gus.ConvXtoI(xp, x_i) >= 0 &&
           _gus.InterpolateData(x_i, (size_t)m_selectedData-1, dval) ) {
	sampleData[i] = dval;
	sampleFlgs[i] = 1;
      }
    } // end of for(i)
  }

  // calc lines
  vector3 lp1, lp2;
  register float val;
  register float dv = (m_numLines < 2) ? 0.f
    : (m_max - m_min)/(float)(m_numLines - 1);

  for ( val = m_min, k = 0; k < m_numLines; val += dv, k++ ) {
    vfrLines* plns = dynamic_cast<vfrLines*>(getChild(k));
    if ( ! plns ) continue;
    
    idx = 0;
    if ( m_selectedData == DATA_Veclen ) {
      for ( j = 0; j < sampleSize.y -1; j++ ) {
        for ( i = 0; i < sampleSize.x -1; i++ ) {
	  vector3 vd;

	  if ( ! sampleFlgs[IDX(i,j)] || ! sampleFlgs[IDX(i+1,j+1)] )
	    continue;
	  vd[0] = Vec3<float>(&sampleData[IDX(i,j)*3]).Length();
	  vd[1] = Vec3<float>(&sampleData[IDX(i+1,j+1)*3]).Length();

          // lower triangle
	  if ( sampleFlgs[IDX(i+1,j)] ) {
	    vd[2] = Vec3<float>(&sampleData[IDX(i+1,j)*3]).Length();
	    if ( calcLineSegment(samplePts[IDX(i,j)], samplePts[IDX(i+1,j)],
				 samplePts[IDX(i+1,j+1)],
				 vd[0], vd[2], vd[1], val, lp1, lp2) ) {
	      plns->alcVerts(idx + 2);
	      plns->setVert(idx++, lp1, FALSE);
	      plns->setVert(idx++, lp2, FALSE);
	    }
	  }

	  // upper triangle
	  if ( sampleFlgs[IDX(i,j+1)] ) {
	    vd[2] = Vec3<float>(&sampleData[IDX(i,j+1)*3]).Length();
	    if ( calcLineSegment(samplePts[IDX(i,j)],samplePts[IDX(i+1,j+1)],
				 samplePts[IDX(i,j+1)],
				 vd[0], vd[1], vd[2], val, lp1, lp2) ) {
	      plns->alcVerts(idx + 2);
	      plns->setVert(idx++, lp1, FALSE);
	      plns->setVert(idx++, lp2, FALSE);
	    }
	  }
        } // end of for(i)
      } // end of for(j)
    } // end of if(DATA_Veclen)
    else {
      for ( j = 0; j < sampleSize.y -1; j++ ) {
        for ( i = 0; i < sampleSize.x -1; i++ ) {
	  vector3 vd;

	  if ( ! sampleFlgs[IDX(i,j)] || ! sampleFlgs[IDX(i+1,j+1)] )
	    continue;
	  vd[0] = sampleData[IDX(i,j)];
	  vd[1] = sampleData[IDX(i+1,j+1)];

          // lower triangle
	  if ( sampleFlgs[IDX(i+1,j)] ) {
	    vd[2] = sampleData[IDX(i+1,j)];
	    if ( calcLineSegment(samplePts[IDX(i,j)], samplePts[IDX(i+1,j)],
				 samplePts[IDX(i+1,j+1)],
				 vd[0], vd[2], vd[1], val, lp1, lp2) ) {
	      plns->alcVerts(idx + 2);
	      plns->setVert(idx++, lp1, FALSE);
	      plns->setVert(idx++, lp2, FALSE);
	    }
	  }

	  // upper triangle
	  if ( sampleFlgs[IDX(i,j+1)] ) {
	    vd[2] = sampleData[IDX(i,j+1)];
	    if ( calcLineSegment(samplePts[IDX(i,j)],samplePts[IDX(i+1,j+1)],
				 samplePts[IDX(i,j+1)],
				 vd[0], vd[1], vd[2], val, lp1, lp2) ) {
	      plns->alcVerts(idx + 2);
	      plns->setVert(idx++, lp1, FALSE);
	      plns->setVert(idx++, lp2, FALSE);
	    }
	  }
        } // end of for(i)
      } // end of for(j)
    } // end of if(!DATA_Veclen)

    // lines' attr
    if ( m_useCMap )
      plns->setColor3(0, &m_lut.lutEntry[m_lut.getValIdx(val) * 4]);
    else
      plns->setColor4(0, m_colour);
    plns->setTransparency(m_antiAlias);
  } // end of for(val, k)

  // ok
  DeAllocate(sampleFlgs);
  DeAllocate(sampleData);
  m_numValidLines = m_numLines;
  getPrivateMaterial()->setRenderMode(RT_WIRE);
  m_updatedStp = m_requestedStp;
  return true;
}
#undef IDX

