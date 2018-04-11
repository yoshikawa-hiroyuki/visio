//
// vsnMethod_timeStepSync
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

#include "vsnMethod_timeStepSync.h"
#include "vsnScene.h"
#include "vsnError.h"

using namespace std;
using namespace CES;
using namespace VSN;


//----------------------------------------------------------------
// class vsnMPP_timeStepSync
//----------------------------------------------------------------

BEGIN_EVENT_TABLE(vsnMPP_timeStepSync, wxPanel)
  EVT_TEXT_ENTER(MPP_timeStepSync_RefTSDataTxt,
		 vsnMPP_timeStepSync::OnEnterRefTSDataTxt)
  EVT_BUTTON(MPP_timeStepSync_SetRefTSDataBtn,
	     vsnMPP_timeStepSync::OnSetRefTSDataBtn)
  EVT_CHECKBOX(MPP_timeStepSync_PauseChk,
	       vsnMPP_timeStepSync::OnPauseChk)
  EVT_CHECKBOX(MPP_timeStepSync_ShowLabelChk,
	       vsnMPP_timeStepSync::OnShowLabelChk)
  EVT_TEXT_ENTER(MPP_timeStepSync_LabelFmtTxt,
		 vsnMPP_timeStepSync::OnLabelTxt)
  EVT_TEXT_ENTER(MPP_timeStepSync_LabelPosXTxt,
		 vsnMPP_timeStepSync::OnLabelTxt)
  EVT_TEXT_ENTER(MPP_timeStepSync_LabelPosYTxt,
		 vsnMPP_timeStepSync::OnLabelTxt)
  EVT_TEXT_ENTER(MPP_timeStepSync_LabelSizeTxt,
		 vsnMPP_timeStepSync::OnLabelTxt)
END_EVENT_TABLE()


/* constructors / destructor */

