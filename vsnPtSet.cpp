//
// vsnPtSet
//
#include "vsnPtSet.h"


/* constructors / destructor */

vsnPtSet::vsnPtSet(const std::string& name)
  : vfrNode(name, FALSE)
{
}

vsnPtSet::~vsnPtSet() {
}


/* vfrNode methods */

void vsnPtSet::renderPoint() {
  // display-list check
  if ( beginDispList(DLF_POINT) ) return;

  Bool auxMode = useAuxPointColor();
  if ( _colorMode == AT_WHOLE && !auxMode )
    glColor4fv(_colors[0]);

  if ( nIndices > 0 ) {
    register int index = 0;
    for ( register int l = 0; l < nIndices; l++ ) {
      if ( ! auxMode && _colorMode == AT_PER_FACE && l < nColors )
        glColor4fv(_colors[l]);
      glBegin(GL_POINTS);
      for ( register int lindex = 0; lindex < _indices[l]; lindex++ ) {
        if ( index < nVerts ) {
          if ( ! auxMode && _colorMode == AT_PER_VERTEX && index < nColors )
            glColor4fv(_colors[index]);
          glVertex3fv(_verts[index++]);
        }
      }
      glEnd();
    }
  }
  else {
    glBegin(GL_POINTS);
    for ( register int index = 0; index < nVerts; index++ ) {
      if ( ! auxMode && _colorMode == AT_PER_VERTEX && index < nColors )
        glColor4fv(_colors[index]);
      glVertex3fv(_verts[index]);
    }
    glEnd();
  }

  // end display-list definition
  endDispList(DLF_POINT);
}

void vsnPtSet::renderWire() {
  // display-list check
  if ( beginDispList(DLF_WIRE) ) return;

  Bool auxMode = useAuxLineColor();
  if ( ! auxMode && _colorMode == AT_WHOLE )
    glColor4fv(_colors[0]);

  if ( nIndices > 0 ) {
    register int index = 0;
    for ( register int l = 0; l < nIndices; l++ ) {
      if ( ! auxMode && _colorMode == AT_PER_FACE && l < nColors )
        glColor4fv(_colors[l]);
      if ( l < nNormals )
	glLineWidth(_normals[l][0]);
      glBegin(GL_LINE_STRIP);
      for ( register int lindex = 0; lindex < _indices[l]; lindex++ ) {
        if ( index < nVerts ) {
          if ( ! auxMode && _colorMode == AT_PER_VERTEX && index < nColors )
            glColor4fv(_colors[index]);
          glVertex3fv(_verts[index++]);
        }
      }
      glEnd();
    }
  }
  else {
    glBegin(GL_LINE_STRIP);
    for ( register int index = 0; index < nVerts; index++ ) {
      if ( ! auxMode && _colorMode == AT_PER_VERTEX && index < nColors )
        glColor4fv(_colors[index]);
      glVertex3fv(_verts[index++]);
    }
    glEnd();
  }

  // end display-list definition
  endDispList(DLF_WIRE);
}

