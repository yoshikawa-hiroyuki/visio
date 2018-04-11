//
// vsnMethod_orthoProbe
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

#include "vsnMethod_Sv_orthoProbe.h"
#include "vsnOctTree.h" // for decomp probeIdx
#include "vsnError.h"

using namespace std;
using namespace CES;
using namespace VSN;


//----------------------------------------------------------------
// class vsnMPP_Sv_orthoProbe::OrthoProbeKeyAct
//----------------------------------------------------------------
vsnMPP_Sv_orthoProbe::OrthoProbeKeyAct::
OrthoProbeKeyAct(vsnGfxView* pgv, vsnMethod_Sv_orthoProbe* pm)
  : vsnGfxAct_KeyIn(pgv), p_method(pm) {
}

void vsnMPP_Sv_orthoProbe::OrthoProbeKeyAct::
setOrthoProbeMethod(vsnMethod_Sv_orthoProbe* pm) {
  if ( p_method == pm ) return;
  p_method = pm;
}

void vsnMPP_Sv_orthoProbe::OrthoProbeKeyAct::execute(vfrEvent& e) {
  if ( ! p_method ) return;
  if ( ! e.isKeyEvent() ) return;

  int saxis = p_method->getSliceAxis();
  Vec3<size_t> pidx = p_method->getProbeIdx();
  wxCommandEvent dmyEv;

  switch ( e.getKey() ) {
  case VFRKC_LEFT:
    switch ( saxis ) {
    case 0: // YZ
      if ( pidx[1] > 0 ) pidx[1] = pidx[1] - 1; break;
    case 1: // ZX
      if ( pidx[2] > 0 ) pidx[2] = pidx[2] - 1; break;
    case 2: // XY
      if ( pidx[0] > 0 ) pidx[0] = pidx[0] - 1; break;
    default: return;
    } // end of switch(saxis)
    break;
  case VFRKC_RIGHT:
    switch ( saxis ) {
    case 0: // YZ
      pidx[1] = pidx[1] + 1; break;
    case 1: // ZX
      pidx[2] = pidx[2] + 1; break;
    case 2: // XY
      pidx[0] = pidx[0] + 1; break;
    default: return;
    } // end of switch(saxis)
    break;
  case VFRKC_DOWN:
    switch ( saxis ) {
    case 0: // YZ
      if ( pidx[2] > 0 ) pidx[2] = pidx[2] - 1; break;
    case 1: // ZX
      if ( pidx[0] > 0 ) pidx[0] = pidx[0] - 1; break;
    case 2: // XY
      if ( pidx[1] > 0 ) pidx[1] = pidx[1] - 1; break;
    default: return;
    } // end of switch(saxis)
    break;
  case VFRKC_UP:
    switch ( saxis ) {
    case 0: // YZ
      pidx[2] = pidx[2] + 1; break;
    case 1: // ZX
      pidx[0] = pidx[0] + 1; break;
    case 2: // XY
      pidx[1] = pidx[1] + 1; break;
    default: return;
    } // end of switch(saxis)
    break;
  case VFRKC_PAGEDOWN: case VFRKC_MINUS:
    switch ( saxis ) {
    case 0: // YZ
      if ( pidx[0] > 0 ) pidx[0] = pidx[0] - 1; break;
    case 1: // ZX
      if ( pidx[1] > 0 ) pidx[1] = pidx[1] - 1; break;
    case 2: // XY
      if ( pidx[2] > 0 ) pidx[2] = pidx[2] - 1; break;
    default: return;
    } // end of switch(saxis)
    break;
  case VFRKC_PAGEUP: case VFRKC_PLUS:
    switch ( saxis ) {
    case 0: // YZ
      pidx[0] = pidx[0] + 1; break;
    case 1: // ZX
      pidx[1] = pidx[1] + 1; break;
    case 2: // XY
      pidx[2] = pidx[2] + 1; break;
    default: return;
    } // end of switch(saxis)
    break;
  default:
    vsnGfxAct_KeyIn::execute(e);
    return;
  } // end of switch(kc)

  (void)p_method->setProbeIdx(pidx);
}


//----------------------------------------------------------------
// class vsnMPP_Sv_orthoProbe::OrthoProbeClickAct
//----------------------------------------------------------------
vsnMPP_Sv_orthoProbe::OrthoProbeClickAct::
OrthoProbeClickAct(vsnGfxView* pgv, vsnMethod_Sv_orthoProbe* pm)
  : vsnGfxBaseAct(pgv), p_method(pm) {
}

void vsnMPP_Sv_orthoProbe::OrthoProbeClickAct::
setOrthoProbeMethod(vsnMethod_Sv_orthoProbe* pm) {
  if ( p_method == pm ) return;
  p_method = pm;
}

void vsnMPP_Sv_orthoProbe::OrthoProbeClickAct::execute(vfrEvent& e) {
  if ( ! e.isClickEvent() ) return;

  // codes below coied from vsnGfxAct_Click::execute()
  if ( p_gfxView )
    p_gfxView->getDrawArea()->getCanvas()->SetFocus();

  vfrScreen* screen = (vfrScreen*)e.getScreen();
  if ( ! screen ) return;
  vsnMethod_Sv_orthoProbe* pm
    = dynamic_cast<vsnMethod_Sv_orthoProbe*>(p_method);

  Point2 cp = screen->getRelativePoint(e.getMPoint());
  (void)pm->clickSelect(cp, screen);
}


//----------------------------------------------------------------
// class vsnMPP_Sv_orthoProbe
//----------------------------------------------------------------

