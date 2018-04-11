//
// vsnMethod_TriaCells_plotVector
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

#include "vsnMethod_TriaCells_plotVector.h"
#include "vsnError.h"

using namespace std;
using namespace CES;
using namespace VSN;


//----------------------------------------------------------------
// class vsnMethod_TriaCells_plotVector
//----------------------------------------------------------------

/* constructors / destructor */

vsnMethod_TriaCells_plotVector::
vsnMethod_TriaCells_plotVector(const string& name)
  : vsnMethod_plotVector(name)
{
  m_useSampler = false;
}

vsnMethod_TriaCells_plotVector::~vsnMethod_TriaCells_plotVector() {
}


/* vsnTimeSeriesMethodIF methods */

bool vsnMethod_TriaCells_plotVector::updateStep(const int stp,
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

  // vectors data
  if ( ! m_vectors ) {
    m_vectors = new vfrVectors();
    if ( ! m_vectors ) {
      ErrMsg(MsgERR, getMethodType() + string("[") + getName()
             + string("]: memory allocation failed"));
      return false;
    }
    m_vectors->alcMaterial();
    m_vectors->setHeadWidth(0.03f);
    addChild(m_vectors);
  }
  m_vectors->getPrivateMaterial()->setRenderMode(RT_NONE);
  if ( ! m_show ) {
    return true;
  }

  // check selected data
  size_t dlen = pData->getDataLen();
  if ( ! isValidVecData() ) return true;  

  // alloc vector datas
  int np = pData->getNumVerts();
  int nc = pData->getNumIndices() / 3;
  if ( np < 1 || nc < 1 ) return true;
  vector3* pVts = pData->getVerts();
  float* pd = pData->getData();
  if ( ! pVts || ! pd ) {
    ErrMsg(MsgERR, getMethodType() + string("[") + getName()
           + string("]: can't get data array"));
    return false;
  }
  if ( ! m_vectors->alcVerts(np) || ! m_vectors->alcNormals(np) ) {
    return false;
  }
  if ( m_selectedData == DATA_Veclen ||
       (m_selectedData > 0 && m_selectedData <= dlen) ) {
    if ( ! m_vectors->alcColors(np) ) {
      return false;
    }
  }

  // set verts of vectors
  vector3* vl = m_vectors->getVerts();
  memcpy(vl, pVts, sizeof(vector3)*np);
  m_vectors->generateBbox();

  // set normals / colors of vectors
  register int i, c;
  vector3* nl = m_vectors->getNormals();
  vector4* cl = m_vectors->getColors();
  Vec3<float> vv;
  if ( m_selectedData == DATA_Veclen ) {
    for ( i = 0; i < np; i++ ) {
      vv = Vec3<float>(pd[i*dlen + m_vecDataIdx[0]],
		       pd[i*dlen + m_vecDataIdx[1]],
		       pd[i*dlen + m_vecDataIdx[2]]);
      memcpy(nl[i], vv.m_v, sizeof(vector3));
      c = m_lut.getValIdx(vv.Length());
      cl[i][0] = m_lut.lutEntry[c*4  ];
      cl[i][1] = m_lut.lutEntry[c*4+1];
      cl[i][2] = m_lut.lutEntry[c*4+2];
      cl[i][3] = 1.f;
    } // end of for(i)
    m_vectors->setColorMode(AT_PER_VERTEX);
  }
  else if ( m_selectedData > 0 && m_selectedData <= dlen ) {
    float dval;
    for ( i = 0; i < np; i++ ) {
      vv = Vec3<float>(pd[i*dlen + m_vecDataIdx[0]],
		       pd[i*dlen + m_vecDataIdx[1]],
		       pd[i*dlen + m_vecDataIdx[2]]);
      memcpy(nl[i], vv.m_v, sizeof(vector3));
      dval = pd[i*dlen + (m_selectedData -1)];
      c = m_lut.getValIdx(dval);
      cl[i][0] = m_lut.lutEntry[c*4  ];
      cl[i][1] = m_lut.lutEntry[c*4+1];
      cl[i][2] = m_lut.lutEntry[c*4+2];
      cl[i][3] = 1.f;
    } // end of for(i)
    m_vectors->setColorMode(AT_PER_VERTEX);
  }
  else { // no color
    for ( i = 0; i < np; i++ ) {
      vv = Vec3<float>(pd[i*dlen + m_vecDataIdx[0]],
		       pd[i*dlen + m_vecDataIdx[1]],
		       pd[i*dlen + m_vecDataIdx[2]]);
      memcpy(nl[i], vv.m_v, sizeof(vector3));
    } // end of for(i)
    cl[0][0] = m_colour[0]; cl[0][1] = m_colour[1];
    cl[0][2] = m_colour[2]; cl[0][3] = 1.f;
    m_vectors->setColorMode(AT_WHOLE);
  }

  // vectors' attr
  m_vectors->setTransparency(m_antiAlias);
  m_vectors->setScaleFac(m_vecScale);
  m_vectors->setHeadMode(m_vecHead);
  m_vectors->getPrivateMaterial()->setLineWidth(m_lineWidth);

  // ok
  m_vectors->getPrivateMaterial()->setRenderMode(m_showType);
  setBaseColor(m_colour);
  setHilight(m_hilight);
  m_updatedStp = m_requestedStp;
  return true;
}
