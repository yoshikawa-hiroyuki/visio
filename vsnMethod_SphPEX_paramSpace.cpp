//
// vsnMethod_SphPEX_paramSpace
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

#include "vsnMethod_SphPEX_paramSpace.h"
#include "vsnError.h"

using namespace std;
using namespace CES;
using namespace VSN;


//----------------------------------------------------------------
// class vsnMPP_SphPEX_paramSpace
//----------------------------------------------------------------

BEGIN_EVENT_TABLE(vsnMPP_SphPEX_paramSpace, wxPanel)
  EVT_COMMAND_SCROLL(MPP_SphPEX_paramSpace_IndexSld,
                     vsnMPP_SphPEX_paramSpace::OnIndexSlider)
  EVT_TEXT_ENTER(MPP_SphPEX_paramSpace_IndexTxt,
                 vsnMPP_SphPEX_paramSpace::OnIndexTxt)
  EVT_BUTTON(MPP_SphPEX_paramSpace_LeftBtn,
             vsnMPP_SphPEX_paramSpace::OnLeftBtn)
  EVT_BUTTON(MPP_SphPEX_paramSpace_RightBtn,
             vsnMPP_SphPEX_paramSpace::OnRightBtn)
  EVT_CHECKBOX(MPP_SphPEX_paramSpace_ShowLabelChk,
	       vsnMPP_SphPEX_paramSpace::OnShowLabelChk)
  EVT_TEXT_ENTER(MPP_SphPEX_paramSpace_LabelFmtTxt,
		 vsnMPP_SphPEX_paramSpace::OnLabelTxt)
  EVT_TEXT_ENTER(MPP_SphPEX_paramSpace_LabelPosXTxt,
		 vsnMPP_SphPEX_paramSpace::OnLabelTxt)
  EVT_TEXT_ENTER(MPP_SphPEX_paramSpace_LabelPosYTxt,
		 vsnMPP_SphPEX_paramSpace::OnLabelTxt)
  EVT_TEXT_ENTER(MPP_SphPEX_paramSpace_LabelSizeTxt,
		 vsnMPP_SphPEX_paramSpace::OnLabelTxt)
END_EVENT_TABLE()


/* constructors / destructor */

