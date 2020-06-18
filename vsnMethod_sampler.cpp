//
// vsnMethod_sampler
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

#include "vsnMethod_sampler.h"
#include "vsnError.h"
#include <iostream>
#include <fstream>

using namespace std;
using namespace CES;
using namespace VSN;


//----------------------------------------------------------------
// class vsnMPP_sampler::SamplerDragTransAct
//----------------------------------------------------------------
vsnMPP_sampler::SamplerDragTransAct::SamplerDragTransAct(vsnGfxView* pgv,
							 vsnMethod_sampler* pm)
  : vsnGfxBaseAct(pgv), p_method(pm) {
}

void vsnMPP_sampler::
SamplerDragTransAct::setSamplerMethod(vsnMethod_sampler* pm) {
  if ( p_method == pm ) return;
  p_method = pm;
}

void vsnMPP_sampler::SamplerDragTransAct::execute(vfrEvent& e) {
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
// class vsnMPP_sampler
//----------------------------------------------------------------

BEGIN_EVENT_TABLE(vsnMPP_sampler, wxPanel)
  EVT_TEXT_ENTER(MPP_sampler_DivM, vsnMPP_sampler::OnDivChanged)
  EVT_TEXT_ENTER(MPP_sampler_DivN, vsnMPP_sampler::OnDivChanged)
  EVT_TEXT_ENTER(MPP_sampler_LineWidth, vsnMPP_sampler::OnLineWidthChanged)
  EVT_TEXT_ENTER(MPP_sampler_PointSize, vsnMPP_sampler::OnPointSizeChanged)
  EVT_BUTTON(MPP_sampler_XFormBtn, vsnMPP_sampler::OnXFormBtn)
  EVT_CHECKBOX(MPP_sampler_UseMouseChk, vsnMPP_sampler::OnUseMouseChk)
  EVT_CHECKBOX(MPP_sampler_AntiAliasChk, vsnMPP_sampler::OnAntiAliasChk)
  EVT_BUTTON(MPP_sampler_ExportBtn, vsnMPP_sampler::OnExportBtn)
END_EVENT_TABLE()


/* constructors / destructor */

vsnMPP_sampler::vsnMPP_sampler(wxPanel* parent, vsnMethodObj* pm)
  : vsnMethodPP(parent, pm), m_pXFormDlg(NULL)
{
  assert(parent);
  assert(dynamic_cast<vsnMethod_sampler*>(pm));

  vsnUiView* puiv = getUiView();
  if ( puiv ) {
    vsnViewFrame* pvf = dynamic_cast<vsnViewFrame*>(puiv->GetParent());
    if ( pvf )
      gfxAct_DTG.setGfxView(pvf->getGfxView());
  } // end of if(puiv)


  wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer* sizerH;

  // division
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND);
  sizerH->Add(new wxStaticText(this, -1, wxT("div")),
	      0, wxALIGN_LEFT|wxALL, 3);
  m_pDivMTxt = new wxTextCtrl(this, MPP_sampler_DivM, wxT(""),
			      wxDefaultPosition, wxSize(70,-1),
			      wxTE_PROCESS_ENTER);
  sizerH->Add(m_pDivMTxt, 1, wxEXPAND|wxALL, 3);
  sizerH->Add(new wxStaticText(this, -1, wxT(" x ")),
	      0, wxALIGN_LEFT|wxALL, 3);
  m_pDivNTxt = new wxTextCtrl(this, MPP_sampler_DivN, wxT(""),
			      wxDefaultPosition, wxSize(70,-1),
			      wxTE_PROCESS_ENTER);
  sizerH->Add(m_pDivNTxt, 1, wxEXPAND|wxALL, 3);

  // line width
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND);
  sizerH->Add(new wxStaticText(this, -1, wxT("line width")),
	      0, wxALIGN_LEFT|wxALL, 3);
  m_pLineWidthTxt = new wxTextCtrl(this, MPP_sampler_LineWidth,
				   wxT(""), wxDefaultPosition, wxDefaultSize,
				   wxTE_PROCESS_ENTER);
  sizerH->Add(m_pLineWidthTxt, 0, wxEXPAND|wxALL, 3);

  // point size
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND);
  sizerH->Add(new wxStaticText(this, -1, wxT("point size")),
	      0, wxALIGN_LEFT|wxALL, 3);
  m_pPointSizeTxt = new wxTextCtrl(this, MPP_sampler_PointSize,
				   wxT(""), wxDefaultPosition, wxDefaultSize,
				   wxTE_PROCESS_ENTER);
  sizerH->Add(m_pPointSizeTxt, 0, wxEXPAND|wxALL, 3);

  // anti-alias
  m_pAntiAliasChk = new wxCheckBox(this, MPP_sampler_AntiAliasChk,
				   wxT("anti-alias line"));
  topsizer->Add(m_pAntiAliasChk, 0, wxALL, 3);

  // xform btn, use_mouse chkbox
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND);
  m_pXformBtn = new wxButton(this, MPP_sampler_XFormBtn, wxT("xform"));
  sizerH->Add(m_pXformBtn, 0, wxALIGN_LEFT|wxALL, 3);
  sizerH->Add(5, 5, 1, wxEXPAND);
  m_pUseMouseChk = new wxCheckBox(this, MPP_sampler_UseMouseChk,
				  wxT("use mouse"));
  sizerH->Add(m_pUseMouseChk, 0, wxALIGN_LEFT|wxALL, 3);

  // export btn
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND);
  m_pExportBtn = new wxButton(this, MPP_sampler_ExportBtn, wxT("export"));
  sizerH->Add(m_pExportBtn, 0, wxALIGN_LEFT|wxALL, 3);

  // post process
  SetSizer(topsizer);
  addTo(parent);
  topsizer->Layout();
  (void)update();
}

