//
// vsnMethod_timeStep
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

#include "vsnMethod_timeStep.h"
#include "vsnDataObj.h"
#include "vsnDataReload.h"
#include "vsnScene.h"
#include "vsnError.h"
#include <sstream>

#define ANIM_INTERVAL 150 // 0.15 sec.

using namespace std;
using namespace CES;
using namespace VSN;


//----------------------------------------------------------------
// class vsnMPP_timeStep::TimeStepKeyAct
//----------------------------------------------------------------

vsnMPP_timeStep::TimeStepKeyAct::TimeStepKeyAct(vsnGfxView* pgv,
						vsnMethod_timeStep* pm)
  : vsnGfxAct_KeyIn(pgv), p_method(pm)
{
}

void vsnMPP_timeStep::TimeStepKeyAct::setTimeStepMethod(vsnMethod_timeStep* pm)
{
  if ( p_method == pm ) return;
  p_method = pm;
}

void vsnMPP_timeStep::TimeStepKeyAct::execute(vfrEvent& e) {
  if ( ! p_method ) return;
  if ( ! e.isKeyEvent() ) return;

  int range[2];
  if ( ! p_method->getStepRange(range) ) {
    ErrMsg(MsgERR,
	   string("method timeStep::TimeStepKeyAct: get range failed"));
    return;
  }

  int newStep;
  switch ( e.getKey() ) {
  case VFRKC_DOWN:
    newStep = range[0]; break;
  case VFRKC_UP:
    newStep = range[1]; break;
  case VFRKC_LEFT:
    newStep = p_method->getTimeStep() - p_method->getSkipSteps();
    break;
  case VFRKC_RIGHT:
    newStep = p_method->getTimeStep() + p_method->getSkipSteps();
    break;
  default:
    vsnGfxAct_KeyIn::execute(e);
    return;
  } // end of switch(kc)

  if ( newStep < range[0] ) newStep = range[0];
  if ( newStep > range[1] ) newStep = range[1];

  if ( p_method->setTimeStep(newStep) )
    p_method->chkNotice();
}


//----------------------------------------------------------------
// class vsnMPP_timeStep
//----------------------------------------------------------------

BEGIN_EVENT_TABLE(vsnMPP_timeStep, wxPanel)
  EVT_COMMAND_SCROLL(MPP_timeStep_StepSld, vsnMPP_timeStep::OnStepSld)
  EVT_TEXT_ENTER(MPP_timeStep_StepTxt, vsnMPP_timeStep::OnStepTxt)
  EVT_BUTTON(MPP_timeStep_LeftBtn, vsnMPP_timeStep::OnLeftBtn)
  EVT_BUTTON(MPP_timeStep_RightBtn, vsnMPP_timeStep::OnRightBtn)
  EVT_TEXT_ENTER(MPP_timeStep_SkipTxt, vsnMPP_timeStep::OnSkipTxt)
  EVT_TEXT_ENTER(MPP_timeStep_StartTxt, vsnMPP_timeStep::OnStartTxt)
  EVT_TEXT_ENTER(MPP_timeStep_EndTxt, vsnMPP_timeStep::OnEndTxt)
  EVT_CHECKBOX(MPP_timeStep_ShowLabelChk, vsnMPP_timeStep::OnShowLabelChk)
  EVT_TEXT_ENTER(MPP_timeStep_LabelFmtTxt, vsnMPP_timeStep::OnLabelTxt)
  EVT_TEXT_ENTER(MPP_timeStep_LabelPosXTxt, vsnMPP_timeStep::OnLabelTxt)
  EVT_TEXT_ENTER(MPP_timeStep_LabelPosYTxt, vsnMPP_timeStep::OnLabelTxt)
  EVT_TEXT_ENTER(MPP_timeStep_LabelSizeTxt, vsnMPP_timeStep::OnLabelTxt)
  EVT_BUTTON(MPP_timeStep_AnimPlayBtn, vsnMPP_timeStep::OnAnimPlayBtn)
  EVT_BUTTON(MPP_timeStep_AnimStopBtn, vsnMPP_timeStep::OnAnimStopBtn)
  EVT_BUTTON(MPP_timeStep_AnimRewBtn, vsnMPP_timeStep::OnAnimRewBtn)
  EVT_BUTTON(MPP_timeStep_AnimFwdBtn, vsnMPP_timeStep::OnAnimFwdBtn)
  EVT_CHECKBOX(MPP_timeStep_AnimLoopChk, vsnMPP_timeStep::OnAnimLoopChk)
  EVT_CHECKBOX(MPP_timeStep_AnimSshotChk, vsnMPP_timeStep::OnAnimSshotChk)
  EVT_TEXT_ENTER(MPP_timeStep_AnimSshotFileTxt,
		 vsnMPP_timeStep::OnAnimSshotFileTxt)
  EVT_BUTTON(MPP_timeStep_AnimSshotFileBtn,
	     vsnMPP_timeStep::OnAnimSshotFileBtn)
  EVT_TEXT_ENTER(MPP_timeStep_AnimSshotViewTxt,
		 vsnMPP_timeStep::OnAnimSshotViewTxt)
  EVT_BUTTON(MPP_timeStep_AnimSshotViewBtn,
	     vsnMPP_timeStep::OnAnimSshotViewBtn)
  EVT_CHAR(vsnMPP_timeStep::OnChar)
END_EVENT_TABLE()


/* constructors / destructor */

