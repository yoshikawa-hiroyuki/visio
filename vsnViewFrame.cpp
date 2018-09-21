//
// vsnViewFrame
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
#include "wx/colordlg.h"
#include "wx/cursor.h"
#include "wx/toolbar.h"
#include "wx/statbmp.h"
#include "wx/filename.h"
#include "wx/mimetype.h"
#include "wx/utils.h"

#include "vsnViewFrame.h"
#include "vsnViewFrameWinDlg.h"
#include "vsnAuxUiFrame.h"
#include "vsnPropDlg.h"
#include "vsnLightAttribute.h"
#include "vsnTextEntryDlg.h"
#include "vsnApp.h"
#include "vsnAnchor.h"
#include "vsnError.h"
#include "vfruStl.h"
#include "utilPath.h"
#include "vsnDataParallelFiles.h"
#include "vsnMethod_keyFrameAnim.h"
#include "vsnMethod_timeStep.h"
#include <sstream>

using namespace std;
using namespace CES;
using namespace VSN;

#define FRAME_DEFAULT_W 767
#define FRAME_DEFAULT_H 512
#define FRAME_DEFAULT_VPNAME "( viewpoint0 )"


//----------------------------------------------------------------
// class vsnViewFrame
//----------------------------------------------------------------

BEGIN_EVENT_TABLE(vsnViewFrame, wxFrame)
  EVT_MENU(ViewFrameMenu_File_New,
	   vsnViewFrame::OnMenuFile_New)
  EVT_MENU(ViewFrameMenu_File_Open,
	   vsnViewFrame::OnMenuFile_Open)
  EVT_MENU(ViewFrameMenu_File_Imp_VsnXML,
	   vsnViewFrame::OnMenuFile_Imp_VsnXML)
  EVT_MENU(ViewFrameMenu_File_Imp_Sph,
	   vsnViewFrame::OnMenuFile_Imp_Sph)
  EVT_MENU(ViewFrameMenu_File_Imp_P3dF,
	   vsnViewFrame::OnMenuFile_Imp_P3dF)
  EVT_MENU(ViewFrameMenu_File_Imp_FdvStr,
	   vsnViewFrame::OnMenuFile_Imp_FdvStr)
  EVT_MENU(ViewFrameMenu_File_Imp_Dfi,
	   vsnViewFrame::OnMenuFile_Imp_Dfi)
  EVT_MENU(ViewFrameMenu_File_Imp_OctVol,
	   vsnViewFrame::OnMenuFile_Imp_OctVol)
  EVT_MENU(ViewFrameMenu_File_Imp_Scatter,
	   vsnViewFrame::OnMenuFile_Imp_Scatter)
  EVT_MENU(ViewFrameMenu_File_Imp_ScatterTS,
	   vsnViewFrame::OnMenuFile_Imp_ScatterTS)
  EVT_MENU(ViewFrameMenu_File_Imp_TriaCells,
	   vsnViewFrame::OnMenuFile_Imp_TriaCells)
  EVT_MENU(ViewFrameMenu_File_Imp_TriaCellsTS,
	   vsnViewFrame::OnMenuFile_Imp_TriaCellsTS)
  EVT_MENU(ViewFrameMenu_File_Imp_Shape,
	   vsnViewFrame::OnMenuFile_Imp_Shape)
  EVT_MENU(ViewFrameMenu_File_Imp_ShapeTS,
	   vsnViewFrame::OnMenuFile_Imp_ShapeTS)
  EVT_MENU(ViewFrameMenu_File_Imp_ShapeLOD,
	   vsnViewFrame::OnMenuFile_Imp_ShapeLOD)
  EVT_MENU(ViewFrameMenu_File_Imp_SvSQL,
	   vsnViewFrame::OnMenuFile_Imp_SvSQL)
  EVT_MENU(ViewFrameMenu_File_Save,
	   vsnViewFrame::OnMenuFile_Save)
  EVT_MENU(ViewFrameMenu_File_SaveAs,
	   vsnViewFrame::OnMenuFile_SaveAs)
  EVT_MENU(ViewFrameMenu_File_OprProp,
	   vsnViewFrame::OnMenuFile_OprProp)
  EVT_MENU(ViewFrameMenu_File_NewParamCanvas,
	   vsnViewFrame::OnMenuFile_NewParamCanvas)
  EVT_MENU(ViewFrameMenu_File_ParamCanvasList,
	   vsnViewFrame::OnMenuFile_ParamCanvasList)
  EVT_MENU(ViewFrameMenu_File_ExecCommand,
	   vsnViewFrame::OnMenuFile_ExecCommand)
  EVT_MENU(ViewFrameMenu_File_Quit,
	   vsnViewFrame::OnMenuFile_Quit)
  EVT_MENU(ViewFrameMenu_View_New,
	   vsnViewFrame::OnMenuView_New)
  EVT_MENU(ViewFrameMenu_View_Close,
	   vsnViewFrame::OnMenuView_Close)
  EVT_MENU(ViewFrameMenu_View_Geometry,
	   vsnViewFrame::OnMenuView_Geometry)
  EVT_MENU(ViewFrameMenu_View_Layout,
	   vsnViewFrame::OnMenuView_Layout)
  EVT_MENU(ViewFrameMenu_View_UiShow,
	   vsnViewFrame::OnMenuView_UiShow)
  EVT_MENU(ViewFrameMenu_View_AuxUiShow,
	   vsnViewFrame::OnMenuView_AuxUiShow)
  EVT_MENU(ViewFrameMenu_View_ShowToolBar,
	   vsnViewFrame::OnMenuView_ShowToolBar)
  EVT_MENU(ViewFrameMenu_View_Perspective,
	   vsnViewFrame::OnMenuView_Perspective)
  EVT_MENU(ViewFrameMenu_View_SetBgColor,
	   vsnViewFrame::OnMenuView_SetBgColor)
  EVT_MENU(ViewFrameMenu_View_Normalize,
	   vsnViewFrame::OnMenuView_Normalize)
  EVT_MENU(ViewFrameMenu_View_NormalizeScene,
	   vsnViewFrame::OnMenuView_NormalizeScene)
  EVT_MENU(ViewFrameMenu_View_SetViewPoint,
	   vsnViewFrame::OnMenuView_SetViewPoint)
  EVT_MENU(ViewFrameMenu_View_SyncViewPoint,
	   vsnViewFrame::OnMenuView_SyncViewPoint)
  EVT_MENU(ViewFrameMenu_View_SetCenter,
	   vsnViewFrame::OnMenuView_SetCenter)
  EVT_MENU(ViewFrameMenu_View_CenterShow,
	   vsnViewFrame::OnMenuView_CenterShow)
  EVT_MENU(ViewFrameMenu_View_NormCenter,
	   vsnViewFrame::OnMenuView_NormCenter)
  EVT_MENU(ViewFrameMenu_View_FrAxisShow,
	   vsnViewFrame::OnMenuView_FrAxisShow)
  EVT_MENU(ViewFrameMenu_View_Shot,
	   vsnViewFrame::OnMenuView_Shot)
  EVT_MENU(ViewFrameMenu_View_ShotAndExec,
	   vsnViewFrame::OnMenuView_ShotAndExec)
  EVT_MENU(ViewFrameMenu_Scene_Add,
	   vsnViewFrame::OnMenuScene_Add)
  EVT_MENU(ViewFrameMenu_Scene_Del,
	   vsnViewFrame::OnMenuScene_Del)
  EVT_MENU(ViewFrameMenu_Scene_Rename,
	   vsnViewFrame::OnMenuScene_Rename)
  EVT_MENU(ViewFrameMenu_Scene_Anchor,
	   vsnViewFrame::OnMenuScene_Anchor)
  //EVT_MENU(ViewFrameMenu_Scene_AddObjGrp,
  //	   vsnViewFrame::OnMenuScene_AddObjGrp)
  EVT_MENU(ViewFrameMenu_Scene_LightAttr,
	   vsnViewFrame::OnMenuScene_LightAttr)
  EVT_MENU(ViewFrameMenu_Help_UserGuide,
	   vsnViewFrame::OnMenuHelp_UserGuide)
  EVT_MENU(ViewFrameMenu_Help_FileFmt,
	   vsnViewFrame::OnMenuHelp_FileFmt)
  EVT_MENU(ViewFrameMenu_Help_About,
	   vsnViewFrame::OnMenuHelp_About)
  EVT_UPDATE_UI(ViewFrameMenu_Scene_Anchor,
		vsnViewFrame::OnUpdateMenuScene_Anchor)
  EVT_TOOL(ViewFrameTooBar_NormView,
	   vsnViewFrame::OnToolBar_Normalize)
  EVT_TOOL_RANGE(ViewFrameTooBar_ProjPers, ViewFrameTooBar_ProjOrtho,
		 vsnViewFrame::OnToolBar_Projection)
  EVT_TOOL_RANGE(ViewFrameTooBar_Front, ViewFrameTooBar_Bottom,
		 vsnViewFrame::OnToolBar_ViewDirs)
  EVT_TOOL_RANGE(ViewFrameTooBar_AnimPlayBtn, ViewFrameTooBar_AnimFwdBtn,
		 vsnViewFrame::OnToolBar_AnimControl)
  EVT_CLOSE(vsnViewFrame::OnClose)
  EVT_MENU(wxID_EXIT,
	   vsnViewFrame::OnMenuFile_Quit)
END_EVENT_TABLE()


/* resources */

#include "icon/proj_pers.xpm"
#include "icon/proj_ortho.xpm"
#include "icon/view_norm.xpm"
#include "icon/view_front.xpm"
#include "icon/view_back.xpm"
#include "icon/view_left.xpm"
#include "icon/view_right.xpm"
#include "icon/view_top.xpm"
#include "icon/view_bottom.xpm"
#include "icon/ts_bgn.xpm"
#include "icon/ts_stop.xpm"
#include "icon/ts_play.xpm"
#include "icon/ts_end.xpm"


/* static members */
size_t vsnViewFrame::s_viewSeq = 0;


/* constructors / destructor */

vsnViewFrame::vsnViewFrame(wxWindow* parent, const wxString& title,
			   const std::string& name,
			   const wxPoint& pos, const wxSize& size,
			   long style)
  : vsnFrameBase(parent, title, pos, size, style),
    m_pGfxView(NULL), m_pUiView(NULL), m_pAuxUiFrame(NULL),
    m_showUiView(true), m_showAuxUiFrame(false), m_showToolBar(true),
    m_showFrAxis(true),
    m_setCenterMode(false), m_showCenter(true), m_normalizeCenter(true),
    m_pViewPointDlg(NULL), p_vpRefMaster(NULL), m_pLightAttrDlg(NULL)
{
  /* seqNo */
  m_seq = ++s_viewSeq;

  /* default name */
  char nbuf[64]; sprintf(nbuf, "View%zd", m_seq);
  if ( name == std::string(VFR_NONAME) )
    m_name = nbuf;
  else
    m_name = name;

  /* prepare GfxView */
  m_pGfxView = new vsnGfxView(this);
  assert(m_pGfxView);

  /* prepare UiView */
  m_pUiView = new vsnUiView(this);
  assert(m_pUiView);

  /* background color */
  vector4 bgc = {0.f, 0.f, 0.f, 1.f};
  setBgColor(bgc);

  /* set Sizer */
  if ( size == wxDefaultSize )
    SetSize(-1, -1, FRAME_DEFAULT_W, FRAME_DEFAULT_H);
  else
    SetSize(-1, -1, size.GetWidth(), size.GetHeight());
  wxBoxSizer* topsizer = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(m_pUiView, 0, wxEXPAND);
  topsizer->Add(m_pGfxView->getDrawArea()->getCanvas(), 1, wxEXPAND);
  SetSizer(topsizer);
  topsizer->Layout(); // need on MacOSX

  /* menu bar */
  setupMenuBar();

  /* tool bar */
  setupToolBar();

  /* status bar */
  setupStatusBar();

  /* set icon */
  SetIcon(vfrDrawAreaWx::GetVFRIcon());
}

vsnViewFrame::~vsnViewFrame() {
  // clear ViewPoint sync
  setViewPointMaster(NULL);
  set<vsnViewFrame*>::iterator it;
  set<vsnViewFrame*> curSlaves = m_vpRefSlaveLst;
  for ( it = curSlaves.begin(); it != curSlaves.end(); it++ )
    if ( *it ) (*it)->setViewPointMaster(NULL);
  m_vpRefSlaveLst.clear();

  if ( m_pLightAttrDlg ) {
    m_pLightAttrDlg->Destroy();
    m_pLightAttrDlg = NULL;
  }

  if ( m_pUiView ) {
    vsnTreeItem ti(TI_Scene);
    m_pUiView->selectObj(&ti);
  }

  if ( m_pGfxView ) delete m_pGfxView;
  //if ( m_pUiView ) delete m_pUiView; // Wx object, don't delete
  if ( m_pAuxUiFrame ) invalidateAuxUiFrame(m_pAuxUiFrame);

  // remove myself from viewList of app
  if ( p_app )
    (void)p_app->delViewFrame(this);
}


void vsnViewFrame::reset() {
  p_scene = NULL;

  if ( m_pUiView )
    m_pUiView->reset();

  //m_wxColorLst.clear();
  vector4 bgc = {0.f, 0.f, 0.f, 1.f};
  setBgColor(bgc);

  escapeSetCenterMode(); // to un-check 'set center mode' menu
  if ( m_pGfxView ) {
    m_pGfxView->setScene(NULL);
  }

  if ( m_pLightAttrDlg ) {
    m_pLightAttrDlg->Hide();
  }

  clearViewpointList();

  setShowToolBar(true);
  setShowCenter(true);
  setNormCenter(true);
  setShowFrAxis(true);

  // Toolbar Animation selector
  updateTBAnimSelector();

  if ( m_pAuxUiFrame ) {
    m_pAuxUiFrame->Destroy();
    invalidateAuxUiFrame(m_pAuxUiFrame);
  }
}

void vsnViewFrame::refresh(const bool reSelObj) {
  // UiView part
  if ( m_pUiView ) {
    vsnDataObj* pDt = NULL;
    vsnMethodObj* pMtd = NULL;
    if ( reSelObj ) {
      pDt = m_pUiView->getCurrentData();
      pMtd = m_pUiView->getCurrentMethod();
    }
    if ( p_scene )
      m_pUiView->updateScene(p_scene);
    if ( pMtd )
      m_pUiView->selectObj(pMtd);
    else if ( pDt )
      m_pUiView->selectObj(pDt);
  }

  // GfxView part
  if ( m_pGfxView ) {
    vfrDrawAreaWx* pda = m_pGfxView->getDrawArea();
    if ( pda ) {
      m_pGfxView->adjustCenterGlyph();
      pda->chkNotice();
    }
    if ( m_pLightAttrDlg ) m_pLightAttrDlg->update();
  }

  // AuxUiView part
  if ( m_pAuxUiFrame ) {
    m_pAuxUiFrame->refresh(reSelObj);
  }

  // Toolbar Animation selector
  updateTBAnimSelector();
}

bool vsnViewFrame::closeView() {
  if ( m_seq == 1 && p_app->getNumViewFrame() > 1 ) {
    ErrMsg(MsgWARN,
	   string("ViewFrame: closeView: can't close the first(master) view"));
    return false;
  }

  // invalidate UiView sub-panels
  if ( m_pUiView ) {
    m_pUiView->enableMethodArea(FALSE);
    m_pUiView->enableDataArea(FALSE);
  }

  // destroy myself
  Destroy();
  if ( p_app ) p_app->delViewFrame(this);

  return true;
}


void vsnViewFrame::setShowUiView(const bool suivm) {
  if ( ! m_pUiView ) return;
  if ( m_showUiView == suivm ) return;
  m_showUiView = suivm;

  wxSizer* sizer = GetSizer();
  if ( ! sizer ) return;
  sizer->Show(m_pUiView, m_showUiView);
  sizer->Layout();

  wxMenuBar* pmb = GetMenuBar();
  if ( ! pmb ) return;
  wxMenu* pvm = pmb->GetMenu(pmb->FindMenu(wxT("View")));
  if ( ! pvm ) return;
  pvm->Check(ViewFrameMenu_View_UiShow, m_showUiView);
}

void vsnViewFrame::invalidateAuxUiFrame(vsnAuxUiFrame* pauf) {
  if ( ! m_pAuxUiFrame || m_pAuxUiFrame != pauf ) return;
  m_pAuxUiFrame = NULL;
  m_showAuxUiFrame = false;

  wxMenuBar* pmb = GetMenuBar();
  if ( ! pmb ) return;
  wxMenu* pvm = pmb->GetMenu(pmb->FindMenu(wxT("View")));
  if ( ! pvm ) return;
  pvm->Check(ViewFrameMenu_View_AuxUiShow, m_showAuxUiFrame);
}

void vsnViewFrame::setShowAuxUiFrame(const bool sauifm) {
  if ( m_showAuxUiFrame == sauifm ) return;
  m_showAuxUiFrame = sauifm;

  if ( ! m_pAuxUiFrame ) {
    m_pAuxUiFrame = new vsnAuxUiFrame(NULL, vsnApp::ConvSysToWx(m_name));
    if ( ! m_pAuxUiFrame ) return;
    if ( p_scene ) m_pAuxUiFrame->changeScene(p_scene);
    vsnUiView* pauiv = m_pAuxUiFrame->getUiView();
    if ( pauiv ) {
      vsnDataObj* pDt = m_pUiView->getCurrentData();
      vsnMethodObj* pMtd = m_pUiView->getCurrentMethod();
      if ( pMtd ) pauiv->selectObj(pMtd);
      else if ( pDt ) pauiv->selectObj(pDt);

      wxSize uvSz = pauiv->GetSize();
      if ( uvSz.x == 0 ) uvSz.x = -1;
      m_pAuxUiFrame->SetSize(-1,-1, uvSz.GetWidth(),-1);
    }
  }
  m_pAuxUiFrame->Show(m_showAuxUiFrame);

  wxMenuBar* pmb = GetMenuBar();
  if ( ! pmb ) return;
  wxMenu* pvm = pmb->GetMenu(pmb->FindMenu(wxT("View")));
  if ( ! pvm ) return;
  pvm->Check(ViewFrameMenu_View_AuxUiShow, m_showAuxUiFrame);
}

