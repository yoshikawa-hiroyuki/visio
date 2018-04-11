//
// vsnMethod_label
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

#include "vsnMethod_label.h"
#include "vsnDataObj.h"
#include "vsnUiView.h"
#include "vsnError.h"


//----------------------------------------------------------------
// class vsnMPP_label
//----------------------------------------------------------------

BEGIN_EVENT_TABLE(vsnMPP_label, wxPanel)
  EVT_BUTTON(MPP_label_XFormBtn, vsnMPP_label::OnXFormBtn)
  EVT_BUTTON(MPP_label_LabelBtn, vsnMPP_label::OnLabelBtn)
END_EVENT_TABLE()


/* constructors / destructor */

vsnMPP_label::vsnMPP_label(wxPanel* parent, vsnMethodObj* pm)
  : vsnMethodPP(parent, pm), m_pXFormDlg(NULL)
{
  assert(parent);
  assert(dynamic_cast<vsnMethod_label*>(pm));

  wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer* sizerH;

  // buttons
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxALL, 0);

  m_pLabelBtn = new wxButton(this, MPP_label_LabelBtn, wxT("update label"));
  sizerH->Add(m_pLabelBtn, 0, wxALIGN_LEFT|wxALL, 3);
  m_pXFormBtn = new wxButton(this, MPP_label_XFormBtn, wxT("xform label"));
  sizerH->Add(m_pXFormBtn, 0, wxALIGN_LEFT|wxALL, 3);

  // string text area
  m_pLabelTxt = new wxTextCtrl(this, MPP_label_LabelTxt, wxT(""),
			       wxDefaultPosition, wxSize(-1, 100),
			       wxTE_MULTILINE);
  topsizer->Add(m_pLabelTxt, 0, wxEXPAND|wxALL, 3);

  // post process
  SetSizer(topsizer);
  addTo(parent);
  topsizer->Layout();
  (void)update();
}

vsnMPP_label::~vsnMPP_label() {
  settlement();
}


/* interface */

bool vsnMPP_label::update() {
  if ( ! m_pXFormBtn || ! m_pLabelTxt ) return false;

  vsnMethod_label* pm = dynamic_cast<vsnMethod_label*>(p_method);
  if ( ! pm ) return false;

  wxString msg;
  string lblStr = pm->getLabelStr();
  if ( lblStr.empty() )
    msg = wxT("");
  else
    msg = vsnApp::ConvSysToWx(lblStr.c_str());
  m_pLabelTxt->SetValue(msg);

  if ( m_pXFormDlg )
    (void)m_pXFormDlg->update();

  return true;
}

void vsnMPP_label::settlement() {
  // hide XFormDlg of ParamPanel
  showXFormDlg(false);
}

void vsnMPP_label::showXFormDlg(const bool show) {
  if ( ! m_pXFormDlg ) {
    if ( ! show ) return;
    m_pXFormDlg
      = new vsnXFormDlg(this, dynamic_cast<vsnMethod_label*>(p_method),
                        false, false);
    if ( ! m_pXFormDlg ) {
      ErrMsg(MsgERR, string("label: can't create XFormDlg"));
      return;
    }
    m_pXFormDlg->SetTitle(wxT("xform label"));
  } // end of if(!m_pXFormDlg)

  m_pXFormDlg->Show(show);
}


/* event handler */

void vsnMPP_label::OnXFormBtn(wxCommandEvent& event) {
  showXFormDlg();
}

void vsnMPP_label::OnLabelBtn(wxCommandEvent& event) {
  if ( ! m_pLabelTxt ) return;
  vsnMethod_label* pm = dynamic_cast<vsnMethod_label*>(p_method);
  if ( ! pm ) return;

  wxString msg = m_pLabelTxt->GetValue();
  std::string lblStr = vsnApp::ConvWxToSys(msg);
  pm->setLabelStr(lblStr);
  pm->chkNotice();
}


//----------------------------------------------------------------
// class vsnMethod_label
//----------------------------------------------------------------

/* constructors / destructor */

vsnMethod_label::vsnMethod_label(const string& name)
  : vsnMethodObj(name), m_pLabel(NULL),
    m_initS(1.f,1.f,1.f), m_xformInited(false)
{
}

vsnMethod_label::~vsnMethod_label() {
  if ( m_pLabel )
    delete m_pLabel;
}


/* methods */

void vsnMethod_label::setLabelStr(const std::string& str) {
  if ( m_labelStr == str ) return;
  m_labelStr = str;

  string::size_type sidx = m_labelStr.find(string("\\_"));
  while ( sidx != string::npos ) {
    m_labelStr.replace(sidx, 2, string(" "));
    sidx = m_labelStr.find(string("\\_"));
  }

  sidx = m_labelStr.find(string("\\n"));
  while ( sidx != string::npos ) {
    m_labelStr.replace(sidx, 2, string("\n"));
    sidx = m_labelStr.find(string("\\n"));
  }

  if ( update() )
    chkNotice();
}

