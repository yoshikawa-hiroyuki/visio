//
// vsnMethod_Shape_info
//
#ifndef _VSN_METHOD_SHAPE_INFO_H_
#define _VSN_METHOD_SHAPE_INFO_H_

#include "vsnMethod_info.h"

//----------------------------------------------------------------
// class vsnMethod_Shape_info
//----------------------------------------------------------------
class vsnMethod_Shape_info
  : public vsnMethod_info,
    public vsnTimeSeriesMethodIF {
public:
  vsnMethod_Shape_info(const std::string& nm=std::string(VFR_NONAME));
  virtual ~vsnMethod_Shape_info();

  // from vsnTimeSeriesMethodIF
  virtual bool updateStep(const int stp,
                          const bool force =true, const bool cascade =true);
  // from vsnMethodObj
  virtual std::deque<std::string> getDataTypes() const {
    std::deque<std::string> r; r.push_back(std::string("Shape")); return r;
  }
  virtual bool update(const bool force =true);

protected:
  // from vsnMethod_info
  virtual bool updateRepStr();
};

#endif // _VSN_METHOD_SHAPE_INFO_H_
