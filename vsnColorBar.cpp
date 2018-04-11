//
// vsnColorBar
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

#include "vsnApp.h"
#include "vsnColorBar.h"
#include <sstream>

#define CB_TITLE_SIZE 0.08f
#define CB_LABEL_SIZE 0.05f

using namespace std;
using namespace VSN;


//----------------------------------------------------------------
// class vsnColorBar
//----------------------------------------------------------------

/* constructors / destructor */

vsnColorBar::vsnColorBar(vsnMethodObj* pm, const std::string& name)
  : vsnFrontObj(name), vsnMethodLutRefer(pm),
    m_pBar(NULL), m_pTitleLbl(NULL), m_pLbls(NULL),
    m_numLbls(2), m_precLbls(2), m_direction(CBD_VERTICAL), m_showAlpha(false)
{
  // set pickable
  setPickMode(PT_OBJECT);

  // create gfx objs
  m_pBar = new vfrMesh2D(); assert(m_pBar);
  m_pBar->setColorMode(AT_PER_FACE);
  m_pBar->setBboxShowMode(TRUE);
  m_pBar->setBboxWidth(1.f);
  m_pTitleLbl = new vfrLineText(); assert(m_pTitleLbl);
  m_pTitleLbl->setFontScale(CB_TITLE_SIZE);
  m_pTitleLbl->setTransparency(TRUE);
  m_pLbls = new vfrGroup(); assert(m_pLbls);
  addChild(m_pBar);
  addChild(m_pTitleLbl);
  addChild(m_pLbls);

  // initial update
  vector2 sz = {0.20f, 1.f}; setSize(sz);
  vector2 ps = {0.70f, 0.f}; setPosition(ps);
  (void)updateColorBar();

  // show mode
  _material->setRenderMode(RT_NOLIGHT);
}

vsnColorBar::~vsnColorBar() {
}


/* from vsnMethdLutRefer */

bool vsnColorBar::updateLut() {
  if ( p_method )
    m_lut = p_method->getLut();

  return updateColorBar();
}


/* methods */

void vsnColorBar::setLut(const vsnLut& lut) {
  m_lut = lut;
  m_lut.normalize();

  if ( updateColorBar() )
    chkNotice();
}

void vsnColorBar::setShowAlpha(const bool sa) {
  if ( m_showAlpha == sa ) return;
  m_showAlpha = sa;

  if ( updateColorBar() )
    chkNotice();
}

void vsnColorBar::setTitle(const std::string& tstr) {
  if ( ! m_pTitleLbl ) return;
  if ( tstr == m_pTitleLbl->getLettersBuff() ) return;
  m_pTitleLbl->setLetters(tstr);

  if ( updateColorBar() )
    chkNotice();
}

std::string vsnColorBar::getTitle() const {
  if ( ! m_pTitleLbl ) return string("");
  return m_pTitleLbl->getLetters();
}

bool vsnColorBar::setNumLabels(const size_t nl) {
  if ( m_numLbls == nl ) return true;
  if ( m_direction == CBD_HORIZONTAL && nl > 3 ) return false;
  m_numLbls = nl;

  if ( updateColorBar() ) {
    chkNotice();
    return true;
  }
  return false;
}

void vsnColorBar::setLabelPrecision(const size_t lp) {
  if ( m_precLbls == lp ) return;
  m_precLbls = lp;

  if ( updateColorBar() )
    chkNotice();
}

void vsnColorBar::setBaseColor(const vector4 cv) {
  setColor3(0, cv);
  if ( updateColorBar() )
    chkNotice();
}


void vsnColorBar::setDirection(const CBDirType dir) {
  if ( m_direction == dir ) return;
  m_direction = dir;

  if ( m_direction == CBD_HORIZONTAL ) {
    if ( m_numLbls > 3 )
      m_numLbls = 3;
  }

  if ( updateColorBar() )
    chkNotice();
}

bool vsnColorBar::setSize(const vector2 s) {
  if ( m_sz[0] == s[0] && m_sz[1] == s[1] ) return true;
  if ( s[0] <= 0.f || s[1] <= 0.f ) return false;
  m_sz[0] = s[0]; m_sz[1] = s[1];

  if ( updateColorBar() ) {
    chkNotice();
    return true;
  }
  return false;
}


