//
// vsnMethod_keyFrameAnim
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
#include "wx/utils.h"
#include "wx/filename.h"

#include "vsnMethod_keyFrameAnim.h"
#include "vsnKeyFrameAnimEditDlg.h"
#include "vsnDataObj.h"
#include "vsnError.h"
#include <sstream>

#define ANIM_INTERVAL 150 // 0.15 sec.
#define ANIM_INTERVAL2 30 // 0.03 sec.

using namespace std;
using namespace CES;
using namespace VSN;


//----------------------------------------------------------------
// class vsnMPP_keyFrameAnim
//----------------------------------------------------------------

BEGIN_EVENT_TABLE(vsnMPP_keyFrameAnim, wxPanel)
  EVT_BUTTON(MPP_keyFrameAnim_RewindBtn, vsnMPP_keyFrameAnim::OnRewindBtn)
  EVT_BUTTON(MPP_keyFrameAnim_StopBtn, vsnMPP_keyFrameAnim::OnStopBtn)
  EVT_BUTTON(MPP_keyFrameAnim_PlayBtn, vsnMPP_keyFrameAnim::OnPlayBtn)
  EVT_BUTTON(MPP_keyFrameAnim_LastBtn, vsnMPP_keyFrameAnim::OnLastBtn)
  EVT_CHECKBOX(MPP_keyFrameAnim_LoopChk, vsnMPP_keyFrameAnim::OnLoopChk)
  EVT_CHECKBOX(MPP_keyFrameAnim_WithTimeStepChk,
	       vsnMPP_keyFrameAnim::OnWithTimeStepChk)
  EVT_CHECKBOX(MPP_keyFrameAnim_PlayerModeChk,
	       vsnMPP_keyFrameAnim::OnPlayerModeChk)
  EVT_CHECKBOX(MPP_keyFrameAnim_SshotChk, vsnMPP_keyFrameAnim::OnSshotChk)
  EVT_TEXT_ENTER(MPP_keyFrameAnim_SshotFileTxt,
		 vsnMPP_keyFrameAnim::OnSshotFileTxt)
  EVT_BUTTON(MPP_keyFrameAnim_SshotFileBtn,
	     vsnMPP_keyFrameAnim::OnSshotFileBtn)
  EVT_TEXT_ENTER(MPP_keyFrameAnim_TargetViewTxt,
		 vsnMPP_keyFrameAnim::OnTargetViewTxt)
  EVT_BUTTON(MPP_keyFrameAnim_TargetViewBtn,
	     vsnMPP_keyFrameAnim::OnTargetViewBtn)
  EVT_BUTTON(MPP_keyFrameAnim_EditBtn, vsnMPP_keyFrameAnim::OnEditBtn)
END_EVENT_TABLE()


/* constructors / destructor */

