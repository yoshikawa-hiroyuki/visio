//
// vsnViewFrameWinDlg
//
#ifndef _VSN_VIEW_FRAME_WIN_DLG_H_
#define _VSN_VIEW_FRAME_WIN_DLG_H_

#include "wx/defs.h"
#include "wx/dialog.h"
#include "wx/textctrl.h"
#include "wx/button.h"
#include "wx/radiobox.h"
#include "wx/statline.h"

namespace VSN {
  // menu ids
  enum {ViewFrameWinGeomDlg_XTxt =1150,
	ViewFrameWinGeomDlg_YTxt,
	ViewFrameWinGeomDlg_WTxt,
	ViewFrameWinGeomDlg_HTxt,
	ViewFrameWinGeomDlg_CloseBtn,

	ViewFrameWinLayoutDlg_MTxt,
	ViewFrameWinLayoutDlg_NTxt,
	ViewFrameWinLayoutDlg_DirRadio,
	ViewFrameWinLayoutDlg_X0Txt,
	ViewFrameWinLayoutDlg_Y0Txt,
	ViewFrameWinLayoutDlg_X1Txt,
	ViewFrameWinLayoutDlg_Y1Txt,
	ViewFrameWinLayoutDlg_OkBtn,
	ViewFrameWinLayoutDlg_CancelBtn,
	ViewFrameWinLayoutDlg_ResetBtn
  };
};

class vsnViewFrame;


//----------------------------------------------------------------
// class vsnViewFrameWinGeomDlg
//----------------------------------------------------------------
class vsnViewFrameWinGeomDlg : public wxDialog {
public:
  vsnViewFrameWinGeomDlg(vsnViewFrame* parent);
  virtual ~vsnViewFrameWinGeomDlg();

  /* interface */
  bool update();

  /* event handler */
  void OnChangeValues(wxCommandEvent& event);
  void OnCloseBtn(wxCommandEvent& event);

private:
  wxTextCtrl *m_pXTxt, *m_pYTxt;
  wxTextCtrl *m_pWTxt, *m_pHTxt;
  wxButton   *m_pCloseBtn;

  DECLARE_EVENT_TABLE()
};


//----------------------------------------------------------------
// class vsnViewFrameWinLayoutDlg
//----------------------------------------------------------------
class vsnViewFrameWinLayoutDlg : public wxDialog {
public:
  enum LDirType {LDIR_HORIZONTAL =0, LDIR_VERTICAL =1};

  vsnViewFrameWinLayoutDlg(vsnViewFrame* parent);
  virtual ~vsnViewFrameWinLayoutDlg();

  /* interface */
  bool update();

  /* event handler */
  void OnOkBtn(wxCommandEvent& event);
  void OnCancelBtn(wxCommandEvent& event);
  void OnResetBtn(wxCommandEvent& event);

private:
  wxTextCtrl *m_pMTxt,  *m_pNTxt;
  wxTextCtrl *m_pX0Txt, *m_pY0Txt;
  wxTextCtrl *m_pX1Txt, *m_pY1Txt;
  wxRadioBox *m_pDirRadio;
  wxButton   *m_pOkBtn, *m_pCancelBtn, *m_pResetBtn;

  static size_t s_M, s_N;
  static size_t s_X0, s_Y0, s_X1, s_Y1;
  static LDirType s_LDir;

  DECLARE_EVENT_TABLE()
};

#endif // _VSN_VIEW_FRAME_WIN_DLG_H_
