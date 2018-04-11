//
// vsnGfxActions
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
#include "wx/cursor.h"
#include "wx/config.h"

#include "vsnGfxActions.h"
#include "vsnGfxView.h"
#include "vsnViewFrame.h"
#include "vsnMethodObj.h"
#include "vsnFrontLabel.h"
#include "vsnFrontImage.h"
#include "vsnAnchor.h"
#include "vsnError.h"

using namespace std;
using namespace VSN;


//----------------------------------------------------------------
// class vsnGfxView
//----------------------------------------------------------------

void vsnGfxView::pushActions(void* key) {
  vfrDrawAreaWx* pda = getDrawArea();
  if ( ! pda ) return;
  vfrDispatch& dispatcher = vfrDispatch::instance(*pda);

  if ( ! key ) return;
  if ( key == m_pCurActKey ) return;
  deque<ActSetEntry>::iterator it;
  for ( it = m_gfxActStack.begin(); it != m_gfxActStack.end(); it++ ) {
    if ( it->first == key ) break;
  } // end of for(it)
  if ( it != m_gfxActStack.end() )
    m_gfxActStack.erase(it);

  vsnGfxActSet actSet;
  actSet.actions[ActKeyIn]
    = vfrEvKeyIn::instance(dispatcher).getAction();
  actSet.actions[ActClick]
    = vfrEvClick::instance(dispatcher).getAction();
  actSet.actions[ActDragStart]
    = vfrEvDragStart::instance(dispatcher).getAction();
  actSet.actions[ActDrag]
    = vfrEvDrag::instance(dispatcher).getAction();
  actSet.actions[ActDragEnd]
    = vfrEvDragEnd::instance(dispatcher).getAction();
  actSet.actions[ActSClick]
    = vfrEvSClick::instance(dispatcher).getAction();
  actSet.actions[ActSDragStart]
    = vfrEvSDragStart::instance(dispatcher).getAction();
  actSet.actions[ActSDrag]
    = vfrEvSDrag::instance(dispatcher).getAction();
  actSet.actions[ActSDragEnd]
    = vfrEvSDragEnd::instance(dispatcher).getAction();
  actSet.actions[ActCClick]
    = vfrEvCClick::instance(dispatcher).getAction();
  actSet.actions[ActCDragStart]
    = vfrEvCDragStart::instance(dispatcher).getAction();
  actSet.actions[ActCDrag]
    = vfrEvCDrag::instance(dispatcher).getAction();
  actSet.actions[ActCDragEnd]
    = vfrEvCDragEnd::instance(dispatcher).getAction();
  actSet.actions[ActSCClick]
    = vfrEvSCClick::instance(dispatcher).getAction();
  actSet.actions[ActSCDragStart]
    = vfrEvSCDragStart::instance(dispatcher).getAction();
  actSet.actions[ActSCDrag]
    = vfrEvSCDrag::instance(dispatcher).getAction();
  actSet.actions[ActSCDragEnd]
    = vfrEvSCDragEnd::instance(dispatcher).getAction();
  actSet.actions[ActRClick]
    = vfrEvRClick::instance(dispatcher).getAction();
  actSet.actions[ActRDragStart]
    = vfrEvRDragStart::instance(dispatcher).getAction();
  actSet.actions[ActRDrag]
    = vfrEvRDrag::instance(dispatcher).getAction();
  actSet.actions[ActRDragEnd]
    = vfrEvRDragEnd::instance(dispatcher).getAction();
  actSet.actions[ActRSClick]
    = vfrEvRSClick::instance(dispatcher).getAction();
  actSet.actions[ActRSDragStart]
    = vfrEvRSDragStart::instance(dispatcher).getAction();
  actSet.actions[ActRSDrag]
    = vfrEvRSDrag::instance(dispatcher).getAction();
  actSet.actions[ActRSDragEnd]
    = vfrEvRSDragEnd::instance(dispatcher).getAction();
  actSet.actions[ActRCClick]
    = vfrEvRCClick::instance(dispatcher).getAction();
  actSet.actions[ActRCDragStart]
    = vfrEvRCDragStart::instance(dispatcher).getAction();
  actSet.actions[ActRCDrag]
    = vfrEvRCDrag::instance(dispatcher).getAction();
  actSet.actions[ActRCDragEnd]
    = vfrEvRCDragEnd::instance(dispatcher).getAction();
  actSet.actions[ActRSCClick]
    = vfrEvRSCClick::instance(dispatcher).getAction();
  actSet.actions[ActRSCDragStart]
    = vfrEvRSCDragStart::instance(dispatcher).getAction();
  actSet.actions[ActRSCDrag]
    = vfrEvRSCDrag::instance(dispatcher).getAction();
  actSet.actions[ActRSCDragEnd]
    = vfrEvRSCDragEnd::instance(dispatcher).getAction();
  actSet.actions[ActMClick]
    = vfrEvMClick::instance(dispatcher).getAction();
  actSet.actions[ActMDragStart]
    = vfrEvMDragStart::instance(dispatcher).getAction();
  actSet.actions[ActMDrag]
    = vfrEvMDrag::instance(dispatcher).getAction();
  actSet.actions[ActMDragEnd]
    = vfrEvMDragEnd::instance(dispatcher).getAction();
  actSet.actions[ActMSClick]
    = vfrEvMSClick::instance(dispatcher).getAction();
  actSet.actions[ActMSDragStart]
    = vfrEvMSDragStart::instance(dispatcher).getAction();
  actSet.actions[ActMSDrag]
    = vfrEvMSDrag::instance(dispatcher).getAction();
  actSet.actions[ActMSDragEnd]
    = vfrEvMSDragEnd::instance(dispatcher).getAction();
  actSet.actions[ActMCClick]
    = vfrEvMCClick::instance(dispatcher).getAction();
  actSet.actions[ActMCDragStart]
    = vfrEvMCDragStart::instance(dispatcher).getAction();
  actSet.actions[ActMCDrag]
    = vfrEvMCDrag::instance(dispatcher).getAction();
  actSet.actions[ActMCDragEnd]
    = vfrEvMCDragEnd::instance(dispatcher).getAction();
  actSet.actions[ActMSCClick]
    = vfrEvMSCClick::instance(dispatcher).getAction();
  actSet.actions[ActMSCDragStart]
    = vfrEvMSCDragStart::instance(dispatcher).getAction();
  actSet.actions[ActMSCDrag]
    = vfrEvMSCDrag::instance(dispatcher).getAction();
  actSet.actions[ActMSCDragEnd]
    = vfrEvMSCDragEnd::instance(dispatcher).getAction();
  actSet.pcursor = p_cursor;

  m_gfxActStack.push_back(make_pair(m_pCurActKey, actSet));
  m_pCurActKey = key;
  invalidateActions();
}

