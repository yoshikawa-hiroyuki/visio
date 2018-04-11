//
// vsnRvrVolumeRender
//
#ifndef _VSN_RVR_VOLUME_RENDER_H_
#define _VSN_RVR_VOLUME_RENDER_H_

#include <string>
#include "rvrVolumeRenderer.h"
#include "vsnVolumeRender.h"
#include "vsnLut.h"


// utils from libtt-cgvo
namespace VSN {
  template <class T> inline
  T linearITP(float u, const T& c0, const T& c1) {
    return (1 - u) * c0 + u * c1;
  }

  template <class T> inline
  T bilinearITP(float u, float v,
                const T& c0, const T& c1, const T& c2, const T& c3) {
    return ((1.f-u) * c0 + u * c1) * (1.f-v) + ((1.f-u) * c2 + u * c3) * v;
  }

  template <class T> inline
  T trilinearITP(float u, float v, float w, const T c[8]) {
    T a = VSN::bilinearITP(u, v, c[0], c[1], c[2], c[3]);
    T b = VSN::bilinearITP(u, v, c[4], c[5], c[6], c[7]);
    return VSN::linearITP(w, a, b);
  }

  template<class T>
  bool resample(const CES::Vec3<size_t>& orgDim,
                const CES::Vec3<size_t>& dim,
                const T *orgData, T *data, const bool itp =true);
};


//----------------------------------------------------------------
// class vsnRvrVolumeRender
//----------------------------------------------------------------
class vsnRvrVolumeRender : public vsnVolumeRender {
public:
  vsnRvrVolumeRender(const std::string& name =std::string(VFR_NONAME));
  virtual ~vsnRvrVolumeRender();

  bool Initialize();
  bool SetData(const size_t dims[3], const size_t dlen, const size_t tgt,
	       const float* pdata, const float drange[2],
	       const unsigned char* pmask =NULL);
  bool SetData(class vsnOctTree* poct, const size_t tgt,
	       const CES::Vec3<int>& didx, const float drange[2],
	       const bool doInterp =false);

  bool SetGeom(const float p0[3], const float p1[3]);
  bool SetLut(const vsnLut& lut);
  vsnLut GetLut() const;
  bool SetDerivLut(const vsnLut& lut);
  vsnLut GetDerivLut() const;

  bool SetSliceNum(const size_t nsl);
  size_t GetSliceNum() const {return m_nslices;}

  void SetShadingMode(const bool mode);
  bool GetShadingMode() const {return m_shading;}

  void SetRegion(const CES::Vec3<float> cp[2]);
  void GetRegion(CES::Vec3<float> cp[2]) const;

  // from vsnVolumeRender
  virtual void DrawVolume(const CES::Mat4<float>& mvm =CES::Mat4<float>());

protected:
  void updateLut();
  void updateGeom();
  void updateData();

  rvrVolumeRenderer* m_render;
  unsigned char*     m_data;
  CES::Vec3<size_t>  m_dims;

  vsnLut             m_lut;
  vsnLut             m_lut2;

  size_t             m_nslices;
  bool               m_shading;

  // utils
  CES::Vec3<size_t> GetMax3DTexSize(const CES::Vec3<size_t>& startSz);

  // static utils
  static bool ConvLut(const vsnLut& lut, rvrLUT& rlut);

  volatile bool      m_data_updated;
};

#endif // _VSN_RVR_VOLUME_RENDER_H_
