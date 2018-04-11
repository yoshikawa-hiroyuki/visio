//
// vsnViewFrameWinDlg
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
#include "wx/gdicmn.h"

#include "vsnViewFrameWinDlg.h"
#include "vsnViewFrame.h"
#include "vsnError.h"

#if defined(WINDOWS)
#define WL_H_MARGIN_TOP 0
#define WL_H_MARGIN_BOT 25
#elif defined(MacOSX)
#define WL_H_MARGIN_TOP 25
#define WL_H_MARGIN_BOT 0
#else
#define WL_H_MARGIN_TOP 25
#define WL_H_MARGIN_BOT 25
#endif
#define WL_WIN_MINSZ 10

using namespace VSN;


//----------------------------------------------------------------
// class vsnViewFrameWinGeomDlg
//----------------------------------------------------------------

BEGIN_EVENT_TABLE(vsnViewFrameWinGeomDlg, wxDialog)
  EVT_TEXT_ENTER(ViewFrameWinGeomDlg_XTxt,
		 vsnViewFrameWinGeomDlg::OnChangeValues)
  EVT_TEXT_ENTER(ViewFrameWinGeomDlg_YTxt,
		 vsnViewFrameWinGeomDlg::OnChangeValues)
  EVT_TEXT_ENTER(ViewFrameWinGeomDlg_WTxt,
		 vsnViewFrameWinGeomDlg::OnChangeValues)
  EVT_TEXT_ENTER(ViewFrameWinGeomDlg_HTxt,
		 vsnViewFrameWinGeomDlg::OnChangeValues)
  EVT_BUTTON(ViewFrameWinGeomDlg_CloseBtn,
	     vsnViewFrameWinGeomDlg::OnCloseBtn)
END_EVENT_TABLE()


/* constructors, destructor */

vsnViewFrameWinGeomDlg::vsnViewFrameWinGeomDlg(vsnViewFrame* parent)
  : wxDialog(parent, -1, wxString(wxT("window geometry"))),
    m_pXTxt(NULL), m_pYTxt(NULL), m_pWTxt(NULL), m_pHTxt(NULL),
    m_pCloseBtn(NULL)
{
  assert(parent);

  wxBoxSizer *sizerTop = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *sizerH;

  // position
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  sizerTop->Add(sizerH, 0, wxEXPAND|wxALL, 5);
  sizerH->Add(new wxStaticText(this, -1, wxT("position")), 0,
	      wxALIGN_LEFT|wxALL, 3);
  sizerH->Add(5, 5, 1, wxEXPAND);
  sizerH->Add(new wxStaticText(this, -1, wxT("X")), 0, wxALL, 3);
  m_pXTxt = new wxTextCtrl(this, ViewFrameWinGeomDlg_XTxt, wxT("0"),
			   wxDefaultPosition,wxDefaultSize,wxTE_PROCESS_ENTER);
  sizerH->Add(m_pXTxt, 0, wxALL, 3);
  sizerH->Add(new wxStaticText(this, -1, wxT("Y")), 0, wxALL, 3);
  m_pYTxt = new wxTextCtrl(this, ViewFrameWinGeomDlg_YTxt, wxT("0"),
			   wxDefaultPosition,wxDefaultSize,wxTE_PROCESS_ENTER);
  sizerH->Add(m_pYTxt, 0, wxALL, 3);

  // size
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  sizerTop->Add(sizerH, 0, wxEXPAND|wxALL, 5);
  sizerH->Add(new wxStaticText(this, -1, wxT("size")), 0,
	      wxALIGN_LEFT|wxALL, 3);
  sizerH->Add(5, 5, 1, wxEXPAND);
  sizerH->Add(new wxStaticText(this, -1, wxT("W")), 0, wxALL, 3);
  m_pWTxt = new wxTextCtrl(this, ViewFrameWinGeomDlg_WTxt, wxT("0"),
			   wxDefaultPosition,wxDefaultSize,wxTE_PROCESS_ENTER);
  sizerH->Add(m_pWTxt, 0, wxALL, 3);
  sizerH->Add(new wxStaticText(this, -1, wxT("H")), 0, wxALL, 3);
  m_pHTxt = new wxTextCtrl(this, ViewFrameWinGeomDlg_HTxt, wxT("0"),
			   wxDefaultPosition,wxDefaultSize,wxTE_PROCESS_ENTER);
  sizerH->Add(m_pHTxt, 0, wxALL, 3);

  // buttons
  sizerTop->Add(0, 5, 0, wxGROW);
  sizerTop->Add(
     new wxStaticLine(this, -1, wxDefaultPosition, wxSize(3,3), wxHORIZONTAL),
     0, wxEXPAND|wxALL, 0);
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  sizerTop->Add(sizerH, 0, wxALIGN_RIGHT|wxALL, 5);
  m_pCloseBtn = new wxButton(this, ViewFrameWinGeomDlg_CloseBtn, wxT("close"));
  sizerH->Add(m_pCloseBtn);

  // post process
  SetAutoLayout(TRUE);
  SetSizer(sizerTop);
  sizerTop->SetSizeHints(this);
  sizerTop->Fit(this);

  (void)update();
}

