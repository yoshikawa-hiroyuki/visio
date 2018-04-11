//
// vsnCMap
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

#include "wx/list.h"

#include <fstream>
#include "vsnCMap.h"
#include "vsnApp.h"
#include "vsnError.h"
#include "vsnMethodObj.h"
#include "vsnUiView.h"
#include "vsnColorBar.h"

#define CMAP_DLG_INITIAL_ALPHA 0

using namespace std;
using namespace VSN;


//----------------------------------------------------------------
// class vsnCMapBar
//----------------------------------------------------------------

BEGIN_EVENT_TABLE(vsnCMapBar, wxWindow)
  EVT_PAINT(vsnCMapBar::OnPaint)
END_EVENT_TABLE()


vsnCMapBar::vsnCMapBar(wxWindow* parent,
		       const wxPoint& pos, const wxSize& size)
  : wxWindow(parent, -1, pos, size, wxNO_FULL_REPAINT_ON_RESIZE),
  m_lut(), m_useLut(true), m_showLutFlg(CMBshowRGBA)
  /* style must include wxNO_FULL_REPAINT_ON_RESIZE
     and must NOT include wxRETAINED in Windows env */
{
  m_colour[0] = m_colour[1] = m_colour[2] = m_colour[3] = 1.f;
  m_hilight = 0.f;
}

vsnCMapBar::~vsnCMapBar() {
}


void vsnCMapBar::setUseLut(const bool ulm) {
  if ( m_useLut == ulm ) return;
  m_useLut = ulm;
  Refresh(FALSE);
}

void vsnCMapBar::setShowLutFlag(const long flg) {
  if ( m_showLutFlg == flg ) return;
  m_showLutFlg = flg;
  Refresh(FALSE);
}

void vsnCMapBar::setBaseColor(const vector4 cv) {
  memcpy(m_colour, cv, sizeof(vector4));
  Refresh(FALSE);
}

void vsnCMapBar::setHilight(const float hl) {
  if ( m_hilight == hl ) return;
  m_hilight = hl;
  if ( m_hilight < 0.f ) m_hilight = 0.f;
  else if ( m_hilight > 1.f ) m_hilight = 1.f;
  Refresh(FALSE);
}

void vsnCMapBar::setLut(const vsnLut& lut) {
  m_lut = lut;
  m_lut.normalize();
  Refresh(FALSE);
}


void vsnCMapBar::OnPaint(wxPaintEvent& WXUNUSED(event)) {
  wxPaintDC dc(this);
  wxSize wsz = GetSize();

  if ( m_useLut ) {
    wxPen pen; pen.SetWidth(1);
    const float delta = (float)wsz.GetWidth() / m_lut.numEntry;
    register float dv;
    register int h = wsz.GetHeight(), ha = wsz.GetHeight() / 3;
    if ( m_showLutFlg == CMBshowALP ) ha = h;

    register int i;
    // show rgb
    if ( m_showLutFlg & CMBshowRGB ) {
      for ( i = 0; i < m_lut.numEntry; i++ ) {
	pen.SetColour((int)(m_lut.lutEntry[i*4]*255),
		      (int)(m_lut.lutEntry[i*4 +1]*255),
		      (int)(m_lut.lutEntry[i*4 +2]*255));
	dc.SetPen(pen);
	dc.DrawLine((int)(i*delta), 0, (int)(i*delta), h);
      } // end of for(i)
    }

    // show alpha
    if ( m_showLutFlg & CMBshowALP ) {
      for ( i = 0; i < m_lut.numEntry; i++ ) {
	dv = m_lut.lutEntry[i*4+3];
	dv = (2.f*dv - dv*dv) * 255.f;
	pen.SetColour((int)dv, (int)dv, (int)dv);
	dc.SetPen(pen);
	dc.DrawLine((int)(i*delta), 0, (int)(i*delta), ha);
      } // end of for(i)
    }
  } // end of if(m_useLut)
  else {
    wxBrush brush;
    int w3 = wsz.GetWidth() / 3 + 1;

    // paint base_color
    brush.SetColour((int)(m_colour[0]*255),
		    (int)(m_colour[1]*255),
		    (int)(m_colour[2]*255));
    dc.SetBrush(brush);
    dc.DrawRectangle(0, 0, w3, wsz.GetHeight());

    // paint alpha
    brush.SetColour((int)(m_colour[3]*255),
		    (int)(m_colour[3]*255),
		    (int)(m_colour[3]*255));
    dc.SetBrush(brush);
    dc.DrawRectangle(w3 -1, 0, w3, wsz.GetHeight());

    // paint hilight
    brush.SetColour((int)(m_hilight*255),
		    (int)(m_hilight*255),
		    (int)(m_hilight*255));
    dc.SetBrush(brush);
    dc.DrawRectangle(w3 * 2 -2, 0, w3, wsz.GetHeight());
  } // end of if(!m_useLut)
}


//----------------------------------------------------------------
// class vsnCMapCanvas
//----------------------------------------------------------------

BEGIN_EVENT_TABLE(vsnCMapCanvas, wxGLCanvas)
  EVT_SIZE(vsnCMapCanvas::OnSize)
  EVT_PAINT(vsnCMapCanvas::OnPaint)
  EVT_ERASE_BACKGROUND(vsnCMapCanvas::OnEraseBackground)
  EVT_MOUSE_EVENTS(vsnCMapCanvas::OnMouse)
END_EVENT_TABLE()


vsnCMapCanvas::vsnCMapCanvas(wxWindow* parent, vsnExtLutRefer* pExtRef,
			     const wxWindowID id,
			     const wxPoint& pos, const wxSize& size,
			     long style, const wxString& name)
  : wxGLCanvas(parent, id, pos, size, style, name),
    p_extRefer(pExtRef), m_rgbaCnl(cnlRED), m_lastX(-1)
{
#if CMAP_DLG_INITIAL_ALPHA
  m_rgbaCnl = cnlALPHA;
#endif
}

