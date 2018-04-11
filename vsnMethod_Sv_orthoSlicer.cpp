//
// vsnMethod_Sv_orthoSlicer
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

#include "vsnMethod_Sv_orthoSlicer.h"
#include "vsnError.h"

using namespace std;
using namespace CES;
using namespace VSN;


//----------------------------------------------------------------
// class vsnMPP_Sv_orthoSlicer::OrthoSlicerKeyAct
//----------------------------------------------------------------
vsnMPP_Sv_orthoSlicer::OrthoSlicerKeyAct::
OrthoSlicerKeyAct(vsnGfxView* pgv, vsnMethod_Sv_orthoSlicer* pm)
  : vsnGfxAct_KeyIn(pgv), p_method(pm) {
}

void vsnMPP_Sv_orthoSlicer::OrthoSlicerKeyAct::
setOrthoSlicerMethod(vsnMethod_Sv_orthoSlicer* pm) {
  if ( p_method == pm ) return;
  p_method = pm;
}

void vsnMPP_Sv_orthoSlicer::OrthoSlicerKeyAct::execute(vfrEvent& e) {
  if ( ! p_method ) return;
  if ( ! e.isKeyEvent() ) return;

  int oplane = p_method->getSlicePlane();
  switch ( e.getKey() ) {
  case VFRKC_LEFT: case VFRKC_DOWN:
    if ( p_method->setSliceParam(p_method->getSliceAxis(), oplane -1) )
      p_method->chkNotice();
    break;
  case VFRKC_RIGHT: case VFRKC_UP:
    if ( p_method->setSliceParam(p_method->getSliceAxis(), oplane +1) )
      p_method->chkNotice();
    break;
  default:
    vsnGfxAct_KeyIn::execute(e);
  } // end of switch(kc)
}


//----------------------------------------------------------------
// class vsnMPP_Sv_orthoSlicer
//----------------------------------------------------------------

BEGIN_EVENT_TABLE(vsnMPP_Sv_orthoSlicer, wxPanel)
  EVT_RADIOBOX(MPP_Sv_orthoSlicer_Axis,
	       vsnMPP_Sv_orthoSlicer::OnSelAxisRadio)
  EVT_COMMAND_SCROLL(MPP_Sv_orthoSlicer_PlaneSld,
		     vsnMPP_Sv_orthoSlicer::OnPlaneSlider)
  EVT_TEXT_ENTER(MPP_Sv_orthoSlicer_PlaneTxt,
		 vsnMPP_Sv_orthoSlicer::OnPlaneTxt)
  EVT_TEXT_ENTER(MPP_Sv_orthoSlicer_LineWidthTxt,
		 vsnMPP_Sv_orthoSlicer::OnLineWidthTxt)
  EVT_CHECKBOX(MPP_Sv_orthoSlicer_ShowGridChk,
	       vsnMPP_Sv_orthoSlicer::OnShowGridChk)
  EVT_CHECKBOX(MPP_Sv_orthoSlicer_AntiAliasChk,
	       vsnMPP_Sv_orthoSlicer::OnAntiAliasChk)
  EVT_BUTTON(MPP_Sv_orthoSlicer_LeftBtn,
	     vsnMPP_Sv_orthoSlicer::OnLeftBtn)
  EVT_BUTTON(MPP_Sv_orthoSlicer_RightBtn,
	     vsnMPP_Sv_orthoSlicer::OnRightBtn)
  EVT_CHAR(vsnMPP_Sv_orthoSlicer::OnChar)
END_EVENT_TABLE()


/* constructors / destructor */