vsnMPP_keyFrameAnim::vsnMPP_keyFrameAnim(wxPanel* parent, vsnMethodObj* pm)
: vsnMethodPP(parent, pm)
{
  assert(parent);
  vsnMethod_keyFrameAnim* pmkfa = dynamic_cast<vsnMethod_keyFrameAnim*>(pm);
  assert(pmkfa);

  // widgets layout
  wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer* sizerH;

  // edit button
  m_pEditBtn = new wxButton(this, MPP_keyFrameAnim_EditBtn,
			    wxT("edit keyframes"));
  assert(m_pEditBtn);
  topsizer->Add(m_pEditBtn, 0, wxEXPAND|wxALL, 3);

  // control buttons, loop check
  topsizer->Add(new wxStaticLine(this, -1, wxDefaultPosition, wxSize(5,5),
				 wxHORIZONTAL), 0, wxEXPAND|wxALL, 0);
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND|wxALL, 0);
  m_pRewindBtn = new wxButton(this, MPP_keyFrameAnim_RewindBtn, wxT("|<"),
			      wxDefaultPosition,wxSize(30,-1), wxBU_EXACTFIT);
  assert(m_pRewindBtn);
  sizerH->Add(m_pRewindBtn, 0, wxALL, 3);
  m_pStopBtn = new wxButton(this, MPP_keyFrameAnim_StopBtn, wxT("[] stop"),
			    wxDefaultPosition,wxSize(60,-1),wxBU_EXACTFIT);
  assert(m_pStopBtn);
  sizerH->Add(m_pStopBtn, 0, wxALL, 3);
  m_pPlayBtn = new wxButton(this, MPP_keyFrameAnim_PlayBtn, wxT("> play"),
			    wxDefaultPosition,wxSize(60,-1),wxBU_EXACTFIT);
  assert(m_pPlayBtn);
  sizerH->Add(m_pPlayBtn, 0, wxALL, 3);
  m_pLastBtn = new wxButton(this, MPP_keyFrameAnim_LastBtn, wxT(">|"),
			    wxDefaultPosition,wxSize(30,-1), wxBU_EXACTFIT);
  assert(m_pLastBtn);
  sizerH->Add(m_pLastBtn, 0, wxALL, 3);

  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND|wxALL, 0);
  m_pLoopChk = new wxCheckBox(this, MPP_keyFrameAnim_LoopChk, wxT("loop"));
  assert(m_pLoopChk);
  sizerH->Add(m_pLoopChk, 0, wxEXPAND|wxALL, 3);

  // with time step, player mode checks
  //sizerH = new wxBoxSizer(wxHORIZONTAL);
  //topsizer->Add(sizerH, 0, wxEXPAND|wxALL, 0);
  m_pWithTimeStepChk = new wxCheckBox(this, MPP_keyFrameAnim_WithTimeStepChk,
				      wxT("with time step"));
  assert(m_pWithTimeStepChk);
  sizerH->Add(m_pWithTimeStepChk, 1, wxEXPAND|wxALL, 3);

  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND|wxALL, 0);
  m_pPlayerModeChk = new wxCheckBox(this, MPP_keyFrameAnim_PlayerModeChk,
				      wxT("player mode"));
  assert(m_pPlayerModeChk);
  sizerH->Add(m_pPlayerModeChk, 1, wxEXPAND|wxALL, 3);

  // target view
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND|wxALL, 0);
  sizerH->Add(new wxStaticText(this, -1, wxT("target view")),
              0, wxALIGN_LEFT|wxALL, 3);
  m_pTargetViewTxt = new wxTextCtrl(this, MPP_keyFrameAnim_TargetViewTxt,
				    wxT(""), wxDefaultPosition, wxSize(50,-1),
				    wxTE_PROCESS_ENTER);
  assert(m_pTargetViewTxt);
  sizerH->Add(m_pTargetViewTxt, 1, wxEXPAND|wxALL, 3);
  m_pTargetViewBtn = new wxButton(this, MPP_keyFrameAnim_TargetViewBtn,
                                     wxT("select"), wxDefaultPosition,
                                     wxSize(60,-1), wxBU_EXACTFIT);
  assert(m_pTargetViewBtn);
  sizerH->Add(m_pTargetViewBtn, 0, wxALL, 3);

  // screen shot items
  topsizer->Add(new wxStaticLine(this, -1, wxDefaultPosition, wxSize(5,5),
				 wxHORIZONTAL), 0, wxEXPAND|wxALL, 0);
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND|wxALL, 0);
  m_pSshotChk = new wxCheckBox(this, MPP_keyFrameAnim_SshotChk,
			       wxT("screen shot"));
  assert(m_pSshotChk);
  sizerH->Add(m_pSshotChk, 1, wxEXPAND|wxALL, 3);

  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND|wxALL, 0);
  sizerH->Add(new wxStaticText(this, -1, wxT("file")),
              0, wxALIGN_LEFT|wxALL, 3);
  m_pSshotFileTxt = new wxTextCtrl(this, MPP_keyFrameAnim_SshotFileTxt,
				   wxT(""), wxDefaultPosition, wxSize(60,-1),
				   wxTE_PROCESS_ENTER);
  assert(m_pSshotFileTxt);
  sizerH->Add(m_pSshotFileTxt, 1, wxEXPAND|wxALL, 3);
  m_pSshotFileBtn = new wxButton(this, MPP_keyFrameAnim_SshotFileBtn,
				 wxT("..."), wxDefaultPosition,
				 wxDefaultSize, wxBU_EXACTFIT);
  assert(m_pSshotFileBtn);
  sizerH->Add(m_pSshotFileBtn, 0, wxALL, 3);

  // post process
  SetSizer(topsizer);
  addTo(parent);
  topsizer->Layout();
  (void)update();
}

vsnMPP_keyFrameAnim::~vsnMPP_keyFrameAnim() {
}


/* interface */

bool vsnMPP_keyFrameAnim::update() {
  vsnMethod_keyFrameAnim* pm = dynamic_cast<vsnMethod_keyFrameAnim*>(p_method);
  if ( ! pm ) return false;
  wxString wwks;
  string wks;

  m_pLoopChk->SetValue(pm->getKFA().getLoopMode());
  m_pWithTimeStepChk->SetValue(pm->getKFA().getStepMode());
  m_pPlayerModeChk->SetValue(pm->getPlayerMode());
  m_pSshotChk->SetValue(pm->getShotMode());

  wks = pm->getShotPath();
  if ( wks.empty() ) wwks = wxT("");
  else wwks = vsnApp::ConvSysToWx(wks);
  m_pSshotFileTxt->SetValue(wwks);

  wks = pm->getTargetView();
  if ( wks.empty() ) wwks = wxT("");
  else wwks = vsnApp::ConvSysToWx(wks);
  m_pTargetViewTxt->SetValue(wwks);

  return true;
}


/* event handler */

void vsnMPP_keyFrameAnim::OnRewindBtn(wxCommandEvent& event) {
  vsnMethod_keyFrameAnim* pm = dynamic_cast<vsnMethod_keyFrameAnim*>(p_method);
  if ( ! pm ) return;
  pm->setCurrentTime(pm->getKFA().getInitialTime());
  pm->chkNotice();
  (void)update();
}

void vsnMPP_keyFrameAnim::OnStopBtn(wxCommandEvent& event) {
  vsnMethod_keyFrameAnim* pm = dynamic_cast<vsnMethod_keyFrameAnim*>(p_method);
  if ( ! pm ) return;
  pm->stop();
  (void)update();
}

void vsnMPP_keyFrameAnim::OnPlayBtn(wxCommandEvent& event) {
  vsnMethod_keyFrameAnim* pm = dynamic_cast<vsnMethod_keyFrameAnim*>(p_method);
  if ( ! pm ) return;
  pm->play(false);
  (void)update();
}