vsnMPP_SphPEX_paramSpace::vsnMPP_SphPEX_paramSpace(wxPanel* parent,
						   vsnMethodObj* pm)
  : vsnMethodPP(parent, pm),
    m_pIndexSlider(NULL), m_pIndexTxt(NULL),
    m_pLeftBtn(NULL), m_pRightBtn(NULL),
    m_pShowLabelChk(NULL), m_pLabelFmtTxt(NULL),
    m_pLabelPosXTxt(NULL), m_pLabelPosYTxt(NULL), m_pLabelSizeTxt(NULL)
{
  assert(parent);
  vsnMethod_SphPEX_paramSpace* ppsm
    = dynamic_cast<vsnMethod_SphPEX_paramSpace*>(pm);
  assert(ppsm);

  // widgets layout
  wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer* sizerH;

  // index
  topsizer->Add(new wxStaticText(this, -1, wxT("parameter space index")),
                0, wxALIGN_LEFT|wxALL, 3);
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxALL, 0);

  m_pIndexSlider = new wxSlider(this, MPP_SphPEX_paramSpace_IndexSld,
                                20000 /*val*/, 10000 /*min*/, 30000 /*max*/,
                                wxDefaultPosition, wxSize(120, -1),
                                wxSL_HORIZONTAL|wxSL_LABELS);
  sizerH->Add(m_pIndexSlider, 0, wxALIGN_RIGHT|wxALL, 3);
  m_pIndexTxt = new wxTextCtrl(this, MPP_SphPEX_paramSpace_IndexTxt, wxT(""),
                               wxDefaultPosition, wxDefaultSize,
                               wxTE_PROCESS_ENTER);
  sizerH->Add(m_pIndexTxt, 0, wxEXPAND|wxALL, 3);

  // index adjust buttons
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxALL, 0);
  m_pLeftBtn = new wxButton(this, MPP_SphPEX_paramSpace_LeftBtn, wxT("<"),
                            wxDefaultPosition, wxSize(60,-1), wxBU_EXACTFIT);
  sizerH->Add(m_pLeftBtn, 1, wxEXPAND|wxALL, 3);
  m_pRightBtn = new wxButton(this, MPP_SphPEX_paramSpace_RightBtn, wxT(">"),
                             wxDefaultPosition, wxSize(60,-1), wxBU_EXACTFIT);
  sizerH->Add(m_pRightBtn, 1, wxEXPAND|wxALL, 3);

  // label
  topsizer->Add(
     new wxStaticLine(this, -1, wxDefaultPosition, wxSize(3,3), wxHORIZONTAL),
     0, wxEXPAND|wxALL, 0);

  m_pShowLabelChk = new wxCheckBox(this, MPP_SphPEX_paramSpace_ShowLabelChk,
                                   wxT("show text label"));
  assert(m_pShowLabelChk);
  topsizer->Add(m_pShowLabelChk, 0, wxALL, 3);

  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND|wxALL, 0);
  sizerH->Add(new wxStaticText(this, -1, wxT("format")),
              0, wxALIGN_LEFT|wxALL, 3);
  m_pLabelFmtTxt = new wxTextCtrl(this, MPP_SphPEX_paramSpace_LabelFmtTxt,
				  wxT(""), wxDefaultPosition, wxDefaultSize,
                                  wxTE_PROCESS_ENTER);
  assert(m_pLabelFmtTxt);
  sizerH->Add(m_pLabelFmtTxt, 1, wxEXPAND|wxALL, 3);

  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxALL, 0);
  sizerH->Add(new wxStaticText(this, -1, wxT("pos X")), 0,
              wxALIGN_LEFT|wxALL, 3);
  m_pLabelPosXTxt = new wxTextCtrl(this, MPP_SphPEX_paramSpace_LabelPosXTxt,
				   wxT(""), wxDefaultPosition, wxDefaultSize,
                                   wxTE_PROCESS_ENTER);
  assert(m_pLabelPosXTxt);
  sizerH->Add(m_pLabelPosXTxt, 0, wxEXPAND|wxALL, 3);
  sizerH->Add(new wxStaticText(this, -1, wxT("Y")), 0, wxALIGN_LEFT|wxALL, 3);
  m_pLabelPosYTxt = new wxTextCtrl(this, MPP_SphPEX_paramSpace_LabelPosYTxt,
				   wxT(""), wxDefaultPosition, wxDefaultSize,
                                   wxTE_PROCESS_ENTER);
  assert(m_pLabelPosYTxt);
  sizerH->Add(m_pLabelPosYTxt, 0, wxEXPAND|wxALL, 3);

  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxALL, 0);
  sizerH->Add(new wxStaticText(this, -1, wxT("size")),
              0, wxALIGN_LEFT|wxALL, 3);
  m_pLabelSizeTxt = new wxTextCtrl(this, MPP_SphPEX_paramSpace_LabelSizeTxt,
				   wxT(""), wxDefaultPosition, wxDefaultSize,
                                   wxTE_PROCESS_ENTER);
  assert(m_pLabelSizeTxt);
  sizerH->Add(m_pLabelSizeTxt, 0, wxEXPAND|wxALL, 3);

  // post process
  SetSizer(topsizer);
  addTo(parent);
  topsizer->Layout();
  (void)update();
}

vsnMPP_SphPEX_paramSpace::~vsnMPP_SphPEX_paramSpace() {
}


/* interface */

bool vsnMPP_SphPEX_paramSpace::update() {
  if ( ! m_pIndexSlider || ! m_pIndexTxt ||
       ! m_pShowLabelChk || ! m_pLabelFmtTxt ||
       ! m_pLabelPosXTxt || ! m_pLabelPosYTxt || ! m_pLabelSizeTxt )
    return false;
  int val; char txt[64];

  vsnMethod_SphPEX_paramSpace* pm
    = dynamic_cast<vsnMethod_SphPEX_paramSpace*>(p_method);
  if ( ! pm ) return false;
  vsnData_SphPEX* pdata = dynamic_cast<vsnData_SphPEX*>(pm->getRefData());
  if ( ! pdata ) return false;

  val = (int)pm->getNumParamIdx();
  m_pIndexSlider->SetRange(0, val -1);

  val = (int)pm->getCurrentParamIdx();
  m_pIndexSlider->SetValue(val);
  sprintf(txt, "%d", val);
  m_pIndexTxt->SetValue(vsnApp::ConvSysToWx(txt));

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

  return true;
}


/* event handler */

void vsnMPP_SphPEX_paramSpace::OnIndexSlider(wxScrollEvent& event) {
  if ( ! m_pIndexSlider || ! m_pIndexTxt ) return;
  vsnMethod_SphPEX_paramSpace* pm
    = dynamic_cast<vsnMethod_SphPEX_paramSpace*>(p_method);
  if ( ! pm ) return;

  int val = m_pIndexSlider->GetValue();
  size_t oidx = pm->getCurrentParamIdx();
  if ( (int)oidx == val ) return;

  if ( pm->setCurrentParamIdx(val) )
    pm->chkNotice();
}