vsnMPP_sampler::~vsnMPP_sampler() {
  settlement();
}


/* interface */

bool vsnMPP_sampler::update() {
  if ( ! m_pDivMTxt || ! m_pDivNTxt || ! m_pUseMouseChk ||
       ! m_pLineWidthTxt || ! m_pPointSizeTxt || ! m_pAntiAliasChk )
    return false;

  vsnMethod_sampler* pm = dynamic_cast<vsnMethod_sampler*>(p_method);
  if ( ! pm ) return false;

  char txt[64];
  Point2 div = pm->getSampleNumber();
  sprintf(txt, "%d", div.x); m_pDivMTxt->SetValue(vsnApp::ConvSysToWx(txt));
  sprintf(txt, "%d", div.y); m_pDivNTxt->SetValue(vsnApp::ConvSysToWx(txt));

  float fval = pm->getLineWidth();
  sprintf(txt, "%g", fval); m_pLineWidthTxt->SetValue(vsnApp::ConvSysToWx(txt));
  fval = pm->getPointSize();
  sprintf(txt, "%g", fval); m_pPointSizeTxt->SetValue(vsnApp::ConvSysToWx(txt));

  m_pAntiAliasChk->SetValue(pm->getAntiAliasMode());

  if ( m_pXFormDlg )
    (void)m_pXFormDlg->update();

  if ( getUiView() )
    m_pUseMouseChk->Enable();
  else
    m_pUseMouseChk->Disable();

  return true;
}

void vsnMPP_sampler::settlement() {
  // hide XFormDlg of ParamPanel
  showXFormDlg(false);

  // set use_mouse mode to off
  setUseMouseMode(false);
}


void vsnMPP_sampler::showXFormDlg(const bool show) {
  if ( ! m_pXFormDlg ) {
    if ( ! show ) return;
    m_pXFormDlg
      = new vsnXFormDlg(this,
			dynamic_cast<vsnMethod_sampler*>(p_method),
			false, false);
    if ( ! m_pXFormDlg ) {
      ErrMsg(MsgERR, string("sampler: can't create XFormDlg"));
      return;
    }
    m_pXFormDlg->SetTitle(wxT("xform sampler"));
  } // end of if(!m_pXFormDlg)

  m_pXFormDlg->Show(show);
}

