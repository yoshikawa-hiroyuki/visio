//
// vsnExtrude
//
#include "vsnExtrude.h"
#include "vsnGridUtilSv.h" // for DVec3

/* GLE Tubing and Extrusion */
#include <GL/gle.h>


//----------------------------------------------------------------
// class vsnTubeLines
//----------------------------------------------------------------
vsnTubeLines::vsnTubeLines(const std::string& nm, const Bool ssm)
  : vfrLines(nm, ssm), m_numRadius(0), m_radiusList(NULL) {
}

vsnTubeLines::~vsnTubeLines() {
  if ( m_radiusList ) {
    DeAllocate(m_radiusList);
    m_radiusList = NULL;
  }
  m_numRadius = 0;
}


bool vsnTubeLines::setRadiusList(const int n, const double* pr) {
  if ( n < 1 || ! pr ) {
    if ( m_radiusList ) {
      DeAllocate(m_radiusList);
      m_radiusList = NULL;
    }
    m_numRadius = 0;
    return true;
  }
  if ( n != m_numRadius ) {
    m_radiusList = (double*)ReAllocate(m_radiusList, sizeof(double)*n);
    if ( ! m_radiusList ) {
      m_numRadius = 0;
      return false;
    }
    m_numRadius = n;
  }
  memcpy(m_radiusList, pr, sizeof(double)*n);

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
  double radbuff[2];
  
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
    if ( index < m_numRadius )
      radbuff[0] = m_radiusList[index];
    index++;
    
    ptbuff[1][0] = _verts[index][0];
    ptbuff[1][1] = _verts[index][1];
    ptbuff[1][2] = _verts[index][2];
    if ( _colorMode == AT_PER_VERTEX )
      if ( index < nColors ) {
	clbuff[1][0] = _colors[index][0]; clbuff[1][1] = _colors[index][1];
	clbuff[1][2] = _colors[index][2]; clbuff[1][3] = _colors[index][3];	
      }
    if ( index < m_numRadius )
      radbuff[1] = m_radiusList[index];
    index++;

    glePolyCone_c4f(2, ptbuff, clbuff, radbuff);
    
    findex++;
  } // end of while(index)
  
  // end display-list definition
  endDispList(DLF_SOLID);
}

//----------------------------------------------------------------
// class vsnTubeLineStrip
//----------------------------------------------------------------
vsnTubeLineStrip::vsnTubeLineStrip(const std::string& nm, const Bool ssm)
  : vfrLineStrip(nm, ssm), m_numRadius(0)
{
}

vsnTubeLineStrip::~vsnTubeLineStrip() {
}


bool vsnTubeLineStrip::alcRadiusList(const int n) {
  if ( n < 1 ) {
    m_radiusList.clear();
    m_numRadius = 0;
    return true;
  }
  if ( n != m_numRadius ) {
    m_radiusList.resize(n);
    m_numRadius = n;
  }

  notice();
  return true;
}


RenderType vsnTubeLineStrip::getRenderMode() const {
  return vfrNode::getRenderMode();
}


void vsnTubeLineStrip::renderSolid() {
  /* None: IGNORE LOOP MODE
   */
  int nvtx = nVerts;
  if ( nvtx > m_numRadius ) nvtx = m_numRadius;
  if ( nvtx < 2 ) return;

  DVec3* ptbuff = (DVec3*)Allocate(sizeof(DVec3)*(nvtx+2));
  if ( ! ptbuff ) return;
  double* prbuff = (double*)Allocate(sizeof(double)*(nvtx+2));
  if ( ! prbuff ) return;
  vector4* pcbuff = NULL;
  bool useColor = (_colorMode == AT_PER_VERTEX || _colorMode == AT_PER_FACE) \
    && nColors >= nvtx;
  if ( useColor )
    pcbuff = (vector4*)Allocate(sizeof(vector4)*(nvtx+2));

  DVec3 dv;
  
  // display-list check
  if ( beginDispList(DLF_SOLID) ) return;

  gleSetJoinStyle(TUBE_NORM_EDGE|TUBE_JN_ANGLE|TUBE_JN_CAP|TUBE_CONTOUR_CLOSED);

  dv[0] = _verts[1][0] - _verts[0][0];
  dv[1] = _verts[1][1] - _verts[0][1];
  dv[2] = _verts[1][2] - _verts[0][2];
  ptbuff[0][0] = _verts[0][0] - dv[0]*EPSF;
  ptbuff[0][1] = _verts[0][1] - dv[1]*EPSF;
  ptbuff[0][2] = _verts[0][2] - dv[2]*EPSF;
  prbuff[0] = m_radiusList[0];
  if ( useColor ) {
    pcbuff[0][0] = _colors[0][0];
    pcbuff[0][1] = _colors[0][1];
    pcbuff[0][2] = _colors[0][2];
    pcbuff[0][3] = _colors[0][3];
  }    

  for ( int i = 0; i < nvtx; i++ ) {
    ptbuff[i+1][0] = _verts[i][0];
    ptbuff[i+1][1] = _verts[i][1];
    ptbuff[i+1][2] = _verts[i][2];
    prbuff[i+1] = m_radiusList[i];
  } // end of for(i)
  if ( useColor ) {
    for ( int i = 0; i < nvtx; i++ ) {
      pcbuff[i+1][0] = _colors[i][0];
      pcbuff[i+1][1] = _colors[i][1];
      pcbuff[i+1][2] = _colors[i][2];
      pcbuff[i+1][3] = _colors[i][3];
    } // end of for(i)
  }

  dv[0] = _verts[nvtx-1][0] - _verts[nvtx-2][0];
  dv[1] = _verts[nvtx-1][1] - _verts[nvtx-2][1];
  dv[2] = _verts[nvtx-1][2] - _verts[nvtx-2][2];
  ptbuff[nvtx+1][0] = _verts[nvtx-1][0] + dv[0]*EPSF;
  ptbuff[nvtx+1][1] = _verts[nvtx-1][1] + dv[1]*EPSF;
  ptbuff[nvtx+1][2] = _verts[nvtx-1][2] + dv[2]*EPSF;
  prbuff[nvtx+1] = m_radiusList[nvtx-1];

  if ( useColor ) {
    pcbuff[nvtx+1][0] = _colors[nvtx-1][0];
    pcbuff[nvtx+1][1] = _colors[nvtx-1][1];
    pcbuff[nvtx+1][2] = _colors[nvtx-1][2];
    pcbuff[nvtx+1][3] = _colors[nvtx-1][3];

    glePolyCone_c4f(nvtx+2, ptbuff, pcbuff, prbuff);

    DeAllocate(pcbuff);
  }
  else {
    glColor4fv(_colors[0]);
    glePolyCone_c4f(nvtx+2, ptbuff, NULL, prbuff);
  }

  DeAllocate(ptbuff);
  DeAllocate(prbuff);
  
  // end display-list definition
  endDispList(DLF_SOLID);
}
