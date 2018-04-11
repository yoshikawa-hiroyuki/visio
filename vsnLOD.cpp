//
// vsnLOD
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

#include "vsnLOD.h"
#include "vsnError.h"

using namespace std;


//----------------------------------------------------------------
// class vsnLOD
//----------------------------------------------------------------

float vsnLOD::s_areaEps = 1e-4f;

vsnLOD::vsnLOD(const string& name, const Bool ssm)
  : vfrGroup(name, ssm)
{
}

vsnLOD::~vsnLOD() {
}


bool vsnLOD::setAreaList(const AreaListType& al) {
  m_areaLst = al;
  notice();
  return true;
}

bool vsnLOD::addArea(const float d) {
  if ( d <= s_areaEps ) return false;
  if ( ! m_areaLst.insert(d).second ) return false;
  notice();
  return true;
}

/* MxV_w: Returns (M * V).
ただし、返す前にVの成分をwで除算する。
これにより、「クリップ座標系」(wで除算する前)ではなく
「正規化デバイス座標系」での値が返る。
 */
static inline CES::Vec3<float>
MxV_w(const CES::Mat4<float>& M, const CES::Vec3<float>& V) {
  CES::Vec3<float> w;
  register size_t i, k;
  for ( i = 0; i < 3; i++ ) {
    w[i] = 0.f;
    for ( k = 0; k < 3; k++)
      w[i] += (M[i + k*4] * V[k]);
    w[i] += M[i + k*4];
  }
  float vw = M[3]*V[0] + M[7]*V[1] + M[11]*V[2] + M[15];
  if ( vw != 0.f )
    w = w * (1.f / vw);
  return w;
}

int vsnLOD::getCurrentLevel() const {
  GLenum glerr;
  GLint param;
  glGetIntegerv(GL_RENDER_MODE, &param);
  glerr = glGetError();
  if ( glerr != GL_NO_ERROR ) {
    while ( glGetError() != GL_NO_ERROR ); // clear errors
    return -1;
  }
  if ( param != GL_RENDER ) return 0;

  CES::Mat4<float> PM, MVM, M;
  //glGetFloatv(GL_PROJECTION_MATRIX, PM.m_v);
  glGetFloatv(GL_MODELVIEW_MATRIX, MVM.m_v);
  glerr = glGetError();
  if ( glerr != GL_NO_ERROR ) {
    while ( glGetError() != GL_NO_ERROR ); // clear errors
    return -1;
  }

  M = PM * MVM;
  CES::Vec3<float> po, pp[8];
  po = (_bbox[0] + _bbox[1]) * 0.5f;
  pp[0][0]=_bbox[1][0]; pp[0][1]=_bbox[0][1]; pp[0][2]=_bbox[0][2];
  pp[1][0]=_bbox[1][0]; pp[1][1]=_bbox[1][1]; pp[1][2]=_bbox[0][2];
  pp[2][0]=_bbox[1][0]; pp[2][1]=_bbox[1][1]; pp[2][2]=_bbox[1][2];
  pp[3][0]=_bbox[1][0]; pp[3][1]=_bbox[0][1]; pp[3][2]=_bbox[1][2];
  pp[4][0]=_bbox[0][0]; pp[4][1]=_bbox[0][1]; pp[4][2]=_bbox[0][2];
  pp[5][0]=_bbox[0][0]; pp[5][1]=_bbox[0][1]; pp[5][2]=_bbox[1][2];
  pp[6][0]=_bbox[0][0]; pp[6][1]=_bbox[1][1]; pp[6][2]=_bbox[1][2];
  pp[7][0]=_bbox[0][0]; pp[7][1]=_bbox[1][1]; pp[7][2]=_bbox[0][2];
  po = MxV_w(M, po); po[2] = 0.f;
  register int i;
  for ( i = 0; i < 8; i++ ) {
    pp[i] = MxV_w(M, pp[i]);
  } // end of for(i)

  float R = 0.0f;
  for ( i = 0; i < 8; i++ ) {
    pp[i][2] = 0.f;
    float wr = (pp[i] - po).Length();
    if ( wr > R ) R = wr;
  } // end of for(i)

  float s = M_PI * R * R;
  float S = s / (4.f * 6.f * 6.f); // 6 == halfH, halfH
  ////printf("s=%f, S=%f\n", s, S);

  int curL;
  set<float, greater<float> >::const_iterator it;
  for ( it = m_areaLst.begin(), curL = 0;
	it != m_areaLst.end(); it++, curL++ ) {
    if ( S > *it ) break;
  } // end of for(it, curL)

  return curL;
}

void vsnLOD::callPrimRender(const RenderType rt, vfrNode* pn) {
  if ( ! pn ) return;
  switch ( rt ) {
  case RT_SMOOTH: case RT_NOLIGHT: case RT_FLAT:
    pn->renderSolid();
    break;
  case RT_WIRE:
    pn->renderWire();
    break;
  case RT_POINT:
    pn->renderPoint();
    break;
  }
}