void vsnMPP_sampler::setUseMouseMode(const bool umm) {
  if ( ! gfxAct_DTG.p_gfxView ) return;
  vfrDrawAreaWx* pda = gfxAct_DTG.p_gfxView->getDrawArea();
  vsnApp* pApp = vsnApp::GetApp();
  if ( ! pda || ! pApp ) return;
  vfrDispatch &dispatcher = vfrDispatch::instance(*pda);
  vsnGfxOprOrientation oprOri = pApp->getOprOrientation();

  // this method may be called from destructor, don't use dynamic_cast
  vsnMethod_sampler* psampler = static_cast<vsnMethod_sampler*>(p_method);
  if ( ! psampler ) return;

  bool curMode = (gfxAct_DTG.p_method != NULL);
  if ( curMode == umm ) return;

  if ( umm ) {
    gfxAct_DTG.p_gfxView->pushActions(this);
    gfxAct_DTG.p_gfxView->setStandardActions();
    oprOri.m_oprLst[OprTranslate].getEvent(dispatcher, EvtDrag)
      .regist(&gfxAct_DTG);
    gfxAct_DTG.p_gfxView->setCursor();
    gfxAct_DTG.setSamplerMethod(psampler);
  }
  else {
    gfxAct_DTG.p_gfxView->popActions(this);
    gfxAct_DTG.setSamplerMethod(NULL);
  }
}


/* event handler */

void vsnMPP_sampler::OnDivChanged(wxCommandEvent& event) {
  if ( ! m_pDivMTxt || ! m_pDivNTxt ) return;
  vsnMethod_sampler* pm = dynamic_cast<vsnMethod_sampler*>(p_method);
  if ( ! pm ) return;

  Point2 div; wxString valStr;
  valStr = m_pDivMTxt->GetValue(); if ( valStr.IsEmpty() ) return;
  div.x = atoi(vsnApp::ConvWxToSys(valStr).c_str());
  valStr = m_pDivNTxt->GetValue(); if ( valStr.IsEmpty() ) return;
  div.y = atoi(vsnApp::ConvWxToSys(valStr).c_str());

  if ( ! pm->setSampleNumber(div) ) {
    update();
    return;
  }
  pm->chkNotice();
}

void vsnMPP_sampler::OnLineWidthChanged(wxCommandEvent& event) {
  if ( ! m_pLineWidthTxt ) return;
  vsnMethod_sampler* pm = dynamic_cast<vsnMethod_sampler*>(p_method);
  if ( ! pm ) return;

  wxString valStr = m_pLineWidthTxt->GetValue();
  if ( valStr.IsEmpty() ) return;
  float value = (float)atof(vsnApp::ConvWxToSys(valStr).c_str());

  if ( ! pm->setLineWidth(value) ) {
    update();
    return;
  }
  pm->chkNotice();
}

void vsnMPP_sampler::OnPointSizeChanged(wxCommandEvent& event) {
  if ( ! m_pPointSizeTxt ) return;
  vsnMethod_sampler* pm = dynamic_cast<vsnMethod_sampler*>(p_method);
  if ( ! pm ) return;

  wxString valStr = m_pPointSizeTxt->GetValue();
  if ( valStr.IsEmpty() ) return;
  float value = (float)atof(vsnApp::ConvWxToSys(valStr).c_str());

  if ( ! pm->setPointSize(value) ) {
    update();
    return;
  }
  pm->chkNotice();
}

void vsnMPP_sampler::OnAntiAliasChk(wxCommandEvent& event) {
  if ( ! m_pAntiAliasChk ) return;
  vsnMethod_sampler* pm = dynamic_cast<vsnMethod_sampler*>(p_method);
  if ( ! pm ) return;

  bool val = m_pAntiAliasChk->GetValue();
  if ( pm->setAntiAliasMode(val) )
    pm->chkNotice();
}

void vsnMPP_sampler::OnXFormBtn(wxCommandEvent& event) {
  showXFormDlg();
}

void vsnMPP_sampler::OnUseMouseChk(wxCommandEvent& event) {
  if ( ! m_pUseMouseChk ) return;
  vsnMethod_sampler* pm = dynamic_cast<vsnMethod_sampler*>(p_method);
  if ( ! pm ) return;

  bool val = m_pUseMouseChk->GetValue();
  setUseMouseMode(val);
}