vsnMPP_timeStepSync::vsnMPP_timeStepSync(wxPanel* parent, vsnMethodObj* pm)
  : vsnMethodPP(parent, pm),
    m_pRefTSDataTxt(NULL), m_pRefTSDataLst(NULL), m_pSetRefTSDataBtn(NULL),
    m_pCurStpTxt(NULL), m_pPauseChk(NULL),
    m_pShowLabelChk(NULL), m_pLabelFmtTxt(NULL),
    m_pLabelPosXTxt(NULL), m_pLabelPosYTxt(NULL), m_pLabelSizeTxt(NULL)
{
  assert(parent);

  // widgets layout
  wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL); assert(topsizer);
  wxBoxSizer* sizerH;

  // create widgets
  m_pRefTSDataTxt = new wxTextCtrl(this, MPP_timeStepSync_RefTSDataTxt,
				   wxT("none"),
				   wxDefaultPosition, wxDefaultSize,
				   wxTE_READONLY|wxTE_PROCESS_ENTER);
  assert(m_pRefTSDataTxt);

  m_pRefTSDataLst = new wxListBox(this, MPP_timeStepSync_RefTSDataLst);
  assert(m_pRefTSDataLst);

  m_pSetRefTSDataBtn = new wxButton(this, MPP_timeStepSync_SetRefTSDataBtn,
				    wxT("set data to sync timestep"));
  assert(m_pSetRefTSDataBtn);

  m_pCurStpTxt = new wxTextCtrl(this, MPP_timeStepSync_CurStpTxt,
				wxT(""), wxDefaultPosition, wxDefaultSize,
				wxTE_READONLY);
  assert(m_pCurStpTxt);

  m_pPauseChk = new wxCheckBox(this, MPP_timeStepSync_PauseChk, wxT("pause"));
  assert(m_pPauseChk);

  // ref data selector
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND|wxALL, 3);
  sizerH->Add(new wxStaticText(this, -1, wxT("selected")),
              0, wxALIGN_LEFT|wxALL, 3);
  sizerH->Add(m_pRefTSDataTxt, 0, wxEXPAND|wxALL, 3);
  topsizer->Add(m_pRefTSDataLst, 0, wxEXPAND|wxALL, 3);
  topsizer->Add(m_pSetRefTSDataBtn, 0, wxALIGN_LEFT|wxALL, 3);

  // current step
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND|wxALL, 3);
  sizerH->Add(new wxStaticText(this, -1, wxT("current step")),
              0, wxALIGN_LEFT|wxALL, 3);
  sizerH->Add(m_pCurStpTxt, 0, wxEXPAND|wxALL, 3);

  // pause
  topsizer->Add(m_pPauseChk, 0, wxALL, 3);

  // label
  topsizer->Add(
     new wxStaticLine(this, -1, wxDefaultPosition, wxSize(3,3), wxHORIZONTAL),
     0, wxEXPAND|wxALL, 0);

  m_pShowLabelChk = new wxCheckBox(this, MPP_timeStepSync_ShowLabelChk,
                                   wxT("show text label"));
  assert(m_pShowLabelChk);
  topsizer->Add(m_pShowLabelChk, 0, wxALL, 3);

  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND|wxALL, 0);
  sizerH->Add(new wxStaticText(this, -1, wxT("format")),
              0, wxALIGN_LEFT|wxALL, 3);
  m_pLabelFmtTxt = new wxTextCtrl(this, MPP_timeStepSync_LabelFmtTxt, wxT(""),
                                  wxDefaultPosition, wxDefaultSize,
                                  wxTE_PROCESS_ENTER);
  assert(m_pLabelFmtTxt);
  sizerH->Add(m_pLabelFmtTxt, 1, wxEXPAND|wxALL, 3);

  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND|wxALL, 0);
  sizerH->Add(new wxStaticText(this, -1, wxT("pos X")), 0,
              wxALIGN_LEFT|wxALL, 3);
  m_pLabelPosXTxt = new wxTextCtrl(this, MPP_timeStepSync_LabelPosXTxt, wxT(""),
                                   wxDefaultPosition, wxSize(60,-1),
                                   wxTE_PROCESS_ENTER);
  assert(m_pLabelPosXTxt);
  sizerH->Add(m_pLabelPosXTxt, 1, wxEXPAND|wxALL, 3);
  sizerH->Add(new wxStaticText(this, -1, wxT("Y")), 0, wxALIGN_LEFT|wxALL, 3);
  m_pLabelPosYTxt = new wxTextCtrl(this, MPP_timeStepSync_LabelPosYTxt, wxT(""),
                                   wxDefaultPosition, wxSize(60,-1),
                                   wxTE_PROCESS_ENTER);
  assert(m_pLabelPosYTxt);
  sizerH->Add(m_pLabelPosYTxt, 1, wxEXPAND|wxALL, 3);

  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxALL, 0);
  sizerH->Add(new wxStaticText(this, -1, wxT("size")),
              0, wxALIGN_LEFT|wxALL, 3);
  m_pLabelSizeTxt = new wxTextCtrl(this, MPP_timeStepSync_LabelSizeTxt, wxT(""),
                                   wxDefaultPosition, wxDefaultSize,
                                   wxTE_PROCESS_ENTER);
  assert(m_pLabelSizeTxt);
  sizerH->Add(m_pLabelSizeTxt, 0, wxEXPAND|wxALL, 3);

  // post process
  SetSizer(topsizer);
  addTo(parent);
  topsizer->Layout();
  (void)update();
}

vsnMPP_timeStepSync::~vsnMPP_timeStepSync() {
}


/* interface */