// XML util
bool vsnColorBar::importXMLNode(xmlNodePtr xnp) {
  if ( ! xnp ) return false;

  // is 'color_bar' node?
  if ( strcmp((const char*)xnp->name, "color_bar") ) return false;

  bool ret = true;

  // traverse children node
  xmlNodePtr cur = xnp->xmlChildrenNode;
  for ( ; cur; cur = cur->next ) {
    if ( cur->type != XML_ELEMENT_NODE ) continue;
    string tagName = (const char*)cur->name;
    if ( tagName.empty() ) continue;
    string tagValue;
    xmlChar* xv = xmlGetProp(cur, (const xmlChar*)"value");
    if ( xv && strlen((const char*)xv) > 0 ) {
      tagValue = (const char*)xv; xmlFree(xv);
    }

    if ( tagName == "direction" ) {
      if ( tagValue == "horizontal" )
	setDirection(CBD_HORIZONTAL);
      else if ( tagValue == "vertical" )
	setDirection(CBD_VERTICAL);
      else
	ret = false;
    } // end of "direction"
    else if ( tagName == "show_alpha" ) {
      if ( tagValue == "yes" )
	setShowAlpha(true);
      else if ( tagValue == "no" )
	setShowAlpha(false);
      else
	ret = false;
    }
    else if ( tagName == "title" ) {
      setTitle(tagValue);
    } // end of "title"
    else if ( tagName == "num_labels" ) {
      if ( tagValue.empty() ) {ret = false; continue;}
      int nlbl = atoi(tagValue.c_str());
      if ( nlbl < 0 ) {ret = false; continue;}
      if ( ! setNumLabels((size_t)nlbl) ) ret = false;
    } // end of "num_labels"
    else if ( tagName == "num_decimal" || tagName == "precision" ) {
      if ( tagValue.empty() ) {ret = false; continue;}
      int preci = atoi(tagValue.c_str());
      if ( preci < 0 ) {ret = false; continue;}
      setLabelPrecision((size_t)preci);
    } // end of "num_decimal" || "precision"
    else if ( tagName == "position" ) {
      vector2 pos = {0.f, 0.f};
      xmlChar* xs = xmlGetProp(cur, (const xmlChar*)"x");
      xmlChar* ys = xmlGetProp(cur, (const xmlChar*)"y");
      if ( xs ) {pos[0] = (float)atof((const char*)xs); xmlFree(xs);}
      if ( ys ) {pos[1] = (float)atof((const char*)ys); xmlFree(ys);}
      setPosition(pos);
    } // end of "position"
    else if ( tagName == "size" ) {
      vector2 sz = {0.f, 0.f};
      xmlChar* xs = xmlGetProp(cur, (const xmlChar*)"x");
      xmlChar* ys = xmlGetProp(cur, (const xmlChar*)"y");
      if ( xs ) {sz[0] = (float)atof((const char*)xs); xmlFree(xs);}
      if ( ys ) {sz[1] = (float)atof((const char*)ys); xmlFree(ys);}
      if ( ! setSize(sz) ) ret = false;
    } // end of "size"
  } // end of for(cur)

  return ret;
}

bool vsnColorBar::exportXMLNode(std::ostream& os, const bool show,
				const size_t ts) const {
  std::string idts;
  for ( int i = 0; i < ts; i++ ) idts.push_back(' ');
  std::string idts2(idts); idts2 += "  ";

  // output
  os << idts << "<color_bar show=\""
     << (show ? "yes" : "no") << "\">" << std::endl;

  if ( m_direction != CBD_VERTICAL ) {
    os << idts2 << "<direction value=\"horizontal\" />" << std::endl;
  }

  if ( m_showAlpha ) {
    os << idts2 << "<show_alpha value=\"yes\" />" << std::endl;
  }

  if ( ! getTitle().empty() ) {
    os << idts2 << "<title value=\"" << getTitle() << "\" />" << std::endl;
  }

  if ( m_numLbls != 2 ) {
    os << idts2 << "<num_labels value=\"" << m_numLbls
       << "\" />" << std::endl;
  }

  if ( m_precLbls != 2 ) {
    os << idts2 << "<precision value=\"" << m_precLbls
       << "\" />" << std::endl;
  }

  vector2 xy;
  getPosition(xy);
  if ( xy[0] != 0.70f || xy[1] != 0.f ) {
    os << idts2 << "<position x=\"" << xy[0] << "\" y=\"" << xy[1]
       << "\" />" << std::endl;
  }

  getSize(xy);
  if ( xy[0] != 0.20f || xy[1] != 1.f ) {
    os << idts2 << "<size x=\"" << xy[0] << "\" y=\"" << xy[1]
       << "\" />" << std::endl;
  }

  os << idts << "</color_bar>" << std::endl;

  return true;
}