void vsnViewFrame::setShowToolBar(const bool stb) {
  if ( m_showToolBar == stb ) return;
  m_showToolBar = stb;

  wxToolBar *ptb = GetToolBar();
  if ( m_showToolBar ) {
    if ( ptb ) return;
    setupToolBar();
  }
  else {
    if ( ptb ) {
      delete ptb;
      SetToolBar(NULL);
    }
  }
#ifdef MacOSX
  wxSizer* sizer = GetSizer();
  if ( sizer ) sizer->Layout();
  int ww, wh; GetSize(&ww, &wh);
  setGeometry(-1, -1, ww, m_showToolBar ? wh + 1 : wh -1);
#endif

  wxMenuBar* pmb = GetMenuBar();
  if ( ! pmb ) return;
  wxMenu* pvm = pmb->GetMenu(pmb->FindMenu(wxT("View")));
  if ( ! pvm ) return;
  pvm->Check(ViewFrameMenu_View_ShowToolBar, m_showToolBar);
}


void vsnViewFrame::setScene(vsnScene* ps) {
  string scName;
  map<string, vsnViewPoint>::iterator xit;

  // don't check scene-unchanged
  //if ( p_scene && p_scene == ps ) return;
  if ( ! m_pGfxView ) return;

  // to un-check 'set center mode' menu
  escapeSetCenterMode();

  // backup the last view of the old scene
  if ( p_scene ) {
    scName = p_scene->getName();
    xit = m_lastViewPointLst.find(scName);
    if ( xit != m_lastViewPointLst.end() ) {
      xit->second = m_pGfxView->getXForm(); // override
    }
    else if ( ! scName.empty() && scName != VFR_NONAME ) {
      vsnViewPoint xfm = m_pGfxView->getXForm();
      m_lastViewPointLst.insert(make_pair(scName, xfm));
    }
  }

  // set new scene
  m_pGfxView->setScene(ps);
  if ( m_pLightAttrDlg ) {
    m_pLightAttrDlg->Hide();
    if ( ! ps )
      m_pLightAttrDlg->setRefLightAtt(NULL);
	else {
      vsnLightAttribute& l = ps->getLightAttr();
      m_pLightAttrDlg->setRefLightAtt(&l);
    }
  }
  if ( m_pUiView ) m_pUiView->updateScene(ps);

  p_scene = ps;
  if ( m_pAuxUiFrame ) m_pAuxUiFrame->changeScene(p_scene);
  if ( ! p_scene ) return;

  // restore the last view of new scene
  scName = p_scene->getName();
  xit = m_lastViewPointLst.find(scName);
  if ( xit != m_lastViewPointLst.end() )
    m_pGfxView->setXForm(xit->second);
  else
    m_pGfxView->normalize();

  // Toolbar Animation selector
  updateTBAnimSelector();

  p_scene->chkNotice();
}


void vsnViewFrame::setApp(vsnApp* pp) {
  p_app = pp;
  if ( ! p_app ) return;

  if ( ! p_scene ) return;
  p_scene->invalidateDispList();
}


bool vsnViewFrame::updateSceneList(const std::deque<vsnScene*>& scl) {
  if ( ! m_pUiView ) return false;
  if ( ! m_pUiView->updateSceneList(scl) ) return false;

  // AuxUiFrame
  if ( m_pAuxUiFrame )
    (void)m_pAuxUiFrame->updateSceneList(scl);

  // check current scene
  if ( ! scl.empty() ) {
    std::deque<vsnScene*>::const_iterator it;
    for ( it = scl.begin(); it != scl.end(); it++ )
      if ( *it ==  p_scene ) return true;
    it = scl.begin();
    setScene(*it);
  } else {
    setScene(NULL);
  }

  return true;
}


bool vsnViewFrame::setBgColor(const vector4 bgc) {
  m_bgColor[0] = bgc[0]; m_bgColor[1] = bgc[1];
  m_bgColor[2] = bgc[2]; m_bgColor[3] = bgc[3];

  if ( ! m_pGfxView ) return false;
  vfrCamera* pcam = m_pGfxView->getCamera();
  if ( ! pcam ) return false;
  pcam->setBgColor(m_bgColor);
  pcam->chkNotice();
  return true;
}

void vsnViewFrame::getBgColor(vector4 bgc) const {
  bgc[0] = m_bgColor[0]; bgc[1] = m_bgColor[1];
  bgc[2] = m_bgColor[2]; bgc[3] = m_bgColor[3];
}


bool vsnViewFrame::setPerspective(const bool persMode) {
  if ( ! m_pGfxView ) return false;

  ProjectType pm = persMode ? PR_PERSPECTIVE : PR_ORTHOGONAL;
  if ( ! m_pGfxView->setProjection(pm) ) return false;
  m_pGfxView->getDrawArea()->chkNotice();

  wxMenuBar* pmb = GetMenuBar();
  if ( pmb ) {
    wxMenu* pvm = pmb->GetMenu(pmb->FindMenu(wxT("View")));
    if ( pvm )
      pvm->Check(ViewFrameMenu_View_Perspective, persMode);
  } // end of if(pmb)
  return true;
}

bool vsnViewFrame::getPerspective() const {
  if ( ! m_pGfxView ) return false;
  vfrCamera* pCam = m_pGfxView->getCamera();
  if ( ! pCam ) return false;
  return (pCam->getProjection() == PR_PERSPECTIVE);
}


bool vsnViewFrame::enterSetCenterMode() {
  if ( ! m_pGfxView ) return false;
  vfrDrawAreaWx* pda = m_pGfxView->getDrawArea();
  vsnApp* pApp = vsnApp::GetApp();
  if ( ! pda || ! pApp ) return false;
  if ( m_setCenterMode ) return false;

  vfrDispatch &dispatcher = vfrDispatch::instance(*pda);
  vsnGfxOprOrientation oprOri = pApp->getOprOrientation();
  m_pGfxView->pushActions(this);

  gfxAct_Rot.setGfxView(m_pGfxView);
  gfxAct_Roll.setGfxView(m_pGfxView);
  gfxAct_Trans.setGfxView(m_pGfxView);
  gfxAct_Scale.setGfxView(m_pGfxView);
  gfxAct_KEC.setGfxView(m_pGfxView);
  gfxAct_CC.setGfxView(m_pGfxView);
  gfxAct_DTC.setGfxView(m_pGfxView);
  gfxAct_DDC.setGfxView(m_pGfxView);
  gfxAct_DEC.setGfxView(m_pGfxView);

  // Rotate
  oprOri.m_oprLst[OprRotate].getEvent(dispatcher, EvtDragStart)
    .regist(&gfxAct_Rot);
  oprOri.m_oprLst[OprRotate].getEvent(dispatcher, EvtDrag)
    .regist(&gfxAct_Rot);
  oprOri.m_oprLst[OprRotate].getEvent(dispatcher, EvtDragEnd)
    .regist(&gfxAct_Rot);

  // Roll
  oprOri.m_oprLst[OprRoll].getEvent(dispatcher, EvtDragStart)
    .regist(&gfxAct_Roll);
  oprOri.m_oprLst[OprRoll].getEvent(dispatcher, EvtDrag)
    .regist(&gfxAct_Roll);
  oprOri.m_oprLst[OprRoll].getEvent(dispatcher, EvtDragEnd)
    .regist(&gfxAct_Roll);

  // KeyIn, Click
  vfrEvKeyIn::instance(dispatcher).regist(&gfxAct_KEC);
  vfrEvClick::instance(dispatcher).regist(&gfxAct_CC);

  // Translate
  oprOri.m_oprLst[OprTranslate].getEvent(dispatcher, EvtDragStart)
    .regist(&gfxAct_Trans);
  oprOri.m_oprLst[OprTranslate].getEvent(dispatcher, EvtDrag)
    .regist(&gfxAct_DTC);
  oprOri.m_oprLst[OprTranslate].getEvent(dispatcher, EvtDragEnd)
    .regist(&gfxAct_DEC);

  // Scale(Dolly)
  oprOri.m_oprLst[OprScale].getEvent(dispatcher, EvtDragStart)
    .regist(&gfxAct_Scale);
  oprOri.m_oprLst[OprScale].getEvent(dispatcher, EvtDrag)
    .regist(&gfxAct_DDC);
  oprOri.m_oprLst[OprScale].getEvent(dispatcher, EvtDragEnd)
    .regist(&gfxAct_DEC);

  m_pGfxView->setCursor(const_cast<wxCursor*>(wxCROSS_CURSOR));
  setShowCenter(true);
  m_setCenterMode = true;
  return true;
}

void vsnViewFrame::escapeSetCenterMode() {
  if ( ! m_setCenterMode ) return;
  m_setCenterMode = false;

  gfxAct_Rot.setGfxView(NULL);
  gfxAct_Roll.setGfxView(NULL);
  gfxAct_KEC.setGfxView(NULL);
  gfxAct_CC.setGfxView(NULL);
  gfxAct_DTC.setGfxView(NULL);
  gfxAct_DDC.setGfxView(NULL);
  gfxAct_DEC.setGfxView(NULL);

  if ( m_pGfxView ) {
    m_pGfxView->popActions(this);
  } // end of if(m_pGfxView)

  wxMenuBar* pmb = GetMenuBar();
  if ( pmb ) {
    wxMenu* pvm = pmb->GetMenu(pmb->FindMenu(wxT("View")));
    if ( pvm )
      pvm->Check(ViewFrameMenu_View_SetCenter, FALSE);
  } // end of if(pmb)
}

void vsnViewFrame::setShowCenter(const bool scm) {
  if ( ! m_pGfxView ) return;
  if ( m_showCenter == scm ) return;
  m_showCenter = scm;

  vfrNode* pcg = m_pGfxView->getCenterGlyph();
  if ( ! pcg ) return;
  vfrMaterial* pcgMate = pcg->getPrivateMaterial();
  if ( ! pcgMate ) return;

  if ( m_showCenter )
    pcgMate->setRenderMode(RT_WIRE);
  else
    pcgMate->setRenderMode(RT_NONE);
  pcg->chkNotice();

  wxMenuBar* pmb = GetMenuBar();
  if ( pmb ) {
    wxMenu* pvm = pmb->GetMenu(pmb->FindMenu(wxT("View")));
    if ( pvm )
      pvm->Check(ViewFrameMenu_View_CenterShow, m_showCenter);
  } // end of if(pmb)
}

void vsnViewFrame::setNormCenter(const bool ncm) {
  if ( m_normalizeCenter == ncm ) return;
  m_normalizeCenter = ncm;

  wxMenuBar* pmb = GetMenuBar();
  if ( pmb ) {
    wxMenu* pvm = pmb->GetMenu(pmb->FindMenu(wxT("View")));
    if ( pvm )
      pvm->Check(ViewFrameMenu_View_NormCenter, m_normalizeCenter);
  } // end of if(pmb)
}

void vsnViewFrame::setShowFrAxis(const bool sfa) {
  if ( ! m_pGfxView ) return;
  if ( m_showFrAxis == sfa ) return;
  m_showFrAxis = sfa;

  vfrNode* pfa = m_pGfxView->getFrontAxis();
  if ( ! pfa ) return;
  vfrMaterial* pfaMate = pfa->getPrivateMaterial();
  if ( ! pfaMate ) return;

  if ( m_showFrAxis )
    pfaMate->setRenderMode(RT_SMOOTH);
  else
    pfaMate->setRenderMode(RT_NONE);
  pfa->chkNotice();

  wxMenuBar* pmb = GetMenuBar();
  if ( pmb ) {
    wxMenu* pvm = pmb->GetMenu(pmb->FindMenu(wxT("View")));
    if ( pvm )
      pvm->Check(ViewFrameMenu_View_FrAxisShow, m_showFrAxis);
  } // end of if(pmb)
}

vsnViewPoint vsnViewFrame::getViewPoint(const std::string& vpname) const {
  vsnViewPoint xfm;
  if ( ! m_pGfxView ) return xfm;

  map<string, vsnViewPoint>::const_iterator it;
  it = m_viewPointLst.find(vpname);
  if ( it != m_viewPointLst.end() ) {
    xfm = it->second;
  }
  return xfm;
}

bool vsnViewFrame::addViewPoint(const std::string& vpname) {
  if ( ! m_pGfxView ) return false;
  if ( vpname.empty() ) return false;
  vsnViewPoint xfm = m_pGfxView->getXForm();
  xfm.name = vpname;

  map<string, vsnViewPoint>::iterator it;
  it = m_viewPointLst.find(xfm.name);
  if ( it == m_viewPointLst.end() ) {
    // not found, add new view point
    if ( ! m_viewPointLst.insert(make_pair(xfm.name, xfm)).second )
      return false;
  }
  else {
    // found, override the view point
    it->second = xfm;
  }
  return updateViewPoint(xfm.name);
}

bool vsnViewFrame::delViewPoint(const std::string& vpname) {
  if ( ! m_pGfxView ) return false;

  map<string, vsnViewPoint>::iterator it;
  it = m_viewPointLst.find(vpname);
  if ( it == m_viewPointLst.end() ) return false;

  m_viewPointLst.erase(it);
  return true;
}

bool vsnViewFrame::updateViewPoint(const std::string& vpname,
				   const bool xanim) {
  if ( ! m_pGfxView ) return false;

  map<string, vsnViewPoint>::iterator it;
  it = m_viewPointLst.find(vpname);
  if ( it == m_viewPointLst.end() ) return false;

  if ( xanim )
    m_pGfxView->setViewXForm(it->second);
  else
    m_pGfxView->setXForm(it->second);
  return true;
}

void vsnViewFrame::clearViewpointList() {
  m_viewPointLst.clear();
  m_lastViewPointLst.clear();

  if ( m_pViewPointDlg ) {
    m_pViewPointDlg->setRefViewFrame(NULL);
    m_pViewPointDlg->Hide();
  }

  p_vpRefMaster = NULL;
  m_vpRefSlaveLst.clear();
}


bool vsnViewFrame::addViewPointSlave(vsnViewFrame* slave) {
  if ( ! slave || slave == this ) return false;
  if ( p_vpRefMaster ) return false; // this is a master, can't be slave

  if ( m_vpRefSlaveLst.find(slave) != m_vpRefSlaveLst.end() ) return false;
  m_vpRefSlaveLst.insert(slave);
  return true;
}

bool vsnViewFrame::delViewPointSlave(vsnViewFrame* slave) {
  if ( ! slave ) return false;
  if ( m_vpRefSlaveLst.erase(slave) < 1 ) return false;
  return true;
}

bool vsnViewFrame::setViewPointMaster(vsnViewFrame* master) {
  if ( master == p_vpRefMaster ) return true;
  if ( master == this ) return false;  // myself ?

  if ( p_vpRefMaster )
    (void)p_vpRefMaster->delViewPointSlave(this);

  p_vpRefMaster = master;
  if ( ! p_vpRefMaster ) return true; // set to 'No Sync', ok
  vsnViewFrame* pvpm = p_vpRefMaster->getViewPointMaster();
  if ( pvpm ) {
    if ( pvpm == this ) p_vpRefMaster->setViewPointMaster(NULL);
    else p_vpRefMaster = pvpm;
  }

  set<vsnViewFrame*>::iterator it;
  set<vsnViewFrame*> curSlaves = m_vpRefSlaveLst;
  for ( it = curSlaves.begin(); it != curSlaves.end(); it++ ) {
    if ( ! *it ) continue;
    if ( *it == p_vpRefMaster ) (*it)->setViewPointMaster(NULL);
    else (*it)->setViewPointMaster(p_vpRefMaster);
  } // end of for(it)
  m_vpRefSlaveLst.clear();

  if ( ! p_vpRefMaster->addViewPointSlave(this) ) {
    p_vpRefMaster = NULL;
    return false;
  }

  m_pGfxView->setXForm(p_vpRefMaster->getGfxView()->getXForm());
  m_pGfxView->getDrawArea()->chkNotice();
  return true;
}


