//
// vsnNvrVolumeRender
//
#ifndef _VSN_NVR_VOLUME_RENDER_H_
#define _VSN_NVR_VOLUME_RENDER_H_

#include "vsnVolumeRender.h"
#include "nvrRender.h"
#include "vsnLut.h"


//----------------------------------------------------------------
// class vsnNvrVolumeRender
//----------------------------------------------------------------
class vsnNvrVolumeRender : public vsnVolumeRender {
public:
  vsnNvrVolumeRender(const std::string& name =std::string(VFR_NONAME));
  virtual ~vsnNvrVolumeRender();

  bool Initialize(CES::Vec3<size_t>& dims, const bool needAlc =true);
  bool SetVolume(const float* pd, const float minmax[2],
		 const CES::Vec3<float>* pbb =NULL,
		 const unsigned char* pmask =NULL);
  bool SetVolume(const std::deque<unsigned char*>& pdl,
		 const CES::Vec3<float>* pbb =NULL);
  bool SetLut(const vsnLut& lut);
  bool getOglChkd() const {return m_oglChkd;}
  bool setReduceMode(const bool rdm);
  bool getReduceMode() const {return m_doReduce;}
  nvrRender& getRender() {return m_render;}

  // from vsnVolumeRender
  virtual void DrawVolume(const CES::Mat4<float>& mvm =CES::Mat4<float>());

protected:
  bool                m_doReduce;
  NVR::Dim3           m_origDims;
  NVR::Dim3           m_wrapDims;
  CES::Vec3<float>    m_origBbox[2];
  nvrRender           m_render;
  unsigned char*      m_pData;
  bool                m_oglChkd;
};

#endif // _VSN_NVR_VOLUME_RENDER_H_
