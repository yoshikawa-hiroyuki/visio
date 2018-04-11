//
// vsnRotScaleCenter
//
#include "vsnRotScaleCenter.h"
#include "vsnGfxView.h"
#include "vsnViewFrame.h"

using namespace CES;


//----------------------------------------------------------------
// class vsnGfxAct_KeyEscCenter
//----------------------------------------------------------------
void vsnGfxAct_KeyEscCenter::execute(vfrEvent& e) {
  if ( e.getKey() != VFRKC_ESCAPE ) return;
  if ( ! p_gfxView ) return;
  vsnViewFrame* pvf = p_gfxView->getParent();
  if ( ! pvf ) return;

  pvf->escapeSetCenterMode();
}


//----------------------------------------------------------------
// class vsnGfxAct_ClickCenter
//----------------------------------------------------------------
void vsnGfxAct_ClickCenter::execute(vfrEvent& e) {
  if ( ! p_gfxView ) return;

  // codes below coied from vsnGfxAct_Click::execute()
  p_gfxView->getDrawArea()->getCanvas()->SetFocus();

  vfrScreen* screen = (vfrScreen*)e.getScreen();
  if ( ! screen ) return;

  p_gfxView->setCenter(e.getMPoint());
}


//----------------------------------------------------------------
// class vsnGfxAct_DragTransCenter
//----------------------------------------------------------------
void vsnGfxAct_DragTransCenter::execute(vfrEvent& e) {
  if ( ! p_gfxView ) return;

  vfrScreen* screen = (vfrScreen*)e.getScreen();
  if ( ! screen ) return;

  vfrNode* cglyph = screen->getNode("CenterGlyph");
  if ( ! cglyph ) return;

  screen->translateNode(cglyph->getID(), e.getMPoint(), e.getMMove());
}


//----------------------------------------------------------------
// class vsnGfxAct_DragDollyCenter
//----------------------------------------------------------------
void vsnGfxAct_DragDollyCenter::execute(vfrEvent& e) {
  if ( ! p_gfxView ) return;

  vfrScreen* screen = (vfrScreen*)e.getScreen();
  if ( ! screen ) return;
  Point2 vpSize = screen->getSize();
  int L = (vpSize.x > vpSize.y) ? vpSize.x : vpSize.y;
  if ( L < 1 ) return;

  vfrGroup* root = p_gfxView->getRootNode();
  if ( ! root ) return;
  const Vec3<float>* rbb = root->getBbox();
  float bbL = (rbb[1] - rbb[0]).Length();
  if ( bbL < 0.00035f ) return;
  unsigned int tid = root->getID();
  
  vfrNode* cglyph = screen->getNode("CenterGlyph");
  if ( ! cglyph ) return;

  Point2 mv = e.getMMove();
  if ( mv.y == 0 ) return;

  Point2 mp0 = e.getMPoint();
  Point2 mp1, mp2;
  mp1 = mp2 = mp0;
  mp1.x += 2;
  mp2.y += 2;
  Vec3<float> objp0, objp1, objp2;
  if ( ! screen->getObjCoord(tid, mp0, objp0.m_v) ) return;
  if ( ! screen->getObjCoord(tid, mp1, objp1.m_v) ) return;
  if ( ! screen->getObjCoord(tid, mp2, objp2.m_v) ) return;

  Vec3<float> zA = (objp1 - objp0) ^ (objp2 - objp0);
  zA.UnitVec();
  zA = zA * (bbL * mv.y / L);

  cglyph->trans(zA.m_v);
}


//----------------------------------------------------------------
// class vsnGfxAct_DragEndCenter
//----------------------------------------------------------------
void vsnGfxAct_DragEndCenter::execute(vfrEvent& e) {
  if ( ! p_gfxView ) return;

  vector3 cp;
  p_gfxView->getCenter(cp);
  p_gfxView->setCenter(cp);

  vsnScene* psc = p_gfxView->getScene();
  if ( psc ) psc->setXforming(false);
}