vsnMPP_timeStep::vsnMPP_timeStep(wxPanel* parent, vsnMethodObj* pm)
  : vsnMethodPP(parent, pm),
    m_pStepSld(NULL), m_pStepTxt(NULL), m_pStartTxt(NULL), m_pEndTxt(NULL),
    m_pSkipTxt(NULL), m_pLeftBtn(NULL), m_pRightBtn(NULL),
    m_pShowLabelChk(NULL), m_pLabelFmtTxt(NULL),
    m_pLabelPosXTxt(NULL), m_pLabelPosYTxt(NULL), m_pLabelSizeTxt(NULL),
    m_pAnimPlayBtn(NULL), m_pAnimStopBtn(NULL),
    m_pAnimRewBtn(NULL), m_pAnimFwdBtn(NULL), m_pAnimSshotChk(NULL),
    m_pAnimLoopChk(NULL), m_pAnimSshotFileTxt(NULL),
    m_pAnimSshotFileBtn(NULL), m_pAnimSshotViewBtn(NULL)
{
  assert(parent);
  vsnMethod_timeStep* ptsm = dynamic_cast<vsnMethod_timeStep*>(pm);
  assert(ptsm);

  // setup gfxAct
  gfxAct_KI.setTimeStepMethod(ptsm);
  vsnUiView* puiv = getUiView();
  if ( puiv ) {
    vsnViewFrame* pvf = dynamic_cast<vsnViewFrame*>(puiv->GetParent());
    if ( pvf ) {
      vsnGfxView* pgv = pvf->getGfxView();
      if ( pgv ) {
        vfrDrawAreaWx* pda = pgv->getDrawArea();
        if ( pda ) {
          vfrDispatch& dispatcher = vfrDispatch::instance(*pda);
          gfxAct_KI.setGfxView(pgv);
          pgv->pushActions(this);
          pgv->setStandardActions();
          vfrEvKeyIn::instance(dispatcher).regist(&gfxAct_KI);
        } // end of if(pda)
      } // end of if(pgv)
    } // end of if(pvf)
  } // end of if(puvi)


  // widgets layout
  wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer* sizerH;

  // step range
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxALL, 0);
  sizerH->Add(new wxStaticText(this, -1, wxT("start step")),
	      0, wxALIGN_LEFT|wxALL, 3);
  m_pStartTxt = new wxTextCtrl(this, MPP_timeStep_StartTxt, wxT(""),
			       wxDefaultPosition, wxDefaultSize,
			       wxTE_READONLY|wxTE_PROCESS_ENTER);
  assert(m_pStartTxt);
  sizerH->Add(m_pStartTxt, 0, wxEXPAND|wxALL, 3);

  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxALL, 0);
  sizerH->Add(new wxStaticText(this, -1, wxT(" end step")),
	      0, wxALIGN_LEFT|wxALL, 3);
  m_pEndTxt = new wxTextCtrl(this, MPP_timeStep_EndTxt, wxT(""),
			     wxDefaultPosition, wxDefaultSize,
			     wxTE_READONLY|wxTE_PROCESS_ENTER);
  assert(m_pEndTxt);
  sizerH->Add(m_pEndTxt, 0, wxEXPAND|wxALL, 3);

  topsizer->Add(5, 5);

  // current step
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND|wxALL, 0);
  sizerH->Add(new wxStaticText(this, -1, wxT("current step")),
	      0, wxALIGN_LEFT|wxALL, 3);
  m_pStepTxt = new wxTextCtrl(this, MPP_timeStep_StepTxt, wxT(""),
			      wxDefaultPosition, wxDefaultSize,
			      wxTE_PROCESS_ENTER);
  assert(m_pStepTxt);
  sizerH->Add(m_pStepTxt, 0, wxEXPAND|wxALL, 3);

  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND|wxALL, 0);
  m_pStepSld = new wxSlider(this, MPP_timeStep_StepSld,
			    20000 /* val */, 10000 /* min */, 30000 /* max */,
			    wxDefaultPosition, wxSize(120, -1),
			    wxSL_HORIZONTAL|wxSL_LABELS);
  assert(m_pStepSld);
  sizerH->Add(m_pStepSld, 1, wxEXPAND|wxALL, 3);
  sizerH->Add(10, 5);

  topsizer->Add(5, 5);

  // skip steps
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxALL, 0);

  sizerH->Add(new wxStaticText(this, -1, wxT("skip steps")),
	      0, wxALIGN_LEFT|wxALL, 3);
  m_pSkipTxt = new wxTextCtrl(this, MPP_timeStep_SkipTxt, wxT(""),
			       wxDefaultPosition, wxDefaultSize,
			       wxTE_PROCESS_ENTER);
  assert(m_pSkipTxt);
  sizerH->Add(m_pSkipTxt, 0, wxEXPAND|wxALL, 3);

  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxALL, 0);
  m_pLeftBtn = new wxButton(this, MPP_timeStep_LeftBtn, wxT("< skip"),
                            wxDefaultPosition, wxSize(60,-1), wxBU_EXACTFIT);
  assert(m_pLeftBtn);
  sizerH->Add(m_pLeftBtn, 1, wxEXPAND|wxALL, 3);
  m_pRightBtn = new wxButton(this, MPP_timeStep_RightBtn, wxT("skip >"),
                             wxDefaultPosition, wxSize(60,-1), wxBU_EXACTFIT);
  assert(m_pRightBtn);
  sizerH->Add(m_pRightBtn, 1, wxEXPAND|wxALL, 3);

  topsizer->Add(5, 5);

  // label
  topsizer->Add(
     new wxStaticLine(this, -1, wxDefaultPosition, wxSize(3,3), wxHORIZONTAL),
     0, wxEXPAND|wxALL, 0);

  m_pShowLabelChk = new wxCheckBox(this, MPP_timeStep_ShowLabelChk,
				   wxT("show text label"));
  assert(m_pShowLabelChk);
  topsizer->Add(m_pShowLabelChk, 0, wxALL, 3);

  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND|wxALL, 0);
  sizerH->Add(new wxStaticText(this, -1, wxT("format")),
	      0, wxALIGN_LEFT|wxALL, 3);
  m_pLabelFmtTxt = new wxTextCtrl(this, MPP_timeStep_LabelFmtTxt, wxT(""),
				  wxDefaultPosition, wxDefaultSize,
				  wxTE_PROCESS_ENTER);
  assert(m_pLabelFmtTxt);
  sizerH->Add(m_pLabelFmtTxt, 1, wxEXPAND|wxALL, 3);

  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND|wxALL, 0);
  sizerH->Add(new wxStaticText(this, -1, wxT("pos X")), 0,
	      wxALIGN_LEFT|wxALL, 3);
  m_pLabelPosXTxt = new wxTextCtrl(this, MPP_timeStep_LabelPosXTxt, wxT(""),
				   wxDefaultPosition, wxSize(60,-1),
				   wxTE_PROCESS_ENTER);
  assert(m_pLabelPosXTxt);
  sizerH->Add(m_pLabelPosXTxt, 1, wxEXPAND|wxALL, 3);
  sizerH->Add(new wxStaticText(this, -1, wxT("Y")), 0, wxALIGN_LEFT|wxALL, 3);
  m_pLabelPosYTxt = new wxTextCtrl(this, MPP_timeStep_LabelPosYTxt, wxT(""),
				   wxDefaultPosition, wxSize(60,-1),
				   wxTE_PROCESS_ENTER);
  assert(m_pLabelPosYTxt);
  sizerH->Add(m_pLabelPosYTxt, 1, wxEXPAND|wxALL, 3);

  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxALL, 0);
  sizerH->Add(new wxStaticText(this, -1, wxT("size")),
	      0, wxALIGN_LEFT|wxALL, 3);
  m_pLabelSizeTxt = new wxTextCtrl(this, MPP_timeStep_LabelSizeTxt, wxT(""),
				   wxDefaultPosition, wxDefaultSize,
				   wxTE_PROCESS_ENTER);
  assert(m_pLabelSizeTxt);
  sizerH->Add(m_pLabelSizeTxt, 0, wxEXPAND|wxALL, 3);

  topsizer->Add(5, 5);

  // anim control
  topsizer->Add(
     new wxStaticLine(this, -1, wxDefaultPosition, wxSize(3,3), wxHORIZONTAL),
     0, wxEXPAND|wxALL, 0);

  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND|wxALL, 0);
  m_pAnimRewBtn = new wxButton(this, MPP_timeStep_AnimRewBtn, wxT("|<"),
			       wxDefaultPosition,wxSize(30,-1), wxBU_EXACTFIT);
  assert(m_pAnimRewBtn);
  sizerH->Add(m_pAnimRewBtn, 0, wxALL, 3);
  m_pAnimStopBtn = new wxButton(this, MPP_timeStep_AnimStopBtn, wxT("[] stop"),
				wxDefaultPosition,wxSize(60,-1),wxBU_EXACTFIT);
  assert(m_pAnimStopBtn);
  sizerH->Add(m_pAnimStopBtn, 0, wxALL, 3);
  m_pAnimPlayBtn = new wxButton(this, MPP_timeStep_AnimPlayBtn, wxT("> play"),
				wxDefaultPosition,wxSize(60,-1),wxBU_EXACTFIT);
  assert(m_pAnimPlayBtn);
  sizerH->Add(m_pAnimPlayBtn, 0, wxALL, 3);
  m_pAnimFwdBtn = new wxButton(this, MPP_timeStep_AnimFwdBtn, wxT(">|"),
			       wxDefaultPosition,wxSize(30,-1), wxBU_EXACTFIT);
  assert(m_pAnimFwdBtn);
  sizerH->Add(m_pAnimFwdBtn, 0, wxALL, 3);

  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND|wxALL, 0);
  m_pAnimLoopChk = new wxCheckBox(this, MPP_timeStep_AnimLoopChk, wxT("loop"));
  assert(m_pAnimLoopChk);
  sizerH->Add(m_pAnimLoopChk, 1, wxEXPAND|wxALL, 3);

  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND|wxALL, 0);
  m_pAnimSshotChk = new wxCheckBox(this, MPP_timeStep_AnimSshotChk,
				   wxT("screen shot"));
  assert(m_pAnimSshotChk);
  sizerH->Add(m_pAnimSshotChk, 1, wxEXPAND|wxALL, 3);

  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND|wxALL, 0);
  sizerH->Add(new wxStaticText(this, -1, wxT("file")),
	      0, wxALIGN_LEFT|wxALL, 3);
  m_pAnimSshotFileTxt = new wxTextCtrl(this, MPP_timeStep_AnimSshotFileTxt,
				       wxT(""), wxDefaultPosition,wxSize(60,-1),
				       wxTE_PROCESS_ENTER);
  assert(m_pAnimSshotFileTxt);
  sizerH->Add(m_pAnimSshotFileTxt, 1, wxEXPAND|wxALL, 3);
  m_pAnimSshotFileBtn = new wxButton(this, MPP_timeStep_AnimSshotFileBtn,
				     wxT("..."), wxDefaultPosition,
				     wxDefaultSize, wxBU_EXACTFIT);
  assert(m_pAnimSshotFileBtn);
  sizerH->Add(m_pAnimSshotFileBtn, 0, wxALL, 3);

  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND|wxALL, 0);
  sizerH->Add(new wxStaticText(this, -1, wxT("view")),
	      0, wxALIGN_LEFT|wxALL, 3);
  m_pAnimSshotViewTxt = new wxTextCtrl(this, MPP_timeStep_AnimSshotViewTxt,
				       wxT(""), wxDefaultPosition,wxSize(60,-1),
				       wxTE_PROCESS_ENTER);
  assert(m_pAnimSshotViewTxt);
  sizerH->Add(m_pAnimSshotViewTxt, 1, wxEXPAND|wxALL, 3);
  m_pAnimSshotViewBtn = new wxButton(this, MPP_timeStep_AnimSshotViewBtn,
				     wxT("..."), wxDefaultPosition,
				     wxDefaultSize, wxBU_EXACTFIT);
  assert(m_pAnimSshotViewBtn);
  sizerH->Add(m_pAnimSshotViewBtn, 0, wxALL, 3);

  // post process
  SetSizer(topsizer);
  addTo(parent);
  topsizer->Layout();
  (void)update();
}