vsnMPP_Sv_orthoSlicer::vsnMPP_Sv_orthoSlicer(wxPanel* parent, vsnMethodObj* pm)
  : vsnMethodPP(parent, pm),
    m_pAxisRadio(NULL), m_pPlaneSlider(NULL), m_pPlaneTxt(NULL),
    m_pLineWidthTxt(NULL), m_pShowGridChk(NULL), m_pAntiAliasChk(NULL)
{
  assert(parent);
  vsnMethod_Sv_orthoSlicer* posm = dynamic_cast<vsnMethod_Sv_orthoSlicer*>(pm);
  assert(posm);

  // setup gfxAct
  gfxAct_KI.setOrthoSlicerMethod(posm);
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
	  pgv->pushActions(this);
	  pgv->setStandardActions();
	  vfrEvKeyIn::instance(dispatcher).regist(&gfxAct_KI);
	} // end of if(pda)
      } // end of if(pgv)
    } // end of if(pvf)
  } // end of if(puvi)


  // widgets layout
  wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer* sizerH;

  // axis
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxALL, 0);
  wxString ritems[] = {wxString(wxT("I ")),
                       wxString(wxT("J ")),
                       wxString(wxT("K "))};
  m_pAxisRadio = new wxRadioBox(this, MPP_Sv_orthoSlicer_Axis,
				wxT("slice axis"),
				wxDefaultPosition, wxDefaultSize,
				3, ritems, 1, wxRA_SPECIFY_ROWS);
  sizerH->Add(m_pAxisRadio, 1, wxEXPAND|wxALL, 3);

  // plane
  topsizer->Add(new wxStaticText(this, -1, wxT("slice plane")),
		0, wxALIGN_LEFT|wxALL, 3);
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxALL, 0);

  m_pPlaneSlider = new wxSlider(this, MPP_Sv_orthoSlicer_PlaneSld,
				20000 /*val*/, 10000 /*min*/, 30000 /*max*/,
				wxDefaultPosition, wxSize(120, -1),
				wxSL_HORIZONTAL|wxSL_LABELS);
  sizerH->Add(m_pPlaneSlider, 1, wxALIGN_RIGHT|wxALL, 3);
  m_pPlaneTxt = new wxTextCtrl(this, MPP_Sv_orthoSlicer_PlaneTxt, wxT(""),
			       wxDefaultPosition, wxSize(60,-1),
			       wxTE_PROCESS_ENTER);
  sizerH->Add(m_pPlaneTxt, 0, wxEXPAND|wxALL, 3);

  // plane adjust buttons
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxALL, 0);
  m_pLeftBtn = new wxButton(this, MPP_Sv_orthoSlicer_LeftBtn, wxT("<"),
			    wxDefaultPosition, wxSize(60,-1), wxBU_EXACTFIT);
  sizerH->Add(m_pLeftBtn, 1, wxEXPAND|wxALL, 3);
  m_pRightBtn = new wxButton(this, MPP_Sv_orthoSlicer_RightBtn, wxT(">"),
			     wxDefaultPosition, wxSize(60,-1), wxBU_EXACTFIT);
  sizerH->Add(m_pRightBtn, 1, wxEXPAND|wxALL, 3);

  // line width
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxALL, 0);
  sizerH->Add(new wxStaticText(this, -1, wxT("line width")),
	      0, wxALIGN_LEFT|wxALL, 3);
  m_pLineWidthTxt = new wxTextCtrl(this, MPP_Sv_orthoSlicer_LineWidthTxt,
				   wxT(""), wxDefaultPosition, wxDefaultSize,
				   wxTE_PROCESS_ENTER);
  sizerH->Add(m_pLineWidthTxt, 0, wxALIGN_RIGHT|wxALL, 3);

  // show grid
  m_pShowGridChk = new wxCheckBox(this, MPP_Sv_orthoSlicer_ShowGridChk,
				  wxT("show grid line"));	  
  topsizer->Add(m_pShowGridChk, 0, wxALL, 3);

  // anti-alias
  m_pAntiAliasChk = new wxCheckBox(this, MPP_Sv_orthoSlicer_AntiAliasChk,
				  wxT("anti-alias line"));	  
  topsizer->Add(m_pAntiAliasChk, 0, wxALL, 3);

  // post process
  SetSizer(topsizer);
  addTo(parent);
  topsizer->Layout();
  (void)update();
}

vsnMPP_Sv_orthoSlicer::~vsnMPP_Sv_orthoSlicer() {
  settlement();
}


/* interface */

bool vsnMPP_Sv_orthoSlicer::update() {
  if ( ! m_pAxisRadio || ! m_pPlaneSlider || ! m_pPlaneTxt ||
       ! m_pLineWidthTxt || ! m_pShowGridChk || ! m_pAntiAliasChk )
    return false;
  int val; char txt[64];

  vsnMethod_Sv_orthoSlicer* pm
    = dynamic_cast<vsnMethod_Sv_orthoSlicer*>(p_method);
  if ( ! pm ) return false;
  vsnData_Sv* pdata = dynamic_cast<vsnData_Sv*>(pm->getRefData());
  if ( ! pdata ) return false;

  val = pm->getSliceAxis();
  m_pAxisRadio->SetSelection(val);

  Vec3<size_t> dims = pdata->getDims();
  m_pPlaneSlider->SetRange(0, dims[(size_t)val]-1);

  val = pm->getSlicePlane();
  m_pPlaneSlider->SetValue(val);
  sprintf(txt, "%d", val);
  m_pPlaneTxt->SetValue(vsnApp::ConvSysToWx(txt));

  sprintf(txt, "%g", pm->getLineWidth());
  m_pLineWidthTxt->SetValue(vsnApp::ConvSysToWx(txt));

  m_pShowGridChk->SetValue(pm->getShowGrid());

  m_pAntiAliasChk->SetValue(pm->getAntiAliasMode());

  return true;
}

void vsnMPP_Sv_orthoSlicer::settlement() {
  vsnGfxView* pgv = gfxAct_KI.getGfxView();
  if ( pgv ) {
    pgv->popActions(this);
    gfxAct_KI.setGfxView(NULL);
  }
}


/* event handler */