vsnCMapCanvas::~vsnCMapCanvas() {
}


/* interface */

void vsnCMapCanvas::setLut(const vsnLut& lut) {
  m_lut = lut;
  Refresh(FALSE);
}

void vsnCMapCanvas::setRGBAChannel(const VSN::RGBAcnlType cnl) {
  if ( m_rgbaCnl == cnl ) return;
  m_rgbaCnl = cnl;
  Refresh(FALSE);
}


/* event handlers */

void vsnCMapCanvas::OnPaint(wxPaintEvent& event) {
  /* must always be here */
  wxPaintDC dc(this);

#ifndef __WXMOTIF__
  if (!GetContext()) return;
#endif
  if ( ! IsShown() ) return;
#if defined(MACOSX)
  wxWindow* pw = GetParent();
  if ( pw ) {
    //pw->Iconize(false);
    pw->SetFocus();
    pw->Raise();
    pw->Show(true);
  }
#endif // MACOSX
  SetCurrent();

  /* draw */
  glClearColor(0.858, 0.858, 0.439, 0.0);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  if ( m_lut.numEntry < 1 ) return;

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  const size_t cOff = (size_t)m_rgbaCnl;
  const float yed = 0.15f;
  register float y, dx = 2.f/m_lut.numEntry;
  vector3 colorv = {0.f, 0.f, 0.f};

  glBegin(GL_QUADS);
  register unsigned i;
  for ( i = 0; i < m_lut.numEntry; i++ ) {
    if ( m_rgbaCnl == cnlALPHA ) {
      register float dv = m_lut.lutEntry[4*i + cOff];
      dv = 2.f*dv - dv*dv;
      colorv[0] = colorv[1] = colorv[2] = dv;
    } else
      colorv[cOff] = m_lut.lutEntry[4*i + cOff];
    glColor3fv(colorv);

    y = (2.f - yed*2.f) * m_lut.lutEntry[4*i + cOff] - (1.f - yed);
    glVertex3f(-1.f + i*dx,     -1.f+yed, 0.f);
    glVertex3f(-1.f + (i+1)*dx, -1.f+yed, 0.f);
    glVertex3f(-1.f + (i+1)*dx,  y,       0.f);
    glVertex3f(-1.f + i*dx,      y,       0.f);
  } // end of for(i)

  // paint band
  glColor3f(0.3f, 0.3f, 0.2f);
  glVertex3f(-1.f, -1.f,     0.f);
  glVertex3f( 1.f, -1.f,     0.f);
  glVertex3f( 1.f, -1.f+yed, 0.f);
  glVertex3f(-1.f, -1.f+yed, 0.f);
  glVertex3f(-1.f, 1.f-yed, 0.f);
  glVertex3f( 1.f, 1.f-yed, 0.f);
  glVertex3f( 1.f, 1.f,     0.f);
  glVertex3f(-1.f, 1.f,     0.f);

  glEnd();

  /* flush */
  glFlush();

  /* swap */
  SwapBuffers();
}

void vsnCMapCanvas::OnSize(wxSizeEvent& event) {
  // this is also necessary to update the context on some platforms
  wxGLCanvas::OnSize(event);

  // set GL viewport (not called by wxGLCanvas::OnSize on all platforms...)
  int w, h;
  GetClientSize(&w, &h);
#ifndef __WXMOTIF__
  if (GetContext())
#endif
    {
#if defined(MACOSX)
      wxWindow* pw = GetParent();
      if ( pw ) {
	//pw->Iconize(false);
	pw->SetFocus();
	pw->Raise();
	pw->Show(true);
      }
#endif // MACOSX
      SetCurrent();
      glViewport(0, 0, (GLint)w, (GLint)h);
    }
}

void vsnCMapCanvas::OnEraseBackground(wxEraseEvent& event) {
  /* Do nothing, to avoid flashing on MSW */
}

void vsnCMapCanvas::OnMouse(wxMouseEvent& event) {
  wxSize sz(GetClientSize());

  if ( event.Dragging() || event.ButtonUp() || event.ButtonDown() ||
       (event.Leaving() && (event.LeftIsDown() || event.RightIsDown())) ) {
    vector2 wsz; wsz[0] = sz.GetWidth(); wsz[1] = sz.GetHeight();
    vector2 evpos;
    evpos[0] = VFR_CLAMP(0, wsz[0], event.m_x);
    evpos[1] = VFR_CLAMP(0, wsz[1], event.m_y);
    const size_t cOff = (size_t)m_rgbaCnl;

    const float yed = 0.15f;
    const float yedpx = wsz[1] * yed / (2.f + yed * 2.f);
    long x = (long)(evpos[0] * (m_lut.numEntry -1.f) / wsz[0]);
    float y = (wsz[1] - evpos[1] - yedpx) / (wsz[1] - yedpx * 2.f);
    if ( x < 0 ) x = 0;
    else if ( x > 255 ) x = 255;
    if ( y < 0.f ) y = 0.f;
    else if ( y > 1.f ) y = 1.f;

    m_lut.lutEntry[4*x + cOff] = y;
    m_lut.isStdLut = FALSE;

    if ( event.Dragging() && m_lastX >= 0 ) {
      register long i;
      register float a, b;
      if ( m_lastX < x ) {
	a = (y - m_lut.lutEntry[4*m_lastX + cOff]) / (float)(x - m_lastX);
	b = m_lut.lutEntry[4*m_lastX + cOff] - a * m_lastX;
	for ( i = m_lastX +1; i < x; i++ )
	  m_lut.lutEntry[4*i + cOff] = a * i + b;
      } else if ( m_lastX > x ) {
	a = (m_lut.lutEntry[4*m_lastX + cOff] - y) / (float)(m_lastX - x);
	b = y - a * x;
	for ( i = x +1; i < m_lastX; i++ )
	  m_lut.lutEntry[4*i + cOff] = a * i + b;
      }
    }
    m_lastX = x;

    Refresh(FALSE);
  } // end of Dragging|ButtonUp|ButtonDown

  if ( event.ButtonUp() ||
       (event.Leaving() && (event.LeftIsDown() || event.RightIsDown())) ) {
    if ( p_extRefer ) {
      p_extRefer->ext_setLut(m_lut);
    }
    //if ( event.ButtonUp() )
      m_lastX = -1;
  }// end of ButtonUp|Leaving
}


