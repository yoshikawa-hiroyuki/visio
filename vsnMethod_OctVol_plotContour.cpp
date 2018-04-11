//
// vsnMethod_OctVol_plotContour
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

#include "vsnMethod_OctVol_plotContour.h"
#include "vsnData_OctVol.h"
#include "vsnError.h"
// MHIR append begin
#include <vector>
// MHIR append end

using namespace std;
using namespace CES;
using namespace VSN;


//----------------------------------------------------------------
// class vsnMethod_OctVol_plotContour
//----------------------------------------------------------------

/* constructors / destructor */

vsnMethod_OctVol_plotContour::
vsnMethod_OctVol_plotContour(const std::string& name)
  : vsnMethod_plotContour(name)
{
}

vsnMethod_OctVol_plotContour::~vsnMethod_OctVol_plotContour() {
}


/* vsnTimeSeriesMethodIF methods */

#define IDX(i, j) (sampleSize.x*(j)+(i))


bool vsnMethod_OctVol_plotContour::updateStep(const int stp,
					      const bool force,
					      const bool cascade)
{
  vsnData_OctVol* pData = dynamic_cast<vsnData_OctVol*>(p_refData);
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

  // get nodelist of samplePts
  vsnOctTree::Node** pnodeLst
    = (vsnOctTree::Node**)Allocate(sizeof(vsnOctTree::Node*)*sampleSz);
  if ( ! pnodeLst ) {
    ErrMsg(MsgERR, getMethodType() + string("[") + getName()
	   + string("]: memory allocation failed"));
    return false;
  }
  for ( i = 0; i < sampleSz; i++ ) {
    pnodeLst[i] = pData->getNode(samplePts[i]);
  } // end of for(i)

  // get datalist on samplePts
  float* sampleData = NULL;
  if ( m_selectedData == DATA_Veclen )
    sampleData = (float*)Allocate(sizeof(vector3)*sampleSz);
  else
    sampleData = (float*)Allocate(sizeof(float)*sampleSz);
  if ( ! sampleData ) {
    ErrMsg(MsgERR, getMethodType() + string("[") + getName()
	   + string("]: memory allocation failed"));
    DeAllocate(pnodeLst);
    return false;
  }

  // MHIR append begin
  std::vector<bool> hasData(sampleSz);
  // MHIR append end

  if ( m_selectedData == DATA_Veclen ) {
    Vec3<float> dval;
    for ( i = 0; i < sampleSz; i++ ) {
      // MHIR modify begin
      if (pnodeLst[i] &&
	  pData->interpolateData(Vec3<float>(samplePts[i]), pnodeLst[i],
				 m_vecDataIdx, dval)) {
	memcpy(&sampleData[i*3], dval.m_v, sizeof(vector3));
	hasData[i] = true;
      } else {
	hasData[i] = false;
      }
      // MHIR modify end
    } // end of for(i)
  } else {
    Vec3<float> dval; Vec3<int> didx(m_selectedData-1, -1, -1);
    for ( i = 0; i < sampleSz; i++ ) {
      // MHIR modify begin
      if (pnodeLst[i] &&
	  pData->interpolateData(Vec3<float>(samplePts[i]), pnodeLst[i],
				 didx, dval)) {
	sampleData[i] = dval.m_v[0];
	hasData[i] = true;
      } else {
	hasData[i] = false;
      }
      // MHIR modify end
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

	  // MHIR append begin
	  if (hasData[IDX(i,j)] == false || hasData[IDX(i+1,j+1)] == false)
	    continue;
	  // MHIR append end

	  if ( ! pnodeLst[IDX(i,j)] || ! pnodeLst[IDX(i,j)]->m_pData ||
	       ! pnodeLst[IDX(i+1,j+1)] || ! pnodeLst[IDX(i+1,j+1)]->m_pData )
	    continue;
	  vd[0] = Vec3<float>(&sampleData[IDX(i,j)*3]).Length();
	  vd[1] = Vec3<float>(&sampleData[IDX(i+1,j+1)*3]).Length();

	  // MHIR append begin
	  if (hasData[IDX(i+1,j)] == false) continue;
	  // MHIR append end

          // lower triangle
	  if ( pnodeLst[IDX(i+1,j)] && pnodeLst[IDX(i+1,j)]->m_pData ) {
	    vd[2] = Vec3<float>(&sampleData[IDX(i+1,j)*3]).Length();
	    if ( calcLineSegment(samplePts[IDX(i,j)], samplePts[IDX(i+1,j)],
				 samplePts[IDX(i+1,j+1)],
				 vd[0], vd[2], vd[1], val, lp1, lp2) ) {
	      plns->alcVerts(idx + 2);
	      plns->setVert(idx++, lp1, FALSE);
	      plns->setVert(idx++, lp2, FALSE);
	    }
	  }

	  // MHIR append begin
	  if (hasData[IDX(i,j+1)] == false) continue;
	  // MHIR append end

	  // upper triangle
	  if ( pnodeLst[IDX(i,j+1)] && pnodeLst[IDX(i,j+1)]->m_pData ) {
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

	  // MHIR append begin
	  if (hasData[IDX(i,j)] == false || hasData[IDX(i+1,j+1)] == false)
	    continue;
	  // MHIR append end

	  if ( ! pnodeLst[IDX(i,j)] || ! pnodeLst[IDX(i,j)]->m_pData ||
	       ! pnodeLst[IDX(i+1,j+1)] || ! pnodeLst[IDX(i+1,j+1)]->m_pData )
	    continue;
	  vd[0] = sampleData[IDX(i,j)];
	  vd[1] = sampleData[IDX(i+1,j+1)];

	  // MHIR append begin
	  if (hasData[IDX(i+1,j)] == false) continue;
	  // MHIR append end

          // lower triangle
	  if ( pnodeLst[IDX(i+1,j)] && pnodeLst[IDX(i+1,j)]->m_pData ) {
	    vd[2] = sampleData[IDX(i+1,j)];
	    if ( calcLineSegment(samplePts[IDX(i,j)], samplePts[IDX(i+1,j)],
				 samplePts[IDX(i+1,j+1)],
				 vd[0], vd[2], vd[1], val, lp1, lp2) ) {
	      plns->alcVerts(idx + 2);
	      plns->setVert(idx++, lp1, FALSE);
	      plns->setVert(idx++, lp2, FALSE);
	    }
	  }

	  // MHIR append begin
	  if (hasData[IDX(i,j+1)] == false) continue;
	  // MHIR append end

	  // upper triangle
	  if ( pnodeLst[IDX(i,j+1)] && pnodeLst[IDX(i,j+1)]->m_pData ) {
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
  DeAllocate(pnodeLst);
  DeAllocate(sampleData);
  m_numValidLines = m_numLines;
  getPrivateMaterial()->setRenderMode(RT_WIRE);
  m_updatedStp = m_requestedStp;
  return true;
}
#undef IDX

