//
// vsnMethod_OctVol_streamLines
//
#ifndef _VSN_METHOD_OCTVOL_STREAMLINES_H_
#define _VSN_METHOD_OCTVOL_STREAMLINES_H_

#include "vsnMethod_streamLines.h"
#include "vsnData_OctVol.h"
#include "vsnPtSet.h"


//----------------------------------------------------------------
// class vsnMethod_OctVol_streamLines
//----------------------------------------------------------------
class vsnMethod_OctVol_streamLines
  : public vsnMethod_streamLines {
public:
  vsnMethod_OctVol_streamLines(const std::string& nm=std::string(VFR_NONAME));
  virtual ~vsnMethod_OctVol_streamLines();

  // from vsnTimeSeriesMethodIF
  virtual bool updateStep(const int stp,
                          const bool force =true, const bool cascade =true);

  // from vsnMethodObj
  virtual std::deque<std::string> getDataTypes() const {
    std::deque<std::string> r; r.push_back(std::string("OctVol")); return r;
  }

private:
  mutable std::deque< CES::Vec3<float> >* m_pArry;

  // internal method
  bool calc_streamLine(const CES::Vec3<float>& x0,
		       std::deque< CES::Vec3<float> >& parry);

  bool calc_nextPosRKG(const CES::Vec3<float>& x0, vsnOctTree::Node* pnode,
		       const float dt, CES::Vec3<float>& xn);
};

#endif // _VSN_METHOD_OCTVOL_STREAMLINES_H_