void vsnMPP_SphPEX_paramSpace::OnIndexTxt(wxCommandEvent& event) {
  if ( ! m_pIndexSlider || ! m_pIndexTxt ) return;
  vsnMethod_SphPEX_paramSpace* pm
    = dynamic_cast<vsnMethod_SphPEX_paramSpace*>(p_method);
  if ( ! pm ) return;

  wxString valStr = m_pIndexTxt->GetValue();
  if ( valStr.IsEmpty() ) {update(); return;}
  int val = atoi(vsnApp::ConvWxToSys(valStr).c_str());
  if ( val < 0 || val > m_pIndexSlider->GetMax() )
    return;

  if ( pm->setCurrentParamIdx(val) )
    pm->chkNotice();
}

void vsnMPP_SphPEX_paramSpace::OnLeftBtn(wxCommandEvent& event) {
  vsnMethod_SphPEX_paramSpace* pm
    = dynamic_cast<vsnMethod_SphPEX_paramSpace*>(p_method);
  if ( ! pm ) return;

  int oidx = pm->getCurrentParamIdx();
  if ( pm->setCurrentParamIdx(oidx -1) )
    pm->chkNotice();
}

void vsnMPP_SphPEX_paramSpace::OnRightBtn(wxCommandEvent& event) {
  vsnMethod_SphPEX_paramSpace* pm
    = dynamic_cast<vsnMethod_SphPEX_paramSpace*>(p_method);
  if ( ! pm ) return;

  int oidx = pm->getCurrentParamIdx();
  if ( pm->setCurrentParamIdx(oidx +1) )
    pm->chkNotice();
}

void vsnMPP_SphPEX_paramSpace::OnShowLabelChk(wxCommandEvent& event) {
  vsnMethod_SphPEX_paramSpace* pm
    = dynamic_cast<vsnMethod_SphPEX_paramSpace*>(p_method);
  if ( ! pm ) return;

  bool val = m_pShowLabelChk->GetValue();
  if ( pm->showFrontLabel(val) )
    pm->chkNotice();
}

void vsnMPP_SphPEX_paramSpace::OnLabelTxt(wxCommandEvent& event) {
  vsnMethod_SphPEX_paramSpace* pm
    = dynamic_cast<vsnMethod_SphPEX_paramSpace*>(p_method);
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


//----------------------------------------------------------------
// class vsnMethod_SphPEX_paramSpace
//----------------------------------------------------------------

/* constructors / destructor */

vsnMethod_SphPEX_paramSpace::vsnMethod_SphPEX_paramSpace(const string& name)
  : vsnMethodObj(name), m_currentIdx(0),
    m_pFrLbl(NULL), m_labelFmt("#N = #P")
{
}

vsnMethod_SphPEX_paramSpace::~vsnMethod_SphPEX_paramSpace() {
  if ( m_pFrLbl )
    delete m_pFrLbl;
}


/* methods */

size_t vsnMethod_SphPEX_paramSpace::getNumParamIdx() const {
  vsnData_SphPEX* pData = dynamic_cast<vsnData_SphPEX*>(p_refData);
  if ( ! pData ) return 0;
  return pData->getNumParamIdx();
}

size_t vsnMethod_SphPEX_paramSpace::getCurrentParamIdx() const {
  return m_currentIdx;
}

bool vsnMethod_SphPEX_paramSpace::setCurrentParamIdx(const size_t idx) {
  vsnData_SphPEX* pData = dynamic_cast<vsnData_SphPEX*>(p_refData);
  if ( ! pData ) return false;
  if ( getNumParamIdx() < 1 ) return false;
  if ( m_currentIdx == idx ) return true;

  if ( ! pData->setCurrentParamIdx(idx) ) return false;
  m_currentIdx = idx;
  if ( ! update() ) return false;
  updateUI();
  return true;
}

vsnFrontLabel* vsnMethod_SphPEX_paramSpace::getFrontLabel() {
  if ( ! m_pFrLbl ) {
    m_pFrLbl = new vsnFrontLabel();
    if ( ! m_pFrLbl ) return NULL;
    m_pFrLbl->setLabelColor(m_colour);
    m_pFrLbl->setPickMode(PT_OBJECT);
    m_pFrLbl->setMethodObj(this);
    vector2 initPos = {0.f, 0.9f}; m_pFrLbl->setPosition(initPos);
    updateLabel();
  }
  return m_pFrLbl;
}

bool vsnMethod_SphPEX_paramSpace::showFrontLabel(const bool sfl) {
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

bool vsnMethod_SphPEX_paramSpace::isShowFrontLabel() const {
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

void vsnMethod_SphPEX_paramSpace::setLabelFmt(const std::string& fmt) {
  if ( m_labelFmt == fmt ) return;
  m_labelFmt = fmt;
  updateLabel();
  updateUI();
}

void vsnMethod_SphPEX_paramSpace::updateLabel() {
  vsnData_SphPEX* pData = dynamic_cast<vsnData_SphPEX*>(p_refData);
  if ( ! pData || ! p_refData->ready() ) return;
  vsnFrontLabel* pfl = getFrontLabel();
  if ( ! pfl ) return;

  string paramName = pData->getParamName();
  float paramValue; pData->getCurrentParamValue(paramValue);

  string wks(m_labelFmt);
  char txt[64];
  size_t np;

  while ( (np = wks.find("#N")) != string::npos ) {
    if ( paramName.empty() || paramName == VFR_NONAME )
      sprintf(txt, "%s", "param");
    else
      sprintf(txt, "%s", paramName.c_str());
    wks.replace(np, 2, string(txt));
  } // end of while(#N)

  while ( (np = wks.find("#I")) != string::npos ) {
    sprintf(txt, "%lu", m_currentIdx);
    wks.replace(np, 2, string(txt));
  } // end of while(#I)

  while ( (np = wks.find("#P")) != string::npos ) {
    if ( np+2 < wks.size() && isdigit(wks[np+2]) ) {
      char txt2[16];
      sprintf(txt2, "%%.%cf", wks[np+2]);
      sprintf(txt, txt2, paramValue);
      wks.replace(np, 3, string(txt));
    } else {
      sprintf(txt, "%g", paramValue);
      wks.replace(np, 2, string(txt));
    }
  } // end of while(#P)

  pfl->setLabelStr(wks);
  chkNotice();
}


/* from vsnMethodObj */

bool vsnMethod_SphPEX_paramSpace::update(const bool force) {
  vsnData_SphPEX* pData = dynamic_cast<vsnData_SphPEX*>(p_refData);
  if ( ! pData ) return false;

  updateLabel();

  return true;
}

vsnMethodPP* vsnMethod_SphPEX_paramSpace::getParamPanel(wxPanel* pp) {
  vsnMPP_SphPEX_paramSpace* pp_paramSpace
    = new vsnMPP_SphPEX_paramSpace(pp, this);
  if ( ! pp_paramSpace ) return NULL;
  return pp_paramSpace;
}

void vsnMethod_SphPEX_paramSpace::setBaseColor(const vector4 cv) {
  vsnMethodObj::setBaseColor(cv);
  if ( m_pFrLbl )
    m_pFrLbl->setLabelColor(m_colour);
}

void vsnMethod_SphPEX_paramSpace::setShow(const bool mode) {
  vsnMethodObj::setShow(mode);
  if ( m_pFrLbl )
    m_pFrLbl->getPrivateMaterial()->setRenderMode(m_show?m_showType:RT_NONE);
}


/* from vsnIoObject */

bool vsnMethod_SphPEX_paramSpace::parseXML(xmlNodePtr xnp) {
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
        if ( ! setCurrentParamIdx(cts) ) {
          ErrMsg(MsgERR, msgHdr
                 + string("failed to set current param index: ") + xsV);
          goto _NEXT_XML_NODE;
        }
      } // end of "current"
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
    } // end of param

  _NEXT_XML_NODE:
    cur = cur->next;
  } // end of while(cur)

  return true;
}