// update method (private)
bool vsnColorBar::updateColorBar() {
  if ( ! m_pBar || ! m_pTitleLbl || ! m_pLbls || ! _material )
    return false;

  // reset
  m_pBar->identity();
  m_pTitleLbl->identity();
  m_pLbls->identity();
  if ( ! p_method ) return true;

  // bar
  register int i;
  register float dv;
  vector3 v = {0.f, 0.f, 0.f};
  if ( ! m_pBar->setMeshSize(LUT_MAX_ENTRY+1, 3) ||
       ! m_pBar->alcColors(LUT_MAX_ENTRY*2) )
    return false;
  if ( m_direction == CBD_HORIZONTAL ) {
    dv = 1.f * m_sz[0] / (float)LUT_MAX_ENTRY;
    for ( i = 0; i <= LUT_MAX_ENTRY; i++ ) {
      v[0] = (float)i * dv;
      v[1] = 0.2f * m_sz[1];
      m_pBar->setVert(i, v, FALSE);
      v[1] = 0.55f * m_sz[1];
      m_pBar->setVert(i + LUT_MAX_ENTRY+1, v, FALSE);
      v[1] = 0.7f * m_sz[1];
      m_pBar->setVert(i + (LUT_MAX_ENTRY+1)*2, v, FALSE);
    } // end of for(i)
  }
  else {
    dv = 0.7f * m_sz[1] / (float)LUT_MAX_ENTRY;
    for ( i = 0; i <= LUT_MAX_ENTRY; i++ ) {
      v[0] = 0.45f * m_sz[0];
      v[1] = (float)i * dv;
      m_pBar->setVert(i, v, FALSE);
      v[0] = 0.1f * m_sz[0];
      m_pBar->setVert(i + LUT_MAX_ENTRY+1, v, FALSE);
      v[0] = 0.0f;
      m_pBar->setVert(i + (LUT_MAX_ENTRY+1)*2, v, FALSE);
    } // end of for(i)
  }
  if ( m_showAlpha )
    for ( i = 0; i < LUT_MAX_ENTRY; i++ ) {
      m_pBar->setColor3(i, &m_lut.lutEntry[i*4]);
      dv = m_lut.lutEntry[i*4+3];
      dv = 2.f*dv - dv*dv;
      v[0] = v[1] = v[2] = dv;
      m_pBar->setColor3(i + LUT_MAX_ENTRY, v);
    } // end of for(i)
  else
    for ( i = 0; i < LUT_MAX_ENTRY; i++ ) {
      m_pBar->setColor3(i, &m_lut.lutEntry[i*4]);
      m_pBar->setColor3(i + LUT_MAX_ENTRY, &m_lut.lutEntry[i*4]);
    } // end of for(i)
  m_pBar->generateBbox();
  m_pBar->setBboxColor(_colors[0]);

  // title
  if ( m_direction == CBD_HORIZONTAL ) {
    m_pTitleLbl->setFontAlign(AL_LEFT);
    m_pTitleLbl->trans(0.f, 0.7f*m_sz[1] + CB_LABEL_SIZE*0.5f, 0.f);
  }
  else {
    m_pTitleLbl->setFontAlign(AL_CENTER);
    m_pTitleLbl->trans(0.45f*m_sz[0], 0.7f*m_sz[1] + CB_LABEL_SIZE*0.5f, 0.f);
  }
  m_pTitleLbl->setColor3(0, _colors[0]);

  // labels
  int n = m_pLbls->getNumChildren();
  if ( m_numLbls > n ) {
    for ( i = n; i < m_numLbls; i++ ) {
      vfrLineText* pltxt = new vfrLineText("labels", TRUE); // cause suicide
      if ( ! pltxt ) return false;
      pltxt->setTransparency(TRUE);
      m_pLbls->addChild(pltxt);
    } // end of for(i)
  }
  else if ( m_numLbls < n ) {
    for ( i = m_numLbls; i < n; i++ ) {
      vfrLineText* pltxt = dynamic_cast<vfrLineText*>(m_pLbls->getChild(i));
      if ( ! pltxt ) continue;
      pltxt->setLetters("");
    } // end of for(i)
  }

  register float dval = 0.f;
  if ( m_numLbls > 1 )
    dval = (m_lut.maxVal - m_lut.minVal) / (float)(m_numLbls -1);
  stringstream fmt; fmt << "% ." << m_precLbls << "f";
  if ( m_direction == CBD_HORIZONTAL ) {
    dv = (m_numLbls > 1) ? (1.f * m_sz[0] / (float)(m_numLbls -1)) : 0.f;
    for ( i = 0; i < m_numLbls; i++ ) {
      vfrLineText* pltxt = dynamic_cast<vfrLineText*>(m_pLbls->getChild(i));
      if ( ! pltxt ) continue;
      pltxt->setFontAlign(AL_CENTER);
      pltxt->setLetters(const_cast<char*>(fmt.str().c_str()),
			m_lut.minVal + dval*i);
      pltxt->identity();
      pltxt->trans((float)i * dv, -CB_LABEL_SIZE*0.2f, 0.f);
      pltxt->setFontScale(CB_LABEL_SIZE);
      pltxt->setColor3(0, _colors[0]);
    } // end of for(i)
  }
  else {
    dv = (m_numLbls > 1) ? (0.7f * m_sz[1] / (float)(m_numLbls -1)) : 0.f;
    for ( i = 0; i < m_numLbls; i++ ) {
      vfrLineText* pltxt = dynamic_cast<vfrLineText*>(m_pLbls->getChild(i));
      if ( ! pltxt ) continue;
      pltxt->setFontAlign(AL_LEFT);
      pltxt->setLetters(const_cast<char*>(fmt.str().c_str()),
			m_lut.minVal + dval*i);
      pltxt->identity();
      pltxt->trans(0.5f * m_sz[0], i*dv - CB_LABEL_SIZE*0.5f, 0.f);
      pltxt->setFontScale(CB_LABEL_SIZE);
      pltxt->setColor3(0, _colors[0]);
    } // end of for(i)
  }

  return true;
}