bool vsnViewFrame::setupMenuBar() {
  wxMenuBar* pmb = GetMenuBar();
  if ( pmb ) return true; // MenuBar already prepared

  pmb = new wxMenuBar;
  if ( ! pmb ) return false;

  // 'Import' sub menu of 'File' menu
  wxMenu* impMenu = new wxMenu;
  if ( ! impMenu ) return false;
  impMenu->Append(ViewFrameMenu_File_Imp_VsnXML,
		  wxT("vsn XML file ..."), wxT("Import/Merge vsn XML file"));
  impMenu->AppendSeparator();
  impMenu->Append(ViewFrameMenu_File_Imp_Sph,
		  wxT("Sph data files..."), wxT("Import Sph data files"));
  impMenu->Append(ViewFrameMenu_File_Imp_P3dF,
		  wxT("PLO3D function files..."),
		  wxT("Import PLOT3D function files"));
  impMenu->Append(ViewFrameMenu_File_Imp_FdvStr,
		  wxT("Fdv data file..."),
		  wxT("Import 4DVisisualizer Str data file"));
  impMenu->AppendSeparator();
  impMenu->Append(ViewFrameMenu_File_Imp_Dfi,
		  wxT("Dfi data file..."),
		  wxT("Import Dfi data file"));
  impMenu->AppendSeparator();
  impMenu->Append(ViewFrameMenu_File_Imp_Scatter,
		  wxT("Scatter(Scat, Pwn) data files..."),
		  wxT("Import Scatter(Scat/Pwn) data files"));
  impMenu->Append(ViewFrameMenu_File_Imp_ScatterTS,
		  wxT("Scatter as Time series data ..."),
		  wxT("Import Scatter data files as time series data"));
  impMenu->AppendSeparator();
  impMenu->Append(ViewFrameMenu_File_Imp_TriaCells,
		  wxT("Triangle cells with data(VTK) files..."),
		  wxT("Import Triangle cells with data(VTK) files"));
  impMenu->Append(ViewFrameMenu_File_Imp_TriaCellsTS,
		  wxT("Triangle cells as Time series data ..."),
		  wxT("Import Triangle cells data files as time series data"));
  impMenu->AppendSeparator();
  impMenu->Append(ViewFrameMenu_File_Imp_Shape,
		  wxT("Shape(Obj, STL) data files..."),
		  wxT("Import Shape(Obj/Stl) data files"));
  impMenu->Append(ViewFrameMenu_File_Imp_ShapeTS,
		  wxT("Shapes as Time series data ..."),
		  wxT("Import Shape data files as time series data"));
  impMenu->Append(ViewFrameMenu_File_Imp_ShapeLOD,
		  wxT("Shapes as LOD data ..."),
		  wxT("Import Shape data files as LOD data"));
  impMenu->AppendSeparator();
  impMenu->Append(ViewFrameMenu_File_Imp_OctVol,
		  wxT("Otv data files..."),
		  wxT("Import Otv data files"));
#ifdef PGSQL
  impMenu->AppendSeparator();
  impMenu->Append(ViewFrameMenu_File_Imp_SvSQL,
		  wxT("SvSQL data"), wxT("Import Sph data from Postgre/SQL"));
#endif // PGSQL

  // 'File' menu
  wxMenu* fileMenu = new wxMenu;
  if ( ! fileMenu ) return false;
  fileMenu->Append(ViewFrameMenu_File_New, wxT("New\tCTRL+N"),
		   wxT("Create new project"));
  fileMenu->AppendSeparator();
  fileMenu->Append(ViewFrameMenu_File_Open, wxT("Open ...\tCTRL+O"),
		   wxT("Open vsn XML project file"));
  fileMenu->Append(ViewFrameMenu_File_Import, wxT("Import"), impMenu);
  fileMenu->AppendSeparator();
  fileMenu->Append(ViewFrameMenu_File_Save, wxT("Save\tCTRL+W"),
		   wxT("Save project in vsn XML file"));
  fileMenu->Append(ViewFrameMenu_File_SaveAs, wxT("Save As ...\tCTRL+A"),
		   wxT("Save project in another vsn XML file"));
  fileMenu->AppendSeparator();
  fileMenu->Append(ViewFrameMenu_File_OprProp, wxT("Properties ..."),
		   wxT("Setup operation properties"));
  fileMenu->AppendSeparator();
  fileMenu->Append(ViewFrameMenu_File_NewParamCanvas, wxT("New Param canvas"),
		   wxT("Create new param canvas"));
  fileMenu->Append(ViewFrameMenu_File_ParamCanvasList,
		   wxT("Param canvas list ..."),
		   wxT("Show param canvas list dialog"));
  fileMenu->AppendSeparator();
  fileMenu->Append(ViewFrameMenu_File_ExecCommand,
		   wxT("Execute external command ..."),
		   wxT("Invoke external command script"));
  fileMenu->AppendSeparator();
  fileMenu->Append(ViewFrameMenu_File_Quit, wxT("Quit\tCTRL+Q"),
		   wxT("Quit Visio"));
  pmb->Append(fileMenu, wxT("File"));

  // 'View' menu
  wxMenu* viewMenu = new wxMenu;
  if ( ! viewMenu ) return false;
  viewMenu->Append(ViewFrameMenu_View_New, wxT("New View"),
		   wxT("Create new ViewFrame"));
  viewMenu->Append(ViewFrameMenu_View_Close, wxT("Close View"),
		   wxT("Close ViewFrame"));
  viewMenu->AppendSeparator();
  viewMenu->Append(ViewFrameMenu_View_UiShow, wxT("Show UI Panel"),
		   wxT("Show/Hide the UI Panel of the ViewFrame"), TRUE);
  viewMenu->Check(ViewFrameMenu_View_UiShow, TRUE);
  viewMenu->Append(ViewFrameMenu_View_AuxUiShow, wxT("Show Aux UI Panel"),
		   wxT("Show/Hide the Aux UI Panel window"), TRUE);
  viewMenu->Append(ViewFrameMenu_View_ShowToolBar, wxT("Show Tool Bar"),
		   wxT("Show/Hide the Tool Bar of the ViewFrame"), TRUE);
  viewMenu->Check(ViewFrameMenu_View_ShowToolBar, TRUE);
  viewMenu->AppendSeparator();
  viewMenu->Append(ViewFrameMenu_View_Perspective, wxT("Perspective"),
		   wxT("Perspective Projection"), TRUE);
  viewMenu->Check(ViewFrameMenu_View_Perspective, TRUE);
  viewMenu->Append(ViewFrameMenu_View_Normalize, wxT("Normalize View\tSPACE"),
		   wxT("Normalize(fit) Viewport to Selected"));
  viewMenu->Append(ViewFrameMenu_View_NormalizeScene,
		   wxT("Normalize View to Scene\tSHIFT+SPACE"),
		   wxT("Normalize(fit) Viewport to Scene"));
  viewMenu->AppendSeparator();
  viewMenu->Append(ViewFrameMenu_View_SetViewPoint, wxT("Set View Point ..."),
		   wxT("Set View Point for scene"));
  viewMenu->Append(ViewFrameMenu_View_SyncViewPoint,
		   wxT("Sync View Point ..."),
		   wxT("Select ViewFrame to sync View Point"));
  viewMenu->AppendSeparator();
  viewMenu->Append(ViewFrameMenu_View_Geometry, wxT("Window Geometry ..."),
		   wxT("Set position and size of ViewFrame window"));
  viewMenu->Append(ViewFrameMenu_View_Layout, wxT("Layout Windows ..."),
		     wxT("Layout all ViewFrame windows"));
  viewMenu->AppendSeparator();
  viewMenu->Append(ViewFrameMenu_View_SetCenter, 
           wxT("Set Center Operation\tCTRL+SHIFT+C"),
		   wxT("Set Rot|Scale Center mode"), TRUE);
  viewMenu->Append(ViewFrameMenu_View_NormCenter,
		   wxT("Normalize with Center Mode"),
		   wxT("Set Rot|Scale Center when Normalize View"), TRUE);
  viewMenu->Check(ViewFrameMenu_View_NormCenter, TRUE);
  viewMenu->AppendSeparator();
  viewMenu->Append(ViewFrameMenu_View_FrAxisShow, wxT("Show Front Axis"),
		   wxT("Show Axis glyph in front layer"), TRUE);
  viewMenu->Check(ViewFrameMenu_View_FrAxisShow, TRUE);
  viewMenu->Append(ViewFrameMenu_View_CenterShow, 
           wxT("Show Center Cross\tCTRL+C"),
		   wxT("Show glyph of Rot|Scale Center"), TRUE);
  viewMenu->Check(ViewFrameMenu_View_CenterShow, TRUE);
  viewMenu->AppendSeparator();
  viewMenu->Append(ViewFrameMenu_View_SetBgColor,
		   wxT("Set Background Color ..."),
		   wxT("Set Background Color of Gfx area"));
  viewMenu->AppendSeparator();
  viewMenu->Append(ViewFrameMenu_View_Shot, wxT("Screen Shot ..."),
		   wxT("Save Screenshot"));
  viewMenu->Append(ViewFrameMenu_View_ShotAndExec,
		   wxT("Screen Shot and Exec Command ..."),
		   wxT("Save Screenshot and invoke external command"));
  pmb->Append(viewMenu, wxT("View"));

  // 'scene' menu
  wxMenu* sceneMenu = new wxMenu;
  if ( ! sceneMenu ) return false;
  sceneMenu->Append(ViewFrameMenu_Scene_Add, wxT("Add New Scene"),
		    wxT("Add new scene"));
  sceneMenu->Append(ViewFrameMenu_Scene_Del, wxT("Delete Scene"),
		    wxT("Delete the current scene"));
  sceneMenu->AppendSeparator();
  sceneMenu->Append(ViewFrameMenu_Scene_Rename, wxT("Rename Scene ..."),
		    wxT("Rename the current scene"));
  sceneMenu->AppendSeparator();
  sceneMenu->Append(ViewFrameMenu_Scene_Anchor, wxT("Edit Anchor ..."),
		    wxT("Edit action of anchor"));
  //sceneMenu->Append(ViewFrameMenu_Scene_AddObjGrp, wxT("Add ObjGrp ..."),
  //		    wxT("Add new ObjGrp"));
  sceneMenu->AppendSeparator();
  sceneMenu->Append(ViewFrameMenu_Scene_LightAttr,
		    wxT("Light Attribute ..."),
		    wxT("Set up the light attribute"));
  pmb->Append(sceneMenu, wxT("Scene"));

  // 'Help' menu
  wxMenu* helpMenu = new wxMenu;
  if ( ! helpMenu ) return false;
  helpMenu->Append(ViewFrameMenu_Help_UserGuide, wxT("User Guide ..."),
		   wxT("Open User Guide PDF"));
  helpMenu->Append(ViewFrameMenu_Help_FileFmt, wxT("File Format ..."),
		   wxT("Open File Format PDF"));
  string xmsg = string("About ") + vsn_app_name + string(" ...");
  helpMenu->Append(ViewFrameMenu_Help_About, vsnApp::ConvSysToWx(xmsg),
		   wxT("Show version"));
  pmb->Append(helpMenu, wxT("&Help"));

  // register the menu-bar
  SetMenuBar(pmb);

  return true;
}

bool vsnViewFrame::setupStatusBar() {
  wxStatusBar* psb = CreateStatusBar(2);
  if ( ! psb ) return false;
  int stws[2] = {240, -1};
  if ( m_pUiView ) {int h; m_pUiView->GetSize(&stws[0], &h);}
  psb->SetStatusWidths(2, stws);
  psb->SetStatusText(wxT("no data selected"), 1);
  return true;
}

bool vsnViewFrame::setupToolBar() {
  wxToolBar* ptb = GetToolBar();
  if ( ptb ) return true;
  ptb = CreateToolBar(wxTB_HORIZONTAL);
  if ( ! ptb ) return false;
  enum {TB_NormView =0, TB_ProjPers, TB_ProjOrtho,
	TB_Front, TB_Back, TB_Right, TB_Left, TB_Top, TB_Bottom,
	TB_NUM};
  wxBitmap tbBmps[TB_NUM];
  tbBmps[TB_NormView] = wxBitmap(view_norm);
  tbBmps[TB_ProjPers] = wxBitmap(proj_pers);
  tbBmps[TB_ProjOrtho] = wxBitmap(proj_ortho);
  tbBmps[TB_Front] = wxBitmap(view_front);
  tbBmps[TB_Back] = wxBitmap(view_back);
  tbBmps[TB_Right] = wxBitmap(view_right);
  tbBmps[TB_Left] = wxBitmap(view_left);
  tbBmps[TB_Top] = wxBitmap(view_top);
  tbBmps[TB_Bottom] = wxBitmap(view_bottom);
  ptb->SetToolBitmapSize(wxSize(tbBmps[0].GetWidth(), tbBmps[0].GetHeight()));
  ptb->AddTool(ViewFrameTooBar_ProjPers, wxT("perspective"),
	       tbBmps[TB_ProjPers], wxT("perspective projection"));
  ptb->AddTool(ViewFrameTooBar_ProjOrtho, wxT("parallel"),
	       tbBmps[TB_ProjOrtho], wxT("parallel projection"));
  ptb->AddSeparator();
  ptb->AddTool(ViewFrameTooBar_NormView, wxT("normalize"),
	       tbBmps[TB_NormView], wxT("normalize(fit) view"));
  ptb->AddSeparator();
  ptb->AddTool(ViewFrameTooBar_Front, wxT("front"),
	       tbBmps[TB_Front], wxT("front view"));
  ptb->AddTool(ViewFrameTooBar_Back, wxT("back"),
	       tbBmps[TB_Back], wxT("back view"));
  ptb->AddTool(ViewFrameTooBar_Right, wxT("right"),
	       tbBmps[TB_Right], wxT("right view"));
  ptb->AddTool(ViewFrameTooBar_Left, wxT("left"),
	       tbBmps[TB_Left], wxT("left view"));
  ptb->AddTool(ViewFrameTooBar_Top, wxT("top"),
	       tbBmps[TB_Top], wxT("top view"));
  ptb->AddTool(ViewFrameTooBar_Bottom, wxT("bottom"),
	       tbBmps[TB_Bottom], wxT("bottom view"));

  ptb->AddSeparator();
  enum {TS_Start =0, TS_Stop, TS_Play, TS_End, TS_NUM};
  wxBitmap tsBmps[TS_NUM];
  tsBmps[TS_Start] = wxBitmap(ts_start);
  tsBmps[TS_Stop] = wxBitmap(ts_stop);
  tsBmps[TS_Play] = wxBitmap(ts_play);
  tsBmps[TS_End] = wxBitmap(ts_end);
  ptb->AddTool(ViewFrameTooBar_AnimRewBtn, wxT("rewind"),
	       tsBmps[TS_Start], wxT("animation rewind"));
  ptb->AddTool(ViewFrameTooBar_AnimStopBtn, wxT("stop"),
	       tsBmps[TS_Stop], wxT("animation stop"));
  ptb->AddTool(ViewFrameTooBar_AnimPlayBtn, wxT("play"),
	       tsBmps[TS_Play], wxT("animation play"));
  ptb->AddTool(ViewFrameTooBar_AnimFwdBtn, wxT("forward"),
	       tsBmps[TS_End], wxT("animation forward"));
  ptb->AddControl(new wxComboBox(ptb, ViewFrameTooBar_AnimSelCombo,
				 wxT(""), wxDefaultPosition,
				 wxDefaultSize, 0, NULL, wxCB_READONLY));

  ptb->Realize();
  return true;
}


bool vsnViewFrame::updateTBAnimSelector() {
  if ( ! m_showToolBar ) return true;
  wxToolBar *ptb = GetToolBar();
  if ( ! ptb ) return true;
  wxControl* pTBAS = ptb->FindControl(ViewFrameTooBar_AnimSelCombo);
  wxComboBox* pTBAnimSelector = (wxComboBox*)pTBAS;
  if ( ! pTBAnimSelector ) return false;

  int oldsel = pTBAnimSelector->GetCurrentSelection();
  wxString oldSelName = pTBAnimSelector->GetValue();
  pTBAnimSelector->Clear();

  if ( ! p_scene ) return false;
  int firstKFA = -1;
  int firstTSA = -1;
  size_t j, k, idx = 0;
  size_t nod = p_scene->getNumDataObj();
  for ( j = 0; j < nod; j++ ) {
    vsnDataObj* pdo = p_scene->getDataObj(j);
    if ( ! pdo ) continue;
    size_t nom = pdo->getNumMethod();
    for ( k = 0; k < nom; k++ ) {
      vsnMethodObj* pmo = pdo->getMethod(k);
      if ( ! pmo ) continue;
      if ( pmo->getMethodType() == "timeStep" ) {
	pTBAnimSelector->Append(vsnApp::ConvSysToWx(pmo->getName()));
	if ( firstTSA < 0 ) firstTSA = idx;
	idx ++;
      } else if ( pmo->getMethodType() == "keyFrameAnim" ) {
	pTBAnimSelector->Append(vsnApp::ConvSysToWx(pmo->getName()));
	if ( firstKFA < 0 ) firstKFA = idx;
	idx ++;
      }
    } // end of for(k)
  } // end of for(j)

  if ( firstKFA >= 0 )
    pTBAnimSelector->SetSelection(firstKFA);
  else if ( firstTSA >= 0 )
    pTBAnimSelector->SetSelection(firstTSA);
  else pTBAnimSelector->SetValue(wxT(""));

  return true;
}

bool vsnViewFrame::adjustSelection(vsnTreeItem* pti) {
  if ( ! pti ) return false;
  if ( ! m_pAuxUiFrame ) return true;
  vsnUiView* pauiv = m_pAuxUiFrame->getUiView();
  if ( ! pauiv ) return false;
  return pauiv->selectObj(pti->getRefNode());
}

bool vsnViewFrame::setStatusText(const std::string& msg) {
  wxStatusBar* psb = GetStatusBar();
  if ( ! psb ) return false;
  if ( msg.empty() )
    psb->SetStatusText(wxT("---"), 1);
  else
    psb->SetStatusText(vsnApp::ConvSysToWx(msg), 1);
  return true;
}

bool vsnViewFrame::setGeometry(const int x, const int y,
			       const int w, const int h) {
  if ( x == -1 && y == -1 && w == -1 && h == -1 ) return false;
  SetSize(x, y, w, h);
  return true;
}


/* event handlers for Menu
    intercept menu commands */

void vsnViewFrame::OnMenuFile_New(wxCommandEvent& event) {
  if ( ! p_app ) return;
  p_app->reset(true);
}


void vsnViewFrame::OnMenuFile_Open(wxCommandEvent& event) {
  if ( ! p_app ) return;

  // prepare file dialog
  wxFileDialog fileDlg(this, wxT("select vsn XML file to open"),
		       wxT(""), wxT(""), // default Dir / File
		       wxT("vsn files(*.xml;*.vfv;*.vsn)|*.xml;*.vfv;*.vsn|(*)|*"),
		       wxFD_OPEN);

  // set default params
  string appCurDir = p_app->getCwd();
  string appCurFile = p_app->getCurrentFilename();
  if ( ! appCurFile.empty() )
    fileDlg.SetPath(vsnApp::ConvSysToWx(appCurFile));
  else if ( ! appCurDir.empty() )
    fileDlg.SetDirectory(vsnApp::ConvSysToWx(appCurDir));

  // load the file
  if ( fileDlg.ShowModal() == wxID_OK ) {
    wxString wpath = fileDlg.GetPath();
    string xpath = vsnPath_normalize(vsnApp::ConvWxToSys(wpath));
    if ( ! p_app->parseXMLFile(xpath) ) {
      ErrMsg(MsgERR, string("XML file parse failed: ") + xpath);
      return;
    }
  }
}