std::string vsnMethod_label::getLabelStrOut() const {
  std::string retStr;
  if ( m_labelStr.empty() ) return retStr;
  retStr = m_labelStr;

  string::size_type sidx = retStr.find(string("\n"));
  while ( sidx != string::npos ) {
    retStr.replace(sidx, 1, string("\\n"));
    sidx = retStr.find(string("\n"));
  }

  return retStr;
}


/* vsnXForm methods */

void vsnMethod_label::updateXForm() {
  m_matXForm = getXFormMatrix();
  if ( update() )
    chkNotice();
}

void vsnMethod_label::resetXForm() {
  m_T = m_initT;
  m_S = m_initS;
  m_HPR = m_initHPR;
  updateXForm(); 
}


/* vsnMethodObj methods */

bool vsnMethod_label::update(const bool force) {
  // xform initialize
  if ( ! m_xformInited ) {
    if ( p_refData ) {
      const CES::Vec3<float>* dbb = p_refData->getBbox();
      m_initT = (dbb[1] + dbb[0]) * 0.5f;
      CES::Vec3<float> bbLen = dbb[1] - dbb[0];
      m_initS[0] = m_initS[1] = m_initS[2] = bbLen.Length() * 0.1f;
      if ( m_initS[0] < 1e-6f )
        m_initS[0] = m_initS[1] = m_initS[2] = 1e-6f;
      m_initHPR = CES::Vec3<float>(0.f,0.f,0.f);

      m_T = m_initT; m_S = m_initS; m_HPR = m_initHPR;
      // don't call updateXForm() here, that will call update() again...
      m_matXForm = getXFormMatrix();
      m_xformInited = true;
    }
  } // end of if(!m_xformInited)

  // prepare pLabel
  if ( ! m_pLabel ) {
    m_pLabel = new vfrHelveticaText();
    if ( ! m_pLabel ) {
      ErrMsg(MsgERR, getMethodType() + string("[") + getName()
             + string("]: memory allocation failed"));
      return false;
    }
    m_pLabel->alcMaterial();

    addChild(m_pLabel);
  } // end of if(!m_pLabel)

  m_pLabel->setMatrix(m_matXForm);
  m_pLabel->setLetters(m_labelStr);
  if ( ! m_show )
    m_pLabel->getPrivateMaterial()->setRenderMode(RT_NONE);
  else
    m_pLabel->getPrivateMaterial()->setRenderMode(RT_SMOOTH);

  updateUI();
  return true;
}

vsnMethodPP* vsnMethod_label::getParamPanel(wxPanel* pp) {
  vsnMPP_label* pp_label = new vsnMPP_label(pp, this);
  if ( ! pp_label ) return NULL;
  return pp_label;
}


/* vsnIoObject methods */

bool vsnMethod_label::parseXML(xmlNodePtr xnp) {
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

      if ( xsN == string("label_str") ) {
	setLabelStr(xsV);
      } // end of "label_str"

      if ( xsV.empty() ) {
        ErrMsg(MsgERR, msgHdr +string("no value in param ") +xsN);
        goto _NEXT_XML_NODE;
      }
    } // end of param
    else if ( !strcmp((const char*)cur->name, "xform") ) {
      if ( ! importXMLNode(cur) ) {
        ErrMsg(MsgERR, msgHdr + string("xform node parse failed, ignore"));
        goto _NEXT_XML_NODE;
      }
      m_initT = m_T; m_initS = m_S; m_initHPR = m_HPR;
      m_xformInited = true;
    } // end of "xform"

  _NEXT_XML_NODE:
    cur = cur->next;
  } // end of while(cur)

  return true;
}

bool vsnMethod_label::outputXML(std::ostream& os, const size_t ts) {
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
  if ( ! vsnMethodObj::exportXMLNode(os, ts +2) ) {
    ErrMsg(MsgERR, msgHdr + string("base MethodObj exportXMLNode failed"));
    ret = false;
  }

  // output original params
  // xform
  if ( ! vsnXForm::exportXMLNode(os, ts +2) ) {
    ErrMsg(MsgERR, msgHdr + string("XForm exportXMLNode failed"));
    ret = false;
  }

  // label_str
  if ( ! m_labelStr.empty() ) {
    os << idts_2 << "<param name=\"label_str\" value=\""
       << ConvXmlEntChars(getLabelStrOut()) << "\" />" << endl;
  }

  os << idts << "</method>" << endl;

  return true;
}

bool vsnMethod_label::commandXML(xmlNodePtr xnp) {
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
  if ( nameStr == "set_xform" ) {
    xmlNodePtr cur = xnp->xmlChildrenNode;
    while ( cur ) {
      if ( (cur = vsnIoObject::SkipCommentXML(cur)) == NULL ) break;
      if ( !strcmp((const char*)cur->name, "xform") ) {
        if ( ! importXMLNode(cur) )
          ErrMsg(MsgWARN, msgHdr +
                 string("command set_xform: xform node parse failed, ignore"));
        break;
      }
      cur = cur->next;
    } // end of while(cur)
  } // end of "set_xform"

  if ( nameStr == "set_label_str" ) {
    setLabelStr(valueStr);
  } // end of "set_label_str"

  return true;
}