vsnMPP_timeStep::~vsnMPP_timeStep() {
}


/* interface */

bool vsnMPP_timeStep::update() {
  int val, range[2]; char txt[256];
  vsnMethod_timeStep* pm = dynamic_cast<vsnMethod_timeStep*>(p_method);
  if ( ! pm ) return false;

  if ( ! pm->getStepRange(range) ) return false;
  if ( range[0] > range[1] ) return false;
  val = pm->getTimeStep();
  if ( val < range[0] || val > range[1] ) return false;
  if ( range[0] == range[1] ) range[1] = range[0] + 1;
  m_pStepSld->SetRange(range[0], range[1]);
  m_pStepSld->SetValue(val);

  sprintf(txt, "%d", val);
  m_pStepTxt->SetValue(vsnApp::ConvSysToWx(txt));

  sprintf(txt, "%d", range[0]);
  m_pStartTxt->SetValue(vsnApp::ConvSysToWx(txt));
  sprintf(txt, "%d", range[1]);
  m_pEndTxt->SetValue(vsnApp::ConvSysToWx(txt));

  sprintf(txt, "%d", pm->getSkipSteps());
  m_pSkipTxt->SetValue(vsnApp::ConvSysToWx(txt));

  m_pShowLabelChk->SetValue(pm->isShowFrontLabel());
  wxString wwks;
  string wks = pm->getLabelFmt();
  if ( wks.empty() ) wwks = wxT("");
  else wwks = vsnApp::ConvSysToWx(wks);
  m_pLabelFmtTxt->SetValue(wwks);
  vsnFrontLabel* pfl = pm->getFrontLabel();
  if ( pfl ) {
    vector2 pos; float sz;
    pfl->getPosition(pos);
    sz = pfl->getLabelScale();
    sprintf(txt, "%g", pos[0]);
    m_pLabelPosXTxt->SetValue(vsnApp::ConvSysToWx(txt));
    sprintf(txt, "%g", pos[1]);
    m_pLabelPosYTxt->SetValue(vsnApp::ConvSysToWx(txt));
    sprintf(txt, "%g", sz); m_pLabelSizeTxt->SetValue(vsnApp::ConvSysToWx(txt));
  }

  // update animation params
  m_pAnimLoopChk->SetValue(pm->anim_getLoop());
  m_pAnimSshotChk->SetValue(pm->anim_getShotMode());
  wks = pm->anim_getShotPath();
  if ( wks.empty() ) wwks = wxT("");
  else wwks = vsnApp::ConvSysToWx(wks);
  m_pAnimSshotFileTxt->SetValue(wwks);
  wks = pm->anim_getShotView();
  if ( wks.empty() ) wwks = wxT("");
  else wwks = vsnApp::ConvSysToWx(wks);
  m_pAnimSshotViewTxt->SetValue(wwks);

  return true;
}

void vsnMPP_timeStep::settlement() {
  vsnGfxView* pgv = gfxAct_KI.getGfxView();
  if ( pgv ) {
    pgv->popActions(this);
    gfxAct_KI.setGfxView(NULL);
  }
}


/* event handler */

void vsnMPP_timeStep::OnStepSld(wxScrollEvent& event) {
  vsnMethod_timeStep* pm = dynamic_cast<vsnMethod_timeStep*>(p_method);
  if ( ! pm ) return;

  int val = m_pStepSld->GetValue();
  if ( val == pm->getTimeStep() ) return;

  if ( pm->setTimeStep(val) )
    pm->chkNotice();
}

void vsnMPP_timeStep::OnStepTxt(wxCommandEvent& event) {
  vsnMethod_timeStep* pm = dynamic_cast<vsnMethod_timeStep*>(p_method);
  if ( ! pm ) return;

  char txt[64];
  int range[2];
  if ( ! pm->getStepRange(range) ) return;

  wxString valStr = m_pStepTxt->GetValue();
  if ( valStr.IsEmpty() ) {
    sprintf(txt, "%d", pm->getTimeStep());
    m_pStepTxt->SetValue(vsnApp::ConvSysToWx(txt));
    return;
  }
  int val = atoi(vsnApp::ConvWxToSys(valStr).c_str());
  if ( val == pm->getTimeStep() ) return;
  if ( val < range[0] || val > range[1] ) {
    sprintf(txt, "%d", pm->getTimeStep());
    m_pStepTxt->SetValue(vsnApp::ConvSysToWx(txt));
    return;
  }

  if ( pm->setTimeStep(val) )
    pm->chkNotice();
}

