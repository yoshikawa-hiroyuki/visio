//
// vsnMethod_Scatter_info
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

#include "vsnMethod_Scatter_info.h"
#include "vsnData_Scatter.h"
#include "vsnError.h"

using namespace std;
using namespace CES;


//----------------------------------------------------------------
// class vsnMethod_Scatter_info
//----------------------------------------------------------------

/* constructors / destructor */

vsnMethod_Scatter_info::vsnMethod_Scatter_info(const string& name)
  : vsnMethod_info(name)
{
}

vsnMethod_Scatter_info::~vsnMethod_Scatter_info() {
}


/* vsnMethod_info methods */

bool vsnMethod_Scatter_info::updateRepStr() {
  vsnData_Scatter* pdScatter = dynamic_cast<vsnData_Scatter*>(p_refData);
  if ( ! pdScatter ) {
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
  if ( BaseName(pdScatter->getFilePath(), string(""), dc) != VSN::vsn_seqfile )
    msg += pdScatter->getFilePath() + "\n";
  else {
    msg += "sequencial\n";
    deque<string> path_lst = pdScatter->getSeqFilePathes();
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

  size_t nstp = pdScatter->getNumSteps();
  int stp0 = -1, stp1 = -1;
  (void)pdScatter->getTimeStepNo(0, stp0);
  (void)pdScatter->getTimeStepNo(nstp -1, stp1);
  sprintf(txt, " Steps = %lu [%d : %d]\n", nstp, stp0, stp1);
  msg += txt;
  float tms0 = pdScatter->getTime(0);
  float tms1 = pdScatter->getTime(nstp -1);
  sprintf(txt, " Time = [%g : %g]\n", tms0, tms1);
  msg += txt;

  size_t curStp = pdScatter->getCurrentStepIdx();
  sprintf(txt, " Current-Step = %lu\n", curStp);
  msg += txt;
  sprintf(txt, " Current-Time = %g\n\n", pdScatter->getTime(curStp));
  msg += txt;

  const Vec3<float>* pbb = pdScatter->getBbox();
  sprintf(txt, " Bbox\n  x = [%.3g : %.3g]\n", pbb[0][0], pbb[1][0]);
  msg += txt;
  sprintf(txt, "  y = [%.3g : %.3g]\n", pbb[0][1], pbb[1][1]);
  msg += txt;
  sprintf(txt, "  z = [%.3g : %.3g]\n", pbb[0][2], pbb[1][2]);
  msg += txt;

  size_t mnPt = pdScatter->getMaxNumPts();
  sprintf(txt, " Max #of points: %lu\n", mnPt);
  msg += txt;

  size_t dataLen = pdScatter->getDataLen();
  sprintf(txt, " Data Length = %lu\n", dataLen);
  msg += txt;
  if ( dataLen > 0 ) {
    size_t d; float minmax[2];
    for ( d = 0; d < dataLen; d++ ) {
      pdScatter->getMinMax(d, minmax);
      sprintf(txt, " Data%lu\n", d); msg += txt;
      sprintf(txt, "  min = %g\n  max = %g\n", minmax[0], minmax[1]);
      msg += txt;
    } // end of for(d)

    if ( pdScatter->getVectorMaxLen(CES::Vec3<int>(0,1,2), minmax[1]) ) {
      sprintf(txt, " Max vector length = %g\n", minmax[1]);
      msg += txt;
    }
  } // end of if(dataLen>0)

  return true;
}


/* vsnMethodObj methods */

bool vsnMethod_Scatter_info::update(const bool force) {
  return updateStep(m_requestedStp, force);
}


/* vsnTimeSeriesMethodIF methods */

bool vsnMethod_Scatter_info::updateStep(const int stp,
					const bool force, const bool cascade)
{
  vsnData_Scatter* pData = dynamic_cast<vsnData_Scatter*>(p_refData);
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