void vsnLOD::render(const Bool transpMode, vfrMaterialStack *mstk) {
  if ( transpMode ) {
    if ( ! isTransparency() )
      return;
  } else {
    if ( isTransparency() )
      return;
  }

  int level = getCurrentLevel();
  vfrNode* c = getChild(level);
  if ( ! c ) return;

  // based on vfrGroup::render
  if ( mstk ) {
    if ( _material )
      mstk->push(_material);
  }

  RenderType renderMode = getRenderMode();
  if ( renderMode == RT_NONE ) {
    if ( mstk && _material )
      mstk->pop();
    return;
  }

  if ( _pickable & PT_OBJECT )
    glPushName((GLuint)_currentSeq);

  // Apply local coodinate
  applyMatrix();

  // Apply material
  const vfrMaterial* mp = getMaterial();
  if ( mp )
    mp->apply();

  // Rendering Children
  vector4 auxc;
  if ( renderMode & (RT_SMOOTH | RT_NOLIGHT | RT_FLAT) ) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    if ( renderMode & RT_NOLIGHT )
      glDisable(GL_LIGHTING);
    if ( _texture && !(renderMode & RT_NOTEXTURE) )
      _texture->apply();
    if ( _colorMode == AT_WHOLE )
      glColor4fv(_colors[0]);
    
    callPrimRender(RT_SMOOTH, c);
    
    if ( _texture && !(renderMode & RT_NOTEXTURE) )
      _texture->unApply();
    if ( renderMode & RT_NOLIGHT )
      glEnable(GL_LIGHTING);
  }
  if ( renderMode & RT_WIRE ) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDisable(GL_LIGHTING);
    if ( useAuxLineColor(auxc) )
      glColor4fv(auxc);
    else if ( _colorMode == AT_WHOLE )
      glColor4fv(_colors[0]);
    
    callPrimRender(RT_WIRE, c);
    
    glEnable(GL_LIGHTING);
  }
  if ( renderMode & RT_POINT ) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    glDisable(GL_LIGHTING);
    if ( useAuxPointColor(auxc) )
      glColor4fv(auxc);
    else if ( _colorMode == AT_WHOLE )
      glColor4fv(_colors[0]);
    
    callPrimRender(RT_POINT, c);
    
    glEnable(GL_LIGHTING);
  }

  // Un-Apply material
  if ( mp )
    mp->unApply();

  // Draw Bounding Box
  if ( _pickable & PT_BBOX || !_pickable )
    ;
  else
    glLoadName((GLuint)0);
  drawBbox();

  if ( _pickable & PT_OBJECT )
    glPopName();
  unApplyMatrix();

  if ( mstk && _material )
    mstk->pop();
}

void vsnLOD::renderSolid() {
  // based on vfrGroup::renderSolid
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  // call renderSolid() of children
  int level = getCurrentLevel();
  vfrNode* c = getChild(level);
  if ( c ) {
    c->applyMatrix();
    vfrMaterial* pm = c->getPrivateMaterial();
    if ( pm ) pm->apply();
    RenderType renderMode = c->getRenderMode();
    if ( renderMode & RT_NOLIGHT ) glDisable(GL_LIGHTING);
    vfrTexture* ptex = c->getTexture();
    if ( ptex && !(renderMode & RT_NOTEXTURE) ) ptex->apply();

    callPrimRender(RT_SMOOTH, c);

    if ( ptex && !(renderMode & RT_NOTEXTURE) ) ptex->unApply();
    if ( renderMode & RT_NOLIGHT ) glEnable(GL_LIGHTING);
    if ( pm ) pm->unApply();
    c->unApplyMatrix();
  }
}

void vsnLOD::renderWire() {
  // based on vfrGroup::renderWire
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glDisable(GL_LIGHTING);
  vector4 auxc;

  // call renderWire() of children
  int level = getCurrentLevel();
  vfrNode* c = getChild(level);
  if ( c ) {
    c->applyMatrix();
    vfrMaterial* pm = c->getPrivateMaterial();
    if ( pm ) pm->apply();
    if ( c->useAuxLineColor(auxc) ) glColor4fv(auxc);

    callPrimRender(RT_WIRE, c);

    if ( pm ) pm->unApply();
    c->unApplyMatrix();
  }
  glEnable(GL_LIGHTING);
}

void vsnLOD::renderPoint() {
  // based on vfrGroup::renderPoint
  glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
  glDisable(GL_LIGHTING);
  vector4 auxc;

  // call renderPoint() of children
  int level = getCurrentLevel();
  vfrNode* c = getChild(level);
  if ( c ) {
    c->applyMatrix();
    vfrMaterial* pm = c->getPrivateMaterial();
    if ( pm ) pm->apply();
    if ( c->useAuxPointColor(auxc) ) glColor4fv(auxc);

    callPrimRender(RT_POINT, c);

    if ( pm ) pm->unApply();
    c->unApplyMatrix();
  }
  glEnable(GL_LIGHTING);
}