//----------------------------------------------------------------
// class vsnColorBarDlg
//----------------------------------------------------------------

BEGIN_EVENT_TABLE(vsnColorBarDlg, wxDialog)
  EVT_RADIOBOX(ColorBarDlg_DirRadio, vsnColorBarDlg::OnDirRadio)
  EVT_CHECKBOX(ColorBarDlg_AlphaChk, vsnColorBarDlg::OnAlphaChk)
  EVT_TEXT_ENTER(ColorBarDlg_TitleTxt, vsnColorBarDlg::OnTitleTxt)
  EVT_TEXT_ENTER(ColorBarDlg_NumLblsTxt, vsnColorBarDlg::OnNumLblsTxt)
  EVT_TEXT_ENTER(ColorBarDlg_NumDeciTxt, vsnColorBarDlg::OnNumDeciTxt)
  EVT_TEXT_ENTER(ColorBarDlg_PosXTxt, vsnColorBarDlg::OnChangePos)
  EVT_TEXT_ENTER(ColorBarDlg_PosYTxt, vsnColorBarDlg::OnChangePos)
  EVT_TEXT_ENTER(ColorBarDlg_SizeXTxt, vsnColorBarDlg::OnChangeSize)
  EVT_TEXT_ENTER(ColorBarDlg_SizeYTxt, vsnColorBarDlg::OnChangeSize)
  EVT_BUTTON(ColorBarDlg_CloseBtn, vsnColorBarDlg::OnCloseBtn)
END_EVENT_TABLE()


/* constructors / destructor */

