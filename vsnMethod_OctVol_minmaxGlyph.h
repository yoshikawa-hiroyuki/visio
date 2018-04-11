//
// vsnMethod_OctVol_minmaxGlyph
//
#ifndef _VSN_METHOD_OCTVOL_MINMAX_GLYPH_H_
#define _VSN_METHOD_OCTVOL_MINMAX_GLYPH_H_

#include "vsnMethod_minmaxGlyph.h"
#include "vsnData_OctVol.h"


//----------------------------------------------------------------
// class vsnMethod_OctVol_minmaxGlyph
//----------------------------------------------------------------
class vsnMethod_OctVol_minmaxGlyph : public vsnMethod_minmaxGlyph {
public:
  vsnMethod_OctVol_minmaxGlyph(const std::string& nm=std::string(VFR_NONAME))
    : vsnMethod_minmaxGlyph(nm) {}
  virtual ~vsnMethod_OctVol_minmaxGlyph() {}

  // from vsnMethod_minmaxGlyph
  virtual bool getMinMaxCurStp(float minmax[2]);

  // from vsnTimeSeriesMethodIF
  virtual bool updateStep(const int stp,
                          const bool force =true, const bool cascade =true);

  // from vsnMethodObj
  virtual std::deque<std::string> getDataTypes() const {
    std::deque<std::string> r; r.push_back(std::string("OctVol")); return r;
  }

};

#endif // _VSN_METHOD_OCTVOL_MINMAX_GLYPH_H_
