//
// vsnMethod_Sv_probe
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

#include "vsnMethod_Sv_probe.h"
#include "vsnError.h"

#include "vfrLines.h"
#include "vfrCone.h"
#include "vfrCylinder.h"

using namespace std;
using namespace CES;
using namespace VSN;


//----------------------------------------------------------------
// class vsnMethod_Sv_probe
//----------------------------------------------------------------

/* constructors / destructor */

vsnMethod_Sv_probe::vsnMethod_Sv_probe(const string& name)
  : vsnMethod_probe(name)
{
}

vsnMethod_Sv_probe::~vsnMethod_Sv_probe() {
}


/* vsnTimeSeriesMethodIF method */

bool vsnMethod_Sv_probe::updateStep(const int stp,
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

  // glyph
  if ( ! m_glyph ) return false;

  // initialize glyph / repStr
  m_glyph->setMatrix(m_matXForm);
  m_repStr = "out of data\n";
  if ( ! m_show )
    m_glyph->getPrivateMaterial()->setRenderMode(RT_NONE);
  else
    m_glyph->getPrivateMaterial()->setRenderMode(RT_SMOOTH);


  // get Sv datas
  CES::Vec3<size_t> dims = pData->getDims();
  if ( ! _gus.setup(dims, pData->getDataLen(),
		    (float*)pData->getCoord(m_requestedStp),
		    (float*)pData->getData(m_requestedStp),
		    (force ? -1 :m_requestedStp)) ) {
    ErrMsg(MsgERR, getMethodType() + string("[") + getName()
             + string("]: can't get grid data"));
    return false;
  }
  const unsigned char* maskptr = pData->getMask();

  // updated step
  m_updatedStp = m_requestedStp;


  //--------------- data probing ---------------
  char txt[128];
  float datVal;
  vsnGridUtilSv::Axis1Type gax1 = _gus.getAxis1();

  Vec3<float> pos = m_matXForm * Vec3<float>(0.f, 0.f, 0.f);
  DVec3 xp; DVec4 x_i;
  xp[0] = (double)pos[0]; xp[1] = (double)pos[1]; xp[2] = (double)pos[2];
  m_repStr = "Coord:\n";
  m_repStr += "  x = ";
  if ( gax1 == vsnGridUtilSv::XAxis ) sprintf(txt, "(%g)\n", xp[0]);
  else sprintf(txt, "%g\n", xp[0]);
  m_repStr += txt;
  m_repStr += "  y = ";
  if ( gax1 == vsnGridUtilSv::YAxis ) sprintf(txt, "(%g)\n", xp[1]);
  else sprintf(txt, "%g\n", xp[1]);
  m_repStr += txt;
  m_repStr += "  z = ";
  if ( gax1 == vsnGridUtilSv::ZAxis ) sprintf(txt, "(%g)\n", xp[2]);
  else sprintf(txt, "%g\n", xp[2]);
  m_repStr += txt;
  m_repStr += "Grid index:\n";

  // pre-check using bbox
  const Vec3<float>* pbb = pData->getBbox();
  if ( (gax1 != vsnGridUtilSv::XAxis &&
	(pos.m_v[0] < pbb[0].m_v[0] || pos.m_v[0] > pbb[1].m_v[0])) ||
       (gax1 != vsnGridUtilSv::YAxis &&
	(pos.m_v[1] < pbb[0].m_v[1] || pos.m_v[1] > pbb[1].m_v[1])) ||
       (gax1 != vsnGridUtilSv::ZAxis &&
	(pos.m_v[2] < pbb[0].m_v[2] || pos.m_v[2] > pbb[1].m_v[2])) ) {
    m_repStr += "  IJK = [out of range]";
    updateUI();
    return true;
  }

  // get contravariant components
  if ( _gus.ConvXtoI(xp, x_i) < 0 ) {
    m_repStr += "  IJK = [out of range]";
    updateUI();
    return true;
  }

  // grid index
  m_repStr += "  IJK = ";
  sprintf(txt, "[%d, %d, %d]", (int)x_i[0], (int)x_i[1], (int)x_i[2]);
  m_repStr += txt;
  m_repStr += " - ";
  sprintf(txt, "[%d, %d, %d]\n", (int)x_i[0]+1, (int)x_i[1]+1, (int)x_i[2]+1);
  m_repStr += txt;

  // datas
  if ( _gus.p_vecd ) {
    register size_t i;
    m_repStr += "Datas:\n";
    for ( i = 0; i < _gus.m_vecLen; i++ ) {
      _gus.InterpolateData(x_i, i, datVal);
      sprintf(txt, "  data#%lu = %g\n", i, datVal);
      m_repStr += txt;
    } // end of for(i)
  }

  // mask
  m_repStr += "Mask = ";
  if ( maskptr ) {
    int didx[3] = {(int)x_i[0], (int)x_i[1], (int)x_i[2]};
    if ( x_i[0] - didx[0] > 0.5 ) didx[0] += 1;
    if ( x_i[1] - didx[1] > 0.5 ) didx[1] += 1;
    if ( x_i[2] - didx[2] > 0.5 ) didx[2] += 1;
    if ( maskptr[dims[0]*dims[1]*didx[2] + dims[0]*didx[1] + didx[0]] )
      m_repStr += "on\n";
    else
      m_repStr += "off\n";
  } else {
    m_repStr += "none\n";
  }

  updateUI();
  return true;
}
