//
// vsnGfxView
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
#include "wx/image.h"
#include "wx/config.h"

#include "vsnGfxView.h"
#include "vsnApp.h"
#include "vsnGfxActions.h"
#include "vsnViewFrame.h"

#include "vfrCamera3D.h"
#include "vfrDefaultActions.h"
#include <assert.h>

#define CAMERA_FOCUS 15.f
#define CAMERA_NEAR  0.5
#define CAMERA_FAR   1000.5

using namespace CES;


//----------------------------------------------------------------
// class vsnGfxView
//----------------------------------------------------------------

/* static members */

bool  vsnGfxView::s_xformAnim = true;
float vsnGfxView::s_xformAnimDuration = 0.5f;
float vsnGfxView::s_keyZoomInRatio = 1.1;
float vsnGfxView::s_keyZoomInRatio2 = 2.0f;
float vsnGfxView::s_keyZoomOutRatio = 0.9f;
float vsnGfxView::s_keyZoomOutRatio2 = 0.5f;


/* constructor / destructor */

vsnGfxView::vsnGfxView(vsnViewFrame* parent,
		       const wxPoint& pos, const wxSize& size)
  : m_pda(NULL), m_pScreen(NULL), m_pCamera(NULL), m_pRootScene(NULL),
    m_pTransNode(NULL), m_pRotateNode(NULL), m_pScaleNode(NULL),
    m_pCenterNode(NULL), m_pInvCentNode(NULL), m_pCentGlyph(NULL),
    m_pRootNode(NULL), m_pFrAxis(NULL),
    p_parent(NULL), m_pCurActKey((void*)this),
    gfxAct_KeyIn(this), gfxAct_Click(this), gfxAct_RotScene(this),
    gfxAct_ScaleScene(this), gfxAct_RollScene(this),
    gfxAct_TransScene(this), gfxAct_Selection(this),
    gfxAct_SweepZoom(this),  gfxAct_StartRBoxSelect(this),
    gfxAct_StartRBoxZoom(this, 1.5f, VFR::ST_DASH), gfxAct_Wheel(this)
{
  p_parent = parent;

  /* gfx draw_area */
  m_pda = vfrDrawAreaWx::GetInstance(parent, pos, size);
  assert(m_pda);

  /* screen */
  m_pScreen = new vfrScreen();
  assert(m_pScreen);
  m_pda->addScreen(m_pScreen);

  /* camera */
  m_pCamera = new vfrCamera3D();
  assert(m_pCamera);
  vfrFrustum Frs = m_pCamera->getFrustum();
  Frs.m_eye = Vec3<float>(0,0,CAMERA_FOCUS);
  Frs.m_dist = CAMERA_FOCUS;
  Frs.m_near = CAMERA_NEAR; Frs.m_far = CAMERA_FAR;
  m_pCamera->setFrustum(Frs);
  m_pScreen->setCamera(m_pCamera);
  vfrGroup* pCamFront = new vfrGroup("CameraFront");
  assert(pCamFront);
  m_pCamera->setFrontNode(pCamFront);

  /* root-scene */
  m_pRootScene = new vfrScene("RootScene");
  assert(m_pRootScene);
  m_pCamera->setScene(m_pRootScene);

  /* xform-nodes */
  m_pTransNode = new vfrGroup("TransNode");
  assert(m_pTransNode);
  m_pRootScene->addChild(m_pTransNode);

  m_pCenterNode = new vfrGroup("CenterNode");
  assert(m_pCenterNode);
  m_pTransNode->addChild(m_pCenterNode);

  m_pRotateNode = new vfrGroup("RotateNode");
  assert(m_pRotateNode);
  m_pCenterNode->addChild(m_pRotateNode);

  m_pScaleNode = new vfrGroup("ScaleNode");
  assert(m_pScaleNode);
  m_pRotateNode->addChild(m_pScaleNode);

  m_pInvCentNode = new vfrGroup("InvCentNode");
  assert(m_pInvCentNode);
  m_pScaleNode->addChild(m_pInvCentNode);

  /* root-node */
  m_pRootNode = new vfrGroup("RootNode");
  assert(m_pRootNode);
  m_pInvCentNode->addChild(m_pRootNode);

  /* center glyph */
  m_pCentGlyph = new vfrLines("CenterGlyph");
  assert(m_pCentGlyph);
  if ( m_pCentGlyph ) {
    vector3 vp[] = {{-1.f, 0.f, 0.f}, {1.f, 0.f, 0.f},
		    {0.f, -1.f, 0.f}, {0.f, 1.f, 0.f},
		    {0.f, 0.f, -1.f}, {0.f, 0.f, 1.f}};
    vector3 cp[] = {{1.f, 0.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 0.f, 1.f}};
    m_pCentGlyph->setVerts(6, vp);
    m_pCentGlyph->setColors3(3, cp);
    vfrMaterial* pcm = m_pCentGlyph->alcMaterial();
    if ( pcm ) pcm->setLineStipple(ST_DOT);
    m_pCentGlyph->setColorMode(AT_PER_FACE);
    m_pInvCentNode->addChild(m_pCentGlyph);
  }
  adjustCenterGlyph();

  /* front axis */
  m_pFrAxis = new vsnFrontAxis("FrontAxis");
  assert(m_pFrAxis);
  pCamFront->addChild(m_pFrAxis);

  /* action handlers of draw_area */
  setStandardActions();
}

