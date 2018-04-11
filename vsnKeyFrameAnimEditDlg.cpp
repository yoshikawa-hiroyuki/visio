//
// vsnKeyFrameAnimEditDlg
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
#include "wx/statline.h"

#include "vsnKeyFrameAnimEditDlg.h"
#include "vsnMethod_keyFrameAnim.h"

using namespace VSN;


//----------------------------------------------------------------
// class vsnKeyFrameAnimEditDlg
//----------------------------------------------------------------

BEGIN_EVENT_TABLE(vsnKeyFrameAnimEditDlg, wxDialog)
EVT_LISTBOX(KFA_EditDlg_KeyframeLst, vsnKeyFrameAnimEditDlg::OnKeyframeLst)
EVT_LISTBOX_DCLICK(KFA_EditDlg_KeyframeLst,
		   vsnKeyFrameAnimEditDlg::OnKeyframeLst2)
EVT_TEXT_ENTER(KFA_EditDlg_CurrentTxt, vsnKeyFrameAnimEditDlg::OnCurrentTxt)
EVT_TEXT_ENTER(KFA_EditDlg_FpsTxt, vsnKeyFrameAnimEditDlg::OnFpsTxt)
EVT_BUTTON(KFA_EditDlg_AddNewFrameBtn,
	   vsnKeyFrameAnimEditDlg::OnAddNewFrameBtn)
EVT_BUTTON(KFA_EditDlg_OverrideFrameBtn,
	   vsnKeyFrameAnimEditDlg::OnOverrideFrameBtn)
EVT_BUTTON(KFA_EditDlg_DeleteFrameBtn,
	   vsnKeyFrameAnimEditDlg::OnDeleteFrameBtn)
EVT_BUTTON(KFA_EditDlg_GoCurrentBtn, vsnKeyFrameAnimEditDlg::OnGoCurrentBtn)
EVT_BUTTON(KFA_EditDlg_ResetBtn, vsnKeyFrameAnimEditDlg::OnResetBtn)
EVT_CHECKBOX(KFA_EditDlg_WithTimeStepChk,
	     vsnKeyFrameAnimEditDlg::OnWithTimeStepChk)
EVT_CHECKBOX(KFA_EditDlg_PlayerModeChk,
	     vsnKeyFrameAnimEditDlg::OnPlayerModeChk)
EVT_BUTTON(KFA_EditDlg_CloseBtn, vsnKeyFrameAnimEditDlg::OnCloseBtn)
EVT_BUTTON(KFA_EditDlg_RewindBtn, vsnKeyFrameAnimEditDlg::OnRewindBtn)
EVT_BUTTON(KFA_EditDlg_StopBtn, vsnKeyFrameAnimEditDlg::OnStopBtn)
EVT_BUTTON(KFA_EditDlg_PlayBtn, vsnKeyFrameAnimEditDlg::OnPlayBtn)
EVT_BUTTON(KFA_EditDlg_LastBtn, vsnKeyFrameAnimEditDlg::OnLastBtn)
EVT_CHECKBOX(KFA_EditDlg_LoopChk, vsnKeyFrameAnimEditDlg::OnLoopChk)
EVT_CLOSE(vsnKeyFrameAnimEditDlg::OnClose)
END_EVENT_TABLE()


/* constructors, destructor */

vsnKeyFrameAnimEditDlg::vsnKeyFrameAnimEditDlg(vsnFrameBase *parent,
					       vsnMethod_keyFrameAnim* ref)
