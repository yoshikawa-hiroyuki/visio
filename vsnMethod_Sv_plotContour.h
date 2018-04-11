//
// vsnMethod_Sv_plotContour
//
#ifndef _VSN_METHOD_SV_PLOTCONTOUR_H_
#define _VSN_METHOD_SV_PLOTCONTOUR_H_

#include "vsnMethodObj.h"
#include "vsnMethod_sampler.h"
#include "vsnMethod_plotContour.h"
#include "vsnData_Sv.h"
#include "vsnGridUtilSv.h"


//----------------------------------------------------------------
// class vsnMethod_Sv_plotContour
//----------------------------------------------------------------
class vsnMethod_Sv_plotContour
  : public vsnMethod_plotContour {
public:
  vsnMethod_Sv_plotContour(const std::string& nm=std::string(VFR_NONAME));
  virtual ~vsnMethod_Sv_plotContour();

  // from vsnMethodObj
  SV_GETDATATYPES_DEFINITION;

  // from vsnTimeSeriesMethodIF
  virtual bool updateStep(const int stp,
                          const bool force =true, const bool cascade =true);

private:
  vsnGridUtilSv _gus;
};

#endif // _VSN_METHOD_SV_PLOTCONTOUR_H_
