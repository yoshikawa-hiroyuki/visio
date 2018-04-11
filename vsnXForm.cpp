//
// vsnXForm
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

#include "vsnApp.h"
#include "vsnXForm.h"

using namespace CES;
using namespace VSN;


//----------------------------------------------------------------
// class vsnXForm
//----------------------------------------------------------------

/* XML util */

bool vsnXForm::importXMLNode(xmlNodePtr xnp) {
  if ( ! xnp ) return false;

  // is 'xform' node?
  if ( strcmp((const char*)xnp->name, "xform") ) return false;
  
  // get children node
  xmlNodePtr cur = xnp->xmlChildrenNode;

  // traverse
  while ( cur ) {
    if ( cur->type == XML_COMMENT_NODE ) {
      cur = cur->next;
      continue;
    }
    else if ( !strcmp((const char*)cur->name, "translate") ) {
      Vec3<float> val;
      xmlChar* xs = xmlGetProp(cur, (const xmlChar*)"x");
      xmlChar* ys = xmlGetProp(cur, (const xmlChar*)"y");
      xmlChar* zs = xmlGetProp(cur, (const xmlChar*)"z");
      if ( xs ) {val[0] = (float)atof((const char*)xs); xmlFree(xs);}
      if ( ys ) {val[1] = (float)atof((const char*)ys); xmlFree(ys);}
      if ( zs ) {val[2] = (float)atof((const char*)zs); xmlFree(zs);}
      m_T = val;
    } // end of "translate"
    else if ( !strcmp((const char*)cur->name, "center") ) {
      Vec3<float> val;
      xmlChar* xs = xmlGetProp(cur, (const xmlChar*)"x");
      xmlChar* ys = xmlGetProp(cur, (const xmlChar*)"y");
      xmlChar* zs = xmlGetProp(cur, (const xmlChar*)"z");
      if ( xs ) {val[0] = (float)atof((const char*)xs); xmlFree(xs);}
      if ( ys ) {val[1] = (float)atof((const char*)ys); xmlFree(ys);}
      if ( zs ) {val[2] = (float)atof((const char*)zs); xmlFree(zs);}
      m_C = val;
    } // end of "center"
    else if ( !strcmp((const char*)cur->name, "scale") ) {
      Vec3<float> val(1.f, 1.f, 1.f);
      xmlChar* xs = xmlGetProp(cur, (const xmlChar*)"x");
      xmlChar* ys = xmlGetProp(cur, (const xmlChar*)"y");
      xmlChar* zs = xmlGetProp(cur, (const xmlChar*)"z");
      if ( xs ) {val[0] = (float)atof((const char*)xs); xmlFree(xs);}
      if ( ys ) {val[1] = (float)atof((const char*)ys); xmlFree(ys);}
      if ( zs ) {val[2] = (float)atof((const char*)zs); xmlFree(zs);}
      m_S = val;
    } // end of "scale"
    else if ( !strcmp((const char*)cur->name, "hpr") ) {
      Vec3<float> val;
      xmlChar* hs = xmlGetProp(cur, (const xmlChar*)"h");
      xmlChar* ps = xmlGetProp(cur, (const xmlChar*)"p");
      xmlChar* rs = xmlGetProp(cur, (const xmlChar*)"r");
      if ( hs ) {val[0] = (float)atof((const char*)hs); xmlFree(hs);}
      if ( ps ) {val[1] = (float)atof((const char*)ps); xmlFree(ps);}
      if ( rs ) {val[2] = (float)atof((const char*)rs); xmlFree(rs);}
      m_HPR = val;
    } // end of "hpr"
    else if ( !strcmp((const char*)cur->name, "rotate") ) {
      Vec3<float> val;
      xmlChar* xs = xmlGetProp(cur, (const xmlChar*)"x");
      xmlChar* ys = xmlGetProp(cur, (const xmlChar*)"y");
      xmlChar* zs = xmlGetProp(cur, (const xmlChar*)"z");
      if ( xs ) {val[1] = (float)atof((const char*)xs); xmlFree(xs);}
      if ( ys ) {val[0] = (float)atof((const char*)ys); xmlFree(ys);}
      if ( zs ) {val[2] = (float)atof((const char*)zs); xmlFree(zs);}
      m_HPR = val;
    } // end of "rotate"

    cur = cur->next;
  } // end of while(cur)

  updateXForm();
  return true;
}