: vsnFrameRefer(parent, -1, wxString(wxT("Keyframe animation"))), p_ref(ref)
{
  assert(p_ref);

  wxBoxSizer *sizerTop = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *sizerH, *sizerV, *sizerH2;

  sizerH = new wxBoxSizer(wxHORIZONTAL);
  sizerTop->Add(sizerH, 0, wxEXPAND|wxALL, 3);

  // key frames list
  m_pKeyframeLst = new wxListBox(this, KFA_EditDlg_KeyframeLst,
				 wxDefaultPosition, wxSize(200,150));
  sizerH->Add(m_pKeyframeLst, 1, wxEXPAND|wxALIGN_LEFT|wxALL, 3);

  sizerV = new wxBoxSizer(wxVERTICAL);
  sizerH->Add(sizerV, 0, wxEXPAND|wxALIGN_LEFT|wxALL, 0);

  // current time
  sizerH2 = new wxBoxSizer(wxHORIZONTAL);
  sizerV->Add(sizerH2, 0, wxEXPAND|wxALIGN_LEFT|wxALL, 0);
  sizerH2->Add(new wxStaticText(this, -1, wxT("Current Time")),
	       0, wxALIGN_LEFT|wxALL, 3);
  m_pCurrentTxt = new wxTextCtrl(this, KFA_EditDlg_CurrentTxt, wxT(""),
				 wxDefaultPosition, wxDefaultSize,
				 wxTE_PROCESS_ENTER);
  sizerH2->Add(m_pCurrentTxt, 1, wxEXPAND|wxALL, 3);

  // edit buttons
  m_pGoCurrentBtn = new wxButton(this, KFA_EditDlg_GoCurrentBtn,
				 wxT("Go to Current Time"));
  sizerV->Add(m_pGoCurrentBtn, 0, wxEXPAND|wxALL, 5);

  m_pAddNewFrameBtn = new wxButton(this, KFA_EditDlg_AddNewFrameBtn,
				   wxT("Add New Keyframe"));
  sizerV->Add(m_pAddNewFrameBtn, 0, wxEXPAND|wxALL, 5);

  m_pOverrideFrameBtn = new wxButton(this, KFA_EditDlg_OverrideFrameBtn,
				     wxT("Override Keyframe"));
  sizerV->Add(m_pOverrideFrameBtn, 0, wxEXPAND|wxALL, 5);

  m_pDeleteFrameBtn = new wxButton(this, KFA_EditDlg_DeleteFrameBtn,
				   wxT("Delete Keyframe"));
  sizerV->Add(m_pDeleteFrameBtn, 0, wxEXPAND|wxALL, 5);

  // FPS
  sizerH2 = new wxBoxSizer(wxHORIZONTAL);
  sizerV->Add(sizerH2, 0, wxEXPAND|wxALIGN_LEFT|wxALL, 0);
  m_pLoopChk = new wxCheckBox(this, KFA_EditDlg_LoopChk, wxT("loop  "));
  sizerH2->Add(m_pLoopChk, 0, wxEXPAND|wxALIGN_BOTTOM|wxALL, 3);
  sizerH2->Add(new wxStaticText(this, -1, wxT("FPS")),
	       0, wxALIGN_LEFT|wxALIGN_BOTTOM|wxALL, 3);
  m_pFpsTxt = new wxTextCtrl(this, KFA_EditDlg_FpsTxt, wxT(""),
				 wxDefaultPosition, wxDefaultSize,
				 wxTE_PROCESS_ENTER);
  sizerH2->Add(m_pFpsTxt, 1, wxEXPAND|wxALL, 3);

  // checks
  sizerH2 = new wxBoxSizer(wxHORIZONTAL);
  sizerV->Add(sizerH2, 0, wxEXPAND|wxALIGN_LEFT|wxALL, 0);
  m_pWithTimeStepChk = new wxCheckBox(this, KFA_EditDlg_WithTimeStepChk,
				      wxT("animate with time step"));
  sizerH2->Add(m_pWithTimeStepChk, 0, wxEXPAND|wxALL, 3);

  sizerH2 = new wxBoxSizer(wxHORIZONTAL);
  sizerV->Add(sizerH2, 0, wxEXPAND|wxALIGN_LEFT|wxALL, 0);
  m_pPlayerModeChk = new wxCheckBox(this, KFA_EditDlg_PlayerModeChk,
				      wxT("player mode"));
  sizerH2->Add(m_pPlayerModeChk, 0, wxEXPAND|wxALL, 3);

  // control buttons
  sizerH2 = new wxBoxSizer(wxHORIZONTAL);
  sizerV->Add(sizerH2, 0, wxEXPAND|wxALIGN_RIGHT|wxALL, 0);
  sizerH2->Add(5, 0, 1, wxEXPAND);
  m_pRewindBtn = new wxButton(this, KFA_EditDlg_RewindBtn, wxT("|<"),
                              wxDefaultPosition,wxSize(30,-1), wxBU_EXACTFIT);
  sizerH2->Add(m_pRewindBtn, 0, wxALL, 3);
  m_pStopBtn = new wxButton(this, KFA_EditDlg_StopBtn, wxT("[] stop"),
                            wxDefaultPosition,wxSize(60,-1),wxBU_EXACTFIT);
  sizerH2->Add(m_pStopBtn, 0, wxALL, 3);
  m_pPlayBtn = new wxButton(this, KFA_EditDlg_PlayBtn, wxT("> play"),
                            wxDefaultPosition,wxSize(60,-1),wxBU_EXACTFIT);
  sizerH2->Add(m_pPlayBtn, 0, wxALL, 3);
  m_pLastBtn = new wxButton(this, KFA_EditDlg_LastBtn, wxT(">|"),
                            wxDefaultPosition,wxSize(30,-1), wxBU_EXACTFIT);
  sizerH2->Add(m_pLastBtn, 0, wxALL, 3);

  // reset/close buttons
  sizerTop->Add(new wxStaticLine(this, -1, wxDefaultPosition, wxSize(3,3),
				 wxHORIZONTAL), 0, wxEXPAND|wxALL, 0);
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  sizerTop->Add(sizerH, 0, wxALIGN_RIGHT|wxALL, 5);
  m_pResetBtn = new wxButton(this, KFA_EditDlg_ResetBtn, wxT("Reset"));
  sizerH->Add(m_pResetBtn, 0, wxALIGN_RIGHT|wxALL, 3);
  m_pCloseBtn = new wxButton(this, KFA_EditDlg_CloseBtn, wxT("Close"));
  sizerH->Add(m_pCloseBtn, 0, wxALIGN_RIGHT|wxALL, 3);

  // post process
  SetAutoLayout(TRUE);
  SetSizer(sizerTop);
  sizerTop->SetSizeHints(this);
  sizerTop->Fit(this);

  (void)update();
}

