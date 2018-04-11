//
// vsnMethod_TriaCells_plotContour
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

#include "vsnMethod_TriaCells_plotContour.h"
#include "vsnError.h"

using namespace std;
using namespace CES;
using namespace VSN;


//----------------------------------------------------------------
// class vsnMethod_TriaCells_plotContour
//----------------------------------------------------------------

/* constructors / destructor */

vsnMethod_TriaCells_plotContour::
vsnMethod_TriaCells_plotContour(const std::string& name)
  : vsnMethod_plotContour(name)
{
  m_useSampler = false;
}

vsnMethod_TriaCells_plotContour::~vsnMethod_TriaCells_plotContour() {
}


/* vsnTimeSeriesMethodIF methods */

bool vsnMethod_TriaCells_plotContour::updateStep(const int stp,
						 const bool force,
						 const bool cascade)
{
  vsnData_TriaCells* pData = dynamic_cast<vsnData_TriaCells*>(p_refData);
  if ( ! pData ) return false;
  m_requestedStp = stp;
  if ( m_requestedStp < 0 )
    m_requestedStp = pData->getCurrentStepIdx();

  if ( force ) m_updatedStp = -1;
  if ( m_updatedStp == m_requestedStp ) return true;
  m_updatedStp = -1;

  // prepare lines
  register int i, k, idx;
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

  // get numPts, numCells, verts, indices, data from pData
  int np = pData->getNumVerts();
  int nc = pData->getNumIndices() / 3;
  if ( np < 3 || nc < 1 ) return true;
  vector3* pVts = pData->getVerts();
  int* pIdcs = pData->getIndices();
  float* pd = pData->getData();
  if ( ! pVts || ! pIdcs || ! pd ) {
    ErrMsg(MsgERR, getMethodType() + string("[") + getName()
           + string("]: can't get data array"));
    return false;
  }

  // alloc lines' data
  //   needs nc(trias) * 2(verts) vertices for one contour
  size_t maxLineSegs = nc;
  for ( i = 0; i < m_numLines; i++ ) {
    vfrNode* p = getChild(i);
    if ( ! p->alcPools(maxLineSegs * 2, -1, -1, -1) ) {
      ErrMsg(MsgERR, getMethodType() + string("[") + getName()
             + string("]: memory allocation failed"));
      return false;
    }
  } // end of for(i)

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
      for ( i = 0; i < nc; i++ ) {
	vector3 vd;
	vd[0] = Vec3<float>(pd[ pIdcs[i*3  ]*dlen + m_vecDataIdx[0] ],
			    pd[ pIdcs[i*3  ]*dlen + m_vecDataIdx[1] ],
			    pd[ pIdcs[i*3  ]*dlen + m_vecDataIdx[2] ]).Length();
	vd[1] = Vec3<float>(pd[ pIdcs[i*3+1]*dlen + m_vecDataIdx[0] ],
			    pd[ pIdcs[i*3+1]*dlen + m_vecDataIdx[1] ],
			    pd[ pIdcs[i*3+1]*dlen + m_vecDataIdx[2] ]).Length();
	vd[2] = Vec3<float>(pd[ pIdcs[i*3+2]*dlen + m_vecDataIdx[0] ],
			    pd[ pIdcs[i*3+2]*dlen + m_vecDataIdx[1] ],
			    pd[ pIdcs[i*3+2]*dlen + m_vecDataIdx[2] ]).Length();

	if ( calcLineSegment(pVts[ pIdcs[i*3  ] ],
			     pVts[ pIdcs[i*3+1] ], pVts[ pIdcs[i*3+2] ], 
			     vd[0], vd[1], vd[2], val, lp1, lp2) ) {
	  plns->alcVerts(idx + 2);
	  plns->setVert(idx++, lp1, FALSE);
	  plns->setVert(idx++, lp2, FALSE);
	}
      } // end of for(i)
    } // end of if(DATA_Veclen)
    else {
      for ( i = 0; i < nc; i++ ) {
	size_t dsel = m_selectedData -1;
	vector3 vd;
	vd[0] = pd[ pIdcs[i*3  ]*dlen + dsel ];
	vd[1] = pd[ pIdcs[i*3+1]*dlen + dsel ];
	vd[2] = pd[ pIdcs[i*3+2]*dlen + dsel ];

	if ( calcLineSegment(pVts[ pIdcs[i*3  ] ],
			     pVts[ pIdcs[i*3+1] ], pVts[ pIdcs[i*3+2] ],
			     vd[0], vd[1], vd[2], val, lp1, lp2) ) {
	  plns->alcVerts(idx + 2);
	  plns->setVert(idx++, lp1, FALSE);
	  plns->setVert(idx++, lp2, FALSE);
	}
      } // end of for(i)
    } // end of if(!DATA_Veclen)

    // lines' attr
    if ( m_useCMap )
      plns->setColor3(0, &m_lut.lutEntry[m_lut.getValIdx(val) * 4]);
    else
      plns->setColor4(0, m_colour);
    plns->setTransparency(m_antiAlias);
    plns->generateBbox();
  } // end of for(val, k)

  // ok
  generateBbox();
  m_numValidLines = m_numLines;
  getPrivateMaterial()->setRenderMode(RT_WIRE);
  m_updatedStp = m_requestedStp;
  return true;
}