vsnGfxView::~vsnGfxView() {
  if ( m_pFrAxis ) delete m_pFrAxis;
  if ( m_pCentGlyph ) delete m_pCentGlyph;
  if ( m_pRootNode ) delete m_pRootNode;
  if ( m_pInvCentNode ) delete m_pInvCentNode;
  if ( m_pScaleNode ) delete m_pScaleNode;
  if ( m_pRotateNode ) delete m_pRotateNode;
  if ( m_pCenterNode ) delete m_pCenterNode;
  if ( m_pTransNode ) delete m_pTransNode;
  if ( m_pRootScene ) delete m_pRootScene;
  if ( m_pCamera ) delete m_pCamera;
  if ( m_pScreen ) delete m_pScreen;

  //if ( m_pda ) delete m_pda; // Wx object, don't delete
}


void vsnGfxView::setScene(vsnScene* sc) {
  vsnScene* oldSc = NULL;
  for ( register int i = 0; i < m_pRootNode->getNumChildren(); i++ ) {
    oldSc = dynamic_cast<vsnScene*>(m_pRootNode->getChild(i));
    if ( oldSc ) break;
  }
  if ( oldSc ) {
    m_pRootNode->remChild(oldSc);
    vfrGroup* pCamFront = dynamic_cast<vfrGroup*>(m_pCamera->getFrontNode());
    vfrGroup* pFrRoot = oldSc->getFrontObjGrp();
    if ( pCamFront && pFrRoot )
      pCamFront->remChild(pFrRoot);
  }

  if ( sc ) {
    m_pRootNode->addChild(sc);
    vfrGroup* pCamFront = dynamic_cast<vfrGroup*>(m_pCamera->getFrontNode());
    vfrGroup* pFrRoot = sc->getFrontObjGrp();
    if ( pCamFront && pFrRoot )
      pCamFront->addChild(pFrRoot);

	vsnLightAttribute& l = sc->getLightAttr();
	l.applyLight();
  }

  // reset actions and ActSet-stack
  m_pCurActKey = (void*)this;
  setStandardActions();
  setCursor();

  // reset transformation
  m_pRootNode->identity();
  m_pInvCentNode->identity();
  m_pScaleNode->identity();
  m_pRotateNode->identity();
  m_pCenterNode->identity();
  m_pTransNode->identity();
  m_pRootScene->identity();
  m_pFrAxis->setRotMatrix(m_pRotateNode->getMatrix());

  vector3 cv = {0.f, 0.f, 0.f};
  setCenter(cv);

  sceneGraphUpdated();
}

vsnScene* vsnGfxView::getScene() {
  vsnScene* psc = NULL;
  for ( register int i = 0; i < m_pRootNode->getNumChildren(); i++ ) {
    psc = dynamic_cast<vsnScene*>(m_pRootNode->getChild(i));
    if ( psc ) break;
  }
  return psc;
}

