//
// vsnRvrVolumeRender
//
#include "vsnRvrVolumeRender.h"
#include "vsnApp.h"
#include "vsnOctTree.h"
#include "vsnDataObj.h"
#include <cstdio>
#include <cstring>


// utils
namespace VSN {
  template<class T>
  bool resample(const CES::Vec3<size_t>& orgDim,
                const CES::Vec3<size_t>& dim,
                const T *orgData, T *data, const bool itp) {
    if ( ! orgData || ! data ) return false;
    if ( orgDim[0] < 2 || orgDim[1] < 2 || orgDim[2] < 2 ||
         dim[0] < 2 || dim[0] < 2 || dim[0] < 2 ) return false;

    CES::Vec3<size_t> orgDimX = orgDim - CES::Vec3<size_t>(1,1,1);
    CES::Vec3<size_t> dimX = dim - CES::Vec3<size_t>(1,1,1);

    register size_t i, x, y, z, sx, sy, sz, s0 = orgDim[0]*orgDim[1];
    register float fx, fy, fz, u, v, w;
    float sval[8];
    for ( i = 0, z = 0; z < dim[2]; z++ ) {
      fz = (float)orgDimX[2] * z / dimX[2];
      sz = (size_t)fz;
      w = fz - sz;
      for ( y = 0; y < dim[1]; y++ ) {
        fy = (float)orgDimX[1] * y / dimX[1];
        sy = (size_t)fy;
        v = fy - sy;
        for ( x = 0; x < dim[0]; x++ ) {
          fx = (float)orgDimX[0] * x / dimX[0];
          sx = (size_t)fx;
          u = fx - sx;
          if ( itp && sx < orgDimX[0] && sy < orgDimX[1] && sz < orgDimX[2] ) {
            sval[0] = orgData[s0*sz + orgDim[0]*sy + sx];
            sval[1] = orgData[s0*sz + orgDim[0]*sy + sx+1];
            sval[2] = orgData[s0*sz + orgDim[0]*(sy+1) + sx];
            sval[3] = orgData[s0*sz + orgDim[0]*(sy+1) + sx+1];
            sval[4] = orgData[s0*(sz+1) + orgDim[0]*sy + sx];
            sval[5] = orgData[s0*(sz+1) + orgDim[0]*sy + sx+1];
            sval[6] = orgData[s0*(sz+1) + orgDim[0]*(sy+1) + sx];
            sval[7] = orgData[s0*(sz+1) + orgDim[0]*(sy+1) + sx+1];
            data[i] = (T)VSN::trilinearITP(u, v, w, sval);
          }
          else {
            register int ssx, ssy, ssz;
            ssx = (u>0.5f) ? sx+1 : sx;
            ssy = (v>0.5f) ? sy+1 : sy;
            ssz = (w>0.5f) ? sz+1 : sz;
            data[i] = (T)orgData[s0*ssz + orgDim[0]*ssy + ssx];
          }
          i++;
        } // end of for(x)
      } // end of for(y)
    } // end of for(z)

    return true;
  }
};


//----------------------------------------------------------------
// class vsnRvrVolumeRender
//----------------------------------------------------------------

#define VSN_RVR_NUMSLICES_INIT 256
#define VSN_RVR_NUMSLICES_OMIT 40

// utils
#define _MIN_SAMPLE_SZ 64
CES::Vec3<size_t>
vsnRvrVolumeRender::GetMax3DTexSize(const CES::Vec3<size_t>& startSz) {
  CES::Vec3<size_t> rDims;
  register int i;

  if ( ! m_render ) return rDims;

  GLint txw = 0, maxTex3DSz = 0;
  glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &maxTex3DSz);
  if ( maxTex3DSz < 1 ) maxTex3DSz = 512;
  for ( i = 0; i < 3; i++ ) {
    for ( rDims[i] = maxTex3DSz; rDims[i] > 1; rDims[i] /= 2 )
      if ( rDims[i] <= startSz[i] ) break;
  } // end of for(i)
  for ( i = 0; i < 3; i++ ) {
    if ( rDims[i] < _MIN_SAMPLE_SZ ) rDims[i] = _MIN_SAMPLE_SZ;
  } // end of for(i)

  /////////////
  //rDims[0] /= 2; rDims[1] /= 2; rDims[2] /= 2; // no need grad tex
  return rDims;
  /////////////

  glTexImage3D(GL_PROXY_TEXTURE_3D, 0, GL_LUMINANCE,
	       rDims[0], rDims[1], rDims[2], 0, GL_LUMINANCE,
	       GL_UNSIGNED_BYTE, NULL);
  glGetTexLevelParameteriv(GL_PROXY_TEXTURE_3D, 0, GL_TEXTURE_WIDTH, &txw);
  while ( ! txw ) {
    if ( rDims[0] > rDims[1] ) i = (rDims[0] > rDims[2]) ? 0 : 2;
    else i = (rDims[1] > rDims[2]) ? 1 : 2;
    rDims[i] /= 2;
    if ( rDims[i] < 1 ) break;
    glTexImage3D(GL_PROXY_TEXTURE_3D, 0, GL_RGBA,
		 rDims[0], rDims[1], rDims[2], 0, GL_RGBA, GL_FLOAT, NULL);
    glGetTexLevelParameteriv(GL_PROXY_TEXTURE_3D, 0, GL_TEXTURE_WIDTH, &txw);
  } // end of while(!txw)

  if ( rDims[0] > rDims[1] ) i = (rDims[1] > rDims[2]) ? 2 : 1;
  else i = (rDims[0] > rDims[2]) ? 2 : 0;
  rDims[(i+1)%3] /= 2;
  rDims[(i+2)%3] /= 2;

  return rDims;
}

