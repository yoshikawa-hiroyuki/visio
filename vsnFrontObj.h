//
// vsnFrontObj
//
#ifndef _VSN_FRONT_OBJ_H_
#define _VSN_FRONT_OBJ_H_

#include "vfrGroup.h"


//----------------------------------------------------------------
// class vsnFrontObj
//----------------------------------------------------------------
class vsnFrontObj : public vfrGroup {
public:
  vsnFrontObj(const std::string& name =std::string(VFR_NONAME))
    : vfrGroup(name, FALSE) {
    alcMaterial();
  }

  virtual ~vsnFrontObj() {
    register int n = getNumChildren();
    while ( n > 0 ) {
      vfrNode* p = getChild(n - 1);
      if ( p ) delete p;
      n = getNumChildren();
    }
  }

  // position interface : valid area is (-1,-1)...(1,1)
  void setPosition(const vector2 p) {
    identity(); trans(p[0], p[1], 0.f);
    chkNotice();
  }
  void getPosition(vector2 p) const {
    p[0] = _matrix[12]; p[1] = _matrix[13];
  }

  // override vfrGroup::render method
  virtual void render(const Bool transpMode, vfrMaterialStack *mstk =NULL) {
    GLint vp[4]; glGetIntegerv(GL_VIEWPORT, vp);
    float asp = 1.f;
    if ( vp[2] > 0 && vp[3] > 0 ) asp = (float)vp[2]/(float)vp[3];
    glPushMatrix();
    glTranslatef(_matrix.m_v[12]*(asp -1.f), 0.f, 0.f);
    vfrGroup::render(transpMode, mstk);
    glPopMatrix();
  }

  // override to publicate
  virtual void generateBbox() {vfrGroup::generateBbox();}
};

#endif // _VSN_FRONT_OBJ_H_