bool vsnGfxView::setProjection(const VFR::ProjectType pm) {
  if ( ! m_pCamera ) return false;
  if ( pm == m_pCamera->getProjection() ) return true;

  m_pCamera->setProjection(pm);
  vfrFrustum Frs = m_pCamera->getFrustum();
  if ( pm == VFR::PR_ORTHOGONAL ) {
    Frs.m_far = CAMERA_FAR*2;
    Frs.m_eye = Vec3<float>(0,0,CAMERA_FAR);
  } else {
    Frs.m_far = CAMERA_FAR;
    Frs.m_eye = Vec3<float>(0,0,CAMERA_FOCUS);
  }
  m_pCamera->setFrustum(Frs);

  return true;
}

void vsnGfxView::setXForm(const vsnViewPoint& xfm) {
  // reset w/o rotateNode
  m_pTransNode->identity();
  m_pScaleNode->identity();
  m_pCenterNode->identity();
  m_pInvCentNode->identity();

  // set transformation
  m_pTransNode->trans(xfm.vT.m_v);
  m_pScaleNode->scale(xfm.vS.m_v);
  m_pCenterNode->trans(xfm.vC.m_v);
  Vec3<float> ivC = xfm.vC * (-1.f);
  m_pInvCentNode->trans(ivC.m_v);
  m_pRotateNode->setMatrix(xfm.mR);
  m_pFrAxis->setRotMatrix(xfm.mR);

  // set center glyph
  m_pCentGlyph->identity();
  m_pCentGlyph->trans(xfm.vC.m_v);
  adjustCenterGlyph();

  sceneGraphUpdated();
}

vsnViewPoint vsnGfxView::getXForm() const {
  vsnViewPoint xfm;
  const vfrMatrix& mT = m_pTransNode->getMatrix();
  xfm.vT.m_v[0] = mT[12]; xfm.vT.m_v[1] = mT[13]; xfm.vT.m_v[2] = mT[14];
  const vfrMatrix& mC = m_pCenterNode->getMatrix();
  xfm.vC.m_v[0] = mC[12]; xfm.vC.m_v[1] = mC[13]; xfm.vC.m_v[2] = mC[14];
  const vfrMatrix& mS = m_pScaleNode->getMatrix();
  xfm.vS.m_v[0] = mS[0]; xfm.vS.m_v[1] = mS[5]; xfm.vS.m_v[2] = mS[10];
  xfm.mR = m_pRotateNode->getMatrix();
  return xfm;
}

void vsnGfxView::setViewXForm(const vsnViewPoint& xfm) {
  if ( ! s_xformAnim || s_xformAnimDuration <= 0.f ) {setXForm(xfm); return;}

  vsnApp* pApp = vsnApp::GetApp();
  vsnScene* psc = getScene();
  if( !psc ) return;

  vsnViewPoint wxfm, xfm0 = getXForm();
  CES::Quat4<float> rQ0 = xfm0.getRotQuat();
  CES::Quat4<float> rQ = xfm.getRotQuat();
  CES::Vec3<float> wT = xfm.vT - xfm0.vT;
  CES::Vec3<float> wC = xfm.vC - xfm0.vC;
  CES::Vec3<float> wS = xfm.vS - xfm0.vS;

  register double totalTime = (double)s_xformAnimDuration;
  register double startTime = vsnApp::GetTime();
  register double elapsTime = 0.0;

  if ( psc && vsnScene::GetXformSimpleRender() )
    psc->setXforming(true);

  while ( elapsTime < totalTime ) {
    register float dt = (float)(elapsTime / totalTime);

    wxfm.mR = CES::QuatSlerp(rQ0, rQ, dt).GetRotMat();
    wxfm.vT = xfm0.vT + (wT * dt);
    wxfm.vC = xfm0.vC + (wC * dt);
    wxfm.vS = xfm0.vS + (wS * dt);
    setXForm(wxfm);
    m_pda->chkNotice();
    pApp->Yield();

    elapsTime = vsnApp::GetTime() - startTime;
  } // end of for(elapsTime)

  setXForm(xfm); // the last xform
  m_pda->chkNotice();

  if ( psc && vsnScene::GetXformSimpleRender() ) {
    psc->setXforming(false);
    m_pda->chkNotice();
  }
}