vsnViewFrameWinGeomDlg::~vsnViewFrameWinGeomDlg() {
}


/* interface */

bool vsnViewFrameWinGeomDlg::update() {
  if ( ! m_pXTxt || ! m_pYTxt || ! m_pWTxt || ! m_pHTxt ) return false;
  vsnViewFrame* parent = dynamic_cast<vsnViewFrame*>(GetParent());
  if ( ! parent ) return false;

  int winX, winY, winW, winH;
  parent->GetPosition(&winX, &winY);
  parent->GetSize(&winW, &winH);
  char txt[64];
  sprintf(txt, "%d", winX); m_pXTxt->SetValue(vsnApp::ConvSysToWx(txt));
  sprintf(txt, "%d", winY); m_pYTxt->SetValue(vsnApp::ConvSysToWx(txt));
  sprintf(txt, "%d", winW); m_pWTxt->SetValue(vsnApp::ConvSysToWx(txt));
  sprintf(txt, "%d", winH); m_pHTxt->SetValue(vsnApp::ConvSysToWx(txt));
  return true;
}


/* event handler */

void vsnViewFrameWinGeomDlg::OnChangeValues(wxCommandEvent& event) {
  if ( ! m_pXTxt || ! m_pYTxt || ! m_pWTxt || ! m_pHTxt ) return;
  vsnViewFrame* parent = dynamic_cast<vsnViewFrame*>(GetParent());
  if ( ! parent ) return;

  int winX, winY, winW, winH;
  wxString vstr;
  vstr = m_pXTxt->GetValue(); winX = atoi(vsnApp::ConvWxToSys(vstr).c_str());
  vstr = m_pYTxt->GetValue(); winY = atoi(vsnApp::ConvWxToSys(vstr).c_str());
  vstr = m_pWTxt->GetValue(); winW = atoi(vsnApp::ConvWxToSys(vstr).c_str());
  vstr = m_pHTxt->GetValue(); winH = atoi(vsnApp::ConvWxToSys(vstr).c_str());

  (void)parent->setGeometry(winX, winY, winW, winH);
}

void vsnViewFrameWinGeomDlg::OnCloseBtn(wxCommandEvent& event)
{
  if ( IsModal() )
    EndModal(vsn_wxIDOK);
  else
    this->Hide();
}


//----------------------------------------------------------------
// class vsnViewFrameWinLayoutDlg
//----------------------------------------------------------------

BEGIN_EVENT_TABLE(vsnViewFrameWinLayoutDlg, wxDialog)
  EVT_BUTTON(ViewFrameWinLayoutDlg_OkBtn,
	     vsnViewFrameWinLayoutDlg::OnOkBtn)
  EVT_BUTTON(ViewFrameWinLayoutDlg_CancelBtn,
	     vsnViewFrameWinLayoutDlg::OnCancelBtn)
  EVT_BUTTON(ViewFrameWinLayoutDlg_ResetBtn,
	     vsnViewFrameWinLayoutDlg::OnResetBtn)
