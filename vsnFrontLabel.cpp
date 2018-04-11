//
// vsnFrontLabel
//
#include "vsnFrontLabel.h"
#include "vsnMethodObj.h"
#include <assert.h>


//----------------------------------------------------------------
// class vsnFrontLabel
//----------------------------------------------------------------

/* constructors / destructor */

vsnFrontLabel::vsnFrontLabel(const std::string& name)
  : vsnFrontObj(name), m_lbl(NULL), p_mtdObj(NULL)
{
  m_lbl = new vfrHelveticaText(); assert(m_lbl);
  m_lbl->setFontScale(0.05f);
  addChild(m_lbl);
}

vsnFrontLabel::~vsnFrontLabel() {
  // don't delete m_lbl, ~vsnFrontObj will delete it.
}


/* methods */

void vsnFrontLabel::setLabelStr(const std::string& str) {
  if ( m_lbl ) m_lbl->setLetters(str);
}

std::string vsnFrontLabel::getLabelStr() const {
  if ( m_lbl ) return m_lbl->getLetters();
  return std::string("");
}

void vsnFrontLabel::setLabelColor(const vector4 lc) {
  if ( ! lc || ! m_lbl ) return;
  m_lbl->setColor4(0, lc);
  m_lbl->setTransparency(lc[3] < OPAC_THRESH);
}

bool vsnFrontLabel::getLabelColor(vector4 lc) const {
  if ( ! lc || ! m_lbl ) return false;
  memcpy(lc, _colors, sizeof(vector4));
  return true;
}

void vsnFrontLabel::setLabelScale(const float ls) {
  if ( m_lbl ) m_lbl->setFontScale(ls);
}

float vsnFrontLabel::getLabelScale() const {
  if ( m_lbl ) return m_lbl->getFontScale();
  return 0.f;
}

void vsnFrontLabel::setMethodObj(vsnMethodObj* pmtd) const {
  if ( p_mtdObj == pmtd ) return;
  p_mtdObj = pmtd;
}