void vsnMPP_keyFrameAnim::OnLastBtn(wxCommandEvent& event) {
  vsnMethod_keyFrameAnim* pm = dynamic_cast<vsnMethod_keyFrameAnim*>(p_method);
  if ( ! pm ) return;
  pm->setCurrentTime(pm->getKFA().getInitialTime()
		     + pm->getKFA().getTotalTime());
  pm->chkNotice();
  (void)update();
}

void vsnMPP_keyFrameAnim::OnLoopChk(wxCommandEvent& event) {
  vsnMethod_keyFrameAnim* pm = dynamic_cast<vsnMethod_keyFrameAnim*>(p_method);
  if ( ! pm ) return;
  bool value = m_pLoopChk->GetValue();
  pm->getKFA().setLoopMode(value);
  pm->updateEditDlg();
}

void vsnMPP_keyFrameAnim::OnWithTimeStepChk(wxCommandEvent& event) {
  vsnMethod_keyFrameAnim* pm = dynamic_cast<vsnMethod_keyFrameAnim*>(p_method);
  if ( ! pm ) return;
  bool value = m_pWithTimeStepChk->GetValue();
  pm->getKFA().setStepMode(value);
  pm->updateEditDlg();
}

void vsnMPP_keyFrameAnim::OnPlayerModeChk(wxCommandEvent& event) {
  vsnMethod_keyFrameAnim* pm = dynamic_cast<vsnMethod_keyFrameAnim*>(p_method);
  if ( ! pm ) return;
  bool value = m_pPlayerModeChk->GetValue();
  pm->setPlayerMode(value);
  pm->updateEditDlg();
}

void vsnMPP_keyFrameAnim::OnSshotChk(wxCommandEvent& event) {
  vsnMethod_keyFrameAnim* pm = dynamic_cast<vsnMethod_keyFrameAnim*>(p_method);
  if ( ! pm ) return;
  bool value = m_pSshotChk->GetValue();
  pm->setShotMode(value);
}

void vsnMPP_keyFrameAnim::OnSshotFileTxt(wxCommandEvent& event) {
  vsnMethod_keyFrameAnim* pm = dynamic_cast<vsnMethod_keyFrameAnim*>(p_method);
  if ( ! pm ) return;
  string wks;
  wxString valStr = m_pSshotFileTxt->GetValue();
  if ( ! valStr.IsEmpty() )
    wks = vsnPath_normalize(vsnApp::ConvWxToSys(valStr));
  pm->setShotPath(wks);
}

void vsnMPP_keyFrameAnim::OnSshotFileBtn(wxCommandEvent& event) {
  vsnMethod_keyFrameAnim* pm = dynamic_cast<vsnMethod_keyFrameAnim*>(p_method);
  if ( ! pm ) return;

  wxFileDialog fileDlg(this, wxT("specify file (pattern) to screenshot"),
                       wxT(""), wxT(""), // default Dir / File
                       wxT("MPEG-1 video file (*.mpg)|*.mpg|")
                       wxT("AVI file (*.avi)|*.avi|")
                       wxT("QuickTime file (*.mov)|*.mov|")
                       wxT("PNG files (*.png)|*.png|")
                       wxT("JPEG files (*.jpg)|*.jpg|")
                       wxT("BMP files (*.bmp)|*.bmp|")
                       wxT("(*)|*"), wxFD_SAVE);
  wxString prevPath = m_pSshotFileTxt->GetValue();
  if ( prevPath.IsEmpty() ) {
    wxString xtmpfn = wxFileName::CreateTempFileName(_T("vsn"));
    wxRemoveFile(xtmpfn);
    string tmpdir = DirName(vsnPath_normalize(vsnApp::ConvWxToSys(xtmpfn)),
                            vsnPath_getDelimChar());
    fileDlg.SetDirectory(vsnApp::ConvSysToWx(tmpdir));
  } else {
    string tmpPath = vsnPath_normalize(vsnApp::ConvWxToSys(prevPath));
    string tmpDir = DirName(tmpPath, vsnPath_getDelimChar());
    string tmpFile = BaseName(tmpPath, "", vsnPath_getDelimChar());
    if ( ! tmpDir.empty() ) fileDlg.SetDirectory(vsnApp::ConvSysToWx(tmpDir));
    if ( ! tmpFile.empty() ) fileDlg.SetFilename(vsnApp::ConvSysToWx(tmpFile));
  }

  if ( fileDlg.ShowModal() == wxID_OK ) {
    wxString newPath = fileDlg.GetPath();
    m_pSshotFileTxt->SetValue(newPath);
    if ( ! newPath.IsEmpty() ) {
      string spath = vsnPath_normalize(vsnApp::ConvWxToSys(newPath));
      pm->setShotPath(spath);
    }
  }
}

void vsnMPP_keyFrameAnim::OnTargetViewTxt(wxCommandEvent& event) {
  vsnMethod_keyFrameAnim* pm = dynamic_cast<vsnMethod_keyFrameAnim*>(p_method);
  if ( ! pm ) return;
  string wks;
  wxString valStr = m_pTargetViewTxt->GetValue();
  if ( ! valStr.IsEmpty() ) wks = vsnApp::ConvWxToSys(valStr);
  pm->setTargetView(wks);
}

