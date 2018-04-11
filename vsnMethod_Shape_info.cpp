//
// vsnMethod_Shape_info
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

#include "vsnMethod_Shape_info.h"
#include "vsnData_Shape.h"
#include "vsnError.h"

using namespace std;
using namespace CES;


//----------------------------------------------------------------
// class vsnMethod_Shape_info
//----------------------------------------------------------------

/* constructors / destructor */

vsnMethod_Shape_info::vsnMethod_Shape_info(const string& name)
  : vsnMethod_info(name)
{
}

vsnMethod_Shape_info::~vsnMethod_Shape_info() {
}


/* vsnMethod_info methods */

bool vsnMethod_Shape_info::updateRepStr() {
  vsnData_Shape* pdShape = dynamic_cast<vsnData_Shape*>(p_refData);
  if ( ! pdShape ) {
    m_repStr = "out of data";
    return false;
  }

  char dc = vsnPath_getDelimChar();
  char txt[256];
  string& msg = m_repStr;
  msg = "Data = ";
  msg += p_refData->getDataType();
  msg += "[";
  msg += p_refData->getName();
  msg += "]\n";
  msg += "Path = ";
  if ( BaseName(pdShape->getFilePath(), string(""), dc) != VSN::vsn_seqfile )
    msg += pdShape->getFilePath() + "\n";
  else {
    msg += "sequencial\n";
    deque<string> path_lst = pdShape->getSeqFilePathes();
    if ( path_lst.empty() )
      msg += "  no file information\n";
    else {
      msg += "  base_dir: " + DirName(path_lst[0], dc) + "\n";
      register int i, k, nf = path_lst.size();
      for ( i = 0, k = 1; (nf / k) > 0; i++, k*=10 );
      char stmpl[32];
      if ( i > 0 ) sprintf(stmpl, "  file#%%0%dd: %%s", i);
      else         sprintf(stmpl, "  file#%%d: %%s");
      for ( i = 0; i < nf; i++ ) {
        sprintf(txt, stmpl, i, BaseName(path_lst[i], "", dc).c_str());
        msg += string(txt) + "\n";
      } // end of for(i)
    }
  }
  msg += "\n";

  size_t nstp = pdShape->getNumSteps();
  int stp0 = -1, stp1 = -1;
  (void)pdShape->getTimeStepNo(0, stp0);
  (void)pdShape->getTimeStepNo(nstp -1, stp1);
  sprintf(txt, " Steps = %lu [%d : %d]\n", nstp, stp0, stp1);
  msg += txt;
  float tms0 = pdShape->getTime(0);
  float tms1 = pdShape->getTime(nstp -1);
  sprintf(txt, " Time = [%g : %g]\n", tms0, tms1);
  msg += txt;

  size_t curStp = pdShape->getCurrentStepIdx();
  sprintf(txt, " Current-Step = %lu\n", curStp);
  msg += txt;
  sprintf(txt, " Current-Time = %g\n\n", pdShape->getTime(curStp));
  msg += txt;

  const char *modestr[] = {
    "none", "WaveFront obj", "STL(Ascii)", "STL(Binary)",
    "STL(Binary, big-endian)"
  };

  vsnData_Shape::ShapeType shapeType = pdShape->getShapeType();
  msg += " Type = ";
  msg += modestr[shapeType];
  msg += "\n";

  switch ( shapeType ) {
  case vsnData_Shape::ShapeOBJ: {
    int nGrps(0), nTrias(0);
    vfrGroup* pTop = dynamic_cast<vfrGroup*>(pdShape->getShape());
    if ( ! pTop ) break;
    nGrps = pTop->getNumChildren();
    for ( int i = 0; i < nGrps; i++ ) {
      vfrNode* pobj = pTop->getChild(i);
      if ( ! pobj ) continue;
      nTrias += (pobj->getNumVerts() / 3);
    } // end of for(i)
    sprintf(txt, " Size = %d trias, %d groups\n", nTrias, nGrps);
    msg += txt;
    break;
  }
  case vsnData_Shape::ShapeSLA:
  case vsnData_Shape::ShapeSLB:
  case vsnData_Shape::ShapeSLB_BE: {
    int nTrias(0);
    vfrNode* pobj = pdShape->getShape();
    if ( ! pobj ) break;
    nTrias = pobj->getNumVerts() / 3;
    sprintf(txt, " Size = %d trias\n", nTrias);
    msg += txt;
    break;
  }
  default:
    break;
  } // end of switch(shapeType)

  const Vec3<float>* pbb = pdShape->getBbox();
  sprintf(txt, " Bbox\n  x = [%.3g : %.3g]\n", pbb[0][0], pbb[1][0]);
  msg += txt;
  sprintf(txt, "  y = [%.3g : %.3g]\n", pbb[0][1], pbb[1][1]);
  msg += txt;
  sprintf(txt, "  z = [%.3g : %.3g]\n", pbb[0][2], pbb[1][2]);
  msg += txt;

  return true;
}


/* vsnMethodObj methods */

bool vsnMethod_Shape_info::update(const bool force) {
  return updateStep(m_requestedStp, force);
}


/* vsnTimeSeriesMethodIF methods */

bool vsnMethod_Shape_info::updateStep(const int stp,
				      const bool force, const bool cascade)
{
  vsnData_Shape* pData = dynamic_cast<vsnData_Shape*>(p_refData);
  if ( ! pData ) return false;
  m_requestedStp = stp;
  if ( m_requestedStp < 0 )
    m_requestedStp = pData->getCurrentStepIdx();

  if ( force ) m_updatedStp = -1;
  if ( m_updatedStp == m_requestedStp ) return true;
  m_updatedStp = -1;

  if ( ! updateRepStr() ) {
    updateUI();
    return false;
  }

  m_updatedStp = m_requestedStp;
  updateUI();
  return true;
}
