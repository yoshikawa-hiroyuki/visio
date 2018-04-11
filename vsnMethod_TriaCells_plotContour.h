//
// vsnMethod_TriaCells_plotContour
//
#ifndef _VSN_METHOD_TRIACELLS_PLOTCONTOUR_H_
#define _VSN_METHOD_TRIACELLS_PLOTCONTOUR_H_

#include "vsnMethodObj.h"
#include "vsnMethod_sampler.h"
#include "vsnMethod_plotContour.h"
#include "vsnData_TriaCells.h"


//----------------------------------------------------------------
// class vsnMethod_TriaCells_plotContour
//----------------------------------------------------------------
class vsnMethod_TriaCells_plotContour
  : public vsnMethod_plotContour {
public:
  vsnMethod_TriaCells_plotContour(const std::string& nm
				  = std::string(VFR_NONAME));
  virtual ~vsnMethod_TriaCells_plotContour();

  // from vsnMethodObj
  virtual std::deque<std::string> getDataTypes() const {
    std::deque<std::string> r;
    r.push_back(std::string("TriaCells"));
    return r;
  }

  // from vsnTimeSeriesMethodIF
  virtual bool updateStep(const int stp,
                          const bool force =true, const bool cascade =true);
};

#endif // _VSN_METHOD_TRIACELLS_PLOTCONTOUR_H_