void vsnMPP_keyFrameAnim::OnTargetViewBtn(wxCommandEvent& event) {
  vsnMethod_keyFrameAnim* pm = dynamic_cast<vsnMethod_keyFrameAnim*>(p_method);
  if ( ! pm ) return;
  vsnApp* pApp = vsnApp::GetApp();
  if ( ! pApp ) return;
  size_t nvf = pApp->getNumViewFrame();
  if ( nvf < 1 ) {
    ErrMsg(MsgERR, "method keyFrameAnim: no ViewFrame");
    return;
  }

  size_t i;
  int preSel = 0, vpcnt = 0;
  wxString* choices = new wxString[nvf];
  for ( i = 0; i < nvf; i++ ) {
    vsnViewFrame* pvf = pApp->getViewFrame(i);
    if ( ! pvf ) continue;
    choices[vpcnt] = vsnApp::ConvSysToWx(pvf->getName());
    if ( pm->getTargetView() == pvf->getName() )
      preSel = (int)vpcnt;
    vpcnt++;
  } // end of for(i)

  wxSingleChoiceDialog
    dlg(this, wxT("select a ViewFrame of screen-shot target"),
        wxT("select ViewFrame"), vpcnt, choices);
  delete [] choices;
  dlg.SetSelection(preSel);
  if ( dlg.ShowModal() != wxID_OK ) return;

  int val = dlg.GetSelection();
  if ( val < 0 || val >= vpcnt ) {
    ErrMsg(MsgERR, "method keyFrameAnim: invalid selection of ViewFrame");
    return;
  }
  wxString valStr = dlg.GetStringSelection();
  vsnViewFrame* pvf = pApp->getViewFrame(vsnApp::ConvWxToSys(valStr));
  if ( ! pvf ) {
    ErrMsg(MsgERR, "method keyFrameAnim: can't find the selected ViewFrame");
    return;
  }

  pm->setTargetView(pvf->getName());
}

void vsnMPP_keyFrameAnim::OnEditBtn(wxCommandEvent& event) {
  vsnMethod_keyFrameAnim* pm = dynamic_cast<vsnMethod_keyFrameAnim*>(p_method);
  if ( ! pm ) return;
  pm->showEditDlg(true);  
}


//----------------------------------------------------------------
// class vsnMethod_keyFrameAnim
//----------------------------------------------------------------

/* constructors / destructor */

vsnMethod_keyFrameAnim::vsnMethod_keyFrameAnim(const string& name)
  : vsnMethodObj(name), m_pEditDlg(NULL), m_needRewind(false),
    m_timeKFA(0.0), m_scrShot(false), m_playerMode(false)
{
  vsnViewFrame* pvf0 = vsnApp::GetApp()->getViewFrame(0);
  if ( pvf0 ) m_targetView = pvf0->getName();
  else m_targetView = string("View_0");
}

vsnMethod_keyFrameAnim::~vsnMethod_keyFrameAnim() {
  if ( m_pEditDlg ) {
    m_pEditDlg->Destroy();
    m_pEditDlg = NULL;
  }
}


/* interface  */

void vsnMethod_keyFrameAnim::showEditDlg(const bool show) {
  if ( ! m_pEditDlg ) {
    if ( ! show ) return;
    vsnViewFrame* pvf0 = vsnApp::GetApp()->getViewFrame(0);
    m_pEditDlg
      = new vsnKeyFrameAnimEditDlg(pvf0, this);
    if ( ! m_pEditDlg ) {
      ErrMsg(MsgERR, string("method keyFrameAnim: "
			    "can't create KeyFrameAnimEditDlg"));
      return;
    }
  }
  m_pEditDlg->Show(show);
}

void vsnMethod_keyFrameAnim::updateEditDlg() {
  if ( ! m_pEditDlg ) return;
  m_pEditDlg->update();
}


void vsnMethod_keyFrameAnim::reset() {
  m_timeKFA = 0.0;
  m_KFA.Reset();
  setPlayerMode(false);
  updateEditDlg();
}

void vsnMethod_keyFrameAnim::play(const bool rewind) {
  // stop timer thread
  Stop();

  // check target view
  vsnViewFrame* pvf = vsnApp::GetApp()->getViewFrame(m_targetView);
  if ( ! pvf ) {
      ErrMsg(MsgERR, string("method keyFrameAnim: no valid View sepcified"));
      return;
  }

  double lastTm = m_KFA.getTotalTime() + m_KFA.getInitialTime();
  if ( fabs(lastTm - m_timeKFA) < 1e-8 ) m_timeKFA = m_KFA.getInitialTime();

  updateEditDlg();

  m_animFileCtx.reset();
  m_animFileIdx = 0;
  if ( m_scrShot && m_scrShotPath.empty() ) {
    ErrMsg(MsgERR, string("method keyFrameAnim: ")
             + string("no screenshot file specified, turned to off"));
    m_scrShot = false;
    updateUI();
  }
  if ( m_scrShot && m_playerMode ) {
    ErrMsg(MsgERR, string("method keyFrameAnim: ")
             + string("screenshot is not valid in player mode, turned to off"));
    m_scrShot = false;
    updateUI();
  }
  if ( m_scrShot ) {
    if ( ! m_animFileCtx.setup(m_scrShotPath) ) {
      ErrMsg(MsgERR, string("method keyFrameAnim: "
			    "can't setup screenshot, turned to off"));
      m_scrShot = false;
      updateUI();
    }
    // override check for movie file
    if ( m_animFileCtx.m_mkMov ) {
      FILE* mvf = fopen(m_animFileCtx.m_sfMov.c_str(), "r");
      if ( mvf ) {
        fclose(mvf);
        string msg = "The specified movie file has already existed\n  ";
        msg += m_animFileCtx.m_sfMov;
        msg += "\n\nAre you sure to override "
          "(or screenshot turned to off) ?\n";
        wxMessageDialog dlg(NULL, vsnApp::ConvSysToWx(msg),
                            wxT("AnimPlay"), vsn_wxOK_CANCEL|wxICON_QUESTION);
        if ( dlg.ShowModal() == vsn_wxIDOK ) {
          wxRemoveFile(vsnApp::ConvSysToWx(m_animFileCtx.m_sfMov));
        }
        else {
          m_scrShot = false;
          updateUI();
        }
      }
    } // end of if(mkMov)
  } // end of if(scrShot)

  // start the timer
  m_needRewind = rewind;
  if ( m_scrShot ) m_needRewind = true;
  if ( m_playerMode )
    Start(ANIM_INTERVAL2);
  else
    Start(ANIM_INTERVAL);
}

