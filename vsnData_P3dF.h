//
// vsnData_P3dF
//
#ifndef _VSN_DATA_P3DF_H_
#define _VSN_DATA_P3DF_H_

#include "vsnData_Sv.h"
#include "vsnDataCache.h"
#include "utilMath.h"
#include "utilEndian.h"
#include <vector>


//----------------------------------------------------------------
// class vsnData_P3dF
//----------------------------------------------------------------
class vsnData_P3dF
  : public vsnData_Sv, public vsnDataCacheMF {
public:
  enum GridType {P3df_Invalid=0, P3dF_SingleGrid, P3dF_MultiGrid};

  vsnData_P3dF(const std::string& name =std::string(VFR_NONAME),
	       const size_t csz =vsnDataCache::DefaultCacheSize);
  virtual ~vsnData_P3dF();

  bool init(const std::deque<std::string>& path_lst);
  bool getOrig(CES::Vec3<float>& orig, const size_t stp =0);
  bool getPitch(CES::Vec3<float>& pitch, const size_t stp =0);

  // from vsnData_Sv
  virtual bool isUniformCoord() const;
  virtual const float* const getCoord(const size_t stp =0);
  virtual const float* const getData(const size_t stp =0);
  virtual std::deque<std::string> getSeqFilePathes() const;
  virtual bool updateBbox(const int stp =0);

  // from vsnDataObj
  virtual std::string getDataType() const {return std::string("P3dF");}
  virtual std::string getFilePath() const;
  virtual bool init(const std::string& path, xmlNodePtr xnp =NULL);
  virtual bool hasMultiFiles() const {return true;}
  virtual std::deque<std::string> getFileList() const {return m_cachePathes;}

  // from vsnDataCache
  virtual bool readin(const size_t stp, unsigned char* pdata);

  // from vsnDataCacheMF
  virtual bool setupMF(const std::deque<std::string>& path_lst,
		       std::deque<std::string>& new_path_lst);

  static bool CheckFileType(FILE* fp, GridType& gt, CES::EMatchType& et);

protected:
  std::deque< CES::Vec3<float> > m_origList;
  std::deque< CES::Vec3<float> > m_pitchList;
  mutable volatile int m_crdUpdStp;
  mutable volatile bool m_nanWarned;

  std::deque<std::string> setupLists(const std::deque<std::string>& path_lst);
  bool adjustStepList(const std::deque<std::string>& path_lst,
		      const std::string& baseDir, xmlNodePtr xnp);
};

#endif // _VSN_DATA_P3DF_H_
