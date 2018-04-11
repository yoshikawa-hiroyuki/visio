//
// vsnMethod_OctVol_plotContour
//
#ifndef _VSN_METHOD_OCTVOL_PLOTCONTOUR_H_
#define _VSN_METHOD_OCTVOL_PLOTCONTOUR_H_

#include "vsnMethodObj.h"
#include "vsnMethod_sampler.h"
#include "vsnMethod_plotContour.h"


//----------------------------------------------------------------
// class vsnMethod_OctVol_plotContour
//----------------------------------------------------------------
class vsnMethod_OctVol_plotContour
  : public vsnMethod_plotContour {
public:
  vsnMethod_OctVol_plotContour(const std::string& nm=std::string(VFR_NONAME));
  virtual ~vsnMethod_OctVol_plotContour();

  // from vsnMethodObj
  virtual std::deque<std::string> getDataTypes() const {
    std::deque<std::string> r; r.push_back(std::string("OctVol")); return r;
  }

  // from vsnTimeSeriesMethodIF
  virtual bool updateStep(const int stp,
                          const bool force =true, const bool cascade =true);
};

#endif // _VSN_METHOD_OCTVOL_PLOTCONTOUR_H_
