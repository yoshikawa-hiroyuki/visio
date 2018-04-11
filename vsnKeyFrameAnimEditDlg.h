//
// vsnKeyFrameAnimEditDlg
//
#ifndef _VSN_KEYFRAMEANIM_EDITDLG_H_
#define _VSN_KEYFRAMEANIM_EDITDLG_H_

#include "wx/defs.h"
#include "wx/dialog.h"
#include "wx/listbox.h"
#include "wx/textctrl.h"
#include "wx/button.h"

#include "vsnFrameBase.h"


//----------------------------------------------------------------
namespace VSN {
  // control ids
  enum {KFA_EditDlg_KeyframeLst = 2650,
	KFA_EditDlg_CurrentTxt,
	KFA_EditDlg_FpsTxt,
	KFA_EditDlg_AddNewFrameBtn,
	KFA_EditDlg_OverrideFrameBtn,
	KFA_EditDlg_DeleteFrameBtn,
	KFA_EditDlg_GoCurrentBtn,
	KFA_EditDlg_ResetBtn,
	KFA_EditDlg_WithTimeStepChk,
	KFA_EditDlg_PlayerModeChk,
	KFA_EditDlg_RewindBtn,
	KFA_EditDlg_StopBtn,
	KFA_EditDlg_PlayBtn,
	KFA_EditDlg_LastBtn,
	KFA_EditDlg_LoopChk,
	KFA_EditDlg_CloseBtn
  };
};


//----------------------------------------------------------------
// class vsnKeyFrameAnimEditDlg
//----------------------------------------------------------------
class vsnKeyFrameAnimEditDlg : public vsnFrameRefer
{
public:
  vsnKeyFrameAnimEditDlg(vsnFrameBase *parent,
			 class vsnMethod_keyFrameAnim* ref);
  virtual ~vsnKeyFrameAnimEditDlg();

  /* interface */
  bool update();

  /* event handler */
  void OnKeyframeLst(wxCommandEvent& event);
  void OnKeyframeLst2(wxCommandEvent& event);
  void OnCurrentTxt(wxCommandEvent& event);
  void OnFpsTxt(wxCommandEvent& event);
  void OnAddNewFrameBtn(wxCommandEvent& event);
  void OnOverrideFrameBtn(wxCommandEvent& event);
  void OnDeleteFrameBtn(wxCommandEvent& event);
  void OnGoCurrentBtn(wxCommandEvent& event);
  void OnResetBtn(wxCommandEvent& event);
  void OnWithTimeStepChk(wxCommandEvent& event);
  void OnPlayerModeChk(wxCommandEvent& event);
  void OnRewindBtn(wxCommandEvent& event);
  void OnStopBtn(wxCommandEvent& event);
  void OnPlayBtn(wxCommandEvent& event);
  void OnLastBtn(wxCommandEvent& event);
  void OnLoopChk(wxCommandEvent& event);
  void OnCloseBtn(wxCommandEvent& event);
  void OnClose(wxCloseEvent& event);

private:
  class vsnMethod_keyFrameAnim* p_ref;

  wxListBox*      m_pKeyframeLst;
  wxTextCtrl*     m_pCurrentTxt;
  wxTextCtrl*     m_pFpsTxt;
  wxButton*       m_pAddNewFrameBtn;
  wxButton*       m_pOverrideFrameBtn;
  wxButton*       m_pDeleteFrameBtn;
  wxButton*       m_pGoCurrentBtn;
  wxButton*       m_pResetBtn;
  wxCheckBox*     m_pWithTimeStepChk;
  wxCheckBox*     m_pPlayerModeChk;
  wxButton*       m_pRewindBtn;
  wxButton*       m_pStopBtn;
  wxButton*       m_pPlayBtn;
  wxButton*       m_pLastBtn;
  wxCheckBox*     m_pLoopChk;
  wxButton*       m_pCloseBtn;

  DECLARE_EVENT_TABLE()
};

#endif // _VSN_KEYFRAMEANIM_EDITDLG_H_