void vsnViewFrame::OnMenuFile_Imp_VsnXML(wxCommandEvent& event) {
  if ( ! p_app ) return;

  // prepare file dialog
  wxFileDialog fileDlg(this, wxT("select vsn XML file to import"),
		       wxT(""), wxT(""), // default Dir / File
		       wxT("vsn files(*.xml;*.vfv;*.vsn)|*.xml;*.vfv;*.vsn|(*)|*"),
		       wxFD_OPEN);

  // set default params
  string appCurDir = p_app->getCwd();
  string appCurFile = p_app->getCurrentFilename();
  if ( ! appCurFile.empty() )
    fileDlg.SetPath(vsnApp::ConvSysToWx(appCurFile));
  else if ( ! appCurDir.empty() )
    fileDlg.SetDirectory(vsnApp::ConvSysToWx(appCurDir));

  // load the file
  if ( fileDlg.ShowModal() == wxID_OK ) {
    string xpath = vsnPath_normalize(vsnApp::ConvWxToSys(fileDlg.GetPath()));
    if ( ! p_app->parseXMLFile(xpath, true) ) {
      ErrMsg(MsgERR, string("XML file parse failed: ") + xpath);
      return;
    }
  }
}

void vsnViewFrame::OnMenuFile_Imp_Sph(wxCommandEvent& event) {
  if ( ! p_app ) return;
  if ( ! p_scene ) return;
  string targScn = p_scene->getName();
  if ( targScn.empty() || targScn == VFR_NONAME ) {
    ErrMsg(MsgERR, string("Import Sph data: can't import data\n")
	   + string(" to the scene with no name"));
    return;
  }

  // prepare file dialog
  wxFileDialog fileDlg(this, wxT("select Sph file(s) to import"),
		       wxT(""), wxT(""), // default Dir / File
		       wxT("Sph files (*.sph;*.sv)|*.sph;*.sv|(*)|*"),
		       wxFD_OPEN | wxFD_MULTIPLE);
  
  // set default params
  string appImpDir = p_app->getImportDir();
  string appCurDir = p_app->getCwd();
  if ( ! appImpDir.empty() )
    fileDlg.SetDirectory(vsnApp::ConvSysToWx(appImpDir));
  else if ( ! appCurDir.empty() )
    fileDlg.SetDirectory(vsnApp::ConvSysToWx(appCurDir));

  // get file path from the dialog
  if ( fileDlg.ShowModal() != wxID_OK ) return;

  // import the file to the current scene
  string cmd = string("<command target=\"") + targScn
    + string("\" name=\"import_data\">\n");

  wxArrayString filePathes;
  fileDlg.GetPaths(filePathes);
  if ( filePathes.GetCount() == 1 ) {
    string filePath = vsnPath_normalize(vsnApp::ConvWxToSys(filePathes[0]));
    if ( filePath.empty() ) return;
    cmd += string("<data type=\"Sph\" file=\"");
    cmd += filePath;
    cmd += string("\" />\n");
  }
  else if ( filePathes.GetCount() > 1 ) {
    wxArrayString fileNames;
    fileDlg.GetFilenames(fileNames);
    fileNames.Sort();
    string baseDir =
      vsnPath_normalize(vsnApp::ConvWxToSys(fileDlg.GetDirectory()));
    cmd += string("<data type=\"Sph\" file=\"") + vsn_seqfile + string("\" ");
    if ( ! baseDir.empty() ) {
      cmd += string("base_dir=\"") + baseDir + string("\"");
    }
    cmd += string(">\n");
    register size_t i;
    for ( i = 0; i < fileNames.GetCount(); i++ ) {
      if ( fileNames[i].IsEmpty() ) continue;
      cmd += string("<seq file=\"") +
	vsnApp::ConvWxToSys(fileNames[i]) + string("\" />\n");
    } // end of for(i)
    cmd += string("</data>\n");
  }
  else return;

  cmd += string("</command>");
  if ( p_app->parseXMLCommand(cmd) ) {
    vsnDataObj* pld = p_scene->getLastLoadedData();
    if ( pld && m_pUiView ) m_pUiView->selectObj(pld);
  }
}

void vsnViewFrame::OnMenuFile_Imp_P3dF(wxCommandEvent& event) {
  if ( ! p_app ) return;
  if ( ! p_scene ) return;
  string targScn = p_scene->getName();
  if ( targScn.empty() || targScn == VFR_NONAME ) {
    ErrMsg(MsgERR, string("Import PLOT3D function: can't import data\n")
	   + string(" to the scene with no name"));
    return;
  }

  // prepare file dialog
  wxFileDialog fileDlg(this, wxT("select PLOT3D function file(s) to import"),
		       wxT(""), wxT(""), // default Dir / File
		       wxT("function files (*.fun;.func)|*.fun;.func|(*)|*"),
		       wxFD_OPEN | wxFD_MULTIPLE);
  
  // set default params
  string appImpDir = p_app->getImportDir();
  string appCurDir = p_app->getCwd();
  if ( ! appImpDir.empty() )
    fileDlg.SetDirectory(vsnApp::ConvSysToWx(appImpDir));
  else if ( ! appCurDir.empty() )
    fileDlg.SetDirectory(vsnApp::ConvSysToWx(appCurDir));

  // get file path from the dialog
  if ( fileDlg.ShowModal() != wxID_OK ) return;

  // import the file to the current scene
  string cmd = string("<command target=\"") + targScn
    + string("\" name=\"import_data\">\n");

  wxArrayString filePathes;
  fileDlg.GetPaths(filePathes);
  if ( filePathes.GetCount() == 1 ) {
    string filePath = vsnPath_normalize(vsnApp::ConvWxToSys(filePathes[0]));
    if ( filePath.empty() ) return;
    cmd += string("<data type=\"P3dF\" file=\"");
    cmd += filePath;
    cmd += string("\" />\n");
  }
  else if ( filePathes.GetCount() > 1 ) {
    wxArrayString fileNames;
    fileDlg.GetFilenames(fileNames);
    fileNames.Sort();
    string baseDir =
      vsnPath_normalize(vsnApp::ConvWxToSys(fileDlg.GetDirectory()));
    cmd += string("<data type=\"P3dF\" file=\"") + vsn_seqfile + string("\" ");
    if ( ! baseDir.empty() ) {
      cmd += string("base_dir=\"") + baseDir + string("\"");
    }
    cmd += string(">\n");
    register size_t i;
    for ( i = 0; i < fileNames.GetCount(); i++ ) {
      if ( fileNames[i].IsEmpty() ) continue;
      cmd += string("<seq file=\"") +
	vsnApp::ConvWxToSys(fileNames[i]) + string("\" />\n");
    } // end of for(i)
    cmd += string("</data>\n");
  }
  else return;

  cmd += string("</command>");
  if ( p_app->parseXMLCommand(cmd) ) {
    vsnDataObj* pld = p_scene->getLastLoadedData();
    if ( pld && m_pUiView ) m_pUiView->selectObj(pld);
  }
}

void vsnViewFrame::OnMenuFile_Imp_FdvStr(wxCommandEvent& event) {
  if ( ! p_app ) return;
  if ( ! p_scene ) return;
  string targScn = p_scene->getName();
  if ( targScn.empty() || targScn == VFR_NONAME ) {
    ErrMsg(MsgERR, string("Import FdvStr data: can't import data\n")
	   + string(" to the scene with no name"));
    return;
  }

  // prepare file dialog
  wxFileDialog fileDlg(this, wxT("select FdvStr file to import"),
		       wxT(""), wxT(""), // default Dir / File
		       wxT("FdvStr files (*.str;*.fdv)|*.str;*.fdv|(*)|*"),
		       wxFD_OPEN);
  
  // set default params
  string appImpDir = p_app->getImportDir();
  string appCurDir = p_app->getCwd();
  if ( ! appImpDir.empty() )
    fileDlg.SetDirectory(vsnApp::ConvSysToWx(appImpDir));
  else if ( ! appCurDir.empty() )
    fileDlg.SetDirectory(vsnApp::ConvSysToWx(appCurDir));

  // get file path from the dialog
  if ( fileDlg.ShowModal() != wxID_OK ) return;
  string filePath = vsnPath_normalize(vsnApp::ConvWxToSys(fileDlg.GetPath()));
  if ( filePath.empty() ) return;

  // import the file to the current scene
  string cmd = string("<command target=\"") + targScn
    + string("\" name=\"import_data\">\n");
  cmd += string("<data type=\"FdvStr\" file=\"");
  cmd += filePath;
  cmd += string("\" />\n");
  cmd += string("</command>");

  if ( p_app->parseXMLCommand(cmd) ) {
    vsnDataObj* pld = p_scene->getLastLoadedData();
    if ( pld && m_pUiView ) m_pUiView->selectObj(pld);
  }
}

void vsnViewFrame::OnMenuFile_Imp_Dfi(wxCommandEvent& event) {
  if ( ! p_app ) return;
  if ( ! p_scene ) return;
  string targScn = p_scene->getName();
  if ( targScn.empty() || targScn == VFR_NONAME ) {
    ErrMsg(MsgERR, string("Import Dfi data: can't import data\n")
	   + string(" to the scene with no name"));
    return;
  }

  // prepare file dialog
  wxFileDialog fileDlg(this, wxT("select Dfi file to import"),
		       wxT(""), wxT(""), // default Dir / File
		       wxT("Dfi file (*.dfi;*.tp)|*.dfi;*.tp|(*)|*"),
		       wxFD_OPEN);
  
  // set default params
  string appImpDir = p_app->getImportDir();
  string appCurDir = p_app->getCwd();
  if ( ! appImpDir.empty() )
    fileDlg.SetDirectory(vsnApp::ConvSysToWx(appImpDir));
  else if ( ! appCurDir.empty() )
    fileDlg.SetDirectory(vsnApp::ConvSysToWx(appCurDir));

  // get file path from the dialog
  if ( fileDlg.ShowModal() != wxID_OK ) return;

  // import the file to the current scene
  string cmd = string("<command target=\"") + targScn
    + string("\" name=\"import_data\">\n");

  string filePath = vsnPath_normalize(vsnApp::ConvWxToSys(fileDlg.GetPath()));
  if ( filePath.empty() ) return;
  cmd += string("<data type=\"Dfi\" file=\"");
  cmd += filePath;
  cmd += string("\" />\n");
  cmd += string("</command>");
  if ( p_app->parseXMLCommand(cmd) ) {
    vsnDataObj* pld = p_scene->getLastLoadedData();
    if ( pld && m_pUiView ) m_pUiView->selectObj(pld);
  }
}

void vsnViewFrame::OnMenuFile_Imp_OctVol(wxCommandEvent& event) {
  if ( ! p_app ) return;
  if ( ! p_scene ) return;
  string targScn = p_scene->getName();
  if ( targScn.empty() || targScn == VFR_NONAME ) {
    ErrMsg(MsgERR, string("Import OctVol data: can't import data\n")
	   + string(" to the scene with no name"));
    return;
  }

  // prepare file dialog
  wxFileDialog fileDlg(this, wxT("select Otv file(s) to import"),
		       wxT(""), wxT(""), // default Dir / File
		       wxT("Otv files (*.otv;*.oct)|*.otv;*.oct|(*)|*"),
		       wxFD_OPEN | wxFD_MULTIPLE);
  
  // set default params
  string appImpDir = p_app->getImportDir();
  string appCurDir = p_app->getCwd();
  if ( ! appImpDir.empty() )
    fileDlg.SetDirectory(vsnApp::ConvSysToWx(appImpDir));
  else if ( ! appCurDir.empty() )
    fileDlg.SetDirectory(vsnApp::ConvSysToWx(appCurDir));

  // get file path from the dialog
  if ( fileDlg.ShowModal() != wxID_OK ) return;

  // import the file to the current scene
  string cmd = string("<command target=\"") + targScn
    + string("\" name=\"import_data\">\n");

  wxArrayString filePathes;
  fileDlg.GetPaths(filePathes);
  if ( filePathes.GetCount() == 1 ) {
    string filePath =
      vsnPath_normalize(vsnApp::ConvWxToSys(fileDlg.GetPath()));
    if ( filePath.empty() ) return;
    cmd += string("<data type=\"OctVol\" file=\"");
    cmd += filePath;
    cmd += string("\" />\n");
  }
  else if ( filePathes.GetCount() > 1 ) {
    wxArrayString fileNames;
    fileDlg.GetFilenames(fileNames);
    fileNames.Sort();
    string baseDir = vsnApp::ConvWxToSys(fileDlg.GetDirectory());
    cmd += string("<data type=\"OctVol\" file=\"") +vsn_seqfile +string("\" ");
    if ( ! baseDir.empty() ) {
      cmd += string("base_dir=\"") + baseDir + string("\"");
    }
    cmd += string(">\n");
    register size_t i;
    for ( i = 0; i < fileNames.GetCount(); i++ ) {
      if ( fileNames[i].IsEmpty() ) continue;
      cmd += string("<seq file=\"") +
	vsnApp::ConvWxToSys(fileNames[i]) +
	string("\" />\n");
    } // end of for(i)
    cmd += string("</data>\n");
  }
  else return;

  cmd += string("</command>");
  if ( p_app->parseXMLCommand(cmd) ) {
    vsnDataObj* pld = p_scene->getLastLoadedData();
    if ( pld && m_pUiView ) m_pUiView->selectObj(pld);
  }
}

void vsnViewFrame::OnMenuFile_Imp_Scatter(wxCommandEvent& event) {
  if ( ! p_app ) return;
  if ( ! p_scene ) return;
  string targScn = p_scene->getName();
  if ( targScn.empty() || targScn == VFR_NONAME ) {
    ErrMsg(MsgERR, string("Import Scatter data: can't import data\n")
	   + string(" to the scene with no name"));
    return;
  }

  // prepare file dialog
  wxFileDialog fileDlg(this, wxT("select SCAT/PWN file(s) to import"),
		       wxT(""), wxT(""), // default Dir / File
		       wxT("Scatter (*.scat;*.sct)|*.scat;*.sct")
		       wxT("|Points with Normal (*.pwn)|*.pwn")
		       /* wxT("|(*)|*") */,
		       wxFD_OPEN | wxFD_MULTIPLE);
  
  // set default params
  string appImpDir = p_app->getImportDir();
  string appCurDir = p_app->getCwd();
  if ( ! appImpDir.empty() )
    fileDlg.SetDirectory(vsnApp::ConvSysToWx(appImpDir));
  else if ( ! appCurDir.empty() )
    fileDlg.SetDirectory(vsnApp::ConvSysToWx(appCurDir));

  // get file path from the dialog
  if ( fileDlg.ShowModal() != wxID_OK ) return;

  // import the file to the current scene
  string cmdPfx = string("<command target=\"") + targScn
    + string("\" name=\"import_data\">\n");
  string cmd;

  wxArrayString filePathes;
  fileDlg.GetPaths(filePathes);
  if ( filePathes.GetCount() > 0 ) {
    wxArrayString fileNames;
    fileDlg.GetFilenames(fileNames);
    fileNames.Sort();
    string baseDir =
      vsnPath_normalize(vsnApp::ConvWxToSys(fileDlg.GetDirectory()));
    register size_t i;
    for ( i = 0; i < fileNames.GetCount(); i++ ) {
      if ( fileNames[i].IsEmpty() ) continue;
      cmd = cmdPfx;
      cmd += string("<data type=\"Scatter\" file=\"");
      if ( ! baseDir.empty() ) cmd += baseDir + "/";
      cmd += vsnApp::ConvWxToSys(fileNames[i]);
      string filePath = vsnApp::ConvWxToSys(fileNames[i]);
      if ( filePath.size() >= 3 ) {
	string sfx = filePath.substr(filePath.size() - 3);
	if ( sfx == "sct" || sfx == "SCT" ) cmd += string(";SCT");
	if ( sfx == "pwn" || sfx == "PWN" ) cmd += string(";PWN");
      }
      cmd += string("\" />\n");
      cmd += string("</command>");
      if ( p_app->parseXMLCommand(cmd) ) {
	vsnDataObj* pld = p_scene->getLastLoadedData();
	if ( pld && m_pUiView ) m_pUiView->selectObj(pld);
      }
    } // end of for(i)
  }
}