void vsnMethod_keyFrameAnim::stop() {
  // stop timer thread
  Stop();

  if ( m_scrShot && m_animFileCtx.m_mkMov ) {
    deque<string>::iterator it;
    for ( it = m_animFileCtx.m_movFrames.begin();
          it != m_animFileCtx.m_movFrames.end(); it++ ) {
      wxRemoveFile(vsnApp::ConvSysToWx(*it));
    }
  }
  m_animFileCtx.reset();
  m_animFileIdx = 0;
  updateUI();
  updateEditDlg();
}

bool vsnMethod_keyFrameAnim::addKeyframe(const double tm) {
  if ( IsRunning() ) return false;
  vsnViewFrame* pvf = vsnApp::GetApp()->getViewFrame(m_targetView);
  if ( ! pvf ) {
    ErrMsg(MsgERR, string("method keyFrameAnim: no valid View sepcified"));
    return false;
  }
  vsnViewPoint vp = pvf->getGfxView()->getXForm();

  vsnAnimFrame F;
  F.rotQuat = vp.getRotQuat();
  F.transVec = vp.vT;
  F.scaleVec = vp.vS;
  F.centerVec = vp.vC;

  vsnTimeSeriesDataIF* pTD = dynamic_cast<vsnTimeSeriesDataIF*>(p_refData);
  if ( pTD ) {
    size_t tsIdx = pTD->getCurrentStepIdx();
    int tsNo;
    if ( pTD->getTimeStepNo(tsIdx, tsNo) ) F.step = tsNo;
  }
  
  if ( ! m_KFA.addKeyframe(tm, F) ) {
    ErrMsg(MsgERR, string("method keyFrameAnim: can't add key frame"));
    return false;
  }

  updateEditDlg();
  return true;
}

bool vsnMethod_keyFrameAnim::delKeyframe(const size_t idx) {
  if ( IsRunning() ) return false;
  if ( ! m_KFA.delKeyframe(idx) ) {
    ErrMsg(MsgERR, string("method keyFrameAnim: can't delete key frame"));
    return false;
  }
  updateEditDlg();
  return true;
}

bool vsnMethod_keyFrameAnim::delKeyframe(const double tm) {
  if ( IsRunning() ) return false;
  if ( ! m_KFA.delKeyframe(tm) ) {
    ErrMsg(MsgERR, string("method keyFrameAnim: can't delete key frame"));
    return false;
  }
  updateEditDlg();
  return true;
}

bool vsnMethod_keyFrameAnim::replaceKeyframe(const size_t idx) {
  if ( IsRunning() ) return false;
  vsnViewFrame* pvf = vsnApp::GetApp()->getViewFrame(m_targetView);
  if ( ! pvf ) {
    ErrMsg(MsgERR, string("method keyFrameAnim: no valid View sepcified"));
    return false;
  }
  vsnViewPoint vp = pvf->getGfxView()->getXForm();

  vsnAnimFrame F;
  F.rotQuat = vp.getRotQuat();
  F.transVec = vp.vT;
  F.scaleVec = vp.vS;
  F.centerVec = vp.vC;

  vsnTimeSeriesDataIF* pTD = dynamic_cast<vsnTimeSeriesDataIF*>(p_refData);
  if ( pTD ) {
    size_t tsIdx = pTD->getCurrentStepIdx();
    int tsNo;
    if ( pTD->getTimeStepNo(tsIdx, tsNo) ) F.step = tsNo;
  }

  if ( ! m_KFA.replaceKeyframe(idx, F) ) {
    ErrMsg(MsgERR, string("method keyFrameAnim: can't replace key frame"));
    return false;
  }
  return true;
}

