//
// vsnData_DfiSv
//
#ifndef _VSN_DATA_DFISV_H_
#define _VSN_DATA_DFISV_H_

#include "vsnData_Dfi.h"
#include "vsnData_Sv.h"


//----------------------------------------------------------------
// class vsnData_DfiSv
//----------------------------------------------------------------
class vsnData_DfiSv
  : public vsnData_Sv
{
public:
  vsnData_DfiSv(const std::string& name =std::string(VFR_NONAME));
  virtual ~vsnData_DfiSv();

  bool init(vsnData_Dfi* pddfi, const CES::Vec3<size_t>* regionIdx);
  class vsnData_Dfi* getRefDataDfi() {return p_refDataDfi;}
  const CES::Vec3<size_t>* getRegionIdx() const {return m_regionIdx;}

  // from vsnData_Sv
  virtual const float* const getCoord(const size_t stp =0);
  virtual const float* const getData(const size_t stp =0);
  virtual std::deque<std::string> getSeqFilePathes() const {
    std::deque<std::string> r; return r;
  }
  virtual bool setCurrentStepIdx(const size_t stpIdx);
  virtual bool getVectorMaxLen(const CES::Vec3<int>& vidx, float& vml);
  virtual bool checkMinMax(const bool wholeStp =false,
                           const bool progress =true);
  virtual bool updateBbox(const int stp =0);

  // from vsnDataObj
  virtual std::string getDataType() const {return std::string("DfiSv");}
  virtual std::string getFilePath() const {return std::string();}
  virtual bool init(const std::string& path, xmlNodePtr xnp =NULL);
  virtual bool hasMultiFiles() const {return false;}
  virtual std::deque<std::string> getFileList() const {
    std::deque<std::string> r; return r;
  }

  static CES::Vec3<size_t> ParseAttrXYZ(xmlNodePtr xnp);

protected:
  class vsnData_Dfi* p_refDataDfi;
  CES::Vec3<size_t>  m_regionIdx[2];
  float*             m_pData;

  mutable volatile bool m_checkingMinMax;

  static bool ReadinSph(const std::string& path, float* pd, const size_t gc=0);
  static bool ReadinP3dF(const std::string& path, float* pd, const size_t gc=0);
};

#endif // _VSN_DATA_DFISV_H_