void vsnViewFrame::OnMenuFile_Imp_ScatterTS(wxCommandEvent& event) {
  if ( ! p_app ) return;
  if ( ! p_scene ) return;
  string targScn = p_scene->getName();
  if ( targScn.empty() || targScn == VFR_NONAME ) {
    ErrMsg(MsgERR, string("Import Scatter data: can't import data\n")
	   + string(" to the scene with no name"));
    return;
  }

  // prepare file dialog
  wxFileDialog fileDlg(this, wxT("select SCAT/PWN file(s) to import"),
		       wxT(""), wxT(""), // default Dir / File
		       wxT("Scatter (*.scat;*.sct)|*.scat;*.sct")
		       wxT("|Points with Normal (*.pwn)|*.pwn")
		       /* wxT("|(*)|*") */,
		       wxFD_OPEN | wxFD_MULTIPLE);
  
  // set default params
  string appImpDir = p_app->getImportDir();
  string appCurDir = p_app->getCwd();
  if ( ! appImpDir.empty() )
    fileDlg.SetDirectory(vsnApp::ConvSysToWx(appImpDir));
  else if ( ! appCurDir.empty() )
    fileDlg.SetDirectory(vsnApp::ConvSysToWx(appCurDir));

  // get file path from the dialog
  if ( fileDlg.ShowModal() != wxID_OK ) return;

  // import the file to the current scene
  string cmd = string("<command target=\"") + targScn
    + string("\" name=\"import_data\">\n");

  wxArrayString filePathes;
  fileDlg.GetPaths(filePathes);
  if ( filePathes.GetCount() == 1 ) {
    string filePath =
      vsnPath_normalize(vsnApp::ConvWxToSys(fileDlg.GetPath()));
    if ( filePath.empty() ) return;
    cmd += string("<data type=\"Scatter\" file=\"");
    cmd += filePath;
    if ( filePath.size() >= 3 ) {
      string sfx = filePath.substr(filePath.size() - 3);
      if ( sfx == "sct" || sfx == "SCT" ) cmd += string(";SCT");
      if ( sfx == "pwn" || sfx == "PWN" ) cmd += string(";PWN");
    }
    cmd += string("\" />\n");
  }
  else if ( filePathes.GetCount() > 1 ) {
    wxArrayString fileNames;
    fileDlg.GetFilenames(fileNames);
    fileNames.Sort();
    string baseDir =
      vsnPath_normalize(vsnApp::ConvWxToSys(fileDlg.GetDirectory()));
    cmd += string("<data type=\"Scatter\" file=\"")+vsn_seqfile +string("\" ");
    if ( ! baseDir.empty() ) {
      cmd += string("base_dir=\"") + baseDir + string("\"");
    }
    cmd += string(">\n");
    register size_t i;
    for ( i = 0; i < fileNames.GetCount(); i++ ) {
      if ( fileNames[i].IsEmpty() ) continue;
      cmd += string("<seq file=\"") + vsnApp::ConvWxToSys(fileNames[i]);
      string filePath = vsnApp::ConvWxToSys(fileNames[i]);
      if ( filePath.size() >= 3 ) {
	string sfx = filePath.substr(filePath.size() - 3);
	if ( sfx == "sct" || sfx == "SCT" ) cmd += string(";SCT");
	if ( sfx == "pwn" || sfx == "PWN" ) cmd += string(";PWN");
      }
      cmd += string("\" />\n");
    } // end of for(i)
    cmd += string("</data>\n");
  }
  else return;

  cmd += string("</command>");
  if ( p_app->parseXMLCommand(cmd) ) {
    vsnDataObj* pld = p_scene->getLastLoadedData();
    if ( pld && m_pUiView ) m_pUiView->selectObj(pld);
  }
}

void vsnViewFrame::OnMenuFile_Imp_Shape(wxCommandEvent& event) {
  if ( ! p_app ) return;
  if ( ! p_scene ) return;
  string targScn = p_scene->getName();
  if ( targScn.empty() || targScn == VFR_NONAME ) {
    ErrMsg(MsgERR, string("Import Shape data: can't import data\n")
	   + string(" to the scene with no name"));
    return;
  }

  // prepare file dialog
  wxFileDialog fileDlg(this, wxT("select Obj/STL file(s) to import"),
		       wxT(""), wxT(""), // default Dir / File
		       wxT("WaveFront Obj (*.obj;*.wfo)|*.obj;*.wfo")
                       wxT("|STL (*.stl;*.sla;*.stla;*.slb;*.stlb)")
                       wxT("|*.stl;*.sla;*.stla;*.slb;*.stlb")
		       wxT("|STL Ascii (*.sla;*.stla;*.stl)|*.sla;*.stla;*.stl")
		       wxT("|STL Binary (*.slb;*.stlb;*.stl)")
		       wxT("|*.slb;*.stlb;*.stl")
		       wxT("|STL Binary big-endian (*.slb;*.stlb;*.stl)")
		       wxT("|*.slb;*.stlb;*.stl")
		       wxT("|(*)|*"),
		       wxFD_OPEN | wxFD_MULTIPLE);
  
  // set default params
  string appImpDir = p_app->getImportDir();
  string appCurDir = p_app->getCwd();
  if ( ! appImpDir.empty() )
    fileDlg.SetDirectory(vsnApp::ConvSysToWx(appImpDir));
  else if ( ! appCurDir.empty() )
    fileDlg.SetDirectory(vsnApp::ConvSysToWx(appCurDir));

  // get file path from the dialog
  if ( fileDlg.ShowModal() != wxID_OK ) return;

  // import the file to the current scene
  string cmdPfx = string("<command target=\"") + targScn
    + string("\" name=\"import_data\">\n");
  string cmd;

  wxArrayString filePathes;
  fileDlg.GetPaths(filePathes);
  if ( filePathes.GetCount() > 0 ) {
    wxArrayString fileNames;
    fileDlg.GetFilenames(fileNames);
    fileNames.Sort();
    string baseDir =
      vsnPath_normalize(vsnApp::ConvWxToSys(fileDlg.GetDirectory()));
    register size_t i;
    for ( i = 0; i < fileNames.GetCount(); i++ ) {
      if ( fileNames[i].IsEmpty() ) continue;
      cmd = cmdPfx;
      cmd += string("<data type=\"Shape\" file=\"");
      if ( ! baseDir.empty() ) cmd += baseDir + "/";
      cmd += vsnApp::ConvWxToSys(fileNames[i]);
      string filePath
	= vsnPath_normalize(vsnApp::ConvWxToSys(filePathes[i]));
      if ( filePath.size() >= 3 ) {
	string sfx = filePath.substr(filePath.size() - 3);
	if ( sfx == "obj" || sfx == "OBJ" || sfx == "wfo" || sfx == "WFO" )
	  cmd += string(";OBJ");
	else {
	  if ( vfruStl::IsAscii(filePath.c_str()) )
	    cmd += string(";SLA");
	  else
	    cmd += string(";SLB");
	}
      }
      cmd += string("\" />\n");
      cmd += string("</command>");
      if ( p_app->parseXMLCommand(cmd) ) {
	vsnDataObj* pld = p_scene->getLastLoadedData();
	if ( pld && m_pUiView ) m_pUiView->selectObj(pld);
      }
    } // end of for(i)
  }
}

void vsnViewFrame::OnMenuFile_Imp_ShapeTS(wxCommandEvent& event) {
  if ( ! p_app ) return;
  if ( ! p_scene ) return;
  string targScn = p_scene->getName();
  if ( targScn.empty() || targScn == VFR_NONAME ) {
    ErrMsg(MsgERR, string("Import Shape data: can't import data\n")
	   + string(" to the scene with no name"));
    return;
  }

  // prepare file dialog
  wxFileDialog fileDlg(this, wxT("select Obj/STL file(s) to import"),
		       wxT(""), wxT(""), // default Dir / File
		       wxT("WaveFront Obj (*.obj;*.wfo)|*.obj;*.wfo")
                       wxT("|STL (*.stl;*.sla;*.stla;*.slb;*.stlb)")
                       wxT("|*.stl;*.sla;*.stla;*.slb;*.stlb")
		       wxT("|STL Ascii (*.sla;*.stla;*.stl)|*.sla;*.stla;*.stl")
		       wxT("|STL Binary (*.slb;*.stlb;*.stl)")
		       wxT("|*.slb;*.stlb;*.stl")
		       wxT("|STL Binary big-endian (*.slb;*.stlb;*.stl)")
		       wxT("|*.slb;*.stlb;*.stl")
		       wxT("|(*)|*"),
		       wxFD_OPEN | wxFD_MULTIPLE);
  
  // set default params
  string appImpDir = p_app->getImportDir();
  string appCurDir = p_app->getCwd();
  if ( ! appImpDir.empty() )
    fileDlg.SetDirectory(vsnApp::ConvSysToWx(appImpDir));
  else if ( ! appCurDir.empty() )
    fileDlg.SetDirectory(vsnApp::ConvSysToWx(appCurDir));

  // get file path from the dialog
  if ( fileDlg.ShowModal() != wxID_OK ) return;

  // import the file to the current scene
  string cmd = string("<command target=\"") + targScn
    + string("\" name=\"import_data\">\n");

  wxArrayString filePathes;
  fileDlg.GetPaths(filePathes);
  if ( filePathes.GetCount() == 1 ) {
    string filePath =
      vsnPath_normalize(vsnApp::ConvWxToSys(fileDlg.GetPath()));
    if ( filePath.empty() ) return;
    cmd += string("<data type=\"Shape\" file=\"");
    cmd += filePath;
    if ( filePath.size() >= 3 ) {
      string sfx = filePath.substr(filePath.size() - 3);
      if ( sfx == "obj" || sfx == "OBJ" || sfx == "wfo" || sfx == "WFO" )
	cmd += string(";OBJ");
      else {
	if ( vfruStl::IsAscii(filePath.c_str()) )
	  cmd += string(";SLA");
	else
	  cmd += string(";SLB");
      }
    }
    cmd += string("\" />\n");
  }
  else if ( filePathes.GetCount() > 1 ) {
    wxArrayString fileNames;
    fileDlg.GetFilenames(fileNames);
    fileNames.Sort();
    string baseDir =
      vsnPath_normalize(vsnApp::ConvWxToSys(fileDlg.GetDirectory()));
    cmd += string("<data type=\"Shape\" file=\"") +vsn_seqfile +string("\" ");
    if ( ! baseDir.empty() ) {
      cmd += string("base_dir=\"") + baseDir + string("\"");
    }
    cmd += string(">\n");
    register size_t i;
    for ( i = 0; i < fileNames.GetCount(); i++ ) {
      if ( fileNames[i].IsEmpty() ) continue;
      cmd += string("<seq file=\"") + vsnApp::ConvWxToSys(fileNames[i]);
      string filePath
	= vsnPath_normalize(vsnApp::ConvWxToSys(fileNames[i]));
      if ( filePath.size() >= 3 ) {
	string sfx = filePath.substr(filePath.size() - 3);
	if ( sfx == "obj" || sfx == "OBJ" || sfx == "wfo" || sfx == "WFO" )
	  cmd += string(";OBJ");
	else {
	  if ( vfruStl::IsAscii((baseDir+"/"+filePath).c_str()) )
	    cmd += string(";SLA");
	  else
	    cmd += string(";SLB");
	}
      }
      cmd += string("\" />\n");
    } // end of for(i)
    cmd += string("</data>\n");
  }
  else return;

  cmd += string("</command>");
  if ( p_app->parseXMLCommand(cmd) ) {
    vsnDataObj* pld = p_scene->getLastLoadedData();
    if ( pld && m_pUiView ) m_pUiView->selectObj(pld);
  }
}

void vsnViewFrame::OnMenuFile_Imp_ShapeLOD(wxCommandEvent& event) {
  if ( ! p_app ) return;
  if ( ! p_scene ) return;
  string targScn = p_scene->getName();
  if ( targScn.empty() || targScn == VFR_NONAME ) {
    ErrMsg(MsgERR, string("Import Shape data: can't import data\n")
	   + string(" to the scene with no name"));
    return;
  }

  // prepare file dialog
  wxFileDialog fileDlg(this, wxT("select Obj/STL file(s) to import"),
		       wxT(""), wxT(""), // default Dir / File
		       wxT("WaveFront Obj (*.obj;*.wfo)|*.obj;*.wfo")
                       wxT("|STL (*.stl;*.sla;*.stla;*.slb;*.stlb)")
                       wxT("|*.stl;*.sla;*.stla;*.slb;*.stlb")
		       wxT("|STL Ascii (*.sla;*.stla;*.stl)|*.sla;*.stla;*.stl")
		       wxT("|STL Binary (*.slb;*.stlb;*.stl)")
		       wxT("|*.slb;*.stlb;*.stl")
		       wxT("|STL Binary big-endian (*.slb;*.stlb;*.stl)")
		       wxT("|*.slb;*.stlb;*.stl")
		       wxT("|(*)|*"),
		       wxFD_OPEN | wxFD_MULTIPLE);
  
  // set default params
  string appImpDir = p_app->getImportDir();
  string appCurDir = p_app->getCwd();
  if ( ! appImpDir.empty() )
    fileDlg.SetDirectory(vsnApp::ConvSysToWx(appImpDir));
  else if ( ! appCurDir.empty() )
    fileDlg.SetDirectory(vsnApp::ConvSysToWx(appCurDir));

  // get file path from the dialog
  if ( fileDlg.ShowModal() != wxID_OK ) return;

  // import the file to the current scene
  string cmd = string("<command target=\"") + targScn
    + string("\" name=\"import_data\">\n");

  wxArrayString filePathes;
  fileDlg.GetPaths(filePathes);
  if ( filePathes.GetCount() >= 1 ) {
    wxArrayString fileNames;
    fileDlg.GetFilenames(fileNames);
    fileNames.Sort();
    string baseDir =
      vsnPath_normalize(vsnApp::ConvWxToSys(fileDlg.GetDirectory()));
    cmd += string("<data type=\"ShapeLOD\" file=\"")+vsn_seqfile+string("\" ");
    if ( ! baseDir.empty() ) {
      cmd += string("base_dir=\"") + baseDir + string("\"");
    }
    cmd += string(">\n");
    register size_t i;
    for ( i = 0; i < fileNames.GetCount(); i++ ) {
      if ( fileNames[i].IsEmpty() ) continue;
      cmd += string("<seq file=\"") + vsnApp::ConvWxToSys(fileNames[i]);
      string filePath
	= vsnPath_normalize(vsnApp::ConvWxToSys(fileNames[i]));
      if ( filePath.size() >= 3 ) {
	string sfx = filePath.substr(filePath.size() - 3);
	if ( sfx == "obj" || sfx == "OBJ" || sfx == "wfo" || sfx == "WFO" )
	  cmd += string(";OBJ");
	else {
	  if ( vfruStl::IsAscii(filePath.c_str()) )
	    cmd += string(";SLA");
	  else
	    cmd += string(";SLB");
	}
      }
      cmd += string("\" />\n");
    } // end of for(i)
    cmd += string("</data>\n");
  }
  else return;

  cmd += string("</command>");
  if ( p_app->parseXMLCommand(cmd) ) {
    vsnDataObj* pld = p_scene->getLastLoadedData();
    if ( pld && m_pUiView ) m_pUiView->selectObj(pld);
  }
}

void vsnViewFrame::OnMenuFile_Imp_SvSQL(wxCommandEvent& event) {
#ifdef PGSQL
  if ( ! p_app ) return;
  if ( ! p_scene ) return;
  string targScn = p_scene->getName();
  if ( targScn.empty() || targScn == VFR_NONAME ) {
    ErrMsg(MsgERR, string("Import SvSQL data: can't import data\n")
	   + string(" to the scene with no name"));
    return;
  }

  // import the file to the current scene
  string cmd = string("<command target=\"") + targScn
    + string("\" name=\"import_data\">\n");
  cmd += string("<data type=\"SvSQL\" />\n");
  cmd += string("</command>");

  if ( p_app->parseXMLCommand(cmd) ) {
    vsnDataObj* pld = p_scene->getLastLoadedData();
    if ( pld && m_pUiView ) m_pUiView->selectObj(pld);
  }
#endif // PGSQL
}

void vsnViewFrame::OnMenuFile_Imp_TriaCells(wxCommandEvent& event) {
  if ( ! p_app ) return;
  if ( ! p_scene ) return;
  string targScn = p_scene->getName();
  if ( targScn.empty() || targScn == VFR_NONAME ) {
    ErrMsg(MsgERR, string("Import Triangle cells data: can't import data\n")
	   + string(" to the scene with no name"));
    return;
  }

  // prepare file dialog
  wxFileDialog fileDlg(this, wxT("select Triangle cells file(s) to import"),
		       wxT(""), wxT(""), // default Dir / File
		       wxT("VTK (*.vtk)|*.vtk")
		       wxT("|(*)|*"),
		       wxFD_OPEN | wxFD_MULTIPLE);

  // set default params
  string appImpDir = p_app->getImportDir();
  string appCurDir = p_app->getCwd();
  if ( ! appImpDir.empty() )
    fileDlg.SetDirectory(vsnApp::ConvSysToWx(appImpDir));
  else if ( ! appCurDir.empty() )
    fileDlg.SetDirectory(vsnApp::ConvSysToWx(appCurDir));

  // get file path from the dialog
  if ( fileDlg.ShowModal() != wxID_OK ) return;

  // import the file to the current scene
  string cmdPfx = string("<command target=\"") + targScn
    + string("\" name=\"import_data\">\n");
  string cmd;

  wxArrayString filePathes;
  fileDlg.GetPaths(filePathes);
  if ( filePathes.GetCount() > 0 ) {
    wxArrayString fileNames;
    fileDlg.GetFilenames(fileNames);
    fileNames.Sort();
    string baseDir =
      vsnPath_normalize(vsnApp::ConvWxToSys(fileDlg.GetDirectory()));
    register size_t i;
    for ( i = 0; i < fileNames.GetCount(); i++ ) {
      if ( fileNames[i].IsEmpty() ) continue;
      cmd = cmdPfx;
      cmd += string("<data type=\"TriaCells\" file=\"");
      if ( ! baseDir.empty() ) cmd += baseDir + "/";
      cmd += vsnApp::ConvWxToSys(fileNames[i]);
      cmd += string(";VTK");
      cmd += string("\" />\n");
      cmd += string("</command>");
      if ( p_app->parseXMLCommand(cmd) ) {
	vsnDataObj* pld = p_scene->getLastLoadedData();
	if ( pld && m_pUiView ) m_pUiView->selectObj(pld);
      }
    } // end of for(i)
  }
}

