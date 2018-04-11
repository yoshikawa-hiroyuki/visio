//
// vsnGfxActions
//
#ifndef _VSN_GFX_ACTIONS_H_
#define _VSN_GFX_ACTIONS_H_

#include "vfrDefaultActions.h"
#include "vfrKeyCode.h"

class vsnGfxView;

namespace VSN {
  enum {ActKeyIn =0,
	ActClick,   ActDragStart,   ActDragEnd,   ActDrag,
	ActSClick,  ActSDragStart,  ActSDragEnd,  ActSDrag,
	ActCClick,  ActCDragStart,  ActCDragEnd,  ActCDrag,
	ActSCClick, ActSCDragStart, ActSCDragEnd, ActSCDrag,
	ActRClick,  ActRDragStart,  ActRDragEnd,  ActRDrag,
	ActRSClick, ActRSDragStart, ActRSDragEnd, ActRSDrag,
	ActRCClick, ActRCDragStart, ActRCDragEnd, ActRCDrag,
	ActRSCClick,ActRSCDragStart,ActRSCDragEnd,ActRSCDrag,
	ActMClick,  ActMDragStart,  ActMDragEnd,  ActMDrag,
	ActMSClick, ActMSDragStart, ActMSDragEnd, ActMSDrag,
	ActMCClick, ActMCDragStart, ActMCDragEnd, ActMCDrag,
	ActMSCClick,ActMSCDragStart,ActMSCDragEnd,ActMSCDrag,
	ActNUM};

  enum {OprRotate =0, OprRoll, OprScale, OprTranslate, OprSelect,
	OprSweepZoom, OprNUM};
  enum {MBLeft =0, MBMiddle, MBRight};
  typedef int MBType;
  enum {ModKeyNone =0, ModKeyCtrl =1, ModKeyShift =(1<<1)};
  typedef int ModKeyType;
  enum {EvtClick =0, EvtDragStart, EvtDrag, EvtDragEnd};
  typedef int EvtType;
};


//----------------------------------------------------------------
// struct vsnGfxActSet
//----------------------------------------------------------------
struct vsnGfxActSet {
  vfrAction *actions[VSN::ActNUM];
  class wxCursor *pcursor;

  vsnGfxActSet();
  vsnGfxActSet(const vsnGfxActSet& org) {*this = org;}
  void operator=(const vsnGfxActSet& org);
};


//----------------------------------------------------------------
// class vsnGfxBaseAct
//----------------------------------------------------------------
class vsnGfxBaseAct {
public:
  vsnGfxBaseAct(vsnGfxView* pgv =NULL) : p_gfxView(pgv) {}
  vsnGfxView* getGfxView() {return p_gfxView;}
  void setGfxView(vsnGfxView* pgv) {p_gfxView = pgv;}

protected:
  vsnGfxView* p_gfxView;
};


//----------------------------------------------------------------
// class vsnGfxAct_KeyIn
//----------------------------------------------------------------
class vsnGfxAct_KeyIn : public vfrAction, public vsnGfxBaseAct {
public:
  vsnGfxAct_KeyIn(vsnGfxView* pgv =NULL) : vsnGfxBaseAct(pgv) {}
  void execute(vfrEvent& e);
};


//----------------------------------------------------------------
// class vsnGfxAct_Click
//----------------------------------------------------------------
class vsnGfxAct_Click : public vfrAction, public vsnGfxBaseAct {
public:
  vsnGfxAct_Click(vsnGfxView* pgv =NULL) : vsnGfxBaseAct(pgv) {}
  void execute(vfrEvent& e);
};


//----------------------------------------------------------------
// class vsnGfxAct_RotScene
//----------------------------------------------------------------
class vsnGfxAct_RotScene : public vfrAction, public vsnGfxBaseAct {
public:
  vsnGfxAct_RotScene(vsnGfxView* pgv =NULL)
    : vsnGfxBaseAct(pgv), m_ok(false) {}
  void execute(vfrEvent& e);
private:
  Point2           m_mp0;
  vfrMatrix        m_M0;
  CES::Vec3<float> m_rAx, m_rAy, m_rAz;
  volatile mutable bool m_ok;
};


//----------------------------------------------------------------
// class vsnGfxAct_RollScene
//----------------------------------------------------------------
class vsnGfxAct_RollScene : public vfrAction, public vsnGfxBaseAct {
public:
  vsnGfxAct_RollScene(vsnGfxView* pgv =NULL)
    : vsnGfxBaseAct(pgv), m_ok(false) {}
  void execute(vfrEvent& e);
private:
  Point2           m_mp0;
  vfrMatrix        m_M0;
  CES::Vec3<float> m_rAx, m_rAy, m_rAz;
  volatile mutable bool m_ok;
};