vsnColorBarDlg::vsnColorBarDlg(wxWindow *parent, vsnColorBar* pcb)
  : wxDialog(parent, -1, wxString(wxT("ColorBar Layout Dlg"))), p_colorBar(pcb)
{
  assert(parent);

  wxBoxSizer *sizerTop = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer* sizerH;

  // direction / alpha
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  sizerTop->Add(sizerH, 0, wxEXPAND|(wxALL & ~wxTOP), 0);
  wxString ritems[] = {wxString(wxT("vertical")), wxString(wxT("horizontal"))};
  m_pDirection = new wxRadioBox(this, ColorBarDlg_DirRadio, wxT("direction"),
				wxDefaultPosition, wxDefaultSize, 2, ritems,
				2, wxRA_SPECIFY_COLS);
  sizerH->Add(m_pDirection, 0, wxEXPAND|wxALL, 3);

  m_pAlphaChk = new wxCheckBox(this, ColorBarDlg_AlphaChk, wxT("show alpha"));
  sizerH->Add(m_pAlphaChk, 0, wxEXPAND|wxALL, 3);

  // title
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  sizerTop->Add(sizerH, 0, wxEXPAND|(wxALL & ~wxTOP), 0);
  sizerH->Add(new wxStaticText(this, -1, wxT("title label")), 0,
	      wxALIGN_LEFT|wxALL, 3);
  m_pTitle = new wxTextCtrl(this, ColorBarDlg_TitleTxt, wxT(""),
			    wxDefaultPosition, wxDefaultSize,
			    wxTE_PROCESS_ENTER);
  sizerH->Add(m_pTitle, 1, wxEXPAND|wxALL, 3);

  // num labels / num decimals
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  sizerTop->Add(sizerH, 0, wxEXPAND|(wxALL & ~wxTOP), 0);

  sizerH->Add(new wxStaticText(this, -1, wxT("#of labels")), 0,
	      wxALIGN_LEFT|wxALL, 3);
  m_pNumLbls = new wxTextCtrl(this, ColorBarDlg_NumLblsTxt, wxT("2"),
			    wxDefaultPosition, wxDefaultSize,
			    wxTE_PROCESS_ENTER);
  sizerH->Add(m_pNumLbls, 1, wxEXPAND|wxALL, 3);
  sizerH->Add(10, 0);
  sizerH->Add(new wxStaticText(this, -1, wxT("#of decimals")), 0,
	      wxALIGN_LEFT|wxALL, 3);
  m_pNumDeci = new wxTextCtrl(this, ColorBarDlg_NumDeciTxt, wxT("2"),
			    wxDefaultPosition, wxDefaultSize,
			    wxTE_PROCESS_ENTER);
  sizerH->Add(m_pNumDeci, 1, wxEXPAND|wxALL, 3);

  // position
  sizerTop->Add(new wxStaticText(this, -1, wxT("position")), 0,
		wxALIGN_LEFT|wxALL, 3);

  sizerH = new wxBoxSizer(wxHORIZONTAL);
  sizerTop->Add(sizerH, 0, wxEXPAND|(wxALL & ~wxTOP), 0);

  sizerH->Add(new wxStaticText(this, -1, wxT("X")), 0,
	      wxALIGN_LEFT|wxALL, 3);
  m_pPosX = new wxTextCtrl(this, ColorBarDlg_PosXTxt, wxT("0.0"),
			   wxDefaultPosition, wxDefaultSize,
			   wxTE_PROCESS_ENTER);
  sizerH->Add(m_pPosX, 1, wxEXPAND|wxALL, 3);
  sizerH->Add(10, 0);
  sizerH->Add(new wxStaticText(this, -1, wxT("Y")), 0,
	      wxALIGN_LEFT|wxALL, 3);
  m_pPosY = new wxTextCtrl(this, ColorBarDlg_PosYTxt, wxT("0.0"),
			   wxDefaultPosition, wxDefaultSize,
			   wxTE_PROCESS_ENTER);
  sizerH->Add(m_pPosY, 1, wxEXPAND|wxALL, 3);

  // size
  sizerTop->Add(new wxStaticText(this, -1, wxT("size")), 0,
		wxALIGN_LEFT|wxALL, 3);

  sizerH = new wxBoxSizer(wxHORIZONTAL);
  sizerTop->Add(sizerH, 0, wxEXPAND|(wxALL & ~wxTOP), 0);

  sizerH->Add(new wxStaticText(this, -1, wxT("X")), 0,
	      wxALIGN_LEFT|wxALL, 3);
  m_pSizeX = new wxTextCtrl(this, ColorBarDlg_SizeXTxt, wxT("1.0"),
			   wxDefaultPosition, wxDefaultSize,
			   wxTE_PROCESS_ENTER);
  sizerH->Add(m_pSizeX, 1, wxEXPAND|wxALL, 3);
  sizerH->Add(10, 0);
  sizerH->Add(new wxStaticText(this, -1, wxT("Y")), 0,
	      wxALIGN_LEFT|wxALL, 3);
  m_pSizeY = new wxTextCtrl(this, ColorBarDlg_SizeYTxt, wxT("1.0"),
			   wxDefaultPosition, wxDefaultSize,
			   wxTE_PROCESS_ENTER);
  sizerH->Add(m_pSizeY, 1, wxEXPAND|wxALL, 3);

  // close
  sizerTop->Add(0, 5, 0, wxGROW);
  sizerTop->Add(
     new wxStaticLine(this, -1, wxDefaultPosition, wxSize(3,3), wxHORIZONTAL),
     0, wxEXPAND|wxALL, 0);

  sizerH = new wxBoxSizer(wxHORIZONTAL);
  sizerTop->Add(sizerH, 0, wxALIGN_RIGHT|wxALL, 5);
  m_pClose = new wxButton(this, ColorBarDlg_CloseBtn, wxT("close"));
  sizerH->Add(m_pClose);

  /* post process */
  SetAutoLayout(TRUE);
  SetSizer(sizerTop);
  sizerTop->SetSizeHints(this);
  sizerTop->Fit(this);

  /* initial update */
  update();
}