void vsnViewFrame::OnMenuFile_Imp_TriaCellsTS(wxCommandEvent& event) {
  if ( ! p_app ) return;
  if ( ! p_scene ) return;
  string targScn = p_scene->getName();
  if ( targScn.empty() || targScn == VFR_NONAME ) {
    ErrMsg(MsgERR, string("Import Tria cells data: can't import data\n")
	   + string(" to the scene with no name"));
    return;
  }

  // prepare file dialog
  wxFileDialog fileDlg(this, wxT("select Tria cells file(s) to import"),
		       wxT(""), wxT(""), // default Dir / File
		       wxT("VTK (*.vtk)|*.vtk")
		       wxT("|(*)|*"),
		       wxFD_OPEN | wxFD_MULTIPLE);
  
  // set default params
  string appImpDir = p_app->getImportDir();
  string appCurDir = p_app->getCwd();
  if ( ! appImpDir.empty() )
    fileDlg.SetDirectory(vsnApp::ConvSysToWx(appImpDir));
  else if ( ! appCurDir.empty() )
    fileDlg.SetDirectory(vsnApp::ConvSysToWx(appCurDir));

  // get file path from the dialog
  if ( fileDlg.ShowModal() != wxID_OK ) return;

  // import the file to the current scene
  string cmd = string("<command target=\"") + targScn
    + string("\" name=\"import_data\">\n");

  wxArrayString filePathes;
  fileDlg.GetPaths(filePathes);
  if ( filePathes.GetCount() == 1 ) {
    string filePath =
      vsnPath_normalize(vsnApp::ConvWxToSys(fileDlg.GetPath()));
    if ( filePath.empty() ) return;
    cmd += string("<data type=\"TriaCells\" file=\"");
    cmd += filePath;
    cmd += string(";VTK");
    cmd += string("\" />\n");
  }
  else if ( filePathes.GetCount() > 1 ) {
    wxArrayString fileNames;
    fileDlg.GetFilenames(fileNames);
    fileNames.Sort();
    string baseDir =
      vsnPath_normalize(vsnApp::ConvWxToSys(fileDlg.GetDirectory()));
    cmd += string("<data type=\"TriaCells\" file=\"")
      + vsn_seqfile +string("\" ");
    if ( ! baseDir.empty() ) {
      cmd += string("base_dir=\"") + baseDir + string("\"");
    }
    cmd += string(">\n");
    register size_t i;
    for ( i = 0; i < fileNames.GetCount(); i++ ) {
      if ( fileNames[i].IsEmpty() ) continue;
      cmd += string("<seq file=\"") + vsnApp::ConvWxToSys(fileNames[i]);
      cmd += string(";VTK");
      cmd += string("\" />\n");
    } // end of for(i)
    cmd += string("</data>\n");
  }
  else return;

  cmd += string("</command>");
  if ( p_app->parseXMLCommand(cmd) ) {
    vsnDataObj* pld = p_scene->getLastLoadedData();
    if ( pld && m_pUiView ) m_pUiView->selectObj(pld);
  }
}

void vsnViewFrame::OnMenuFile_Save(wxCommandEvent& event) {
  if ( ! p_app ) return;

  string outPath = p_app->getCurrentFilename();
  if ( outPath.empty() ) {
    wxFileDialog fileDlg(this, wxT("select vsn XML file to save"),
			 wxT(""), wxT(""), // default Dir / File
			 wxT("vsn files(*.xml;*.vfv;*.vsn)|*.xml;*.vfv;*.vsn|(*)|*"),
			 wxFD_SAVE);
    // set default dir
    string appCurDir = p_app->getCwd();
    if ( ! appCurDir.empty() )
      fileDlg.SetDirectory(vsnApp::ConvSysToWx(appCurDir));

    // get output path
    if ( fileDlg.ShowModal() != wxID_OK ) return;
    outPath = vsnPath_normalize(vsnApp::ConvWxToSys(fileDlg.GetPath()));
    if ( outPath.empty() ) return;

    // override check
    FILE* ofp = fopen(outPath.c_str(), "r");
    if ( ofp ) {
      fclose(ofp);
      string msg = "The specified file has already existed\n  ";
      msg += outPath;
      msg += "\n\nAre you sure to override ?\n";
      wxMessageDialog dlg(NULL, vsnApp::ConvSysToWx(msg),
			  wxT("SaveAs"), vsn_wxOK_CANCEL|wxICON_QUESTION);
      if ( dlg.ShowModal() != vsn_wxIDOK ) return;
    }
  } // end of if(outPath.empty())

  p_app->outputXMLFile(outPath);
}

void vsnViewFrame::OnMenuFile_SaveAs(wxCommandEvent& event) {
  if ( ! p_app ) return;

  wxFileDialog fileDlg(this, wxT("select vsn XML file to save"),
		       wxT(""), wxT(""), // default Dir / File
		       wxT("vsn files(*.xml;*.vfv;*.vsn)|*.xml;*.vfv;*.vsn|(*)|*"),
		       wxFD_SAVE);
  // set default dir
  string targDir;
  string oldPath = p_app->getCurrentFilename();
  if ( ! oldPath.empty() ) {
    targDir = DirName(oldPath, vsnPath_getDelimChar());
    if ( ! targDir.empty() )
      fileDlg.SetDirectory(vsnApp::ConvSysToWx(targDir));
  }
  if ( targDir.empty() ) {
    targDir = p_app->getCwd();
    if ( ! targDir.empty() )
      fileDlg.SetDirectory(vsnApp::ConvSysToWx(targDir));
  }

  // get output path
  if ( fileDlg.ShowModal() != wxID_OK ) return;
  string outPath = vsnPath_normalize(vsnApp::ConvWxToSys(fileDlg.GetPath()));
  if ( outPath.empty() ) return;

  // override check
  FILE* ofp = fopen(outPath.c_str(), "r");
  if ( ofp ) {
    fclose(ofp);
    string msg = "The specified file has already existed\n  ";
    msg += outPath;
    msg += "\n\nAre you sure to override ?\n";
    wxMessageDialog dlg(NULL, vsnApp::ConvSysToWx(msg),
			wxT("SaveAs"), vsn_wxOK_CANCEL|wxICON_QUESTION);
    if ( dlg.ShowModal() != vsn_wxIDOK ) return;
  }

  p_app->outputXMLFile(outPath);
}

void vsnViewFrame::OnMenuFile_OprProp(wxCommandEvent& event) {
  if ( ! p_app ) return;

  vsnPropDlg dlg(this);
  dlg.ShowModal();
}

void vsnViewFrame::OnMenuFile_NewParamCanvas(wxCommandEvent& event) {
  if ( ! p_app ) return;
  vsnMethodParamCanvas* pmpc = p_app->addMethodParamCanvas();
  if ( ! pmpc ) return;
  pmpc->Show();
}

void vsnViewFrame::OnMenuFile_ParamCanvasList(wxCommandEvent& event) {
  if ( ! p_app ) return;
  vsnMPCListDlg dlg(this, p_app->getMethodParamCanvasList());
  dlg.ShowModal();
}

void vsnViewFrame::OnMenuFile_ExecCommand(wxCommandEvent& event) {
  if ( ! p_app ) return;
  vsnTextEntryBrowsDlg dlg(this, wxT("commandline to invoke"),
			   wxT("Exec command"));
  if ( dlg.ShowModal() != wxID_OK ) return;

  wxString comm = dlg.GetValue();
  if ( comm.IsEmpty() ) return;
  long pid = wxExecute(comm);
  if ( pid < 1 ) {
    wxMessageDialog dlg(this, wxT("Execute external command failed"),
			wxT("Exec command"),
			wxOK|wxICON_INFORMATION);
    dlg.ShowModal();
  }
}


void vsnViewFrame::OnMenuFile_Quit(wxCommandEvent& event) {
#if 0
  string cmd("<command target=\"app\" name=\"quit\" />");
  if ( p_app ) p_app->parseXMLCommand(cmd);
#else
  if ( p_app ) p_app->quit(true);
  else exit(0);
#endif
}


void vsnViewFrame::OnMenuView_New(wxCommandEvent& event) {
  vsnViewFrame* pnvf = vsnApp::GetApp()->addViewFrame();
  if ( ! pnvf ) {
    ErrMsg(MsgERR, "can't create new ViewFrame");
    return;
  }
  if ( p_scene ) pnvf->setScene(p_scene);
  pnvf->Show();
}

void vsnViewFrame::OnMenuView_Close(wxCommandEvent& event) {
  closeView();
}

void vsnViewFrame::OnMenuView_Geometry(wxCommandEvent& event) {
  vsnViewFrameWinGeomDlg dlg(this);
  dlg.ShowModal();
}

void vsnViewFrame::OnMenuView_Layout(wxCommandEvent& event) {
  vsnViewFrameWinLayoutDlg dlg(this);
  dlg.ShowModal();
}

void vsnViewFrame::OnMenuView_UiShow(wxCommandEvent& event) {
  setShowUiView(event.IsChecked());
}

void vsnViewFrame::OnMenuView_AuxUiShow(wxCommandEvent& event) {
  setShowAuxUiFrame(event.IsChecked());
}

void vsnViewFrame::OnMenuView_ShowToolBar(wxCommandEvent& event) {
  setShowToolBar(event.IsChecked());
}

void vsnViewFrame::OnMenuView_Perspective(wxCommandEvent& event) {
  if ( ! m_pGfxView ) return;

  ProjectType pm = event.IsChecked() ? PR_PERSPECTIVE : PR_ORTHOGONAL;
  if ( m_pGfxView->setProjection(pm) )
    m_pGfxView->getDrawArea()->chkNotice();
}

void vsnViewFrame::OnMenuView_SetBgColor(wxCommandEvent& event) {
  vector4 cv; getBgColor(cv);
  wxColour ccol((unsigned char)(cv[0]*255.f),
                (unsigned char)(cv[1]*255.f),
                (unsigned char)(cv[2]*255.f));
  wxColourData cdata;
  cdata.SetColour(ccol);
  cdata.SetChooseFull(TRUE);

  register int c;
  for ( c = 0; c < m_wxColorLst.size(); c++ )
    cdata.SetCustomColour(c%16, m_wxColorLst[c]);

  wxColourDialog cdlg(this, &cdata);
  cdlg.SetTitle(wxT("Choose background color"));
  if ( cdlg.ShowModal() != wxID_OK ) return;

  cdata = cdlg.GetColourData();
  wxColor rcol = cdata.GetColour();
  if ( ccol == rcol ) return;
  m_wxColorLst.push_back(rcol);
  cv[0] = rcol.Red() / 255.f;
  cv[1] = rcol.Green() / 255.f;
  cv[2] = rcol.Blue() / 255.f;
  setBgColor(cv);
}

void vsnViewFrame::OnMenuView_Normalize(wxCommandEvent& event) {
  normalizeViewport(false);
}

void vsnViewFrame::OnMenuView_NormalizeScene(wxCommandEvent& event) {
  normalizeViewport(true);
}

void vsnViewFrame::normalizeViewport(const bool tgtScene) {
  if ( ! m_pGfxView || ! m_pUiView ) return;
  vfrNode* pTgt = NULL;
  if ( ! tgtScene ) {
    pTgt = m_pUiView->getCurrentMethod();
    vsnMethodObj* pMtd = dynamic_cast<vsnMethodObj*>(pTgt);
    if ( ! pMtd || ! pMtd->hasBbox() )
      pTgt = m_pUiView->getCurrentData();
  }
  if ( m_pGfxView->normalizeView(m_normalizeCenter, pTgt) )
    m_pGfxView->getDrawArea()->chkNotice();
  Refresh();
}

void vsnViewFrame::OnMenuView_SetViewPoint(wxCommandEvent& event) {
  if ( ! m_pGfxView ) return;

  if ( ! m_pViewPointDlg ) {
    m_pViewPointDlg = new vsnViewPointDlg(this, NULL);
    if ( ! m_pViewPointDlg ) {
      ErrMsg(MsgERR, string("ViewFrame: can't create ViewPointDlg"));
      return;
    }
  } // end of if(!m_pViewPointDlg)

  m_pViewPointDlg->setRefViewFrame(this);
  m_pViewPointDlg->Show(TRUE);
}

void vsnViewFrame::OnMenuView_SyncViewPoint(wxCommandEvent& event) {
  vsnApp* pApp = vsnApp::GetApp();
  if ( ! pApp ) return;
  size_t nvf = pApp->getNumViewFrame();
  if ( nvf <= 1 ) {
    ErrMsg(MsgINFO, "Sync ViewPoint: no valid ViewFrame to sync ViewPoint");
    return;
  }

  size_t i;
  int preSel = 0, vpcnt = 1;
  wxString* choices = new wxString[nvf];
  choices[0] = wxT("-- Not Sync --");
  for ( i = 0; i < nvf; i++ ) {
    vsnViewFrame* pvf = pApp->getViewFrame(i);
    if ( ! pvf || pvf == this ) continue;
    choices[vpcnt] = vsnApp::ConvSysToWx(pvf->getName());
    if ( p_vpRefMaster && p_vpRefMaster->getName() == pvf->getName() )
      preSel = (int)vpcnt;
    vpcnt++;
  } // end of for(i)

  wxSingleChoiceDialog
    dlg(this, wxT("select the ViewFrame to sync ViewPoint"),
        wxT("sync view point"), vpcnt, choices);
  delete [] choices;
  dlg.SetSelection(preSel);
  if ( dlg.ShowModal() != wxID_OK ) return;

  int val = dlg.GetSelection();
  if ( val < 0 || val >= vpcnt ) {
    ErrMsg(MsgERR, "Sync ViewPoint: invalid selection");
    return;
  }
  if ( val == 0 ) {
    setViewPointMaster(NULL);
    return;
  }
  wxString valStr = dlg.GetStringSelection();
  vsnViewFrame* pvf = pApp->getViewFrame(vsnApp::ConvWxToSys(valStr));
  if ( ! pvf ) {
    ErrMsg(MsgERR, "Sync ViewPoint: can't find the selected ViewFrame");
    return;
  }
  if ( ! setViewPointMaster(pvf) ) {
    ErrMsg(MsgERR, "Sync ViewPoint: can't set the selected ViewFrame to sync");
    return;
  }
}

void vsnViewFrame::OnMenuView_SetCenter(wxCommandEvent& event) {
  if ( ! m_pGfxView ) return;

  if ( event.IsChecked() ) {
    enterSetCenterMode(); // Start Set-Center mode
  }
  else {
    escapeSetCenterMode(); // End Set-Center mode
  }

  Refresh();
}

void vsnViewFrame::OnMenuView_CenterShow(wxCommandEvent& event) {
  setShowCenter(event.IsChecked());
}

void vsnViewFrame::OnMenuView_NormCenter(wxCommandEvent& event) {
  setNormCenter(event.IsChecked());
}

void vsnViewFrame::OnMenuView_FrAxisShow(wxCommandEvent& event) {
  setShowFrAxis(event.IsChecked());
}

void vsnViewFrame::OnMenuView_Shot(wxCommandEvent& event) {
  if ( ! p_app || ! m_pGfxView ) return;

  wxFileDialog fileDlg(this, wxT("select image file to save screenshot"),
                       wxT(""), wxT(""), // default Dir / File
#if defined(WINDOWS)
                       wxT("BMP files (*.bmp)|*.bmp|")
#endif
		       wxT("PNG files (*.png)|*.png|")
		       wxT("JPEG files (*.jpg)|*.jpg|")
//		       wxT("GIF files (*.gif)|*.gif|")
		       wxT("TIFF files (*.tif)|*.tif|")
#if !defined(WINDOWS)
                       wxT("BMP files (*.bmp)|*.bmp|")
#endif
		       wxT("(*)|*"),
                       wxFD_SAVE);
  // set default dir
  string shotDir = p_app->getScreenshotDir();
  string curDir = p_app->getCwd();
  if ( ! shotDir.empty() )
    fileDlg.SetDirectory(vsnApp::ConvSysToWx(shotDir));
  else if ( ! curDir.empty() )
    fileDlg.SetDirectory(vsnApp::ConvSysToWx(curDir));

  // get output path
  if ( fileDlg.ShowModal() != wxID_OK ) return;
  string outPath = vsnPath_normalize(vsnApp::ConvWxToSys(fileDlg.GetPath()));
  if ( outPath.empty() ) return;

  // override check
  FILE* ofp = fopen(outPath.c_str(), "r");
  if ( ofp ) {
    fclose(ofp);
    string msg = "The specified file has already existed\n  ";
    msg += outPath;
    msg += "\n\nAre you sure to override ?\n";
    wxMessageDialog dlg(NULL, vsnApp::ConvSysToWx(msg),
                        wxT("Screen Shot"), vsn_wxOK_CANCEL|wxICON_QUESTION);
    if ( dlg.ShowModal() != vsn_wxIDOK ) return;
  }

  if ( ! m_pGfxView->screenShot(outPath) ) {
    ErrMsg(MsgERR, string("Screen Shot failed: ") + outPath);
    return;
  }
  p_app->setScreenshotDir(DirName(outPath, vsnPath_getDelimChar()));
}

void vsnViewFrame::OnMenuView_ShotAndExec(wxCommandEvent& event) {
  if ( ! p_app || ! m_pGfxView ) return;
  string orgComm = getSshotCommStr();
  vsnTextEntryBrowsDlg dlg(this,
	   wxT("execute command (%s replaced to screenshot image file)"),
	   wxT("Screenshot and Exec command"), vsnApp::ConvSysToWx(orgComm));
  if ( dlg.ShowModal() != wxID_OK ) return;
  wxString comm = dlg.GetValue();
  if ( comm.IsEmpty() ) return;
  orgComm = vsnApp::ConvWxToSys(comm);

  wxString tmpPath = wxFileName::CreateTempFileName(wxT("vsnSshot"));
  tmpPath += wxT(".png");
  comm.Replace(wxT("%s"), tmpPath);

  if ( ! m_pGfxView->screenShot(vsnApp::ConvWxToSys(tmpPath)) ) {
    ErrMsg(MsgERR, string("Screen Shot to temporary file failed"));
    return;
  }

  long pid = wxExecute(comm);
  if ( pid < 1 ) {
    wxMessageDialog dlg(this, wxT("Execute external command failed"),
			wxT("Exec command"),
			wxOK|wxICON_INFORMATION);
    dlg.ShowModal();
    wxRemoveFile(tmpPath);
    return;
  }

  p_app->Yield();
  wxMilliSleep(5000L);
  wxRemoveFile(tmpPath);

  setSshotCommStr(orgComm);
}


void vsnViewFrame::OnMenuScene_Add(wxCommandEvent& event) {
  vsnScene* pnsc = vsnApp::GetApp()->addScene();
  if ( ! pnsc ) {
    ErrMsg(MsgERR, "can't create new Scene");
    return;
  }
  setScene(pnsc);
}

