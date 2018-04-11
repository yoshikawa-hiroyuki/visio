//
// vsnData_Sv
//
#ifndef _VSN_DATA_SV_H_
#define _VSN_DATA_SV_H_

#include "vsnDataObj.h"
#include "utilMath.h"

// PGSQL driven

#define SV_GETDATATYPES_DEFINITION \
  virtual std::deque<std::string> getDataTypes() const { \
    std::deque<std::string> r; \
    r.push_back(std::string("Sv")); \
    r.push_back(std::string("Sph")); \
    r.push_back(std::string("P3dF")); \
    r.push_back(std::string("FdvStr")); \
    r.push_back(std::string("DfiSv")); \
    r.push_back(std::string("SphPEX")); \
    r.push_back(std::string("SvSQL")); \
    return r; \
  }

//----------------------------------------------------------------
// class vsnData_Sv
//----------------------------------------------------------------
class vsnData_Sv
  : public vsnDataObj,
    public vsnTimeSeriesDataIF,
    public vsnNumericalDataIF
{
public:
  enum MaskType {MASK_None =0, MASK_VolRate, MASK_Medium};
  enum SbxDataType {SBX_BYTE =1, SBX_SHORT =2, SBX_INT =4};
  enum SbxGridType {SBX_REGULAR =1, SBX_COLLOCATE =2,
		    SBX_STAGGERED1 =3, SBX_STAGGERED2 =4};

  vsnData_Sv(const std::string& name =std::string(VFR_NONAME));
  virtual ~vsnData_Sv();

  const CES::Vec3<size_t>& getDims() const {return m_dims;}

  const unsigned char* const getMask() {return m_mask;}
  bool setMaskSVX(const std::string& mskPath,
		  const MaskType mskType =MASK_None,
		  const float* vrRange =NULL, const int medId =0,
		  const bool chkMinMax =false, const int* pOfst =NULL);
  bool setMaskSBX(const std::string& mskPath,
		  const MaskType mskType =MASK_None,
		  const float* vrRange =NULL, const int medId =0,
		  const bool chkMinMax =false, const int* pOfst =NULL);
  bool setMask(const bool apply, const int x1, const int x2, 
	       const int y1, const int y2, const int z1, const int z2,
	       const bool chkMinMax =false);

  virtual bool isUniformCoord() const {return true;}
  virtual const float* const getCoord(const size_t stp =0);
  virtual const float* const getData(const size_t stp =0) =0;
  virtual std::deque<std::string> getSeqFilePathes() const =0;

  // from vsnNumericalDataIF
  virtual bool getVectorMaxLen(const CES::Vec3<int>& vidx, float& vml);
  virtual bool checkMinMax(const bool wholeStp =false,
			   const bool progress =true);

  // from vsnDataObj
  virtual std::string getDataType() const {return std::string("Sv");}
  virtual std::deque<std::string> getSupportMethodList() const;
  virtual vsnMethodObj* addNewMethod(const std::string& mtype,
				     const std::string& mname =std::string());
  // *** need to implement in derrivered class ***
  // virtual bool init(const std::string& path, xmlNodePtr xnp =NULL);

  // from vsnTimeSeriesDataIF
  virtual size_t getNumSteps() const {return m_numStps;}
  virtual bool   getStepIdx(const int NSTEP, size_t& stp) const;
  virtual bool   getTimeStepNo(const size_t stpIdx, int& tsNo) const;
  virtual float  getTime(const size_t stp) const;
  virtual bool   setCurrentStepIdx(const size_t stpIdx);

  // override Bbox methods
  virtual void generateBbox() {}
  virtual void generateBbox(CES::Vec3<float>&) {}
  virtual bool updateBbox(const int stp =0);

protected:
  size_t              m_numStps;
  std::deque<StpUnit> m_stpList; // [m_numSteps]
  CES::Vec3<size_t>   m_dims;
  unsigned char*      m_mask;
  void*               m_maskWk;
  bool                m_minMaxMask;

  // may be set in readin() if error has occurred
  mutable std::string m_readinErrMsg;

  // static members
  static std::deque<std::string> s_methodList;
};

#endif // _VSN_DATA_SV_H_