void vsnMPP_timeStep::OnSkipTxt(wxCommandEvent& event) {
  vsnMethod_timeStep* pm = dynamic_cast<vsnMethod_timeStep*>(p_method);
  if ( ! pm ) return;

  char txt[64];
  int val, range[2];
  if ( ! pm->getStepRange(range) ) return;

  wxString valStr = m_pSkipTxt->GetValue();
  if ( valStr.IsEmpty() ) {
    val = pm->getSkipSteps(true); // reset to default
    sprintf(txt, "%d", val); m_pSkipTxt->SetValue(vsnApp::ConvSysToWx(txt));
  } else
    val = atoi(vsnApp::ConvWxToSys(valStr).c_str());

  int oldSkip = pm->getSkipSteps();
  if ( val == oldSkip ) return;
  pm->setSkipSteps(val);
}

void vsnMPP_timeStep::OnStartTxt(wxCommandEvent& event) {
  vsnMethod_timeStep* pm = dynamic_cast<vsnMethod_timeStep*>(p_method);
  if ( ! pm ) return;

  int range[2];
  if ( ! pm->getStepRange(range) ) return;
  if ( pm->getTimeStep() == range[0] ) return;

  if ( pm->setTimeStep(range[0]) )
    pm->chkNotice();
}

void vsnMPP_timeStep::OnEndTxt(wxCommandEvent& event) {
  vsnMethod_timeStep* pm = dynamic_cast<vsnMethod_timeStep*>(p_method);
  if ( ! pm ) return;

  int range[2];
  if ( ! pm->getStepRange(range) ) return;
  if ( pm->getTimeStep() == range[1] ) return;

  if ( pm->setTimeStep(range[1]) )
    pm->chkNotice();
}

void vsnMPP_timeStep::OnLeftBtn(wxCommandEvent& event) {
  vsnMethod_timeStep* pm = dynamic_cast<vsnMethod_timeStep*>(p_method);
  if ( ! pm ) return;
  int newStep = pm->getTimeStep() - pm->getSkipSteps();

  int range[2];
  if ( pm->getStepRange(range) ) {
    if ( newStep < range[0] ) newStep = range[0];
    if ( newStep > range[1] ) newStep = range[1];
  }

  if ( pm->setTimeStep(newStep) )
    pm->chkNotice();
}

void vsnMPP_timeStep::OnRightBtn(wxCommandEvent& event) {
  vsnMethod_timeStep* pm = dynamic_cast<vsnMethod_timeStep*>(p_method);
  if ( ! pm ) return;
  int newStep = pm->getTimeStep() + pm->getSkipSteps();

  int range[2];
  if ( pm->getStepRange(range) ) {
    if ( newStep < range[0] ) newStep = range[0];
    if ( newStep > range[1] ) newStep = range[1];
  }

  if ( pm->setTimeStep(newStep) )
    pm->chkNotice();
}

void vsnMPP_timeStep::OnShowLabelChk(wxCommandEvent& event) {
  vsnMethod_timeStep* pm = dynamic_cast<vsnMethod_timeStep*>(p_method);
  if ( ! pm ) return;

  bool val = m_pShowLabelChk->GetValue();
  if ( pm->showFrontLabel(val) )
    pm->chkNotice();
}

void vsnMPP_timeStep::OnLabelTxt(wxCommandEvent& event) {
  vsnMethod_timeStep* pm = dynamic_cast<vsnMethod_timeStep*>(p_method);
  if ( ! pm ) return;

  wxString valStr;
  float sz = 0.f; vector2 pos = {0.f, 0.f};
  string wks;

  valStr = m_pLabelFmtTxt->GetValue();
  if ( ! valStr.IsEmpty() ) wks = vsnApp::ConvWxToSys(valStr);
  pm->setLabelFmt(wks);

  valStr = m_pLabelPosXTxt->GetValue();
  if ( ! valStr.IsEmpty() )
    pos[0] = (float)atof(vsnApp::ConvWxToSys(valStr).c_str());
  valStr = m_pLabelPosYTxt->GetValue();
  if ( ! valStr.IsEmpty() )
    pos[1] = (float)atof(vsnApp::ConvWxToSys(valStr).c_str());
  valStr = m_pLabelSizeTxt->GetValue();
  if ( ! valStr.IsEmpty() )
    sz = (float)atof(vsnApp::ConvWxToSys(valStr).c_str());
  vsnFrontLabel* pfl = pm->getFrontLabel();
  if ( pfl ) {
    pfl->setPosition(pos);
    pfl->setLabelScale(sz);
  }

  pm->chkNotice();
}

void vsnMPP_timeStep::OnChar(wxKeyEvent& event) {
  // the codes below does not works good on Windows|MacOSX platform
  // (because of Windows' key-acceleraton and Cocoa's TextCtrl focus)
  wxCommandEvent ce;
  switch ( event.GetKeyCode() ) {
  case WXK_LEFT: case WXK_DOWN:
    OnLeftBtn(ce);
    break;
  case WXK_RIGHT: case WXK_UP:
    OnRightBtn(ce);
    break;
  default:
    ;
  }
}


/* event handlers for AnimPlay section */

void vsnMPP_timeStep::OnAnimRewBtn(wxCommandEvent& event) {
  vsnMethod_timeStep* pm = dynamic_cast<vsnMethod_timeStep*>(p_method);
  if ( ! pm ) return;
  int stpRange[2];
  if ( ! pm->getStepRange(stpRange) ) {
    ErrMsg(MsgERR, string("method timeStep: get range failed"));
    return;
  }
  if ( pm->setTimeStep(stpRange[0]) )
    pm->chkNotice();
}

void vsnMPP_timeStep::OnAnimFwdBtn(wxCommandEvent& event) {
  vsnMethod_timeStep* pm = dynamic_cast<vsnMethod_timeStep*>(p_method);
  if ( ! pm ) return;
  int stpRange[2];
  if ( ! pm->getStepRange(stpRange) ) {
    ErrMsg(MsgERR, string("method timeStep: get range failed"));
    return;
  }
  if ( pm->setTimeStep(stpRange[1]) )
    pm->chkNotice();
}

void vsnMPP_timeStep::OnAnimLoopChk(wxCommandEvent& event) {
  vsnMethod_timeStep* pm = dynamic_cast<vsnMethod_timeStep*>(p_method);
  if ( ! pm ) return;

  bool value = m_pAnimLoopChk->GetValue();
  pm->anim_setLoop(value);
}

void vsnMPP_timeStep::OnAnimSshotChk(wxCommandEvent& event) {
  vsnMethod_timeStep* pm = dynamic_cast<vsnMethod_timeStep*>(p_method);
  if ( ! pm ) return;

  bool value = m_pAnimSshotChk->GetValue();
  pm->anim_setShotMode(value);
}

void vsnMPP_timeStep::OnAnimPlayBtn(wxCommandEvent& event) {
  vsnMethod_timeStep* pm = dynamic_cast<vsnMethod_timeStep*>(p_method);
  if ( ! pm ) return;
  pm->anim_play(false);
}

void vsnMPP_timeStep::OnAnimStopBtn(wxCommandEvent& event) {
  if ( m_pAnimSshotChk ) m_pAnimSshotChk->SetValue(false);
  vsnMethod_timeStep* pm = dynamic_cast<vsnMethod_timeStep*>(p_method);
  if ( ! pm ) return;
  pm->anim_stop();
}

void vsnMPP_timeStep::OnAnimSshotFileTxt(wxCommandEvent& event) {
  vsnMethod_timeStep* pm = dynamic_cast<vsnMethod_timeStep*>(p_method);
  if ( ! pm ) return;

  string wks;
  wxString valStr = m_pAnimSshotFileTxt->GetValue();
  if ( ! valStr.IsEmpty() ) wks = vsnApp::ConvWxToSys(valStr);
  pm->anim_setShotPath(wks);
}

