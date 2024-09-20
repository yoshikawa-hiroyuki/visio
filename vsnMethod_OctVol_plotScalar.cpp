//
// vsnMethod_OctVol_plotScalar
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

#include "vsnMethod_OctVol_plotScalar.h"
#include "vsnData_OctVol.h"
#include "vsnError.h"

using namespace std;
using namespace CES;
using namespace VSN;


//----------------------------------------------------------------
// class vsnMethod_OctVol_plotScalar
//----------------------------------------------------------------

/* constructors / destructor */

vsnMethod_OctVol_plotScalar::vsnMethod_OctVol_plotScalar(const string& name)
  : vsnMethod_plotScalar(name)
{
}

vsnMethod_OctVol_plotScalar::~vsnMethod_OctVol_plotScalar() {
}


/* vsnTimeSeriesMethodIF methods */

bool vsnMethod_OctVol_plotScalar::updateStep(const int stp,
					     const bool force,
					     const bool cascade)
{
  vsnData_OctVol* pData = dynamic_cast<vsnData_OctVol*>(p_refData);
  if ( ! pData ) return false;
  m_requestedStp = stp;
  if ( m_requestedStp < 0 )
    m_requestedStp = pData->getCurrentStepIdx();

  if ( force ) m_updatedStp = -1;
  if ( m_updatedStp == m_requestedStp ) return true;
  m_updatedStp = -1;

  // mesh data
  if ( ! m_mesh ) {
    m_mesh = new vsnMesh2D_EX();
    if ( ! m_mesh ) {
      ErrMsg(MsgERR, getMethodType() + string("[") + getName()
             + string("]: memory allocation failed"));
      return false;
    }
    m_mesh->alcMaterial();
    m_mesh->setNormalMode(AT_WHOLE);
    addChild(m_mesh);
  }
  m_mesh->getPrivateMaterial()->setRenderMode(RT_NONE);
  if ( ! m_show ) {
    return true;
  }

  vsnMesh2D_EX* pMeshEX = (vsnMesh2D_EX*)m_mesh;

  // check selected data
  size_t dlen = pData->getDataLen();
  if ( m_selectedData == DATA_Veclen ) {
    if ( ! isValidVecData() ) return true;
  }
  else if ( m_selectedData > dlen ) return true;

  // check sampler
  if ( ! p_splr ) return true;
  Point2 sampleSize = p_splr->getSampleNumber();
  size_t sampleSz = sampleSize.x * sampleSize.y;
  if ( sampleSz < 2 ) return true;
  const vector3* const samplePts = p_splr->getSamplePoints();
  if ( ! samplePts ) {
    ErrMsg(MsgERR, getMethodType() + string("[") + getName()
           + string("]: can't get sampling points data"));
    return false;
  }

  // alloc mesh datas
  if ( ! m_mesh->setMeshSize(sampleSize) ||
       ! m_mesh->alcColors(sampleSz) ) {
    return false;
  }

  // set mesh verts
  register int i, c;
  vector3* vl = m_mesh->getVerts();
  memcpy(vl, samplePts, sizeof(vector3)*sampleSz);
  m_mesh->generateBbox();

  // set normal vector
  Vec3<float> vn = p_splr->getNormalVec();
  m_mesh->setNormal(0, vn.m_v);

  // MHIR append begin
  pMeshEX -> InitMask();
  // MHIR append end

  // set mesh colors
  vector4* cl = m_mesh->getColors();
  if ( m_selectedData == DATA_Veclen ) {
    Vec3<float> vv;
    for ( i = 0; i < sampleSz; i++ ) {
      vsnOctTree::Node* pnode = pData->getNode(samplePts[i]);
      if ( pnode && pData->interpolateData(Vec3<float>(samplePts[i]),
					   pnode, m_vecDataIdx, vv) ) {
	c = m_lut.getValIdx(vv.Length());
	cl[i][0] = m_lut.lutEntry[c*4  ];
	cl[i][1] = m_lut.lutEntry[c*4+1];
	cl[i][2] = m_lut.lutEntry[c*4+2];
	cl[i][3] = 1.f;
      } else {
	cl[i][0] = cl[i][1] = cl[i][2] = 0.141f; cl[i][3] = 1.f;
	// MHIR append begin
	pMeshEX -> SetMask(i);
	// MHIR append end
      }
    } // end of for(i)
    m_mesh->setColorMode(AT_PER_VERTEX);
  } // end of if(DATA_Veclen)
  else if ( m_selectedData > 0 && m_selectedData <= dlen ) {
    Vec3<float> vv; Vec3<int> didx(m_selectedData-1, -1, -1);
    for ( i = 0; i < sampleSz; i++ ) {
      vsnOctTree::Node* pnode = pData->getNode(samplePts[i]);
      if ( pnode && pData->interpolateData(Vec3<float>(samplePts[i]),
					   pnode, didx, vv) ) {
	c = m_lut.getValIdx(vv.m_v[0]);
	cl[i][0] = m_lut.lutEntry[c*4  ];
	cl[i][1] = m_lut.lutEntry[c*4+1];
	cl[i][2] = m_lut.lutEntry[c*4+2];
	cl[i][3] = 1.f;
      } else {
	cl[i][0] = cl[i][1] = cl[i][2] = 0.141f; cl[i][3] = 1.f;
	// MHIR append begin
	pMeshEX -> SetMask(i);
	// MHIR append end
      }
    } // end of for(i)    
    m_mesh->setColorMode(AT_PER_VERTEX);
  } // end of if(!DATA_Veclen&&!DATA_None)
  else {
    m_mesh->setColorMode(AT_WHOLE);
  } // end of if(DATA_None)

  // MHIR append begin
  // 必要なければマスク情報をクリアする
  pMeshEX -> ClearMaskIfNotNeed();
  // MHIR append end

  // ok
  m_mesh->getPrivateMaterial()->setRenderMode(m_showType);
  setBaseColor(m_colour);
  setHilight(m_hilight);
  m_updatedStp = m_requestedStp;
  return true;
}