// STATIC
bool vsnRvrVolumeRender::ConvLut(const vsnLut& lut, rvrLUT& rlut) {
  vsnLut xlut = lut;
  if ( ! xlut.normalize() ) return false;
  for ( int i = 0; i < VSN::LUT_MAX_ENTRY; i++ ) {
    rlut.m_rgba[i][0] = (unsigned char)(xlut.lutEntry[i*4   ] * 255);
    rlut.m_rgba[i][1] = (unsigned char)(xlut.lutEntry[i*4 +1] * 255);
    rlut.m_rgba[i][2] = (unsigned char)(xlut.lutEntry[i*4 +2] * 255);
    rlut.m_rgba[i][3] = (unsigned char)(xlut.lutEntry[i*4 +3] * 255);
  } // end of for(i)
  return true;
}


/* constructors / destructor */

vsnRvrVolumeRender::vsnRvrVolumeRender(const std::string& name)
  : vsnVolumeRender(name), m_render(NULL), m_data(NULL),
    m_nslices(VSN_RVR_NUMSLICES_INIT), m_shading(true),
    m_data_updated(false)
{
  // set params of base class
  m_status = VSN::VRen_Drawable;

  // setup initial LUTs
  vsnLut lut;
  SetLut(lut);

  for ( int i = 0; i < VSN::LUT_MAX_ENTRY; i++ )
    lut.lutEntry[i*4+3] = 1.f;
  SetDerivLut(lut);
}

vsnRvrVolumeRender::~vsnRvrVolumeRender() {
  if ( m_render ) {
    delete m_render;
    m_render = NULL;
  }
  if ( m_data ) {
    delete [] m_data;
    m_data = NULL;
  }
}


/* methods */

bool vsnRvrVolumeRender::Initialize() {
  if ( m_render ) {
    delete m_render;
    m_render = NULL;
  }
  m_render = new rvrVolumeRenderer(0.01, 2000.01);
  if ( ! m_render ) {
    return false;
  }
  return true;
}


bool vsnRvrVolumeRender::SetData(const size_t dims[3],
				 const size_t dlen, const size_t tgt,
				 const float* pdata, const float drange[2],
				 const unsigned char* pmask) {
  if ( ! m_render ) return false;
  size_t dSz = dims[0]*dims[1]*dims[2];
  if ( dSz < 1 ) return false;
  if ( dlen < 1 || tgt >= dlen || ! pdata ) return false;

  CES::Vec3<size_t> xdims = GetMax3DTexSize(CES::Vec3<size_t>(dims));
  size_t xdSz = xdims[0]*xdims[1]*xdims[2];
  if ( xdSz < 1 ) return false;

  unsigned char* data = new unsigned char[dSz];
  if ( ! data ) return false;
  memset(data, 0, dSz);

  float d = drange[1] - drange[0];
  if ( fabs(d) < 1e-8 ) d = 1e-8f;

  register size_t i, j, k, idx;
  register float x, a, b;
  a = 255.f / d;
  b = -255.f * drange[0] / d;
  for ( k = 0; k < dims[2]; k++ )
    for ( j = 0; j < dims[1]; j++ )
      for ( i = 0; i < dims[0]; i++ ) {
	idx = dims[0]*dims[1]*k + dims[0]*j + i;
	if ( pmask && pmask[idx] ) x = drange[0];
	else x = *(pdata + dlen*idx + tgt);
	data[idx] = (unsigned char)(a * x + b);
      } // end of for(i)

  m_data_updated = true;
  if ( m_data ) {
    delete [] m_data;
    m_data = NULL;
  }
  m_data = new unsigned char[xdSz];
  if ( ! m_data || ! VSN::resample(dims, xdims, data, m_data) ) {
    delete [] data;
    return false;
  }
  delete [] data;
  m_dims = xdims;

  notice();
  return true;
}