void vsnMPP_timeStep::OnAnimSshotFileBtn(wxCommandEvent& event) {
  vsnMethod_timeStep* pm = dynamic_cast<vsnMethod_timeStep*>(p_method);
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

  wxString prevPath = m_pAnimSshotFileTxt->GetValue();
  if ( prevPath.IsEmpty() ) {
    wxString xtmpfn = wxFileName::CreateTempFileName(wxT("vsn"));
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
    m_pAnimSshotFileTxt->SetValue(newPath);
    if ( ! newPath.IsEmpty() ) {
      string spath = vsnApp::ConvWxToSys(newPath);
      pm->anim_setShotPath(spath);
    }
  }
}

void vsnMPP_timeStep::OnAnimSshotViewTxt(wxCommandEvent& event) {
  vsnMethod_timeStep* pm = dynamic_cast<vsnMethod_timeStep*>(p_method);
  if ( ! pm ) return;

  string wks;
  wxString valStr = m_pAnimSshotViewTxt->GetValue();
  if ( ! valStr.IsEmpty() ) wks = vsnApp::ConvWxToSys(valStr);
  pm->anim_setShotView(wks);
}

void vsnMPP_timeStep::OnAnimSshotViewBtn(wxCommandEvent& event) {
  vsnMethod_timeStep* pm = dynamic_cast<vsnMethod_timeStep*>(p_method);
  if ( ! pm ) return;
  vsnApp* pApp = vsnApp::GetApp();
  if ( ! pApp ) return;
  size_t nvf = pApp->getNumViewFrame();
  if ( nvf < 1 ) {
    ErrMsg(MsgERR, "method timeStep: no ViewFrame");
    return;
  }

  size_t i;
  int preSel = 0, vpcnt = 0;
  wxString* choices = new wxString[nvf];
  for ( i = 0; i < nvf; i++ ) {
    vsnViewFrame* pvf = pApp->getViewFrame(i);
    if ( ! pvf ) continue;
    choices[vpcnt] = vsnApp::ConvSysToWx(pvf->getName());
    if ( pm->anim_getShotView() == pvf->getName() )
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
    ErrMsg(MsgERR, "method timeStep: invalid selection of ViewFrame");
    return;
  }
  wxString valStr = dlg.GetStringSelection();
  vsnViewFrame* pvf = pApp->getViewFrame(vsnApp::ConvWxToSys(valStr));
  if ( ! pvf ) {
    ErrMsg(MsgERR, "method timeStep: can't find the selected ViewFrame");
    return;
  }

  pm->anim_setShotView(pvf->getName());
}


//----------------------------------------------------------------
// class vsnMethod_timeStep
//----------------------------------------------------------------

/* constructors / destructor */

vsnMethod_timeStep::vsnMethod_timeStep(const string& name)
  : vsnMethodObj(name), m_timeStep(0), m_skipSteps(1),
    m_pFrLbl(NULL), m_labelFmt("step = #S"),
    m_onceSetTs(false), m_needRewind(false),
    m_animIdx(0), m_animLoop(false), m_animSshot(false)
{
  vsnViewFrame* pvf0 = vsnApp::GetApp()->getViewFrame(0);
  if ( pvf0 ) m_animShotView = pvf0->getName();
  else m_animShotView = string("View_0");
}

vsnMethod_timeStep::~vsnMethod_timeStep() {
  if ( m_pFrLbl )
    delete m_pFrLbl;
}


/* interface  */

bool vsnMethod_timeStep::setTimeStep(const int stp) {
  if ( stp == m_timeStep ) return true;

  int stepRange[2];
  if ( ! getStepRange(stepRange) ) return false;
  if ( stp < stepRange[0] || stp > stepRange[1] ) return false;

  m_timeStep = stp;
  m_onceSetTs = true;

  if ( ! update(false) ) return false;
  updateUI();

  return true;  
}

bool vsnMethod_timeStep::getStepRange(int* range) const {
  if ( ! range ) return false;

  vsnTimeSeriesDataIF* pData = dynamic_cast<vsnTimeSeriesDataIF*>(p_refData);
  if ( ! pData ) return false;

  size_t numStps = pData->getNumSteps();
  if ( numStps < 1 ) return false;
  if ( ! pData->getTimeStepNo(0, range[0]) ) return false;
  if ( ! pData->getTimeStepNo(numStps -1, range[1]) ) return false;
  return true;
}

void vsnMethod_timeStep::setSkipSteps(const int skip) {
  if ( m_skipSteps == skip ) return;
  m_skipSteps = skip;
  updateUI();
}

int vsnMethod_timeStep::getSkipSteps(bool initialVal) const {
  if ( initialVal ) {
    vsnTimeSeriesDataIF* pData = dynamic_cast<vsnTimeSeriesDataIF*>(p_refData);
    if ( ! pData ) return 1;
    if ( pData->getNumSteps() < 2 ) return 0;
    int stp0, stp1;
    if ( ! pData->getTimeStepNo(0, stp0) ) return 1;
    if ( ! pData->getTimeStepNo(1, stp1) ) return 1;
    return (stp1 - stp0);
  }

  return m_skipSteps;
}

vsnFrontLabel* vsnMethod_timeStep::getFrontLabel() {
  if ( ! m_pFrLbl ) {
    m_pFrLbl = new vsnFrontLabel();
    if ( ! m_pFrLbl ) return NULL;
    m_pFrLbl->setLabelColor(m_colour);
    m_pFrLbl->setPickMode(PT_OBJECT);
    m_pFrLbl->setMethodObj(this);
    vector2 initPos = {0.f, -0.9f}; m_pFrLbl->setPosition(initPos);
    updateLabel();
  }
  return m_pFrLbl;
}

bool vsnMethod_timeStep::showFrontLabel(const bool sfl) {
  vsnFrontLabel* pfl = getFrontLabel();
  if ( ! pfl ) return false;

  // get the scene
  vsnApp* pApp = vsnApp::GetApp();
  if ( ! pApp ) return false;
  size_t nsc = pApp->getNumScene();
  vsnScene* psc = NULL;
  for ( size_t i = 0; i < nsc; i++ ) {
    vsnScene* xsc = pApp->getScene(i);
    if ( ! xsc ) continue;
    if ( xsc->getNode(this->getID()) ) {
      psc = xsc;
      break;
    }
  } // end of for(i)
  if ( ! psc ) return false;

  // show/hide
  psc->delFrontObj(pfl);
  if ( sfl ) psc->addFrontObj(pfl);

  chkNotice();
  return true;
}

bool vsnMethod_timeStep::isShowFrontLabel() const {
  if ( ! m_pFrLbl ) return false;

  // get the scene
  vsnApp* pApp = vsnApp::GetApp();
  if ( ! pApp ) return false;
  size_t nsc = pApp->getNumScene();
  vsnScene* psc = NULL;
  for ( size_t i = 0; i < nsc; i++ ) {
    vsnScene* xsc = pApp->getScene(i);
    if ( ! xsc ) continue;
    if ( xsc->getNode(this->getID()) ) {
      psc = xsc;
      break;
    }
  } // end of for(i)
  if ( ! psc ) return false;

  bool disp_mode = false;
  size_t nfo = psc->getNumFrontObj();
  for ( size_t i = 0; i < nfo; i++ ) {
    vsnFrontLabel* pfl = dynamic_cast<vsnFrontLabel*>(psc->getFrontObj(i));
    if ( ! pfl ) continue;
    if ( pfl == m_pFrLbl ) {
      disp_mode = true;
      break;
    }
  } // end of for(i)

  return disp_mode;
}

