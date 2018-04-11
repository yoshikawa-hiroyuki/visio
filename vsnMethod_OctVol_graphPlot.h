//
// vsnMethod_OctVol_graphPlot
//
#ifndef _VSN_METHOD_OCTVOL_GRAPHPLOT_H_
#define _VSN_METHOD_OCTVOL_GRAPHPLOT_H_

#include "vsnMethod_graphPlot.h"
#include "vsnData_OctVol.h"


//----------------------------------------------------------------
// class vsnMethod_OctVol_graphPlot
//----------------------------------------------------------------
class vsnMethod_OctVol_graphPlot
  : public vsnMethod_graphPlot, public vsnTimeSeriesMethodIF
{
public:
  vsnMethod_OctVol_graphPlot(const std::string& nm=std::string(VFR_NONAME));
  virtual ~vsnMethod_OctVol_graphPlot();

  // from vsnMethodObj
  virtual std::deque<std::string> getDataTypes() const {
    std::deque<std::string> r; r.push_back(std::string("OctVol")); return r;
  }
  virtual bool update(const bool force =true);

  // from vsnTimeSeriesMethodIF
  virtual bool updateStep(const int stp,
                          const bool force =true, const bool cascade =true);

private:
  vsnOctTree::Node** m_pnodeLst;
  float*             m_sampleData;
};

#endif // _VSN_METHOD_OCTVOL_GRAPHPLOT_H_