//----------------------------------------------------------------
// class vsnGfxAct_ScaleScene
//----------------------------------------------------------------
class vsnGfxAct_ScaleScene : public vfrAction, public vsnGfxBaseAct {
public:
  vsnGfxAct_ScaleScene(vsnGfxView* pgv =NULL) : vsnGfxBaseAct(pgv) {}
  void execute(vfrEvent& e);
};


//----------------------------------------------------------------
// class vsnGfxAct_TransScene
//----------------------------------------------------------------
class vsnGfxAct_TransScene : public vfrAction, public vsnGfxBaseAct {
public:
  vsnGfxAct_TransScene(vsnGfxView* pgv =NULL) : vsnGfxBaseAct(pgv) {}
  void execute(vfrEvent& e);
};


//----------------------------------------------------------------
// class vsnGfxAct_Selection
//----------------------------------------------------------------
class vsnGfxAct_Selection : public vfrAction, public vsnGfxBaseAct {
public:
  vsnGfxAct_Selection(vsnGfxView* pgv =NULL) : vsnGfxBaseAct(pgv) {}
  void execute(vfrEvent& e);
};


//----------------------------------------------------------------
// class vsnGfxAct_StartRBox
//----------------------------------------------------------------
class vsnGfxAct_StartRBox : public vfrAction, public vsnGfxBaseAct {
public:
  vsnGfxAct_StartRBox(vsnGfxView* pgv =NULL, const float lw = 2.f,
		      const VFR::StippleType lt = VFR::ST_SOLID,
		      const CES::Vec3<float> color
		      = CES::Vec3<float>(.8f,.8f,.8f))
    : vsnGfxBaseAct(pgv), m_lw(lw), m_lt(lt) {
    m_color[0] = color[0]; m_color[1] = color[1]; m_color[2] = color[2];
    m_color[3] = 1.f;
  }
  void execute(vfrEvent& e);
  //private:
  float m_lw;
  VFR::StippleType m_lt;
  vector4 m_color;
};


//----------------------------------------------------------------
// class vsnGfxAct_SweepZoom
//----------------------------------------------------------------
class vsnGfxAct_SweepZoom : public vfrAction, public vsnGfxBaseAct {
public:
  vsnGfxAct_SweepZoom(vsnGfxView* pgv =NULL) : vsnGfxBaseAct(pgv) {}
  void execute(vfrEvent& e);
};


//----------------------------------------------------------------
// class vsnGfxAct_Wheel
//----------------------------------------------------------------
class vsnGfxAct_Wheel : public vfrAction, public vsnGfxBaseAct {
public:
  vsnGfxAct_Wheel(vsnGfxView* pgv =NULL) : vsnGfxBaseAct(pgv) {}
  void execute(vfrEvent& e);
};


//----------------------------------------------------------------
// struct vsnGfxOperation
//----------------------------------------------------------------
struct vsnGfxOperation {
  VSN::MBType m_mouseButton;
  VSN::ModKeyType  m_modKey;

  vsnGfxOperation(const VSN::MBType btn =VSN::MBLeft,
		  const VSN::ModKeyType mkey =VSN::ModKeyNone)
    : m_mouseButton(btn), m_modKey(mkey) {}
  vsnGfxOperation(const vsnGfxOperation& org) {*this = org;}

  void operator=(const vsnGfxOperation& org) {
    m_mouseButton = org.m_mouseButton; m_modKey = org.m_modKey;
  }
  bool operator==(const vsnGfxOperation& org) const {
    return (m_mouseButton == org.m_mouseButton && m_modKey == org.m_modKey);
  }

  vfrEvent& getEvent(vfrDispatch& d, const VSN::EvtType et) const;
};

//----------------------------------------------------------------
// struct vsnGfxOprOrientation
//----------------------------------------------------------------
struct vsnGfxOprOrientation {
  vsnGfxOperation m_oprLst[VSN::OprNUM];

  vsnGfxOprOrientation(const bool stdOri =true) {
    if ( stdOri ) setupStandardOrientation();
  }
  vsnGfxOprOrientation(const vsnGfxOprOrientation& org) {*this = org;}
  void operator=(const vsnGfxOprOrientation& org);
  void setupStandardOrientation();
  bool isValid() const;

  // configuration
  void readConfig();
  void writeConfig() const;
};

#endif // _VSN_GFX_ACTIONS_H_