BEGIN_EVENT_TABLE(vsnMPP_Sv_orthoProbe, wxPanel)
  EVT_RADIOBOX(MPP_Sv_orthoProbe_AxisRadio,
	       vsnMPP_Sv_orthoProbe::OnAxisRadio)
  EVT_TEXT_ENTER(MPP_Sv_orthoProbe_ITxt, vsnMPP_Sv_orthoProbe::OnIdxTxt)
  EVT_TEXT_ENTER(MPP_Sv_orthoProbe_JTxt, vsnMPP_Sv_orthoProbe::OnIdxTxt)
  EVT_TEXT_ENTER(MPP_Sv_orthoProbe_KTxt, vsnMPP_Sv_orthoProbe::OnIdxTxt)
  EVT_TEXT_ENTER(MPP_Sv_orthoProbe_PointSizeTxt,
		 vsnMPP_Sv_orthoProbe::OnPointSizeTxt)
  EVT_TEXT_ENTER(MPP_Sv_orthoProbe_GlyphBiasTxt,
		 vsnMPP_Sv_orthoProbe::OnGlyphBiasTxt)
  EVT_BUTTON(MPP_Sv_orthoProbe_LeftBtn, vsnMPP_Sv_orthoProbe::OnLeftBtn)
  EVT_BUTTON(MPP_Sv_orthoProbe_RightBtn, vsnMPP_Sv_orthoProbe::OnRightBtn)
  EVT_BUTTON(MPP_Sv_orthoProbe_DownBtn, vsnMPP_Sv_orthoProbe::OnDownBtn)
  EVT_BUTTON(MPP_Sv_orthoProbe_UpBtn, vsnMPP_Sv_orthoProbe::OnUpBtn)
  EVT_BUTTON(MPP_Sv_orthoProbe_MinusBtn, vsnMPP_Sv_orthoProbe::OnMinusBtn)
  EVT_BUTTON(MPP_Sv_orthoProbe_PlusBtn, vsnMPP_Sv_orthoProbe::OnPlusBtn)
  EVT_CHAR(vsnMPP_Sv_orthoProbe::OnChar)
END_EVENT_TABLE()


/* index macros */

#define IDX1(d,i,j,k) ((k)*(d)[0]*(d)[1] +(j)*(d)[0] +(i))
#define IDX3(d,i,j,k) (3*((k)*(d)[0]*(d)[1] +(j)*(d)[0] +(i)))


/* constructors / destructor */

vsnMPP_Sv_orthoProbe::vsnMPP_Sv_orthoProbe(wxPanel* parent, vsnMethodObj* pm)
  : vsnMethodPP(parent, pm),
    m_pAxisRadio(NULL), m_pRepTxt(NULL),
    m_pITxt(NULL), m_pJTxt(NULL), m_pKTxt(NULL),
    m_pPointSizeTxt(NULL), m_pGlyphBiasTxt(NULL),
    m_pLeftBtn(NULL), m_pRightBtn(NULL), m_pDownBtn(NULL), m_pUpBtn(NULL),
    m_pMinusBtn(NULL), m_pPlusBtn(NULL)
{
  assert(parent);
  vsnMethod_Sv_orthoProbe* posm = dynamic_cast<vsnMethod_Sv_orthoProbe*>(pm);
  assert(posm);

  // setup gfxAct
  gfxAct_KI.setOrthoProbeMethod(posm);
  gfxAct_CLK.setOrthoProbeMethod(posm);
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
          gfxAct_CLK.setGfxView(pgv);
          pgv->pushActions(this);
          pgv->setStandardActions();
          vfrEvKeyIn::instance(dispatcher).regist(&gfxAct_KI);
          vfrEvClick::instance(dispatcher).regist(&gfxAct_CLK);
        } // end of if(pda)
      } // end of if(pgv)
    } // end of if(pvf)
  } // end of if(puvi)


  // widgets layout
  wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer* sizerH;

  // report
  m_pRepTxt = new wxTextCtrl(this, MPP_Sv_orthoProbe_RepTxt,
			     wxT("out of data\n"),
			     wxDefaultPosition, wxSize(-1, 200),
			     wxTE_MULTILINE|wxTE_READONLY);
  topsizer->Add(m_pRepTxt, 0, wxEXPAND|wxALL, 3);

  // probe index
  topsizer->Add(new wxStaticText(this, -1, wxT("probe index")),
                0, wxALIGN_LEFT|wxALL, 3);
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND);
  sizerH->Add(new wxStaticText(this, -1, wxT("I")), 0, wxALIGN_LEFT|wxALL, 3);
  m_pITxt = new wxTextCtrl(this, MPP_Sv_orthoProbe_ITxt, wxT(""),
			   wxDefaultPosition, wxDefaultSize,
			   wxTE_PROCESS_ENTER);
  sizerH->Add(m_pITxt, 0, wxALL, 3);

  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND);
  sizerH->Add(new wxStaticText(this, -1, wxT("J")), 0, wxALIGN_LEFT|wxALL, 3);
  m_pJTxt = new wxTextCtrl(this, MPP_Sv_orthoProbe_JTxt, wxT(""),
			   wxDefaultPosition, wxDefaultSize,
			   wxTE_PROCESS_ENTER);
  sizerH->Add(m_pJTxt, 0, wxALL, 3);

  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND);
  sizerH->Add(new wxStaticText(this, -1, wxT("K")), 0, wxALIGN_LEFT|wxALL, 3);
  m_pKTxt = new wxTextCtrl(this, MPP_Sv_orthoProbe_KTxt, wxT(""),
			   wxDefaultPosition, wxDefaultSize,
			   wxTE_PROCESS_ENTER);
  sizerH->Add(m_pKTxt, 0, wxALL, 3);

  // buttons
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxALL, 0);
  m_pLeftBtn = new wxButton(this, MPP_Sv_orthoProbe_LeftBtn, wxT("<"),
                            wxDefaultPosition, wxSize(30,-1), wxBU_EXACTFIT);
  sizerH->Add(m_pLeftBtn, 1, wxEXPAND|wxALL, 3);
  m_pDownBtn = new wxButton(this, MPP_Sv_orthoProbe_DownBtn, wxT("v"),
                            wxDefaultPosition, wxSize(30,-1), wxBU_EXACTFIT);
  sizerH->Add(m_pDownBtn, 1, wxEXPAND|wxALL, 3);
  m_pUpBtn = new wxButton(this, MPP_Sv_orthoProbe_UpBtn, wxT("^"),
			  wxDefaultPosition, wxSize(30,-1), wxBU_EXACTFIT);
  sizerH->Add(m_pUpBtn, 1, wxEXPAND|wxALL, 3);
  m_pRightBtn = new wxButton(this, MPP_Sv_orthoProbe_RightBtn, wxT(">"),
                             wxDefaultPosition, wxSize(30,-1), wxBU_EXACTFIT);
  sizerH->Add(m_pRightBtn, 1, wxEXPAND|wxALL, 3);
  sizerH->Add(10, 10);
  m_pMinusBtn = new wxButton(this, MPP_Sv_orthoProbe_MinusBtn, wxT("-"),
			     wxDefaultPosition, wxSize(30,-1), wxBU_EXACTFIT);
  sizerH->Add(m_pMinusBtn, 1, wxEXPAND|wxALL, 3);
  m_pPlusBtn = new wxButton(this, MPP_Sv_orthoProbe_PlusBtn, wxT("+"),
			    wxDefaultPosition, wxSize(30,-1), wxBU_EXACTFIT);
  sizerH->Add(m_pPlusBtn, 1, wxEXPAND|wxALL, 3);

  // axis
  wxString ritems1[] = {wxString(wxT("I ")),
			wxString(wxT("J ")),
			wxString(wxT("K "))};
  m_pAxisRadio = new wxRadioBox(this, MPP_Sv_orthoProbe_AxisRadio,
                                wxT("slice axis"),
                                wxDefaultPosition, wxDefaultSize,
                                3, ritems1, 1, wxRA_SPECIFY_ROWS);
  topsizer->Add(m_pAxisRadio, 0, wxEXPAND|wxALL, 3);

  // glyph bias
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxALL, 0);
  sizerH->Add(new wxStaticText(this, -1, wxT("glyph size bias")),
              0, wxALIGN_LEFT|wxALL, 3);
  m_pGlyphBiasTxt = new wxTextCtrl(this, MPP_Sv_orthoProbe_GlyphBiasTxt,
                                   wxT(""), wxDefaultPosition, wxDefaultSize,
                                   wxTE_PROCESS_ENTER);
  sizerH->Add(m_pGlyphBiasTxt, 0, wxALIGN_RIGHT|wxALL, 3);

  // point size
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxALL, 0);
  sizerH->Add(new wxStaticText(this, -1, wxT("point size")),
              0, wxALIGN_LEFT|wxALL, 3);
  m_pPointSizeTxt = new wxTextCtrl(this, MPP_Sv_orthoProbe_PointSizeTxt,
                                   wxT(""), wxDefaultPosition, wxDefaultSize,
                                   wxTE_PROCESS_ENTER);
  sizerH->Add(m_pPointSizeTxt, 0, wxALIGN_RIGHT|wxALL, 3);

  // post process
  SetSizer(topsizer);
  addTo(parent);
  topsizer->Layout();
  (void)update();
}