//----------------------------------------------------------------
// class vsnCMapDlg
//----------------------------------------------------------------

BEGIN_EVENT_TABLE(vsnCMapDlg, wxDialog)
  EVT_RADIOBOX(CMapDlg_ChannelRadio, vsnCMapDlg::OnChannelRadio)
  EVT_BUTTON(CMapDlg_ImportBtn, vsnCMapDlg::OnImportBtn)
  EVT_BUTTON(CMapDlg_ExportBtn, vsnCMapDlg::OnExportBtn)
  EVT_BUTTON(CMapDlg_RampBtn, vsnCMapDlg::OnRampBtn)
  EVT_BUTTON(CMapDlg_InvertBtn, vsnCMapDlg::OnInvertBtn)
  EVT_BUTTON(CMapDlg_CopyBtn, vsnCMapDlg::OnCopyBtn)
  EVT_BUTTON(CMapDlg_ResetBtn, vsnCMapDlg::OnResetBtn)
  EVT_CHECKBOX(CMapDlg_DisplayChk, vsnCMapDlg::OnDisplayChk)
  EVT_BUTTON(CMapDlg_LayoutBtn, vsnCMapDlg::OnLayoutBtn)
  EVT_BUTTON(CMapDlg_CancelBtn, vsnCMapDlg::OnCancelBtn)
  EVT_BUTTON(CMapDlg_CloseBtn, vsnCMapDlg::OnCloseBtn)
  EVT_CLOSE(vsnCMapDlg::OnClose)
END_EVENT_TABLE()


/* static members */

std::string vsnCMapDlg::s_lutImpDir;


/* constructors / destructor */

vsnCMapDlg::vsnCMapDlg(wxWindow *parent, vsnMethodObj* pMtd)
  : wxDialog(parent, -1, wxString(wxT("CMapDlg"))), p_refMethod(pMtd),
    m_pGlCanvas(NULL), m_pCMapBar(NULL), m_pChannel(NULL), m_dispCBar(false),
    m_pRamp(NULL), m_pImport(NULL), m_pExport(NULL),
    m_pInvert(NULL), m_pCopy(NULL), m_pReset(NULL),
    m_pDisplay(NULL), m_pLayout(NULL), m_pCancel(NULL), m_pClose(NULL)
{
  assert(parent);

  /* prepare parts */
  m_pGlCanvas = new vsnCMapCanvas(this, this);
  assert(m_pGlCanvas);

  m_pCMapBar = new vsnCMapBar(this, wxDefaultPosition, wxSize(256,25));
  assert(m_pCMapBar);

  wxString ritems[] = {wxString(wxT("red")), wxString(wxT("green")),
                       wxString(wxT("blue")), wxString(wxT("alpha"))};
  m_pChannel = new wxRadioBox(this, CMapDlg_ChannelRadio,
			      wxT("color channel"),
			      wxDefaultPosition, wxDefaultSize,
			      4, ritems, 1, wxRA_SPECIFY_COLS);
  assert(m_pChannel);
#if CMAP_DLG_INITIAL_ALPHA
  m_pChannel->SetSelection(3);
#endif

  m_pRamp = new wxButton(this, CMapDlg_RampBtn, wxT("ramp"));
  assert(m_pRamp);
  m_pImport = new wxButton(this, CMapDlg_ImportBtn, wxT("import"));
  assert(m_pImport);
  m_pExport = new wxButton(this, CMapDlg_ExportBtn, wxT("export"));
  assert(m_pExport);
  m_pInvert = new wxButton(this, CMapDlg_InvertBtn, wxT("invert"));
  assert(m_pInvert);
  m_pCopy = new wxButton(this, CMapDlg_CopyBtn, wxT("copy"));
  assert(m_pCopy);
  m_pReset = new wxButton(this, CMapDlg_ResetBtn, wxT("reset"));
  assert(m_pReset);
  m_pDisplay = new wxCheckBox(this, CMapDlg_DisplayChk, wxT("display"));
  assert(m_pReset);
  m_pLayout = new wxButton(this, CMapDlg_LayoutBtn, wxT("layout"));
  assert(m_pLayout);
  m_pCancel = new wxButton(this, CMapDlg_CancelBtn, wxT("cancel"));
  assert(m_pCancel);
  m_pClose = new wxButton(this, CMapDlg_CloseBtn, wxT("close"));
  assert(m_pClose);

  /* top Sizer */
  wxBoxSizer* topsizer = new wxBoxSizer(wxHORIZONTAL);

  /* left hand side */
  wxBoxSizer* sizerV = new wxBoxSizer(wxVERTICAL);
  sizerV->Add(m_pGlCanvas, 1, wxEXPAND|wxALL, 5);
  sizerV->Add(m_pCMapBar, 0, wxEXPAND|wxALL, 5);
  topsizer->Add(sizerV, 0, wxEXPAND);

  /* right hand side */
  sizerV = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer* sizerH = new wxBoxSizer(wxHORIZONTAL);
  sizerH->Add(m_pChannel, 1, wxEXPAND|wxALL, 3);
  wxBoxSizer* sizerV2 = new wxBoxSizer(wxVERTICAL);
  sizerH->Add(sizerV2, 0, wxALL, 0);
  sizerV2->Add(m_pImport, 0, wxALL, 3);
  sizerV2->Add(m_pExport, 0, wxALL, 3);
  sizerV->Add(sizerH, 0, wxEXPAND|wxALL, 0);
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  sizerH->Add(m_pRamp, 1, wxEXPAND|wxALL, 3);
  sizerH->Add(m_pInvert, 1, wxEXPAND|wxALL, 3);
  sizerV->Add(sizerH, 0, wxEXPAND|wxALL, 0);
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  sizerH->Add(m_pCopy, 1, wxEXPAND|wxALL, 3);
  sizerH->Add(m_pReset, 1, wxEXPAND|wxALL, 3);
  sizerV->Add(sizerH, 0, wxEXPAND|wxALL, 0);
  sizerV->Add(5, 5, 1, wxEXPAND);
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  sizerH->Add(m_pDisplay, 1, wxEXPAND|wxALL, 3);
  sizerH->Add(m_pLayout, 1, wxEXPAND|wxALL, 3);
  sizerV->Add(sizerH, 0, wxEXPAND|wxALL, 0);
  sizerV->Add(10, 10);
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  sizerH->Add(m_pCancel, 1, wxEXPAND|wxALL, 3);
  sizerH->Add(m_pClose, 1, wxEXPAND|wxALL, 3);
  sizerV->Add(sizerH, 0, wxEXPAND|wxALL, 0);
  topsizer->Add(sizerV, 1, wxEXPAND);

  /* post process */
  SetAutoLayout(TRUE);
  SetSizer(topsizer);
  topsizer->SetSizeHints(this);
  topsizer->Fit(this);

  /* set lut */
  if ( p_refMethod ) {
    setLut(p_refMethod->getLut());
    m_lut_back = m_lut;
  }
}

