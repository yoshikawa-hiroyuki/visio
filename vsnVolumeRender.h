//
// vsnVolumeRender
//
#ifndef _VSN_VOLUME_RENDER_H_
#define _VSN_VOLUME_RENDER_H_

#include "vfrNode.h"

namespace VSN {
  // VRen status
  enum {VRen_Nodata = 0,
	VRen_Inited  = (1<<0),
	VRen_HasData = (1<<1),
	VRen_HasLut  = (1<<2),
	VRen_Drawable= (VRen_HasData|VRen_HasLut)
  };
  typedef int VRenStatusType;
};


//----------------------------------------------------------------
// class vsnVolumeRender
//----------------------------------------------------------------
class vsnVolumeRender : public vfrNode {
public:
  vsnVolumeRender(const std::string& name =std::string(VFR_NONAME))
    : vfrNode(name, FALSE), m_status(VSN::VRen_Nodata) {
    setAlpha(TRUE);
    setColor4(0, 1.f, 1.f, 1.f, 1.f);
    setPickMode(PT_NONE);
    alcMaterial();
  }
  virtual ~vsnVolumeRender() {}

  // interface
  virtual void DrawVolume(const CES::Mat4<float>& mvm =CES::Mat4<float>()) =0;
  VSN::VRenStatusType getStatus() const {return m_status;}

  // override
  virtual void renderSolid() {}
  virtual void renderWire();

protected:
  VSN::VRenStatusType m_status;
};

/* inline methods */

inline void vsnVolumeRender::renderWire() {
  // display-list check
  if ( beginDispList(DLF_WIRE) ) return;

  if ( ! useAuxLineColor() )
    glColor4fv(_colors[0]);

  glColor3f(1.f, 1.f, 1.f);
  glBegin(GL_LINE_LOOP);
  glVertex3f(_bbox[1].m_v[0], _bbox[0].m_v[1], _bbox[0].m_v[2]);
  glVertex3f(_bbox[1].m_v[0], _bbox[1].m_v[1], _bbox[0].m_v[2]);
  glVertex3f(_bbox[1].m_v[0], _bbox[1].m_v[1], _bbox[1].m_v[2]);
  glVertex3f(_bbox[1].m_v[0], _bbox[0].m_v[1], _bbox[1].m_v[2]);
  glEnd();

  glBegin(GL_LINE_LOOP);
  glVertex3f(_bbox[0].m_v[0], _bbox[0].m_v[1], _bbox[0].m_v[2]);
  glVertex3f(_bbox[0].m_v[0], _bbox[0].m_v[1], _bbox[1].m_v[2]);
  glVertex3f(_bbox[0].m_v[0], _bbox[1].m_v[1], _bbox[1].m_v[2]);
  glVertex3f(_bbox[0].m_v[0], _bbox[1].m_v[1], _bbox[0].m_v[2]);
  glEnd();

  glBegin(GL_LINES);
  glVertex3f(_bbox[1].m_v[0], _bbox[0].m_v[1], _bbox[0].m_v[2]);
  glVertex3f(_bbox[0].m_v[0], _bbox[0].m_v[1], _bbox[0].m_v[2]);
  glVertex3f(_bbox[1].m_v[0], _bbox[1].m_v[1], _bbox[0].m_v[2]);
  glVertex3f(_bbox[0].m_v[0], _bbox[1].m_v[1], _bbox[0].m_v[2]);
  glVertex3f(_bbox[0].m_v[0], _bbox[1].m_v[1], _bbox[1].m_v[2]);
  glVertex3f(_bbox[1].m_v[0], _bbox[1].m_v[1], _bbox[1].m_v[2]);
  glVertex3f(_bbox[0].m_v[0], _bbox[0].m_v[1], _bbox[1].m_v[2]);
  glVertex3f(_bbox[1].m_v[0], _bbox[0].m_v[1], _bbox[1].m_v[2]);
  glEnd();

  // end display-list definition
  endDispList(DLF_WIRE);
}

#endif // _VSN_VOLUME_RENDER_H_