vsnMPP_Sv_orthoProbe::~vsnMPP_Sv_orthoProbe() {
  settlement();
}


/* interface */

bool vsnMPP_Sv_orthoProbe::update() {
  if ( ! m_pAxisRadio || ! m_pRepTxt || ! m_pITxt || ! m_pJTxt || ! m_pKTxt ||
       ! m_pPointSizeTxt || ! m_pGlyphBiasTxt )
    return false;

  vsnMethod_Sv_orthoProbe* pm
    = dynamic_cast<vsnMethod_Sv_orthoProbe*>(p_method);
  if ( ! pm ) return false;

  int val; float fval; char txt[64];

  val = pm->getSliceAxis();
  m_pAxisRadio->SetSelection(val);

  Vec3<size_t> p_idx = pm->getProbeIdx();
  sprintf(txt, "%lu", p_idx[0]); m_pITxt->SetValue(vsnApp::ConvSysToWx(txt));
  sprintf(txt, "%lu", p_idx[1]); m_pJTxt->SetValue(vsnApp::ConvSysToWx(txt));
  sprintf(txt, "%lu", p_idx[2]); m_pKTxt->SetValue(vsnApp::ConvSysToWx(txt));

  fval = pm->getGlyphBias();
  sprintf(txt, "%g", fval); m_pGlyphBiasTxt->SetValue(vsnApp::ConvSysToWx(txt));
  fval = pm->getPointSize();
  sprintf(txt, "%g", fval); m_pPointSizeTxt->SetValue(vsnApp::ConvSysToWx(txt));

  string repStr = pm->getRepStr();
  if ( repStr.empty() ) {
    m_pRepTxt->SetValue(wxT("out of data"));
    return false;
  }
  m_pRepTxt->SetValue(vsnApp::ConvSysToWx(repStr));

  return true;
}

void vsnMPP_Sv_orthoProbe::settlement() {
  vsnGfxView* pgv = gfxAct_KI.getGfxView();
  if ( pgv ) {
    pgv->popActions(this);
    gfxAct_KI.setGfxView(NULL);
    gfxAct_CLK.setGfxView(NULL);
  }
}


/* event handler */

void vsnMPP_Sv_orthoProbe::OnAxisRadio(wxCommandEvent& event) {
  if ( ! m_pAxisRadio ) return;
  vsnMethod_Sv_orthoProbe* pm
    = dynamic_cast<vsnMethod_Sv_orthoProbe*>(p_method);
  if ( ! pm ) return;

  int val = m_pAxisRadio->GetSelection();
  int oval = pm->getSliceAxis();
  if ( val == oval ) return;

  if ( pm->setSliceAxis(val) )
    pm->chkNotice();
}

