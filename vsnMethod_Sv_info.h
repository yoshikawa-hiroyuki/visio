//
// vsnMethod_Sv_info
//
#ifndef _VSN_METHOD_SV_INFO_H_
#define _VSN_METHOD_SV_INFO_H_

#include "vsnMethod_info.h"
#include "vsnData_Sv.h"


//----------------------------------------------------------------
// class vsnMethod_Sv_info
//----------------------------------------------------------------
class vsnMethod_Sv_info
  : public vsnMethod_info,
    public vsnTimeSeriesMethodIF
{
public:
  vsnMethod_Sv_info(const std::string& nm=std::string(VFR_NONAME));
  virtual ~vsnMethod_Sv_info();

  // from vsnTimeSeriesMethodIF
  virtual bool updateStep(const int stp,
                          const bool force =true, const bool cascade =true);
  // from vsnMethodObj
  virtual bool update(const bool force =true);
  SV_GETDATATYPES_DEFINITION;

protected:
  // from vsnMethod_info
  virtual bool updateRepStr();
};

#endif // _VSN_METHOD_SV_INFO_H_