vsnCMapDlg::~vsnCMapDlg() {
}


/* interface */

void vsnCMapDlg::setLut(const vsnLut& lut, const bool updRef) {
  m_lut = lut;

  if ( m_pGlCanvas ) m_pGlCanvas->setLut(m_lut);
  if ( m_pCMapBar ) m_pCMapBar->setLut(m_lut);

  if ( updRef ) updateRefMethod();
}

void vsnCMapDlg::setRefMethod(vsnMethodObj* pMtd) {
  //if ( p_refMethod == pMtd ) return; // don't check same-val, for lut_back
  p_refMethod = pMtd;

  if ( p_refMethod ) {
    setLut(p_refMethod->getLut());
    m_lut_back = m_lut;
  }
}

void vsnCMapDlg::updateRefMethod() {
  if ( p_refMethod ) {
    m_lut.minVal = p_refMethod->getLut().minVal;
    m_lut.maxVal = p_refMethod->getLut().maxVal;
    p_refMethod->setLut(m_lut);
  }
}

void vsnCMapDlg::setDispCBar(const bool disp) {
  if ( m_dispCBar == disp ) return;
  m_dispCBar = disp;

  if ( m_pDisplay ) {
    m_pDisplay->SetValue(m_dispCBar);
  }
}


/* event handler */

void vsnCMapDlg::OnChannelRadio(wxCommandEvent& event) {
  if ( ! m_pChannel || ! m_pGlCanvas ) return;

  int val = m_pChannel->GetSelection();
  int cval = (int)m_pGlCanvas->getRGBAChannel();
  if ( val == cval ) return;

  m_pGlCanvas->setRGBAChannel((RGBAcnlType)val);
}

void vsnCMapDlg::OnImportBtn(wxCommandEvent& event) {
  vsnApp* pApp = vsnApp::GetApp();
  if ( ! pApp ) return;

  // prepare file dialog
  wxFileDialog fileDlg(this, wxT("select lut file to import"),
                       wxT(""), wxT(""), // default Dir / File
                       wxT("lut file (*.lut)|*.lut|(*)|*"),
                       wxFD_OPEN);
  // set default dir
  string appImpDir = pApp->getImportDir();
  string appCurDir = pApp->getCwd();
  if ( ! s_lutImpDir.empty() )
    fileDlg.SetDirectory(vsnApp::ConvSysToWx(s_lutImpDir));
  else if ( ! appImpDir.empty() )
    fileDlg.SetDirectory(vsnApp::ConvSysToWx(appImpDir));
  else if ( ! appCurDir.empty() )
    fileDlg.SetDirectory(vsnApp::ConvSysToWx(appCurDir));

  // get file path from the dialog
  if ( fileDlg.ShowModal() != wxID_OK ) return;
  string fpath = vsnApp::ConvWxToSys(fileDlg.GetPath()); 
  if ( fpath.empty() ) return;
  fpath = vsnPath_normalize(fpath);
  s_lutImpDir = CES::DirName(fpath, vsnPath_getDelimChar());

  // confirm
  vsnCMapConfirmDlg confDlg(this, fpath);
  if ( ! confDlg.getStatus() ) {
    ErrMsg(MsgINFO, string("Import colormap: can't load lut file: ") + fpath);
    return;
  }
  if ( confDlg.ShowModal() != wxID_OK ) return;

  // override lut (ignore min/max)
  setLut(confDlg.getLut(), true);
}