void vsnMPP_sampler::OnExportBtn(wxCommandEvent& event) {
  vsnMethod_sampler* pm = dynamic_cast<vsnMethod_sampler*>(p_method);
  if ( ! pm ) return;
  vsnApp* pApp = vsnApp::GetApp();

  wxFileDialog fileDlg(this, wxT("sampler: specify file to export"),
                       wxT(""), wxT(""), // default Dir / File
                       wxT("CSV (*.csv)|*.csv")
                       wxT("|(*)|*"),
                       wxFD_SAVE);

  // set default params
  string targDir = pApp->getImportDir();
  if ( targDir.empty() ) {
    string appCurFile = pApp->getCurrentFilename();
    if ( ! appCurFile.empty() )
      targDir = DirName(appCurFile, vsnPath_getDelimChar());
  }
  if ( targDir.empty() )
    targDir = pApp->getCwd();
  if ( ! targDir.empty() )
    fileDlg.SetDirectory(vsnApp::ConvSysToWx(targDir));

  // get output path
  if ( fileDlg.ShowModal() != wxID_OK ) return;
  string outPath = vsnPath_normalize(vsnApp::ConvWxToSys(fileDlg.GetPath()));
  if ( outPath.empty() ) return;

  // override check
  FILE* ofp = fopen(outPath.c_str(), "r");
  if ( ofp ) {
    fclose(ofp);
    wxString msg = wxT("The specified file has already existed\n  ");
    msg += wxString::FromUTF8(outPath.c_str());
    msg += wxT("\n\nAre you sure to override ?\n");
    wxMessageDialog dlg(NULL, msg, wxT("sampler: export"),
                        vsn_wxOK_CANCEL|wxICON_QUESTION);
    if ( dlg.ShowModal() != vsn_wxIDOK ) return;
  }

  // export
  if ( ! pm->exportFile(outPath) ) {
    ErrMsg(MsgERR, string("sampler: export failed.\n  File: ") + outPath);
    return;
  }

  return;
}


//----------------------------------------------------------------
// class vsnRef_sampler
//----------------------------------------------------------------

vsnRef_sampler::~vsnRef_sampler() {
  if ( p_splr )
    p_splr->removeRefMethod(this);
}


deque<vsnMethod_sampler*> vsnRef_sampler::getSamplerList(vsnDataObj* pdo) {
  deque<vsnMethod_sampler*> retLst;
  if ( ! pdo ) return retLst;

  vsnMethodObj* pmo = pdo->getNextMethod(NULL);
  while ( pmo ) {
    vsnMethod_sampler* pmo_splr = dynamic_cast<vsnMethod_sampler*>(pmo);
    if ( pmo_splr ) retLst.push_back(pmo_splr);

    pmo = pdo->getNextMethod(pmo);
  } // end of while(pmo)

  return retLst;
}


void
vsnRef_sampler::setSampler(vsnMethod_sampler* splr) {
  if ( splr == p_splr ) return;
  if ( p_splr ) p_splr->removeRefMethod(this);
  p_splr = splr;
  if ( p_splr ) p_splr->appendRefMethod(this);

  // adjust method order: sampler must be ordered before this(for serialize)
  vsnDataObj* pdo = NULL;
  vsnMethodObj* pmo = dynamic_cast<vsnMethodObj*>(this);
  if ( p_splr ) pdo = p_splr->getRefData();
  if ( pdo && pmo ) pdo->adjustMethodOrder(p_splr, pmo);

  noticeUpdate();
}

void
vsnRef_sampler::setSampler(vsnDataObj* pdo, const int n) {
  if ( ! pdo ) return;
  if ( n < 0 ) {
    setSampler(NULL);
    return;
  }
  deque<vsnMethod_sampler*> splrlst = getSamplerList(pdo);
  if ( n >= splrlst.size() ) {
    setSampler(NULL);
    return;
  }
  setSampler(splrlst[n]);
}

bool
vsnRef_sampler::setSampler(vsnDataObj* pdo, const string& samplerName) {
  if ( ! pdo ) return false;
  if ( samplerName.empty() || samplerName == string(VFR_NONAME) )
    return false;

  deque<vsnMethod_sampler*> splrlst = getSamplerList(pdo);
  for ( int i = 0; i < splrlst.size(); i++ ) {
    if ( ! splrlst[i] ) continue;
    if ( samplerName == splrlst[i]->getName() ) {
      this->vsnRef_sampler::setSampler(splrlst[i]);
      return (p_splr != NULL);
    }
  } // end of for(i)
  return false;
}

void vsnRef_sampler::noticeDie() {
  if ( p_splr ) p_splr = NULL;
  noticeUpdate();
}