void vsnGfxView::popActions(void* key) {
  vfrDrawAreaWx* pda = getDrawArea();
  if ( ! pda ) return;
  vfrDispatch &dpt = vfrDispatch::instance(*pda);

  if ( ! key ) return;

  deque<ActSetEntry>::iterator it;
  for ( it = m_gfxActStack.begin(); it != m_gfxActStack.end(); it++ ) {
    if ( it->first == key ) break;
  } // end of for(it)
  if ( it != m_gfxActStack.end() ) {
    // key has found in ActSet stack, erase it from stack
    m_gfxActStack.erase(it);
    return;
  }
  // key is not the current, and not found in stack... ignore
  if ( key != m_pCurActKey ) return;

  //-------- key is the current --------
  if ( m_gfxActStack.empty() ) {
    // fail safe
    m_pCurActKey = (void*)this;
    setStandardActions();
    setCursor();
    return;
  }

  // key is the current, restore the top-ActSet and erase it from stack
  it = m_gfxActStack.end(); it --;
  vsnGfxActSet actSet = it->second;
  vfrEvKeyIn::instance(dpt).regist(actSet.actions[ActKeyIn]);
  vfrEvClick::instance(dpt).regist(actSet.actions[ActClick]);
  vfrEvDragStart::instance(dpt).regist(actSet.actions[ActDragStart]);
  vfrEvDrag::instance(dpt).regist(actSet.actions[ActDrag]);
  vfrEvDragEnd::instance(dpt).regist(actSet.actions[ActDragEnd]);
  vfrEvSClick::instance(dpt).regist(actSet.actions[ActSClick]);
  vfrEvSDragStart::instance(dpt).regist(actSet.actions[ActSDragStart]);
  vfrEvSDrag::instance(dpt).regist(actSet.actions[ActSDrag]);
  vfrEvSDragEnd::instance(dpt).regist(actSet.actions[ActSDragEnd]);
  vfrEvCClick::instance(dpt).regist(actSet.actions[ActCClick]);
  vfrEvCDragStart::instance(dpt).regist(actSet.actions[ActCDragStart]);
  vfrEvCDrag::instance(dpt).regist(actSet.actions[ActCDrag]);
  vfrEvCDragEnd::instance(dpt).regist(actSet.actions[ActCDragEnd]);
  vfrEvSCClick::instance(dpt).regist(actSet.actions[ActSCClick]);
  vfrEvSCDragStart::instance(dpt).regist(actSet.actions[ActSCDragStart]);
  vfrEvSCDrag::instance(dpt).regist(actSet.actions[ActSCDrag]);
  vfrEvSCDragEnd::instance(dpt).regist(actSet.actions[ActSCDragEnd]);
  vfrEvRClick::instance(dpt).regist(actSet.actions[ActRClick]);
  vfrEvRDragStart::instance(dpt).regist(actSet.actions[ActRDragStart]);
  vfrEvRDrag::instance(dpt).regist(actSet.actions[ActRDrag]);
  vfrEvRDragEnd::instance(dpt).regist(actSet.actions[ActRDragEnd]);
  vfrEvRSClick::instance(dpt).regist(actSet.actions[ActRSClick]);
  vfrEvRSDragStart::instance(dpt).regist(actSet.actions[ActRSDragStart]);
  vfrEvRSDrag::instance(dpt).regist(actSet.actions[ActRSDrag]);
  vfrEvRSDragEnd::instance(dpt).regist(actSet.actions[ActRSDragEnd]);
  vfrEvRCClick::instance(dpt).regist(actSet.actions[ActRCClick]);
  vfrEvRCDragStart::instance(dpt).regist(actSet.actions[ActRCDragStart]);
  vfrEvRCDrag::instance(dpt).regist(actSet.actions[ActRCDrag]);
  vfrEvRCDragEnd::instance(dpt).regist(actSet.actions[ActRCDragEnd]);
  vfrEvRSCClick::instance(dpt).regist(actSet.actions[ActRSCClick]);
  vfrEvRSCDragStart::instance(dpt).regist(actSet.actions[ActRSCDragStart]);
  vfrEvRSCDrag::instance(dpt).regist(actSet.actions[ActRSCDrag]);
  vfrEvRSCDragEnd::instance(dpt).regist(actSet.actions[ActRSCDragEnd]);
  vfrEvMClick::instance(dpt).regist(actSet.actions[ActMClick]);
  vfrEvMDragStart::instance(dpt).regist(actSet.actions[ActMDragStart]);
  vfrEvMDrag::instance(dpt).regist(actSet.actions[ActMDrag]);
  vfrEvMDragEnd::instance(dpt).regist(actSet.actions[ActMDragEnd]);
  vfrEvMSClick::instance(dpt).regist(actSet.actions[ActMSClick]);
  vfrEvMSDragStart::instance(dpt).regist(actSet.actions[ActMSDragStart]);
  vfrEvMSDrag::instance(dpt).regist(actSet.actions[ActMSDrag]);
  vfrEvMSDragEnd::instance(dpt).regist(actSet.actions[ActMSDragEnd]);
  vfrEvMCClick::instance(dpt).regist(actSet.actions[ActMCClick]);
  vfrEvMCDragStart::instance(dpt).regist(actSet.actions[ActMCDragStart]);
  vfrEvMCDrag::instance(dpt).regist(actSet.actions[ActMCDrag]);
  vfrEvMCDragEnd::instance(dpt).regist(actSet.actions[ActMCDragEnd]);
  vfrEvMSCClick::instance(dpt).regist(actSet.actions[ActMSCClick]);
  vfrEvMSCDragStart::instance(dpt).regist(actSet.actions[ActMSCDragStart]);
  vfrEvMSCDrag::instance(dpt).regist(actSet.actions[ActMSCDrag]);
  vfrEvMSCDragEnd::instance(dpt).regist(actSet.actions[ActMSCDragEnd]);
  setCursor(actSet.pcursor);

  m_pCurActKey = it->first;
  m_gfxActStack.erase(it);
}


