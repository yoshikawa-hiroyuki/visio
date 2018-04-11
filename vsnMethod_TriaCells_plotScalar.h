//
// vsnMethod_TriaCellsv_plotScalar
//
#ifndef _VSN_METHOD_TRIACELLS_PLOTSCALAR_H_
#define _VSN_METHOD_TRIACELLS_PLOTSCALAR_H_

#include "vsnMethodObj.h"
#include "vsnMethod_sampler.h"
#include "vsnMethod_plotScalar.h"
#include "vsnData_TriaCells.h"
#include "vfrIndexPolygons.h"


//----------------------------------------------------------------
// class vsnMethod_TriaCells_plotScalar
//----------------------------------------------------------------
class vsnMethod_TriaCells_plotScalar
  : public vsnMethod_plotScalar {
public:
  vsnMethod_TriaCells_plotScalar(const std::string& nm=std::string(VFR_NONAME));
  virtual ~vsnMethod_TriaCells_plotScalar();

  // from vsnMethodObj
  virtual std::deque<std::string> getDataTypes() const {
    std::deque<std::string> r;
    r.push_back(std::string("TriaCells"));
    return r;
  }

  // from vsnTimeSeriesMethodIF
  virtual bool updateStep(const int stp,
                          const bool force =true, const bool cascade =true);

private:
  vfrIndexPolygons* m_shape;
};

#endif // _VSN_METHOD_TRIACELLS_PLOTSCALAR_H_