void vsnCMapDlg::OnExportBtn(wxCommandEvent& event) {
  vsnApp* pApp = vsnApp::GetApp();
  if ( ! pApp ) return;

  // prepare file dialog
  wxFileDialog fileDlg(this, wxT("select lut file to export"),
                       wxT(""), wxT(""), // default Dir / File
                       wxT("lut file (*.lut)|*.lut|(*)|*"),
                       wxFD_SAVE);
  // set default dir
  string appImpDir = pApp->getImportDir();
  string appCurDir = pApp->getCwd();
  if ( ! s_lutImpDir.empty() )
    fileDlg.SetDirectory(vsnApp::ConvSysToWx(s_lutImpDir));
  else if ( ! appImpDir.empty() )
    fileDlg.SetDirectory(vsnApp::ConvSysToWx(appImpDir));
  else if ( ! appCurDir.empty() )
    fileDlg.SetDirectory(vsnApp::ConvSysToWx(appCurDir));

  // get output path
  if ( fileDlg.ShowModal() != wxID_OK ) return;
  string outPath = vsnApp::ConvWxToSys(fileDlg.GetPath());
  if ( outPath.empty() ) return;
  outPath = vsnPath_normalize(outPath);
  s_lutImpDir = CES::DirName(outPath, vsnPath_getDelimChar());

  // override check
  FILE* ofp = fopen(outPath.c_str(), "r");
  if ( ofp ) {
    fclose(ofp);
    string msg = "The specified file has already existed\n  ";
    msg += outPath;
    msg += "\n\nAre you sure to override ?\n";
    wxMessageDialog dlg(NULL, vsnApp::ConvSysToWx(msg), wxT("Export colormap"),
			vsn_wxOK_CANCEL|wxICON_QUESTION);
    if ( dlg.ShowModal() != vsn_wxIDOK ) return;
  }

  // export
  ofstream ofs(outPath.c_str());
  if ( ! ofs ) {
    ErrMsg(MsgINFO, string("Export colormap: can't open lut file: ")
	   + outPath.c_str());
    return;
  }
  if ( ! m_lut.ExportStream(ofs) ) {
    ErrMsg(MsgINFO, string("Export colormap: can't save lut file: ")
	   + outPath.c_str());
    return;
  }
}

void vsnCMapDlg::OnRampBtn(wxCommandEvent& event) {
  if ( ! m_pChannel || ! m_pGlCanvas ) return;

  vsnLut rlut(m_lut);
  int cval = m_pChannel->GetSelection();
  register int i;
  for ( i = 0; i < rlut.numEntry; i++ )
    rlut.lutEntry[i*4+cval] = (float)i/(float)(rlut.numEntry -1);
  rlut.isStdLut = FALSE;

  setLut(rlut, true);
}

void vsnCMapDlg::OnInvertBtn(wxCommandEvent& event) {
  if ( ! m_pChannel || ! m_pGlCanvas ) return;

  vsnLut rlut(m_lut);
  int cval = m_pChannel->GetSelection();
  register int i;
  for ( i = 0; i < rlut.numEntry/2; i++ ) {
    float x = rlut.lutEntry[i*4+cval];
    rlut.lutEntry[i*4+cval] = rlut.lutEntry[(rlut.numEntry -1 -i)*4+cval];
    rlut.lutEntry[(rlut.numEntry -1 -i)*4+cval] = x;
  }
  rlut.isStdLut = FALSE;

  setLut(rlut, true);
}

void vsnCMapDlg::OnCopyBtn(wxCommandEvent& event) {
  if ( ! p_refMethod ) return;

  register size_t i, j, k;
  deque<vsnMethodObj*> mtdLst;
  size_t numScene = vsnApp::GetApp()->getNumScene();
  for ( i = 0; i < numScene; i++ ) {
    vsnScene* psc = vsnApp::GetApp()->getScene(i);
    if ( ! psc ) continue;
    size_t numOG = psc->getNumObjGroup();
    for ( j = 0; j < numOG; j++ ) {
      vsnObjGroup* pog = psc->getObjGroup(j);
      if ( ! pog ) continue;
      size_t numData = pog->getNumData();
      for ( k = 0; k < numData; k++ ) {
	vsnDataObj* pdt = pog->getData(k);
	if ( ! pdt ) continue;
	vsnMethodObj* pmtd = pdt->getNextMethod(NULL);
	while ( pmtd ) {
	  if ( pmtd != p_refMethod )
	    mtdLst.push_back(pmtd);
	  pmtd = pdt->getNextMethod(pmtd);
	} // end of while(pmtd)
      } // end of for(k)
    } // end of for(j)
  } // end of for(i)

  if ( mtdLst.size() < 1 ) {
    ErrMsg(MsgINFO, "CMapDlg: there is no other method to reference CMap");
    return;
  }

  wxString* choices = new wxString[mtdLst.size()];
  for ( i = 0; i < mtdLst.size(); i++ ) {
    wxString item = vsnApp::ConvSysToWx(mtdLst[i]->getMethodType());
    item += wxT("[");
    item += vsnApp::ConvSysToWx(mtdLst[i]->getName());
    item += wxT("] of data ");
    vsnDataObj* pdt = mtdLst[i]->getRefData();
    if ( pdt ) {
      item += vsnApp::ConvSysToWx(pdt->getDataType());
      item += wxT("[");
      item += vsnApp::ConvSysToWx(pdt->getName());
      item += wxT("]");
    }
    choices[i] = item;
  } // end of for(i)

  wxSingleChoiceDialog
    dlg(this, wxT("select the method of the copy origin"),
	wxT("reference copy of CMap"),
	mtdLst.size(), choices);
  delete [] choices;
  if ( dlg.ShowModal() != wxID_OK ) return;

  int val = dlg.GetSelection();
  if ( val < 0 || val >= mtdLst.size() ) {
    ErrMsg(MsgERR, "CMapDlg: invalid selection");
    return;
  }

  setLut(mtdLst[val]->getLut(), false);
  // updateRefMethod() won't update min/max, so call mtd->setLut() directly
  p_refMethod->setLut(m_lut);
}

