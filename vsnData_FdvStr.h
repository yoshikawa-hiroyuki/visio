//
// vsnData_FdvStr
//
#ifndef _VSN_DATA_FDV_STR_H_
#define _VSN_DATA_FDV_STR_H_

#include "vsnData_Sv.h"
#include "vsnDataCache.h"
#include "utilMath.h"
#include "utilEndian.h"


class vsnData_FdvStr
  : public vsnData_Sv, public vsnDataCacheUF {
public:
  enum DataType {GRID=0, SCALAR=1, VECTOR=2};
  enum ModeType {NODATA=0, STATIC=1, DYNAMIC=2};

  vsnData_FdvStr(const std::string& name =std::string(VFR_NONAME));
  virtual ~vsnData_FdvStr();

  ModeType getDataMode(const DataType dt) const {return m_dataMode[dt];}
  CES::EMatchType getEndianFlag() const {return m_endianFlag;}

  // from vsnData_Sv
  virtual bool isUniformCoord() const;
  virtual const float* const getCoord(const size_t stp =0);
  virtual const float* const getData(const size_t stp =0);
  virtual std::deque<std::string> getSeqFilePathes() const {
    std::deque<std::string> r; r.push_back(getFilePath()); return r;
  }
  virtual bool updateBbox(const int stp =0);

  // from vsnDataObj
  virtual std::string getDataType() const {return std::string("FdvStr");}
  virtual std::string getFilePath() const {return getCachePath();}
  virtual bool init(const std::string& path, xmlNodePtr xnp =NULL);

  // from vsnDataCache
  virtual bool readin(const size_t stp, unsigned char* pdata);

  // from vsnDataCacheUF
  virtual bool setupUF(const std::string& path);

private:
  ModeType          m_dataMode[3];
  CES::EMatchType   m_endianFlag;
  mutable int       m_dtUpdStp;
  mutable int       m_bbUpdStp;
  float            *m_pStaticData[3];
  mutable float    *m_pWkData;

  // static util
  static int CheckStrDataType(const std::string& path);
};

#endif // _VSN_DATA_FDV_STR_H_