bool vsnMethod_SphPEX_paramSpace::outputXML(std::ostream& os, const size_t ts)
{
  string idts;
  for ( int i = 0; i < ts; i++ ) idts.push_back(' ');
  string idts_2 = idts; idts_2.push_back(' '); idts_2.push_back(' ');
  bool ret= true;
  string msgHdr
    = getMethodType() +string("[") +getName() +string("]: outputXML: ");

  vsnData_SphPEX* pData = dynamic_cast<vsnData_SphPEX*>(p_refData);
  if ( ! pData || ! p_refData->ready() ) {
    ErrMsg(MsgERR, msgHdr + string("can't get valid data"));
    return false;
  }

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
  if ( m_currentIdx != 0 ) {
    os << idts_2 << "<param name=\"current\" value=\""
       << m_currentIdx << "\" />" << endl;
  }

  // labels
  if ( isShowFrontLabel() ) {
    os << idts_2 << "<param name=\"show_label\" value=\"yes\" />" << endl;
  }
  if ( m_labelFmt != string("step = #S") ) {
    os << idts_2 << "<param name=\"label_format\" value=\""
       << m_labelFmt << "\" />" << endl;
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

  os << idts << "</method>" << endl;

  return ret;
}

bool vsnMethod_SphPEX_paramSpace::commandXML(xmlNodePtr xnp) {
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


  // do the command
  if ( valueStr.empty() ) {
    ErrMsg(MsgERR, msgHdr + "invalid command: " + nameStr + ": no value");
    return false;
  }

  if ( nameStr == "set_current" ) {
    int cts = atoi(nameStr.c_str());
    if ( ! setCurrentParamIdx(cts) ) {
      ErrMsg(MsgERR, msgHdr +
             string("command set_current: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_current"
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
  else {
    // not my command
    ErrMsg(MsgERR, msgHdr + string("unknown command: ") + nameStr);
    return false;
  }

  return true;
}