END_EVENT_TABLE()


/* static members */

size_t vsnViewFrameWinLayoutDlg::s_M = 2;
size_t vsnViewFrameWinLayoutDlg::s_N = 2;
size_t vsnViewFrameWinLayoutDlg::s_X0 = 0;
size_t vsnViewFrameWinLayoutDlg::s_Y0 = 0;
size_t vsnViewFrameWinLayoutDlg::s_X1 = 0;
size_t vsnViewFrameWinLayoutDlg::s_Y1 = 0;
vsnViewFrameWinLayoutDlg::LDirType
vsnViewFrameWinLayoutDlg::s_LDir = vsnViewFrameWinLayoutDlg::LDIR_HORIZONTAL;


/* constructors, destructor */

vsnViewFrameWinLayoutDlg::vsnViewFrameWinLayoutDlg(vsnViewFrame* parent)
  : wxDialog(parent, -1, wxString(wxT("layout windows"))),
    m_pMTxt(NULL), m_pNTxt(NULL), m_pX0Txt(NULL), m_pY0Txt(NULL),
    m_pX1Txt(NULL), m_pY1Txt(NULL), m_pDirRadio(NULL),
    m_pOkBtn(NULL), m_pCancelBtn(NULL)
{
  assert(parent);

  wxBoxSizer *sizerTop = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *sizerH, *sizerV;

  // grid
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  sizerTop->Add(sizerH, 0, wxEXPAND|wxALL, 5);
  sizerH->Add(new wxStaticText(this, -1, wxT("grid  M")), 0,
	      wxALIGN_LEFT|wxALL, 3);
  m_pMTxt = new wxTextCtrl(this, ViewFrameWinLayoutDlg_MTxt, wxT("0"),
			   wxDefaultPosition, wxDefaultSize);
  sizerH->Add(m_pMTxt, 1, wxEXPAND|wxALL, 3);
  sizerH->Add(new wxStaticText(this, -1, wxT(" x N")), 0,
	      wxALIGN_LEFT|wxALL, 3);
  m_pNTxt = new wxTextCtrl(this, ViewFrameWinLayoutDlg_NTxt, wxT("0"),
			   wxDefaultPosition, wxDefaultSize);
  sizerH->Add(m_pNTxt, 1, wxEXPAND|wxALL, 3);

  // direction
  wxString ritems[] = {wxString(wxT("horizontal")), wxString(wxT("vertical"))};
  m_pDirRadio = new wxRadioBox(this, ViewFrameWinLayoutDlg_DirRadio,
			       wxT("direction"),
			       wxDefaultPosition, wxDefaultSize,
			       2, ritems, 1, wxRA_SPECIFY_ROWS);
  sizerTop->Add(m_pDirRadio, 0, wxEXPAND|wxALL, 5);

  // area
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  sizerTop->Add(sizerH, 0, wxEXPAND|wxALL, 5);
  sizerH->Add(new wxStaticText(this,-1,wxT("area")), 0, wxALIGN_LEFT|wxALL, 3);
  sizerV = new wxBoxSizer(wxVERTICAL);
  sizerH->Add(sizerV, 1, wxEXPAND|wxALL, 3);

  sizerH = new wxBoxSizer(wxHORIZONTAL);
  sizerV->Add(sizerH, 0, wxEXPAND|wxALL, 0);
  sizerH->Add(new wxStaticText(this, -1, wxT("X0")), 0, wxALIGN_LEFT|wxALL, 3);
  m_pX0Txt = new wxTextCtrl(this, ViewFrameWinLayoutDlg_X0Txt, wxT("0"),
			   wxDefaultPosition, wxDefaultSize);
  sizerH->Add(m_pX0Txt, 1, wxEXPAND|wxALL, 3);
  sizerH->Add(new wxStaticText(this, -1, wxT("Y0")), 0, wxALIGN_LEFT|wxALL, 3);
  m_pY0Txt = new wxTextCtrl(this, ViewFrameWinLayoutDlg_Y0Txt, wxT("0"),
			   wxDefaultPosition, wxDefaultSize);
  sizerH->Add(m_pY0Txt, 1, wxEXPAND|wxALL, 3);

  sizerH = new wxBoxSizer(wxHORIZONTAL);
  sizerV->Add(sizerH, 0, wxEXPAND|wxALL, 0);
  sizerH->Add(new wxStaticText(this, -1, wxT("X1")), 0, wxALIGN_LEFT|wxALL, 3);
  m_pX1Txt = new wxTextCtrl(this, ViewFrameWinLayoutDlg_X1Txt, wxT("0"),
			   wxDefaultPosition, wxDefaultSize);
  sizerH->Add(m_pX1Txt, 1, wxEXPAND|wxALL, 3);
  sizerH->Add(new wxStaticText(this, -1, wxT("Y1")), 0, wxALIGN_LEFT|wxALL, 3);
  m_pY1Txt = new wxTextCtrl(this, ViewFrameWinLayoutDlg_Y1Txt, wxT("0"),
			   wxDefaultPosition, wxDefaultSize);
  sizerH->Add(m_pY1Txt, 1, wxEXPAND|wxALL, 3);

  // buttons
  sizerTop->Add(0, 5, 0, wxGROW);
  sizerTop->Add(
     new wxStaticLine(this, -1, wxDefaultPosition, wxSize(3,3), wxHORIZONTAL),
     0, wxEXPAND|wxALL, 0);
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  sizerTop->Add(sizerH, 0, wxALIGN_RIGHT|wxALL, 5);
  m_pOkBtn = new wxButton(this, ViewFrameWinLayoutDlg_OkBtn, wxT("OK"));
  sizerH->Add(m_pOkBtn);
  m_pResetBtn = new wxButton(this,ViewFrameWinLayoutDlg_ResetBtn,wxT("Reset"));
  sizerH->Add(m_pResetBtn);
  m_pCancelBtn = new wxButton(this,
			      ViewFrameWinLayoutDlg_CancelBtn, wxT("Cancel"));
  sizerH->Add(m_pCancelBtn);

  // post process
  SetAutoLayout(TRUE);
  SetSizer(sizerTop);
  sizerTop->SetSizeHints(this);
  sizerTop->Fit(this);

  (void)update();
}