void vsnMPP_Sv_orthoProbe::OnIdxTxt(wxCommandEvent& event) {
  if ( ! m_pITxt || ! m_pJTxt || ! m_pKTxt ) return;
  vsnMethod_Sv_orthoProbe* pm
    = dynamic_cast<vsnMethod_Sv_orthoProbe*>(p_method);
  if ( ! pm ) return;

  Vec3<size_t> valIdx, ovalIdx = pm->getProbeIdx();
  valIdx = ovalIdx;

  wxString valStr = m_pITxt->GetValue();
  if ( ! valStr.IsEmpty() )
    valIdx[0] = (size_t)abs(atoi(vsnApp::ConvWxToSys(valStr).c_str()));
  valStr = m_pJTxt->GetValue();
  if ( ! valStr.IsEmpty() )
    valIdx[1] = (size_t)abs(atoi(vsnApp::ConvWxToSys(valStr).c_str()));
  valStr = m_pKTxt->GetValue();
  if ( ! valStr.IsEmpty() )
    valIdx[2] = (size_t)abs(atoi(vsnApp::ConvWxToSys(valStr).c_str()));
  if ( valIdx[0] == ovalIdx[0] &&
       valIdx[1] == ovalIdx[1] && valIdx[2] == ovalIdx[2] ) return;

  if ( pm->setProbeIdx(valIdx) )
    pm->chkNotice();
}

void vsnMPP_Sv_orthoProbe::OnPointSizeTxt(wxCommandEvent& event) {
  if ( ! m_pPointSizeTxt ) return;
  vsnMethod_Sv_orthoProbe* pm
    = dynamic_cast<vsnMethod_Sv_orthoProbe*>(p_method);
  if ( ! pm ) return;

  wxString valStr = m_pPointSizeTxt->GetValue();
  if ( valStr.IsEmpty() ) return;
  float fval = (float)atof(vsnApp::ConvWxToSys(valStr).c_str());

  if ( pm->setPointSize(fval) )
    pm->chkNotice();
}

void vsnMPP_Sv_orthoProbe::OnGlyphBiasTxt(wxCommandEvent& event) {
  if ( ! m_pGlyphBiasTxt ) return;
  vsnMethod_Sv_orthoProbe* pm
    = dynamic_cast<vsnMethod_Sv_orthoProbe*>(p_method);
  if ( ! pm ) return;

  wxString valStr = m_pGlyphBiasTxt->GetValue();
  if ( valStr.IsEmpty() ) return;
  float fval = (float)atof(vsnApp::ConvWxToSys(valStr).c_str());

  if ( pm->setGlyphBias(fval) )
    pm->chkNotice();
}

void vsnMPP_Sv_orthoProbe::OnLeftBtn(wxCommandEvent& event) {
  vsnMethod_Sv_orthoProbe* pm
    = dynamic_cast<vsnMethod_Sv_orthoProbe*>(p_method);
  if ( ! pm ) return;

  Vec3<size_t> pidx = pm->getProbeIdx();
  int saxis = m_pAxisRadio->GetSelection();
  switch ( saxis ) {
  case 0: // YZ
    if ( pidx[1] > 0 ) pidx[1] = pidx[1] - 1; break;
  case 1: // ZX
    if ( pidx[2] > 0 ) pidx[2] = pidx[2] - 1; break;
  case 2: // XY
    if ( pidx[0] > 0 ) pidx[0] = pidx[0] - 1; break;
  default: return;
  } // end of switch(saxis)

  if ( pm->setProbeIdx(pidx) )
    pm->chkNotice();
}

void vsnMPP_Sv_orthoProbe::OnRightBtn(wxCommandEvent& event) {
  vsnMethod_Sv_orthoProbe* pm
    = dynamic_cast<vsnMethod_Sv_orthoProbe*>(p_method);
  if ( ! pm ) return;

  Vec3<size_t> pidx = pm->getProbeIdx();
  int saxis = m_pAxisRadio->GetSelection();
  switch ( saxis ) {
  case 0: // YZ
    pidx[1] = pidx[1] + 1; break;
  case 1: // ZX
    pidx[2] = pidx[2] + 1; break;
  case 2: // XY
    pidx[0] = pidx[0] + 1; break;
  default: return;
  } // end of switch(saxis)

  if ( pm->setProbeIdx(pidx) )
    pm->chkNotice();
}

void vsnMPP_Sv_orthoProbe::OnDownBtn(wxCommandEvent& event) {
  vsnMethod_Sv_orthoProbe* pm
    = dynamic_cast<vsnMethod_Sv_orthoProbe*>(p_method);
  if ( ! pm ) return;

  Vec3<size_t> pidx = pm->getProbeIdx();
  int saxis = m_pAxisRadio->GetSelection();
  switch ( saxis ) {
  case 0: // YZ
    if ( pidx[2] > 0 ) pidx[2] = pidx[2] - 1; break;
  case 1: // ZX
    if ( pidx[0] > 0 ) pidx[0] = pidx[0] - 1; break;
  case 2: // XY
    if ( pidx[1] > 0 ) pidx[1] = pidx[1] - 1; break;
  default: return;
  } // end of switch(saxis)

  if ( pm->setProbeIdx(pidx) )
    pm->chkNotice();
}

void vsnMPP_Sv_orthoProbe::OnUpBtn(wxCommandEvent& event) {
  vsnMethod_Sv_orthoProbe* pm
    = dynamic_cast<vsnMethod_Sv_orthoProbe*>(p_method);
  if ( ! pm ) return;

  Vec3<size_t> pidx = pm->getProbeIdx();
  int saxis = m_pAxisRadio->GetSelection();
  switch ( saxis ) {
  case 0: // YZ
    pidx[2] = pidx[2] + 1; break;
  case 1: // ZX
    pidx[0] = pidx[0] + 1; break;
  case 2: // XY
    pidx[1] = pidx[1] + 1; break;
  default: return;
  } // end of switch(saxis)

  if ( pm->setProbeIdx(pidx) )
    pm->chkNotice();
}