void vsnMethod_timeStep::setLabelFmt(const std::string& fmt) {
  if ( m_labelFmt == fmt ) return;
  m_labelFmt = fmt;

  string::size_type sidx = m_labelFmt.find(string("\\_"));
  while ( sidx != string::npos ) {
    m_labelFmt.replace(sidx, 2, string(" "));
    sidx = m_labelFmt.find(string("\\_"));
  }

  updateLabel();
  updateUI();
}

void vsnMethod_timeStep::updateLabel() {
  vsnTimeSeriesDataIF* pData = dynamic_cast<vsnTimeSeriesDataIF*>(p_refData);
  if ( ! pData || ! p_refData->ready() ) return;
  vsnFrontLabel* pfl = getFrontLabel();
  if ( ! pfl ) return;

  size_t stpIdx;
  if ( ! pData->getStepIdx(m_timeStep, stpIdx) ) return;
  float stpTime = pData->getTime(stpIdx);

  string wks(m_labelFmt);
  char txt[64];
  size_t np;

  while ( (np = wks.find("#S")) != string::npos ) {
    sprintf(txt, "%d", m_timeStep);
    wks.replace(np, 2, string(txt));
  } // end of while(#S)

  while ( (np = wks.find("#T")) != string::npos ) {
    if ( np+2 < wks.size() && isdigit(wks[np+2]) ) {
      char txt2[16];
      sprintf(txt2, "%%.%cf", wks[np+2]);
      sprintf(txt, txt2, stpTime);
      wks.replace(np, 3, string(txt));
    } else {
      sprintf(txt, "%g", stpTime);
      wks.replace(np, 2, string(txt));
    }
  } // end of while(#T)

  pfl->setLabelStr(wks);
  chkNotice();
}


/* vsnMethodObj methods */

bool vsnMethod_timeStep::update(const bool force) {
  vsnTimeSeriesDataIF* pData = dynamic_cast<vsnTimeSeriesDataIF*>(p_refData);
  if ( ! pData || ! p_refData->ready() ) return false;

  if ( ! m_onceSetTs || force ) {
    setSkipSteps(getSkipSteps(true));
    if ( ! pData->getTimeStepNo(0, m_timeStep) )
      return false;
  }

  vsnDataReload* pReloader = p_refData->getReloader(false);
  if ( pReloader && pReloader->IsRunning() ) {
    int tgtStep = pData->getNumSteps() - 1;
    if ( tgtStep < 0 ) tgtStep = 0;
    if ( ! pData->getTimeStepNo(tgtStep, m_timeStep) )
      return false;
  }

  size_t stp;
  if ( ! pData->getStepIdx(m_timeStep, stp) ) return false;
  if ( ! pData->setCurrentStepIdx(stp) ) return false;

  updateLabel();

  return true;
}

vsnMethodPP* vsnMethod_timeStep::getParamPanel(wxPanel* pp) {
  vsnMPP_timeStep* pp_timeStep = new vsnMPP_timeStep(pp, this);
  if ( ! pp_timeStep ) return NULL;
  return pp_timeStep;
}

void vsnMethod_timeStep::setBaseColor(const vector4 cv) {
  vsnMethodObj::setBaseColor(cv);
  if ( m_pFrLbl )
    m_pFrLbl->setLabelColor(m_colour);
}

void vsnMethod_timeStep::setShow(const bool mode) {
  vsnMethodObj::setShow(mode);
  if ( m_pFrLbl )
    m_pFrLbl->getPrivateMaterial()->setRenderMode(m_show?m_showType:RT_NONE);
}


/* from vsnTimeSeriesMethodIF */

bool vsnMethod_timeStep::updateStep(const int stp,
                                    const bool force, const bool cascade)
{
  vsnTimeSeriesDataIF* pData = dynamic_cast<vsnTimeSeriesDataIF*>(p_refData);
  if ( ! pData ) return false;
  m_requestedStp = stp;
  if ( m_requestedStp < 0 )
    m_requestedStp = pData->getCurrentStepIdx();

  if ( force ) m_updatedStp = -1;
  if ( m_updatedStp == m_requestedStp ) return true;
  m_updatedStp = -1;

  if ( IsRunning() ) {
    m_updatedStp = m_requestedStp;
    return true;
  }
  
  if ( ! pData->getTimeStepNo(m_requestedStp, m_timeStep) ) return false;
  updateLabel();
  updateUI();
  m_updatedStp = m_requestedStp;
  return true;
}


/* from vsnIoObject */

bool vsnMethod_timeStep::parseXML(xmlNodePtr xnp) {
  static xmlChar* xs;
  xmlNodePtr cur;

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

      if ( xsN == string("label_format") ) {
	string xfmt;
	if ( ! xsV.empty() ) xfmt = xsV.c_str();
	setLabelFmt(xfmt);
      } // end of "label_format"

      if ( xsV.empty() ) {
        ErrMsg(MsgERR, msgHdr +string("no value in param ") +xsN);
        goto _NEXT_XML_NODE;
      }

      if ( xsN == string("current") ) {
	int cts = atoi(xsV.c_str());
	if ( ! setTimeStep(cts) ) {
	  ErrMsg(MsgERR, msgHdr
		 + string("failed to set current time step: ") + xsV);
          goto _NEXT_XML_NODE;
	}
      } // end of "current"
      else if ( xsN == string("skip_steps") ) {
	int sts = atoi(xsV.c_str());
	setSkipSteps(sts);
      } // end of "skip_steps"
      else if ( xsN == string("show_label") ) {
	bool slm;
	if ( xsV == string("yes") ) slm = true;
        else if ( xsV == string("no") ) slm = false;
        else {
          ErrMsg(MsgERR, msgHdr + string("invalid value in param show_label"));
          goto _NEXT_XML_NODE;
        }
	if ( ! showFrontLabel(slm) ) {
	  ErrMsg(MsgERR, msgHdr + string("can't set show_label"));
          goto _NEXT_XML_NODE;
	}
      } // end of "show_label"
      else if ( xsN == string("label_pos") ) {
	vector2 lpos = {0.f, 0.f};
	string wks; char c;
	istringstream iss(xsV);
	while( iss.get(c) && c != ':' ) wks.push_back(c);
	if ( ! wks.empty() ) lpos[0] = (float)atof(wks.c_str());
	wks = "";
	while( iss.get(c) ) wks.push_back(c);
	if ( ! wks.empty() ) lpos[1] = (float)atof(wks.c_str());

	vsnFrontLabel* pfl = getFrontLabel();
	if ( ! pfl ) {
	  ErrMsg(MsgERR, msgHdr + string("can't get front_label"));
          goto _NEXT_XML_NODE;
	}
	pfl->setPosition(lpos);
      } // end of "label_pos"
      else if ( xsN == string("label_size") ) {
	float lsz = (float)atof(xsV.c_str());
	if ( lsz < 0.f ) {
	  ErrMsg(MsgERR, msgHdr + string("invalid value in param label_size"));
          goto _NEXT_XML_NODE;
	}
	vsnFrontLabel* pfl = getFrontLabel();
	if ( ! pfl ) {
	  ErrMsg(MsgERR, msgHdr + string("can't get front_label"));
          goto _NEXT_XML_NODE;
	}
	pfl->setLabelScale(lsz);
      } // end of "label_size"
      else if ( xsN == string("anim_loop") ) {
	bool alm;
	if ( xsV == string("yes") ) alm = true;
	else if ( xsV == string("no") ) alm = false;
	else {
	  ErrMsg(MsgERR, msgHdr + string("invalid value in param anim_loop"));
          goto _NEXT_XML_NODE;
	}
	anim_setLoop(alm);
      } // end of "anim_loop"
      else if ( xsN == string("anim_sshot") ) {
	bool ssm;
	if ( xsV == string("yes") ) ssm = true;
	else if ( xsV == string("no") ) ssm = false;
	else {
	  ErrMsg(MsgERR, msgHdr + string("invalid value in param anim_sshot"));
          goto _NEXT_XML_NODE;
	}
	anim_setShotMode(ssm);
      } // end of "anim_sshot"
      else if ( xsN == string("anim_sshot_path") ) {
	anim_setShotPath(xsV);
      } // end of "anim_sshot_path"
      else if ( xsN == string("anim_sshot_view") ) {
	anim_setShotView(xsV);
      } // end of "anim_sshot_view"
    } // end of param

  _NEXT_XML_NODE:
    cur = cur->next;
  } // end of while(cur)

  return true;
}

