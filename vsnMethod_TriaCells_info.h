//
// vsnMethod_TriaCells_info
//
#ifndef _VSN_METHOD_TRIACELLS_INFO_H_
#define _VSN_METHOD_TRIACELLS_INFO_H_

#include "vsnMethod_info.h"

//----------------------------------------------------------------
// class vsnMethod_TriaCells_info
//----------------------------------------------------------------
class vsnMethod_TriaCells_info
  : public vsnMethod_info,
    public vsnTimeSeriesMethodIF {
public:
  vsnMethod_TriaCells_info(const std::string& nm=std::string(VFR_NONAME));
  virtual ~vsnMethod_TriaCells_info();

  // from vsnTimeSeriesMethodIF
  virtual bool updateStep(const int stp,
                          const bool force =true, const bool cascade =true);
  // from vsnMethodObj
  virtual std::deque<std::string> getDataTypes() const {
    std::deque<std::string> r; r.push_back(std::string("TriaCells"));
    return r;
  }
  virtual bool update(const bool force =true);

protected:
  // from vsnMethod_info
  virtual bool updateRepStr();
};

#endif // _VSN_METHOD_TRIACELLS_INFO_H_