void vsnMPP_Sv_orthoProbe::OnMinusBtn(wxCommandEvent& event) {
  vsnMethod_Sv_orthoProbe* pm
    = dynamic_cast<vsnMethod_Sv_orthoProbe*>(p_method);
  if ( ! pm ) return;

  Vec3<size_t> pidx = pm->getProbeIdx();
  int saxis = m_pAxisRadio->GetSelection();
  switch ( saxis ) {
  case 0: // YZ
    if ( pidx[0] > 0 ) pidx[0] = pidx[0] - 1; break;
  case 1: // ZX
    if ( pidx[1] > 0 ) pidx[1] = pidx[1] - 1; break;
  case 2: // XY
    if ( pidx[2] > 0 ) pidx[2] = pidx[2] - 1; break;
  default: return;
  } // end of switch(saxis)

  if ( pm->setProbeIdx(pidx) )
    pm->chkNotice();
}

void vsnMPP_Sv_orthoProbe::OnPlusBtn(wxCommandEvent& event) {
  vsnMethod_Sv_orthoProbe* pm
    = dynamic_cast<vsnMethod_Sv_orthoProbe*>(p_method);
  if ( ! pm ) return;

  Vec3<size_t> pidx = pm->getProbeIdx();
  int saxis = m_pAxisRadio->GetSelection();
  switch ( saxis ) {
  case 0: // YZ
    pidx[0] = pidx[0] + 1; break;
  case 1: // ZX
    pidx[1] = pidx[1] + 1; break;
  case 2: // XY
    pidx[2] = pidx[2] + 1; break;
  default: return;
  } // end of switch(saxis)

  if ( pm->setProbeIdx(pidx) )
    pm->chkNotice();
}

void vsnMPP_Sv_orthoProbe::OnChar(wxKeyEvent& event) {
  // the codes below does not works good on Windows|MacOSX platform
  // (because of Windows' key-acceleraton and Cocoa's TextCtrl focus)
  wxCommandEvent ce;
  switch ( event.GetKeyCode() ) {
  case WXK_LEFT:
    OnLeftBtn(ce); break;
  case WXK_RIGHT:
    OnRightBtn(ce); break;
  case WXK_DOWN:
    OnDownBtn(ce); break;
  case WXK_UP:
    OnUpBtn(ce); break;
  case WXK_PAGEDOWN:
    OnMinusBtn(ce); break;
  case WXK_PAGEUP:
    OnPlusBtn(ce); break;
  default:
    ;
  }
}


//----------------------------------------------------------------
// class vsnMethod_Sv_orthoProbe
//----------------------------------------------------------------

/* constructors / destructor */

vsnMethod_Sv_orthoProbe::vsnMethod_Sv_orthoProbe(const string& name)
  : vsnMethodObj(name), m_root(NULL), m_slice(NULL), m_glyph(NULL),
    m_sliceAxis(2), m_glyphBias(1.f), m_pointSize(2.f), m_glyphRad(1.f),
    m_idxInited(false)
{
}

vsnMethod_Sv_orthoProbe::~vsnMethod_Sv_orthoProbe() {
  if ( m_slice )
    delete m_slice;
  if ( m_glyph )
    delete m_glyph;
  if ( m_root )
    delete m_root;
}


/* methods */

bool vsnMethod_Sv_orthoProbe::setSliceAxis(const int sa) {
  int xsa = sa % 3;
  if ( m_sliceAxis == xsa ) return true;
  m_sliceAxis = xsa;

  if ( ! update() ) return false;
  updateUI();
  return true;
}

bool vsnMethod_Sv_orthoProbe::setProbeIdx(const CES::Vec3<size_t>& pidx) {
  if ( m_probeIdx[0] == pidx[0] &&
       m_probeIdx[1] == pidx[1] && m_probeIdx[2] == pidx[2] )
    return true;

  vsnData_Sv* pData = dynamic_cast<vsnData_Sv*>(p_refData);
  if ( ! pData ) return false;
  Vec3<size_t> dims = pData->getDims();
  Vec3<size_t> xidx = pidx;

  if ( xidx[0] >= dims[0] ) xidx[0] = dims[0] - 1;
  if ( xidx[1] >= dims[1] ) xidx[1] = dims[1] - 1;
  if ( xidx[2] >= dims[2] ) xidx[2] = dims[2] - 1;
  m_probeIdx = xidx;

  if ( ! update() ) return false;
  updateUI();
  return true;
}

bool vsnMethod_Sv_orthoProbe::setGlyphBias(const float gb) {
  if ( m_glyphBias == gb ) return true;
  if ( gb <= 0.f ) return false;
  m_glyphBias = gb;

  if ( m_glyph )
    m_glyph->setRadius(m_glyphRad * m_glyphBias);
  updateUI();
  return true;
}

bool vsnMethod_Sv_orthoProbe::setPointSize(const float ps) {
  if ( m_pointSize == ps ) return true;
  if ( ps < 0.f ) return false;
  m_pointSize = ps;

  if ( m_slice ) {
    if ( m_pointSize <= 0.f )
      m_slice->getPrivateMaterial()->setRenderMode(RT_NONE);
    else {
      m_slice->getPrivateMaterial()->setRenderMode(RT_POINT);
      m_slice->getPrivateMaterial()->setPointSize(m_pointSize);
    }
  }
  updateUI();
  return true;
}