bool vsnXForm::exportXMLNode(std::ostream& os, const size_t ts) const {
  std::string idts;
  for ( int i = 0; i < ts; i++ ) idts.push_back(' ');
  std::string idts2(idts); idts2 += "  ";

  // output
  os << idts << "<xform>" << std::endl;

  //if ( m_T[0] != 0.f || m_T[1] != 0.f || m_T[2] != 0.f )
    os << idts2 << "<translate x=\"" << m_T[0] << "\" y=\""
       << m_T[1] << "\" z=\"" << m_T[2] << "\" />" << std::endl;

  //if ( m_HPR[0] != 0.f || m_HPR[1] != 0.f || m_HPR[2] != 0.f )
    os << idts2 << "<hpr h=\"" << m_HPR[0] << "\" p=\""
       << m_HPR[1] << "\" r=\"" << m_HPR[2] << "\" />" << std::endl;

  //if ( m_S[0] != 1.f || m_S[1] != 1.f || m_S[2] != 1.f )
    os << idts2 << "<scale x=\"" << m_S[0] << "\" y=\""
       << m_S[1] << "\" z=\"" << m_S[2] << "\" />" << std::endl;

  //if ( m_C[0] != 0.f || m_C[1] != 0.f || m_C[2] != 0.f )
    os << idts2 << "<center x=\"" << m_C[0] << "\" y=\""
       << m_C[1] << "\" z=\"" << m_C[2] << "\" />" << std::endl;

  os << idts << "</xform>" << std::endl;

  return true;
}


//----------------------------------------------------------------
// class vsnXFormDlg
//----------------------------------------------------------------

BEGIN_EVENT_TABLE(vsnXFormDlg, wxDialog)
  EVT_TEXT_ENTER(XFormDlg_Tx, vsnXFormDlg::OnChangeValues)
  EVT_TEXT_ENTER(XFormDlg_Ty, vsnXFormDlg::OnChangeValues)
  EVT_TEXT_ENTER(XFormDlg_Tz, vsnXFormDlg::OnChangeValues)
  EVT_TEXT_ENTER(XFormDlg_Rx, vsnXFormDlg::OnChangeValues)
  EVT_TEXT_ENTER(XFormDlg_Ry, vsnXFormDlg::OnChangeValues)
  EVT_TEXT_ENTER(XFormDlg_Rz, vsnXFormDlg::OnChangeValues)
  EVT_TEXT_ENTER(XFormDlg_Sx, vsnXFormDlg::OnChangeValues)
  EVT_TEXT_ENTER(XFormDlg_Sy, vsnXFormDlg::OnChangeValues)
  EVT_TEXT_ENTER(XFormDlg_Sz, vsnXFormDlg::OnChangeValues)
  EVT_TEXT_ENTER(XFormDlg_Cx, vsnXFormDlg::OnChangeValues)
  EVT_TEXT_ENTER(XFormDlg_Cy, vsnXFormDlg::OnChangeValues)
  EVT_TEXT_ENTER(XFormDlg_Cz, vsnXFormDlg::OnChangeValues)
  EVT_BUTTON(XFormDlg_Reset, vsnXFormDlg::OnResetBtn)
  EVT_BUTTON(XFormDlg_Close, vsnXFormDlg::OnCloseBtn)
  EVT_CLOSE(vsnXFormDlg::OnClose)
END_EVENT_TABLE()


/* constructors, destructor */

