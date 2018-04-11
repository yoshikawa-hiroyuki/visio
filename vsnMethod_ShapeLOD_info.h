//
// vsnMethod_Shape_info
//
#ifndef _VSN_METHOD_SHAPE_LOD_INFO_H_
#define _VSN_METHOD_SHAPE_LOD_INFO_H_

#include "vsnMethod_info.h"

//----------------------------------------------------------------
// class vsnMethod_ShapeLOD_info
//----------------------------------------------------------------
class vsnMethod_ShapeLOD_info
  : public vsnMethod_info {
public:
  vsnMethod_ShapeLOD_info(const std::string& nm=std::string(VFR_NONAME));
  virtual ~vsnMethod_ShapeLOD_info();

  // from vsnMethodObj
  virtual std::deque<std::string> getDataTypes() const {
    std::deque<std::string> r; r.push_back(std::string("ShapeLOD"));
    return r;
  }
  virtual bool update(const bool force =true);

protected:
  // from vsnMethod_info
  virtual bool updateRepStr();
};

#endif // _VSN_METHOD_SHAPE_LOD_INFO_H_