void vsnCMapDlg::OnResetBtn(wxCommandEvent& event) {
  vsnLut rlut;
  setLut(rlut, true);
}

void vsnCMapDlg::OnDisplayChk(wxCommandEvent& event) {
  if ( ! m_pDisplay ) return;
  bool val = m_pDisplay->GetValue();
  if ( m_dispCBar == val ) return;
  m_dispCBar = val;

  if ( ! p_refMethod ) return;
  vsnColorBar* pcb = p_refMethod->getColorBar();
  if ( ! pcb ) return;

  p_refMethod->showColorBar(m_dispCBar);
}

void vsnCMapDlg::OnLayoutBtn(wxCommandEvent& event) {
  if ( ! p_refMethod ) return;
  vsnColorBar* pcb = p_refMethod->getColorBar();
  if ( ! pcb ) return;

  vsnColorBarDlg dlg(this, pcb);
  dlg.ShowModal();
}

void vsnCMapDlg::OnCancelBtn(wxCommandEvent& event) {
  setLut(m_lut_back, true);

  if ( IsModal() )
    EndModal(vsn_wxIDCANCEL);
  else
    this->Hide();
}

void vsnCMapDlg::OnCloseBtn(wxCommandEvent& event) {
  if ( IsModal() )
    EndModal(vsn_wxIDOK);
  else
    this->Hide();
}

void vsnCMapDlg::OnClose(wxCloseEvent& event) {
}


//----------------------------------------------------------------
// class vsnAMapDlg
//----------------------------------------------------------------

BEGIN_EVENT_TABLE(vsnAMapDlg, wxDialog)
  EVT_BUTTON(AMapDlg_ImportBtn, vsnAMapDlg::OnImportBtn)
  EVT_BUTTON(AMapDlg_ExportBtn, vsnAMapDlg::OnExportBtn)
  EVT_BUTTON(AMapDlg_RampBtn, vsnAMapDlg::OnRampBtn)
  EVT_BUTTON(AMapDlg_InvertBtn, vsnAMapDlg::OnInvertBtn)
  EVT_BUTTON(AMapDlg_ResetBtn, vsnAMapDlg::OnResetBtn)
  EVT_BUTTON(AMapDlg_CancelBtn, vsnAMapDlg::OnCancelBtn)
  EVT_BUTTON(AMapDlg_CloseBtn, vsnAMapDlg::OnCloseBtn)
  EVT_CLOSE(vsnAMapDlg::OnClose)
END_EVENT_TABLE()


/* constructors / destructor */

vsnAMapDlg::vsnAMapDlg(wxWindow *parent, vsnExtLutRefer* pref,
		       const VSN::RGBAcnlType cnl)
: wxDialog(parent, -1, wxString(wxT("AMapDlg"))),
  m_pGlCanvas(NULL), m_pCMapBar(NULL),
  m_pRamp(NULL), m_pImport(NULL), m_pExport(NULL),
  m_pInvert(NULL), m_pReset(NULL), m_pCancel(NULL), m_pClose(NULL),
  m_useChannel(cnl), p_refer(NULL)
{
  assert(parent);

  /* prepare parts */
  m_pGlCanvas = new vsnCMapCanvas(this, this);
  assert(m_pGlCanvas);

  m_pCMapBar = new vsnCMapBar(this, wxDefaultPosition, wxSize(256,25));
  assert(m_pCMapBar);
  m_pCMapBar->setShowLutFlag(vsnCMapBar::CMBshowALP);

  m_pRamp = new wxButton(this, AMapDlg_RampBtn, wxT("ramp"));
  assert(m_pRamp);
  m_pImport = new wxButton(this, AMapDlg_ImportBtn, wxT("import"));
  assert(m_pImport);
  m_pExport = new wxButton(this, AMapDlg_ExportBtn, wxT("export"));
  assert(m_pExport);
  m_pInvert = new wxButton(this, AMapDlg_InvertBtn, wxT("invert"));
  assert(m_pInvert);
  m_pReset = new wxButton(this, AMapDlg_ResetBtn, wxT("reset"));
  assert(m_pReset);
  m_pCancel = new wxButton(this, AMapDlg_CancelBtn, wxT("cancel"));
  assert(m_pCancel);
  m_pClose = new wxButton(this, AMapDlg_CloseBtn, wxT("close"));
  assert(m_pClose);

  /* top Sizer */
  wxBoxSizer *topsizer = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *sizerV;

  /* left hand side */
  sizerV = new wxBoxSizer(wxVERTICAL);
  sizerV->Add(m_pGlCanvas, 1, wxEXPAND|wxALL, 5);
  sizerV->Add(m_pCMapBar, 0, wxEXPAND|wxALL, 5);
  topsizer->Add(sizerV, 0, wxEXPAND);

  /* right hand side */
  sizerV = new wxBoxSizer(wxVERTICAL);
  sizerV->Add(m_pImport, 0, wxEXPAND|wxALL, 3);
  sizerV->Add(m_pExport, 0, wxEXPAND|wxALL, 3);
  sizerV->Add(5, 5);
  sizerV->Add(m_pRamp, 0, wxEXPAND|wxALL, 3);
  sizerV->Add(m_pInvert, 0, wxEXPAND|wxALL, 3);
  sizerV->Add(m_pReset, 0, wxEXPAND|wxALL, 3);
  sizerV->Add(
     new wxStaticLine(this,-1,wxDefaultPosition,wxSize(5,5),wxHORIZONTAL),
     0, wxEXPAND|wxALL, 0);
  sizerV->Add(m_pCancel, 0, wxEXPAND|wxALL, 3);
  sizerV->Add(m_pClose, 0, wxEXPAND|wxALL, 3);
  topsizer->Add(sizerV, 1, wxEXPAND);

  /* post process */
  m_pGlCanvas->setRGBAChannel(m_useChannel);
  SetAutoLayout(TRUE);
  SetSizer(topsizer);
  topsizer->SetSizeHints(this);
  topsizer->Fit(this);
  setRefer(pref);
}