bool vsnMethod_keyFrameAnim::setCurrentTime(const double tm) {
  vsnViewFrame* pvf = vsnApp::GetApp()->getViewFrame(m_targetView);
  if ( ! pvf ) {
    ErrMsg(MsgERR, string("method keyFrameAnim: no valid View sepcified"));
    return false;
  }

  vsnAnimFrame wkf;
  if ( ! m_KFA.getFrame(tm, wkf) ) {
    ErrMsg(MsgERR, string("method keyFrameAnim: "
			  "can't get frame at specified time"));
    return false;
  }

  vsnViewPoint vp;
  vp.vT = wkf.transVec;
  vp.vS = wkf.scaleVec;
  vp.vC = wkf.centerVec;
  vp.mR = wkf.rotQuat.GetRotMat();
  pvf->getGfxView()->setXForm(vp);

  if ( m_KFA.getStepMode() ) {
    vsnTimeSeriesDataIF* pTD = dynamic_cast<vsnTimeSeriesDataIF*>(p_refData);
    if ( pTD ) {
      size_t tsIdx;
      if ( ! pTD->getStepIdx(wkf.step, tsIdx) ||
	   ! pTD->setCurrentStepIdx(tsIdx) ) {
	ErrMsg(MsgWARN, string("method keyFrameAnim: "
			       "can't set time-step of frame"));
      }
    } else {
      ErrMsg(MsgWARN, string("method keyFrameAnim: "
			     "can't set time-step: not time-series data"));
      m_KFA.setStepMode(false);
      updateUI();
    }
  }

  if ( m_timeKFA != tm ) {
    m_timeKFA = tm;
    updateEditDlg();
  }

  chkNotice();
  return true;
}

void vsnMethod_keyFrameAnim::setPlayerMode(const bool pm) {
  if ( m_playerMode == pm ) return;
  if ( IsRunning() ) return;
  m_playerMode = pm;
  updateUI();
}

void vsnMethod_keyFrameAnim::setShotMode(const bool mode) {
  if ( m_scrShot == mode ) return;
  if ( IsRunning() ) return;
  m_scrShot = mode;
  updateUI();
}

void vsnMethod_keyFrameAnim::setShotPath(const std::string& spath) {
  if ( m_scrShotPath == spath ) return;
  if ( IsRunning() ) return;
  m_scrShotPath = spath;
  updateUI();
}

void vsnMethod_keyFrameAnim::setTargetView(const std::string& tview) {
  if ( m_targetView == tview ) return;
  // don't validation view
  m_targetView = tview;
  updateUI();
}

void vsnMethod_keyFrameAnim::clearEditDlg() {
  m_pEditDlg = NULL;
}

void vsnMethod_keyFrameAnim::animTimerJob() {
  vsnApp* pApp = vsnApp::GetApp();
  if ( ! pApp ) return;

  if ( m_inProcMutex.TryLock() == wxMUTEX_BUSY )
    return;
  //-------------------- LOCK --------------------

  // calc the next KFA time
  double lastTm = m_KFA.getTotalTime() + m_KFA.getInitialTime();
  double nextTm;
  if ( m_needRewind ) {
    nextTm = m_KFA.getInitialTime();
    m_needRewind = false;
  } else {
    nextTm = m_timeKFA + 1.0 / (double)m_KFA.getFps();
  }
  if ( nextTm - lastTm >= -1e-7 ) {
    Stop();
    nextTm = lastTm;
  }

  // redraw (no need to chkNotice)
  if ( ! setCurrentTime(nextTm) ) {
    m_inProcMutex.Unlock();
    stop();
    return;
  }

  // screen shot
  if ( m_scrShot ) {
    vsnViewFrame* pvf = pApp->getViewFrame(m_targetView);
    if ( ! pvf ) {
      m_inProcMutex.Unlock();
      ErrMsg(MsgERR, string("method keyFrameAnim: no valid View sepcified"));
      return;
    }
    vsnGfxView* pgv = (pvf ? pvf->getGfxView() : NULL);
    if ( ! pgv ) {
      ErrMsg(MsgERR, string("method keyFrameAnim: "
			    "can't get GFX View, screenshot turned to off"));
      m_scrShot = false;
      updateUI();
    }
    else {
      string sspath = m_animFileCtx.getPath(m_animFileIdx++);
      if ( ! pgv->screenShot(sspath) ) {
	ErrMsg(MsgERR, string("method keyFrameAnim: "
			      "can't save screenshot, turned to off"));
	m_scrShot = false;
	updateUI();
      }
      else
	m_animFileCtx.m_movFrames.push_back(sspath);
    }
  }

  //-------------------- UNLOCK --------------------
  m_inProcMutex.Unlock();
  if ( ! m_playerMode )
    pApp->Yield();

  // post animation process
  if ( ! IsRunning() ) {
    // create movie if need
    if ( m_scrShot && m_animFileCtx.m_mkMov ) {
      if ( ! m_animFileCtx.makeMovieFile() ) {
	ErrMsg(MsgERR, string("method keyFrameAnim: "
			      "can't create movie file"));
      }
    }

    // reset animation context
    m_animFileCtx.reset(); m_animFileIdx = 0;
    m_scrShot = false;
    updateUI();

    // loop ?
    if ( m_KFA.getLoopMode() && m_timeKFA - lastTm >= -1e-7 ) {
      m_needRewind = true;
      if ( m_playerMode )
	Start(ANIM_INTERVAL2);
      else
	Start(ANIM_INTERVAL);
    }
  }
}


/* from vsnMethodObj */

bool vsnMethod_keyFrameAnim::update(const bool force) {
  if ( ! p_refData || ! p_refData->ready() ) return false;
  return true;
}

vsnMethodPP* vsnMethod_keyFrameAnim::getParamPanel(wxPanel* pp) {
  vsnMPP_keyFrameAnim* pp_keyFrameAnim = new vsnMPP_keyFrameAnim(pp, this);
  if ( ! pp_keyFrameAnim ) return NULL;
  return pp_keyFrameAnim;
}


