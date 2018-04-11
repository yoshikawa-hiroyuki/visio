
//
// vsnNvrVolumeRender
//
#include <GL/glew.h>
#include "nvrOrthoSliceBrick.h"
#include "vsnNvrVolumeRender.h"

using namespace std;
using namespace CES;
using namespace NVR;
using namespace VSN;


//----------------------------------------------------------------
// class vsnNvrVolumeRender
//----------------------------------------------------------------

/* constructors / destructor */

vsnNvrVolumeRender::vsnNvrVolumeRender(const std::string& name)
  : vsnVolumeRender(name), m_pData(NULL), m_doReduce(true), m_oglChkd(false)
{
  // setup NVR parameters
  if ( nvrBrick::s_renderMode != NVR::T3D ) {
    nvrBrick::s_renderMode = NVR::T3D;
    nvrOrthoSliceBrick::s_intermediate = Dim3(2,2,2);
  }
  NVR::g_useTexColorTable = true;

  // check OpenGL extensions
  nvrOrthoSliceBrickFactory brickFactory;
  if ( m_render.CheckReqExtensions(&brickFactory) )
    m_oglChkd = true;
}

vsnNvrVolumeRender::~vsnNvrVolumeRender() {
  if ( m_pData ) VFR::DeAllocate(m_pData);
}


/* methods */

bool
vsnNvrVolumeRender::Initialize(CES::Vec3<size_t>& dims, const bool needAlc) {
  if ( ! m_oglChkd )
    return false;
  if ( dims[0] < 2 || dims[1] < 2 || dims[2] < 2 )
    return false;
  m_status = VRen_Nodata;

  m_origDims = Dim3(dims[0], dims[1], dims[2]);
  m_wrapDims = NVR::GetWrapDims(m_origDims, CalcDivTimes(m_origDims));

  if ( needAlc ) {
    m_pData = (unsigned char*)VFR::ReAllocate(m_pData, m_wrapDims.Size());
    if ( ! m_pData ) return false;
  }

  nvrOrthoSliceBrickFactory brickFactory;
  if ( ! m_render.Initialize(&brickFactory, UNSIGNED_BYTE, m_wrapDims) )
    return false;

  if ( ! m_render.SetValidDims(m_origDims) )
    return false;

  m_status |= VRen_Inited;
  m_status |= VRen_HasLut;
  return true;
}

bool vsnNvrVolumeRender::SetVolume(const float* pd, const float minmax[2],
				   const CES::Vec3<float>* pbb,
				   const unsigned char* pmask) {
  if ( !(m_status & VRen_Inited) ) return false;
  if ( ! pd || ! m_pData ) return false;
  m_status ^= VRen_HasData;

  Vec3<float> bb[2];
  if ( pbb ) {
    _bbox[0] = pbb[0]; _bbox[1] = pbb[1]; checkBbox();
    m_origBbox[0] = pbb[0]; m_origBbox[1] = pbb[1];
    bb[0] = pbb[0]; bb[1] = pbb[0];
    bb[1][0] += (pbb[1][0]-pbb[0][0])*(m_wrapDims[0]-1.f)/(m_origDims[0]-1.f);
    bb[1][1] += (pbb[1][1]-pbb[0][1])*(m_wrapDims[1]-1.f)/(m_origDims[1]-1.f);
    bb[1][2] += (pbb[1][2]-pbb[0][2])*(m_wrapDims[2]-1.f)/(m_origDims[2]-1.f);
  } else {
    bb[0] = Vec3<float>(0, 0, 0);
    bb[1] = Vec3<float>(m_wrapDims[0]-1.f,m_wrapDims[1]-1.f,m_wrapDims[2]-1.f);
    m_origBbox[0] = bb[0]; m_origBbox[1] = bb[1];
  }

  memset(m_pData, 0, m_wrapDims.Size());
  register float facA, facB;
  if ( fabs(minmax[1] - minmax[0]) < 1e-8 ) {facA = facB = 0.f;}
  else {
    facA = 254.f / (minmax[1] - minmax[0]);
    facB = facA * minmax[0];
  }
  register size_t i, j, k, oidx, widx;
  for ( k = 0; k < m_origDims.size[2]; k++ )
    for ( j = 0; j < m_origDims.size[1]; j++ )
      for ( i = 0; i < m_origDims.size[0]; i++ ) {
	oidx = k*m_origDims.size[0]*m_origDims.size[1]+j*m_origDims.size[0]+i;
	widx = k*m_wrapDims.size[0]*m_wrapDims.size[1]+j*m_wrapDims.size[0]+i;
	if ( pmask && pmask[oidx] ) {m_pData[widx] = 0; continue;}
	if ( pd[oidx] < minmax[0] ) {m_pData[widx] = minmax[0]; continue;}
	if ( pd[oidx] > minmax[1] ) {m_pData[widx] = minmax[1]; continue;}
	m_pData[widx] = (unsigned char)(pd[oidx] * facA - facB) + 1;
      } // end of for(i)

  if ( ! m_render.UpdateData((void*)m_pData, bb) )
    return false;

  if ( m_doReduce ) {
    if ( ! m_render.Reduce(MEMORY_SIZE) )
      return false;
  } else {
    if ( ! m_render.Reduce(SIMPLE, 0) )
      return false;
  }

  m_status |= VRen_HasData;
  return true;
}