void vsnMPP_Sv_orthoSlicer::OnSelAxisRadio(wxCommandEvent& event) {
  if ( ! m_pAxisRadio || ! m_pPlaneSlider || ! m_pPlaneTxt ) return;
  vsnMethod_Sv_orthoSlicer* pm
    = dynamic_cast<vsnMethod_Sv_orthoSlicer*>(p_method);
  if ( ! pm ) return;

  int val = m_pAxisRadio->GetSelection();
  int oaxis = pm->getSliceAxis();
  if ( val == oaxis ) return;

  vsnData_Sv* pdata = dynamic_cast<vsnData_Sv*>(pm->getRefData());
  if ( pdata ) {
    Vec3<size_t> dims = pdata->getDims();
    m_pPlaneSlider->SetRange(0, dims[(size_t)val]-1);
    m_pPlaneSlider->SetValue(dims[(size_t)val]/2);

    char txt[64]; sprintf(txt, "%d", m_pPlaneSlider->GetValue());
    m_pPlaneTxt->SetValue(vsnApp::ConvSysToWx(txt));
  }

  if ( pm->setSliceParam(val, m_pPlaneSlider->GetValue()) )
    pm->chkNotice();
}

void vsnMPP_Sv_orthoSlicer::OnPlaneSlider(wxScrollEvent& event) {
  if ( ! m_pPlaneSlider ) return;
  vsnMethod_Sv_orthoSlicer* pm
    = dynamic_cast<vsnMethod_Sv_orthoSlicer*>(p_method);
  if ( ! pm ) return;

  int val = m_pPlaneSlider->GetValue();
  int oplane = pm->getSlicePlane();
  if ( val == oplane ) return;

  if ( pm->setSliceParam(pm->getSliceAxis(), val) )
    pm->chkNotice();
}

void vsnMPP_Sv_orthoSlicer::OnPlaneTxt(wxCommandEvent& event) {
  if ( ! m_pPlaneTxt ) return;
  vsnMethod_Sv_orthoSlicer* pm
    = dynamic_cast<vsnMethod_Sv_orthoSlicer*>(p_method);
  if ( ! pm ) return;

  wxString valStr = m_pPlaneTxt->GetValue();
  if ( valStr.IsEmpty() ) return;
  int val = atoi(vsnApp::ConvWxToSys(valStr).c_str());
  if ( val < 0 || val > m_pPlaneSlider->GetMax() )
    return;

  if ( pm->setSliceParam(pm->getSliceAxis(), val) )
    pm->chkNotice();
}

void vsnMPP_Sv_orthoSlicer::OnLineWidthTxt(wxCommandEvent& event) {
  if ( ! m_pLineWidthTxt ) return;
  vsnMethod_Sv_orthoSlicer* pm
    = dynamic_cast<vsnMethod_Sv_orthoSlicer*>(p_method);
  if ( ! pm ) return;

  wxString valStr = m_pLineWidthTxt->GetValue();
  if ( valStr.IsEmpty() ) return;
  float value = (float)atof(vsnApp::ConvWxToSys(valStr).c_str());

  if ( pm->setLineWidth(value) )
    pm->chkNotice();
}

void vsnMPP_Sv_orthoSlicer::OnShowGridChk(wxCommandEvent& event) {
  if ( ! m_pShowGridChk ) return;
  vsnMethod_Sv_orthoSlicer* pm
    = dynamic_cast<vsnMethod_Sv_orthoSlicer*>(p_method);
  if ( ! pm ) return;

  bool value = m_pShowGridChk->GetValue();
  if ( pm->setShowGrid(value) )
    pm->chkNotice();
}

void vsnMPP_Sv_orthoSlicer::OnAntiAliasChk(wxCommandEvent& event) {
  if ( ! m_pAntiAliasChk ) return;
  vsnMethod_Sv_orthoSlicer* pm
    = dynamic_cast<vsnMethod_Sv_orthoSlicer*>(p_method);
  if ( ! pm ) return;

  bool value = m_pAntiAliasChk->GetValue();
  if ( pm->setAntiAliasMode(value) )
    pm->chkNotice();
}

void vsnMPP_Sv_orthoSlicer::OnLeftBtn(wxCommandEvent& event) {
  vsnMethod_Sv_orthoSlicer* pm
    = dynamic_cast<vsnMethod_Sv_orthoSlicer*>(p_method);
  if ( ! pm ) return;

  int oplane = pm->getSlicePlane();
  if ( pm->setSliceParam(pm->getSliceAxis(), oplane -1) )
    pm->chkNotice();
}

void vsnMPP_Sv_orthoSlicer::OnRightBtn(wxCommandEvent& event) {
  vsnMethod_Sv_orthoSlicer* pm
    = dynamic_cast<vsnMethod_Sv_orthoSlicer*>(p_method);
  if ( ! pm ) return;

  int oplane = pm->getSlicePlane();
  if ( pm->setSliceParam(pm->getSliceAxis(), oplane +1) )
    pm->chkNotice();
}

void vsnMPP_Sv_orthoSlicer::OnChar(wxKeyEvent& event) {
  // the codes below does not works good on Windows|MacOSX platform
  // (because of Windows' key-acceleraton and Cocoa's TextCtrl focus)
  wxCommandEvent ce;
  switch ( event.GetKeyCode() ) {
  case WXK_LEFT: case WXK_DOWN:
    OnLeftBtn(ce);
    break;
  case WXK_RIGHT: case WXK_UP:
    OnRightBtn(ce);
    break;
  default:
    ;
  }
}