/* from vsnIoObject */

bool vsnMethod_keyFrameAnim::parseXML(xmlNodePtr xnp) {
  static xmlChar* xs;
  xmlNodePtr cur;
  vsnApp* pApp = vsnApp::GetApp();
  if ( ! pApp ) return false;

  if ( ! vsnMethodObj::parseXML(xnp) ) return false;
  string msgHdr
    = getMethodType() +string("[") +getName() +string("]: parseXML: ");

  // get children node
  cur = xnp->xmlChildrenNode;

  // traverse
  while ( cur ) {
    if ( (cur = vsnIoObject::SkipCommentXML(cur)) == NULL ) break;

    if ( !strcmp((const char*)cur->name, "param") ) {
      string xsN, xsV;
      if ( xs ) xmlFree(xs);
      xs = xmlGetProp(cur, (const xmlChar*)"name");
      if ( ! xs || strlen((const char*)xs) < 1 ) goto _NEXT_XML_NODE;
      xsN = (const char*)xs;
      if ( xs ) xmlFree(xs);
      xs = xmlGetProp(cur, (const xmlChar*)"value");
      if ( xs && strlen((const char*)xs) > 0 ) xsV = (const char*)xs;
      else xsV = "";

      if ( xsV.empty() ) {
        ErrMsg(MsgERR, msgHdr +string("no value in param ") +xsN);
        goto _NEXT_XML_NODE;
      }

      if ( xsN == string("target_view") ) {
        setTargetView(xsV);
      } // end of "target_view"
      else if ( xsN == string("player_mode") ) {
        bool pm;
        if ( xsV == string("yes") ) pm = true;
        else if ( xsV == string("no") ) pm = false;
        else {
          ErrMsg(MsgERR, msgHdr + string("invalid value in param player_mode"));
          goto _NEXT_XML_NODE;
        }
        setPlayerMode(pm);
      } // end of "player_mode"
      else if ( xsN == string("anim_sshot") ) {
        bool ssm;
        if ( xsV == string("yes") ) ssm = true;
        else if ( xsV == string("no") ) ssm = false;
        else {
          ErrMsg(MsgERR, msgHdr + string("invalid value in param anim_sshot"));
          goto _NEXT_XML_NODE;
        }
        setShotMode(ssm);
      } // end of "anim_sshot"
      else if ( xsN == string("anim_sshot_path") ) {
        if ( pApp->needPathEncode() ) {
          wxString cvtPath = wxString::FromUTF8((const char*)xs);
          xsV = vsnApp::ConvWxToSys(cvtPath);
	}
        setShotPath(xsV);
      } // end of "anim_sshot_path"
    } // end of param

    else if ( !strcmp((const char*)cur->name, "keyframes") ) {
      if ( ! m_KFA.parseXML(cur) ) {
	ErrMsg(MsgERR, msgHdr + string("keyframes node parse failed"));
	goto _NEXT_XML_NODE;
      }
    } // end of keyframes

  _NEXT_XML_NODE:
    cur = cur->next;
  } // end of while(cur)

  return true;
}

bool vsnMethod_keyFrameAnim::outputXML(std::ostream& os, const size_t ts) {
  string idts;
  for ( int i = 0; i < ts; i++ ) idts.push_back(' ');
  string idts_2 = idts; idts_2.push_back(' '); idts_2.push_back(' ');
  bool ret= true;
  string msgHdr
    = getMethodType() +string("[") +getName() +string("]: outputXML: ");

  // output
  os << idts << "<method type=\"" << getMethodType() << "\"";
  if ( !_name.empty() && _name != string(VFR_NONAME) )
    os << " name=\"" << _name << "\"";
  os << " >" << endl;

  // output base method params
  if ( ! vsnMethodObj::exportXMLNode(os, ts+2) ) {
    ErrMsg(MsgERR, msgHdr + string("base MethodObj exportXMLNode failed"));
    ret = false;
  }

  // output original params
  // target_view
  vsnViewFrame* pvf0 = vsnApp::GetApp()->getViewFrame(0);
  if ( pvf0 && m_targetView != pvf0->getName() ) {
    os << idts_2 << "<param name=\"target_view\" value=\""
       << m_targetView << "\" />" << endl;
  }

  // player_mode
  if ( m_playerMode ) {
    os << idts_2 << "<param name=\"player_mode\" value=\"yes\" />" << endl;
  }

  // anim_sshot
  if ( m_scrShot ) {
    os << idts_2 << "<param name=\"anim_sshot\" value=\"yes\" />" << endl;
  }
  if ( ! m_scrShotPath.empty() ) {
    os << idts_2 << "<param name=\"anim_sshot_path\" value=\""
       << m_scrShotPath << "\" />" << endl;
  }

  // output keyframes
  if ( ! m_KFA.outputXML(os, ts+2) ) {
    ErrMsg(MsgERR, msgHdr + string("output keyframes XMLNode failed"));
    ret = false;
  }

  os << idts << "</method>" << endl;

  // 'show edit dialog' command
  if ( m_pEditDlg && m_pEditDlg->IsShown() ) {
    string extComm = string("<command target=\"") + getName() +
      string("\" scene=\"") + p_refData->getAncestorScene()->getName() +
      string("\" name=\"show_edit_dialog\" value=\"yes\" />\n");
    vsnApp::GetApp()->registExtOutStr(extComm);
  }

  return ret;
}