// XML util
bool vsnRef_sampler::exportXMLNode(std::ostream& os,
					  const size_t ts) const {
  std::string idts;
  for ( int i = 0; i < ts; i++ ) idts.push_back(' ');

  if ( p_splr ) {
    string splrn = p_splr->getName();
    if ( ! splrn.empty() && splrn != VFR_NONAME ) {
      os << idts << "<param name=\"sampler\" value=\""
         << splrn << "\" />" << endl;
    } else {
      return false;
    }
  }
  return true;
}


//----------------------------------------------------------------
// class vsnMethod_sampler
//----------------------------------------------------------------

/* constructors / destructor */

vsnMethod_sampler::vsnMethod_sampler(const std::string& name)
  : vsnMethodObj(name),
    m_initS(1.f,1.f,1.f), m_xformInited(false),
    m_outline(NULL), m_points(NULL)
{
  m_sampleSize = getIniSampleNumber();
  m_lineWidth = getIniLineWidth();
  m_pointSize = getIniPointSize();
}

vsnMethod_sampler::~vsnMethod_sampler() {
  if ( m_outline )
    delete m_outline;

  if ( m_points )
    delete m_points;

  set<vsnRef_sampler*>::iterator it;
  for ( it = m_refMethodLst.begin(); it != m_refMethodLst.end(); it++ ) {
    if ( *it ) (*it)->noticeDie();
  } // end of for(it)
}


/* methods */

bool vsnMethod_sampler::setSampleNumber(const Point2& sn) {
  if ( m_sampleSize.x == sn.x && m_sampleSize.y == sn.y )
    return true;
  if ( sn.x < 1 || sn.y < 1 ) return false;

  m_sampleSize = sn;

  if ( ! update() ) return false;
  updateUI();
  return true;
}

bool vsnMethod_sampler::setLineWidth(const float lw) {
  if ( m_lineWidth == lw ) return true;
  if ( lw < 0.f ) return false; // allow 0.0

  m_lineWidth = lw;

  if ( m_outline ) {
    if ( m_lineWidth <= 0.f )
      m_outline->getPrivateMaterial()->setRenderMode(RT_NONE);
    else {
      m_outline->getPrivateMaterial()->setRenderMode(RT_WIRE);
      m_outline->getPrivateMaterial()->setLineWidth(m_lineWidth);
    }
  }

  updateUI();
  chkNotice();
  return true;
}

bool vsnMethod_sampler::setPointSize(const float psz) {
  if ( m_pointSize == psz ) return true;
  if ( psz < 0.f ) return false; // allow 0.0

  m_pointSize = psz;

  if ( m_points ) {
    if ( m_pointSize <= 0.f )
      m_points->getPrivateMaterial()->setRenderMode(RT_NONE);
    else {
      m_points->getPrivateMaterial()->setRenderMode(RT_POINT);
      m_points->getPrivateMaterial()->setPointSize(m_pointSize);
    }
  }

  updateUI();
  chkNotice();
  return true;
}

CES::Vec3<float> vsnMethod_sampler::getNormalVec() const {
  vfrMatrix M;
  M.RotY(CES::Deg2Rad(m_HPR[0]));
  M.RotX(CES::Deg2Rad(m_HPR[1]));
  M.RotZ(CES::Deg2Rad(m_HPR[2]));

  Vec3<float> vn = M * Vec3<float>(0.f, 0.f, 1.f);
  vn.UnitVec();
  return vn;
}

bool vsnMethod_sampler::exportFile(const std::string& path) {
  if ( path.empty() ) return false;
  size_t sampleNum = m_sampleSize.x * m_sampleSize.y;
  if ( sampleNum < 1 ) return false;
  vector3* vp = getVerts();
  if ( ! vp ) return false;

  ofstream os(path.c_str());
  if ( ! os ) return false;
  os << "# TRANSLATE=0.0, 0.0, 0.0" << endl;
  os << "# SCALE=1.0, 1.0, 1.0" << endl;
  os << "# ROTATE=0.0, 0.0, 0.0" << endl;

  os << sampleNum << endl;
  for ( auto j = 0; j < m_sampleSize.y; j++ ) {
    for ( auto i = 0; i < m_sampleSize.x; i++ ) {
      auto k = m_sampleSize.x * j + i;
      os << vp[k][0] << ", " << vp[k][1] << ", " << vp[k][2] << endl;
    } // end of for(i)
  } // end of for(j)

  os.close();
  return true;
}


/* refMethodLst control */