//----------------------------------------------------------------
// class vsnMesh2D_EX
//----------------------------------------------------------------

void
vsnMesh2D_EX::renderSolid() {
  if ( _meshSize.x < 2 || _meshSize.y < 2 )
    return;

  // display-list check
  if ( beginDispList(DLF_SOLID) ) return;

  vfrTexture::MapType mapType = vfrTexture::NOMAP;
  int num_uvc = 0;
  vector2* uvcs = NULL;

  if ( _texture ) {
    if ( _texture->getMapType() == vfrTexture::UVC ) {
      mapType = vfrTexture::UVC;
      num_uvc = _texture->getNumUVCs();
      uvcs = _texture->getUVCs();
      if ( ! uvcs || num_uvc < 1 ) mapType = vfrTexture::NOMAP;
    }
  }

  if ( _normalMode == AT_WHOLE )
    glNormal3fv(_normals[0]);

  register int index, findex = 0;
  glBegin(GL_QUADS);
  for ( register int j = 0; j < _meshSize.y -1; j++ ) {
    for ( register int i = 0; i < _meshSize.x -1; i++ ) {

      // MHIR append begin
      // 頂点のうち一つでも非表示の頂点があれば、表示しない
      //
      if (_colorMode == AT_PER_VERTEX && ! m_mask.empty()) {
	index = j       * _meshSize.x + i;	if (m_mask[index]) continue;
	index = j       * _meshSize.x + i + 1;	if (m_mask[index]) continue;
	index = (j + 1) * _meshSize.x + i + 1;	if (m_mask[index]) continue;
	index = (j + 1) * _meshSize.x + i;	if (m_mask[index]) continue;
      }
      // MHIR append end

      if ( _colorMode == AT_PER_FACE && findex < nColors )
        glColor4fv(_colors[findex]);
      if ( _normalMode == AT_PER_FACE && findex < nNormals )
        glNormal3fv(_normals[findex]);

      // V1
      index = j*_meshSize.x + i;
      if ( _colorMode == AT_PER_VERTEX && index < nColors )
	glColor4fv(_colors[index]);
      if ( _normalMode == AT_PER_VERTEX && index < nNormals )
	glNormal3fv(_normals[index]);
      if ( mapType == vfrTexture::UVC && index < num_uvc )
	glTexCoord2fv(uvcs[index]);
      glVertex3fv(_verts[index]);

      // V2
      index = j*_meshSize.x + i +1;
      if ( _colorMode == AT_PER_VERTEX && index < nColors )
	glColor4fv(_colors[index]);
      if ( _normalMode == AT_PER_VERTEX && index < nNormals )
	glNormal3fv(_normals[index]);
      if ( mapType == vfrTexture::UVC && index < num_uvc )
	glTexCoord2fv(uvcs[index]);
      glVertex3fv(_verts[index]);

      // V3
      index = (j +1)*_meshSize.x + i +1;
      if ( _colorMode == AT_PER_VERTEX && index < nColors )
	glColor4fv(_colors[index]);
      if ( _normalMode == AT_PER_VERTEX && index < nNormals )
	glNormal3fv(_normals[index]);
      if ( mapType == vfrTexture::UVC && index < num_uvc )
	glTexCoord2fv(uvcs[index]);
      glVertex3fv(_verts[index]);

      // V4
      index = (j +1)*_meshSize.x + i;
      if ( _colorMode == AT_PER_VERTEX && index < nColors )
	glColor4fv(_colors[index]);
      if ( _normalMode == AT_PER_VERTEX && index < nNormals )
	glNormal3fv(_normals[index]);
      if ( mapType == vfrTexture::UVC && index < num_uvc )
	glTexCoord2fv(uvcs[index]);
      glVertex3fv(_verts[index]);

      findex++;
    } // end of for(i)
  } // end of for(j)
  glEnd();

  // end display-list definition
  endDispList(DLF_SOLID);
}
