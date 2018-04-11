//
// vsnMethod_Sv_histogram
//
#ifndef _VSN_METHOD_SV_HISTOGRAM_H_
#define _VSN_METHOD_SV_HISTOGRAM_H_

#include "vsnMethod_histogram.h"
#include "vsnData_Sv.h"

//----------------------------------------------------------------
// class vsnMethod_Sv_histogram
//----------------------------------------------------------------
class vsnMethod_Sv_histogram
  : public vsnMethod_histogram, public vsnTimeSeriesMethodIF
{
public:
  vsnMethod_Sv_histogram(const std::string& nm=std::string(VFR_NONAME));
  virtual ~vsnMethod_Sv_histogram();

  // from vsnMethodObj
  SV_GETDATATYPES_DEFINITION;
  virtual bool update(const bool force =true);

  // from vsnTimeSeriesMethodIF
  virtual bool updateStep(const int stp,
                          const bool force =true, const bool cascade =true);

private:
  // work members
  vector2*  _values;
};

#endif // _VSN_METHOD_SV_HISTOGRAM_H_
