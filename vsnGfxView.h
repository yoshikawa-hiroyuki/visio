//
// vsnGfxView
//
#ifndef _VSN_GFX_VIEW_H_
#define _VSN_GFX_VIEW_H_

#include "vfrDrawAreaWx.h"
#include "vfrScreen.h"
#include "vfrCamera.h"
#include "vsnScene.h"
#include "vfrScene.h"
#include "vfrLines.h"
#include "vsnGfxActions.h"
#include "vsnViewPoint.h"
#include "vsnFrontAxis.h"
#include <deque>

class vsnViewFrame;

namespace VSN {
  typedef std::pair<void*, vsnGfxActSet> ActSetEntry;
};


//----------------------------------------------------------------
// class vsnGfxView
//----------------------------------------------------------------
class vsnGfxView {
public:
  vsnGfxView(vsnViewFrame* parent,
	     const wxPoint& pos =wxDefaultPosition,
	     const wxSize& size =wxDefaultSize);
  virtual ~vsnGfxView();

  vfrDrawAreaWx* getDrawArea() {return m_pda;}
  vfrScreen* getScreen() {return m_pScreen;}
  vfrCamera* getCamera() {return m_pCamera;}
  vfrScene* getRootScene() {return m_pRootScene;}
  vfrGroup* getRootNode() {return m_pRootNode;}
  vsnFrontAxis* getFrontAxis() {return m_pFrAxis;}
  vsnViewFrame* getParent() {return p_parent;}

  vfrNode* getTranslateNode() {return m_pTransNode;}
  vfrNode* getRotateNode() {return m_pRotateNode;}
  vfrNode* getScaleNode() {return m_pScaleNode;}

  // setup the scene
  void setScene(vsnScene* sc);
  vsnScene* getScene();

  bool setProjection(const VFR::ProjectType pm);
  void setXForm(const vsnViewPoint& xfm);
  vsnViewPoint getXForm() const;
  bool normalize(const bool withCenter =true, const vfrNode* pTgt =NULL);
  bool sweepZoom(const Point2 p0, const Point2 p1);
  void sceneGraphUpdated();

  void setViewXForm(const vsnViewPoint& xfm);
  bool normalizeView(const bool withCenter =true, const vfrNode* pTgt =NULL);
  bool sweepZoomView(const Point2 p0, const Point2 p1);

  // rot/scale center control interface
  void adjustCenterGlyph();
  void setCenter(const vector3 cp);
  void setCenter(const Point2 cp);
  void getCenter(vector3 cp);
  vfrNode* getCenterGlyph() {return m_pCentGlyph;}

  // action control interface
  void pushActions(void* key);
  void popActions(void* key);
  void setStandardActions();
  void invalidateActions();

  // cursor control interface
  void setCursor(class wxCursor* pcurs =NULL);

  // screen shot interface
  Point2 getScreenShotSize() const;
  bool screenShot(const std::string& path);
  unsigned char* screenShot(const bool flip =false);

  // static methods
  static void  SetXformAnim(const bool mode, const float duration =-1.f);
  static bool  GetXformAnim();
  static float GetXformAnimDuration();
  static void  SetKeyZoomInRatio(const float r1 =-1.f, const float r2 =-1.f);
  static void  GetKeyZoomInRatio(float& r1, float& r2);
  static void  SetKeyZoomOutRatio(const float r1 =-1.f, const float r2 =-1.f);
  static void  GetKeyZoomOutRatio(float& r1, float& r2);
  static void  ReadConfig();
  static void  WriteConfig();

private:
  vfrDrawAreaWx*  m_pda; // Wx object, don't delete
  vsnViewFrame*   p_parent;
  class wxCursor* p_cursor;

  // scene graph objects
  vfrScreen*      m_pScreen;
  vfrCamera*      m_pCamera;
  vfrScene*       m_pRootScene;
  vfrGroup*       m_pTransNode;   // [T]
  vfrGroup*       m_pCenterNode;  // [C]
  vfrGroup*       m_pRotateNode;  // [R]
  vfrGroup*       m_pScaleNode;   // [S]
  vfrGroup*       m_pInvCentNode; // [-C]
  vfrGroup*       m_pRootNode;    // [Root]
  vfrLines*       m_pCentGlyph;
  vsnFrontAxis*   m_pFrAxis;

  // ActSet stack
  void*                        m_pCurActKey;
  std::deque<VSN::ActSetEntry> m_gfxActStack;

  // actions
  vsnGfxAct_KeyIn      gfxAct_KeyIn;
  vsnGfxAct_Click      gfxAct_Click;
  vsnGfxAct_RotScene   gfxAct_RotScene;
  vsnGfxAct_RollScene  gfxAct_RollScene;
  vsnGfxAct_ScaleScene gfxAct_ScaleScene;
  vsnGfxAct_TransScene gfxAct_TransScene;
  vsnGfxAct_Selection  gfxAct_Selection;
  vsnGfxAct_SweepZoom  gfxAct_SweepZoom;
  vsnGfxAct_StartRBox  gfxAct_StartRBoxSelect;
  vsnGfxAct_StartRBox  gfxAct_StartRBoxZoom;
  vsnGfxAct_Wheel      gfxAct_Wheel;

  // static members
  static bool  s_xformAnim;
  static float s_xformAnimDuration;
  static float s_keyZoomInRatio, s_keyZoomInRatio2;
  static float s_keyZoomOutRatio, s_keyZoomOutRatio2;
};

#endif // _VSN_GFX_VIEW_H_
