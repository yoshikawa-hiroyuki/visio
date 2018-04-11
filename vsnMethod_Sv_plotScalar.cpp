//
// vsnMethod_Sv_plotScalar
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

#include "vsnMethod_Sv_plotScalar.h"
#include "vsnError.h"

using namespace std;
using namespace CES;
using namespace VSN;


//----------------------------------------------------------------
// class vsnMethod_Sv_plotScalar
//----------------------------------------------------------------

/* constructors / destructor */

vsnMethod_Sv_plotScalar::vsnMethod_Sv_plotScalar(const string& name)
  : vsnMethod_plotScalar(name)
{
}

vsnMethod_Sv_plotScalar::~vsnMethod_Sv_plotScalar() {
}


/* vsnTimeSeriesMethodIF methods */

bool vsnMethod_Sv_plotScalar::updateStep(const int stp,
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

  // mesh data
  if ( ! m_mesh ) {
    m_mesh = new vfrMesh2D();
    if ( ! m_mesh ) {
      ErrMsg(MsgERR, getMethodType() + string("[") + getName()
             + string("]: memory allocation failed"));
      return false;
    }
    m_mesh->alcMaterial();
    m_mesh->setNormalMode(AT_WHOLE);
    addChild(m_mesh);
  }
  m_mesh->getPrivateMaterial()->setRenderMode(RT_NONE);
  if ( ! m_show ) {
    return true;
  }

  // check selected data
  size_t dlen = pData->getDataLen();
  if ( m_selectedData == DATA_Veclen ) {
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

  // alloc mesh datas
  if ( ! m_mesh->setMeshSize(sampleSize) ||
       ! m_mesh->alcColors(sampleSz) ) {
    return false;
  }

  // set mesh verts
  register int i, c;
  vector3* vl = m_mesh->getVerts();
  memcpy(vl, samplePts, sizeof(vector3)*sampleSz);
  m_mesh->generateBbox();

  // set normal vector
  Vec3<float> vn = p_splr->getNormalVec();
  m_mesh->setNormal(0, vn.m_v);

  // get GridUtil of Sv data
  if ( ! _gus.setup(pData->getDims(), pData->getDataLen(),
		    (float*)pData->getCoord(m_requestedStp),
		    (float*)pData->getData(m_requestedStp)) ) {
    return false;
  }

  // get bbox
  const Vec3<float>* pbb = pData->getBbox();

  // set mesh colors
  vector4* cl = m_mesh->getColors();
  if ( m_selectedData == DATA_Veclen ) {
    DVec3 xp; DVec4 x_i;
    Vec3<float> vv;
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
	   _gus.InterpolateData(x_i, m_vecDataIdx, vv.m_v) ) {
	c = m_lut.getValIdx(vv.Length());
	cl[i][0] = m_lut.lutEntry[c*4  ];
	cl[i][1] = m_lut.lutEntry[c*4+1];
	cl[i][2] = m_lut.lutEntry[c*4+2];
	cl[i][3] = 1.f;
      } else {
	cl[i][0] = cl[i][1] = cl[i][2] = 0.141f; cl[i][3] = 1.f;
      }
    } // end of for(i)
    m_mesh->setColorMode(AT_PER_VERTEX);
  } // end of if(DATA_Veclen)
  else if ( m_selectedData > 0 && m_selectedData <= dlen ) {
    DVec3 xp; DVec4 x_i;
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
	c = m_lut.getValIdx(dval);
	cl[i][0] = m_lut.lutEntry[c*4  ];
	cl[i][1] = m_lut.lutEntry[c*4+1];
	cl[i][2] = m_lut.lutEntry[c*4+2];
	cl[i][3] = 1.f;
      } else {
	cl[i][0] = cl[i][1] = cl[i][2] = 0.141f; cl[i][3] = 1.f;
      }
    } // end of for(i)    
    m_mesh->setColorMode(AT_PER_VERTEX);
  } // end of if(!DATA_Veclen&&!DATA_None)
  else {
    m_mesh->setColorMode(AT_WHOLE);
  } // end of if(DATA_None)

  // ok
  m_mesh->getPrivateMaterial()->setRenderMode(m_showType);
  setBaseColor(m_colour);
  setHilight(m_hilight);
  m_updatedStp = m_requestedStp;
  return true;
}