bool vsnMPP_timeStepSync::update() {
  vsnMethod_timeStepSync* pm
    = dynamic_cast<vsnMethod_timeStepSync*>(p_method);
  if ( ! pm ) return false;

  // selected ref data / tsd list
  vsnDataObj* rtsd = dynamic_cast<vsnDataObj*>(pm->getTimeSeriesData());
  if ( ! rtsd ) m_pRefTSDataTxt->SetValue(wxT("none"));
  m_pRefTSDataLst->Clear();
  deque< pair<vsnTimeSeriesDataIF*, vsnScene*> > tsdLst
    = pm->getTimeSeriesDataList(pm->getRefData());
  deque< pair<vsnTimeSeriesDataIF*, vsnScene*> >::iterator it;
  for ( it = tsdLst.begin(); it != tsdLst.end(); it++ ) {
    vsnDataObj* pdo = dynamic_cast<vsnDataObj*>(it->first);
    vsnScene* psc = dynamic_cast<vsnScene*>(it->second);
    if ( ! pdo || ! psc ) continue;
    string tsdName = pdo->getName();
    string scnName = psc->getName();
    wxString wxStr;
    if ( tsdName.empty() ) wxStr = wxT(VFR_NONAME);
    else wxStr = vsnApp::ConvSysToWx(tsdName);
    wxStr += wxT("@");
    if ( scnName.empty() ) wxStr += wxT(VFR_NONAME);
    else wxStr += vsnApp::ConvSysToWx(scnName);
    m_pRefTSDataLst->Append(wxStr);

    if ( rtsd == pdo ) m_pRefTSDataTxt->SetValue(wxStr);
  } // end of for(it)

  // current step
  int val = pm->getTimeStep();
  char txt[64]; sprintf(txt, "%d", val); 
  m_pCurStpTxt->SetValue(vsnApp::ConvSysToWx(txt));

  // pause
  m_pPauseChk->SetValue(pm->getPause());

  // label
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

void vsnMPP_timeStepSync::OnEnterRefTSDataTxt(wxCommandEvent& event) {
  if ( ! m_pRefTSDataTxt ) return;
  vsnMethod_timeStepSync* pm
    = dynamic_cast<vsnMethod_timeStepSync*>(p_method);
  if ( ! pm ) return;
  vfrNode* rtsd = dynamic_cast<vfrNode*>(pm->getTimeSeriesData());
  if ( ! rtsd ) return;
  vsnUiView* puiv = getUiView();
  if ( ! puiv ) return;

  puiv->selectObj((vfrNode*)rtsd);
}

void vsnMPP_timeStepSync::OnSetRefTSDataBtn(wxCommandEvent& event) {
  if ( ! m_pRefTSDataTxt || ! m_pRefTSDataLst || ! m_pSetRefTSDataBtn )
    return;

  int sel = m_pRefTSDataLst->GetSelection();
  if ( sel < 0 ) return;
  wxString wkStr = m_pRefTSDataLst->GetString(sel);
  if ( wkStr.IsEmpty() ) return;
  string tgtName = vsnApp::ConvWxToSys(wkStr);

  vsnMethod_timeStepSync* pm
    = dynamic_cast<vsnMethod_timeStepSync*>(p_method);
  if ( ! pm ) return;

  string dataName;
  istringstream iss(tgtName); char c;
  while ( iss.get(c) && (c != '@') ) dataName.push_back(c);
  if ( dataName == string(VFR_NONAME) ) {
    ErrMsg(MsgERR, pm->getMethodType() +string("[") +pm->getName()
	   +string("]: set_ref_data: can't set data with no name"));
    return;
  }

  if ( ! pm->setTimeSeriesData(pm->getRefData(), tgtName) )
    return;

  m_pRefTSDataTxt->SetValue(wkStr);
  pm->chkNotice();
}

void vsnMPP_timeStepSync::OnPauseChk(wxCommandEvent& event) {
  if ( ! m_pPauseChk ) return;
  bool val = m_pPauseChk->GetValue();

  vsnMethod_timeStepSync* pm
    = dynamic_cast<vsnMethod_timeStepSync*>(p_method);
  if ( ! pm ) return;

  if ( pm->setPause(val) )
    pm->chkNotice();
}

void vsnMPP_timeStepSync::OnShowLabelChk(wxCommandEvent& event) {
  vsnMethod_timeStepSync* pm = dynamic_cast<vsnMethod_timeStepSync*>(p_method);
  if ( ! pm ) return;

  bool val = m_pShowLabelChk->GetValue();
  if ( pm->showFrontLabel(val) )
    pm->chkNotice();
}

void vsnMPP_timeStepSync::OnLabelTxt(wxCommandEvent& event) {
  vsnMethod_timeStepSync* pm = dynamic_cast<vsnMethod_timeStepSync*>(p_method);
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
// class vsnMethod_timeStepSync
//----------------------------------------------------------------

/* constructors / destructor */

vsnMethod_timeStepSync::vsnMethod_timeStepSync(const string& name)
  : vsnMethodObj(name), m_timeStep(0), m_pause(false), m_onceSetTs(false),
    m_pFrLbl(NULL), m_labelFmt("step = #S")
{
}

vsnMethod_timeStepSync::~vsnMethod_timeStepSync() {
  if ( m_pFrLbl )
    delete m_pFrLbl;
}


/* interface  */

bool vsnMethod_timeStepSync::setTimeStep(const int stp) {
  if ( stp == m_timeStep ) return true;

  vsnTimeSeriesDataIF* pData = dynamic_cast<vsnTimeSeriesDataIF*>(p_refData);
  if ( ! pData ) return false;
  size_t numStps = pData->getNumSteps();
  if ( numStps < 1 ) return false;
  int stepRange[2];
  if ( ! pData->getTimeStepNo(0, stepRange[0]) ) return false;
  if ( ! pData->getTimeStepNo(numStps -1, stepRange[1]) ) return false;

  if ( stp < stepRange[0] ) m_timeStep = stepRange[0];
  else if ( stp > stepRange[1] ) m_timeStep = stepRange[1];
  else m_timeStep = stp;
  m_onceSetTs = true;

  if ( ! update(false) ) return false;
  updateUI();
  return true;  
}

bool vsnMethod_timeStepSync::setPause(const bool pause) {
  if ( m_pause == pause ) return true;
  m_pause = pause;

  noticeUpdate();
  return true;
}

vsnFrontLabel* vsnMethod_timeStepSync::getFrontLabel() {
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

bool vsnMethod_timeStepSync::showFrontLabel(const bool sfl) {
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

bool vsnMethod_timeStepSync::isShowFrontLabel() const {
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

void vsnMethod_timeStepSync::setLabelFmt(const std::string& fmt) {
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

void vsnMethod_timeStepSync::updateLabel() {
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


/* from vsnTSDataRefer */

void vsnMethod_timeStepSync::noticeUpdate() {
  if ( ! m_pause && p_tsd ) {
    size_t refTsIdx = p_tsd->getCurrentStepIdx();
    int refTsNo;
    if ( ! p_tsd->getTimeStepNo(refTsIdx, refTsNo) ) return;
    if ( ! setTimeStep(refTsNo) ) return;
  }
  updateUI();
}


/* from vsnMethodObj */

bool vsnMethod_timeStepSync::update(const bool force) {
  vsnTimeSeriesDataIF* pData = dynamic_cast<vsnTimeSeriesDataIF*>(p_refData);
  if ( ! pData || ! p_refData->ready() ) return false;

  if ( ! m_onceSetTs ) {
    if ( ! pData->getTimeStepNo(0, m_timeStep) )
      return false;
  }

  size_t stp;
  if ( ! pData->getStepIdx(m_timeStep, stp) ) return false;
  if ( ! pData->setCurrentStepIdx(stp) ) return false;

  updateLabel();

  return true;
}

vsnMethodPP* vsnMethod_timeStepSync::getParamPanel(wxPanel* pp) {
  vsnMPP_timeStepSync* pp_timeStepSync = new vsnMPP_timeStepSync(pp, this);
  if ( ! pp_timeStepSync ) return NULL;
  return pp_timeStepSync;
}


/* from vsnIoObject */

bool vsnMethod_timeStepSync::parseXML(xmlNodePtr xnp) {
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

      if ( xsN == string("pause") ) {
	bool pause;
	if ( xsV == string("yes") ) pause = true;
	else if ( xsV == string("no") ) pause = false;
	else {
	  ErrMsg(MsgERR, msgHdr + string("invalid value in param pause"));
          goto _NEXT_XML_NODE;
        }
	if ( ! setPause(pause) ) {
	  ErrMsg(MsgERR, msgHdr + string("can't set pause"));
          goto _NEXT_XML_NODE;
	}
      } // end of "pause"
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

      // --- there is no param tag with "ref_data", use command ---
    } // end of param

  _NEXT_XML_NODE:
    cur = cur->next;
  } // end of while(cur)

  return true;
}

bool vsnMethod_timeStepSync::outputXML(std::ostream& os, const size_t ts) {
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
  // pause
  if ( m_pause ) {
    os << idts_2 << "<param name=\"pause\" value=\"yes\" />" << endl;
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

  os << idts << "</method>" << endl;

  // output 'set_ref_data' command
  if ( p_tsd ) {
    if ( ! vsnTSDataRefer::exportXMLCommand(getName(), os, ts) )
      return false;
  }

  return true;
}

bool vsnMethod_timeStepSync::commandXML(xmlNodePtr xnp) {
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
  if ( nameStr == "set_ref_data" ) {
    if ( valueStr == VFR_NONAME ) {
      ErrMsg(MsgERR, msgHdr + "command set_ref_data: " +
             "can't set data with no name");
      return false;
    }
    if ( ! setTimeSeriesData(p_refData, valueStr) ) {
      ErrMsg(MsgERR, msgHdr +
             string("command set_ref_data: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_ref_data"
  else if ( nameStr == "set_pause" ) {
    bool pause;
    if ( valueStr == string("yes") ) pause = true;
    else if ( valueStr == string("no") ) pause = false;
    else {
      ErrMsg(MsgERR, msgHdr + string("command set_pause: invalid value"));
      return false;
    }
    if ( ! setPause(pause) ) {
      ErrMsg(MsgERR, msgHdr +
             string("command set_pause: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_pause"
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