vsnKeyFrameAnimEditDlg::~vsnKeyFrameAnimEditDlg() {
  if ( p_ref ) {
    p_ref->clearEditDlg();
    p_ref = NULL;
  }
}


/* interface */

bool vsnKeyFrameAnimEditDlg::update() {
  if ( ! p_ref ) return false;
  vsnKeyFrameAnim& kfa = p_ref->getKFA();
  char buff[64];

  // key-frame list
  m_pKeyframeLst->Clear();
  const std::map<double, vsnAnimFrame>& kfLst = kfa.getFrameList();
  std::map<double, vsnAnimFrame>::const_iterator it;
  for ( it = kfLst.begin(); it != kfLst.end(); it++ ) {
    sprintf(buff, "%12.6f [%8d]", it->first, it->second.step);
    m_pKeyframeLst->Append(vsnApp::ConvSysToWx(buff));
  } // end of for(it)

  // current time
  sprintf(buff, "%.6f", p_ref->getCurrentTime());
  m_pCurrentTxt->SetValue(vsnApp::ConvSysToWx(buff));

  // fps
  sprintf(buff, "%d", kfa.getFps());
  m_pFpsTxt->SetValue(vsnApp::ConvSysToWx(buff));

  // with time-step
  m_pWithTimeStepChk->SetValue(kfa.getStepMode());

  // player mode
  m_pPlayerModeChk->SetValue(p_ref->getPlayerMode());

  // loop
  m_pLoopChk->SetValue(kfa.getLoopMode());

  return true;
}


/* event handler */

void vsnKeyFrameAnimEditDlg::OnKeyframeLst(wxCommandEvent& event) {
  if ( ! p_ref ) return;
  vsnKeyFrameAnim& kfa = p_ref->getKFA();
  const std::map<double, vsnAnimFrame>& kfLst = kfa.getFrameList();

  int sel = m_pKeyframeLst->GetSelection();
  if ( sel == wxNOT_FOUND ) {
    update();
    return;
  }
  int i;
  std::map<double, vsnAnimFrame>::const_iterator it;
  for ( i = 0, it = kfLst.begin(); i != sel && it != kfLst.end(); i++, it++ );
  if ( it == kfLst.end() ) {
    update();
    return;
  }

  char buff[64];
  sprintf(buff, "%.6f", it->first);
  m_pCurrentTxt->SetValue(vsnApp::ConvSysToWx(buff));
}

void vsnKeyFrameAnimEditDlg::OnKeyframeLst2(wxCommandEvent& event) {
  if ( ! p_ref ) return;
  vsnKeyFrameAnim& kfa = p_ref->getKFA();
  const std::map<double, vsnAnimFrame>& kfLst = kfa.getFrameList();

  int sel = m_pKeyframeLst->GetSelection();
  if ( sel == wxNOT_FOUND ) {
    update();
    return;
  }
  int i;
  std::map<double, vsnAnimFrame>::const_iterator it;
  for ( i = 0, it = kfLst.begin(); i != sel && it != kfLst.end(); i++, it++ );
  if ( it == kfLst.end() ) {
    update();
    return;
  }
  double atm = it->first;
  if ( ! p_ref->setCurrentTime(atm) ) {
     char buff[64];
     sprintf(buff, "can't set current time to %.6f", atm);
     ErrMsg(MsgERR, string("Keyframe animation: ") + buff);
     return;
  }
  p_ref->chkNotice();
}

