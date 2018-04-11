//
// vsnData_SphPEX
//
#ifndef _VSN_DATA_SPH_PEX_H_
#define _VSN_DATA_SPH_PEX_H_

#include "vsnData_Sph.h"


//----------------------------------------------------------------
// class vsnData_SphPEX
//----------------------------------------------------------------
class vsnData_SphPEX
  : public vsnData_Sv
{
public:
  vsnData_SphPEX(const std::string& name =std::string(VFR_NONAME));
  virtual ~vsnData_SphPEX();

  size_t getNumParamIdx() const {return m_pDataLst.size();}
  std::deque<vsnData_Sph*> getDataList() {return m_pDataLst;}
  size_t getCurrentParamIdx() const {return m_currentParamIdx;}
  bool setCurrentParamIdx(const size_t idx);

  std::string getParamName() const {return m_paramName;}
  bool getParams(float params[2]) const;
  bool getCurrentParamValue(float& pval) const;

  // equivarent to Sph interface
  bool getOrig(CES::Vec3<float>& orig, const size_t stp =0);
  bool getPitch(CES::Vec3<float>& pitch, const size_t stp =0);
  bool hasRectCoord() const;
  std::string getRectCoordPath() const;
  float** getRectCoord();
  size_t* getRectCoordOffset();
  bool loadRectCoord(const std::string& path, const size_t* ofst =NULL);

  // from vsnData_Sv
  virtual bool isUniformCoord() const;
  virtual const float* const getCoord(const size_t stp =0);
  virtual const float* const getData(const size_t stp =0);
  virtual std::deque<std::string> getSeqFilePathes() const;
  virtual bool updateBbox(const int stp =0);
  virtual bool getMinMax(const size_t n, float minmax[2]) const;
  virtual bool getVectorMaxLen(const CES::Vec3<int>& vidx, float& vml);

  // from vsnDataObj
  virtual std::string getDataType() const {return std::string("SphPEX");}
  virtual std::string getFilePath() const;
  virtual bool init(const std::string& path, xmlNodePtr xnp =NULL);
  virtual bool hasMultiFiles() const {return true;}
  virtual std::deque<std::string> getFileList() const;
  virtual std::deque<std::string> getSupportMethodList() const;
  virtual vsnMethodObj* addNewMethod(const std::string& mtype,
                                     const std::string& mname =std::string());

  // from vsnIoObject
  virtual bool outputXML(std::ostream& os, const size_t ts =0);

protected:
  std::deque<vsnData_Sph*> m_pDataLst;
  size_t m_currentParamIdx;

  std::string m_paramName;
  float m_param0, m_paramSkip;
};

#endif // _VSN_DATA_SPH_PEX_H_
