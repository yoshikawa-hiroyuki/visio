//
// vsnFrontAxis
//
#ifndef _VSN_FRONT_AXIS_H_
#define _VSN_FRONT_AXIS_H_

#include "vsnFrontObj.h"
#include "vfrVectors.h"

#define FR_AXIS_SIZE  0.1f


class vsnFrontAxis : public vsnFrontObj {
public:
  vsnFrontAxis(const std::string& name =std::string(VFR_NONAME));
  virtual ~vsnFrontAxis();

  // set rotation matrix
  void setRotMatrix(const vfrMatrix& rm) {
    vfrMatrix M(rm);
    M.m_v[12] = -(1.f - FR_AXIS_SIZE);
    M.m_v[13] = -(1.f - FR_AXIS_SIZE);
    M.m_v[14] = 0.f;
    setMatrix(M);
  }

  // override vsnFrontObj::render method
  virtual void render(const Bool transpMode, vfrMaterialStack *mstk =NULL);

protected:
  static const size_t s_symW, s_symH;
  static const GLubyte s_sym[3][8];
};

#endif // _VSN_FRONT_AXIS_H_