void vsnKeyFrameAnimEditDlg::OnCurrentTxt(wxCommandEvent& event) {
  // nothing to do.
}

void vsnKeyFrameAnimEditDlg::OnFpsTxt(wxCommandEvent& event) {
  if ( ! p_ref ) return;
  vsnKeyFrameAnim& kfa = p_ref->getKFA();

  wxString vstr = m_pFpsTxt->GetValue();
  if ( vstr.IsEmpty() ) {
    update();
    return;
  }
  int val = atoi(vsnApp::ConvWxToSys(vstr).c_str());
  if ( val < 1 ) {
    ErrMsg(MsgERR, string("Keyframe animation: invalid FPS specified."));
    update();
    return;
  }

  if ( ! kfa.setFps(val) ) {
    ErrMsg(MsgERR, string("Keyframe animation: can't set fps."));
    update();
    return;
  }
}

void vsnKeyFrameAnimEditDlg::OnAddNewFrameBtn(wxCommandEvent& event) {
  if ( ! p_ref ) return;
  vsnKeyFrameAnim& kfa = p_ref->getKFA();
  const std::map<double, vsnAnimFrame>& kfLst = kfa.getFrameList();
  char buff[64];

  wxString vstr = m_pCurrentTxt->GetValue();
  if ( vstr.IsEmpty() ) {
    update();
    return;
  }
  double atm =
    (double)((int)(atof(vsnApp::ConvWxToSys(vstr).c_str()) * 1e6) * 1e-6);
  if ( ! p_ref->addKeyframe(atm) ) {
#if 0
    sprintf(buff, "can't add keyframe at %.6f", atm);
    ErrMsg(MsgERR, string("Keyframe animation: ") + buff);
#endif
    return;
  }
  update();

  // update current time
  std::map<double, vsnAnimFrame>::const_iterator it = kfLst.end();
  it --; // something added, it must be ok.
  sprintf(buff, "%.6f", it->first + 1.0);
  m_pCurrentTxt->SetValue(vsnApp::ConvSysToWx(buff));
}

void vsnKeyFrameAnimEditDlg::OnOverrideFrameBtn(wxCommandEvent& event) {
  if ( ! p_ref ) return;
  vsnKeyFrameAnim& kfa = p_ref->getKFA();
  char buff[64];

  wxString vstr = m_pCurrentTxt->GetValue();
  if ( vstr.IsEmpty() ) {
    update();
    return;
  }
  double atm =
    (double)((int)(atof(vsnApp::ConvWxToSys(vstr).c_str()) * 1e6) * 1e-6);
  
  size_t i, nkf = kfa.getNumKeyframes();
  int idx = -1; double tm; vsnAnimFrame AF;
  for ( i = 0; i < nkf; i++ ) {
    kfa.getKeyframe(i, tm, AF);
    if ( fabs(atm - tm) < 1e-6 ) {idx = i; break;}
  } // end of for(i)
  if ( idx < 0 || ! p_ref->delKeyframe((size_t)idx) ) {
    sprintf(buff, "can't find keyframe at %.6f", atm);
    ErrMsg(MsgERR, string("Keyframe animation: ") + buff);
    return;
  }
  if ( ! p_ref->addKeyframe(atm) ) {
    sprintf(buff, "can't replace keyframe at %.6f", atm);
    ErrMsg(MsgERR, string("Keyframe animation: ") + buff);
    return;
  }
  update();
}

void vsnKeyFrameAnimEditDlg::OnDeleteFrameBtn(wxCommandEvent& event) {
  if ( ! p_ref ) return;
  vsnKeyFrameAnim& kfa = p_ref->getKFA();
  char buff[64];

  wxString vstr = m_pCurrentTxt->GetValue();
  if ( vstr.IsEmpty() ) {
    update();
    return;
  }
  double atm =
    (double)((int)(atof(vsnApp::ConvWxToSys(vstr).c_str()) * 1e6) * 1e-6);

  // find keyframe
  size_t i, nkf = kfa.getNumKeyframes();
  int idx = -1; double tm; vsnAnimFrame AF;
  for ( i = 0; i < nkf; i++ ) {
    kfa.getKeyframe(i, tm, AF);
    if ( fabs(atm - tm) < 1e-6 ) {idx = i; break;}
  } // end of for(i)
  if ( idx < 0 ) {
    sprintf(buff, "can't find keyframe at %.6f", atm);
    ErrMsg(MsgERR, string("Keyframe animation: ") + buff);
    return;
  }

  // confirm
  sprintf(buff, "Are you sure to delete keyframe at %.6f ?", atm);
  wxMessageDialog dlg(NULL, vsnApp::ConvSysToWx(buff),
		      wxT("Keyframe animation: Delete"),
		      vsn_wxOK_CANCEL|wxICON_QUESTION);
  if ( dlg.ShowModal() != vsn_wxIDOK )
    return;

  // delete keyframe
  if ( ! p_ref->delKeyframe((size_t)idx) ) {
    sprintf(buff, "can't delete keyframe at %.6f", atm);
    ErrMsg(MsgERR, string("Keyframe animation: ") + buff);
    return;
  }
  update();
}