bool vsnRvrVolumeRender::SetData(vsnOctTree* poct, const size_t tgt,
				 const CES::Vec3<int>& didx,
				 const float drange[2],
				 const bool doInterp) {
  register int i, j, k, idx;

  if ( ! poct ) return false;
  if ( poct->m_rootDims[0]*poct->m_rootDims[1]*poct->m_rootDims[2] < 1 )
    return false;
  const size_t dlen = poct->m_dataLen;

  CES::Vec3<size_t> maxDims, dims;
  maxDims[0] = poct->m_rootDims[0] << poct->m_maxLevel;
  maxDims[1] = poct->m_rootDims[1] << poct->m_maxLevel;
  maxDims[2] = poct->m_rootDims[2] << poct->m_maxLevel;
  dims = GetMax3DTexSize(maxDims);
  size_t dSz = dims[0]*dims[1]*dims[2];
  if ( dSz < 1 ) return false;

  CES::Vec3<float> pitch = (_bbox[1] - _bbox[0]);
  pitch[0] /= (float)dims[0];
  pitch[1] /= (float)dims[1];
  pitch[2] /= (float)dims[2];
  
  m_data_updated = true;
  if ( m_data ) {
    delete [] m_data;
    m_data = NULL;
  }
  m_data = new unsigned char[dSz];
  if ( ! m_data ) return false;
  memset(m_data, 0, dSz);
  m_dims = dims;

  float d = drange[1] - drange[0];
  if ( fabs(d) < 1e-8 ) d = 1e-8f;
  
  float x, a, b;
  CES::Vec3<float> pos;
  a = 255.f / d;
  b = -255.f * drange[0] / d;
  if ( tgt == VSN::DATA_Veclen ) {
    CES::Vec3<float> dval;
    pos[2] = _bbox[0][2] + pitch[2]*0.5f;
    for ( k = 0; k < dims[2]; k++, pos[2] += pitch[2] ) {
      pos[1] = _bbox[0][1] + pitch[1]*0.5f;
      for ( j = 0; j < dims[1]; j++, pos[1] += pitch[1] ) {
	pos[0] = _bbox[0][0] + pitch[0]*0.5f;
	for ( i = 0; i < dims[0]; i++, pos[0] += pitch[0] ) {
	  idx = dims[0]*dims[1]*k + dims[0]*j + i;
	  vsnOctTree::Node* pnode = poct->FindByPos(pos);
	  if ( ! pnode )
	    x = 0.f;
	  else if ( doInterp ) {
	    poct->InterpolateData(pos, pnode, didx, dval);
	    x = dval.Length();
	  } else {
	    dval.m_v[0] = (didx[0] < 0) ? 0.f : pnode->m_pData[didx[0]];
	    dval.m_v[1] = (didx[1] < 0) ? 0.f : pnode->m_pData[didx[1]];
	    dval.m_v[2] = (didx[2] < 0) ? 0.f : pnode->m_pData[didx[2]];
	    x = dval.Length();
	  }
	  m_data[idx] = (unsigned char)(a * x + b);
	} // end of for(i)
      } // end of for(j)
    } // end of for(k)
  } // end of DATA_Veclen
  else {
    pos[2] = _bbox[0][2] + pitch[2]*0.5f;
    for ( k = 0; k < dims[2]; k++, pos[2] += pitch[2] ) {
      pos[1] = _bbox[0][1] + pitch[1]*0.5f;
      for ( j = 0; j < dims[1]; j++, pos[1] += pitch[1] ) {
	pos[0] = _bbox[0][0] + pitch[0]*0.5f;
	for ( i = 0; i < dims[0]; i++, pos[0] += pitch[0] ) {
	  idx = dims[0]*dims[1]*k + dims[0]*j + i;
	  vsnOctTree::Node* pnode = poct->FindByPos(pos);
	  if ( ! pnode )
	    x = drange[0];
	  else if ( doInterp )
	    poct->InterpolateData(pos, pnode, tgt -1, x);
	  else
	    x = pnode->m_pData[tgt -1];
	  m_data[idx] = (unsigned char)(a * x + b);
	} // end of for(i)
      } // end of for(j)
    } // end of for(k)
  }

  notice();
  return true;
}

bool vsnRvrVolumeRender::SetSliceNum(const size_t nsl) {
  if ( m_nslices == nsl ) return true;
  if ( nsl < 1 ) return false;
  m_nslices = nsl;
  if ( m_render ) {
    m_render->SetNumOfLayers((int)m_nslices);
    notice();
  }
  return true;
}