void vsnMethod_sampler::appendRefMethod(vsnRef_sampler* refm) {
  m_refMethodLst.insert(refm);
}

void vsnMethod_sampler::removeRefMethod(vsnRef_sampler* refm) {
  set<vsnRef_sampler*>::iterator it = m_refMethodLst.find(refm);
  if ( it == m_refMethodLst.end() ) return;
  m_refMethodLst.erase(it);
}

void vsnMethod_sampler::updateRefMethod() {
  set<vsnRef_sampler*>::iterator it;
  for ( it = m_refMethodLst.begin(); it != m_refMethodLst.end(); it++ ) {
    if ( *it ) (*it)->noticeUpdate();
  } // end of for(it)
}


/* vsnXForm method */

void vsnMethod_sampler::updateXForm() {
  m_matXForm = getXFormMatrix();
  if ( update() )
    chkNotice();
}

void vsnMethod_sampler::resetXForm() {
  m_T = m_initT;
  m_S = m_initS;
  m_HPR = m_initHPR;
  updateXForm();
}


/* initialize XForm according to Data Bbox */

void vsnMethod_sampler::initXForm() {
  if ( ! p_refData ) return;
  if ( m_xformInited ) return;

  const Vec3<float>* dbb = p_refData->getBbox();
  m_initT = (dbb[1] + dbb[0]) * 0.5f;
  Vec3<float> bbLen = dbb[1] - dbb[0];
  float xLen = (bbLen[0] > bbLen[1]) ? bbLen[1] : bbLen[0];
  m_initS[0] = m_initS[1] = xLen * 0.5f; m_initS[2] = 1.f;
  m_initHPR = Vec3<float>(0.f,0.f,0.f);

  m_T = m_initT; m_S = m_initS; m_HPR = m_initHPR;
  // don't call updateXForm() here, that will call update() again...
  m_matXForm = getXFormMatrix();

  m_xformInited = true;
}


/* vsnMethodObj method */

bool vsnMethod_sampler::update(const bool force) {
  vsnDataObj* pData = p_refData;
  if ( ! pData ) return false;

  // initial-xform
  if ( ! m_xformInited )
    initXForm();

  // #of sample
  size_t sampleNum = m_sampleSize.x * m_sampleSize.y;
  if ( sampleNum < 1 ) {
    return false;
  }

  // verts myself
  alcVerts(sampleNum);
  vector3* vp = getVerts();
  if ( ! vp ) {
    ErrMsg(MsgERR, getMethodType() + string("[") + getName()
           + string("]: memory allocation failed"));
    return false;
  }

  register int i, j, k;
  Vec3<float> p0(0.f,0.f,0.f);
  Vec3<float> p1;
  const float dx = (m_sampleSize.x > 1) ? 1.f / (m_sampleSize.x -1) : 0.f;
  const float dy = (m_sampleSize.y > 1) ? 1.f / (m_sampleSize.y -1) : 0.f;

  for ( j = 0; j < m_sampleSize.y; j++ ) {
    p0[1] = (m_sampleSize.y == 1) ? 0.f : -0.5f + dy*j;
    for ( i = 0; i < m_sampleSize.x; i++ ) {
      p0[0] = (m_sampleSize.x == 1) ? 0.f : -0.5f + dx*i;
      p1 = m_matXForm * p0;
      k = m_sampleSize.x * j + i;
      vp[k][0] = p1[0];
      vp[k][1] = p1[1];
      vp[k][2] = p1[2];
    } // end of for(i)
  } // end of for(j)

  // update refMethods
  updateRefMethod();

  // allocate outline
  if ( ! m_outline ) {
    m_outline = new vfrLineStrip();
    if ( ! m_outline ||
	 ! m_outline->alcMaterial() || ! m_outline->alcVerts(4) ) {
      ErrMsg(MsgERR, getMethodType() + string("[") + getName()
             + string("]: memory allocation failed"));
      return false;
    }
    m_outline->setLoopMode(TRUE);
    addChild(m_outline);
  }
  m_outline->getPrivateMaterial()->setRenderMode(RT_NONE);
  if ( m_lineWidth > 0.f )
    m_outline->getPrivateMaterial()->setLineWidth(m_lineWidth);

  // allocate points
  if ( ! m_points ) {
    m_points = new vfrLineStrip();
    if ( ! m_points || ! m_points->alcMaterial() ) {
      ErrMsg(MsgERR, getMethodType() + string("[") + getName()
             + string("]: memory allocation failed"));
      return false;
    }
    m_points->setLoopMode(FALSE);
    addChild(m_points);
  }
  m_points->getPrivateMaterial()->setRenderMode(RT_NONE);
  if ( m_pointSize > 0.f )
    m_points->getPrivateMaterial()->setPointSize(m_pointSize);
  if ( ! m_points->alcVerts(sampleNum) ) {
    ErrMsg(MsgERR, getMethodType() + string("[") + getName()
	   + string("]: memory allocation failed"));
    return false;
  }

  // update UI (for use_mouse mode)
  updateUI();

  // method show mode check
  if ( ! m_show ) return true;

  // setup outline
  m_outline->setVert(0, vp[0]);
  m_outline->setVert(1, vp[m_sampleSize.x -1]);
  m_outline->setVert(2, vp[m_sampleSize.x * m_sampleSize.y -1]);
  m_outline->setVert(3, vp[m_sampleSize.x * (m_sampleSize.y -1)]);
  m_outline->generateBbox();
  m_outline->setColor4(0, m_colour);
  m_outline->setTransparency(m_antiAlias);

  // setup points
  m_points->setVerts(sampleNum, vp);
  m_points->setColor4(0, m_colour);
  m_points->setTransparency(m_antiAlias);

  // ok
  if ( m_lineWidth > 0.0f )
    m_outline->getPrivateMaterial()->setRenderMode(RT_WIRE);
  if ( m_pointSize > 0.0f )
    m_points->getPrivateMaterial()->setRenderMode(RT_POINT);
  return true;
}

