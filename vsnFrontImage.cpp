//
// vsnFrontImage
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
#include "wx/image.h"

#include "vsnFrontImage.h"


//----------------------------------------------------------------
// class vsnFrontImage
//----------------------------------------------------------------

/* constructors / destructor */

vsnFrontImage::vsnFrontImage(const std::string& name)
  : vsnFrontObj(name), m_pImage(NULL), m_depth(0)
{
  m_validArea[0] = m_validArea[1] = 0.f;
  setAlpha(TRUE, 0.5f);
}

vsnFrontImage::~vsnFrontImage() {
  if ( m_pImage ) DeAllocate(m_pImage);
}


/* methods */

bool vsnFrontImage::setImageData(const unsigned char* pdata,
				 const int w, const int h, const int d)
{
  // cleanup
  if ( m_pImage ) {
    DeAllocate(m_pImage); m_pImage = NULL;
  }

  if ( ! pdata || w < 1 || h < 1 || d < 1 )
    return true;

  // allocate
  Point2 wsz;
  GetWrapSize(w, h, wsz);
  size_t dsz = wsz.x * wsz.y * d;
  m_pImage = (unsigned char*)Allocate(dsz);
  if ( ! m_pImage ) return false;
  memset(m_pImage, 0, dsz);

  // copy data
  register int i;
  dsz = (size_t)(w * d);
  unsigned char* pSrc = const_cast<unsigned char*>(pdata);
  unsigned char* pDst = m_pImage;
  for ( i = 0; i < h; i++ ) {
    memcpy(pDst, pSrc, dsz);    
    pSrc += dsz;
    pDst += wsz.x * d;
  } // end of for(i)

  m_size = wsz;
  m_depth = d;
  m_validArea[0] = (float)w / (float)wsz.x;
  m_validArea[1] = (float)h / (float)wsz.y;

  notice();
  return true;
}


/* override methods */

void vsnFrontImage::render(const Bool transpMode, vfrMaterialStack *mstk) {
  if ( getRenderMode() == RT_NONE || transpMode != isTransparency() )
    return;
  if ( ! m_pImage ) return;
  const unsigned char* pd = m_pImage;

  int _format;
  switch ( m_depth ) {
  case 2: _format = GL_LUMINANCE_ALPHA; break;
  case 3: _format = GL_RGB; break;
  case 4: _format = GL_RGBA; break;
  case 1: default:
    _format = GL_LUMINANCE; break;
  }

  // get aspect ratio
  GLint vp[4];
  glGetIntegerv(GL_VIEWPORT, vp);
  GLenum glerr = glGetError();
  if ( glerr != GL_NO_ERROR ) {
    while ( glGetError() != GL_NO_ERROR ); // clear errors
    return;
  }
  float asp = 1.f;
  if ( vp[2] > 0 && vp[3] > 0 ) asp = (float)vp[2]/(float)vp[3];

  glEnable(GL_TEXTURE_2D);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glBindTexture(GL_TEXTURE_2D, getID());
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_size.x, m_size.y,
	       0, _format, GL_UNSIGNED_BYTE, pd);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  // draw
  glDisable(GL_LIGHTING);
  glPolygonMode(GL_FRONT, GL_FILL);

  glColor4fv(_colors[0]);
  glBegin(GL_POLYGON);
  glTexCoord2f(0.0f, 0.0f);
  glVertex3f(-asp, -1.0f, 0.0f);
  glTexCoord2f(m_validArea[0], 0.0f);
  glVertex3f(asp, -1.0f, 0.0f);
  glTexCoord2f(m_validArea[0], m_validArea[1]);
  glVertex3f(asp, 1.0f, 0.0f);
  glTexCoord2f(0.0f, m_validArea[1]);
  glVertex3f(-asp, 1.0f, 0.0f);
  glEnd();

  glDisable(GL_TEXTURE_2D);
  glEnable(GL_LIGHTING);
}

#define _IMG_TILE_MAX 8192
#define _IMG_ASPR_MAX 1024

// static
void vsnFrontImage::GetWrapSize(const int x, const int y, Point2& sz) {
  register int s;
  if ( x < y ) {
    for ( s = 1; s < x && s <= _IMG_TILE_MAX; s *= 2 );
    sz.x = s;
    for ( s = 1; sz.x*s < y && s <= _IMG_ASPR_MAX; s *= 2 );
    sz.y = sz.x * s;
  } else { 
    for ( s = 1; s < y && s <= _IMG_TILE_MAX; s *= 2 );
    sz.y = s;
    for ( s = 1; sz.y*s < x && s <= _IMG_ASPR_MAX; s *= 2 );
    sz.x = sz.y * s;
  }
}

#undef _IMG_TILE_MAX
#undef _IMG_ASPR_MAX