//----------------------------------------------------------------
// class vsnRefOrthoSlicerKeyAct
//----------------------------------------------------------------
vsnRefOrthoSlicerKeyAct::
vsnRefOrthoSlicerKeyAct(vsnGfxView* pgv, vsnRef_Sv_orthoSlicer* prosr)
  : vsnGfxAct_KeyIn(pgv), p_rosr(prosr) {
}

void vsnRefOrthoSlicerKeyAct::setRefOrthoSlicer(vsnRef_Sv_orthoSlicer* prosr) {
  if ( p_rosr == prosr ) return;
  p_rosr = prosr;
}

void vsnRefOrthoSlicerKeyAct::execute(vfrEvent& e) {
  if ( ! p_rosr ) return;
  if ( ! e.isKeyEvent() ) return;

  vsnMethod_Sv_orthoSlicer* posr = p_rosr->getOrthoSlicer();
  if ( ! posr ) return;

  int oplane = posr->getSlicePlane();
  switch ( e.getKey() ) {
  case VFRKC_LEFT: case VFRKC_DOWN:
    if ( posr->setSliceParam(posr->getSliceAxis(), oplane -1) )
      posr->chkNotice();
    break;
  case VFRKC_RIGHT: case VFRKC_UP:
    if ( posr->setSliceParam(posr->getSliceAxis(), oplane +1) )
      posr->chkNotice();
    break;
  default:
    vsnGfxAct_KeyIn::execute(e);
  } // end of switch(kc)
}


//----------------------------------------------------------------
// class vsnRef_Sv_orthoSlicer
//----------------------------------------------------------------

vsnRef_Sv_orthoSlicer::~vsnRef_Sv_orthoSlicer() {
  if ( p_osr ) p_osr->removeRefMethod(this);
}


deque<vsnMethod_Sv_orthoSlicer*>
vsnRef_Sv_orthoSlicer::getOrthoSlicerList(vsnData_Sv* pdo) {
  deque<vsnMethod_Sv_orthoSlicer*> retLst;
  if ( ! pdo ) return retLst;

  vsnMethodObj* pmo = pdo->getNextMethod(NULL);
  while ( pmo ) {
    vsnMethod_Sv_orthoSlicer* pmo_os
      = dynamic_cast<vsnMethod_Sv_orthoSlicer*>(pmo);
    if ( pmo_os ) retLst.push_back(pmo_os);

    pmo = pdo->getNextMethod(pmo);
  } // end of while(pmo)

  return retLst;
}


void
vsnRef_Sv_orthoSlicer::setOrthoSlicer(vsnMethod_Sv_orthoSlicer* osr) {
  if ( osr == p_osr ) return;
  if ( p_osr ) p_osr->removeRefMethod(this);
  p_osr = osr;
  if ( p_osr ) p_osr->appendRefMethod(this);

  // adjust method order: orthoSlice must be ordered before this(for serialize)
  vsnDataObj* pdo = NULL;
  vsnMethodObj* pmo = dynamic_cast<vsnMethodObj*>(this);
  if ( p_osr ) pdo = p_osr->getRefData();
  if ( pdo && pmo ) pdo->adjustMethodOrder(p_osr, pmo);

  noticeUpdate();
}

void
vsnRef_Sv_orthoSlicer::setOrthoSlicer(vsnData_Sv* pdo, const int n) {
  if ( ! pdo ) return;
  if ( n < 0 ) {
    setOrthoSlicer(NULL);
    return;
  }
  deque<vsnMethod_Sv_orthoSlicer*> oslst = getOrthoSlicerList(pdo);
  if ( n >= oslst.size() ) {
    setOrthoSlicer(NULL);
    return;
  }
  setOrthoSlicer(oslst[n]);
}

bool vsnRef_Sv_orthoSlicer::setOrthoSlicer(vsnData_Sv* pdo,
					   const string& slicerName) {
  if ( slicerName.empty() || slicerName == string(VFR_NONAME) )
    return false;
  if ( ! pdo ) return false;

  deque<vsnMethod_Sv_orthoSlicer*> osLst = getOrthoSlicerList(pdo);
  for ( int i = 0; i < osLst.size(); i++ ) {
    if ( ! osLst[i] ) continue;
    if ( slicerName == osLst[i]->getName() ) {
      this->vsnRef_Sv_orthoSlicer::setOrthoSlicer(osLst[i]);
      return (p_osr != NULL);
    }
  } // end of for(i)
  return false;
}


void vsnRef_Sv_orthoSlicer::noticeDie() {
  if ( p_osr ) p_osr = NULL;
  noticeUpdate();
}


