//
// vsnMethod_probe
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

#include "vsnMethod_probe.h"
#include "vsnDataObj.h"
#include "vsnUiView.h"
#include "vsnError.h"

#include "vfrLines.h"
#include "vfrCone.h"
#include "vfrCylinder.h"

using namespace std;
using namespace CES;
using namespace VSN;


//----------------------------------------------------------------
// class vsnMPP_probe::ProbeDragTransAct
//----------------------------------------------------------------
vsnMPP_probe::
ProbeDragTransAct::ProbeDragTransAct(vsnGfxView* pgv, vsnMethod_probe* pm)
  : vsnGfxBaseAct(pgv), p_method(pm) {
}

void vsnMPP_probe::
ProbeDragTransAct::setProbeMethod(vsnMethod_probe* pm) {
  if ( p_method == pm ) return;
  p_method = pm;
}

void vsnMPP_probe::
ProbeDragTransAct::execute(vfrEvent& e) {
  if ( ! p_gfxView || ! p_method ) return;
  vfrScreen* screen = (vfrScreen*)e.getScreen();
  if ( ! screen ) return;

  unsigned int tid = p_method->getID();
  Point2 mp2 = e.getMPoint();
  Point2 mp1 = mp2 - e.getMMove();
  vector3 objp1, objp2;
  if ( ! screen->getObjCoord(tid, mp1, objp1) ) return;
  if ( ! screen->getObjCoord(tid, mp2, objp2) ) return;
  Vec3<float> tv = Vec3<float>(objp2) - Vec3<float>(objp1);

  p_method->setT(p_method->getT() + tv);
}


//----------------------------------------------------------------
// class vsnMPP_probe
//----------------------------------------------------------------

BEGIN_EVENT_TABLE(vsnMPP_probe, wxPanel)
  EVT_BUTTON(MPP_probe_XFormBtn, vsnMPP_probe::OnXFormBtn)
  EVT_CHECKBOX(MPP_probe_UseMouseChk, vsnMPP_probe::OnUseMouseChk)
END_EVENT_TABLE()


/* constructors / destructor */

vsnMPP_probe::vsnMPP_probe(wxPanel* parent, vsnMethodObj* pm)
  : vsnMethodPP(parent, pm), m_pXFormDlg(NULL)
{
  assert(parent);
  assert(dynamic_cast<vsnMethod_probe*>(pm));

  vsnUiView* puiv = getUiView();
  if ( puiv ) {
    vsnViewFrame* pvf = dynamic_cast<vsnViewFrame*>(puiv->GetParent());
    if ( pvf )
      gfxAct_DTG.setGfxView(pvf->getGfxView());
  } // end of if(puiv)


  wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer* sizerH;

  // xform button, use_mouse chkbox
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND);
  m_pXFormBtn = new wxButton(this, MPP_probe_XFormBtn, wxT("xform"));
  sizerH->Add(m_pXFormBtn, 0, wxALIGN_LEFT|wxALL, 3);
  sizerH->Add(10, 0, 1, wxEXPAND);
  m_pUseMouseChk = new wxCheckBox(this, MPP_probe_UseMouseChk,
				  wxT("use mouse"));
  sizerH->Add(m_pUseMouseChk, 0, wxALIGN_RIGHT|wxALL, 3);

  // rep text area
  m_pRepTxt = new wxTextCtrl(this, -1, wxT("out of data\n"),
			     wxDefaultPosition, wxSize(-1, 250),
			     wxTE_MULTILINE|wxTE_READONLY);
  topsizer->Add(m_pRepTxt, 0, wxEXPAND|wxALL, 3);

  // post process
  SetSizer(topsizer);
  addTo(parent);
  topsizer->Layout();
  (void)update();
}

vsnMPP_probe::~vsnMPP_probe() {
  settlement();
}


/* interface */