void vsnRvrVolumeRender::SetShadingMode(const bool mode) {
  if ( m_shading == mode ) return;
  m_shading = mode;
  if ( m_render ) {
    m_render->SetShading(m_shading);
    notice();
  }
}

bool vsnRvrVolumeRender::SetGeom(const float p0[3], const float p1[3]) {
  if ( ! p0 || ! p1 ) return false;
  _bbox[0] = CES::Vec3<float>(p0);
  _bbox[1] = CES::Vec3<float>(p1);
  checkBbox();

  if ( m_render ) {
    m_render->SetBbox(_bbox[0].m_v, _bbox[1].m_v);
    notice();
  }

  return true;
}

void vsnRvrVolumeRender::SetRegion(const CES::Vec3<float> cp[2]) {
  if ( ! m_render ) return;
  double p[6];
  p[0] = cp[0][0];  p[1] = cp[1][0];
  p[2] = cp[0][1];  p[3] = cp[1][1];
  p[4] = cp[0][2];  p[5] = cp[1][2];
  m_render->SetClipPlane(p);
  notice();
}

void vsnRvrVolumeRender::GetRegion(CES::Vec3<float> cp[2]) const {
  if ( ! m_render ) return;
  double p[6];
  m_render->GetClipPlane(p);
  cp[0][0] = p[0];  cp[1][0] = p[1];
  cp[0][1] = p[2];  cp[1][1] = p[3];
  cp[0][2] = p[4];  cp[1][2] = p[5];
}

bool vsnRvrVolumeRender::SetLut(const vsnLut& lut) {
  m_lut = lut;
  if ( m_render )
    notice();
  return true;
}

vsnLut vsnRvrVolumeRender::GetLut() const {
  return m_lut;
}

bool vsnRvrVolumeRender::SetDerivLut(const vsnLut& lut) {
  m_lut2 = lut;
  if ( m_render )
    notice();
  return true;
}

vsnLut vsnRvrVolumeRender::GetDerivLut() const {
  return m_lut2;
}

void vsnRvrVolumeRender::updateLut() {
  if ( ! m_render ) return;
  rvrLUT rLut;

  ConvLut(m_lut, rLut);
  m_render->SetLUT(rLut);

  ConvLut(m_lut2, rLut);
  m_render->SetLUT(rLut, true);

  notice();
}

void vsnRvrVolumeRender::updateGeom() {
  if ( ! m_render ) return;
  m_render->SetBbox(_bbox[0].m_v, _bbox[1].m_v);
}

void vsnRvrVolumeRender::updateData() {
  if ( ! m_render ) return;
  if ( ! m_data || m_dims[0]*m_dims[1]*m_dims[2] < 1 ) return;
  if ( ! m_data_updated ) return;

  int ret = m_render->SetVolume(m_data, m_dims[0], m_dims[1], m_dims[2]);
  string errStr = m_render->GetLastErr();
  if ( ! errStr.empty() )
    cerr << errStr;
  if ( ret != 0 )
    return;

  m_data_updated = false;
}


void vsnRvrVolumeRender::DrawVolume(const CES::Mat4<float>& mvm) {
  if ( ! m_render ) return;
  if ( _material->getRenderMode() == RT_NONE ) return;

  vsnScene* psc = NULL;
  vsnMethodObj* pMtd = dynamic_cast<vsnMethodObj*>(_Ref[0]);
  if ( pMtd ) psc = pMtd->getRefData()->getAncestorScene();

  vsnLightAttribute lightAttr(NULL);
  if ( psc ) lightAttr = psc->getLightAttr();
  CES::Vec3<float> lightPos(0, 0, 0);
  if ( lightAttr.getLightType() == vsnLightAttribute::Light_Point ) {
    lightPos = lightAttr.getPosition();
  } else {
    CES::Vec3<float> lxyz = lightAttr.getDirection();
    vfrMatrix M;
    M.RotY(CES::Deg2Rad(lxyz[1]));
    M.RotX(CES::Deg2Rad(lxyz[0]));
    M.RotZ(CES::Deg2Rad(lxyz[2]));
    CES::Vec3<float> l0(0.f, 0.f, 100.f);
    lightPos = M * l0;
  }
  glLightfv(GL_LIGHT0, GL_POSITION, lightPos.m_v);

  bool xforming = false;
  if ( vsnScene::GetXformSimpleRender() ) {
    if ( psc ) xforming = psc->getXforming();
  }
  m_render->SetNumOfLayers(xforming ? VSN_RVR_NUMSLICES_OMIT : m_nslices);

  _material->apply();

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glMultMatrixf(mvm.m_v);

  updateLut();
  updateGeom();
  updateData();

  m_render->Draw();
  string errStr = m_render->GetLastErr();
  if ( ! errStr.empty() )
    cerr << errStr;

  _material->unApply();

  glPopMatrix();
}
