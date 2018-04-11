//
// vsnMethod_Sv_plotScalar
//
#ifndef _VSN_METHOD_SV_PLOTSCALAR_H_
#define _VSN_METHOD_SV_PLOTSCALAR_H_

#include "vsnMethodObj.h"
#include "vsnMethod_sampler.h"
#include "vsnMethod_plotScalar.h"
#include "vsnData_Sv.h"
#include "vsnGridUtilSv.h"


//----------------------------------------------------------------
// class vsnMethod_Sv_plotScalar
//----------------------------------------------------------------
class vsnMethod_Sv_plotScalar
  : public vsnMethod_plotScalar {
public:
  vsnMethod_Sv_plotScalar(const std::string& nm=std::string(VFR_NONAME));
  virtual ~vsnMethod_Sv_plotScalar();

  // from vsnMethodObj
  SV_GETDATATYPES_DEFINITION;

  // from vsnTimeSeriesMethodIF
  virtual bool updateStep(const int stp,
                          const bool force =true, const bool cascade =true);

private:
  vsnGridUtilSv _gus;
};

#endif // _VSN_METHOD_SV_PLOTSCALAR_H_