bool vsnMPP_probe::update() {
  if ( ! m_pXFormBtn || ! m_pUseMouseChk ) return false;

  vsnMethod_probe* pm = dynamic_cast<vsnMethod_probe*>(p_method);
  if ( ! pm ) return false;

  wxString msg;
  string repStr = pm->getRepStr();
  if ( repStr.empty() )
    msg = wxT("out of data\n");
  else
    msg = vsnApp::ConvSysToWx(repStr.c_str());
  m_pRepTxt->SetValue(msg);

  if ( m_pXFormDlg )
    (void)m_pXFormDlg->update();

  if ( getUiView() )
    m_pUseMouseChk->Enable();
  else
    m_pUseMouseChk->Disable();

  return true;
}

void vsnMPP_probe::settlement() {
  // hide XFormDlg of ParamPanel
  showXFormDlg(false);

  // set use_mouse mode to off
  setUseMouseMode(false);
}


void vsnMPP_probe::showXFormDlg(const bool show) {
  if ( ! m_pXFormDlg ) {
    if ( ! show ) return;
    m_pXFormDlg
      = new vsnXFormDlg(this, dynamic_cast<vsnMethod_probe*>(p_method),
			false, true);
    if ( ! m_pXFormDlg ) {
      ErrMsg(MsgERR, string("probe: can't create XFormDlg"));
      return;
    }
    m_pXFormDlg->SetTitle(wxT("xform probe"));
  } // end of if(!m_pXFormDlg)

  m_pXFormDlg->Show(show);
}

void vsnMPP_probe::setUseMouseMode(const bool umm) {
  if ( ! gfxAct_DTG.p_gfxView ) return;
  vfrDrawAreaWx* pda = gfxAct_DTG.p_gfxView->getDrawArea();
  vsnApp* pApp = vsnApp::GetApp();
  if ( ! pda || ! pApp ) return;
  vfrDispatch &dispatcher = vfrDispatch::instance(*pda);
  vsnGfxOprOrientation oprOri = pApp->getOprOrientation();

  // this method may be called from destructor, don't use dynamic_cast
  vsnMethod_probe* pprobe = static_cast<vsnMethod_probe*>(p_method);
  if ( ! pprobe ) return;

  bool curMode = (gfxAct_DTG.p_method != NULL);
  if ( curMode == umm ) return;

  if ( umm ) {
    gfxAct_DTG.p_gfxView->pushActions(this);
    gfxAct_DTG.p_gfxView->setStandardActions();
    oprOri.m_oprLst[OprTranslate].getEvent(dispatcher, EvtDrag)
      .regist(&gfxAct_DTG);
    gfxAct_DTG.p_gfxView->setCursor();
    gfxAct_DTG.setProbeMethod(pprobe);
  }
  else {
    gfxAct_DTG.p_gfxView->popActions(this);
    gfxAct_DTG.setProbeMethod(NULL);
  }
}


/* event handler */

void vsnMPP_probe::OnXFormBtn(wxCommandEvent& event) {
  showXFormDlg();
}

void vsnMPP_probe::OnUseMouseChk(wxCommandEvent& event) {
  if ( ! m_pUseMouseChk ) return;
  vsnMethod_probe* pm = dynamic_cast<vsnMethod_probe*>(p_method);
  if ( ! pm ) return;

  bool val = m_pUseMouseChk->GetValue();
  setUseMouseMode(val);
}


//----------------------------------------------------------------
// class vsnMethod_probe
//----------------------------------------------------------------

#define PROBE_GLYPH_RADIUS 0.05f

/* constructors / destructor */

vsnMethod_probe::vsnMethod_probe(const string& name)
  : vsnMethodObj(name), m_glyph(NULL),
    m_initS(1.f,1.f,1.f), m_xformInited(false)
{
}

vsnMethod_probe::~vsnMethod_probe() {
  if ( m_glyph )
    delete m_glyph;
}


/* vsnXForm methods */

void vsnMethod_probe::updateXForm() {
  m_matXForm = getXFormMatrix();
  if ( update() )
    chkNotice();
}