void vsnGfxView::invalidateActions() {
  vfrDrawAreaWx* pda = getDrawArea();
  if ( ! pda ) return;
  vfrDispatch &dpt = vfrDispatch::instance(*pda);
  
  vfrEvKeyIn::instance(dpt).regist(NULL);
  vfrEvClick::instance(dpt).regist(NULL);
  vfrEvDragStart::instance(dpt).regist(NULL);
  vfrEvDrag::instance(dpt).regist(NULL);
  vfrEvDragEnd::instance(dpt).regist(NULL);
  vfrEvSClick::instance(dpt).regist(NULL);
  vfrEvSDragStart::instance(dpt).regist(NULL);
  vfrEvSDrag::instance(dpt).regist(NULL);
  vfrEvSDragEnd::instance(dpt).regist(NULL);
  vfrEvCClick::instance(dpt).regist(NULL);
  vfrEvCDragStart::instance(dpt).regist(NULL);
  vfrEvCDrag::instance(dpt).regist(NULL);
  vfrEvCDragEnd::instance(dpt).regist(NULL);
  vfrEvSCClick::instance(dpt).regist(NULL);
  vfrEvSCDragStart::instance(dpt).regist(NULL);
  vfrEvSCDrag::instance(dpt).regist(NULL);
  vfrEvSCDragEnd::instance(dpt).regist(NULL);
  vfrEvRClick::instance(dpt).regist(NULL);
  vfrEvRDragStart::instance(dpt).regist(NULL);
  vfrEvRDrag::instance(dpt).regist(NULL);
  vfrEvRDragEnd::instance(dpt).regist(NULL);
  vfrEvRSClick::instance(dpt).regist(NULL);
  vfrEvRSDragStart::instance(dpt).regist(NULL);
  vfrEvRSDrag::instance(dpt).regist(NULL);
  vfrEvRSDragEnd::instance(dpt).regist(NULL);
  vfrEvRCClick::instance(dpt).regist(NULL);
  vfrEvRCDragStart::instance(dpt).regist(NULL);
  vfrEvRCDrag::instance(dpt).regist(NULL);
  vfrEvRCDragEnd::instance(dpt).regist(NULL);
  vfrEvRSCClick::instance(dpt).regist(NULL);
  vfrEvRSCDragStart::instance(dpt).regist(NULL);
  vfrEvRSCDrag::instance(dpt).regist(NULL);
  vfrEvRSCDragEnd::instance(dpt).regist(NULL);
  vfrEvMClick::instance(dpt).regist(NULL);
  vfrEvMDragStart::instance(dpt).regist(NULL);
  vfrEvMDrag::instance(dpt).regist(NULL);
  vfrEvMDragEnd::instance(dpt).regist(NULL);
  vfrEvMSClick::instance(dpt).regist(NULL);
  vfrEvMSDragStart::instance(dpt).regist(NULL);
  vfrEvMSDrag::instance(dpt).regist(NULL);
  vfrEvMSDragEnd::instance(dpt).regist(NULL);
  vfrEvMCClick::instance(dpt).regist(NULL);
  vfrEvMCDragStart::instance(dpt).regist(NULL);
  vfrEvMCDrag::instance(dpt).regist(NULL);
  vfrEvMCDragEnd::instance(dpt).regist(NULL);
  vfrEvMSCClick::instance(dpt).regist(NULL);
  vfrEvMSCDragStart::instance(dpt).regist(NULL);
  vfrEvMSCDrag::instance(dpt).regist(NULL);
  vfrEvMSCDragEnd::instance(dpt).regist(NULL);
}

void vsnGfxView::setStandardActions() {
  vfrDrawAreaWx* pda = getDrawArea();
  vsnApp* pApp = vsnApp::GetApp();
  if ( ! pda || ! pApp ) return;
  vfrDispatch& dpt = vfrDispatch::instance(*pda);
  vsnGfxOprOrientation oprOri = pApp->getOprOrientation();

  // invalidate all actions
  invalidateActions();

  // register actions
  vfrEvKeyIn::instance(dpt).regist(&gfxAct_KeyIn);
  vfrEvClick::instance(dpt).regist(&gfxAct_Click);

  // Rotate
  oprOri.m_oprLst[OprRotate].getEvent(dpt, EvtDragStart)
    .regist(&gfxAct_RotScene);
  oprOri.m_oprLst[OprRotate].getEvent(dpt, EvtDrag)
    .regist(&gfxAct_RotScene);
  oprOri.m_oprLst[OprRotate].getEvent(dpt, EvtDragEnd)
    .regist(&gfxAct_RotScene);

  // Roll
  oprOri.m_oprLst[OprRoll].getEvent(dpt, EvtDragStart)
    .regist(&gfxAct_RollScene);
  oprOri.m_oprLst[OprRoll].getEvent(dpt, EvtDrag)
    .regist(&gfxAct_RollScene);
  oprOri.m_oprLst[OprRoll].getEvent(dpt, EvtDragEnd)
    .regist(&gfxAct_RollScene);

  // Scale
  oprOri.m_oprLst[OprScale].getEvent(dpt, EvtDragStart)
    .regist(&gfxAct_ScaleScene);
  oprOri.m_oprLst[OprScale].getEvent(dpt, EvtDrag)
    .regist(&gfxAct_ScaleScene);
  oprOri.m_oprLst[OprScale].getEvent(dpt, EvtDragEnd)
    .regist(&gfxAct_ScaleScene);

  // Translate
  oprOri.m_oprLst[OprTranslate].getEvent(dpt, EvtDragStart)
    .regist(&gfxAct_TransScene);
  oprOri.m_oprLst[OprTranslate].getEvent(dpt, EvtDrag)
    .regist(&gfxAct_TransScene);
  oprOri.m_oprLst[OprTranslate].getEvent(dpt, EvtDragEnd)
    .regist(&gfxAct_TransScene);

  // Select
  oprOri.m_oprLst[OprSelect].getEvent(dpt, EvtClick)
    .regist(&gfxAct_Selection);
  oprOri.m_oprLst[OprSelect].getEvent(dpt, EvtDragStart)
    .regist(&gfxAct_StartRBoxSelect);
  oprOri.m_oprLst[OprSelect].getEvent(dpt, EvtDrag)
    .regist(&vfrDrawRBoxAction::instance());
  oprOri.m_oprLst[OprSelect].getEvent(dpt, EvtDragEnd)
    .regist(&gfxAct_Selection);

  // SweepZoom
  oprOri.m_oprLst[OprSweepZoom].getEvent(dpt, EvtDragStart)
    .regist(&gfxAct_StartRBoxZoom);
  oprOri.m_oprLst[OprSweepZoom].getEvent(dpt, EvtDrag)
    .regist(&vfrDrawRBoxAction::instance());
  oprOri.m_oprLst[OprSweepZoom].getEvent(dpt, EvtDragEnd)
    .regist(&gfxAct_SweepZoom);

  // Wheel
  vfrEvWheel::instance(dpt).regist(&gfxAct_Wheel);
}


