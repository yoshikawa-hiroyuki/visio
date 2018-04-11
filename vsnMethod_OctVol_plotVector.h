//
// vsnMethod_OctVol_plotVector
//
#ifndef _VSN_METHOD_OCTVOL_PLOTVECTOR_H_
#define _VSN_METHOD_OCTVOL_PLOTVECTOR_H_

#include "vsnMethodObj.h"
#include "vsnMethod_sampler.h"
#include "vsnMethod_plotVector.h"


//----------------------------------------------------------------
// class vsnMethod_OctVol_plotVector
//----------------------------------------------------------------
class vsnMethod_OctVol_plotVector
  : public vsnMethod_plotVector {
public:
  vsnMethod_OctVol_plotVector(const std::string& nm=std::string(VFR_NONAME));
  virtual ~vsnMethod_OctVol_plotVector();

  // from vsnTimeSeriesMethodIF
  virtual bool updateStep(const int stp,
                          const bool force =true, const bool cascade =true);

  // from vsnMethodObj
  virtual std::deque<std::string> getDataTypes() const {
    std::deque<std::string> r; r.push_back(std::string("OctVol")); return r;
  }
};

#endif // _VSN_METHOD_OCTVOL_PLOTVECTOR_H_