bool vsnMethod_timeStep::outputXML(std::ostream& os, const size_t ts) {
  string idts;
  for ( int i = 0; i < ts; i++ ) idts.push_back(' ');
  string idts_2 = idts; idts_2.push_back(' '); idts_2.push_back(' ');
  bool ret= true;
  string msgHdr
    = getMethodType() +string("[") +getName() +string("]: outputXML: ");

  vsnTimeSeriesDataIF* pData = dynamic_cast<vsnTimeSeriesDataIF*>(p_refData);
  if ( ! pData || ! p_refData->ready() ) {
    ErrMsg(MsgERR, msgHdr + string("can't get valid data"));
    return false;
  }
  int val;

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
  // current
  pData->getTimeStepNo(0, val);
  if ( m_timeStep != val ) {
    os << idts_2 << "<param name=\"current\" value=\""
       << m_timeStep << "\" />" << endl;
  }

  // skip_steps
  if ( m_skipSteps != getSkipSteps(true) ) {
    os << idts_2 << "<param name=\"skip_steps\" value=\""
       << m_skipSteps << "\" />" << endl;
  }

  // labels
  if ( isShowFrontLabel() ) {
    os << idts_2 << "<param name=\"show_label\" value=\"yes\" />" << endl;
  }
  if ( m_labelFmt != string("step = #S") ) {
    os << idts_2 << "<param name=\"label_format\" value=\""
       << ConvXmlEntChars(m_labelFmt) << "\" />" << endl;
  }
  if ( m_pFrLbl ) {
    vector2 lpos; m_pFrLbl->getPosition(lpos);
    float lsz = m_pFrLbl->getLabelScale();
    if ( lpos[0] != 0.f || lpos[1] != 0.f )
      os << idts_2 << "<param name=\"label_pos\" value=\""
	 << lpos[0] << ":" << lpos[1] << "\" />" << endl;
    if ( lsz != 0.05f )
      os << idts_2 << "<param name=\"label_size\" value=\""
	 << lsz << "\" />" << endl;
  } // end of if(m_pFrLbl)

  // animation params
  if ( m_animLoop ) {
    os << idts_2 << "<param name=\"anim_loop\" value=\"yes\" />" << endl;
  }
  if ( m_animSshot ) {
    os << idts_2 << "<param name=\"anim_sshot\" value=\"yes\" />" << endl;
  }
  if ( ! m_animShotPath.empty() ) {
    os << idts_2 << "<param name=\"anim_sshot_path\" value=\""
       << m_animShotPath << "\" />" << endl;
  }
  vsnViewFrame* pvf0 = vsnApp::GetApp()->getViewFrame(0);
  if ( pvf0 && pvf0->getName() != m_animShotView ) {
    os << idts_2 << "<param name=\"anim_sshot_view\" value=\""
       << m_animShotView << "\" />" << endl;
  }

  os << idts << "</method>" << endl;

  return ret;
}