void vsnMethod_probe::resetXForm() {
  m_T = m_initT;
  m_S = m_initS;
  m_HPR = m_initHPR;
  updateXForm(); 
}


/* vsnMethodObj methods */

bool vsnMethod_probe::update(const bool force) {
  if ( ! m_xformInited ) {
    if ( p_refData ) {
      const Vec3<float>* dbb = p_refData->getBbox();
      m_initT = (dbb[1] + dbb[0]) * 0.5f;
      Vec3<float> bbLen = dbb[1] - dbb[0];
      m_initS[0] = m_initS[1] = m_initS[2] = bbLen.Length() * 0.2f;
      if ( m_initS[0] < 1e-6f )
	m_initS[0] = m_initS[1] = m_initS[2] = 1e-6f;
      m_initHPR = Vec3<float>(0.f,0.f,0.f);

      m_T = m_initT; m_S = m_initS; m_HPR = m_initHPR;
      // don't call updateXForm() here, that will call update() again...
      m_matXForm = getXFormMatrix();
      m_xformInited = true;
    }
  } // end of if(!m_xformInited)

  // glyph
  if ( ! m_glyph ) {
    m_glyph = new vfrGroup("probe_glyph");
    if ( ! m_glyph ) {
      ErrMsg(MsgERR, getMethodType() + string("[") + getName()
             + string("]: memory allocation failed"));
      return false;
    }
    m_glyph->alcMaterial();

    // glyph parts
    vfrLines* pneedle
      = new vfrLines("probe_needle", TRUE);
    vfrCone* pcone
      = new vfrCone(PROBE_GLYPH_RADIUS*0.75f, 0.5f, "probe_cone", TRUE);
    vfrCylinder* pbody
      = new vfrCylinder(PROBE_GLYPH_RADIUS, 0.4f, "probe_body", TRUE);
    vector3 needleVtx[] = {{0.f, -0.05f, 0.f}, {0.f, 1.f, 0.f},
			   {-0.05f, 0.f, 0.05f}, {0.05f, 0.f, -0.05f},
			   {0.05f, 0.f, 0.05f}, {-0.05f, 0.f, -0.05f}};
    pneedle->setVerts(sizeof(needleVtx)/sizeof(vector3), needleVtx);
    pcone->rotx(M_PI); pcone->trans(0.f, -(0.1f+0.5f/2.f), 0.f);
    pbody->trans(0.f, 0.6f+0.4f/2.f, 0.f);
    m_glyph->addChild(pneedle);
    m_glyph->addChild(pcone);
    m_glyph->addChild(pbody);

    addChild(m_glyph);
  } // end of if(!m_glyph)

  return updateStep(m_requestedStp, force);
}

vsnMethodPP* vsnMethod_probe::getParamPanel(wxPanel* pp) {
  vsnMPP_probe* pp_probe = new vsnMPP_probe(pp, this);
  if ( ! pp_probe ) return NULL;
  return pp_probe;
}


/* vsnMethodObj methods */

bool vsnMethod_probe::parseXML(xmlNodePtr xnp) {
  xmlNodePtr cur;

  if ( ! vsnMethodObj::parseXML(xnp) ) return false;
  string msgHdr
    = getMethodType() +string("[") +getName() +string("]: parseXML: ");

  // get children node
  cur = xnp->xmlChildrenNode;

  // traverse
  while ( cur ) {
    if ( (cur = vsnIoObject::SkipCommentXML(cur)) == NULL ) break;

    if ( !strcmp((const char*)cur->name, "xform") ) {
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

bool vsnMethod_probe::outputXML(std::ostream& os, const size_t ts) {
  std::string idts;
  for ( int i = 0; i < ts; i++ ) idts.push_back(' ');
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

  os << idts << "</method>" << endl;

  return ret;
}

bool vsnMethod_probe::commandXML(xmlNodePtr xnp) {
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
  else {
    // not my command
    ErrMsg(MsgERR, msgHdr + string("unknown command: ") + nameStr);
    return false;
  }

  return true;
}

