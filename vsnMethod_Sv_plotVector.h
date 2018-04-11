//
// vsnMethod_Sv_plotVector
//
#ifndef _VSN_METHOD_SV_PLOTVECTOR_H_
#define _VSN_METHOD_SV_PLOTVECTOR_H_

#include "vsnMethodObj.h"
#include "vsnMethod_sampler.h"
#include "vsnMethod_plotVector.h"
#include "vsnData_Sv.h"
#include "vsnGridUtilSv.h"


//----------------------------------------------------------------
// class vsnMethod_Sv_plotVector
//----------------------------------------------------------------
class vsnMethod_Sv_plotVector
  : public vsnMethod_plotVector {
public:
  vsnMethod_Sv_plotVector(const std::string& nm=std::string(VFR_NONAME));
  virtual ~vsnMethod_Sv_plotVector();

  // from vsnTimeSeriesMethodIF
  virtual bool updateStep(const int stp,
                          const bool force =true, const bool cascade =true);

  // from vsnMethodObj
  SV_GETDATATYPES_DEFINITION;

private:
  vsnGridUtilSv _gus;
};

#endif // _VSN_METHOD_SV_PLOTVECTOR_H_
