//
// vsnGfxOprProp
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

#include "vsnGfxOprProp.h"
#include "vsnViewFrame.h"
#include "vsnError.h"

using namespace std;
using namespace VFR;
using namespace VSN;


//----------------------------------------------------------------
// class vsnGfxOprProp
//----------------------------------------------------------------

/* constructors, destructor */

vsnGfxOprProp::vsnGfxOprProp(wxWindow* parent)
  : wxPanel(parent, wxID_ANY)
{
  assert(parent);

  wxBoxSizer *sizerTop = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *sizerH;
  wxBoxSizer *sizerS;

  // prepare widgets
  wxString ritems[] = {wxString(wxT("left")),
		       wxString(wxT("middle")), wxString(wxT("right"))};

  m_pRotate_CtrlChk = new wxCheckBox(this, GfxOprProp_Rotate_CtrlChk,
				     wxT("Ctrl"));
  m_pRotate_ShiftChk = new wxCheckBox(this, GfxOprProp_Rotate_ShiftChk,
				      wxT("Shift"));
  m_pRotate_MBtnRadio = new wxRadioBox(this, GfxOprProp_Rotate_MBtnRadio,
				       wxT("Rotate"), wxDefaultPosition,
				       wxDefaultSize,
				       3, ritems, 1, wxRA_SPECIFY_COLS);

  m_pRoll_CtrlChk = new wxCheckBox(this, GfxOprProp_Roll_CtrlChk,
				   wxT("Ctrl"));
  m_pRoll_ShiftChk = new wxCheckBox(this, GfxOprProp_Roll_ShiftChk,
				    wxT("Shift"));
  m_pRoll_MBtnRadio = new wxRadioBox(this, GfxOprProp_Roll_MBtnRadio,
				     wxT("Roll"), wxDefaultPosition,
				     wxDefaultSize,
				     3, ritems, 1, wxRA_SPECIFY_COLS);

  m_pScale_CtrlChk = new wxCheckBox(this, GfxOprProp_Scale_CtrlChk,
				    wxT("Ctrl"));
  m_pScale_ShiftChk = new wxCheckBox(this, GfxOprProp_Scale_ShiftChk,
				     wxT("Shift"));
  m_pScale_MBtnRadio = new wxRadioBox(this, GfxOprProp_Scale_MBtnRadio,
				      wxT("Scale"), wxDefaultPosition,
				      wxDefaultSize,
				      3, ritems, 1, wxRA_SPECIFY_COLS);

  m_pTranslate_CtrlChk = new wxCheckBox(this, GfxOprProp_Translate_CtrlChk,
					wxT("Ctrl"));
  m_pTranslate_ShiftChk = new wxCheckBox(this, GfxOprProp_Translate_ShiftChk,
					 wxT("Shift"));
  m_pTranslate_MBtnRadio = new wxRadioBox(this, GfxOprProp_Translate_MBtnRadio,
					  wxT("Translate"), wxDefaultPosition,
					  wxDefaultSize,
					  3, ritems, 1, wxRA_SPECIFY_COLS);

  m_pSelect_CtrlChk = new wxCheckBox(this, GfxOprProp_Select_CtrlChk,
				     wxT("Ctrl"));
  m_pSelect_ShiftChk = new wxCheckBox(this, GfxOprProp_Select_ShiftChk,
				      wxT("Shift"));
  m_pSelect_MBtnRadio = new wxRadioBox(this, GfxOprProp_Select_MBtnRadio,
				       wxT("Select"), wxDefaultPosition,
				       wxDefaultSize,
				       3, ritems, 1, wxRA_SPECIFY_COLS);

  m_pSweepZoom_CtrlChk = new wxCheckBox(this, GfxOprProp_SweepZoom_CtrlChk,
					wxT("Ctrl"));
  m_pSweepZoom_ShiftChk = new wxCheckBox(this, GfxOprProp_SweepZoom_ShiftChk,
					 wxT("Shift"));
  m_pSweepZoom_MBtnRadio = new wxRadioBox(this, GfxOprProp_SweepZoom_MBtnRadio,
					  wxT("Sweep zoom"), wxDefaultPosition,
					  wxDefaultSize,
					  3, ritems, 1, wxRA_SPECIFY_COLS);
  // layout
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  sizerTop->Add(sizerH, 0, wxALIGN_RIGHT|wxALL, 5);

  // rotate
  sizerS = new wxBoxSizer(wxVERTICAL);
  sizerH->Add(sizerS, 0, wxALL, 3);
  sizerS->Add(m_pRotate_MBtnRadio, 0, wxALL, 3);
  sizerS->Add(m_pRotate_CtrlChk, 0, wxALL, 3);
  sizerS->Add(m_pRotate_ShiftChk, 0, wxALL, 3);

  // roll
  sizerS = new wxBoxSizer(wxVERTICAL);
  sizerH->Add(sizerS, 0, wxALL, 3);
  sizerS->Add(m_pRoll_MBtnRadio, 0, wxALL, 3);
  sizerS->Add(m_pRoll_CtrlChk, 0, wxALL, 3);
  sizerS->Add(m_pRoll_ShiftChk, 0, wxALL, 3);

  // scale
  sizerS = new wxBoxSizer(wxVERTICAL);
  sizerH->Add(sizerS, 0, wxALL, 3);
  sizerS->Add(m_pScale_MBtnRadio, 0, wxALL, 3);
  sizerS->Add(m_pScale_CtrlChk, 0, wxALL, 3);
  sizerS->Add(m_pScale_ShiftChk, 0, wxALL, 3);

  // translate
  sizerS = new wxBoxSizer(wxVERTICAL);
  sizerH->Add(sizerS, 0, wxALL, 3);
  sizerS->Add(m_pTranslate_MBtnRadio, 0, wxALL, 3);
  sizerS->Add(m_pTranslate_CtrlChk, 0, wxALL, 3);
  sizerS->Add(m_pTranslate_ShiftChk, 0, wxALL, 3);

  // select
  sizerS = new wxBoxSizer(wxVERTICAL);
  sizerH->Add(sizerS, 0, wxALL, 3);
  sizerS->Add(m_pSelect_MBtnRadio, 0, wxALL, 3);
  sizerS->Add(m_pSelect_CtrlChk, 0, wxALL, 3);
  sizerS->Add(m_pSelect_ShiftChk, 0, wxALL, 3);

  // sweep zoom
  sizerS = new wxBoxSizer(wxVERTICAL);
  sizerH->Add(sizerS, 0, wxALL, 3);
  sizerS->Add(m_pSweepZoom_MBtnRadio, 0, wxALL, 3);
  sizerS->Add(m_pSweepZoom_CtrlChk, 0, wxALL, 3);
  sizerS->Add(m_pSweepZoom_ShiftChk, 0, wxALL, 3);

  // post process
  SetAutoLayout(TRUE);
  SetSizer(sizerTop);
  sizerTop->SetSizeHints(this);
  sizerTop->Fit(this);

  (void)update();
}

