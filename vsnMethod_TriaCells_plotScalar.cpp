//
// vsnMethod_TriaCells_plotScalar
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

#include "vsnMethod_TriaCells_plotScalar.h"
#include "vsnError.h"

using namespace std;
using namespace CES;
using namespace VSN;


//----------------------------------------------------------------
// class vsnMethod_TriaCells_plotScalar
//----------------------------------------------------------------

/* constructors / destructor */

vsnMethod_TriaCells_plotScalar::
vsnMethod_TriaCells_plotScalar(const string& name)
  : vsnMethod_plotScalar(name), m_shape(NULL)
{
  m_useSampler = false;
}

vsnMethod_TriaCells_plotScalar::~vsnMethod_TriaCells_plotScalar() {
  if ( m_shape ) {
    delete m_shape;
    m_shape = NULL;
  }
}


/* vsnTimeSeriesMethodIF methods */

bool vsnMethod_TriaCells_plotScalar::updateStep(const int stp,
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

  // shape data
  if ( ! m_shape ) {
    m_shape = new vfrIndexPolygons();
    if ( ! m_shape || ! m_shape->alcMaterial() ) {
      ErrMsg(MsgERR, getMethodType() + string("[") + getName()
             + string("]: memory allocation failed"));
      return false;
    }
    m_shape->setNormalMode(AT_PER_VERTEX);
    addChild(m_shape);
  }
  m_shape->getPrivateMaterial()->setRenderMode(RT_NONE);
  if ( ! m_show ) {
    return true;
  }

  // check selected data
  size_t dlen = pData->getDataLen();
  if ( m_selectedData == DATA_Veclen ) {
    if ( ! isValidVecData() ) return true;
  }
  else if ( m_selectedData > dlen ) return true;

  // alloc shape datas
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
  if ( ! m_shape->alcVerts(np) || ! m_shape->alcColors(np) ||
       ! m_shape->alcIndices(nc * 4) ) {
    ErrMsg(MsgERR, getMethodType() + string("[") + getName()
           + string("]: memory allocation failed"));
    return false;
  }

  // set shape verts
  register int i, c;
  vector3* vl = m_shape->getVerts();
  memcpy(vl, pVts, sizeof(vector3)*np);
  m_shape->generateBbox();

  int* il = m_shape->getIndices();
  for ( i = 0; i < nc; i++ ) {
    il[i*4    ] = pIdcs[i*3    ];
    il[i*4 + 1] = pIdcs[i*3 + 1];
    il[i*4 + 2] = pIdcs[i*3 + 2];
    il[i*4 + 3] = VFR_END_OF_ELEM;
  } // end of for(i)
  m_shape->generateNormals();

  // set shape colors
  vector4* cl = m_shape->getColors();
  if ( m_selectedData == DATA_Veclen ) {
    for ( i = 0; i < np; i++ ) {
      float dval = Vec3<float>(pd[i*dlen + m_vecDataIdx[0]],
			       pd[i*dlen + m_vecDataIdx[1]],
			       pd[i*dlen + m_vecDataIdx[2]]).Length();
      c = m_lut.getValIdx(dval);
      cl[i][0] = m_lut.lutEntry[c*4  ];
      cl[i][1] = m_lut.lutEntry[c*4+1];
      cl[i][2] = m_lut.lutEntry[c*4+2];
      cl[i][3] = 1.f;
    } // end of for(i)
    m_shape->setColorMode(AT_PER_VERTEX);
  } // end of if(DATA_Veclen)
  else if ( m_selectedData > 0 && m_selectedData <= dlen ) {
    for ( i = 0; i < np; i++ ) {
      float dval = pd[i*dlen + (m_selectedData - 1)];
      c = m_lut.getValIdx(dval);
      cl[i][0] = m_lut.lutEntry[c*4  ];
      cl[i][1] = m_lut.lutEntry[c*4+1];
      cl[i][2] = m_lut.lutEntry[c*4+2];
      cl[i][3] = 1.f;
    } // end of for(i)    
    m_shape->setColorMode(AT_PER_VERTEX);
  } // end of if(!DATA_Veclen&&!DATA_None)
  else {
    m_shape->setColorMode(AT_WHOLE);
  } // end of if(DATA_None)

  // ok
  m_shape->getPrivateMaterial()->setRenderMode(m_showType);
  setBaseColor(m_colour);
  setHilight(m_hilight);
  m_updatedStp = m_requestedStp;
  return true;
}
