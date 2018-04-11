//
// vsnMethod_OctVol_info
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

#include "vsnMethod_OctVol_info.h"
#include "vsnData_OctVol.h"
#include "vsnError.h"

using namespace std;
using namespace CES;


//----------------------------------------------------------------
// class vsnMethod_OctVol_info
//----------------------------------------------------------------

/* constructors / destructor */

vsnMethod_OctVol_info::vsnMethod_OctVol_info(const string& name)
  : vsnMethod_info(name)
{
}

vsnMethod_OctVol_info::~vsnMethod_OctVol_info() {
}


/* vsnMethod_info methods */

bool vsnMethod_OctVol_info::updateRepStr() {
  vsnData_OctVol* pdOctVol = dynamic_cast<vsnData_OctVol*>(p_refData);
  if ( ! pdOctVol ) {
    m_repStr = "out of data";
    return false;
  }

  char dc = vsnPath_getDelimChar();
  char txt[256];
  string& msg = m_repStr;
  msg = "Data = ";
  msg += pdOctVol->getDataType();
  msg += "[";
  msg += pdOctVol->getName();
  msg += "]\n";
  msg += "Path = ";
  if ( BaseName(pdOctVol->getFilePath(), string(""), dc) != VSN::vsn_seqfile )
    msg += pdOctVol->getFilePath() + "\n";
  else {
    msg += "sequencial\n";
    deque<string> path_lst = pdOctVol->getSeqFilePathes();
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

  size_t nstp = pdOctVol->getNumSteps();
  int stp0 = -1, stp1 = -1;
  (void)pdOctVol->getTimeStepNo(0, stp0);
  (void)pdOctVol->getTimeStepNo(nstp -1, stp1);
  sprintf(txt, " Steps = %lu [%d : %d]\n", nstp, stp0, stp1);
  msg += txt;
  float tms0 = pdOctVol->getTime(0);
  float tms1 = pdOctVol->getTime(nstp -1);
  sprintf(txt, " Time = [%g : %g]\n", tms0, tms1);
  msg += txt;

  size_t curStp = pdOctVol->getCurrentStepIdx();
  sprintf(txt, " Current-Step = %lu\n", curStp);
  msg += txt;
  sprintf(txt, " Current-Time = %g\n\n", pdOctVol->getTime(curStp));
  msg += txt;

  const Vec3<size_t> dims = pdOctVol->getRootDims();
  sprintf(txt, " Root Dims = (%lu, %lu, %lu)\n", dims[0], dims[1], dims[2]);
  msg += txt;

  sprintf(txt, " Max-Level = %lu\n", pdOctVol->getMaxLevel());
  msg += txt;

  const Vec3<float>* pbb = pdOctVol->getBbox();
  const Vec3<float> pitc = pdOctVol->getPitch();
  msg += " Bbox / Pitch\n";
  sprintf(txt, "  x = [%.3g : %.3g / %g]\n", pbb[0][0], pbb[1][0], pitc[0]);
  msg += txt;
  sprintf(txt, "  y = [%.3g : %.3g / %g]\n", pbb[0][1], pbb[1][1], pitc[1]);
  msg += txt;
  sprintf(txt, "  z = [%.3g : %.3g / %g]\n", pbb[0][2], pbb[1][2], pitc[2]);
  msg += txt;

  msg += " #of cells\n";
  sprintf(txt, "  whole = %lu cells\n", pdOctVol->getWholeNodeList().size());
  msg += txt;
  sprintf(txt, "  leaf  = %lu cells\n", pdOctVol->getLeafNodeList().size());
  msg += txt;

  size_t dataLen = pdOctVol->getDataLen();
  sprintf(txt, " Data Length = %lu\n", dataLen);
  msg += txt;

  if ( dataLen > 0 ) {
    size_t d; float minmax[2];
    for ( d = 0; d < dataLen; d++ ) {
      pdOctVol->getMinMax(d, minmax);
      sprintf(txt, " Data%lu\n", d); msg += txt;
      sprintf(txt, "  min = %g\n  max = %g\n", minmax[0], minmax[1]);
      msg += txt;
    } // end of for(d)
  } // end of if(dataLen>0)

  return true;
}


/* vsnMethodObj methods */

bool vsnMethod_OctVol_info::update(const bool force) {
  return updateStep(m_requestedStp, force);
}


/* vsnTimeSeriesMethodIF methods */

bool vsnMethod_OctVol_info::updateStep(const int stp,
				       const bool force, const bool cascade)
{
  vsnData_OctVol* pData = dynamic_cast<vsnData_OctVol*>(p_refData);
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