// XML util
bool vsnRef_Sv_orthoSlicer::exportXMLNode(std::ostream& os,
					  const size_t ts) const {
  std::string idts;
  for ( int i = 0; i < ts; i++ ) idts.push_back(' ');

  if ( p_osr ) {
    string osrn = p_osr->getName();
    if ( ! osrn.empty() && osrn != VFR_NONAME ) {
      os << idts << "<param name=\"ortho_slicer\" value=\""
	 << osrn << "\" />" << endl;
    } else {
      return false;
    }
  }
  return true;
}


//----------------------------------------------------------------
// class vsnMethod_Sv_orthoSlicer
//----------------------------------------------------------------

/* constructors / destructor */

vsnMethod_Sv_orthoSlicer::vsnMethod_Sv_orthoSlicer(const string& name)
  : vsnMethodObj(name),
    m_sliceAxis(2), m_slicePlane(0), m_lineWidth(1.f), m_showGrid(false),
    m_outline(NULL), m_grid(NULL)
{
}

vsnMethod_Sv_orthoSlicer::~vsnMethod_Sv_orthoSlicer() {
  if ( m_outline )
    delete m_outline;

  if ( m_grid )
    delete m_grid;

  set<vsnRef_Sv_orthoSlicer*>::iterator it;
  for ( it = m_refMethodLst.begin(); it != m_refMethodLst.end(); it++ ) {
    if ( *it ) (*it)->noticeDie();
  } // end of for(it)
}


/* refMethodLst control */
void
vsnMethod_Sv_orthoSlicer::appendRefMethod(vsnRef_Sv_orthoSlicer* refm)
{
  m_refMethodLst.insert(refm);
}

void
vsnMethod_Sv_orthoSlicer::removeRefMethod(vsnRef_Sv_orthoSlicer* refm)
{
  set<vsnRef_Sv_orthoSlicer*>::iterator it = m_refMethodLst.find(refm);
  if ( it == m_refMethodLst.end() ) return;
  m_refMethodLst.erase(it);
}

void vsnMethod_Sv_orthoSlicer::updateRefMethod() {
  set<vsnRef_Sv_orthoSlicer*>::iterator it;
  for ( it = m_refMethodLst.begin(); it != m_refMethodLst.end(); it++ ) {
    if ( *it ) (*it)->noticeUpdate();
  } // end of for(it)
}


/* vsnMethodObj method */

bool vsnMethod_Sv_orthoSlicer::update(const bool force) {
  return updateStep(m_requestedStp, force);
}

vsnMethodPP* vsnMethod_Sv_orthoSlicer::getParamPanel(wxPanel* pp) {
  vsnMPP_Sv_orthoSlicer* pp_orthoSlicer
    = new vsnMPP_Sv_orthoSlicer(pp, this);
  if ( ! pp_orthoSlicer ) return NULL;
  return pp_orthoSlicer;
}


/* methods */

bool vsnMethod_Sv_orthoSlicer::setSliceParam(const int sa, const int sp) {
  if ( sa == m_sliceAxis && sp == m_slicePlane )
    return true;

  vsnData_Sv* pData = dynamic_cast<vsnData_Sv*>(p_refData);
  if ( ! pData ) return false;
  Vec3<size_t> dims = pData->getDims();
  
  if ( sa < 0 || sa > 2 ) return false;
  if ( sp < 0 ) return false;

  m_sliceAxis = sa;
  m_slicePlane = sp;
  if ( m_slicePlane >= dims[sa] ) m_slicePlane = dims[sa] -1;

  switch ( m_sliceAxis ) {
  case 0: // YZ
    m_sliceSize.x = dims[1]; m_sliceSize.y = dims[2]; break;
  case 1: // ZX
    m_sliceSize.x = dims[2]; m_sliceSize.y = dims[0]; break;
  case 2: // XY
    m_sliceSize.x = dims[0]; m_sliceSize.y = dims[1]; break;
  } // end of switch(m_sliceAxis)

  if ( ! update() ) return false;
  updateUI();
  return true;
}

bool vsnMethod_Sv_orthoSlicer::setLineWidth(const float lw) {
  if ( m_lineWidth == lw ) return true;
  if ( lw <= 0.f ) return false;

  m_lineWidth = lw;

  if ( m_outline )
    m_outline->getPrivateMaterial()->setLineWidth(m_lineWidth);
  if ( m_grid )
    m_grid->getPrivateMaterial()->setLineWidth(m_lineWidth);

  updateUI();
  return true;
}

bool vsnMethod_Sv_orthoSlicer::setShowGrid(const bool sgm) {
  if ( m_showGrid == sgm ) return true;
  m_showGrid = sgm;

  if ( m_grid ) {
    RenderType grt = RT_NONE;
    if ( m_showGrid ) grt = RT_WIRE;
    m_grid->getPrivateMaterial()->setRenderMode(grt);
  }

  // we don't want to casscade-update, so don't call update()
  if ( ! updateStep(m_requestedStp, true, false) ) {
    return false;
  }

  updateUI();
  return true;
}


/* vsnTimeSeriesMethodIF method */

#define IDX1(d,i,j,k) ((k)*(d)[0]*(d)[1] +(j)*(d)[0] +(i))
#define IDX3(d,i,j,k) (3*((k)*(d)[0]*(d)[1] +(j)*(d)[0] +(i)))

