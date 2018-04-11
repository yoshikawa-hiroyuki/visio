//
// vsnMethod_Sv_streamLines
//
#ifndef _VSN_METHOD_SV_STREAMLINES_H_
#define _VSN_METHOD_SV_STREAMLINES_H_

#include "vsnMethod_streamLines.h"
#include "vsnData_Sv.h"
#include "vsnPtSet.h"
#include "vsnGridUtilSv.h"


//----------------------------------------------------------------
// class vsnMethod_Sv_streamLines
//----------------------------------------------------------------
class vsnMethod_Sv_streamLines
  : public vsnMethod_streamLines {
public:
  vsnMethod_Sv_streamLines(const std::string& nm=std::string(VFR_NONAME));
  virtual ~vsnMethod_Sv_streamLines();

  // from vsnTimeSeriesMethodIF
  virtual bool updateStep(const int stp,
                          const bool force =true, const bool cascade =true);

  // from vsnMethodObj
  SV_GETDATATYPES_DEFINITION;

private:
  // point array of lines
  struct PTarray {
    int      cnt;
    vector3* x;
    PTarray() : cnt(0), x(NULL) {}
    ~PTarray() {if(x) DeAllocate(x);}
  };

  // work members
  vsnGridUtilSv _gus;
  bool          _bound[3];
  DVec3*        _x0;
  PTarray*      _array;

  // internal method
  double GetIntegrand(const DVec4 x_i, DVec3 func);
  int RKG(const double t_step, DVec4 x_i); // Runge-Kutta-Gill
  int StreamLine(DVec4 x_i, const int l);
};

#endif // _VSN_METHOD_SV_STREAMLINES_H_
