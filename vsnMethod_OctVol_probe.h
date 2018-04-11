//
// vsnMethod_OctVol_probe
//
#ifndef _VSN_METHOD_OCTVOL_PROBE_H_
#define _VSN_METHOD_OCTVOL_PROBE_H_

#include "vsnMethod_probe.h"


//----------------------------------------------------------------
// class vsnMethod_OctVol_probe
//----------------------------------------------------------------
class vsnMethod_OctVol_probe : public vsnMethod_probe {
public:
  vsnMethod_OctVol_probe(const std::string& nm=std::string(VFR_NONAME));
  virtual ~vsnMethod_OctVol_probe();

  // from vsnTimeSeriesMethodIF
  virtual bool updateStep(const int stp,
                          const bool force =true, const bool cascade =true);

  // from vsnMethodObj
  virtual std::deque<std::string> getDataTypes() const {
    std::deque<std::string> r; r.push_back(std::string("OctVol")); return r;
  }

private:
};

#endif // _VSN_METHOD_OCTVOL_PROBE_H_