bool vsnMethod_Sv_orthoSlicer::updateStep(const int stp,
					  const bool force, const bool cascade)
{
  vsnData_Sv* pData = dynamic_cast<vsnData_Sv*>(p_refData);
  if ( ! pData ) return false;
  m_requestedStp = stp;
  if ( m_requestedStp < 0 )
    m_requestedStp = pData->getCurrentStepIdx();

  // slice size
  Vec3<size_t> dims = pData->getDims();
  size_t slSz = m_sliceSize.x * m_sliceSize.y;
  if ( slSz < 1 ) {
    m_slicePlane = dims[m_sliceAxis] / 2;
    m_sliceSize.x = dims[(m_sliceAxis+1)%3];
    m_sliceSize.y = dims[(m_sliceAxis+2)%3];
    slSz = m_sliceSize.x * m_sliceSize.y;
    m_updatedStp = -1; // force
  }
  if ( slSz < 4 ) {
    m_updatedStp = -1;
    return false;
  }

  if ( force ) m_updatedStp = -1;
  if ( m_updatedStp == m_requestedStp ) return true;
  m_updatedStp = -1;

  // indices
  alcIndices(slSz);
  int* il = getIndices();
  if ( ! il ) {
    ErrMsg(MsgERR, getMethodType() + string("[") + getName()
	   + string("]: memory allocation failed"));
    return false;
  }

  register int i, j, k, idx;
  idx = 0;
  switch ( m_sliceAxis ) {
  case 0: // YZ
    i = m_slicePlane;
    for ( k = 0; k < m_sliceSize.y; k++ )
      for ( j = 0; j < m_sliceSize.x; j++ )
	il[idx++] = IDX1(dims, i, j, k);
    break;
  case 1: // ZX
    j = m_slicePlane;
    for ( i = 0; i < m_sliceSize.y; i++ )
      for ( k = 0; k < m_sliceSize.x; k++ )
	il[idx++] = IDX1(dims, i, j, k);
    break;
  case 2: // XY
    k = m_slicePlane;
    for ( j = 0; j < m_sliceSize.y; j++ )
      for ( i = 0; i < m_sliceSize.x; i++ )
	il[idx++] = IDX1(dims, i, j, k);
    break;
  } // end of switch(m_sliceAxis)

  // update refMethods
  if ( cascade )
    updateRefMethod();

  // outline
  if ( ! m_outline ) {
    m_outline = new vfrLineStrip();
    if ( ! m_outline ) {
      ErrMsg(MsgERR, getMethodType() + string("[") + getName()
	     + string("]: memory allocation failed"));
      return false;
    }
    m_outline->alcMaterial();
    m_outline->setLoopMode(FALSE);
    addChild(m_outline);
  }
  m_outline->getPrivateMaterial()->setRenderMode(RT_NONE);

  // grid
  if ( ! m_grid ) {
    m_grid = new vfrMesh2D();
    if ( ! m_grid ) {
      ErrMsg(MsgERR, getMethodType() + string("[") + getName()
             + string("]: memory allocation failed"));
      return false;
    }
    m_grid->alcMaterial();
    addChild(m_grid);
  }
  m_grid->getPrivateMaterial()->setRenderMode(RT_NONE);

  // method show mode check
  if ( ! m_show ) return true;

  // get grid data
  const float* gptr = pData->getCoord(m_requestedStp);
  if ( ! gptr ) {
    return false;
  }

  // alloc verts of outline
  m_outline->alcVerts(m_sliceSize.x *2 + m_sliceSize.y *2);
  vector3* vl = m_outline->getVerts();
  if ( ! vl ) {
    ErrMsg(MsgERR, getMethodType() + string("[") + getName()
	   + string("]: memory allocation failed"));
    return false;
  }

  // setup outline
  register int i3;
  idx = 0;
  switch ( m_sliceAxis ) {
  case 0: // YZ : jk
    i = m_slicePlane;
    // +X0
    for ( k = 0, j = 0; j < m_sliceSize.x; j++ ) {
      i3 = IDX3(dims, i,j,k);
      vl[idx][0] = gptr[i3++];
      vl[idx][1] = gptr[i3++];
      vl[idx][2] = gptr[i3++]; idx++;
    }
    // +Y1
    for ( j = m_sliceSize.x -1, k = 0; k < m_sliceSize.y; k++ ) {
      i3 = IDX3(dims, i,j,k);
      vl[idx][0] = gptr[i3++];
      vl[idx][1] = gptr[i3++];
      vl[idx][2] = gptr[i3++]; idx++;
    }
    // -X1
    for ( k = m_sliceSize.y -1, j = m_sliceSize.x -1; j >= 0; j-- ) {
      i3 = IDX3(dims, i,j,k);
      vl[idx][0] = gptr[i3++];
      vl[idx][1] = gptr[i3++];
      vl[idx][2] = gptr[i3++]; idx++;
    }
    // -Y0
    for ( j = 0, k = m_sliceSize.y -1; k >= 0; k-- ) {
      i3 = IDX3(dims, i,j,k);
      vl[idx][0] = gptr[i3++];
      vl[idx][1] = gptr[i3++];
      vl[idx][2] = gptr[i3++]; idx++;
    }
    break;
  case 1: // ZX
    j = m_slicePlane;
    // +X0
    for ( i = 0, k = 0; k < m_sliceSize.x; k++ ) {
      i3 = IDX3(dims, i,j,k);
      vl[idx][0] = gptr[i3++];
      vl[idx][1] = gptr[i3++];
      vl[idx][2] = gptr[i3++]; idx++;
    }
    // +Y1
    for ( k = m_sliceSize.x -1, i = 0; i < m_sliceSize.y; i++ ) {
      i3 = IDX3(dims, i,j,k);
      vl[idx][0] = gptr[i3++];
      vl[idx][1] = gptr[i3++];
      vl[idx][2] = gptr[i3++]; idx++;
    }
    // -X1
    for ( i = m_sliceSize.y -1, k = m_sliceSize.x -1; k >= 0; k-- ) {
      i3 = IDX3(dims, i,j,k);
      vl[idx][0] = gptr[i3++];
      vl[idx][1] = gptr[i3++];
      vl[idx][2] = gptr[i3++]; idx++;
    }
    // -Y0
    for ( k = 0, i = m_sliceSize.y -1; i >= 0; i-- ) {
      i3 = IDX3(dims, i,j,k);
      vl[idx][0] = gptr[i3++];
      vl[idx][1] = gptr[i3++];
      vl[idx][2] = gptr[i3++]; idx++;
    }
    break;
  case 2: // XY
    k = m_slicePlane;
    // +X0
    for ( j = 0, i = 0; i < m_sliceSize.x; i++ ) {
      i3 = IDX3(dims, i,j,k);
      vl[idx][0] = gptr[i3++];
      vl[idx][1] = gptr[i3++];
      vl[idx][2] = gptr[i3++]; idx++;
    }
    // +Y1
    for ( i = m_sliceSize.x -1, j = 0; j < m_sliceSize.y; j++ ) {
      i3 = IDX3(dims, i,j,k);
      vl[idx][0] = gptr[i3++];
      vl[idx][1] = gptr[i3++];
      vl[idx][2] = gptr[i3++]; idx++;
    }
    // -X1
    for ( j = m_sliceSize.y -1, i = m_sliceSize.x -1; i >= 0; i-- ) {
      i3 = IDX3(dims, i,j,k);
      vl[idx][0] = gptr[i3++];
      vl[idx][1] = gptr[i3++];
      vl[idx][2] = gptr[i3++]; idx++;
    }
    // -Y0
    for ( i = 0, j = m_sliceSize.y -1; j >= 0; j-- ) {
      i3 = IDX3(dims, i,j,k);
      vl[idx][0] = gptr[i3++];
      vl[idx][1] = gptr[i3++];
      vl[idx][2] = gptr[i3++]; idx++;
    }
    break;
  } // end of switch(m_sliceAxis)
  m_outline->generateBbox();
  m_outline->setColor4(0, m_colour);
  m_outline->setTransparency(m_antiAlias);

  // grid lines
  if ( m_showGrid ) {
    if ( ! m_grid->setMeshSize(m_sliceSize) ) {
      return false;
    }

    size_t sliceSz = m_sliceSize.x * m_sliceSize.y;
    vector3* vl = m_grid->getVerts();
    for ( i = 0; i < sliceSz; i++ ) {
      idx = il[i] * 3;
      vl[i][0] = gptr[idx   ];
      vl[i][1] = gptr[idx +1];
      vl[i][2] = gptr[idx +2];
    } // end of for(i)
    m_grid->generateBbox();
    m_grid->setColor4(0, m_colour);
    m_grid->setTransparency(m_antiAlias);

    m_grid->getPrivateMaterial()->setRenderMode(RT_WIRE);
  } // end of if(m_showGrid)

  // ok
  m_outline->getPrivateMaterial()->setRenderMode(RT_WIRE);
  m_updatedStp = m_requestedStp;
  return true;
}


