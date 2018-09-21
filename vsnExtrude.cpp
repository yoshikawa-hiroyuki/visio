//
// vsnExtrude
//
#include "vsnExtrude.h"

/* GLE Tubing and Extrusion */
#ifdef __APPLE__
#include <OpenGL/gle.h>
#else // __APPLE__
#include <GL/gle.h>
#endif // __APPLE__


//----------------------------------------------------------------
// class vsnTubeLines
//----------------------------------------------------------------
vsnTubeLines::vsnTubeLines(const std::string& nm, const Bool ssm)
  : vfrLines(nm, ssm), m_radius(0.1) {
}

vsnTubeLines::~vsnTubeLines() {
}


bool vsnTubeLines::setRadius(const double r) {
  if ( r == m_radius ) return true;
  if ( r < EPSF ) return false;
  m_radius = r;
  notice();
  return true;
}


RenderType vsnTubeLines::getRenderMode() const {
  return vfrNode::getRenderMode();
}


void vsnTubeLines::renderSolid() {
  if ( nVerts < 2 ) return;

  double ptbuff[2][3];
  float clbuff[2][4];
  
  // display-list check
  if ( beginDispList(DLF_SOLID) ) return;

  gleSetJoinStyle(TUBE_NORM_EDGE | TUBE_JN_ANGLE | TUBE_JN_CAP);
  
  register int index = 0;
  register int findex = 0;
  while ( index < nVerts -1 ) {
    if ( _colorMode == AT_PER_FACE )
      if ( findex < nColors ) {
	clbuff[1][0] = clbuff[0][0] = _colors[findex][0];
	clbuff[1][1] = clbuff[0][1] = _colors[findex][1];
	clbuff[1][2] = clbuff[0][2] = _colors[findex][2];
	clbuff[1][3] = clbuff[0][3] = _colors[findex][3];
      }
    
    ptbuff[0][0] = _verts[index][0];
    ptbuff[0][1] = _verts[index][1];
    ptbuff[0][2] = _verts[index][2];
    if ( _colorMode == AT_PER_VERTEX )
      if ( index < nColors ) {
	clbuff[0][0] = _colors[index][0]; clbuff[0][1] = _colors[index][1];
	clbuff[0][2] = _colors[index][2]; clbuff[0][3] = _colors[index][3];	
      }
    index++;
    
    ptbuff[1][0] = _verts[index][0];
    ptbuff[1][1] = _verts[index][1];
    ptbuff[1][2] = _verts[index][2];
    if ( _colorMode == AT_PER_VERTEX )
      if ( index < nColors ) {
	clbuff[1][0] = _colors[index][0]; clbuff[1][1] = _colors[index][1];
	clbuff[1][2] = _colors[index][2]; clbuff[1][3] = _colors[index][3];	
      }
    index++;

    glePolyCylinder_c4f(2, ptbuff, clbuff, m_radius);
    
    findex++;
  } // end of while(index)
  
  // end display-list definition
  endDispList(DLF_SOLID);
}

//----------------------------------------------------------------
// class vsnTubeLineStrip
//----------------------------------------------------------------
vsnTubeLineStrip::vsnTubeLineStrip(const std::string& nm, const Bool ssm)
  : vfrLineStrip(nm, ssm), m_radius(0.1), m_ptbuff(NULL) {
}

vsnTubeLineStrip::~vsnTubeLineStrip() {
  if ( m_ptbuff ) {
    DeAllocate(m_ptbuff);
    m_ptbuff = NULL;
  }  
}


bool vsnTubeLineStrip::setRadius(const double r) {
  if ( r == m_radius ) return true;
  if ( r < EPSF ) return false;
  m_radius = r;
  notice();
  return true;
}


RenderType vsnTubeLineStrip::getRenderMode() const {
  return vfrNode::getRenderMode();
}


void vsnTubeLineStrip::renderSolid() {
  /* IGNORE LOOP MODE
   */
  if ( nVerts < 2 ) return;

  // display-list check
  if ( beginDispList(DLF_SOLID) ) return;

  gleSetJoinStyle(TUBE_NORM_EDGE | TUBE_JN_ANGLE | TUBE_JN_CAP);

  int nvtx = nVerts;
  m_ptbuff = (DVec3*)ReAllocate(m_ptbuff, sizeof(VSN::DVec3)*nvtx);
  if ( ! m_ptbuff ) {
    endDispList(DLF_SOLID);
    return;
  }
  for ( register int i = 0; i < nVerts; i++ ) {
    m_ptbuff[i][0] = _verts[i][0];
    m_ptbuff[i][1] = _verts[i][1];
    m_ptbuff[i][2] = _verts[i][2];
  } // end of for(i)

  if ( (_colorMode == AT_PER_VERTEX || _colorMode == AT_PER_FACE) \
       && nColors >= nvtx )
    glePolyCylinder_c4f(nvtx, m_ptbuff, _colors, m_radius);
  else {
    glColor4fv(_colors[0]);
    glePolyCylinder_c4f(nvtx, m_ptbuff, NULL, m_radius);
  }

  // end display-list definition
  endDispList(DLF_SOLID);
}