vsnViewFrameWinLayoutDlg::~vsnViewFrameWinLayoutDlg() {
}


/* interface */

bool vsnViewFrameWinLayoutDlg::update() {
  if ( ! m_pMTxt || ! m_pNTxt || ! m_pX0Txt || ! m_pY0Txt ||
       ! m_pX1Txt || ! m_pY1Txt || ! m_pDirRadio )
    return false;

  if ( (s_X1 - s_X0) == 0 && (s_Y1 - s_Y0) == 0 ) {
    int dw, dh; wxDisplaySize(&dw, &dh);
    s_X0 = 0; s_X1 = dw;
    s_Y0 = WL_H_MARGIN_TOP;
    s_Y1 = dh - WL_H_MARGIN_TOP - WL_H_MARGIN_BOT;
  }

  char txt[64];
  sprintf(txt, "%lu", s_M); m_pMTxt->SetValue(vsnApp::ConvSysToWx(txt));
  sprintf(txt, "%lu", s_N); m_pNTxt->SetValue(vsnApp::ConvSysToWx(txt));
  sprintf(txt, "%lu", s_X0); m_pX0Txt->SetValue(vsnApp::ConvSysToWx(txt));
  sprintf(txt, "%lu", s_Y0); m_pY0Txt->SetValue(vsnApp::ConvSysToWx(txt));
  sprintf(txt, "%lu", s_X1); m_pX1Txt->SetValue(vsnApp::ConvSysToWx(txt));
  sprintf(txt, "%lu", s_Y1); m_pY1Txt->SetValue(vsnApp::ConvSysToWx(txt));
  m_pDirRadio->SetSelection(s_LDir);
  return true;
}


