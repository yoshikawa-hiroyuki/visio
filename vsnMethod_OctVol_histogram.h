//
// vsnMethod_OctVol_histogram
//
#ifndef _VSN_METHOD_OCTVOL_HISTOGRAM_H_
#define _VSN_METHOD_OCTVOL_HISTOGRAM_H_

#include "vsnMethod_histogram.h"
#include "vsnData_OctVol.h"

//----------------------------------------------------------------
// class vsnMethod_OctVol_histogram
//----------------------------------------------------------------
class vsnMethod_OctVol_histogram
  : public vsnMethod_histogram, public vsnTimeSeriesMethodIF
{
public:
  vsnMethod_OctVol_histogram(const std::string& nm=std::string(VFR_NONAME));
  virtual ~vsnMethod_OctVol_histogram();

  // from vsnMethodObj
  virtual std::deque<std::string> getDataTypes() const {
    std::deque<std::string> r; r.push_back(std::string("OctVol")); return r;
  }
  virtual bool update(const bool force =true);

  // from vsnTimeSeriesMethodIF
  virtual bool updateStep(const int stp,
                          const bool force =true, const bool cascade =true);

private:
  // work members
  vector2*  _values;
};

#endif // _VSN_METHOD_OCTVOL_HISTOGRAM_H_