bool vsnGfxView::normalize(const bool withCenter, const vfrNode* pTgt) {
  // reset w/o rotateNode
  m_pTransNode->identity();
  m_pScaleNode->identity();
  if ( withCenter ) {
    m_pCenterNode->identity();
    m_pInvCentNode->identity();
    m_pCentGlyph->identity();
  }
  m_pRootNode->identity();

  // get bbox of Root
  Vec3<float> sbb[2];
  if ( pTgt ) {
    vfrMatrix M; m_pRootNode->accumMatrix(pTgt->getID(), M);
    const CES::Vec3<float>* bbox = pTgt->getBbox();
    CES::Vec3<float> local[8], out;
    local[0][0]=bbox[1][0]; local[0][1]=bbox[0][1]; local[0][2]=bbox[0][2];
    local[1][0]=bbox[1][0]; local[1][1]=bbox[1][1]; local[1][2]=bbox[0][2];
    local[2][0]=bbox[1][0]; local[2][1]=bbox[1][1]; local[2][2]=bbox[1][2];
    local[3][0]=bbox[1][0]; local[3][1]=bbox[0][1]; local[3][2]=bbox[1][2];
    local[4][0]=bbox[0][0]; local[4][1]=bbox[0][1]; local[4][2]=bbox[0][2];
    local[5][0]=bbox[0][0]; local[5][1]=bbox[0][1]; local[5][2]=bbox[1][2];
    local[6][0]=bbox[0][0]; local[6][1]=bbox[1][1]; local[6][2]=bbox[1][2];
    local[7][0]=bbox[0][0]; local[7][1]=bbox[1][1]; local[7][2]=bbox[0][2];
    out = M * local[0];
    sbb[0] = out; sbb[1] = out;
    register int i;
    for ( i = 1; i < 8; i++ ) {
      out = M * local[i];
      if ( sbb[0][0] > out[0] ) sbb[0][0] = out[0];
      if ( sbb[1][0] < out[0] ) sbb[1][0] = out[0];
      if ( sbb[0][1] > out[1] ) sbb[0][1] = out[1];
      if ( sbb[1][1] < out[1] ) sbb[1][1] = out[1];
      if ( sbb[0][2] > out[2] ) sbb[0][2] = out[2];
      if ( sbb[1][2] < out[2] ) sbb[1][2] = out[2];
    } // end of for(i)
  } else m_pRootNode->getMatrixBbox(sbb);
  float bbLen = (sbb[1] - sbb[0]).Length();

  // scale
  float vw = m_pCamera->getFrustum().m_halfH * 2.f;
  if ( bbLen > 1e-6f )
    m_pScaleNode->scale(vw / bbLen);

  // translate
  Vec3<float> bbC = (sbb[1] + sbb[0]) * 0.5f;
  Vec3<float> xbbC(bbC);
  vfrMatrix X;
  if ( m_pCenterNode->accumMatrix(m_pRootNode->getID(), X) )
    xbbC = X * bbC;
  m_pTransNode->trans(-xbbC[0], -xbbC[1], -xbbC[2]);

  // set center
  if ( withCenter )
    setCenter(bbC.m_v);
  else
    adjustCenterGlyph();

  sceneGraphUpdated();
  return true;
}

bool vsnGfxView::normalizeView(const bool withCenter, const vfrNode* pTgt) {
  if ( ! s_xformAnim || s_xformAnimDuration <= 0.f )
    return normalize(withCenter, pTgt);

  vsnViewPoint vp0 = getXForm(); // current xform
  if ( ! normalize(withCenter, pTgt) ) return false;
  vsnViewPoint vp1 = getXForm(); // normalized xform
  setXForm(vp0);

  setViewXForm(vp1);
  return true;
}