//----------------------------------------------------------------
// struct vsnGfxActSet
//----------------------------------------------------------------
vsnGfxActSet::vsnGfxActSet() {
  pcursor = const_cast<wxCursor*>(wxSTANDARD_CURSOR);
  for ( int i = 0; i < ActNUM; i++ )
    actions[i] = NULL;
}

void vsnGfxActSet::operator=(const vsnGfxActSet& org) {
  pcursor = org.pcursor;
  for ( int i = 0; i < ActNUM; i++ )
    actions[i] = org.actions[i];
}


//----------------------------------------------------------------
// class vsnGfxAct_KeyIn
//----------------------------------------------------------------
void vsnGfxAct_KeyIn::execute(vfrEvent& e) {
  vsnScene* psc = p_gfxView->getScene();
  if ( ! psc ) return;
  if ( psc->getXforming() ) return;

  VFRkeyCode kc = e.getKey();
  if ( kc == VFRKC_ESCAPE ) {
    vsnViewFrame* pvf = dynamic_cast<vsnViewFrame*>
      (p_gfxView->getDrawArea()->getCanvas()->GetParent());
    if ( ! pvf ) return;
    vsnUiView* puiv = pvf->getUiView();
    if ( ! puiv ) return;
    puiv->selectObj((vfrNode*)NULL);
    return;
  }
#ifdef MacOSX
  // need for MacOSX, SPACE acceleration not work...
  else if ( kc == VFRKC_SPACE ) {
    vsnViewFrame* pvf = dynamic_cast<vsnViewFrame*>
      (p_gfxView->getDrawArea()->getCanvas()->GetParent());
    pvf->normalizeViewport(e.withShiftKey());
  }
#endif // MacOSX
  else if ( kc == VFRKC_C || kc == VFRKC_c ) {
    vfrScreen* screen = (vfrScreen*)e.getScreen();
    if ( ! screen ) return;
    GLint vp[4]; screen->getPort(vp);
    Point2 cp0, cp1; cp1.x = vp[2]; cp1.y = vp[3];
    p_gfxView->sweepZoomView(cp0, cp1);
  } // end of C key
  else if ( kc == VFRKC_Z || kc == VFRKC_z ) {
    vfrNode* psn = p_gfxView->getScaleNode();
    if ( ! psn ) return;
    vsnViewPoint vp0 = p_gfxView->getXForm();
    float r1, r2;
    vsnGfxView::GetKeyZoomInRatio(r1, r2);
    psn->scale(e.withShiftKey() ? r2 : r1);
    vsnViewPoint vp1 = p_gfxView->getXForm();
    p_gfxView->setXForm(vp0);
    p_gfxView->setViewXForm(vp1);
  } // end of Z key
  else if ( kc == VFRKC_X || kc == VFRKC_x ) {
    vfrNode* psn = p_gfxView->getScaleNode();
    if ( ! psn ) return;
    vsnViewPoint vp0 = p_gfxView->getXForm();
    float r1, r2;
    vsnGfxView::GetKeyZoomOutRatio(r1, r2);
    psn->scale(e.withShiftKey() ? r2 : r1);
    vsnViewPoint vp1 = p_gfxView->getXForm();
    p_gfxView->setXForm(vp0);
    p_gfxView->setViewXForm(vp1);
  } // end of X key
}


//----------------------------------------------------------------
// class vsnGfxAct_Click
//----------------------------------------------------------------
void vsnGfxAct_Click::execute(vfrEvent& e) {
  if ( ! p_gfxView ) return;
  p_gfxView->getDrawArea()->getCanvas()->SetFocus();

  // check double click for Anchor
  if ( ! e.isDblClickEvent() ) return;

  vfrScreen* screen = (vfrScreen*)e.getScreen();
  if ( ! screen ) return;
  vsnScene* psc = p_gfxView->getScene();
  if( ! psc ) return;
  unsigned int* r;
  Point2 cp = screen->getRelativePoint(e.getMPoint());
  r = screen->clickSelect(cp.x, cp.y);
  if ( r[0] > 0 ) {
    vfrNode* pNode = screen->getNode(r[1]);
    vsnAnchor* pAnc = dynamic_cast<vsnAnchor*>(pNode);
    if ( pAnc )
      pAnc->execAction();
    else {
      vsnMethodLutRefer* pMLR = dynamic_cast<vsnMethodLutRefer*>(pNode);
      if ( pMLR ) {
        pAnc = dynamic_cast<vsnAnchor*>(pMLR->getMethodObj());
        if ( pAnc )
	  pAnc->execAction();
      }
    }
  }
}


