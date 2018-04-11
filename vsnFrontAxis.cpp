//
// vsnFrontAxis
//
#include "vsnFrontAxis.h"
#include <assert.h>

/* static members */

const size_t vsnFrontAxis::s_symW = 8;
const size_t vsnFrontAxis::s_symH = 8;
const GLubyte vsnFrontAxis::s_sym[3][8] = {
  {0x81, 0xc2, 0x64, 0x38, 0x1c, 0x26, 0x43, 0x81}, // X
  {0x18, 0x18, 0x18, 0x18, 0x1c, 0x26, 0x43, 0x81}, // Y
  {0xFF, 0x61, 0x30, 0x18, 0x0c, 0x06, 0x83, 0x7f}  // Z
};


/* constructors / destructor */

vsnFrontAxis::vsnFrontAxis(const std::string& name)
  : vsnFrontObj(name)
{
  // prepare axis arrows
  vfrVectors* paxs = new vfrVectors(); assert(paxs);
  vector3 wkv[3] = {{0.f,0.f,0.f}, {0.f,0.f,0.f}, {0.f,0.f,0.f}};
  paxs->alcVerts(3); paxs->alcNormals(3); paxs->alcColors(3);
  paxs->setVerts(3, wkv);
  wkv[0][0] = wkv[1][1] = wkv[2][2] = FR_AXIS_SIZE;
  paxs->setNormals(3, wkv);
  wkv[0][0] = wkv[1][1] = wkv[2][2] = 1.0f;
  paxs->setColors3(3, wkv);
  paxs->setColorMode(AT_PER_VERTEX);
  addChild(paxs);

  // set initial position to left-bottom
  trans(-(1.f - FR_AXIS_SIZE), -(1.f - FR_AXIS_SIZE), 0.f);
}

vsnFrontAxis::~vsnFrontAxis() {
  // don't delete paxs, ~vsnFrontObj will delete it.
}


/* methods from vsnFrontObj */

void vsnFrontAxis::render(const Bool transpMode, vfrMaterialStack *mstk) {
  GLint vp[4]; glGetIntegerv(GL_VIEWPORT, vp);
  float asp = 1.f;
  if ( vp[2] > 0 && vp[3] > 0 ) asp = (float)vp[2]/(float)vp[3];
  glPushMatrix();
  glTranslatef(_matrix.m_v[12]*(asp -1.f), 0.f, 0.f);
  
  vfrGroup::render(transpMode, mstk);
  
  // draw axis labels
  if ( getRenderMode() && transpMode == isTransparency() ) {
    glDisable(GL_LIGHTING);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glColor4fv(_colors[0]);
    glMultMatrixf(_matrix.m_v);
    glRasterPos3f(FR_AXIS_SIZE, 0.f, 0.f);
    glBitmap(s_symW, s_symH, 0.f, 0.f, 0.f, 0.f, s_sym[0]);
    glRasterPos3f(0.f, FR_AXIS_SIZE, 0.f);
    glBitmap(s_symW, s_symH, 0.f, 0.f, 0.f, 0.f, s_sym[1]);
    glRasterPos3f(0.f, 0.f, FR_AXIS_SIZE);
    glBitmap(s_symW, s_symH, 0.f, 0.f, 0.f, 0.f, s_sym[2]);
    glEnable(GL_LIGHTING);
  }

  glPopMatrix();
}