bool vsnGfxView::sweepZoom(const Point2 p0, const Point2 p1) {
  Point2 pp; pp.x = (p0.x + p1.x) / 2; pp.y = (p0.y + p1.y) / 2;
  Point2 vpSize = m_pScreen->getSize();
  Point2 m0; m0.x = vpSize.x / 2; m0.y = vpSize.y / 2;

  // translate
  unsigned int tid = m_pRootScene->getID();
  CES::Vec3<float> objp1, objp2;
  if ( ! m_pScreen->getObjCoord(tid, m0, objp1.m_v) ) return false;
  if ( ! m_pScreen->getObjCoord(tid, pp, objp2.m_v) ) return false;
  CES::Vec3<float> tv = objp1 - objp2;
  m_pTransNode->trans(tv.m_v);

  //center 
  setCenter(m0);

  // zoom
  float ppLen = CES::Vec3<float>(p1.x - p0.x, p1.y - p0.y, 0.f).Length();
  float vpLen = CES::Vec3<float>(vpSize.x, vpSize.y, 0.f).Length();
  if ( ppLen > 1e-6f ) m_pScaleNode->scale(vpLen / ppLen);

  sceneGraphUpdated();
  return true;
}

bool vsnGfxView::sweepZoomView(const Point2 p0, const Point2 p1) {
  if ( ! s_xformAnim || s_xformAnimDuration <= 0.f ) {
    if ( ! sweepZoom(p0, p1) ) return false;
    m_pda->chkNotice();
    return true;
  }

  vsnViewPoint vp0 = getXForm(); // current xform
  if ( ! sweepZoom(p0, p1) ) {
    setXForm(vp0);
    return false;
  }
  vsnViewPoint vp1 = getXForm(); // normalized xform
  setXForm(vp0);

  setViewXForm(vp1);
  return true;
}


void vsnGfxView::sceneGraphUpdated() {
  // feedback to front-axis
  m_pFrAxis->setRotMatrix(m_pRotateNode->getMatrix());

  // adjust far-clip
#if 0
  const Vec3<float>* sbb= m_pRootNode->getBbox();
  float farLen = (sbb[1] - sbb[0]).Length() * m_pScaleNode->getMatrix().m_v[0];
  if ( farLen < CAMERA_FAR ) farLen = CAMERA_FAR;
  vfrFrustum Frs = m_pCamera->getFrustum();
  Frs.m_far = farLen;
  if ( m_pCamera->getProjection() == PR_ORTHOGONAL )
    Frs.m_near = -farLen;
  m_pCamera->setFrustum(Frs);
#endif

  // sync ViewPoint
  if ( p_parent ) {
    const set<vsnViewFrame*>& vpSlaves = p_parent->getViewPointSlaveList();
    set<vsnViewFrame*>::const_iterator it;
    for ( it = vpSlaves.begin(); it != vpSlaves.end(); it++ )
      (*it)->getGfxView()->setXForm(getXForm());
  }
}


void vsnGfxView::setCenter(const vector3 cp) {
  // scene graph tree: RootScene->T->C->R->S->C~->RootNode
  //  [X] := [T][C][R][S][C~]
  //  [Tnew] = [X]([Cnew][R][S][Cnew~])~

  vfrMatrix X, Y, iY;
  if ( ! m_pRootScene->accumMatrix(m_pRootNode->getID(), X) )
    return;
  Vec3<float> cv(cp);

  m_pTransNode->identity();
  m_pCenterNode->identity();
  m_pInvCentNode->identity();

  m_pCenterNode->trans(cv[0], cv[1], cv[2]);
  m_pInvCentNode->trans(-cv[0], -cv[1], -cv[2]);

  if ( ! m_pCenterNode->accumMatrix(m_pRootNode->getID(), Y) )
    return;
  iY = Y.Inverse();
  Y = X * iY; // [Tnew]
  Vec3<float> cv0 = Y * Vec3<float>(0.f, 0.f, 0.f);
  m_pTransNode->trans(cv0[0], cv0[1], cv0[2]);

  m_pCentGlyph->identity();
  m_pCentGlyph->trans(cv[0], cv[1], cv[2]);
  adjustCenterGlyph();

  sceneGraphUpdated();
}

void vsnGfxView::setCenter(const Point2 cp) {
  Point2 cp0, cd; vector3 o = {0.f, 0.f, 0.f};
  m_pScreen->getWinCoord(m_pCentGlyph->getID(), o, cp0);
  cp0.y = m_pScreen->getSize().y - cp0.y;
  cd.x = cp.x - cp0.x; cd.y = cp.y - cp0.y;
  m_pScreen->translateNode(m_pCentGlyph->getID(), cp0, cd);

  vector3 oc;
  getCenter(oc);
  setCenter(oc);
}