//----------------------------------------------------------------
// class vsnGfxAct_RotScene
//----------------------------------------------------------------
void vsnGfxAct_RotScene::execute(vfrEvent& e) {
  if ( ! p_gfxView ) return;
  vfrScreen* screen = (vfrScreen*)e.getScreen();
  if ( ! screen ) return;
  vfrNode* pnode = p_gfxView->getRotateNode();
  if ( ! pnode ) return;
  vsnScene* psc = p_gfxView->getScene();

  //// trackball rotation code
  ////screen->rotateNode(pnode->getID(), e.getMPoint(), e.getMMove());

  Point2 vpSize = screen->getSize();
  GLint l = ((vpSize.x > vpSize.y) ? vpSize.x : vpSize.y) / 2;
  if ( l < 1 ) return;

  if ( e.isDragStartEvent() ) {
    m_ok = false;

    m_mp0 = e.getMPoint() - e.getMMove();
    m_M0 = pnode->getMatrix();

    unsigned int tid = pnode->getID();
    Point2 mp1, mp2;
    mp1 = mp2 = m_mp0;
    mp1.x += 2; mp2.y += 2;
    CES::Vec3<float> objp0, objp1, objp2;

    if ( ! screen->getObjCoord(tid, m_mp0, objp0.m_v) ) return;
    if ( ! screen->getObjCoord(tid, mp1, objp1.m_v) ) return;
    if ( ! screen->getObjCoord(tid, mp2, objp2.m_v) ) return;
    m_rAx = objp0 - objp2; m_rAx.UnitVec();
    m_rAy = objp1 - objp0; m_rAy.UnitVec();
    m_rAz = m_rAx ^ m_rAy;

    m_ok = true;
	if( psc ) psc->setXforming(true);
  } // end of "DragStart"
  else if ( e.isDragEndEvent() ) {
    m_ok = false;
	if( psc ) psc->setXforming(false);
  } // end of "DragEnd"

  if ( ! m_ok ) return;

  // do rotate
  vfrMatrix M = m_M0;
  Point2 dm = e.getMPoint() - m_mp0;
  if ( dm.x == 0 && dm.y == 0 ) return;
  CES::Vec3<float> rV = (m_rAx * (float)dm.y) - (m_rAy * (float)dm.x);
  rV.UnitVec();
  CES::Vec3<float> rA = m_rAz ^ rV;
  float ang = (float)sqrt((double)(dm.x*dm.x)+(double)(dm.y*dm.y))*HALF_PI/l;
  M.Rotation(ang, rA);
  pnode->setMatrix(M);

  p_gfxView->sceneGraphUpdated();
}


//----------------------------------------------------------------
// class vsnGfxAct_RollScene
//----------------------------------------------------------------
void vsnGfxAct_RollScene::execute(vfrEvent& e) {
  if ( ! p_gfxView ) return;
  vfrScreen* screen = (vfrScreen*)e.getScreen();
  if ( ! screen ) return;
  vfrNode* pnode = p_gfxView->getRotateNode();
  if ( ! pnode ) return;
  vsnScene* psc = p_gfxView->getScene();

  Point2 vpSize = screen->getSize();
  GLint l = ((vpSize.x > vpSize.y) ? vpSize.x : vpSize.y) / 2;
  if ( l < 1 ) return;

  if ( e.isDragStartEvent() ) {
    m_ok = false;

    m_mp0 = e.getMPoint() - e.getMMove();
    m_M0 = pnode->getMatrix();

    unsigned int tid = pnode->getID();
    Point2 mp1, mp2;
    mp1 = mp2 = m_mp0;
    mp1.x += 2; mp2.y += 2;
    CES::Vec3<float> objp0, objp1, objp2;

    if ( ! screen->getObjCoord(tid, m_mp0, objp0.m_v) ) return;
    if ( ! screen->getObjCoord(tid, mp1, objp1.m_v) ) return;
    if ( ! screen->getObjCoord(tid, mp2, objp2.m_v) ) return;
    m_rAx = objp0 - objp2; m_rAx.UnitVec();
    m_rAy = objp1 - objp0; m_rAy.UnitVec();
    m_rAz = m_rAx ^ m_rAy;
    m_rAz.UnitVec();

    m_ok = true;
	if( psc ) psc->setXforming(true);
  } // end of "DragStart"
  else if ( e.isDragEndEvent() ) {
    m_ok = false;
	if( psc ) psc->setXforming(false);
  } // end of "DragEnd"

  if ( ! m_ok ) return;

  // do rotate
  vfrMatrix M = m_M0;
  Point2 dm = e.getMPoint() - m_mp0;
  if ( dm.x - dm.y == 0 ) return;
  float ang = (dm.x - dm.y) * HALF_PI / l;
  M.Rotation(ang, m_rAz);
  pnode->setMatrix(M);

  p_gfxView->sceneGraphUpdated();
}


//----------------------------------------------------------------
// class vsnGfxAct_ScaleScene
//----------------------------------------------------------------
void vsnGfxAct_ScaleScene::execute(vfrEvent& e) {
  if ( ! p_gfxView ) return;
  vfrScreen* screen = (vfrScreen*)e.getScreen();
  if ( ! screen ) return;
  vfrNode* pnode = p_gfxView->getScaleNode();
  if ( ! pnode ) return;

  vsnScene* psc = p_gfxView->getScene();
  if ( e.isDragStartEvent() ) {
    if ( psc ) psc->setXforming(true);
    return;
  } else if ( e.isDragEndEvent() ) {
    if ( psc ) psc->setXforming(false);
    return;
  }

  screen->scaleNode(pnode->getID(), e.getMPoint(), e.getMMove());

  p_gfxView->sceneGraphUpdated();
}


//----------------------------------------------------------------
// class vsnGfxAct_TransScene
//----------------------------------------------------------------
void vsnGfxAct_TransScene::execute(vfrEvent& e) {
  if ( ! p_gfxView ) return;
  vfrScreen* screen = (vfrScreen*)e.getScreen();
  if ( ! screen ) return;
  vfrNode* pT = p_gfxView->getTranslateNode();
  if ( ! pT ) return;
  vfrNode* pX = p_gfxView->getRootScene();
  if ( ! pX ) return;

  vsnScene* psc = p_gfxView->getScene();
  if ( e.isDragStartEvent() ) {
    if ( psc ) psc->setXforming(true);
    return;
  } else if ( e.isDragEndEvent() ) {
    if ( psc ) psc->setXforming(false);
    return;
  }

  unsigned int tid = pX->getID();
  Point2 mp1, mp2, md;
  mp2 = e.getMPoint();
  md = e.getMMove();
  mp1 = mp2 - md;
  CES::Vec3<float> objp1, objp2;
  if ( ! screen->getObjCoord(tid, mp1, objp1.m_v) ) return;
  if ( ! screen->getObjCoord(tid, mp2, objp2.m_v) ) return;

  CES::Vec3<float> tv = objp2 - objp1;
  pT->trans(tv.m_v);

  p_gfxView->sceneGraphUpdated();
}


