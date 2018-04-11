//
// vsnFrontLabel
//
#ifndef _VSN_FRONT_LABEL_H_
#define _VSN_FRONT_LABEL_H_

#include "vsnFrontObj.h"
#include "vfrHelveticaText.h"

class vsnMethodObj;


//----------------------------------------------------------------
// class vsnFrontLabel
//----------------------------------------------------------------
class vsnFrontLabel : public vsnFrontObj {
public:
  vsnFrontLabel(const std::string& name =std::string(VFR_NONAME));
  virtual ~vsnFrontLabel();

  // methods
  void setLabelStr(const std::string& str);
  std::string getLabelStr() const;

  void setLabelColor(const vector4 lc);
  bool getLabelColor(vector4 lc) const;

  void setLabelScale(const float ls);
  float getLabelScale() const;

  void setMethodObj(vsnMethodObj* pmtd) const;
  vsnMethodObj* getMethodObj() const {return p_mtdObj;}

protected:
  vfrHelveticaText*     m_lbl;
  mutable vsnMethodObj* p_mtdObj;
};

#endif // _VSN_FRONT_LABEL_H_