vsnMethodPP* vsnMethod_sampler::getParamPanel(wxPanel* pp) {
  vsnMPP_sampler* pp_sampler = new vsnMPP_sampler(pp, this);
  if ( ! pp_sampler ) return NULL;
  return pp_sampler;
}


/* vsnIoObject methods */

bool vsnMethod_sampler::parseXML(xmlNodePtr xnp) {
  static xmlChar* xs;
  xmlNodePtr cur;
  Point2 div = getIniSampleNumber();

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

      if ( xsN == string("divM") ) {
	div.x = atoi(xsV.c_str());
	if ( div.x < 1 ) {
	  ErrMsg(MsgERR, msgHdr + string("invalid value in param divM"));
	  div.x = 8;
	  goto _NEXT_XML_NODE;
	}
      } // end of "divM"
      else if ( xsN == string("divN") ) {
	div.y = atoi(xsV.c_str());
	if ( div.y < 1 ) {
	  ErrMsg(MsgERR, msgHdr + string("invalid value in param divN"));
	  div.y = 1;
	  goto _NEXT_XML_NODE;
	}
      } // end of "divN"
      else if ( xsN == string("line_width") ) {
        float line_width = (float)atof(xsV.c_str());
        if ( ! setLineWidth(line_width) ) {
          ErrMsg(MsgERR, msgHdr + string("invalid value in param line_width"));
          goto _NEXT_XML_NODE;
        }
      } // end of "line_width"
      else if ( xsN == string("point_size") ) {
        float point_size = (float)atof(xsV.c_str());
        if ( ! setPointSize(point_size) ) {
          ErrMsg(MsgERR, msgHdr + string("invalid value in param point_size"));
          goto _NEXT_XML_NODE;
        }
      } // end of "point_size"
      else if ( xsN == string("antialias") ) {
	bool aam;
	if ( xsV == string("yes") ) aam = true;
	else if ( xsV == string("no") ) aam = false;
	else {
          ErrMsg(MsgERR, msgHdr + string("invalid value in param antialias"));
          goto _NEXT_XML_NODE;
        }
	if ( ! setAntiAliasMode(aam) ) {
	  ErrMsg(MsgERR, msgHdr + string("can't set antialias"));
          goto _NEXT_XML_NODE;
	}
      } // end of "antialias"
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

  // update parameters
  if ( ! setSampleNumber(div) ) {
    ErrMsg(MsgERR, msgHdr + string("set sampleNumber params failed"));
  }

  return true;
}