vsnGfxOprProp::~vsnGfxOprProp() {
}


/* interface */

bool vsnGfxOprProp::update() {
  if ( ! m_pRotate_CtrlChk || ! m_pRotate_ShiftChk || ! m_pRotate_MBtnRadio ||
       ! m_pRoll_CtrlChk || ! m_pRoll_ShiftChk || ! m_pRoll_MBtnRadio ||
       ! m_pScale_CtrlChk || ! m_pScale_ShiftChk || ! m_pScale_MBtnRadio ||
       ! m_pTranslate_CtrlChk || ! m_pTranslate_ShiftChk ||
       ! m_pTranslate_MBtnRadio ||
       ! m_pSelect_CtrlChk || ! m_pSelect_ShiftChk || ! m_pSelect_MBtnRadio ||
       ! m_pSweepZoom_CtrlChk || ! m_pSweepZoom_ShiftChk ||
       ! m_pSweepZoom_MBtnRadio )
    return false;

  vsnGfxOprOrientation oprOri = vsnApp::GetApp()->getOprOrientation();
  bool val;

  // rotate
  val = (oprOri.m_oprLst[OprRotate].m_modKey & ModKeyCtrl);
  m_pRotate_CtrlChk->SetValue(val);
  val = (oprOri.m_oprLst[OprRotate].m_modKey & ModKeyShift);
  m_pRotate_ShiftChk->SetValue(val);
  m_pRotate_MBtnRadio->SetSelection(oprOri.m_oprLst[OprRotate].m_mouseButton);

  // roll
  val = (oprOri.m_oprLst[OprRoll].m_modKey & ModKeyCtrl);
  m_pRoll_CtrlChk->SetValue(val);
  val = (oprOri.m_oprLst[OprRoll].m_modKey & ModKeyShift);
  m_pRoll_ShiftChk->SetValue(val);
  m_pRoll_MBtnRadio->SetSelection(oprOri.m_oprLst[OprRoll].m_mouseButton);

  // scale
  val = (oprOri.m_oprLst[OprScale].m_modKey & ModKeyCtrl);
  m_pScale_CtrlChk->SetValue(val);
  val = (oprOri.m_oprLst[OprScale].m_modKey & ModKeyShift);
  m_pScale_ShiftChk->SetValue(val);
  m_pScale_MBtnRadio->SetSelection(oprOri.m_oprLst[OprScale].m_mouseButton);

  // translate
  val = (oprOri.m_oprLst[OprTranslate].m_modKey & ModKeyCtrl);
  m_pTranslate_CtrlChk->SetValue(val);
  val = (oprOri.m_oprLst[OprTranslate].m_modKey & ModKeyShift);
  m_pTranslate_ShiftChk->SetValue(val);
  m_pTranslate_MBtnRadio->
    SetSelection(oprOri.m_oprLst[OprTranslate].m_mouseButton);

  // select
  val = (oprOri.m_oprLst[OprSelect].m_modKey & ModKeyCtrl);
  m_pSelect_CtrlChk->SetValue(val);
  val = (oprOri.m_oprLst[OprSelect].m_modKey & ModKeyShift);
  m_pSelect_ShiftChk->SetValue(val);
  m_pSelect_MBtnRadio->SetSelection(oprOri.m_oprLst[OprSelect].m_mouseButton);

  // sweep zoom
  val = (oprOri.m_oprLst[OprSweepZoom].m_modKey & ModKeyCtrl);
  m_pSweepZoom_CtrlChk->SetValue(val);
  val = (oprOri.m_oprLst[OprSweepZoom].m_modKey & ModKeyShift);
  m_pSweepZoom_ShiftChk->SetValue(val);
  m_pSweepZoom_MBtnRadio->
    SetSelection(oprOri.m_oprLst[OprSweepZoom].m_mouseButton);

  return true;
}