bool vsnMethod_keyFrameAnim::commandXML(xmlNodePtr xnp) {
  static xmlChar* xs;
  string msgHdr
    = getMethodType() + string("[") + getName() + string("]: commandXML: ");

  // is 'command' node?
  if ( ! xnp ) return false;
  if ( strcmp((const char*)xnp->name, "command") ) return false;

  // is my command?
  if ( xs ) xmlFree(xs);
  xs = xmlGetProp(xnp, (const xmlChar*)"target");
  if ( ! xs || strlen((const char*)xs) < 1 ) return false;
  if ( string((const char*)xs) != getName() ) return false;

  // get command name
  string nameStr, valueStr;
  if ( xs ) xmlFree(xs);
  xs = xmlGetProp(xnp, (const xmlChar*)"name");
  if ( ! xs || strlen((const char*)xs) < 1 ) {
    ErrMsg(MsgERR, msgHdr + "invalid command: no 'name' property");
    return false;
  }
  nameStr = string((const char*)xs);

  // traverse base MethdObj command
  if ( vsnMethodObj::commandXML(xnp) )
    return true; // this is a base MethdObj command, ok


  // get command value (if there)
  if ( xs ) xmlFree(xs);
  xs = xmlGetProp(xnp, (const xmlChar*)"value");
  if ( xs && strlen((const char*)xs) > 0 )
    valueStr = string((const char*)xs);

  // do the command (value not required)
  if ( nameStr == "play" || nameStr == "anim_play" ) {
    play();
    return true;
  } // end of "play"
  else if ( nameStr == "stop" || nameStr == "anim_stop" ) {
    stop();
    return true;
  } // end of "stop"
  else if ( nameStr == "reset" ) {
    reset();
    return true;
  } // end of "reset"


  // do the command
  if ( valueStr.empty() ) {
    ErrMsg(MsgERR, msgHdr + "invalid command: " + nameStr + ": no value");
    return false;
  }

  if ( nameStr == "set_target_view" ) {
    setTargetView(valueStr);
  } // end of "set_target_view"
  else if ( nameStr == "set_anim_sshot" ) {
    bool ssm;
    if ( valueStr == string("yes") ) ssm = true;
    else if ( valueStr == string("no") ) ssm = false;
    else {
      ErrMsg(MsgERR, msgHdr + string("command set_anim_sshot: invalid value"));
      return false;
    }
    setShotMode(ssm);
  } // end of "set_anim_sshot"
  else if ( nameStr == "set_anim_sshot_path" ) {
    setShotPath(valueStr);
  } // end of "set_anim_sshot_path"
  else if ( nameStr == "set_current_time" ) {
    double tm = atof(valueStr.c_str());
    if ( ! setCurrentTime(tm) ) {
      return false;
    }
  } // end of "set_current_time"
  else if ( nameStr == "set_loop" || nameStr == "set_anim_loop" ) {
    bool alm;
    if ( valueStr == string("yes") ) alm = true;
    else if ( valueStr == string("no") ) alm = false;
    else {
      ErrMsg(MsgERR, msgHdr + string("command set_loop: invalid value"));
      return false;
    }
    m_KFA.setLoopMode(alm);
    updateEditDlg();
  } // end of "set_loop"
  else if ( nameStr == "set_use_step" ) {
    bool usm;
    if ( valueStr == string("yes") ) usm = true;
    else if ( valueStr == string("no") ) usm = false;
    else {
      ErrMsg(MsgERR, msgHdr + string("command set_use_step: invalid value"));
      return false;
    }
    m_KFA.setStepMode(usm);
    updateEditDlg();
  } // end of "set_use_step"
  else if ( nameStr == "set_player_mode" ) {
    bool pm;
    if ( valueStr == string("yes") ) pm = true;
    else if ( valueStr == string("no") ) pm = false;
    else {
      ErrMsg(MsgERR, msgHdr + string("command set_player_mode: invalid value"));
      return false;
    }
    setPlayerMode(pm);
    updateEditDlg();
  } // end of "set_player_mode"
  else if ( nameStr == "set_fps" ) {
    int fps = atoi(valueStr.c_str());
    if ( ! m_KFA.setFps(fps) ) {
      ErrMsg(MsgERR, msgHdr + string("command set_fps: set FPS failed"));
      return false;
    }
    updateEditDlg();
  } // end of "set_fps"
  else if ( nameStr == "add_keyframe" ) {
    double tm = atof(valueStr.c_str());
    if ( ! addKeyframe(tm) ) {
      return false;
    }
  } // end of "add_keyframe"
  else if ( nameStr == "del_keyframe" ) {
    double tm = atof(valueStr.c_str());
    if ( ! delKeyframe(tm) ) {
      return false;
    }
  } // end of "del_keyframe"
  else if ( nameStr == "show_edit_dialog" ) {
    bool sdm;
    if ( valueStr == string("yes") ) sdm = true;
    else if ( valueStr == string("no") ) sdm = false;
    else {
      ErrMsg(MsgERR, msgHdr +string("command show_edit_dialog: invalid value"));
      return false;
    }
    showEditDlg(sdm);
  } // end of "show_edit_dialog"
  else {
    // not my command
    ErrMsg(MsgERR, msgHdr + string("unknown command: ") + nameStr);
    return false;
  }

  return true;
}