bool vsnMethod_sampler::outputXML(std::ostream& os, const size_t ts) {
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

  // div
  Point2 defaultSS = getIniSampleNumber();
  if ( m_sampleSize.x != defaultSS.x ) {
    os << idts_2 << "<param name=\"divM\" value=\""
       << m_sampleSize.x << "\" />" << endl;
  }
  if ( m_sampleSize.y != defaultSS.y ) {
    os << idts_2 << "<param name=\"divN\" value=\""
       << m_sampleSize.y << "\" />" << endl;
  }

  // line_width
  if ( m_lineWidth != getIniLineWidth() ) {
    os << idts_2 << "<param name=\"line_width\" value=\""
       << m_lineWidth << "\" />" << endl;
  }

  // point_size
  if ( m_pointSize != getIniPointSize() ) {
    os << idts_2 << "<param name=\"point_size\" value=\""
       << m_pointSize << "\" />" << endl;
  }

  // antialias
  if ( m_antiAlias ) {
    os << idts_2 << "<param name=\"antialias\" value=\"yes\" />" << endl;
  }

  os << idts << "</method>" << endl;

  return ret;
}

bool vsnMethod_sampler::commandXML(xmlNodePtr xnp) {
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
  else if ( valueStr.empty() ) {
    ErrMsg(MsgERR, msgHdr + "invalid command: " + nameStr + ": no value");
    return false;
  }

  if ( nameStr == "set_divM" ) {
    Point2 div = m_sampleSize;
    div.x = atoi(valueStr.c_str());
    if ( ! (div == m_sampleSize) ) {
      if ( ! setSampleNumber(div) ) {
	ErrMsg(MsgERR, msgHdr +
	       string("command set_divM: set failed: ") + valueStr);
	return false;
      }
    }
  } // end of "set_divM"
  else if ( nameStr == "set_divN" ) {
    Point2 div = m_sampleSize;
    div.y = atoi(valueStr.c_str());
    if ( ! (div == m_sampleSize) ) {
      if ( ! setSampleNumber(div) ) {
	ErrMsg(MsgERR, msgHdr +
	       string("command set_divN: set failed: ") + valueStr);
	return false;
      }
    }
  } // end of "set_divN"
  else if ( nameStr == "set_line_width" ) {
    float lw = (float)atof(valueStr.c_str());
    if ( ! setLineWidth(lw) ) {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_line_width: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_line_width"
  else if ( nameStr == "set_point_size" ) {
    float ps = (float)atof(valueStr.c_str());
    if ( ! setPointSize(ps) ) {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_point_size: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_point_size"
  else if ( nameStr == "set_antialias" ) {
    bool am;
    if ( valueStr == string("yes") ) am = true;
    else if ( valueStr == string("no") ) am = false;
    else {
      ErrMsg(MsgERR, msgHdr +
	     string("invalid command: set_antialias: invalid value"));
      return false;
    }
    if ( ! setAntiAliasMode(am) ) {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_antialias: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_antialias"
  else if ( nameStr == "export" ) {
    if ( ! exportFile(valueStr) ) {
      ErrMsg(MsgERR, msgHdr +
             string("command export: failed to export to file: ") + valueStr);
      return false;
    }
  } // end of "export"
  else {
    // not my command
    ErrMsg(MsgERR, msgHdr + string("unknown command: ") + nameStr);
    return false;
  }

  return true;
}


//----------------------------------------------------------------
// class vsnMethod_sliceSampler
//----------------------------------------------------------------

/* constructors /destructor */

vsnMethod_sliceSampler::vsnMethod_sliceSampler(const std::string& nm)
  : vsnMethod_sampler(nm)
{
  m_pointSize = getIniPointSize(); // 0.0: don't show sample points
  m_sampleSize = getIniSampleNumber();
}


/* methods */

void vsnMethod_sliceSampler::initXForm() {
  if ( ! p_refData ) return;
  if ( m_xformInited ) return;

  const Vec3<float>* dbb = p_refData->getBbox();
  m_initT = (dbb[1] + dbb[0]) * 0.5f;
  Vec3<float> bbLen = dbb[1] - dbb[0];
  m_initS[0] = bbLen[0]; m_initS[1] = bbLen[1]; m_initS[2] = 1.f;
  m_initHPR = Vec3<float>(0.f,0.f,0.f);

  m_T = m_initT; m_S = m_initS; m_HPR = m_initHPR;
  // don't call updateXForm() here, that will call update() again...
  m_matXForm = getXFormMatrix();

  m_xformInited = true;
}