vsnAMapDlg::~vsnAMapDlg() {
}


/* interface */

void vsnAMapDlg::setLut(const vsnLut& lut, const bool updRef) {
  m_lut = lut;

  if ( m_pGlCanvas ) m_pGlCanvas->setLut(m_lut);
  if ( m_pCMapBar ) m_pCMapBar->setLut(m_lut);

  if ( updRef ) updateRefer();
}

void vsnAMapDlg::setRefer(vsnExtLutRefer* pref) {
  // don't check same-val, for lut_back
  p_refer = pref;

  if ( p_refer ) {
    setLut(p_refer->ext_getLut());
    m_lut_back = m_lut;
  }
}

void vsnAMapDlg::updateRefer() {
  if ( ! p_refer ) return;
  p_refer->ext_setLut(m_lut);
}


/* event handler */

void vsnAMapDlg::OnImportBtn(wxCommandEvent& event) {
  vsnApp* pApp = vsnApp::GetApp();
  if ( ! pApp ) return;

  // prepare file dialog
  wxFileDialog fileDlg(this, wxT("select lut file to import"),
                       wxT(""), wxT(""), // default Dir / File
                       wxT("lut file (*.lut)|*.lut|(*)|*"),
                       wxFD_OPEN);
  // set default dir
  string appImpDir = CES::DirName(pApp->getCurrentFilename(),
                                  vsnPath_getDelimChar());
  string appCurDir = pApp->getCwd();
  if ( ! vsnCMapDlg::s_lutImpDir.empty() )
    fileDlg.SetDirectory(vsnApp::ConvSysToWx(vsnCMapDlg::s_lutImpDir));
  else if ( ! appImpDir.empty() )
    fileDlg.SetDirectory(vsnApp::ConvSysToWx(appImpDir));
  else if ( ! appCurDir.empty() )
    fileDlg.SetDirectory(vsnApp::ConvSysToWx(appCurDir));

  // get file path from the dialog
  if ( fileDlg.ShowModal() != wxID_OK ) return;
  string fpath = vsnApp::ConvWxToSys(fileDlg.GetPath()); 
  if ( fpath.empty() ) return;
  fpath = vsnPath_normalize(fpath);
  vsnCMapDlg::s_lutImpDir = CES::DirName(fpath, vsnPath_getDelimChar());

  // confirm
  vsnCMapConfirmDlg confDlg(this, fpath);
  if ( ! confDlg.getStatus() ) {
    ErrMsg(MsgINFO, string("Import colormap: can't load lut file: ") + fpath);
    return;
  }
  if ( confDlg.ShowModal() != wxID_OK ) return;

  // override lut (ignore min/max)
  setLut(confDlg.getLut(), true);
}

void vsnAMapDlg::OnExportBtn(wxCommandEvent& event) {
  vsnApp* pApp = vsnApp::GetApp();
  if ( ! pApp ) return;

  // prepare file dialog
  wxFileDialog fileDlg(this, wxT("select lut file to export"),
                       wxT(""), wxT(""), // default Dir / File
                       wxT("lut file (*.lut)|*.lut|(*)|*"),
                       wxFD_SAVE);
  // set default dir
  string appImpDir = CES::DirName(pApp->getCurrentFilename(),
                                  vsnPath_getDelimChar());
  string appCurDir = pApp->getCwd();
  if ( ! vsnCMapDlg::s_lutImpDir.empty() )
    fileDlg.SetDirectory(vsnApp::ConvSysToWx(vsnCMapDlg::s_lutImpDir));
  else if ( ! appImpDir.empty() )
    fileDlg.SetDirectory(vsnApp::ConvSysToWx(appImpDir));
  else if ( ! appCurDir.empty() )
    fileDlg.SetDirectory(vsnApp::ConvSysToWx(appCurDir));

  // get output path
  if ( fileDlg.ShowModal() != wxID_OK ) return;
  string outPath = vsnApp::ConvWxToSys(fileDlg.GetPath());
  if ( outPath.empty() ) return;
  outPath = vsnPath_normalize(outPath);
  vsnCMapDlg::s_lutImpDir = CES::DirName(outPath, vsnPath_getDelimChar());

  // override check
  FILE* ofp = fopen(outPath.c_str(), "r");
  if ( ofp ) {
    fclose(ofp);
    string msg = "The specified file has already existed\n  ";
    msg += outPath;
    msg += "\n\nAre you sure to override ?\n";
    wxMessageDialog dlg(NULL, vsnApp::ConvSysToWx(msg), wxT("Export colormap"),
                        vsn_wxOK_CANCEL|wxICON_QUESTION);
    if ( dlg.ShowModal() != vsn_wxIDOK ) return;
  }

  // export
  ofstream ofs(outPath.c_str());
  if ( ! ofs ) {
    ErrMsg(MsgINFO, string("Export colormap: can't open lut file: ")
           + outPath.c_str());
    return;
  }
  if ( ! m_lut.ExportStream(ofs) ) {
    ErrMsg(MsgINFO, string("Export colormap: can't save lut file: ")
           + outPath.c_str());
    return;
  }
}