/* serialize methods */

bool vsnMethod_Sv_orthoSlicer::parseXML(xmlNodePtr xnp) {
  static xmlChar* xs;
  xmlNodePtr cur;
  int axis(-1), plane(-1);

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
	if ( xsV == "0" || xsV == "i" || xsV == "I" ||
	     xsV == "x" || xsV == "X" ) axis = 0;
	else if ( xsV == "1" || xsV == "j" || xsV == "J" ||
		  xsV == "y" || xsV == "Y" ) axis = 1;
	else if ( xsV == "2" || xsV == "k" || xsV == "K" ||
		  xsV == "z" || xsV == "Z" ) axis = 2;
	else {
	  ErrMsg(MsgERR, msgHdr + string("invalid value in param axis"));
	  axis = -1;
	  goto _NEXT_XML_NODE;
	}
      } // end of "axis"
      else if ( xsN == string("plane") ) {
	plane = atoi(xsV.c_str());
	if ( plane < 0 ) {
	  ErrMsg(MsgERR, msgHdr + string("invalid value in param plane"));
	  plane = -1;
	  goto _NEXT_XML_NODE;
	}
      } // end of "plane"
      else if ( xsN == string("line_width") ) {
	float lw = (float)atof(xsV.c_str());
	if ( ! setLineWidth(lw) ) {
	  ErrMsg(MsgERR, msgHdr + string("invalid value in param line_width"));
	  goto _NEXT_XML_NODE;
	}
      } // end of "line_width"
      else if ( xsN == string("show_grid") ) {
	bool gsm;
	if ( xsV == string("yes") ) gsm = true;
        else if ( xsV == string("no") ) gsm = false;
        else {
          ErrMsg(MsgERR, msgHdr + string("invalid value in param show_grid"));
          goto _NEXT_XML_NODE;
        }
	if ( ! setShowGrid(gsm) ) {
	  ErrMsg(MsgERR, msgHdr + string("failed to set show_grid"));
          goto _NEXT_XML_NODE;
	}
      } // end of "show_grid"
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

  _NEXT_XML_NODE:
    cur = cur->next;
  } // end of while(cur)

  // update slice parameters
  vsnData_Sv* pData = dynamic_cast<vsnData_Sv*>(p_refData);
  if ( pData ) {
    Vec3<size_t> dims = pData->getDims();
    if ( axis < 0 || plane < 0 ) {
      if ( axis < 0 ) axis = 2; // Z
      if ( plane < 0 ) plane = dims[axis] / 2;
    }
    if ( plane >= dims[axis] ) {
      ErrMsg(MsgERR, msgHdr + string("param plane is out of range"));
      plane = dims[axis] / 2;
    }

    if ( ! setSliceParam(axis, plane) ) {
      ErrMsg(MsgERR, msgHdr + string("set slice params failed"));
      return false;
    }
  }

  return true;
}

