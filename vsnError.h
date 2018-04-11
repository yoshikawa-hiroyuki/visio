//
// vsnError
//
#ifndef _VSN_ERROR_H_
#define _VSN_ERROR_H_

#define VSN_ERR_WXDLG

#if defined(VSN_ERR_WXDLG)
#include "wx/defs.h"
#include "wx/msgdlg.h"
#endif

#include "vsnApp.h"
#include <iostream>
#include <string>


namespace VSN {
  enum MsgLevel {MsgINFO =0, MsgWARN =1, MsgERR =2, MsgFATAL =3};

  extern std::ostream* g_pOutStream;
  extern bool          g_useOutStream;


  inline void SetOutStream(std::ostream* os) {
    g_pOutStream = os;
  }
  inline std::ostream* GetOutStream() {
    return g_pOutStream;
  }

  inline void SetUseOutStream(const bool uosm) {
#if defined(VSN_ERR_WXDLG)
    g_useOutStream = uosm;
#else
    g_useOutStream = true; // always use OutStream
#endif
  }
  inline bool GetUseOutStream() {
    return g_useOutStream;
  }

  inline void ErrMsg(const MsgLevel l, std::string msg) {
    const char* _LV_STR[] = {"Notice", "Warning", "Error", "Fatal"};
    std::string msgbuff = "vsn ";
    msgbuff += _LV_STR[l];
    msgbuff += ":\n ";
    msgbuff += msg;
#if defined(VSN_ERR_WXDLG)
    vsnViewFrame* pvf = NULL;
    if ( ! g_useOutStream )
      pvf = vsnApp::GetApp()->getViewFrame(0);
    if ( pvf ) {
      int icon_style;
      switch ( l ) {
      case MsgINFO: icon_style = wxICON_INFORMATION; break;
      case MsgWARN: icon_style = wxICON_EXCLAMATION; break;
      default: icon_style = wxICON_ERROR;
      } // end of switch(l)
      wxMessageDialog dlg(pvf, vsnApp::ConvSysToWx(msgbuff),
			  vsnApp::ConvSysToWx(_LV_STR[l]),
			  wxOK|wxCENTRE|icon_style);
      dlg.ShowModal();
    } else
#endif
      (*g_pOutStream) << msgbuff << std::endl << std::flush;
  }
};

#endif // _VSN_ERROR_H_
