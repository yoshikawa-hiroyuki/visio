//
// vsnMethod_OctVol_plotScalar
//
#ifndef _VSN_METHOD_OCTVOL_PLOTSCALAR_H_
#define _VSN_METHOD_OCTVOL_PLOTSCALAR_H_

#include "vsnMethodObj.h"
#include "vsnMethod_sampler.h"
#include "vsnMethod_plotScalar.h"


//----------------------------------------------------------------
// class vsnMethod_OctVol_plotScalar
//----------------------------------------------------------------
class vsnMethod_OctVol_plotScalar
  : public vsnMethod_plotScalar {
public:
  vsnMethod_OctVol_plotScalar(const std::string& nm=std::string(VFR_NONAME));
  virtual ~vsnMethod_OctVol_plotScalar();

  // from vsnMethodObj
  virtual std::deque<std::string> getDataTypes() const {
    std::deque<std::string> r; r.push_back(std::string("OctVol")); return r;
  }

  // from vsnTimeSeriesMethodIF
  virtual bool updateStep(const int stp,
                          const bool force =true, const bool cascade =true);
};

//----------------------------------------------------------------
// class vsnMesh2D_EX
//----------------------------------------------------------------
#include <vector>
class vsnMesh2D_EX : public vfrMesh2D {
public:
  vsnMesh2D_EX(const std::string& nm=std::string(VFR_NONAME),
	       const Bool ssm=FALSE) : vfrMesh2D(nm, ssm) {}
  vsnMesh2D_EX(const int m, const int n,
	       const std::string& nm=std::string(VFR_NONAME),
	       const Bool ssm=FALSE) : vfrMesh2D(m, n, nm, ssm) {}
  vsnMesh2D_EX(const Point2 msz,
	       const std::string& nm=std::string(VFR_NONAME),
	       const Bool ssm=FALSE) : vfrMesh2D(msz, nm, ssm) {}

  /**
   * 必要であればマスク情報をクリアする
   * マスク情報が設定されていなければ必要ないので、クリアする
   * @author MHIR
   * @date 2009-05-29
   */
  void ClearMaskIfNotNeed(void) {
    bool need = false;
    for (size_t i = 0; i < m_mask.size(); ++i) 
      if (m_mask[i]) { need = true; break; }
    if (! need) m_mask.clear();
  }

  /**
   * マスク情報の初期化
   * @author MHIR
   * @date 2009-05-29
   */
  void InitMask(void) {
    m_mask.clear();
    // 領域の確保, マスク無しで初期化
    m_mask.resize(_meshSize.x * _meshSize.y, false);
  }

  /**
   * マスク情報を設定する
   * @author MHIR
   * @date 2009-05-29
   */
  void SetMask(size_t idx) { m_mask[idx] = true; }

protected:
  /**
   * true の頂点は表示しない
   * false の頂点は表示する
   * @author MHIR
   * @date 2009-05-29
   */
  std::vector<bool> m_mask;

  virtual void renderSolid();
};

#endif // _VSN_METHOD_OCTVOL_PLOTSCALAR_H_