bool vsnMethod_Sv_orthoProbe::clickSelect(const Point2& rpos, vfrScreen* pscr)
{
  if ( ! pscr ) return false;
  if ( ! m_slice ) return false;
  vsnData_Sv* pData = dynamic_cast<vsnData_Sv*>(p_refData);
  if ( ! pData ) return false;

  int* r = pscr->clickFeedback(rpos.x, rpos.y, m_slice->getID());
  if ( r[0] < 1 ) return false;
  Vec3<size_t> dims = pData->getDims();
  Vec3<size_t> pidx = m_probeIdx;
  switch ( m_sliceAxis % 3 ) {
  case 0:
    pidx[1] = r[1] % dims[1];
    pidx[2] = r[1] / dims[1];
    break;
  case 1:
    pidx[2] = r[1] % dims[2];
    pidx[0] = r[1] / dims[2];
    break;
  case 2:
    pidx[0] = r[1] % dims[0];
    pidx[1] = r[1] / dims[0];
    break;
  } // end of switch(m_sliceAxis)

  if ( ! setProbeIdx(pidx) ) return false;
  return true;
}

bool vsnMethod_Sv_orthoProbe::updateRepStr(const int stp) {
  m_repStr = "out of data\n";

  vsnData_Sv* pData = dynamic_cast<vsnData_Sv*>(p_refData);
  if ( ! pData ) return false;

  Vec3<size_t> dims = pData->getDims();
  size_t dlen = pData->getDataLen();
  const float* gptr = pData->getCoord(stp);
  const float* dptr = pData->getData(stp);
  if ( ! gptr || ! dptr ) return false;
  const unsigned char* maskptr = pData->getMask();

  register size_t i, idx;
  register float datVal;
  char txt[64];

  m_repStr = "Grid index:\n";
  m_repStr += "  IJK = ";
  sprintf(txt, "[%lu, %lu, %lu]\n",
	  m_probeIdx[0], m_probeIdx[1], m_probeIdx[2]);
  m_repStr += txt;
  if ( m_probeIdx[0] >= dims[0] ||
       m_probeIdx[1] >= dims[1] || m_probeIdx[2] >= dims[2] ) {
    m_repStr += "  out of range\n";
    return false;
  }

  vector3 xp;
  idx = IDX3(dims, m_probeIdx[0], m_probeIdx[1], m_probeIdx[2]);
  xp[0] = gptr[idx]; xp[1] = gptr[idx +1]; xp[2] = gptr[idx +2];
  m_repStr += "Coord:\n";
  m_repStr += "  x = "; sprintf(txt, "%g\n", xp[0]); m_repStr += txt;
  m_repStr += "  y = "; sprintf(txt, "%g\n", xp[1]); m_repStr += txt;
  m_repStr += "  z = "; sprintf(txt, "%g\n", xp[2]); m_repStr += txt;

  m_repStr += "Datas:\n";
  idx = IDX1(dims, m_probeIdx[0], m_probeIdx[1], m_probeIdx[2]) * dlen;
  for ( i = 0; i < dlen; i++ ) {
    datVal = dptr[idx + i];
    sprintf(txt, "  data#%lu = %g\n", i, datVal);
    m_repStr += txt;
  } // end of for(i)

  m_repStr += "Mask = ";
  if ( maskptr ) {
    idx = IDX1(dims, m_probeIdx[0], m_probeIdx[1], m_probeIdx[2]);
    if ( maskptr[idx] )
      m_repStr += "on\n";
    else
      m_repStr += "off\n";
  } else {
    m_repStr += "none\n";
  }

  return true;
}


/* vsnTimeSeriesMethodIF method */