//----------------------------------------------------------------
// class vsnGfxAct_Selection
//----------------------------------------------------------------
void vsnGfxAct_Selection::execute(vfrEvent& e) {
  vfrScreen* screen = (vfrScreen*)e.getScreen();
  if ( ! screen ) return;
  vsnScene* psc = p_gfxView->getScene();
  if( ! psc ) return;

  unsigned int* r;

  if ( e.isClickEvent() ) {
    Point2 cp = screen->getRelativePoint(e.getMPoint());
    r = screen->clickSelect(cp.x, cp.y);
  }
  else if ( e.isDragEndEvent() ) {
    Point2 cp0, cp1;
    screen->clearRubberBox(&cp0, &cp1);
    psc->setXforming(false);
    cp0 = screen->getRelativePoint(cp0);
    cp1 = screen->getRelativePoint(cp1);
    int w = cp1.x - cp0.x;
    int h = cp1.y - cp0.y;
    if ( w < 0 ) {
      cp0.x += w;
      w *= -1;
    }
    if ( h < 0 ) {
      cp0.y += h;
      h *= -1;
    }
    r = screen->sweepSelect(cp0.x, cp0.y, w, h);
  }
  else return;

  if ( r[0] > 0 ) {
    vfrNode* co = screen->getNode(r[1]);
    if ( co ){
      ////VSN::ErrMsg(VSN::MsgINFO, co->getName() + string(" selected"));

      // get UiView
      vfrDrawAreaWx* pda
	= dynamic_cast<vfrDrawAreaWx*>(e.getScreen()->getParent());
      if ( ! pda ) return;
      vsnViewFrame* pvf
	= dynamic_cast<vsnViewFrame*>(pda->getCanvas()->GetParent());
      if ( ! pvf ) return;
      vsnUiView* puiv = pvf->getUiView();
      if ( ! puiv ) return;

      vfrNode* ptgt = co;

      // check MethodLutRefer (colorBar)
      vsnMethodLutRefer* plr = dynamic_cast<vsnMethodLutRefer*>(co);
      if ( plr ) ptgt = plr->getMethodObj();

      // check FrontLabel
      vsnFrontLabel* pfl = dynamic_cast<vsnFrontLabel*>(co);
      if ( pfl ) ptgt = pfl->getMethodObj();

      // set current obj of UiView
      puiv->selectObj(ptgt);
      
      return;
    } // end of if(co)
  } // end of if(r[0])

  ////VSN::ErrMsg(VSN::MsgINFO, "nothing selected");
}


//----------------------------------------------------------------
// class vsnGfxAct_StartRBox
//----------------------------------------------------------------
void vsnGfxAct_StartRBox::execute(vfrEvent& e) {
  vfrScreen* screen = (vfrScreen*)e.getScreen();
  if ( ! screen ) return;
  vfrDrawArea* pda = screen->getParent();
  if ( ! pda ) return;
  vsnScene* psc = p_gfxView->getScene();
  if ( psc ) psc->setXforming(true);
  pda->setRubberBoxColor(m_color);
  pda->setRubberBoxLineWidth((GLfloat)m_lw);
  pda->setRubberBoxLineType(m_lt);
  screen->startRubberBox(e.getMPoint());
}


//----------------------------------------------------------------
// class vsnGfxAct_SweepZoom
//----------------------------------------------------------------
void vsnGfxAct_SweepZoom::execute(vfrEvent& e) {
  vfrScreen* screen = (vfrScreen*)e.getScreen();
  if ( ! screen ) return;

  Point2 cp0, cp1;
  screen->clearRubberBox(&cp0, &cp1);
  vsnScene* psc = p_gfxView->getScene();
  if ( psc ) psc->setXforming(false);
  cp0 = screen->getRelativePoint(cp0);
  cp1 = screen->getRelativePoint(cp1);
  p_gfxView->sweepZoomView(cp0, cp1);
}


//----------------------------------------------------------------
// class vsnGfxAct_Wheel
//----------------------------------------------------------------
void vsnGfxAct_Wheel::execute(vfrEvent& e) {
  if ( ! p_gfxView ) return;
  vfrScreen* screen = (vfrScreen*)e.getScreen();
  if ( ! screen ) return;
  vfrNode* pnode = p_gfxView->getScaleNode();
  if ( ! pnode ) return;

  int wr = e.getWheelRot();
  if ( wr == 0 ) return;
  float scale = 1.f;
  if ( wr < 0 )
    scale = 1.05f;
  else
    scale = 0.9756f; // 1/1.025
  pnode->scale(scale);
  p_gfxView->sceneGraphUpdated();
}


//----------------------------------------------------------------
// struct vsnGfxOperation
//----------------------------------------------------------------