vsnColorBarDlg::~vsnColorBarDlg() {
}


/* interface */

// copy values from p_colorBar to this
bool vsnColorBarDlg::update() {
  if ( ! m_pDirection || ! m_pAlphaChk ||
       ! m_pTitle || ! m_pNumLbls || ! m_pNumDeci ||
       ! m_pPosX || ! m_pPosY || ! m_pSizeX || ! m_pSizeY ) return false;
  if ( ! p_colorBar ) return false;

  // direction
  vsnColorBar::CBDirType dt = p_colorBar->getDirection();
  m_pDirection->SetSelection((int)dt);

  // alpha
  m_pAlphaChk->SetValue(p_colorBar->getShowAlpha());

  // title
  string valstr = p_colorBar->getTitle();
  m_pTitle->SetValue(vsnApp::ConvSysToWx(valstr));

  // num labels
  size_t val = p_colorBar->getNumLabels();
  char txt[64]; sprintf(txt, "%lu", val);
  m_pNumLbls->SetValue(vsnApp::ConvSysToWx(txt));

  // num decimal
  val = p_colorBar->getLabelPrecision();
  sprintf(txt, "%lu", val);
  m_pNumDeci->SetValue(vsnApp::ConvSysToWx(txt));

  // position
  vector2 v;
  p_colorBar->getPosition(v);
  sprintf(txt, "%g", v[0]); m_pPosX->SetValue(vsnApp::ConvSysToWx(txt));
  sprintf(txt, "%g", v[1]); m_pPosY->SetValue(vsnApp::ConvSysToWx(txt));

  // size
  p_colorBar->getSize(v);
  sprintf(txt, "%g", v[0]); m_pSizeX->SetValue(vsnApp::ConvSysToWx(txt));
  sprintf(txt, "%g", v[1]); m_pSizeY->SetValue(vsnApp::ConvSysToWx(txt));

  return true;
}

void vsnColorBarDlg::setColorBar(vsnColorBar* pcb) {
  if ( p_colorBar == pcb ) return;
  p_colorBar = pcb;

  (void)update();
}


/* callbacks */

void vsnColorBarDlg::OnDirRadio(wxCommandEvent& event) {
  if ( ! m_pDirection || ! m_pSizeX || ! m_pSizeY ) return;
  if ( ! p_colorBar ) return;

  vsnColorBar::CBDirType val
    = (vsnColorBar::CBDirType)m_pDirection->GetSelection();
  if ( val == p_colorBar->getDirection() ) return;

  p_colorBar->setDirection(val);

  // adjust size
  vector2 s; p_colorBar->getSize(s);
  float x = s[0]; s[0] = s[1]; s[1] = x;
  if ( p_colorBar->setSize(s) ) {
    char txt[64];
    sprintf(txt, "%g", s[0]); m_pSizeX->SetValue(vsnApp::ConvSysToWx(txt));
    sprintf(txt, "%g", s[1]); m_pSizeY->SetValue(vsnApp::ConvSysToWx(txt));
  }

  // adjust #of labels
  if ( val == vsnColorBar::CBD_HORIZONTAL && m_pNumLbls ) {
    char txt[64]; sprintf(txt, "%lu", p_colorBar->getNumLabels());
    m_pNumLbls->SetValue(vsnApp::ConvSysToWx(txt));
  }
}