bool vsnMethod_Sv_orthoProbe::updateStep(const int stp,
					 const bool force, const bool cascade)
{
  vsnData_Sv* pData = dynamic_cast<vsnData_Sv*>(p_refData);
  if ( ! pData ) return false;
  Vec3<size_t> dims = pData->getDims();
  size_t dimSz = dims.m_v[0] * dims.m_v[1] * dims.m_v[2];
  if ( dimSz < 1 ) return true;

  m_requestedStp = stp;
  if ( m_requestedStp < 0 )
    m_requestedStp = pData->getCurrentStepIdx();

  if ( force ) m_updatedStp = -1;
  if ( m_updatedStp == m_requestedStp ) return true;
  m_updatedStp = -1;

  // root
  if ( ! m_root ) {
    m_root = new vfrGroup();
    if ( ! m_root ) {
      ErrMsg(MsgERR, getMethodType() + string("[") + getName()
             + string("]: memory allocation failed"));
      return false;
    }
    m_root->alcMaterial();
    addChild(m_root);
  }
  m_root->getPrivateMaterial()->setRenderMode(RT_NONE);

  // slice
  if ( ! m_slice ) {
    m_slice = new vfrMesh2D();
    if ( ! m_slice ) {
      ErrMsg(MsgERR, getMethodType() + string("[") + getName()
             + string("]: memory allocation failed"));
      return false;
    }
    m_slice->alcMaterial();
    m_slice->setColor3(0, m_colour);
    m_slice->setAlpha(TRUE, 0.9f);
    m_slice->getPrivateMaterial()->setPointSize(m_pointSize);
    if ( m_pointSize > 0.f )
      m_slice->getPrivateMaterial()->setRenderMode(RT_POINT);
    m_root->addChild(m_slice);
  }

  // glyph
  if ( ! m_glyph ) {
    m_glyph = new vfrBall();
    if ( ! m_glyph ) {
      ErrMsg(MsgERR, getMethodType() + string("[") + getName()
             + string("]: memory allocation failed"));
      return false;
    }
    m_glyph->alcMaterial();
    m_glyph->setSubdiv(3);
    m_glyph->setColor4(0, m_colour);
    m_glyph->getPrivateMaterial()->setRenderMode(RT_SMOOTH);
    m_root->addChild(m_glyph);
  }

  // reports
  if ( ! updateRepStr(m_requestedStp) ) {
    updateUI();
    return false;
  }
  
  //------------------------------------------------------------
  updateUI();
  if ( ! m_show ) return true;
  //------------------------------------------------------------

  // get grid data
  const float* gptr = pData->getCoord(m_requestedStp);
  const Vec3<float>* pbb = pData->getBbox();
  if ( ! gptr || ! pbb ) {
    return false;
  }

  // slice vertices
  Point2 sliceSz;
  switch ( m_sliceAxis % 3 ) {
  case 0: sliceSz.x = dims[1]; sliceSz.y = dims[2]; break;
  case 1: sliceSz.x = dims[2]; sliceSz.y = dims[0]; break;
  case 2: sliceSz.x = dims[0]; sliceSz.y = dims[1]; break;
  } // end of switch(m_sliceAxis)
  if ( ! m_slice->setMeshSize(sliceSz) ) {
    ErrMsg(MsgERR, getMethodType() + string("[") + getName()
             + string("]: memory allocation failed"));
      return false;
  }
  vector3* vl = m_slice->getVerts();

  register int ii, i, j, k, idx;
  ii = 0;
  switch ( m_sliceAxis % 3 ) {
  case 0: // YZ
    i = m_probeIdx[0];
    for ( k = 0; k < sliceSz.y; k++ )
      for ( j = 0; j < sliceSz.x; j++ ) {
        idx = IDX3(dims, i, j, k);
	vl[ii][0] = gptr[idx   ];
	vl[ii][1] = gptr[idx +1];
	vl[ii][2] = gptr[idx +2];
	ii ++;
      } // end of for(j)
    break;
  case 1: // ZX
    j = m_probeIdx[1];
    for ( i = 0; i < sliceSz.y; i++ )
      for ( k = 0; k < sliceSz.x; k++ ) {
	idx = IDX3(dims, i, j, k);
	vl[ii][0] = gptr[idx   ];
	vl[ii][1] = gptr[idx +1];
	vl[ii][2] = gptr[idx +2];
	ii ++;
      } // end of for(k)
    break;
  case 2: // XY
    k = m_probeIdx[2];
    for ( j = 0; j < sliceSz.y; j++ )
      for ( i = 0; i < sliceSz.x; i++ ) {
	idx = IDX3(dims, i, j, k);
	vl[ii][0] = gptr[idx   ];
	vl[ii][1] = gptr[idx +1];
	vl[ii][2] = gptr[idx +2];
	ii ++;
      } // end of for(i)
    break;
  } // end of switch(m_sliceAxis)
  m_slice->generateBbox();

  // glyph size
  Vec3<float> bSz = pbb[1] - pbb[0];
  bSz[0] = fabs(bSz[0] / (float)dims[0]);
  bSz[1] = fabs(bSz[1] / (float)dims[1]);
  bSz[2] = fabs(bSz[2] / (float)dims[2]);
  float minSz = (bSz[0] < bSz[1]) ? bSz[0] : bSz[1];
  minSz = (minSz < bSz[2]) ? minSz : bSz[2];
  m_glyphRad = minSz * 0.5f;
  if ( m_glyphRad < 1e-3f ) m_glyphRad = 1e-3f;
  m_glyph->setRadius(m_glyphRad * m_glyphBias);

  // glyph position
  vector3 xp;
  idx = IDX3(dims, m_probeIdx[0], m_probeIdx[1], m_probeIdx[2]);
  xp[0] = gptr[idx]; xp[1] = gptr[idx +1]; xp[2] = gptr[idx +2];
  m_glyph->identity();
  m_glyph->trans(xp);

  // ok
  m_root->getPrivateMaterial()->setRenderMode(m_showType);
  m_slice->notice();
  m_glyph->notice();
  m_updatedStp = m_requestedStp;
  return true;
}


/* vsnMethodObj method */

void vsnMethod_Sv_orthoProbe::setLighting(const bool mode) {
  if ( m_glyph ) {
    m_glyph->getPrivateMaterial()->
      setRenderMode(mode ? RT_SMOOTH : RT_NOLIGHT);
  }

  vsnMethodObj::setLighting(mode);
}

bool vsnMethod_Sv_orthoProbe::update(const bool force) {
  if ( ! m_idxInited ) {
    vsnData_Sv* pData = dynamic_cast<vsnData_Sv*>(p_refData);
    if ( pData ) {
      Vec3<size_t> dims = pData->getDims();
      m_probeIdx[0] = dims[0] / 2;
      m_probeIdx[1] = dims[1] / 2;
      m_probeIdx[2] = dims[2] / 2;
      updateUI();
      m_idxInited = true;
    }
  }

  return updateStep(m_requestedStp, force);
}

vsnMethodPP* vsnMethod_Sv_orthoProbe::getParamPanel(wxPanel* pp) {
  vsnMPP_Sv_orthoProbe* pp_orthoProbe
    = new vsnMPP_Sv_orthoProbe(pp, this);
  if ( ! pp_orthoProbe ) return NULL;
  return pp_orthoProbe;
}


/* serialize : from vsnIoObject */