void vsnGfxView::getCenter(vector3 cp) {
  const vfrMatrix& mC = m_pCentGlyph->getMatrix();
  cp[0] = mC[12]; cp[1] = mC[13]; cp[2] = mC[14];
}

void vsnGfxView::adjustCenterGlyph() {
  if ( ! m_pCentGlyph ) return;

  const Vec3<float>* sbb = m_pRootNode->getBbox();
  Vec3<float> xbb = (sbb[1] - sbb[0]);
  if ( xbb.Length() < 0.00035f )
    xbb = Vec3<float>(1.f,1.f,1.f);
  if ( xbb.m_v[0] < 1e-6f ) xbb.m_v[0] = 1e-6f;
  if ( xbb.m_v[1] < 1e-6f ) xbb.m_v[1] = 1e-6f;
  if ( xbb.m_v[2] < 1e-6f ) xbb.m_v[2] = 1e-6f;
  vector3* pv = m_pCentGlyph->getVerts();
  pv[0][0] = -xbb.m_v[0] * 0.3f; pv[1][0] = xbb.m_v[0] * 0.3f;
  pv[2][1] = -xbb.m_v[1] * 0.3f; pv[3][1] = xbb.m_v[1] * 0.3f;
  pv[4][2] = -xbb.m_v[2] * 0.3f; pv[5][2] = xbb.m_v[2] * 0.3f;

  m_pCentGlyph->notice();
}


void vsnGfxView::setCursor(class wxCursor* pcurs) {
  if ( p_cursor == pcurs ) return;
  p_cursor = pcurs;

  if ( ! m_pda ) return;
  vfrWxCanvas* pcanv = m_pda->getCanvas();
  if ( ! pcanv ) return;

  if ( ! p_cursor )
    pcanv->SetCursor(*wxSTANDARD_CURSOR);
  else
    pcanv->SetCursor(*p_cursor);
}


Point2 vsnGfxView::getScreenShotSize() const {
  Point2 psz;
  if ( ! m_pda ) return psz;
  psz = m_pda->getSize();
  while ( psz.x % 4 ) psz.x --;
  return psz;
}

bool vsnGfxView::screenShot(const std::string& path) {
  if ( path.empty() ) return false;
  Point2 psz = getScreenShotSize();
  if ( psz.x * psz.y < 1 ) return false;

  m_pda->redraw();

  unsigned char* imgData = (unsigned char*)Allocate(psz.x * psz.y * 3);
  unsigned char* flipData = (unsigned char*)Allocate(psz.x * psz.y * 3);
  if ( ! imgData || ! flipData ) return false;

  glReadPixels(0, 0, psz.x, psz.y, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*)imgData);

  register int i;
  for ( i = 0; i < psz.y; i++ )
    memcpy(&flipData[3 * psz.x * i], &imgData[3 * psz.x *(psz.y -1 -i)],
           3 * psz.x);

  wxImage flipImg;
  flipImg.SetData(flipData, psz.x, psz.y);
  if ( ! flipImg.SaveFile(vsnApp::ConvSysToWx(path)) ) return false;

  DeAllocate(imgData); // no need to free flipData
  return true;
}

unsigned char* vsnGfxView::screenShot(const bool flip) {
  Point2 psz = getScreenShotSize();
  if ( psz.x * psz.y < 1 ) return NULL;

  m_pda->redraw();

  unsigned char* imgData = (unsigned char*)Allocate(psz.x * psz.y * 3);
  if ( ! imgData ) return NULL;
  unsigned char* flipData = NULL;
  if ( flip ) {
    flipData = (unsigned char*)Allocate(psz.x * psz.y * 3);
    if ( ! flipData ) {
      DeAllocate(imgData);
      return NULL;
    }
  }

  glReadPixels(0, 0, psz.x, psz.y, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*)imgData);

  if ( flip ) {
    register int i;
    for ( i = 0; i < psz.y; i++ )
      memcpy(&flipData[3 * psz.x * i], &imgData[3 * psz.x *(psz.y -1 -i)],
	     3 * psz.x);
    DeAllocate(imgData);
    imgData = flipData;
    flipData = NULL;
  }

  return imgData;
}


/* static methods */