void vsnColorBarDlg::OnAlphaChk(wxCommandEvent& event) {
  if ( ! m_pAlphaChk ) return;
  if ( ! p_colorBar ) return;

  bool val = m_pAlphaChk->GetValue();
  p_colorBar->setShowAlpha(val);
}

void vsnColorBarDlg::OnTitleTxt(wxCommandEvent& event) {
  if ( ! m_pTitle ) return;
  if ( ! p_colorBar ) return;

  string tstr;
  wxString valStr = m_pTitle->GetValue();
  if ( ! valStr.IsEmpty() ) tstr = vsnApp::ConvWxToSys(valStr);
  p_colorBar->setTitle(tstr);
}

void vsnColorBarDlg::OnNumLblsTxt(wxCommandEvent& event) {
  if ( ! m_pNumLbls ) return;
  if ( ! p_colorBar ) return;

  wxString valStr = m_pNumLbls->GetValue();
  int val = atoi(vsnApp::ConvWxToSys(valStr).c_str());
  if ( val < 0 || ! p_colorBar->setNumLabels((size_t)val) ) {
    char txt[64]; sprintf(txt, "%lu", p_colorBar->getNumLabels());
    m_pNumLbls->SetValue(vsnApp::ConvSysToWx(txt));
  }
}

void vsnColorBarDlg::OnNumDeciTxt(wxCommandEvent& event) {
  if ( ! m_pNumDeci ) return;
  if ( ! p_colorBar ) return;

  wxString valStr = m_pNumDeci->GetValue();
  int val = atoi(vsnApp::ConvWxToSys(valStr).c_str());
  if ( val < 0 ) {
    char txt[64]; sprintf(txt, "%lu", p_colorBar->getLabelPrecision());
    m_pNumDeci->SetValue(vsnApp::ConvSysToWx(txt));
    return;
  }

  p_colorBar->setLabelPrecision((size_t)val);
}

void vsnColorBarDlg::OnChangePos(wxCommandEvent& event) {
  if ( ! m_pPosX || ! m_pPosY ) return;
  if ( ! p_colorBar ) return;

  vector2 p, p0;
  wxString valStr = m_pPosX->GetValue();
  p[0] = (float)atof(vsnApp::ConvWxToSys(valStr).c_str());
  valStr = m_pPosY->GetValue();
  p[1] = (float)atof(vsnApp::ConvWxToSys(valStr).c_str());
  p_colorBar->getPosition(p0);
  if ( p[0] == p0[0] && p[1] == p0[1] ) return;

  p_colorBar->setPosition(p);
}

void vsnColorBarDlg::OnChangeSize(wxCommandEvent& event) {
  if ( ! m_pSizeX || ! m_pSizeY ) return;
  if ( ! p_colorBar ) return;

  vector2 s, s0;
  wxString valStr = m_pSizeX->GetValue();
  s[0] = (float)atof(vsnApp::ConvWxToSys(valStr).c_str());
  valStr = m_pSizeY->GetValue();
  s[1] = (float)atof(vsnApp::ConvWxToSys(valStr).c_str());
  p_colorBar->getSize(s0);
  if ( s[0] == s0[0] && s[1] == s0[1] ) return;

  if ( ! p_colorBar->setSize(s) ) {
    char txt[64];
    sprintf(txt, "%g", s0[0]); m_pSizeX->SetValue(vsnApp::ConvSysToWx(txt));
    sprintf(txt, "%g", s0[1]); m_pSizeY->SetValue(vsnApp::ConvSysToWx(txt));
  }
}

void vsnColorBarDlg::OnCloseBtn(wxCommandEvent& event) {
  if ( IsModal() )
    EndModal(vsn_wxIDOK);
  else
    this->Hide();
}

void vsnColorBarDlg::OnClose(wxCloseEvent& event) {
  if ( p_colorBar )
    p_colorBar->chkNotice();
}