bool vsnMethod_Sv_orthoProbe::parseXML(xmlNodePtr xnp) {
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

      if ( xsV.empty() ) {
        ErrMsg(MsgERR, msgHdr +string("no value in param ") +xsN);
        goto _NEXT_XML_NODE;
      }

      if ( xsN == string("axis") ) {
	int axis = -1;
        if ( xsV == "0" || xsV == "i" || xsV == "I" ||
	     xsV == "x" || xsV == "X" ) axis = 0;
        else if ( xsV == "1" || xsV == "j" || xsV == "J" ||
		  xsV == "y" || xsV == "Y" ) axis = 1;
        else if ( xsV == "2" || xsV == "k" || xsV == "K" ||
		  xsV == "z" || xsV == "Z" ) axis = 2;
        else {
          ErrMsg(MsgERR, msgHdr +string("invalid value in param axis: ") +xsV);
          goto _NEXT_XML_NODE;
        }
	if ( ! setSliceAxis(axis) ) {
	  ErrMsg(MsgERR, msgHdr + string("can't set param axis: ") +xsV);
          goto _NEXT_XML_NODE;
	}
      } // end of "axis"
      else if ( xsN == string("probe_idx") ) {
        Vec3<int> idcs(-1, -1, -1);
        istringstream iss(xsV);
        iss >> idcs;
        if ( idcs[0] < 0 || idcs[1] < 0 ||  idcs[2] < 0 ) {
          ErrMsg(MsgERR, msgHdr + string("bad value format: ") +xsV);
          goto _NEXT_XML_NODE;
        }
	Vec3<size_t> xidx(idcs[0], idcs[1], idcs[2]);
        if ( ! setProbeIdx(xidx) ) {
          ErrMsg(MsgERR, msgHdr + string("failed to set probe_idx: ") +xsV);
          goto _NEXT_XML_NODE;
        }
      } // end of "probe_idx"
      else if ( xsN == string("glyph_bias") ) {
        float gb = (float)atof(xsV.c_str());
        if ( ! setGlyphBias(gb) ) {
          ErrMsg(MsgERR, msgHdr + string("invalid value in param glyph_bias")
		 + xsV);
          goto _NEXT_XML_NODE;
        }
      } // end of "glyph_bias"
      else if ( xsN == string("point_size") ) {
        float psz = (float)atof(xsV.c_str());
        if ( ! setPointSize(psz) ) {
          ErrMsg(MsgERR, msgHdr + string("invalid value in param point_size")
		 + xsV);
          goto _NEXT_XML_NODE;
        }
      } // end of "point_size"
    } // end of param

  _NEXT_XML_NODE:
    cur = cur->next;
  } // end of while(cur)
 
  return true;
}

bool vsnMethod_Sv_orthoProbe::outputXML(std::ostream& os, const size_t ts) {
  vsnData_Sv* pData = dynamic_cast<vsnData_Sv*>(p_refData);
  if ( ! pData ) return false;

  std::string idts;
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
  // axis
  if ( m_sliceAxis != 2 ) {
    os << idts_2 << "<param name=\"axis\" value=\"";
    switch ( m_sliceAxis ) {
    case 0: os << "I"; break;
    case 1: os << "J"; break;
    case 2: os << "K"; break;
    default: ret = false;
    } // end of switch(m_sliceAxis)
    os << "\" />" << endl;
  }

  // probe_idx
  Vec3<size_t> dims = pData->getDims();
  if ( m_probeIdx[0] != dims[0] / 2 ||
       m_probeIdx[1] != dims[1] / 2 ||
       m_probeIdx[2] != dims[2] / 2 ) {
    os << idts_2 << "<param name=\"probe_idx\" value=\""
       << m_probeIdx[0] << "/" << m_probeIdx[1] << "/" << m_probeIdx[2]
       << "\" />" << endl;
  }

  // glyph_bias
  if ( m_glyphBias != 1.f ) {
    os << idts_2 << "<param name=\"glyph_bias\" value=\""
       << m_glyphBias << "\" />" << endl;
  }

  // point_size
  if ( m_pointSize != 2.f ) {
    os << idts_2 << "<param name=\"point_size\" value=\""
       << m_pointSize << "\" />" << endl;
  }

  os << idts << "</method>" << endl;

  return ret;
}

bool vsnMethod_Sv_orthoProbe::commandXML(xmlNodePtr xnp) {
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

  // do the command
  if ( valueStr.empty() ) {
    ErrMsg(MsgERR, msgHdr + "invalid command: " + nameStr + ": no value");
    return false;
  }

  if ( nameStr == "set_axis" ) {
    int axis = -1;
    if ( valueStr == "0" || valueStr == "i" || valueStr == "I" ||
	 valueStr == "x" || valueStr == "X" ) axis = 0;
    else if ( valueStr == "1" || valueStr == "j" || valueStr == "J" ||
	      valueStr == "y" || valueStr == "Y" ) axis = 1;
    else if ( valueStr == "2" || valueStr == "k" || valueStr == "K" ||
	      valueStr == "z" || valueStr == "Z" ) axis = 2;
    else {
      ErrMsg(MsgERR, msgHdr +
             string("invalid command: set_axis: invalid value"));
      return false;
    }
    if ( ! setSliceAxis(axis) ) {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_axis: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_axis"
  else if ( nameStr == string("set_probe_idx") ) {
    Vec3<int> idcs(-1, -1, -1);
    istringstream iss(valueStr);
    iss >> idcs;
    if ( idcs[0] < 0 || idcs[1] < 0 ||  idcs[2] < 0 ) {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_probe_idx: bad value format: ") +valueStr);
      return false;
    }
    Vec3<size_t> xidx(idcs[0], idcs[1], idcs[2]);
    if ( ! setProbeIdx(xidx) ) {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_probe_idx: set failed: ") +valueStr);
      return false;
    }
  } // end of "set_probe_idx"
  else if ( nameStr == string("set_glyph_bias") ) {
    float gb = (float)atof(valueStr.c_str());
    if ( ! setGlyphBias(gb) ) {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_probe_idx: invalid value:") + valueStr);
      return false;
    }
  } // end of "set_glyph_bias"
  else if ( nameStr == string("set_point_size") ) {
    float psz = (float)atof(valueStr.c_str());
    if ( ! setPointSize(psz) ) {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_glyph_bias: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_point_size"
  else {
    // not my command
    ErrMsg(MsgERR, msgHdr + string("unknown command: ") + nameStr);
    return false;
  }

  return true;
}