bool vsnNvrVolumeRender::SetVolume(const deque<unsigned char*>& pdl,
				   const CES::Vec3<float>* pbb) {
  if ( !(m_status & VRen_Inited) ) return false;
  deque<nvrBrick*>& bl = m_render.GetBrickList();
  if ( bl.size() != pdl.size() ) return false;
  m_status ^= VRen_HasData;

  Vec3<float> bb[2];
  if ( pbb ) {
    _bbox[0] = pbb[0]; _bbox[1] = pbb[1]; checkBbox();
    m_origBbox[0] = pbb[0]; m_origBbox[1] = pbb[1];
    bb[0] = pbb[0]; bb[1] = pbb[0];
    bb[1][0] += (pbb[1][0]-pbb[0][0])*(m_wrapDims[0]-1.f)/(m_origDims[0]-1.f);
    bb[1][1] += (pbb[1][1]-pbb[0][1])*(m_wrapDims[1]-1.f)/(m_origDims[1]-1.f);
    bb[1][2] += (pbb[1][2]-pbb[0][2])*(m_wrapDims[2]-1.f)/(m_origDims[2]-1.f);
  } else {
    bb[0] = Vec3<float>(0, 0, 0);
    bb[1] = Vec3<float>(m_wrapDims[0]-1.f,m_wrapDims[1]-1.f,m_wrapDims[2]-1.f);
    m_origBbox[0] = bb[0]; m_origBbox[1] = bb[1];
  }

  deque<nvrBrick*>::iterator itB;
  deque<unsigned char*>::const_iterator itPD;
  for ( itB = bl.begin(), itPD = pdl.begin();
	itB != bl.end() && itPD != pdl.end(); itB++, itPD++ ) {
    if ( ! *itB || ! *itPD ) continue;
    (*itB)->SetRefData(*itPD, UNSIGNED_BYTE, nvrBrick::RD_Brick);
  } // end of for(itB,itPD)

  if ( m_doReduce ) {
    if ( ! m_render.Reduce(MEMORY_SIZE) )
      return false;
  } else {
    if ( ! m_render.Reduce(SIMPLE, 0) )
      return false;
  }

  m_status |= VRen_HasData;
  return true;
}

bool vsnNvrVolumeRender::SetLut(const vsnLut& lut) {
  nvrLUT nlut;
  vsnLut wkLut(lut);
  if ( ! wkLut.normalize() ) return false;
  memset(wkLut.lutEntry, 0, sizeof(float)*4);
  if ( ! nlut.SetTable(wkLut.lutEntry) ) return false;

  if ( ! m_render.UpdateLUT(nlut) ) return false;

  //m_status |= VRen_HasLut;
  return true;
}

void vsnNvrVolumeRender::DrawVolume(const CES::Mat4<float>& mvm) {
  if ( (m_status & VRen_Drawable) != VRen_Drawable )
    return;
  if ( _material->getRenderMode() == RT_NONE )
    return;

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glMultMatrixf(mvm.m_v);

  glDisable(GL_LIGHTING);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  /* set clip planes (for frag-shader) */
  Vec3<float> hp;
  hp[0] = (m_origBbox[1][0]-m_origBbox[0][0])/(m_origDims[0]-1.f)*0.25;
  hp[1] = (m_origBbox[1][1]-m_origBbox[0][1])/(m_origDims[1]-1.f)*0.25;
  hp[2] = (m_origBbox[1][2]-m_origBbox[0][2])/(m_origDims[2]-1.f)*0.25;
  double clip0[]  = {-1.0,  0.0,  0.0,  m_origBbox[1][0]-hp[0]};
  double clip1[]  = { 0.0, -1.0,  0.0,  m_origBbox[1][1]-hp[1]};
  double clip2[]  = { 0.0,  0.0, -1.0,  m_origBbox[1][2]-hp[2]};
  glClipPlane(GL_CLIP_PLANE0, clip0);
  glClipPlane(GL_CLIP_PLANE1, clip1);
  glClipPlane(GL_CLIP_PLANE2, clip2);
  glEnable(GL_CLIP_PLANE0);
  glEnable(GL_CLIP_PLANE1);
  glEnable(GL_CLIP_PLANE2);

  glColor4fv(_colors[0]);
  m_render.DrawVolume();

  /* unset clip planes (for frag-shader) */
  glDisable(GL_CLIP_PLANE0);
  glDisable(GL_CLIP_PLANE1);
  glDisable(GL_CLIP_PLANE2);

  glEnable(GL_LIGHTING);

  glPopMatrix();
}

bool vsnNvrVolumeRender::setReduceMode(const bool rdm) {
  if ( m_doReduce == rdm ) return true;
  m_doReduce = rdm;

  if ( !(m_status & VRen_HasData) ) return true;
  m_status ^= VRen_HasData;

  if ( m_doReduce ) {
    if ( ! m_render.Reduce(MEMORY_SIZE) )
      return false;
  } else {
    if ( ! m_render.Reduce(SIMPLE, 0) )
      return false;
  }

  m_status |= VRen_HasData;
  return true;
}