/* event handler */

void vsnViewFrameWinLayoutDlg::OnOkBtn(wxCommandEvent& event) {
  if ( ! m_pMTxt || ! m_pNTxt || ! m_pX0Txt || ! m_pY0Txt ||
       ! m_pX1Txt || ! m_pY1Txt || ! m_pDirRadio ) {this->Hide(); return;}
  vsnApp* pApp = vsnApp::GetApp();
  if ( ! pApp ) {this->Hide(); return;}
  size_t nvf = pApp->getNumViewFrame();
  if ( nvf < 1 ) {
    ErrMsg(MsgERR, "Layout Windows: no valid ViewFrame");
    if ( IsModal() )
      EndModal(vsn_wxIDOK);
    else
      this->Hide();
    return;
  }

  int m, n, x0, y0, x1, y1;
  LDirType ldir;
  wxString vstr;
  vstr = m_pMTxt->GetValue(); m = atoi(vsnApp::ConvWxToSys(vstr).c_str());
  vstr = m_pNTxt->GetValue(); n = atoi(vsnApp::ConvWxToSys(vstr).c_str());
  vstr = m_pX0Txt->GetValue(); x0 = atoi(vsnApp::ConvWxToSys(vstr).c_str());
  vstr = m_pY0Txt->GetValue(); y0 = atoi(vsnApp::ConvWxToSys(vstr).c_str());
  vstr = m_pX1Txt->GetValue(); x1 = atoi(vsnApp::ConvWxToSys(vstr).c_str());
  vstr = m_pY1Txt->GetValue(); y1 = atoi(vsnApp::ConvWxToSys(vstr).c_str());
  ldir = (LDirType)m_pDirRadio->GetSelection();

  if ( nvf > m * n ) {
    ErrMsg(MsgWARN, "Layout Windows: grid size(MxN) less than #of ViewFrame");
    return;
  }
  int wpx, wpy, ww, wh;
  ww = (x1 - x0) / m; wh = (y1 - y0) / n;
  if ( ww < WL_WIN_MINSZ || wh < WL_WIN_MINSZ ) {
    ErrMsg(MsgWARN, "Layout Windows: ViewFrame will be collapsed");
    return;
  }

  register int i, j, idx;
  vsnViewFrame* pvf;
  idx = 0;
  if ( ldir == LDIR_HORIZONTAL ) {
    for ( j = 0, wpy = y0; j < n; j++, wpy += wh ) {
      for ( i = 0, wpx = x0; i < m; i++, wpx += ww ) {
	pvf = pApp->getViewFrame(idx++);
	if ( ! pvf ) continue;
	(void)pvf->setGeometry(wpx, wpy, ww, wh);
      } // end of for(i)
    } // end of for(j)
  } else {
    for ( i = 0, wpx = x0; i < m; i++, wpx += ww ) {
      for ( j = 0, wpy = y0; j < n; j++, wpy += wh ) {
	pvf = pApp->getViewFrame(idx++);
	if ( ! pvf ) continue;
	(void)pvf->setGeometry(wpx, wpy, ww, wh);
      } // end of for(j)
    } // end of for(i)
  }

  s_M = m; s_N = n; s_X0 = x0; s_Y0 = y0; s_X1 = x1; s_Y1 = y1; s_LDir = ldir;

  if ( IsModal() )
    EndModal(vsn_wxIDOK);
  else
    this->Hide();
}

void vsnViewFrameWinLayoutDlg::OnCancelBtn(wxCommandEvent& event) {
  if ( IsModal() )
    EndModal(vsn_wxIDCANCEL);
  else
    this->Hide();
}

void vsnViewFrameWinLayoutDlg::OnResetBtn(wxCommandEvent& event) {
  s_M = s_N = 2;
  s_X0 = s_Y0 = s_X1 = s_Y1 = 0;
  s_LDir = LDIR_HORIZONTAL;
  (void)update();
}
