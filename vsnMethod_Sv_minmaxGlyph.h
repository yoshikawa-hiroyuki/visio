//
// vsnMethod_Sv_minmaxGlyph
//
#ifndef _VSN_METHOD_SV_MINMAX_GLYPH_H_
#define _VSN_METHOD_SV_MINMAX_GLYPH_H_

#include "vsnMethod_minmaxGlyph.h"
#include "vsnData_Sv.h"


//----------------------------------------------------------------
// class vsnMethod_Sv_minmaxGlyph
//----------------------------------------------------------------
class vsnMethod_Sv_minmaxGlyph : public vsnMethod_minmaxGlyph {
public:
  vsnMethod_Sv_minmaxGlyph(const std::string& nm=std::string(VFR_NONAME))
    : vsnMethod_minmaxGlyph(nm) {}
  virtual ~vsnMethod_Sv_minmaxGlyph() {}

  // from vsnMethod_minmaxGlyph
  virtual bool getMinMaxCurStp(float minmax[2]);

  // from vsnTimeSeriesMethodIF
  virtual bool updateStep(const int stp,
                          const bool force =true, const bool cascade =true);

  // from vsnMethodObj
  SV_GETDATATYPES_DEFINITION;
};

#endif // _VSN_METHOD_SV_MINMAX_GLYPH_H_