bool vsnGfxOprProp::apply()
{
  if ( ! m_pRotate_CtrlChk || ! m_pRotate_ShiftChk || ! m_pRotate_MBtnRadio ||
       ! m_pRoll_CtrlChk || ! m_pRoll_ShiftChk || ! m_pRoll_MBtnRadio ||
       ! m_pScale_CtrlChk || ! m_pScale_ShiftChk || ! m_pScale_MBtnRadio ||
       ! m_pTranslate_CtrlChk || ! m_pTranslate_ShiftChk ||
       ! m_pTranslate_MBtnRadio ||
       ! m_pSelect_CtrlChk || ! m_pSelect_ShiftChk || ! m_pSelect_MBtnRadio ||
       ! m_pSweepZoom_CtrlChk || ! m_pSweepZoom_ShiftChk ||
       ! m_pSweepZoom_MBtnRadio )
    return false;

  vsnApp* pApp = vsnApp::GetApp();
  if ( ! pApp ) return false;
  vsnGfxOprOrientation oprOri(false);

  // rotate
  if ( m_pRotate_CtrlChk->GetValue() )
    oprOri.m_oprLst[OprRotate].m_modKey |= ModKeyCtrl;
  if ( m_pRotate_ShiftChk->GetValue() )
    oprOri.m_oprLst[OprRotate].m_modKey |= ModKeyShift;
  oprOri.m_oprLst[OprRotate].m_mouseButton
    = m_pRotate_MBtnRadio->GetSelection();

  // roll
  if ( m_pRoll_CtrlChk->GetValue() )
    oprOri.m_oprLst[OprRoll].m_modKey |= ModKeyCtrl;
  if ( m_pRoll_ShiftChk->GetValue() )
    oprOri.m_oprLst[OprRoll].m_modKey |= ModKeyShift;
  oprOri.m_oprLst[OprRoll].m_mouseButton
    = m_pRoll_MBtnRadio->GetSelection();

  // scale
  if ( m_pScale_CtrlChk->GetValue() )
    oprOri.m_oprLst[OprScale].m_modKey |= ModKeyCtrl;
  if ( m_pScale_ShiftChk->GetValue() )
    oprOri.m_oprLst[OprScale].m_modKey |= ModKeyShift;
  oprOri.m_oprLst[OprScale].m_mouseButton
    = m_pScale_MBtnRadio->GetSelection();

  // translate
  if ( m_pTranslate_CtrlChk->GetValue() )
    oprOri.m_oprLst[OprTranslate].m_modKey |= ModKeyCtrl;
  if ( m_pTranslate_ShiftChk->GetValue() )
    oprOri.m_oprLst[OprTranslate].m_modKey |= ModKeyShift;
  oprOri.m_oprLst[OprTranslate].m_mouseButton
    = m_pTranslate_MBtnRadio->GetSelection();

  // select
  if ( m_pSelect_CtrlChk->GetValue() )
    oprOri.m_oprLst[OprSelect].m_modKey |= ModKeyCtrl;
  if ( m_pSelect_ShiftChk->GetValue() )
    oprOri.m_oprLst[OprSelect].m_modKey |= ModKeyShift;
  oprOri.m_oprLst[OprSelect].m_mouseButton
    = m_pSelect_MBtnRadio->GetSelection();

  // sweep zoom
  if ( m_pSweepZoom_CtrlChk->GetValue() )
    oprOri.m_oprLst[OprSweepZoom].m_modKey |= ModKeyCtrl;
  if ( m_pSweepZoom_ShiftChk->GetValue() )
    oprOri.m_oprLst[OprSweepZoom].m_modKey |= ModKeyShift;
  oprOri.m_oprLst[OprSweepZoom].m_mouseButton
    = m_pSweepZoom_MBtnRadio->GetSelection();

  if ( ! pApp->setOprOrientation(oprOri) ) {
    ErrMsg(MsgERR, "Setup Operation: some operations are conflicted");
    return false;
  }

  return true;
}
