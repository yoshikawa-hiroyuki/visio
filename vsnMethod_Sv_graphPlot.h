//
// vsnMethod_Sv_graphPlot
//
#ifndef _VSN_METHOD_SV_GRAPHPLOT_H_
#define _VSN_METHOD_SV_GRAPHPLOT_H_

#include "vsnMethod_graphPlot.h"
#include "vsnData_Sv.h"
#include "vsnGridUtilSv.h"


//----------------------------------------------------------------
// class vsnMethod_Sv_graphPlot
//----------------------------------------------------------------
class vsnMethod_Sv_graphPlot
  : public vsnMethod_graphPlot, public vsnTimeSeriesMethodIF
{
public:
  vsnMethod_Sv_graphPlot(const std::string& nm=std::string(VFR_NONAME));
  virtual ~vsnMethod_Sv_graphPlot();

  // from vsnMethod_graphPlot
  virtual bool exportCsv(const std::string& path);
  
  // from vsnMethodObj
  SV_GETDATATYPES_DEFINITION;
  virtual bool update(const bool force =true);

  // from vsnTimeSeriesMethodIF
  virtual bool updateStep(const int stp,
                          const bool force =true, const bool cascade =true);

private:
  // work members
  float*        _values;
  vsnGridUtilSv _gus;
};

#endif // _VSN_METHOD_SV_GRAPHPLOT_H_