vsnXFormDlg::vsnXFormDlg(wxWindow *parent, vsnXForm* refXForm,
			 const bool useC, const bool useSz)
  : wxDialog(parent, -1, wxString(wxT("xform"))), p_xform(refXForm)
{
  assert(parent);

  wxBoxSizer *sizerTop = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer* sizerH;

  // translation
  sizerTop->Add(new wxStaticText(this, -1, wxT("Translation")),
		0, wxALIGN_LEFT|(wxALL & ~wxBOTTOM), 3);
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  sizerTop->Add(sizerH, 0, wxEXPAND|(wxALL & ~wxTOP), 5);
  sizerH->Add(new wxStaticText(this, -1, wxT("X")), 0, wxALIGN_LEFT|wxALL, 3);
  m_pTx = new wxTextCtrl(this, XFormDlg_Tx, wxT("0.0"),
			 wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
  sizerH->Add(m_pTx, 1, wxEXPAND|wxALL, 3);
  sizerH->Add(new wxStaticText(this, -1, wxT("Y")), 0, wxALIGN_LEFT|wxALL, 3);
  m_pTy = new wxTextCtrl(this, XFormDlg_Ty, wxT("0.0"),
			 wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
  sizerH->Add(m_pTy, 1, wxEXPAND|wxALL, 3);
  sizerH->Add(new wxStaticText(this, -1, wxT("Z")), 0, wxALIGN_LEFT|wxALL, 3);
  m_pTz = new wxTextCtrl(this, XFormDlg_Tz, wxT("0.0"),
			 wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
  sizerH->Add(m_pTz, 1, wxEXPAND|wxALL, 3);

  // rotation
  sizerTop->Add(new wxStaticText(this, -1, wxT("Rotation")),
		0, wxALIGN_LEFT|(wxALL & ~wxBOTTOM), 3);
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  sizerTop->Add(sizerH, 0, wxEXPAND|(wxALL & ~wxTOP), 5);
  sizerH->Add(new wxStaticText(this, -1, wxT("X")), 0, wxALIGN_LEFT|wxALL, 3);
  m_pRx = new wxTextCtrl(this, XFormDlg_Rx, wxT("0.0"),
			 wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
  sizerH->Add(m_pRx, 1, wxEXPAND|wxALL, 3);
  sizerH->Add(new wxStaticText(this, -1, wxT("Y")), 0, wxALIGN_LEFT|wxALL, 3);
  m_pRy = new wxTextCtrl(this, XFormDlg_Ry, wxT("0.0"),
			 wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
  sizerH->Add(m_pRy, 1, wxEXPAND|wxALL, 3);
  sizerH->Add(new wxStaticText(this, -1, wxT("Z")), 0, wxALIGN_LEFT|wxALL, 3);
  m_pRz = new wxTextCtrl(this, XFormDlg_Rz, wxT("0.0"),
			 wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
  sizerH->Add(m_pRz, 1, wxEXPAND|wxALL, 3);

  // scale
  sizerTop->Add(new wxStaticText(this, -1, wxT("Scale")),
		0, wxALIGN_LEFT|(wxALL & ~wxBOTTOM), 3);
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  sizerTop->Add(sizerH, 0, wxEXPAND|(wxALL & ~wxTOP), 5);
  sizerH->Add(new wxStaticText(this, -1, wxT("X")), 0, wxALIGN_LEFT|wxALL, 3);
  m_pSx = new wxTextCtrl(this, XFormDlg_Sx, wxT("1.0"),
			 wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
  sizerH->Add(m_pSx, 1, wxEXPAND|wxALL, 3);
  sizerH->Add(new wxStaticText(this, -1, wxT("Y")), 0, wxALIGN_LEFT|wxALL, 3);
  m_pSy = new wxTextCtrl(this, XFormDlg_Sy, wxT("1.0"),
			 wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
  sizerH->Add(m_pSy, 1, wxEXPAND|wxALL, 3);
  sizerH->Add(new wxStaticText(this, -1, wxT("Z")), 0, wxALIGN_LEFT|wxALL, 3);
  m_pSz = new wxTextCtrl(this, XFormDlg_Sz, wxT("1.0"),
			 wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
  sizerH->Add(m_pSz, 1, wxEXPAND|wxALL, 3);
  if ( ! useSz ) {
    m_pSz->Enable(FALSE);
  }

  // rotation/scale center
  if ( useC ) {
    sizerTop->Add(new wxStaticText(this, -1, wxT("Rot/Scale Center")),
		  0, wxALIGN_LEFT|(wxALL & ~wxBOTTOM), 3);
    sizerH = new wxBoxSizer(wxHORIZONTAL);
    sizerTop->Add(sizerH, 0, wxEXPAND|(wxALL & ~wxTOP), 5);
    sizerH->Add(new wxStaticText(this,-1,wxT("X")), 0, wxALIGN_LEFT|wxALL, 3);
    m_pCx = new wxTextCtrl(this, XFormDlg_Cx, wxT("0.0"),
			   wxDefaultPosition,wxDefaultSize,wxTE_PROCESS_ENTER);
    sizerH->Add(m_pCx, 1, wxEXPAND|wxALL, 3);
    sizerH->Add(new wxStaticText(this,-1,wxT("Y")), 0, wxALIGN_LEFT|wxALL, 3);
    m_pCy = new wxTextCtrl(this, XFormDlg_Cy, wxT("0.0"),
			   wxDefaultPosition,wxDefaultSize,wxTE_PROCESS_ENTER);
    sizerH->Add(m_pCy, 1, wxEXPAND|wxALL, 3);
    sizerH->Add(new wxStaticText(this,-1,wxT("Z")), 0, wxALIGN_LEFT|wxALL, 3);
    m_pCz = new wxTextCtrl(this, XFormDlg_Cz, wxT("0.0"),
			   wxDefaultPosition,wxDefaultSize,wxTE_PROCESS_ENTER);
    sizerH->Add(m_pCz, 1, wxEXPAND|wxALL, 3);
  } else {
    m_pCx = m_pCy = m_pCz = NULL;
  } // end of if(useC)

  sizerTop->Add(0, 5, 0, wxGROW);
  sizerTop->Add(
     new wxStaticLine(this, -1, wxDefaultPosition, wxSize(3,3), wxHORIZONTAL),
     0, wxEXPAND|wxALL, 0);

  // buttons
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  sizerTop->Add(sizerH, 0, wxALIGN_RIGHT|wxALL, 5);
  m_pReset = new wxButton(this, XFormDlg_Reset, wxT("reset"));
  sizerH->Add(m_pReset);
  sizerH->Add(10, 0);
  m_pClose = new wxButton(this, XFormDlg_Close, wxT("close"));
  sizerH->Add(m_pClose);

  // post process
  SetAutoLayout(TRUE);
  SetSizer(sizerTop);
  sizerTop->SetSizeHints(this);
  sizerTop->Fit(this);

  (void)update();
}

vsnXFormDlg::~vsnXFormDlg() {
}


/* interface */

bool vsnXFormDlg::update() {
  if ( ! p_xform ) return false;
  Vec3<float> v;
  char txt[64];

  // T
  v = p_xform->getT();
  sprintf(txt, "%g", v[0]); m_pTx->SetValue(vsnApp::ConvSysToWx(txt));
  sprintf(txt, "%g", v[1]); m_pTy->SetValue(vsnApp::ConvSysToWx(txt));
  sprintf(txt, "%g", v[2]); m_pTz->SetValue(vsnApp::ConvSysToWx(txt));

  // R
  v = p_xform->getHPR();
  sprintf(txt, "%g", v[0]); m_pRy->SetValue(vsnApp::ConvSysToWx(txt)); // H
  sprintf(txt, "%g", v[1]); m_pRx->SetValue(vsnApp::ConvSysToWx(txt)); // P
  sprintf(txt, "%g", v[2]); m_pRz->SetValue(vsnApp::ConvSysToWx(txt)); // R

  // S
  v = p_xform->getS();
  sprintf(txt, "%g", v[0]); m_pSx->SetValue(vsnApp::ConvSysToWx(txt));
  sprintf(txt, "%g", v[1]); m_pSy->SetValue(vsnApp::ConvSysToWx(txt));
  if ( m_pSz->IsEnabled() ) {
    sprintf(txt, "%g", v[2]); m_pSz->SetValue(vsnApp::ConvSysToWx(txt));
  }

  // C
  if ( m_pCx ) {
    v = p_xform->getC();
    sprintf(txt, "%g", v[0]); m_pCx->SetValue(vsnApp::ConvSysToWx(txt));
    sprintf(txt, "%g", v[1]); m_pCy->SetValue(vsnApp::ConvSysToWx(txt));
    sprintf(txt, "%g", v[2]); m_pCz->SetValue(vsnApp::ConvSysToWx(txt));
  }

  return true;
}

void vsnXFormDlg::setRefXForm(vsnXForm* refXForm) {
  if ( p_xform == refXForm ) return;
  p_xform = refXForm;

  (void)update();
}

void vsnXFormDlg::updateRefXForm() {
  if ( ! p_xform ) return;
  Vec3<float> v;
  wxString vstr;

  // T
  vstr = m_pTx->GetValue();
  v[0] = (float)atof(vsnApp::ConvWxToSys(vstr).c_str());
  vstr = m_pTy->GetValue();
  v[1] = (float)atof(vsnApp::ConvWxToSys(vstr).c_str());
  vstr = m_pTz->GetValue();
  v[2] = (float)atof(vsnApp::ConvWxToSys(vstr).c_str());
  p_xform->setT(v, false);

  // R
  vstr = m_pRy->GetValue();
  v[0] = (float)atof(vsnApp::ConvWxToSys(vstr).c_str()); // H
  vstr = m_pRx->GetValue();
  v[1] = (float)atof(vsnApp::ConvWxToSys(vstr).c_str()); // P
  vstr = m_pRz->GetValue();
  v[2] = (float)atof(vsnApp::ConvWxToSys(vstr).c_str()); // R
  p_xform->setHPR(v, false);

  // S
  vstr = m_pSx->GetValue();
  v[0] = (float)atof(vsnApp::ConvWxToSys(vstr).c_str());
  vstr = m_pSy->GetValue();
  v[1] = (float)atof(vsnApp::ConvWxToSys(vstr).c_str());
  if ( m_pSz->IsEnabled() ) {
    vstr = m_pSz->GetValue();
    v[2] = (float)atof(vsnApp::ConvWxToSys(vstr).c_str());
  } else v[2] = 1.f;
  p_xform->setS(v, false);

  // C
  if ( m_pCx ) {
    vstr = m_pCx->GetValue();
    v[0] = (float)atof(vsnApp::ConvWxToSys(vstr).c_str());
    vstr = m_pCy->GetValue();
    v[1] = (float)atof(vsnApp::ConvWxToSys(vstr).c_str());
    vstr = m_pCz->GetValue();
    v[2] = (float)atof(vsnApp::ConvWxToSys(vstr).c_str());
    p_xform->setC(v, false);
  } else {
    v = Vec3<float>(0.f, 0.f, 0.f);
    p_xform->setC(v, false);
  }

  // update p_xform
  p_xform->updateXForm();
}


/* event handler */

void vsnXFormDlg::OnChangeValues(wxCommandEvent& event) {
  updateRefXForm();
}

void vsnXFormDlg::OnResetBtn(wxCommandEvent& event) {
  if ( ! p_xform ) return;
  p_xform->resetXForm();
  update();
}

void vsnXFormDlg::OnCloseBtn(wxCommandEvent& event) {
  if ( IsModal() )
    EndModal(vsn_wxIDOK);
  else
    this->Hide();
}

void vsnXFormDlg::OnClose(wxCloseEvent& event) {
}
