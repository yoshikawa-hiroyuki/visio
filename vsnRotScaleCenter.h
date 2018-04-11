//
// vsnRotScaleCenter
//
#ifndef _VSN_ROT_SCALE_CENTER_H_
#define _VSN_ROT_SCALE_CENTER_H_

#include "vsnGfxActions.h"


//----------------------------------------------------------------
// class vsnGfxAct_KeyEscCenter
//----------------------------------------------------------------
class vsnGfxAct_KeyEscCenter : public vfrAction, public vsnGfxBaseAct {
public:
  vsnGfxAct_KeyEscCenter(vsnGfxView* pgv =NULL) : vsnGfxBaseAct(pgv) {}
  void execute(vfrEvent& e);
};


//----------------------------------------------------------------
// class vsnGfxAct_ClickCenter
//----------------------------------------------------------------
class vsnGfxAct_ClickCenter : public vfrAction, public vsnGfxBaseAct {
public:
  vsnGfxAct_ClickCenter(vsnGfxView* pgv =NULL) : vsnGfxBaseAct(pgv) {}
  void execute(vfrEvent& e);
};


//----------------------------------------------------------------
// class vsnGfxAct_DragTransCenter
//----------------------------------------------------------------
class vsnGfxAct_DragTransCenter : public vfrAction, public vsnGfxBaseAct {
public:
  vsnGfxAct_DragTransCenter(vsnGfxView* pgv =NULL) : vsnGfxBaseAct(pgv) {}
  void execute(vfrEvent& e);
};


//----------------------------------------------------------------
// class vsnGfxAct_DragDollyCenter
//----------------------------------------------------------------
class vsnGfxAct_DragDollyCenter : public vfrAction, public vsnGfxBaseAct {
public:
  vsnGfxAct_DragDollyCenter(vsnGfxView* pgv =NULL) : vsnGfxBaseAct(pgv) {}
  void execute(vfrEvent& e);
};


//----------------------------------------------------------------
// class vsnGfxAct_DragEndCenter
//----------------------------------------------------------------
class vsnGfxAct_DragEndCenter : public vfrAction, public vsnGfxBaseAct {
public:
  vsnGfxAct_DragEndCenter(vsnGfxView* pgv =NULL) : vsnGfxBaseAct(pgv) {}
  void execute(vfrEvent& e);
};

#endif // _VSN_ROT_SCALE_CENTER_H_