vfrEvent&
vsnGfxOperation::getEvent(vfrDispatch& d, const VSN::EvtType et) const {
  switch ( m_mouseButton ) {
  case MBLeft:
    if ( m_modKey == (ModKeyCtrl|ModKeyShift) ) {
      switch ( et ) {
      case EvtClick:     return d.SCClick();
      case EvtDragStart: return d.SCDragStart();
      case EvtDrag:      return d.SCDrag();
      case EvtDragEnd:   return d.SCDragEnd();
      }
    } else if ( m_modKey == ModKeyCtrl ) {
      switch ( et ) {
      case EvtClick:     return d.CClick();
      case EvtDragStart: return d.CDragStart();
      case EvtDrag:      return d.CDrag();
      case EvtDragEnd:   return d.CDragEnd();
      }
    } else if ( m_modKey == ModKeyShift ) {
      switch ( et ) {
      case EvtClick:     return d.SClick();
      case EvtDragStart: return d.SDragStart();
      case EvtDrag:      return d.SDrag();
      case EvtDragEnd:   return d.SDragEnd();
      }
    } else {
     switch ( et ) {
      case EvtClick:     return d.Click();
      case EvtDragStart: return d.DragStart();
      case EvtDrag:      return d.Drag();
      case EvtDragEnd:   return d.DragEnd();
      }
    }
    break;
  case MBMiddle:
    if ( m_modKey == (ModKeyCtrl|ModKeyShift) ) {
      switch ( et ) {
      case EvtClick:     return d.MSCClick();
      case EvtDragStart: return d.MSCDragStart();
      case EvtDrag:      return d.MSCDrag();
      case EvtDragEnd:   return d.MSCDragEnd();
      }
    } else if ( m_modKey == ModKeyCtrl ) {
      switch ( et ) {
      case EvtClick:     return d.MCClick();
      case EvtDragStart: return d.MCDragStart();
      case EvtDrag:      return d.MCDrag();
      case EvtDragEnd:   return d.MCDragEnd();
      }
    } else if ( m_modKey == ModKeyShift ) {
      switch ( et ) {
      case EvtClick:     return d.MSClick();
      case EvtDragStart: return d.MSDragStart();
      case EvtDrag:      return d.MSDrag();
      case EvtDragEnd:   return d.MSDragEnd();
      }
    } else {
     switch ( et ) {
      case EvtClick:     return d.MClick();
      case EvtDragStart: return d.MDragStart();
      case EvtDrag:      return d.MDrag();
      case EvtDragEnd:   return d.MDragEnd();
      }
    }
    break;
  case MBRight:
    if ( m_modKey == (ModKeyCtrl|ModKeyShift) ) {
      switch ( et ) {
      case EvtClick:     return d.RSCClick();
      case EvtDragStart: return d.RSCDragStart();
      case EvtDrag:      return d.RSCDrag();
      case EvtDragEnd:   return d.RSCDragEnd();
      }
    } else if ( m_modKey == ModKeyCtrl ) {
      switch ( et ) {
      case EvtClick:     return d.RCClick();
      case EvtDragStart: return d.RCDragStart();
      case EvtDrag:      return d.RCDrag();
      case EvtDragEnd:   return d.RCDragEnd();
      }
    } else if ( m_modKey == ModKeyShift ) {
      switch ( et ) {
      case EvtClick:     return d.RSClick();
      case EvtDragStart: return d.RSDragStart();
      case EvtDrag:      return d.RSDrag();
      case EvtDragEnd:   return d.RSDragEnd();
      }
    } else {
     switch ( et ) {
      case EvtClick:     return d.RClick();
      case EvtDragStart: return d.RDragStart();
      case EvtDrag:      return d.RDrag();
      case EvtDragEnd:   return d.RDragEnd();
      }
    }
    break;
  } // end of switch(m_mouseButton)

  return d.KeyIn(); // error
}


//----------------------------------------------------------------
// struct vsnGfxOprOrientation
//----------------------------------------------------------------

void vsnGfxOprOrientation::operator=(const vsnGfxOprOrientation& org) {
  register int i;
  for ( i = 0; i < OprNUM; i++ )
    m_oprLst[i] = org.m_oprLst[i];
}

void vsnGfxOprOrientation::setupStandardOrientation() {
  m_oprLst[OprRotate]    = vsnGfxOperation(MBLeft);
  m_oprLst[OprRoll]      = vsnGfxOperation(MBMiddle);
  m_oprLst[OprTranslate] = vsnGfxOperation(MBLeft, ModKeyShift);
  m_oprLst[OprScale]     = vsnGfxOperation(MBLeft, ModKeyCtrl);
  m_oprLst[OprSelect]    = vsnGfxOperation(MBLeft, (ModKeyCtrl|ModKeyShift));
  m_oprLst[OprSweepZoom] = vsnGfxOperation(MBMiddle, ModKeyShift);
}

bool vsnGfxOprOrientation::isValid() const {
  register int i, j;
  for ( j = 0; j < OprNUM -1; j++ )
    for ( i = j + 1; i < OprNUM; i++ )
      if ( m_oprLst[i] == m_oprLst[j] ) return false;
  return true;
}

/* configuration methods */