void vsnKeyFrameAnimEditDlg::OnGoCurrentBtn(wxCommandEvent& event) {
  if ( ! p_ref ) return;
  vsnKeyFrameAnim& kfa = p_ref->getKFA();
  char buff[64];

  wxString vstr = m_pCurrentTxt->GetValue();
  if ( vstr.IsEmpty() ) {
    update();
    return;
  }
  double atm =
    (double)((int)(atof(vsnApp::ConvWxToSys(vstr).c_str()) * 1e6) * 1e-6);

  if ( atm < kfa.getInitialTime() ||
       atm > kfa.getInitialTime() + kfa.getTotalTime() ) {
     sprintf(buff, "can't set current time to %.6f,\n out of duration.", atm);
     ErrMsg(MsgERR, string("Keyframe animation: ") + buff);
     return;
  }

  if ( ! p_ref->setCurrentTime(atm) ) {
#if 0
     sprintf(buff, "can't set current time to %.6f", atm);
     ErrMsg(MsgERR, string("Keyframe animation: ") + buff);
#endif
     return;
  }
  p_ref->chkNotice();
}

void vsnKeyFrameAnimEditDlg::OnResetBtn(wxCommandEvent& event) {
  if ( ! p_ref ) return;
  vsnKeyFrameAnim& kfa = p_ref->getKFA();
  size_t nkf = kfa.getNumKeyframes();
  if ( nkf > 0 ) {
    wxMessageDialog dlg(NULL, wxT("Are you sure to clear all key frame(s) ?"),
			wxT("Keyframe animation: Reset"),
			vsn_wxOK_CANCEL|wxICON_QUESTION);
    if ( dlg.ShowModal() != vsn_wxIDOK )
      return;
  }
  p_ref->reset();
  update();
}

void vsnKeyFrameAnimEditDlg::OnWithTimeStepChk(wxCommandEvent& event) {
  if ( ! p_ref ) return;
  vsnKeyFrameAnim& kfa = p_ref->getKFA();

  bool val = m_pWithTimeStepChk->GetValue();
  kfa.setStepMode(val);
  p_ref->updateUI();
}

void vsnKeyFrameAnimEditDlg::OnPlayerModeChk(wxCommandEvent& event) {
  if ( ! p_ref ) return;
  bool val = m_pPlayerModeChk->GetValue();
  p_ref->setPlayerMode(val);
}

void vsnKeyFrameAnimEditDlg::OnRewindBtn(wxCommandEvent& event) {
  if ( ! p_ref ) return;
  if ( ! p_ref->setCurrentTime(p_ref->getKFA().getInitialTime()) )
    return;
  p_ref->chkNotice();
  p_ref->updateUI();
}

void vsnKeyFrameAnimEditDlg::OnStopBtn(wxCommandEvent& event) {
  if ( ! p_ref ) return;
  p_ref->stop();
  p_ref->updateUI();
}

void vsnKeyFrameAnimEditDlg::OnPlayBtn(wxCommandEvent& event) {
  if ( ! p_ref ) return;
  p_ref->play(false);
  p_ref->updateUI();
}

void vsnKeyFrameAnimEditDlg::OnLastBtn(wxCommandEvent& event) {
  if ( ! p_ref ) return;
  p_ref->setCurrentTime(p_ref->getKFA().getInitialTime() +
			p_ref->getKFA().getTotalTime());
  p_ref->chkNotice();
  p_ref->updateUI();
}

void vsnKeyFrameAnimEditDlg::OnLoopChk(wxCommandEvent& event) {
  if ( ! p_ref ) return;
  bool value = m_pLoopChk->GetValue();
  p_ref->getKFA().setLoopMode(value);
  p_ref->updateUI();
}

void vsnKeyFrameAnimEditDlg::OnCloseBtn(wxCommandEvent& event) {
  if ( IsModal() )
    EndModal(vsn_wxIDOK);
  else
    this->Hide();
}

void vsnKeyFrameAnimEditDlg::OnClose(wxCloseEvent& event) {
}
