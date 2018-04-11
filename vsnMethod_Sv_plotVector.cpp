//
// vsnMethod_Sv_plotVector
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

#include "vsnMethod_Sv_plotVector.h"
#include "vsnError.h"

using namespace std;
using namespace CES;
using namespace VSN;


//----------------------------------------------------------------
// class vsnMethod_Sv_plotVector
//----------------------------------------------------------------

/* constructors / destructor */

vsnMethod_Sv_plotVector::vsnMethod_Sv_plotVector(const string& name)
  : vsnMethod_plotVector(name)
{
}

vsnMethod_Sv_plotVector::~vsnMethod_Sv_plotVector() {
}


/* vsnTimeSeriesMethodIF methods */

bool vsnMethod_Sv_plotVector::updateStep(const int stp,
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

  // alloc vector datas
  if ( ! m_vectors->alcVerts(sampleSz) ||
       ! m_vectors->alcNormals(sampleSz) ) {
    return false;
  }
  if ( m_selectedData == DATA_Veclen ||
       (m_selectedData > 0 && m_selectedData <= dlen) ) {
    if ( ! m_vectors->alcColors(sampleSz) ) {
      return false;
    }
  }

  // get GridUtil of Sv data
  if ( ! _gus.setup(pData->getDims(), dlen,
		    (float*)pData->getCoord(m_requestedStp),
                    (float*)pData->getData(m_requestedStp)) ) {
    return false;
  }

  // get bbox
  const Vec3<float>* pbb = pData->getBbox();

  // set verts of vectors
  vector3* vl = m_vectors->getVerts();
  memcpy(vl, samplePts, sizeof(vector3)*sampleSz);
  m_vectors->generateBbox();

  // set normals / colors of vectors
  register int i, c;
  vector3* nl = m_vectors->getNormals();
  vector4* cl = m_vectors->getColors();
  Vec3<float> vv;
  DVec3 xp; DVec4 x_i;
  if ( m_selectedData == DATA_Veclen ) {
    for ( i = 0; i < sampleSz; i++ ) {
      if ( ! _gus.is2D() &&
	   (samplePts[i][0] < pbb[0].m_v[0] ||
	    samplePts[i][0] > pbb[1].m_v[0] ||
	    samplePts[i][1] < pbb[0].m_v[1] ||
	    samplePts[i][1] > pbb[1].m_v[1] ||
	    samplePts[i][2] < pbb[0].m_v[2] ||
	    samplePts[i][2] > pbb[1].m_v[2]) ) {
	nl[i][0] = nl[i][1] = nl[i][2] = 0.f;
	cl[i][0] = cl[i][1] = cl[i][2] = 0.141f; cl[i][3] = 1.f;
	continue;
      }
      xp[0] = (double)samplePts[i][0];
      xp[1] = (double)samplePts[i][1];
      xp[2] = (double)samplePts[i][2];
      if ( _gus.ConvXtoI(xp, x_i) >= 0 &&
           _gus.InterpolateData(x_i, m_vecDataIdx, vv.m_v) ) {
	memcpy(nl[i], vv.m_v, sizeof(vector3));
	c = m_lut.getValIdx(vv.Length());
        cl[i][0] = m_lut.lutEntry[c*4  ];
        cl[i][1] = m_lut.lutEntry[c*4+1];
        cl[i][2] = m_lut.lutEntry[c*4+2];
        cl[i][3] = 1.f;
      } else {
	nl[i][0] = nl[i][1] = nl[i][2] = 0.f;
	cl[i][0] = cl[i][1] = cl[i][2] = 0.141f; cl[i][3] = 1.f;
      }
    } // end of for(i)
    m_vectors->setColorMode(AT_PER_VERTEX);
  }
  else if ( m_selectedData > 0 && m_selectedData <= dlen ) {
    float dval;
    for ( i = 0; i < sampleSz; i++ ) {
      if ( ! _gus.is2D() &&
	   (samplePts[i][0] < pbb[0].m_v[0] ||
	    samplePts[i][0] > pbb[1].m_v[0] ||
	    samplePts[i][1] < pbb[0].m_v[1] ||
	    samplePts[i][1] > pbb[1].m_v[1] ||
	    samplePts[i][2] < pbb[0].m_v[2] ||
	    samplePts[i][2] > pbb[1].m_v[2]) ) {
	nl[i][0] = nl[i][1] = nl[i][2] = 0.f;
	cl[i][0] = cl[i][1] = cl[i][2] = 0.141f; cl[i][3] = 1.f;
	continue;
      }
      xp[0] = (double)samplePts[i][0];
      xp[1] = (double)samplePts[i][1];
      xp[2] = (double)samplePts[i][2];
      if ( _gus.ConvXtoI(xp, x_i) >= 0 &&
           _gus.InterpolateData(x_i, m_vecDataIdx, vv.m_v) ) {
	memcpy(nl[i], vv.m_v, sizeof(vector3));
	if ( _gus.InterpolateData(x_i, m_selectedData -1, dval) ) {
	  c = m_lut.getValIdx(dval);
	  cl[i][0] = m_lut.lutEntry[c*4  ];
	  cl[i][1] = m_lut.lutEntry[c*4+1];
	  cl[i][2] = m_lut.lutEntry[c*4+2];
	  cl[i][3] = 1.f;
	} else {
	  cl[i][0] = cl[i][1] = cl[i][2] = 0.141f; cl[i][3] = 1.f;
	}
      } else {
	nl[i][0] = nl[i][1] = nl[i][2] = 0.f;
	cl[i][0] = cl[i][1] = cl[i][2] = 0.141f; cl[i][3] = 1.f;
      }
    } // end of for(i)
    m_vectors->setColorMode(AT_PER_VERTEX);
  }
  else { // no color
    for ( i = 0; i < sampleSz; i++ ) {
      if ( ! _gus.is2D() &&
	   (samplePts[i][0] < pbb[0].m_v[0] ||
	    samplePts[i][0] > pbb[1].m_v[0] ||
	    samplePts[i][1] < pbb[0].m_v[1] ||
	    samplePts[i][1] > pbb[1].m_v[1] ||
	    samplePts[i][2] < pbb[0].m_v[2] ||
	    samplePts[i][2] > pbb[1].m_v[2]) ) {
	nl[i][0] = nl[i][1] = nl[i][2] = 0.f;
	continue;
      }
      xp[0] = (double)samplePts[i][0];
      xp[1] = (double)samplePts[i][1];
      xp[2] = (double)samplePts[i][2];
      if ( _gus.ConvXtoI(xp, x_i) >= 0 &&
           _gus.InterpolateData(x_i, m_vecDataIdx, vv.m_v) ) {
	memcpy(nl[i], vv.m_v, sizeof(vector3));
      } else {
	nl[i][0] = nl[i][1] = nl[i][2] = 0.f;
      }
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
