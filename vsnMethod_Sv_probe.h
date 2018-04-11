//
// vsnMethod_Sv_probe
//
#ifndef _VSN_METHOD_SV_PROBE_H_
#define _VSN_METHOD_SV_PROBE_H_

#include "vsnMethod_probe.h"
#include "vsnData_Sv.h"
#include "vsnGridUtilSv.h"


//----------------------------------------------------------------
// class vsnMethod_Sv_probe
//----------------------------------------------------------------
class vsnMethod_Sv_probe : public vsnMethod_probe {
public:
  vsnMethod_Sv_probe(const std::string& nm=std::string(VFR_NONAME));
  virtual ~vsnMethod_Sv_probe();

  // from vsnTimeSeriesMethodIF
  virtual bool updateStep(const int stp,
                          const bool force =true, const bool cascade =true);

  // from vsnMethodObj
  SV_GETDATATYPES_DEFINITION;

private:
  vsnGridUtilSv _gus;
};

#endif // _VSN_METHOD_SV_PROBE_H_