void vsnGfxOprOrientation::readConfig() {
  wxConfigBase* pConfig = wxConfigBase::Get();
  if ( ! pConfig ) return;
  bool val;
  vsnGfxOprOrientation oprOri(false);
  vsnGfxOprOrientation stdOri(true);

  // rotate
  val = (stdOri.m_oprLst[OprRotate].m_modKey & ModKeyCtrl);
  if ( pConfig->Read(wxT("/Gfx/Opr/RotateCtrl"), val) )
    oprOri.m_oprLst[OprRotate].m_modKey |= ModKeyCtrl;
  val = (stdOri.m_oprLst[OprRotate].m_modKey & ModKeyShift);
  if ( pConfig->Read(wxT("/Gfx/Opr/RotateShift"), val) )
    oprOri.m_oprLst[OprRotate].m_modKey |= ModKeyShift;
  oprOri.m_oprLst[OprRotate].m_mouseButton
    = pConfig->Read(wxT("/Gfx/Opr/RotateMBtn"),
		    (long)stdOri.m_oprLst[OprRotate].m_mouseButton);

  // roll
  val = (stdOri.m_oprLst[OprRoll].m_modKey & ModKeyCtrl);
  if ( pConfig->Read(wxT("/Gfx/Opr/RollCtrl"), val) )
    oprOri.m_oprLst[OprRoll].m_modKey |= ModKeyCtrl;
  val = (stdOri.m_oprLst[OprRoll].m_modKey & ModKeyShift);
  if ( pConfig->Read(wxT("/Gfx/Opr/RollShift"), val) )
    oprOri.m_oprLst[OprRoll].m_modKey |= ModKeyShift;
  oprOri.m_oprLst[OprRoll].m_mouseButton
    = pConfig->Read(wxT("/Gfx/Opr/RollMBtn"),
		    (long)stdOri.m_oprLst[OprRoll].m_mouseButton);

  // scale
  val = (stdOri.m_oprLst[OprScale].m_modKey & ModKeyCtrl);
  if ( pConfig->Read(wxT("/Gfx/Opr/ScaleCtrl"), val) )
    oprOri.m_oprLst[OprScale].m_modKey |= ModKeyCtrl;
  val = (stdOri.m_oprLst[OprScale].m_modKey & ModKeyShift);
  if ( pConfig->Read(wxT("/Gfx/Opr/ScaleShift"), val) )
    oprOri.m_oprLst[OprScale].m_modKey |= ModKeyShift;
  oprOri.m_oprLst[OprScale].m_mouseButton
    = pConfig->Read(wxT("/Gfx/Opr/ScaleMBtn"),
		    (long)stdOri.m_oprLst[OprScale].m_mouseButton);

  // translate
  val = (stdOri.m_oprLst[OprTranslate].m_modKey & ModKeyCtrl);
  if ( pConfig->Read(wxT("/Gfx/Opr/TranslateCtrl"), val) )
    oprOri.m_oprLst[OprTranslate].m_modKey |= ModKeyCtrl;
  val = (stdOri.m_oprLst[OprTranslate].m_modKey & ModKeyShift);
  if ( pConfig->Read(wxT("/Gfx/Opr/TranslateShift"), val) )
    oprOri.m_oprLst[OprTranslate].m_modKey |= ModKeyShift;
  oprOri.m_oprLst[OprTranslate].m_mouseButton
    = pConfig->Read(wxT("/Gfx/Opr/TranslateMBtn"),
		    (long)stdOri.m_oprLst[OprTranslate].m_mouseButton);

  // select
  val = (stdOri.m_oprLst[OprSelect].m_modKey & ModKeyCtrl);
  if ( pConfig->Read(wxT("/Gfx/Opr/SelectCtrl"), val) )
    oprOri.m_oprLst[OprSelect].m_modKey |= ModKeyCtrl;
  val = (stdOri.m_oprLst[OprSelect].m_modKey & ModKeyShift);
  if ( pConfig->Read(wxT("/Gfx/Opr/SelectShift"), val) )
    oprOri.m_oprLst[OprSelect].m_modKey |= ModKeyShift;
  oprOri.m_oprLst[OprSelect].m_mouseButton
    = pConfig->Read(wxT("/Gfx/Opr/SelectMBtn"),
		    (long)stdOri.m_oprLst[OprSelect].m_mouseButton);

  // sweep zoom
  val = (stdOri.m_oprLst[OprSweepZoom].m_modKey & ModKeyCtrl);
  if ( pConfig->Read(wxT("/Gfx/Opr/SweepZoomCtrl"), val) )
    oprOri.m_oprLst[OprSweepZoom].m_modKey |= ModKeyCtrl;
  val = (stdOri.m_oprLst[OprSweepZoom].m_modKey & ModKeyShift);
  if ( pConfig->Read(wxT("/Gfx/Opr/SweepZoomShift"), val) )
    oprOri.m_oprLst[OprSweepZoom].m_modKey |= ModKeyShift;
  oprOri.m_oprLst[OprSweepZoom].m_mouseButton
    = pConfig->Read(wxT("/Gfx/Opr/SweepZoomMBtn"),
		    (long)stdOri.m_oprLst[OprSweepZoom].m_mouseButton);

  if ( oprOri.isValid() )
    *this = oprOri;
}

void vsnGfxOprOrientation::writeConfig() const {
  wxConfigBase* pConfig = wxConfigBase::Get();
  if ( ! pConfig ) return;
  bool val;

  // rotate
  val = (m_oprLst[OprRotate].m_modKey & ModKeyCtrl);
  pConfig->Write(wxT("/Gfx/Opr/RotateCtrl"), val);
  val = (m_oprLst[OprRotate].m_modKey & ModKeyShift);
  pConfig->Write(wxT("/Gfx/Opr/RotateShift"), val);
  pConfig->Write(wxT("/Gfx/Opr/RotateMBtn"),
		 (long)m_oprLst[OprRotate].m_mouseButton);

  // roll
  val = (m_oprLst[OprRoll].m_modKey & ModKeyCtrl);
  pConfig->Write(wxT("/Gfx/Opr/RollCtrl"), val);
  val = (m_oprLst[OprRoll].m_modKey & ModKeyShift);
  pConfig->Write(wxT("/Gfx/Opr/RollShift"), val);
  pConfig->Write(wxT("/Gfx/Opr/RollMBtn"),
		 (long)m_oprLst[OprRoll].m_mouseButton);

  // scale
  val = (m_oprLst[OprScale].m_modKey & ModKeyCtrl);
  pConfig->Write(wxT("/Gfx/Opr/ScaleCtrl"), val);
  val = (m_oprLst[OprScale].m_modKey & ModKeyShift);
  pConfig->Write(wxT("/Gfx/Opr/ScaleShift"), val);
  pConfig->Write(wxT("/Gfx/Opr/ScaleMBtn"),
		 (long)m_oprLst[OprScale].m_mouseButton);

  // translate
  val = (m_oprLst[OprTranslate].m_modKey & ModKeyCtrl);
  pConfig->Write(wxT("/Gfx/Opr/TranslateCtrl"), val);
  val = (m_oprLst[OprTranslate].m_modKey & ModKeyShift);
  pConfig->Write(wxT("/Gfx/Opr/TranslateShift"), val);
  pConfig->Write(wxT("/Gfx/Opr/TranslateMBtn"),
		 (long)m_oprLst[OprTranslate].m_mouseButton);

  // select
  val = (m_oprLst[OprSelect].m_modKey & ModKeyCtrl);
  pConfig->Write(wxT("/Gfx/Opr/SelectCtrl"), val);
  val = (m_oprLst[OprSelect].m_modKey & ModKeyShift);
  pConfig->Write(wxT("/Gfx/Opr/SelectShift"), val);
  pConfig->Write(wxT("/Gfx/Opr/SelectMBtn"),
		 (long)m_oprLst[OprSelect].m_mouseButton);

  // sweep zoom
  val = (m_oprLst[OprSweepZoom].m_modKey & ModKeyCtrl);
  pConfig->Write(wxT("/Gfx/Opr/SweepZoomCtrl"), val);
  val = (m_oprLst[OprSweepZoom].m_modKey & ModKeyShift);
  pConfig->Write(wxT("/Gfx/Opr/SweepZoomShift"), val);
  pConfig->Write(wxT("/Gfx/Opr/SweepZoomMBtn"),
		 (long)m_oprLst[OprSweepZoom].m_mouseButton);
}