void vsnAMapDlg::OnRampBtn(wxCommandEvent& event) {
  if ( ! m_pGlCanvas ) return;

  vsnLut rlut(m_lut);
  int cval = 3;
  register int i;
  for ( i = 0; i < rlut.numEntry; i++ )
    rlut.lutEntry[i*4+cval] = (float)i/(float)(rlut.numEntry -1);
  rlut.isStdLut = FALSE;

  setLut(rlut, true);
}

void vsnAMapDlg::OnInvertBtn(wxCommandEvent& event) {
  if ( ! m_pGlCanvas ) return;

  vsnLut rlut(m_lut);
  int cval = 3;
  register int i;
  for ( i = 0; i < rlut.numEntry/2; i++ ) {
    float x = rlut.lutEntry[i*4+cval];
    rlut.lutEntry[i*4+cval] = rlut.lutEntry[(rlut.numEntry -1 -i)*4+cval];
    rlut.lutEntry[(rlut.numEntry -1 -i)*4+cval] = x;
  }
  rlut.isStdLut = FALSE;

  setLut(rlut, true);
}

void vsnAMapDlg::OnResetBtn(wxCommandEvent& event) {
  vsnLut rlut;
#if 1 // for deriv map
  size_t i;
  for ( i = 0; i < VSN::LUT_MAX_ENTRY; i++ )
    rlut.lutEntry[i*4+3] = 1.f;
#endif
  setLut(rlut, true);
}

void vsnAMapDlg::OnCancelBtn(wxCommandEvent& event) {
  setLut(m_lut_back, true);

  if ( IsModal() )
    EndModal(vsn_wxIDCANCEL);
  else
    this->Hide();
}

void vsnAMapDlg::OnCloseBtn(wxCommandEvent& event) {
  if ( IsModal() )
    EndModal(vsn_wxIDOK);
  else
    this->Hide();
}

void vsnAMapDlg::OnClose(wxCloseEvent& event) {
}


//----------------------------------------------------------------
// class vsnCMapConfirmDlg
//----------------------------------------------------------------

BEGIN_EVENT_TABLE(vsnCMapConfirmDlg, wxDialog)
  EVT_BUTTON(CMapConfirmDlg_OkBtn, vsnCMapConfirmDlg::OnOkBtn)
  EVT_BUTTON(CMapConfirmDlg_CancelBtn, vsnCMapConfirmDlg::OnCancelBtn)
END_EVENT_TABLE()


/* constructors / destructor */

vsnCMapConfirmDlg::vsnCMapConfirmDlg(wxWindow *parent, const string& path)
  : wxDialog(parent, -1, wxString(wxT("Import colormap"))), m_status(false),
    m_pDirTxt(NULL), m_pFileTxt(NULL), m_pCMapBar(NULL)
{
  assert(parent);

  /* prepare parts */
  m_pDirTxt = new wxStaticText(this, -1, wxT(" Dir: ")); assert(m_pDirTxt);
  m_pFileTxt = new wxStaticText(this, -1, wxT(" File: ")); assert(m_pFileTxt);
  m_pCMapBar = new vsnCMapBar(this, wxDefaultPosition, wxSize(256,25));
  assert(m_pCMapBar);

  /* layout */
  wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL);
  topsizer->Add(new wxStaticText(this, -1,
	 wxT("\n Are you sure to import and override colormap ? \n")));

  topsizer->Add(m_pDirTxt, 0, wxEXPAND|wxALL, 3);
  topsizer->Add(m_pFileTxt, 0, wxEXPAND|wxALL, 3);
  topsizer->Add(m_pCMapBar, 0, wxALIGN_CENTER|wxALL, 3);
  topsizer->Add(0, 5, 0, wxGROW);
  topsizer->Add(
     new wxStaticLine(this,-1,wxDefaultPosition,wxSize(384,3),wxHORIZONTAL),
     0, wxEXPAND|wxALL, 0);

  wxBoxSizer* sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxALIGN_RIGHT|wxALL, 5);
  sizerH->Add(new wxButton(this, CMapConfirmDlg_OkBtn, wxT("OK")));
  sizerH->Add(5, 5);
  sizerH->Add(new wxButton(this, CMapConfirmDlg_CancelBtn, wxT("Cancel")));

  /* post process */
  SetAutoLayout(TRUE);
  SetSizer(topsizer);
  topsizer->SetSizeHints(this);
  topsizer->Fit(this);

  (void)setPath(path);
}

vsnCMapConfirmDlg::~vsnCMapConfirmDlg() {
}


/* interface */

bool vsnCMapConfirmDlg::setPath(const std::string& fpath) {
  if ( m_path == fpath ) return m_status;

  m_status = false;
  m_path = fpath;
  memset(m_lut.lutEntry, 0, sizeof(m_lut.lutEntry));
  m_pCMapBar->setLut(m_lut);

  string dstr = string(" Dir: ") + CES::DirName(fpath, vsnPath_getDelimChar());
  string fstr = string(" File: ")
    + CES::BaseName(fpath, string(""), vsnPath_getDelimChar());
  m_pDirTxt->SetLabel(vsnApp::ConvSysToWx(dstr));
  m_pFileTxt->SetLabel(vsnApp::ConvSysToWx(fstr));

  ifstream ifs(fpath.c_str());
  if ( ! ifs ) return false;
  if ( ! m_lut.ImportStream(ifs) ) return false;

  m_pCMapBar->setLut(m_lut);
  m_status = true;
  return m_status;
}


/* event handler */

void vsnCMapConfirmDlg::OnOkBtn(wxCommandEvent& event) {
  if ( IsModal() )
    EndModal(vsn_wxIDOK);
  else
    this->Hide();
}

void vsnCMapConfirmDlg::OnCancelBtn(wxCommandEvent& event) {
  if ( IsModal() )
    EndModal(vsn_wxIDCANCEL);
  else
    this->Hide();
}