bool vsnMethod_timeStep::commandXML(xmlNodePtr xnp) {
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
  if ( nameStr == "set_label_format" ) {
    string xfmt;
    if ( ! valueStr.empty() ) xfmt = valueStr.c_str();
    setLabelFmt(xfmt);
    return true;
  } // end of "set_label_format"
  else if ( nameStr == "anim_play" ) {
    anim_play();
    return true;
  } // end of "anim_play"
  else if ( nameStr == "anim_stop" ) {
    anim_stop();
    return true;
  } // end of "anim_stop"

  // do the command
  if ( valueStr.empty() ) {
    ErrMsg(MsgERR, msgHdr + "invalid command: " + nameStr + ": no value");
    return false;
  }

  if ( nameStr == "set_current" ) {
    int cts = atoi(valueStr.c_str());
    if ( ! setTimeStep(cts) ) {
      ErrMsg(MsgERR, msgHdr +
             string("command set_current: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_current"
  else if ( nameStr == "set_skip_steps" ) {
    int sts = atoi(valueStr.c_str());
    setSkipSteps(sts);
  } // end of "set_skip_steps"
  else if ( nameStr == "skip" ) {
    int newStep;
    if ( valueStr == "forward" ||
	 valueStr == "Forward" || valueStr == "FORWARD" )
      newStep = getTimeStep() + getSkipSteps();
    else if ( valueStr == "backward" ||
	 valueStr == "Backward" || valueStr == "BACKWARD" )
      newStep = getTimeStep() - getSkipSteps();
    else {
      ErrMsg(MsgERR, msgHdr +
             string("command skip: invalid value") + valueStr);
      return false;
    }
    int range[2];
    if ( ! getStepRange(range) ) {
      ErrMsg(MsgERR, msgHdr + string("command skip: can't get step-range"));
      return false;
    }
    if ( newStep < range[0] ) newStep = range[0];
    if ( newStep > range[1] ) newStep = range[1];
    if ( ! setTimeStep(newStep) ) {
      ErrMsg(MsgERR, msgHdr + string("command skip: set time-step failed"));
      return false;
    }
  } // end of "skip"
  else if ( nameStr == "set_show_label" ) {
    bool slm;
    if ( valueStr == string("yes") ) slm = true;
    else if ( valueStr == string("no") ) slm = false;
    else {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_show_label: set failed: invalid value"));
      return false;
    }
    if ( ! showFrontLabel(slm) ) {
      ErrMsg(MsgERR, msgHdr + string("command set_show_label: set failed"));
      return false;
    }
  } // end of "set_show_label"
  else if ( nameStr == "set_label_pos" ) {
    vector2 lpos = {0.f, 0.f};
    string wks; char c;
    istringstream iss(valueStr);
    while( iss.get(c) && c != ':' ) wks.push_back(c);
    if ( ! wks.empty() ) lpos[0] = (float)atof(wks.c_str());
    wks = "";
    while( iss.get(c) ) wks.push_back(c);
    if ( ! wks.empty() ) lpos[1] = (float)atof(wks.c_str());
    
    vsnFrontLabel* pfl = getFrontLabel();
    if ( ! pfl ) {
      ErrMsg(MsgERR, msgHdr + string("command set_label_pos: set failed"));
      return false;
    }
    pfl->setPosition(lpos);
  } // end of "set_label_pos"
  else if ( nameStr == "set_label_size" ) {
    float lsz = (float)atof(valueStr.c_str());
    if ( lsz < 0.f ) {
      ErrMsg(MsgERR, msgHdr + string("command set_label_size: invalid value"));
      return false;
    }
    vsnFrontLabel* pfl = getFrontLabel();
    if ( ! pfl ) {
      ErrMsg(MsgERR, msgHdr + string("command set_label_size: set failed"));
      return false;
    }
    pfl->setLabelScale(lsz);
  } // end of "set_label_size"
  else if ( nameStr == "set_anim_loop" ) {
    bool alm;
    if ( valueStr == string("yes") ) alm = true;
    else if ( valueStr == string("no") ) alm = false;
    else {
      ErrMsg(MsgERR, msgHdr + string("command set_anim_loop: invalid value"));
      return false;
    }
    anim_setLoop(alm);
  } // end of "set_anim_loop"
  else if ( nameStr == "set_anim_sshot" ) {
    bool ssm;
    if ( valueStr == string("yes") ) ssm = true;
    else if ( valueStr == string("no") ) ssm = false;
    else {
      ErrMsg(MsgERR, msgHdr + string("command set_anim_sshot: invalid value"));
      return false;
    }
    anim_setShotMode(ssm);
  } // end of "set_anim_sshot"
  else if ( nameStr == "set_anim_sshot_path" ) {
    anim_setShotPath(valueStr);
  } // end of "set_anim_sshot_path"
  else if ( nameStr == "set_anim_sshot_view" ) {
    anim_setShotView(valueStr);
  } // end of "set_anim_sshot_view"
  else {
    // not my command
    ErrMsg(MsgERR, msgHdr + string("unknown command: ") + nameStr);
    return false;
  }

  return true;
}


/* animation interface */

void vsnMethod_timeStep::anim_play(const bool rewind) {
  Stop();

  int stpRange[2];
  if ( ! getStepRange(stpRange) ) {
    ErrMsg(MsgERR, string("method timeStep: AnimPlay: get range failed"));
    return;
  }
  m_animCtx.reset(); m_animIdx = 0;
  if ( m_animShotPath.empty() && m_animSshot ) {
    ErrMsg(MsgERR, string("method timeStep: AnimPlay: ")
             + string("no screenshot file specified, turned to off"));
    m_animSshot = false;
    updateUI();
  }
  if ( m_animSshot ) {
    if ( ! m_animCtx.setup(m_animShotPath) ) {
      ErrMsg(MsgERR, string("method timeStep: AnimPlay: ")
             + string("can't setup screenshot, turned to off"));
      m_animSshot = false;
      updateUI();
    }

    // override check for movie file
    if ( m_animCtx.m_mkMov ) {
      FILE* mvf = fopen(m_animCtx.m_sfMov.c_str(), "r");
      if ( mvf ) {
        fclose(mvf);
        string msg = "The specified movie file has already existed\n  ";
        msg += m_animCtx.m_sfMov;
        msg += "\n\nAre you sure to override (or screenshot turned to off) ?\n";
        wxMessageDialog dlg(NULL, vsnApp::ConvSysToWx(msg),
                            wxT("AnimPlay"), vsn_wxOK_CANCEL|wxICON_QUESTION);
        if ( dlg.ShowModal() == vsn_wxIDOK ) {
          wxRemoveFile(vsnApp::ConvSysToWx(m_animCtx.m_sfMov));
        }
        else {
	  m_animSshot = false;
	  updateUI();
        }
      }
    }
  }

  // start the timer
  m_needRewind = rewind;
  if ( m_timeStep >= stpRange[1] || m_animSshot )
    m_needRewind = true;
  Start(ANIM_INTERVAL);
}

void vsnMethod_timeStep::anim_stop() {
  Stop();

  if ( m_animSshot && m_animCtx.m_mkMov ) {
    deque<string>::iterator it;
    for ( it = m_animCtx.m_movFrames.begin();
          it != m_animCtx.m_movFrames.end(); it++ ) {
      wxRemoveFile(vsnApp::ConvSysToWx(*it));
    }
  }
  m_animCtx.reset();
  m_animIdx = 0;
  updateUI();
}

void vsnMethod_timeStep::anim_setLoop(const bool mode) {
  if ( m_animLoop == mode ) return;
  m_animLoop = mode;
  updateUI();
}

void vsnMethod_timeStep::anim_setShotMode(const bool mode) {
  if ( m_animSshot == mode ) return;
  if ( IsRunning() )
    return;
  m_animSshot = mode;
  updateUI();
}

void vsnMethod_timeStep::anim_setShotPath(const std::string& spath) {
  if ( m_animShotPath == spath ) return;
  m_animShotPath = spath;
  updateUI();
}

void vsnMethod_timeStep::anim_setShotView(const std::string& sview) {
  if ( m_animShotView == sview ) return;
  m_animShotView = sview;
  updateUI();
}

void vsnMethod_timeStep::Notify() {
  animTimerJob();
}

void vsnMethod_timeStep::animTimerJob() {
  vsnApp* pApp = vsnApp::GetApp();
  if ( ! pApp ) return;

  int stpRange[2];
  if ( ! getStepRange(stpRange) ) {
    ErrMsg(MsgERR, string("method timeStep: AnimPlay: get range failed"));
    return;
  }

  if ( m_animInProcMutex.TryLock() == wxMUTEX_BUSY )
    return;
  //-------------------- LOCK --------------------

  int newStp;
  if ( m_needRewind ) {
    newStp = stpRange[0];
    m_needRewind = false;
  } else {
    newStp = getTimeStep() + getSkipSteps();
  }
  if ( newStp >= stpRange[1] ) {
    Stop();
    newStp = stpRange[1];
  }
  if ( setTimeStep(newStp) )
    chkNotice();

  if ( m_animSshot ) {
    vsnViewFrame* pvf
      = dynamic_cast<vsnViewFrame*>(pApp->getViewFrame(m_animShotView));
    vsnGfxView* pgv = (pvf ? pvf->getGfxView() : NULL);
    if ( ! pgv ) {
      ErrMsg(MsgERR, string("method timeStep: AnimPlay: ")
	     + string("can't get GFX View, screenshot turned to off"));
      m_animSshot = false;
      updateUI();
    }
    else {
      string sspath = m_animCtx.getPath(m_animIdx++);
      if ( ! pgv->screenShot(sspath) ) {
	ErrMsg(MsgERR, string("method timeStep: AnimPlay: ")
	       + string("can't save screenshot, turned to off"));
	m_animSshot = false;
	updateUI();
      }
      else
	m_animCtx.m_movFrames.push_back(sspath);
    }
  }

  //-------------------- UNLOCK --------------------
  m_animInProcMutex.Unlock();
  pApp->Yield();

  // post animation process
  if ( ! IsRunning() ) {
    // create movie if need
    if ( m_animSshot && m_animCtx.m_mkMov ) {
      if ( ! m_animCtx.makeMovieFile() ) {
	ErrMsg(MsgERR, string("method timeStep: "
			      "can't create movie file"));
      }
    }

    // reset animation context
    m_animCtx.reset(); m_animIdx = 0;
    m_animSshot = false;
    updateUI();

    // loop ?
    if ( m_animLoop && m_timeStep >= stpRange[1] ) {
      m_needRewind = true;
      Start(ANIM_INTERVAL);      
    }
  }
}