void vsnViewFrame::OnMenuScene_Del(wxCommandEvent& event) {
  if ( ! p_scene || ! p_app ) return;
  if ( ! p_app->delScene(p_scene) ) return;
}

void vsnViewFrame::OnMenuScene_Rename(wxCommandEvent& event) {
  if ( ! p_scene || ! p_app ) return;

  string oldName = p_scene->getName();
  if ( oldName.empty() ) oldName = VFR_NONAME;

  vsnTextEntryDlg dlg(this,
		      wxT("Enter new name of the current scene\n"),
		      wxT("rename scene"), vsnApp::ConvSysToWx(oldName));
  if ( dlg.ShowModal() != wxID_OK ) return;

  wxString newNameStr = dlg.GetValue();
  if ( newNameStr.IsEmpty() ) return;
  string newName = vsnApp::ConvWxToSys(newNameStr);
  if ( oldName == newName ) return;

  vsnScene* psc = p_app->getScene(newName);
  if ( psc ) {
    string msg = "can't set scene name: scene named ";
    msg += newName;
    msg += " has already exists";
    ErrMsg(MsgERR, msg);
    return;
  } // end of if(psc)

  p_scene->setName(newName);
  p_app->update();
}

void vsnViewFrame::OnMenuScene_Anchor(wxCommandEvent& event) {
  if ( ! p_scene || ! p_app ) return;
  vsnMethodObj* pMtd = m_pUiView->getCurrentMethod();
  if ( ! pMtd ) return;
  vsnAnchor* pAnc = dynamic_cast<vsnAnchor*>(pMtd);
  if ( ! pAnc ) return;

  vsnEditAnchorDlg ancDlg(this, pAnc);
  ancDlg.ShowModal();

  return;
}
void vsnViewFrame::OnUpdateMenuScene_Anchor(wxUpdateUIEvent& event) {
  if ( ! p_scene || ! p_app ) {event.Enable(false); return;}
  vsnMethodObj* pMtd = m_pUiView->getCurrentMethod();
  if ( ! pMtd ) {event.Enable(false); return;}
  vsnAnchor* pAnc = dynamic_cast<vsnAnchor*>(pMtd);
  if ( ! pAnc ) {event.Enable(false); return;}
  event.Enable(true);
}

#if 0
void vsnViewFrame::OnMenuScene_AddObjGrp(wxCommandEvent& event) {
  if ( ! p_scene || ! p_app ) return;
  return;
}
#endif

void vsnViewFrame::OnMenuScene_LightAttr(wxCommandEvent& event) {
  if ( ! m_pGfxView || ! p_app ) return;
  vsnLightAttribute& light = m_pGfxView->getScene()->getLightAttr();

  if ( ! m_pLightAttrDlg ) {
    m_pLightAttrDlg = new vsnLightAttributeDlg(this, &light);
    if ( ! m_pLightAttrDlg ) {
      ErrMsg(MsgERR, string("can't create LightAttributeDlg"));
      return;
    }
  }
  m_pLightAttrDlg->update();
  m_pLightAttrDlg->Show();
}


void vsnViewFrame::OnMenuHelp_UserGuide(wxCommandEvent& event) {
#if defined(LINUX)
  wxString commPDF(wxT("xdg-open "));
#elif defined(WINDOWS)
  wxString commPDF(wxT("start ")); // may not be used
#elif defined(MACOSX)
  wxString commPDF(wxT("open "));
#else
  return;
#endif
  wxString pathPDF(wxT("/../doc/Visio_UG.pdf"));
  pathPDF = vsnApp::ConvSysToWx(vsnApp::GetAppDir()) + pathPDF;

  wxString command = commPDF + pathPDF;
#if defined(WINDOWS)
  wxFileType* fileType
    = wxTheMimeTypesManager->GetFileTypeFromExtension(wxT("pdf"));
  command = fileType->GetOpenCommand(pathPDF);
#endif

  wxExecute(command);
}

void vsnViewFrame::OnMenuHelp_FileFmt(wxCommandEvent& event) {
#if defined(LINUX)
  wxString commPDF(wxT("xdg-open "));
#elif defined(WINDOWS)
  wxString commPDF(wxT("start ")); // may not be used
#elif defined(MACOSX)
  wxString commPDF(wxT("open "));
#else
  return;
#endif
  wxString pathPDF(wxT("/../doc/DFF_V-Tools.pdf"));
  pathPDF = vsnApp::ConvSysToWx(vsnApp::GetAppDir()) + pathPDF;

  wxString command = commPDF + pathPDF;
#if defined(WINDOWS)
  wxFileType* fileType
    = wxTheMimeTypesManager->GetFileTypeFromExtension(wxT("pdf"));
  command = fileType->GetOpenCommand(pathPDF);
#endif

  wxExecute(command);
}

void vsnViewFrame::OnMenuHelp_About(wxCommandEvent& event) {
  string msg = vsn_app_name +
    string(", a new V-Tools Visualizer  Version ") + vsn_version +
    string("    \n\n") + vsn_copyright;
  string title = string("About ") + vsn_app_name;
  wxMessageDialog dlg(this, vsnApp::ConvSysToWx(msg),vsnApp::ConvSysToWx(title),
                      wxOK|wxICON_INFORMATION);
  dlg.ShowModal();
}

void vsnViewFrame::OnToolBar_Normalize(wxCommandEvent& event) {
  normalizeViewport(false);
}

void vsnViewFrame::OnToolBar_Projection(wxCommandEvent& event) {
  vsnGfxView* pgv = getGfxView();
  if ( ! pgv ) return;

  ProjectType pm;
  switch ( event.GetId() ) {
  case ViewFrameTooBar_ProjPers:
    pm = PR_PERSPECTIVE; break;
  case ViewFrameTooBar_ProjOrtho:
    pm = PR_ORTHOGONAL; break;
  default:
    return;
  } // end of switch(event.GetId)
  
  if ( pgv->setProjection(pm) )
    pgv->getDrawArea()->chkNotice();
  
  wxMenuBar* pmb = GetMenuBar();
  if ( pmb ) {
    wxMenu* pvm = pmb->GetMenu(pmb->FindMenu(wxT("View")));
    if ( pvm )
      pvm->Check(ViewFrameMenu_View_Perspective, (pm == PR_PERSPECTIVE));
  } // end of if(pmb)

  Refresh();
}

void vsnViewFrame::OnToolBar_ViewDirs(wxCommandEvent& event) {
  vsnGfxView* pgv = getGfxView();
  if ( ! pgv ) return;
  vfrNode* prot = pgv->getRotateNode();
  if ( ! prot ) return;

  vsnViewPoint vp0 = pgv->getXForm();
  prot->identity();

  switch ( event.GetId() ) {
  case ViewFrameTooBar_Back:
    prot->roty((float)M_PI); break;
  case ViewFrameTooBar_Right:
    prot->roty(-HALF_PI); break;
  case ViewFrameTooBar_Left:
    prot->roty(HALF_PI); break;
  case ViewFrameTooBar_Top:
    prot->rotx(HALF_PI); break;
  case ViewFrameTooBar_Bottom:
    prot->rotx(-HALF_PI); break;
  case ViewFrameTooBar_Front: default:
    break;
  } // end of switch(event.GetId)
  
  if ( ! pgv->normalize(getNormCenter()) ) {
    pgv->setXForm(vp0);
    return;
  }
  if ( vsnGfxView::GetXformAnim() ) {
    vsnViewPoint vp1 = pgv->getXForm();
    pgv->setXForm(vp0);
    pgv->setViewXForm(vp1);
  }
  else
    prot->chkNotice();

  Refresh();
}

void vsnViewFrame::OnToolBar_AnimControl(wxCommandEvent& event) {
  if ( ! p_scene ) return;
  wxToolBar *ptb = GetToolBar();
  if ( ! ptb ) return;
  wxControl* pTBAS = ptb->FindControl(ViewFrameTooBar_AnimSelCombo);
  wxComboBox* pTBAnimSelector = (wxComboBox*)pTBAS;
  if ( ! pTBAnimSelector ) return;

  int selIdx = pTBAnimSelector->GetCurrentSelection();
  if ( selIdx < 0 ) return;
  wxString selName = pTBAnimSelector->GetValue();
  if ( selName.IsEmpty() ) return;
  string mtdName = vsnApp::ConvWxToSys(selName);
  vsnMethod_keyFrameAnim* pmKFA
    = dynamic_cast<vsnMethod_keyFrameAnim*>(p_scene->getNode(mtdName));
  vsnMethod_timeStep* pmTSA
    = dynamic_cast<vsnMethod_timeStep*>(p_scene->getNode(mtdName));
  if ( ! pmKFA && ! pmTSA ) return;

  switch ( event.GetId() ) {
  case ViewFrameTooBar_AnimPlayBtn:
    if ( pmKFA ) pmKFA->play(false);
    else if ( pmTSA ) pmTSA->anim_play(false);
    break;
  case ViewFrameTooBar_AnimStopBtn:
    if ( pmKFA ) pmKFA->stop();
    else if ( pmTSA ) pmTSA->anim_stop();
    break;
  case ViewFrameTooBar_AnimRewBtn:
    if ( pmKFA ) pmKFA->setCurrentTime(pmKFA->getKFA().getInitialTime());
    else if ( pmTSA ) {
      int stpRange[2];
      if ( pmTSA->getStepRange(stpRange) ) pmTSA->setTimeStep(stpRange[0]);
    }
    break;
  case ViewFrameTooBar_AnimFwdBtn:
    if ( pmKFA ) pmKFA->setCurrentTime(pmKFA->getKFA().getInitialTime() +
				       pmKFA->getKFA().getTotalTime());
    else if ( pmTSA ) {
      int stpRange[2];
      if ( pmTSA->getStepRange(stpRange) ) pmTSA->setTimeStep(stpRange[1]);
    }
    break;
  } // end of switch

  if ( pmKFA ) {pmKFA->chkNotice(); pmKFA->updateUI();}
  if ( pmTSA ) {pmTSA->chkNotice(); pmTSA->updateUI();}
}

void vsnViewFrame::OnClose(wxCloseEvent& event) {
  if ( m_seq == 1 && p_app->getNumViewFrame() > 1 ) {
    ErrMsg(MsgWARN,
	   string("ViewFrame: closeView: can't close the first(master) view"));
    return;
  }

  // invalidate AuxUiFrame
  if ( m_pAuxUiFrame ) {
    m_pAuxUiFrame->Destroy();
    invalidateAuxUiFrame(m_pAuxUiFrame);
  }

  // ok, destroy myself
#if 1
  p_app->quit();
#else
  event.Skip();
#endif
}


/* Serialize */

bool vsnViewFrame::parseXML(xmlNodePtr xnp) {
  static xmlChar* xs;
  if ( ! xnp ) return false;

  // is 'view' node?
  if ( strcmp((const char*)xnp->name, "view") ) return false;

  // get name
  if ( xs ) xmlFree(xs);
  xs = xmlGetProp(xnp, (const xmlChar*)"name");
  if (  xs && strlen((const char*)xs) > 0 &&
	strcmp(VFR_NONAME, (const char*)xs) ) {
    vsnViewFrame* pov = NULL;
    if ( p_app ) pov = p_app->getViewFrame((const char*)xs);
    if ( pov && pov != this ) {
      ErrMsg(MsgWARN, string("ViewFrame: parseXML: the view named ") +
	     (const char*)xs + " has already exists, don't set name");
    } else
      setName((const char*)xs);
  }
  string msgHdr
    = string("View[") + getName() + string("]: parseXML: ");

  // get children node
  xmlNodePtr cur = xnp->xmlChildrenNode;

  // traverse
  while ( cur ) {
    if ( (cur = vsnIoObject::SkipCommentXML(cur)) == NULL )
      break;

    if ( !strcmp((const char*)cur->name, "param") ) {
      string xsN, xsV;
      if ( xs ) xmlFree(xs);
      xs = xmlGetProp(cur, (const xmlChar*)"name");
      if ( ! xs || strlen((const char*)xs) < 1 ) {
        ErrMsg(MsgERR, msgHdr + string("param node without name, ignore"));
        goto _NEXT_XML_NODE;
      }
      xsN = (const char*)xs;
      if ( xs ) xmlFree(xs);
      xs = xmlGetProp(cur, (const xmlChar*)"value");
      if ( xs && strlen((const char*)xs) > 0 ) xsV = (const char*)xs;
      else xsV = "";

      if ( xsN == string("show_ui") ) {
	bool showUiMode;
	if ( xsV == string("yes") ) showUiMode = true;
	else if ( xsV == string("no") ) showUiMode = false;
	else {
	  ErrMsg(MsgERR, msgHdr + string("invalid value in param show_ui"));
          goto _NEXT_XML_NODE;
	}
	setShowUiView(showUiMode);
      } // end of "show_ui"
      else if ( xsN == string("show_aux_ui") ) {
	bool showAUiMode;
	if ( xsV == string("yes") ) showAUiMode = true;
	else if ( xsV == string("no") ) showAUiMode = false;
	else {
	  ErrMsg(MsgERR, msgHdr +string("invalid value in param show_aux_ui"));
          goto _NEXT_XML_NODE;
	}
	setShowAuxUiFrame(showAUiMode);
      } // end of "show_aux_ui"
      else if ( xsN == string("show_toolbar") ) {
	bool showTBMode;
	if ( xsV == string("yes") ) showTBMode = true;
	else if ( xsV == string("no") ) showTBMode = false;
	else {
	  ErrMsg(MsgERR, msgHdr+string("invalid value in param show_toolbar"));
          goto _NEXT_XML_NODE;
	}
	setShowToolBar(showTBMode);
      } // end of "show_toolbar"
      else if ( xsN == string("perspective") ) {
	bool pm;
	if ( xsV == string("yes") ) pm = true;
	else if ( xsV == string("no") ) pm = false;
	else {
	  ErrMsg(MsgERR, msgHdr+string("invalid value in param perspective"));
          goto _NEXT_XML_NODE;
	}
	if ( ! setPerspective(pm) ) {
	  ErrMsg(MsgERR, msgHdr +
		 string("param perspective: setPerspective failed"));
          goto _NEXT_XML_NODE;
	}
      } // end of "prspective"
      else if ( xsN == string("show_center") ) {
	bool showCenter;
	if ( xsV == string("yes") ) showCenter = true;
	else if ( xsV == string("no") ) showCenter = false;
	else {
	  ErrMsg(MsgERR, msgHdr +string("invalid value in param show_center"));
	  goto _NEXT_XML_NODE;
	}
	setShowCenter(showCenter);
      } // end of "show_center"
      else if ( xsN == string("normalize_with_center") ) {
	bool normCenter;
	if ( xsV == string("yes") ) normCenter = true;
	else if ( xsV == string("no") ) normCenter = false;
	else {
	  ErrMsg(MsgERR, msgHdr +
		 string("invalid value in param normalize_with_center"));
	  goto _NEXT_XML_NODE;
	}
	setNormCenter(normCenter);
      } // end of "normalize_with_center"
      else if ( xsN == string("show_front_axis") ) {
	bool showFrAxis;
	if ( xsV == string("yes") ) showFrAxis = true;
	else if ( xsV == string("no") ) showFrAxis = false;
	else {
	  ErrMsg(MsgERR, msgHdr
		 + string("invalid value in param show_front_axis"));
	  goto _NEXT_XML_NODE;
	}
	setShowFrAxis(showFrAxis);
      } // end of "show_front_axis"
      else if ( xsN == string("bg_color") ) {
	if ( xsV.empty() ) {
	  ErrMsg(MsgERR, msgHdr +
		 string("invalid value in param bg_color: no value"));
	  goto _NEXT_XML_NODE;
	}
	vector4 cv = {0.f, 0.f, 0.f, 1.f};
	int nscan = sscanf(xsV.c_str(), "%f %f %f %f",
			   &cv[0], &cv[1], &cv[2], &cv[3]);
	if ( nscan < 3 ) {
	  ErrMsg(MsgERR, msgHdr +
		 string("invalid value in param bg_color: lack of values"));
	  goto _NEXT_XML_NODE;
	}
	if ( nscan < 4 ) cv[3] = 1.f;
	setBgColor(cv);
      } // end of "bg_color"
      else if ( xsN == string("sshot_command_str") ) {
	if ( xsV.empty() ) {
	  ErrMsg(MsgERR, msgHdr +
		 string("invalid value in param sshot_command_str: no value"));
	  goto _NEXT_XML_NODE;
	}
	setSshotCommStr(xsV);
      } // end of "sshot_command_str"
      // don't accept "center", use command
      else if ( xsN == string("scene") ) {
	if ( xsV.empty() || xsV == VFR_NONAME ) {
	  ErrMsg(MsgERR, msgHdr +
		 string("invalid value in param scene: no value"));
	  goto _NEXT_XML_NODE;
	}
	vsnScene* psc = NULL;
	if ( ! p_app || ! (psc = p_app->getScene(xsV.c_str())) ) {
	  ErrMsg(MsgERR, msgHdr +
		 string("invalid value in param scene: ") +
		 string("can't find the scene: ") + xsV);
	  goto _NEXT_XML_NODE;
	}
	setScene(psc);
	if ( m_pLightAttrDlg ) {
	  vsnLightAttribute& l = psc->getLightAttr();
	  m_pLightAttrDlg->setRefLightAtt(&l);
	}
      } // end of "scene"
      else if ( xsN == string("geometry") ) {
	if ( xsV.empty() ) {
	  ErrMsg(MsgERR, msgHdr +
		 string("invalid value in param geometry: no value"));
	  goto _NEXT_XML_NODE;
	}
	char xc; int c, x, y, w, h; x = y = w = h = -1;
	istringstream iss(xsV); string delim(" ,:/");
	iss >> x;
	while ( (c=iss.peek())!=EOF && delim.find((char)c)!=string::npos )
	  iss.get(xc);
	if ( iss.good() ) {
	  iss >> y;
	  while ( (c=iss.peek())!=EOF && delim.find((char)c)!=string::npos )
	    iss.get(xc);
	  if ( iss.good() ) {
	    iss >> w;
	    while ( (c=iss.peek())!=EOF && delim.find((char)c)!=string::npos )
	      iss.get(xc);
	    if ( iss.good() ) iss >> h;
	  }
	}
	if ( x == -1 && y == -1 && w == -1 && h == -1 ) {
	  ErrMsg(MsgERR, msgHdr +
		 string("invalid value in param geometry: ") + xsV);
	  goto _NEXT_XML_NODE;
	}
	if ( ! setGeometry(x, y, w, h) ) {
	  ErrMsg(MsgERR, msgHdr + string("set geometry failed: ") + xsV);
	  goto _NEXT_XML_NODE;
	}
      } // end of "geometry"
    } // end of "param"

    else if ( !strcmp((const char*)cur->name, "view_point") ) {
      if ( xs ) xmlFree(xs);
      xs = xmlGetProp(cur, (const xmlChar*)"name");
      char* vpName = (char*)xs;
      if ( ! vpName || strlen(vpName) < 1 || ! strcmp(vpName, VFR_NONAME) ) {
	ErrMsg(MsgERR, msgHdr + string("invalid view_point: no name"));
	goto _NEXT_XML_NODE;
      }
      map<string, vsnViewPoint>::iterator vpit
	= m_viewPointLst.find(vpName);
      if ( vpit != m_viewPointLst.end() ) {
	ErrMsg(MsgERR, msgHdr + string("view_point named ") + vpName +
	       string(" already exists, ignore"));
	goto _NEXT_XML_NODE;
      }
      if ( ! m_pGfxView ) {
	ErrMsg(MsgERR, msgHdr +
	       string("param view_point: add viewpoint failed"));
	goto _NEXT_XML_NODE;
      }
      vsnViewPoint xfm;
      if ( ! xfm.importXMLNode(cur) ) {
	ErrMsg(MsgERR, msgHdr +
	       string("param view_point: view_point node parse failed"));
	goto _NEXT_XML_NODE;
      }
      if ( ! m_viewPointLst.insert(make_pair(xfm.name, xfm)).second ) {
	ErrMsg(MsgERR, msgHdr +
	       string("param view_point: add viewpoint failed"));
	goto _NEXT_XML_NODE;
      }
    } // end of "view_point"

  _NEXT_XML_NODE:
    cur = cur->next;
  } // end of while(cur)

  return true;
}