void vsnGfxView::SetXformAnim(const bool mode, const float duration) {
  s_xformAnim = mode;
  if ( duration > 0.f ) s_xformAnimDuration = duration;
}

bool vsnGfxView::GetXformAnim() {
  return s_xformAnim;
}

float vsnGfxView::GetXformAnimDuration() {
  return s_xformAnimDuration;
}

void vsnGfxView::SetKeyZoomInRatio(const float r1, const float r2) {
  if ( r1 > 0.f ) s_keyZoomInRatio = r1;
  if ( r2 > 0.f ) s_keyZoomInRatio2 = r2;
}

void vsnGfxView::GetKeyZoomInRatio(float& r1, float& r2) {
  r1 = s_keyZoomInRatio;
  r2 = s_keyZoomInRatio2;
}

void vsnGfxView::SetKeyZoomOutRatio(const float r1, const float r2) {
  if ( r1 > 0.f ) s_keyZoomOutRatio = r1;
  if ( r2 > 0.f ) s_keyZoomOutRatio2 = r2;
}

void vsnGfxView::GetKeyZoomOutRatio(float& r1, float& r2) {
  r1 = s_keyZoomOutRatio;
  r2 = s_keyZoomOutRatio2;
}

void vsnGfxView::ReadConfig() {
  wxConfigBase* pConfig = wxConfigBase::Get();
  if ( ! pConfig ) return;
  wxString wks;

  s_xformAnim = pConfig->Read(wxT("/Gfx/Xform/Anim"), s_xformAnim);

  wks = wxString::Format(wxT("%f"), s_xformAnimDuration);
  wks = pConfig->Read(wxT("/Gfx/Xform/AnimDuration"), wks);
  if ( ! wks.IsEmpty() ) s_xformAnimDuration = (float)atof(wks.mb_str());

  wks = wxString::Format(wxT("%f"), s_keyZoomInRatio);
  wks = pConfig->Read(wxT("/Gfx/Xform/KeyZoomInRatio"), wks);
  if ( ! wks.IsEmpty() ) s_keyZoomInRatio = (float)atof(wks.mb_str());
  wks = wxString::Format(wxT("%f"), s_keyZoomInRatio2);
  wks = pConfig->Read(wxT("/Gfx/Xform/KeyZoomInRatio2"), wks);
  if ( ! wks.IsEmpty() ) s_keyZoomInRatio2 = (float)atof(wks.mb_str());

  wks = wxString::Format(wxT("%f"), s_keyZoomOutRatio);
  wks = pConfig->Read(wxT("/Gfx/Xform/KeyZoomOutRatio"), wks);
  if ( ! wks.IsEmpty() ) s_keyZoomOutRatio = (float)atof(wks.mb_str());
  wks = wxString::Format(wxT("%f"), s_keyZoomOutRatio2);
  wks = pConfig->Read(wxT("/Gfx/Xform/KeyZoomOutRatio2"), wks);
  if ( ! wks.IsEmpty() ) s_keyZoomOutRatio2 = (float)atof(wks.mb_str());
}

void vsnGfxView::WriteConfig() {
  wxConfigBase* pConfig = wxConfigBase::Get();
  if ( ! pConfig ) return;
  wxString wks;

  pConfig->Write(wxT("/Gfx/Xform/Anim"), s_xformAnim);

  wks = wxString::Format(wxT("%f"), s_xformAnimDuration);
  pConfig->Write(wxT("/Gfx/Xform/AnimDuration"), wks);

  wks = wxString::Format(wxT("%f"), s_keyZoomInRatio);
  pConfig->Write(wxT("/Gfx/Xform/KeyZoomInRatio"), wks);
  wks = wxString::Format(wxT("%f"), s_keyZoomInRatio2);
  pConfig->Write(wxT("/Gfx/Xform/KeyZoomInRatio2"), wks);

  wks = wxString::Format(wxT("%f"), s_keyZoomOutRatio);
  pConfig->Write(wxT("/Gfx/Xform/KeyZoomOutRatio"), wks);
  wks = wxString::Format(wxT("%f"), s_keyZoomOutRatio2);
  pConfig->Write(wxT("/Gfx/Xform/KeyZoomOutRatio2"), wks);
}
