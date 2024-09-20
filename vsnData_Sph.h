//
// vsnData_Sph
//
#ifndef _VSN_DATA_SPH_H_
#define _VSN_DATA_SPH_H_

#include "vsnData_Sv.h"
#include "vsnDataCache.h"
#include "utilMath.h"
#include <vector>


//----------------------------------------------------------------
// class vsnData_Sph
//----------------------------------------------------------------
class vsnData_Sph
  : public vsnData_Sv, public vsnDataCacheMF {
public:
  vsnData_Sph(const std::string& name =std::string(VFR_NONAME),
	      const size_t csz =vsnDataCache::DefaultCacheSize);
  virtual ~vsnData_Sph();

  bool init(const std::deque<std::string>& path_lst);
  bool getOrig(CES::Vec3<float>& orig, const size_t stp =0);
  bool getPitch(CES::Vec3<float>& pitch, const size_t stp =0);

  bool hasRectCoord() const {return m_hasRectCrd;}
  std::string getRectCoordPath() const {return m_rectCrdPath;}
  float** getRectCoord() {return m_rectCoord;}
  size_t* getRectCoordOffset() {return m_rectCrdOfst;}
  bool loadRectCoord(const std::string& path, const size_t* ofst =NULL);

  // from vsnData_Sv
  virtual bool isUniformCoord() const;
  virtual const float* const getCoord(const size_t stp =0);
  virtual const float* const getData(const size_t stp =0);
  virtual std::deque<std::string> getSeqFilePathes() const;
  virtual bool updateBbox(const int stp =0);

  // MHIR append begin
  /**
   * idx の位置のボクセルの値を取得する
   * @param idx ボクセルのインデックス
   * @param didx データのインデックス。trueのデータを返す
   * @author MHIR
   * @since 2.2.0
   */
  CES::Vec3<float> getValue(const CES::Vec3<int>& idx,
			    const std::vector<bool>& didx);

  /**
   * idx の位置のボクセルの値を取得する
   * @param idx ボクセルのインデックス
   * @param vecIdx ベクトルのインデックス
   * @author MHIR
   * @since 2.2.0
   */
  float getValue(const CES::Vec3<int>& idx, int vecIdx);

  /**
   * 位置(pos)からインデックス(idx)とボクセル内の率(rate)を取得する
   * @param pos 位置(in)
   * @param idx インデックス(out)
   * @param rate ボクセル内の位置(out)
   * @author MHIR
   * @since 2.2.0
   */
  void getCellIndex(const CES::Vec3<float>& pos,
		    CES::Vec3<int>* idx,
		    CES::Vec3<float>* rate);

  /**
   * 位置(pos)の値を取得する
   * @param pos 位置(in)
   * @param ddx 取得する値のインデクス
   * @param interpolatMode 内挿のモード
   *                       0 : ゼロ次
   *                       1: trilinear
   *                       2: 外挿1
   *                       3: 外挿2
   * @param exUserilinear 外挿でtrilinearを使うかどうか
   * @author MHIR
   * @since 2.2.0
   */
  std::vector<float> interpolateData(const CES::Vec3<float>& pos,
				     const std::vector<bool>& ddx,
				     int interpolateMode,
				     const CES::Vec3<float>& n,
				     bool exUseTrilinear);
  // MHIR append end

  // from vsnDataObj
  virtual std::string getDataType() const {return std::string("Sph");}
  virtual std::string getFilePath() const;
  virtual bool init(const std::string& path, xmlNodePtr xnp =NULL);
  virtual bool hasMultiFiles() const {return true;}
  virtual std::deque<std::string> getFileList() const {return m_cachePathes;}

  // from vsnDataCache
  virtual bool readin(const size_t stp, unsigned char* pdata);

  // from vsnDataCacheMF
  virtual bool setupMF(const std::deque<std::string>& path_lst,
		       std::deque<std::string>& new_path_lst);

protected:
  std::deque< CES::Vec3<float> > m_origList;
  std::deque< CES::Vec3<float> > m_pitchList;
  mutable volatile int m_crdUpdStp;
  mutable volatile bool m_nanWarned;

  std::deque<std::string> setupLists(const std::deque<std::string>& path_lst);
  bool adjustStepList(const std::deque<std::string>& path_lst,
		      const std::string& baseDir, xmlNodePtr xnp);

  bool m_hasRectCrd;
  float *m_rectCoord[3];
  std::string m_rectCrdPath;
  size_t m_rectCrdOfst[3];
};

#endif // _VSN_DATA_SPH_H_