bool vsnViewFrame::outputXML(std::ostream& os, const size_t ts) {
  std::string idts;
  for ( int i = 0; i < ts; i++ ) idts.push_back(' ');
  string idts_2 = idts; idts_2.push_back(' '); idts_2.push_back(' ');

  bool ret = true;
  string nameStr = getName();

  // output
  os << idts << "<view";
  if ( ! nameStr.empty() && nameStr != string(VFR_NONAME) )
    os << " name=\"" << nameStr << "\"";
  os << ">" << endl;

  // output view_point(s)
  addViewPoint(FRAME_DEFAULT_VPNAME); // current viewpoint
  map<string, vsnViewPoint>::iterator it;
  for ( it = m_viewPointLst.begin(); it != m_viewPointLst.end(); it++ ) {
    if ( ! it->second.exportXMLNode(os, ts+2) )
      ret = false;
  } // end of for(it)

  // output params
  // show_ui
  if ( ! m_showUiView ) {
    os << idts_2 << "<param name=\"show_ui\" value=\"no\" />" << endl;
  }

  // show_aux_ui
  if ( m_showAuxUiFrame ) {
    os << idts_2 << "<param name=\"show_aux_ui\" value=\"yes\" />" << endl;
  }

  // show_toolbar
  if ( ! m_showToolBar ) {
    os << idts_2 << "<param name=\"show_toolbar\" value=\"no\" />" << endl;
  }

  // perspective
  if ( ! getPerspective() ) {
    os << idts_2 << "<param name=\"perspective\" value=\"no\" />" << endl;
  }

  // show_center
  if ( ! m_showCenter ) {
    os << idts_2 << "<param name=\"show_center\" value=\"no\" />" << endl;
  }

  // show_front_axis
  if ( ! m_showFrAxis ) {
    os << idts_2 << "<param name=\"show_front_axis\" value=\"no\" />" << endl;
  }

  // bg_color
  float* cv = m_bgColor;
  if ( cv[0] != 0.f || cv[1] != 0.f || cv[2] != 0.f || cv[3] != 1.f ) {
    os << idts_2 << "<param name=\"bg_color\" value=\""
       << cv[0] << " " << cv[1] << " " << cv[2] << " " << cv[3]
       << "\" />" << endl;
  }

  // sshot_command_str
  if ( ! m_scshotCommStr.empty() ) {
    os << idts_2 << "<param name=\"sshot_command_str\" value=\""
       << m_scshotCommStr << "\" />" << endl;
  }

  // center --- don't output

  // scene
  if ( p_scene && p_app && p_scene != p_app->getScene(0) ) {
    string scName = p_scene->getName();
    if ( ! scName.empty() && scName != VFR_NONAME ) {
      os << idts_2 << "<param name=\"scene\" value=\""
	 << scName << "\" />" << endl;
    }
  }

  // geometry
  int winX, winY, winW, winH;
  GetPosition(&winX, &winY); GetSize(&winW, &winH);
  os << idts_2 << "<param name=\"geometry\" value=\""
     << winX << " " << winY << " " << winW << " " << winH << "\" />" << endl;

  os << idts << "</view>" << endl;

  // output sync viewpoint as command
  if ( p_vpRefMaster && ! nameStr.empty() && nameStr != VFR_NONAME ) {
    string vpMasterName = p_vpRefMaster->getName();
    if ( ! vpMasterName.empty() && vpMasterName != VFR_NONAME ) {
      os << idts << "<command target=\"view;" << getName()
	 << "\" name=\"sync_view_point\" value=\"" << vpMasterName
	 << "\" />" << endl;
    }
  }

  // output select method as command
  vsnDataObj* pDt = m_pUiView->getCurrentData();
  vsnMethodObj* pMtd = m_pUiView->getCurrentMethod();
  string objName;
  if ( pMtd ) objName = pMtd->getName();
  if ( objName == VFR_NONAME ) objName = "";
  if ( objName.empty() ) {
    if ( pDt ) objName = pDt->getName();
    if ( objName == VFR_NONAME ) objName = "";
  }
  if ( ! objName.empty() ) {
    os << idts << "<command target=\"view;" << getName()
       << "\" name=\"select_obj\" value=\"" << objName << "\" />" << endl;
  }

  return true;
}

bool vsnViewFrame::commandXML(xmlNodePtr xnp) {
  static xmlChar* xs;
  string msgHdr
    = string("View[") + getName() + string("]: commandXML: ");

  // is 'command' node?
  if ( ! xnp ) return false;
  if ( strcmp((const char*)xnp->name, "command") ) return false;

  // is my command?
  if ( xs ) xmlFree(xs);
  xs = xmlGetProp(xnp, (const xmlChar*)"target");
  if ( ! xs || strlen((const char*)xs) < 1 ) return false;
  string targStr = (const char*)xs;
  string targSub = targStr.substr(0, 5);
  if ( targSub != "view;" && targSub != "View;" && targSub != "VIEW;" )
    return false;
  targSub = targStr.substr(5);
  if ( targSub.empty() || targSub == VFR_NONAME ) return false;
  if ( targSub != getName() ) return false;

  // get command name
  string nameStr, valueStr;
  if ( xs ) xmlFree(xs);
  xs = xmlGetProp(xnp, (const xmlChar*)"name");
  if ( ! xs || strlen((const char*)xs) < 1 ) {
    ErrMsg(MsgERR, msgHdr + "invalid command: no 'name' property");
    return false;
  }
  nameStr = string((const char*)xs);

  // get command value (if there)
  if ( xs ) xmlFree(xs);
  xs = xmlGetProp(xnp, (const xmlChar*)"value");
  if ( xs && strlen((const char*)xs) > 0 )
    valueStr = string((const char*)xs);

  // do the command
  if ( nameStr == "set_name" ) {
    if ( valueStr == m_name ) return true;
    if ( valueStr.empty() || valueStr == VFR_NONAME ) {
      ErrMsg(MsgERR, msgHdr + string("command set_name: no value"));
      return false;
    }
    vsnViewFrame* pov = NULL;
    if ( p_app ) pov = p_app->getViewFrame(valueStr);
    if ( pov ) {
      ErrMsg(MsgERR, msgHdr + "command set_name: the view named " +
	     valueStr + " has already exists");
      return false;
    }
    setName(valueStr);
  } // end of "set_name"
  else if ( nameStr == "set_show_ui" ) {
    bool showUiMode;
    if ( valueStr == string("yes") ) showUiMode = true;
    else if ( valueStr == string("no") ) showUiMode = false;
    else {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_show_ui: invalid value: ") + valueStr);
      return false;
    }
    setShowUiView(showUiMode);
  } // end of "set_show_ui"
  else if ( nameStr == "set_show_aux_ui" ) {
    bool showAUiMode;
    if ( valueStr == string("yes") ) showAUiMode = true;
    else if ( valueStr == string("no") ) showAUiMode = false;
    else {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_show_aux_ui: invalid value: ") + valueStr);
      return false;
    }
    setShowAuxUiFrame(showAUiMode);
  } // end of "set_show_aux_ui"
  else if ( nameStr == "set_show_toolbar" ) {
    bool showTBMode;
    if ( valueStr == string("yes") ) showTBMode = true;
    else if ( valueStr == string("no") ) showTBMode = false;
    else {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_show_toolbar: invalid value: ") + valueStr);
      return false;
    }
    setShowToolBar(showTBMode);
  } // end of "set_show_toolbar"
  else if ( nameStr == "set_perspective" ) {
    bool pm;
    if ( valueStr == string("yes") ) pm = true;
    else if ( valueStr == string("no") ) pm = false;
    else {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_perspective: invalid value: ") + valueStr);
      return false;
    }
    if ( ! setPerspective(pm) ) {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_perspective: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_perspective"
  else if ( nameStr == "normalize" && m_pGfxView && p_scene ) {
    vfrNode* pTgt;
    if ( valueStr.empty() ||
	 valueStr == "scene" || valueStr == "Scene" || valueStr == "SCENE" )
      pTgt = NULL;
    else
      pTgt = p_scene->getNode(valueStr);
    if ( m_pGfxView->normalize(m_normalizeCenter, pTgt) )
      m_pGfxView->getDrawArea()->chkNotice();
  } // end of "normalize"
  else if ( nameStr == "set_view_point" ) {
    if ( ! updateViewPoint(valueStr) ) {
      ErrMsg(MsgERR, msgHdr + string("command set_view_point: viewpoint ") +
	     valueStr + string(" not found"));
      return false;
    }
  } // end of "set_view_point"
  else if ( nameStr == "sync_view_point" ) {
    vsnViewFrame* pvpm = NULL;
    if ( valueStr != "none" && valueStr != "None" && valueStr != "NONE" ) {
      if ( p_app ) pvpm = p_app->getViewFrame(valueStr);
      if ( ! pvpm ) {
	ErrMsg(MsgERR, msgHdr + string("command sync_view_point: target ") +
	       valueStr + string(" not found"));
	return false;
      }
    }
    if ( ! setViewPointMaster(pvpm) ) {
      ErrMsg(MsgERR, msgHdr + string("command sync_view_point: target ") +
	     valueStr + string(" : sync viewpoint failed"));
      return false;
    }
  } // end of "sync_view_point"
  else if ( nameStr == "set_show_center" ) {
    bool showCenter;
    if ( valueStr == string("yes") ) showCenter = true;
    else if ( valueStr == string("no") ) showCenter = false;
    else {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_show_center: invalid value: ") + valueStr);
      return false;
    }
    setShowCenter(showCenter);
  } // end of "set_show_center"
  else if ( nameStr == "set_normalize_with_center" ) {
    bool normCenter;
    if ( valueStr == string("yes") ) normCenter = true;
    else if ( valueStr == string("no") ) normCenter = false;
    else {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_normalize_with_center: invalid value: ") +
	     valueStr);
      return false;
    }
    setNormCenter(normCenter);
  } // end of "set_normalize_with_center"
  else if ( nameStr == "set_show_front_axis" ) {
    bool showFrAxis;
    if ( valueStr == string("yes") ) showFrAxis = true;
    else if ( valueStr == string("no") ) showFrAxis = false;
    else {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_show_front_axis: invalid value: ") +valueStr);
      return false;
    }
    setShowFrAxis(showFrAxis);
  } // end of "set_show_front_axis"
  else if ( nameStr == "set_bg_color" ) {
    if ( valueStr.empty() ) {
      ErrMsg(MsgERR, msgHdr+string("command set_bg_color: no value"));
      return false;
    }
    vector4 cv = {0.f, 0.f, 0.f, 1.f};
    int nscan = sscanf(valueStr.c_str(), "%f %f %f %f",
                       &cv[0], &cv[1], &cv[2], &cv[3]);
    if ( nscan < 3 ) {
      ErrMsg(MsgERR, msgHdr + string("command set_bg_color: lack of values"));
      return false;
    }
    if ( nscan < 4 ) cv[3] = 1.f;
    setBgColor(cv);
  } // end of "set_bg_color"
  else if ( nameStr == "set_sshot_command_str" ) {
    if ( valueStr.empty() ) {
      ErrMsg(MsgERR, msgHdr+string("command set_sshot_command_str: no value"));
      return false;
    }
    setSshotCommStr(valueStr);
  } // end of "set_sshot_command_str"
  else if ( nameStr == "set_center" ) {
    if ( valueStr.empty() ) {
      ErrMsg(MsgERR, msgHdr + string("command set_center: no value"));
      return false;
    }
    vector3 cv = {0.f, 0.f, 0.f};
    int nscan = sscanf(valueStr.c_str(), "%f %f %f", &cv[0], &cv[1], &cv[2]);
    if ( nscan < 3 ) {
      ErrMsg(MsgERR, msgHdr + string("command set_center: lack of values"));
      return false;
    }
    if ( m_pGfxView ) {
      m_pGfxView->setCenter(cv);
      m_pGfxView->getDrawArea()->chkNotice();
    }
  } // end of "set_center"
  else if ( nameStr == "set_scene" ) {
    if ( valueStr.empty() || valueStr == VFR_NONAME ) {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_scene: no value"));
      return false;
    }
    vsnScene* psc = NULL;
    if ( ! p_app || ! (psc = p_app->getScene(valueStr.c_str())) ) {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_scene: can't find the scene: ") + valueStr);
      return false;
    }
    setScene(psc);
  } // end of "set_scene"
  else if ( nameStr == "select_obj" ) {
    if ( valueStr.empty() || valueStr == VFR_NONAME ) {
      ErrMsg(MsgERR, msgHdr + string("command select_obj: no value"));
      return false;
    }
    vfrNode* pnode = NULL;
    if ( ! p_scene || ! (pnode = p_scene->getNode(valueStr.c_str())) ) {
      ErrMsg(MsgERR, msgHdr +
	     string("command select_obj: can't find obj: ") + valueStr);
      return false;
    }
    if ( ! m_pUiView || ! m_pUiView->selectObj(pnode) ) {
      ErrMsg(MsgERR, msgHdr +
	     string("command select_obj: set failed: ") + valueStr);
      return false;
    }
  } // end of "select_obj"
  else if ( nameStr == "screenshot" ) {
    if ( valueStr.empty() || valueStr == VFR_NONAME ) {
      ErrMsg(MsgERR, msgHdr + string("command screenshot: no value"));
      return false;
    }
    valueStr = vsnPath_normalize(valueStr);
    if ( ! m_pGfxView || ! m_pGfxView->screenShot(valueStr) ) {
      ErrMsg(MsgERR, msgHdr +
	     string("command screenshot: failed to shot: ") + valueStr);
      return false;
    }
    p_app->setScreenshotDir(DirName(valueStr, vsnPath_getDelimChar()));
  } // end of "screenshot"
  else if ( nameStr == "set_geometry" ) {
    if ( valueStr.empty() ) {
      ErrMsg(MsgERR, msgHdr + string("command set_geometry: no value"));
      return false;
    }
    char xc; int c, x, y, w, h; x = y = w = h = -1;
    istringstream iss(valueStr); string delim(" ,:/");
    iss >> x;
    while ( (c=iss.peek()) != EOF && delim.find((char)c) != string::npos )
      iss.get(xc);
    if ( iss.good() ) {
      iss >> y;
      while ( (c=iss.peek()) != EOF && delim.find((char)c) != string::npos )
	iss.get(xc);
      if ( iss.good() ) {
	iss >> w;
	while ( (c=iss.peek()) != EOF && delim.find((char)c) != string::npos )
	  iss.get(xc);
	if ( iss.good() ) iss >> h;
      }
    }
    if ( x == -1 && y == -1 && w == -1 && h == -1 ) {
      ErrMsg(MsgERR, msgHdr + string("command set_geometry: invalid value: ")
	     + valueStr);
      return false;
    }
    if ( ! setGeometry(x, y, w, h) ) {
      ErrMsg(MsgERR, msgHdr + string("command set_geometry: set failed: ")
	     + valueStr);
      return false;
    }
  } // end of "set_geometry"
  else {
    // not 'view' command
    ErrMsg(MsgERR, msgHdr + string("unknown command: ") + nameStr);
    return false;
  }

  return true;
}


/* static utils */

void vsnViewFrame::UpdateAllLightAttrDlg() {
  vsnApp* papp = vsnApp::GetApp();
  if ( ! papp ) return;
  size_t i, nvf = papp->getNumViewFrame();
  for ( i = 0; i < nvf; i++ ) {
    vsnViewFrame* pvf = papp->getViewFrame(i);
	if ( ! pvf ) continue;
    vsnLightAttributeDlg* plDlg = pvf->getLightAttDlg();
	if ( ! plDlg ) continue;
	plDlg->update();
  } // end of for(i)
}

