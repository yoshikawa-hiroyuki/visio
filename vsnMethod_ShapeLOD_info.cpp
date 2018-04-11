//
// vsnMethod_ShapeLOD_info
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

#include "vsnMethod_ShapeLOD_info.h"
#include "vsnData_ShapeLOD.h"
#include "vsnError.h"

using namespace std;
using namespace CES;


//----------------------------------------------------------------
// class vsnMethod_ShapeLOD_info
//----------------------------------------------------------------

/* constructors / destructor */

vsnMethod_ShapeLOD_info::vsnMethod_ShapeLOD_info(const string& name)
  : vsnMethod_info(name)
{
}

vsnMethod_ShapeLOD_info::~vsnMethod_ShapeLOD_info() {
}


/* vsnMethod_info methods */

bool vsnMethod_ShapeLOD_info::updateRepStr() {
  vsnData_ShapeLOD* pdShape = dynamic_cast<vsnData_ShapeLOD*>(p_refData);
  if ( ! pdShape ) {
    m_repStr = "out of data";
    return false;
  }

  const char *modestr[] = {
    "none", "WaveFront obj", "STL(Ascii)", "STL(Binary)",
    "STL(Binary, big-endian)"
  };

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
    deque<string> path_lst = pdShape->getFileList();
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

	vsnData_ShapeLOD::ShapeType shapeType = pdShape->getShapeType(i);
	msg += "    Type = ";
	msg += modestr[shapeType];
	msg += "\n";

	switch ( shapeType ) {
	case vsnData_ShapeLOD::ShapeOBJ: {
	  int nGrps(0), nTrias(0);
	  vfrGroup* pTop = dynamic_cast<vfrGroup*>(pdShape->getShape(i));
	  if ( ! pTop ) break;
	  nGrps = pTop->getNumChildren();
	  for ( int i = 0; i < nGrps; i++ ) {
	    vfrNode* pobj = pTop->getChild(i);
	    if ( ! pobj ) continue;
	    nTrias += (pobj->getNumVerts() / 3);
	  } // end of for(i)
	  sprintf(txt, "    Size = %d trias, %d groups\n", nTrias, nGrps);
	  msg += txt;
	  break;
	}
	case vsnData_ShapeLOD::ShapeSLA:
	case vsnData_ShapeLOD::ShapeSLB:
	case vsnData_ShapeLOD::ShapeSLB_BE: {
	  int nTrias(0);
	  vfrNode* pobj = pdShape->getShape(i);
	  if ( ! pobj ) break;
	  nTrias = pobj->getNumVerts() / 3;
	  sprintf(txt, "    Size = %d trias\n", nTrias);
	  msg += txt;
	  break;
	}
	default:
	  break;
	} // end of switch(shapeType)

      } // end of for(i)
    }
  }
  msg += "\n";

  const Vec3<float>* pbb = pdShape->getBbox();
  sprintf(txt, " Bbox\n  x = [%.3f : %.3f]\n", pbb[0][0], pbb[1][0]);
  msg += txt;
  sprintf(txt, "  y = [%.3f : %.3f]\n", pbb[0][1], pbb[1][1]);
  msg += txt;
  sprintf(txt, "  z = [%.3f : %.3f]\n", pbb[0][2], pbb[1][2]);
  msg += txt;

  return true;
}


/* vsnMethodObj methods */

bool vsnMethod_ShapeLOD_info::update(const bool force) {
  vsnData_ShapeLOD* pData = dynamic_cast<vsnData_ShapeLOD*>(p_refData);
  if ( ! pData ) return false;
  if ( ! updateRepStr() ) {
    updateUI();
    return false;
  }
  updateUI();
  return true;
}