bool vsnMethod_Sv_orthoSlicer::outputXML(std::ostream& os, const size_t ts)
{
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

  // plane
  vsnData_Sv* pData = dynamic_cast<vsnData_Sv*>(p_refData);
  if ( pData ) {
    Vec3<size_t> dims = pData->getDims();
    if ( m_slicePlane != dims[m_sliceAxis] / 2 ) {
      os << idts_2 << "<param name=\"plane\" value=\""
	 << m_slicePlane << "\" />" << endl;
    }
  }

  // line_width
  if ( m_lineWidth != 1.f ) {
    os << idts_2 << "<param name=\"line_width\" value=\""
       << m_lineWidth << "\" />" << endl;
  }

  // show_grid
  if ( m_showGrid ) {
    os << idts_2 << "<param name=\"show_grid\" value=\"yes\" />" << endl;
  }

  // antialias
  if ( m_antiAlias ) {
    os << idts_2 << "<param name=\"antialias\" value=\"yes\" />" << endl;
  }

  os << idts << "</method>" << endl;

  return ret;
}

bool vsnMethod_Sv_orthoSlicer::commandXML(xmlNodePtr xnp) {
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
    int axis = -1, plane = -1;
    if ( valueStr == "0" || valueStr == "i" || valueStr == "I" ||
	 valueStr == "x" || valueStr == "X" ) axis = 0;
    else if ( valueStr == "1" || valueStr == "j" || valueStr == "J" ||
	      valueStr == "y" || valueStr == "Y" ) axis = 1;
    else if ( valueStr == "2" || valueStr == "k" || valueStr == "K" ||
	      valueStr == "z" || valueStr == "Z" ) axis = 2;
    else {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_axis: invalid value") + valueStr);
      return false;
    }
    vsnData_Sv* pData = dynamic_cast<vsnData_Sv*>(p_refData);
    if ( ! pData ) {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_axis: can't set slice params"));
      return false;
    }
    Vec3<size_t> dims = pData->getDims();
    plane = dims[axis] / 2;
    if ( ! setSliceParam(axis, plane) ) {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_axis: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_axis"
  else if ( nameStr == "set_plane" ) {
    int plane = atoi(valueStr.c_str());
    if ( ! setSliceParam(m_sliceAxis, plane) ) {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_plane: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_plane"
  else if ( nameStr == "set_line_width" ) {
    float lw = (float)atof(valueStr.c_str());
    if ( ! setLineWidth(lw) ) {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_line_width: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_line_width"
  
  else if ( nameStr == "set_show_grid" ) {
    bool gsm;
    if ( valueStr == string("yes") ) gsm = true;
    else if ( valueStr == string("no") ) gsm = false;
    else {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_show_grid: invalid value") + valueStr);
      return false;
    }
    if ( ! setShowGrid(gsm) ) {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_show_grid: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_show_grid"
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
  else {
    // not my command
    ErrMsg(MsgERR, msgHdr + string("unknown command: ") + nameStr);
    return false;
  }

  return true;
}
