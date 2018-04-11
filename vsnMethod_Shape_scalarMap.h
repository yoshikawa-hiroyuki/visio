//
// vsnMethod_Shape_scalarMap
//
#ifndef _VSN_METHOD_SV_SCALAR_MAP_HPP_
#define _VSN_METHOD_SV_SCALAR_MAP_HPP_

#include <vector>

#include "wx/defs.h"
#include "wx/textctrl.h"
#include "wx/listbox.h"
#include "wx/button.h"
#include "wx/checklst.h"
#include "wx/checkbox.h"

#include "vsnMethodObj.h"
#include "vfrTriangles.h"

class vsnData_Sph;

/**
 * @file
 * @brief ScalarMapのヘッダーファイル
 * @author MHIR
 * @since 2.2.0
 */

/**
 * @namespace
 * @brief Visio の名前空間
 */
namespace VSN {
  /**
   * @enum
   * ScalarMap のGUIコンポーネント
   */
  enum {
    MPP_Shape_scalarMap_SphLst = 33000,
    MPP_Shape_scalarMap_RenderingModeRadio,
    MPP_Shape_scalarMap_InterpolateRadio,
    MPP_Shape_scalarMap_NormalDirectionRadio,
    MPP_Shape_scalarMap_ExUseTrilinearChk,
    MPP_Shape_scalarMap_PhysLst,
    MPP_Shape_scalarMap_UpdMinMaxChk,
    MPP_Shape_scalarMap_OutputFileBtn,
    MPP_Shape_scalarMap_PhysParamArea,
    //    MPP_Shape_scalarMap_DivideChk,

    // 物理量でスカラー値が選ばれたときのコンポーネント
    //
    MPP_Shape_scalarMap_Phys_scalar_ScalarCombo,
    MPP_Shape_scalarMap_Phys_scalar_NumLinesTxt,
    MPP_Shape_scalarMap_Phys_scalar_BiLinearChk,
    MPP_Shape_scalarMap_Phys_scalar_MinTxt,
    MPP_Shape_scalarMap_Phys_scalar_MaxTxt,
    MPP_Shape_scalarMap_Phys_scalar_UseCMapChk,
    MPP_Shape_scalarMap_Phys_scalar_UpdMinMaxChk,
    MPP_Shape_scalarMap_Phys_scalar_LineWidthTxt,
    MPP_Shape_scalarMap_Phys_scalar_AntiAliasChk,
    MPP_Shape_scalarMap_Phys_scalar_DispContourLineCB,

    // 物理量で圧力抵抗寄与率が選ばえた時のコンポーネント
    //
    MPP_Shape_scalarMap_Phys_pressDrag_PressCombo,
    MPP_Shape_scalarMap_Phys_pressDrag_DirRadio,

    // 物理量で剪断応力が選ばえた時のコンポーネント
    //
    MPP_Shape_scalarMap_Phys_shearing_CoeffValTxt,
    MPP_Shape_scalarMap_Phys_shearing_StencilWidthTxt,
    MPP_Shape_scalarMap_Phys_shearing_SurfVelocXTxt,
    MPP_Shape_scalarMap_Phys_shearing_SurfVelocYTxt,
    MPP_Shape_scalarMap_Phys_shearing_SurfVelocZTxt,
    MPP_Shape_scalarMap_Phys_shearing_MapCompoRadio,

    // 全抵抗を求めるときに使用する
    // 圧力値のデータの指定
    //
    MPP_Shape_scalarMap_Phys_shearing_PressSphCB,
    MPP_Shape_scalarMap_Phys_shearing_PressCompoCB,
    MPP_Shape_scalarMap_Phys_shearing_TotalResistX,
    MPP_Shape_scalarMap_Phys_shearing_TotalResistY,
    MPP_Shape_scalarMap_Phys_shearing_TotalResistZ
  };
};

class vsnData_Shape;

/**
 * 頂点クラス
 */
class Vertex {
public:

  /**
   * コンストラクタ
   * @param x X座標値
   * @param y Y座標値
   * @param z Z座標値
   */
  Vertex(float x, float y, float z) {
    m_pRef = NULL;
    m_p[0] = x;
    m_p[1] = y;
    m_p[2] = z;
    m_normal[0] = 0;
    m_normal[1] = 0;
    m_normal[2] = 0;

    m_area = 0.0;
    m_projectionArea[0] = 0.0;
    m_projectionArea[1] = 0.0;
    m_projectionArea[2] = 0.0;
  }

  /**
   * LESS のオペレータ
   */
  bool operator<(const Vertex& lhs) const {
    if (m_p[0] < lhs.m_p[0]) return true;
    if (m_p[0] > lhs.m_p[0]) return false;

    if (m_p[1] < lhs.m_p[1]) return true;
    if (m_p[1] > lhs.m_p[1]) return false;
    
    if (m_p[2] < lhs.m_p[2]) return true;
    if (m_p[2] > lhs.m_p[2]) return false;

    return false;
  }

  /**
   * 法線ベクトルの加算
   * @param normal 法線ベクトル
   */
  void AppendNormal(const VFR::vector3& normal) {
    m_normal[0] += normal[0];
    m_normal[1] += normal[1];
    m_normal[2] += normal[2];
  }

  /**
   * 面積の加算
   * @param area 加算する面積
   */
  void AppendArea(double area) {
    m_area += area;
  }

  /**
   * 面積の取得
   * @result 面積
   */
  double GetArea(void) const {
    return m_area;
  }

  /**
   * 座標値
   */
  CES::Vec3<float> m_p;

  /**
   * 法線ベクトル
   */
  VFR::vector3 m_normal;

  /**
   * 実体の参照先
   */
  Vertex* m_pRef;

  /**
   * 面積
   */
  float m_area;

  /**
   * 方向別投影面積
   */
  float m_projectionArea[3];
};

/**
 * 頂点の管理クラス
 */
class VertexManager {
public:
  /**
   * 頂点の追加
   * @param v 頂点
   */
  Vertex* Insert(const VFR::vector3& v) {
    return Insert(Vertex(v[0], v[1], v[2]));
  }

  /**
   * 頂点の追加
   * @param vertex 頂点
   */
  Vertex* Insert(const Vertex& vertex) {
    Vertex* result = NULL;
    std::set<Vertex>::iterator it = m_s.find(vertex);
    if (it == m_s.end()) {
      Vertex v(vertex);
      result = new Vertex(vertex);
      v.m_pRef = result;
      m_s.insert(v);
    } else {
      result = (*it).m_pRef;
    }
    return result;
  }

  /**
   * 頂点の検索
   * @param pos 座標値
   * @return 座標値が一致する頂点
   */
  Vertex* Find(const CES::Vec3<float>& pos) {
    return Find(pos[0], pos[1], pos[2]);
  }

  /**
   * 頂点の検索
   * @param v 座標値
   * @return 座標値が一致する頂点
   */
  Vertex* Find(const VFR::vector3& v) {
    return Find(v[0], v[1], v[2]);
  }

  /**
   * 頂点の検索
   * @param x X座標値
   * @param y Y座標値
   * @param z Z座標値
   * @return 座標値が一致する頂点
   */
  Vertex* Find(float x, float y, float z) {
    return Find(Vertex(x, y, z));
  }

  /**
   * 頂点の検索
   * @param vertex 座標値
   * @return 座標値が一致する頂点
   */
  Vertex* Find(const Vertex& vertex) {
    std::set<Vertex>::iterator it = m_s.find(vertex);
    return (it == m_s.end()) ? NULL :  (*it).m_pRef;
  }

  /**
   * 最初のイテレーター
   */
  std::set<Vertex>::iterator Begin(void) {
    return m_s.begin();
  }

  /**
   * 最後のイテレーター
   */
  std::set<Vertex>::iterator End(void) {
    return m_s.end();
  }

  /**
   * 管理している頂点情報の削除
   */
  void Clear(void) {
    m_s.clear();
  }

  /**
   * 頂点のセット
   * 座標値でソートされている
   */
  std::set<Vertex> m_s;
};

/**
 * 表示物理量で「剪断応力」又は「摩擦抵抗」が選ばれた時のパネル
 */
class vsnMPP_Shape_scalarMap_Phys_shearing : public vsnMethodPP {
public:
  vsnMPP_Shape_scalarMap_Phys_shearing(wxPanel* parent, vsnMethodObj* pm);
  virtual ~vsnMPP_Shape_scalarMap_Phys_shearing(void);

  // interface
  virtual bool update(void);


private:
  // event handler
  void OnCoeffValTxt(wxCommandEvent& event);
  void OnStencilWidthTxt(wxCommandEvent& event);
  void OnSurfVelocXTxt(wxCommandEvent& event);
  void OnSurfVelocYTxt(wxCommandEvent& event);
  void OnSurfVelocZTxt(wxCommandEvent& event);
  void OnMapCompoRadio(wxCommandEvent& event);
  void OnPressSphCB(wxCommandEvent& event);
  void OnPressCompoCB(wxCommandEvent& event);

  void OnSurfVelocTxt(size_t dir);

  wxTextCtrl* m_pCoeffValTxt;
  wxTextCtrl* m_pStencilWidthTxt;
  wxTextCtrl* m_pSurfVelocTxt[3];
  wxRadioBox* m_pMapCompoRadio;
  wxComboBox* m_pPressSphCB;
  wxStaticText* m_pPressSphTxt;
  wxComboBox* m_pPressCompoCB;
  wxStaticText* m_pPressCompoTxt;
  wxTextCtrl* m_pTotalResistTxt[3];
  wxStaticText* m_pTotalResistLabel[4];

  DECLARE_EVENT_TABLE()
};

/**
 * 物理量でスカラーが選ばれたときのパネル
 */
class vsnMPP_Shape_scalarMap_Phys_scalar : public vsnMethodPP {
public:
  vsnMPP_Shape_scalarMap_Phys_scalar(wxPanel* parent, vsnMethodObj* pm);
  virtual ~vsnMPP_Shape_scalarMap_Phys_scalar(void);

  // interface
  virtual bool update(void);

private:
  // Event Handler
  //
  void OnScalarLst(wxCommandEvent& event);
  void OnNumLinesTxt(wxCommandEvent& event);
  void OnMinMaxTxt(wxCommandEvent& event);
  void OnUseCMapChk(wxCommandEvent& event);
  void OnLineWidthTxt(wxCommandEvent& event);
  void OnAntiAliasChk(wxCommandEvent& event);
  void OnDispContourLineCB(wxCommandEvent& event);

  wxComboBox* m_pScalarLst;
  wxTextCtrl* m_pNumLinesTxt;
  wxCheckBox* m_pBiLinearChk;
  wxTextCtrl* m_pMinTxt;
  wxTextCtrl* m_pMaxTxt;
  wxCheckBox* m_pUseCMapChk;
  wxCheckBox* m_pUpdMinMaxChk;
  wxTextCtrl* m_pLineWidthTxt;
  wxCheckBox* m_pAntiAliasChk;
  wxCheckBox* m_pDispContourLineCB;

  DECLARE_EVENT_TABLE()
};

/**
 * 圧力寄与率
 */
class vsnMPP_Shape_scalarMap_Phys_pressDrag : public vsnMethodPP {
public:
  vsnMPP_Shape_scalarMap_Phys_pressDrag(wxPanel* parent, vsnMethodObj* pm);
  virtual ~vsnMPP_Shape_scalarMap_Phys_pressDrag(void);

  // interface
  virtual bool update(void);

private:
  // Event Handler
  void OnDirRadio(wxCommandEvent& event);
  void OnPressCombo(wxCommandEvent& event);

  wxComboBox* m_pPressCombo;
  wxRadioBox* m_pDirRadio;

  DECLARE_EVENT_TABLE()
};

/**
 * ScalarMap の操作パネル
 * @author MHIR
 * @since 2.2.0
 */
class vsnMPP_Shape_scalarMap : public vsnMethodPP {
public:
  vsnMPP_Shape_scalarMap(wxPanel* parent, vsnMethodObj* pm);
  virtual ~vsnMPP_Shape_scalarMap(void);

  // interface
  virtual bool update(void);

private:
  wxComboBox* m_pSphLst;
  wxRadioBox* m_pRenderingModeRadio;
  wxRadioBox* m_pInterpolateRadio;
  wxCheckBox* m_pExUseTrilinearChk;
  wxRadioBox* m_pNormalDirectionRadio;
  wxComboBox* m_pPhysLst;
  wxCheckBox* m_pUpdMinMaxChk;
  //  wxCheckBox* m_pDivideChk;
  wxComboBox* m_pScalarLst;
  wxPanel*    m_pPanelMethod;
  wxPanel*    m_pPhysParamArea;
  wxPanel*    m_parentPanel;
  wxButton*   m_pOutputFileBtn;

  vsnMethodPP* m_mpp;

  // Event Handler
  bool changePhysPanel(void);
  void OnSphLst(wxCommandEvent& event);
  void OnInterpolateRadio(wxCommandEvent& event);
  void OnRenderingModeRadio(wxCommandEvent& event);
  void OnPhysLst(wxCommandEvent& event);
  void OnScalarLst(wxCommandEvent& event);
  void OnUpdMinMaxChk(wxCommandEvent& event);
  void OnNormalDirectionRadio(wxCommandEvent& event);
  //  void OnDivideChk(wxCommandEvent& event);
  void OnExUseTrilinearChk(wxCommandEvent& event);
  void OnOutputFileBtn(wxCommandEvent& event);

  DECLARE_EVENT_TABLE()
};

/**
 * ScalarMap のメソッド
 * @author MHIR
 * @since 2.2.0
 */
class vsnMethod_Shape_scalarMap : public vsnMethodObj, public vsnTimeSeriesMethodIF {
public:
  vsnMethod_Shape_scalarMap(const std::string& nm=std::string(VFR_NONAME));
  virtual ~vsnMethod_Shape_scalarMap(void);

  // from vsnMethodObj
  //  SV_GETDATATYPES_DEFINITION;
  virtual std::deque<std::string> getDataTypes() const {
    std::deque<std::string> r;
    r.push_back(std::string("Shape"));
    return r;
  }
  virtual std::string getMethodType() const {
    return std::string("scalarMap");
  }
  virtual bool update(const bool force =true);
  virtual void reloaded();
  virtual vsnMethodPP* getParamPanel(wxPanel* pp);
  virtual void setBaseColor(const vector4 cv);

  // from vsnTimeSeriesMethodIF
  virtual bool updateStep(const int stp,
                          const bool force =true, const bool cascade =true);

  // from vsnIoObject
  virtual bool parseXML(xmlNodePtr xnp);
  virtual bool outputXML(std::ostream& os, const size_t ts =0);
  virtual bool commandXML(xmlNodePtr xnp);

  /**
   * 値の設定
   * @exception std::runtime_error エラー
   * @exception std::bad_alloc メモリ不足
   */
  bool setValue(void);

  /**
   * スカラー値の設定
   */
  void setScalarValue(void);
  bool setContourRange(const float min, const float max);
  void getContourRange(float* min, float* max) const;
  /**
   * コンターラインの数
   */
  size_t getNumContourLineChildren(void);
  bool setLineWidth(float val);
  float getLineWidth(void) const { return m_lineWidth; }
  bool setUseCMap(bool val);
  bool getUseCMap(void) const { return m_useCMap; }
  void getRange(float& min, float& max) { min = m_minmax[0]; max = m_minmax[1]; }
  //  void setUseLutContourLine(const bool ulm);
  //  bool setAntiAliasMode(bool val);
  //  bool getAntiAliasMode(void) const { return m_antiAliasMode; }

  /**
   * 圧力抵抗寄与率の設定
   */
  void setPressDragValue(void);
  /**
   * 圧力抵抗寄与率データの取得
   * m_minmaxとm_allValuesが更新される
   */
  bool getPressDragValue(void);

  /**
   * せん断応力の設定
   * @param isShearingMode true ならば剪断応力モード
   *                       false ならば摩擦抵抗モード
   */
  bool setShearingValue(bool isShearingMode);
  /**
   * せん断応力データの取得
   * m_minmax と m_allValues が更新される
   */
  bool getShearingValue(bool isShearingMode);

  /**
   * 圧力抵抗を求める
   * m_minmax と m_allValues が更新される
   */
  bool getPressResistValue(void);

  bool getTotalResistValue(void);

#ifdef OLD
  /**
   * 摩擦抵抗の設定
   */
  void setFricResistValue(void);
#endif // OLD

  int getSphIdx(void) const { return m_sphIdx; }
  bool setSphIdx(int val);

  int getRenderingMode(void) const { return m_renderingMode; }
  bool setRenderingMode(int val);

  bool loadSph(void);

  void clearSph(void) { m_sphList.clear(); }
  void addSph(vsnData_Sph* shape) { m_sphList.push_back(shape); }
  //  vsnData_Sph* getSph(size_t idx) { return m_sphList[idx]; }
  size_t sphSize(void) const { return m_sphList.size(); }

  /**
   * 選択されているSPHを取得する
   * @return SPHへのポインタ。失敗すると NULL を返す。
   */
  vsnData_Sph* getSelectedSph(void);

  /**
   * SPHを取得する
   * @return SPH へのポインタ。失敗すると NULL を返す。
   */
  vsnData_Sph* getSph(size_t idx);

  int getPhysIdx(void) const { return m_physIdx; }
  bool setPhysIdx(int val);

  int getScalarValIdx(void) const { return m_scalarValIdx; }
  bool setScalarValIdx(int val);

  int getInterpolateMode(void) const { return m_interpolateMode; }
  bool setInterpolateMode(int val);

  bool getUpdateMinMaxMode() const {return m_updateMinMax;}
  bool setUpdateMinMaxMode(const bool mode);

  int getNormalDirection() const { return m_normalDirection; }
  bool setNormalDirection(int val);

  int getPressValIdx(void) const { return m_pressValIdx; }
  bool setPressValIdx(int val);

  int getPressDir(void) const { return m_pressDir; }
  bool setPressDir(int val);

  //  bool getDivideMode(void) const { return m_divideMode; }
  //  bool setDivideMode(const bool mode);

  bool getExUseTrilinear(void) const { return m_exUseTrilinear; }
  bool setExUseTrilinear(const bool mode);

  /**
   * せん断応力の粘性係数を設定する
   */
  bool setShearingCoeffVal(float val);
  /**
   * せん断応力の粘性係数を取得する
   */
  float getShearingCoeffVal(void) const { return m_shearingCoeffVal; }

  /**
   * せん断応力のステンシル幅を設定する
   */
  bool setShearingStencilWidth(float val);
  /**
   * せん断応力のステンシル幅を取得する
   */
  float getShearingStencilWidth(void) const { return m_shearingStencil; }
  /**
   * せん断応力の表面速度ベクトルを設定する
   * @param val 値
   * @param idx =0(X), =1(Y), =2(Z)
   */
  bool setShearingSurfVeloc(float val, int idx);
  /**
   * せん断応力の表面速度ベクトルを取得する
   * @param dir =0(X), =1(Y), =2(Z)
   * @return 指定された方向(dir)の速度ベクトル成分
   */
  float getShearingSurfVeloc(int dir) const {
    float val = 0.0;
    if (dir == 0 || dir == 1 || dir == 2) val = m_shearingSurfVeloc[dir];
    return val;
  }
  /**
   * せん断応力の表面速度ベクトルを取得する
   * @return 速度ベクトル
   */
  const CES::Vec3<float>& getShearingSurfVeloc(void) const { return m_shearingSurfVeloc; }
  /**
   * せん断応力の方向の設定
   * @param val =0(X), =1(Y), =2(Z)
   */
  bool setShearingMapCompo(int val);
  /**
   * せん断応力の方向の取得
   * @retval 0 X
   * @retval 1 Y
   * @retval 2 Z
   * @retval 3 VecLen
   */
  int getShearingMapCompo(void) const { return m_shearingMapCompo; }


  /**
   * 摩擦抵抗の粘性係数を設定する
   */
  bool setFricResistCoeffVal(float val) { return setShearingCoeffVal(val); }
  /**
   * 摩擦抵抗の粘性係数を取得する
   */
  float getFricResistCoeffVal(void) const { 
    return getShearingCoeffVal();
    //    return m_fricResistCoeffVal;
  }

  /**
   * 摩擦抵抗のステンシル幅を設定する
   */
  bool setFricResistStencilWidth(float val) { return setShearingStencilWidth(val); }

  /**
   * 摩擦抵抗のステンシル幅を取得する
   */
  float getFricResistStencilWidth(void) const {
    return getShearingStencilWidth();
    //    return m_fricResistStencil;
  }
  /**
   * 摩擦抵抗の表面速度ベクトルを設定する
   * @param val 値
   * @param idx =0(X), =1(Y), =2(Z)
   */
  bool setFricResistSurfVeloc(float val, int idx) { return setShearingSurfVeloc(val, idx); }
  /**
   * 摩擦抵抗の表面速度ベクトルを取得する
   * @param dir =0(X), =1(Y), =2(Z)
   * @return 指定された方向(dir)の速度ベクトル成分
   */
  float getFricResistSurfVeloc(int dir) const {
    return getShearingSurfVeloc(dir);
//     float val = 0.0;
//     if (dir == 0 || dir == 1 || dir == 2) val = m_fricResistSurfVeloc[dir];
//     return val;
  }
  /**
   * 摩擦抵抗の表面速度ベクトルを取得する
   * @return 速度ベクトル
   */
  const CES::Vec3<float>& getFricResistSurfVeloc(void) const { 
    return getShearingSurfVeloc();
    //    return m_fricResistSurfVeloc;
  }
  /**
   * 摩擦抵抗の方向の設定
   * @param val =0(X), =1(Y), =2(Z)
   */
  bool setFricResistMapCompo(int val) { return setShearingMapCompo(val); }
  /**
   * 摩擦抵抗の方向の取得
   * @retval 0 X
   * @retval 1 Y
   * @retval 2 Z
   * @retval 3 VecLen
   */
  int getFricResistMapCompo(void) const { 
    return getShearingMapCompo();
    //    return m_fricResistMapCompo;
  }

  /**
   * 全抵抗
   * 圧力のデータのSPHのインデックス
   */
  int getPressSphIdx(void) const { return m_totalResistPressSphIdx; }
  bool setPressSphIdx(int idx);

  /**
   * 全抵抗
   * 圧力のコンポーネントの指定
   */
  int getPressCompoIdx(void) const { return m_totalResistPressCompoIdx; }
  bool setPressCompoIdx(int idx);


#ifdef OLD
  /**
   * mode が AT_PER_FACE の時は、面の面積を求める。
   * mode が AT_PER_VERTEX の時は、頂点が含まれる面を重心と辺の中点で分割し、頂点が含まれる部分面の合計面積を求める
   * @brief 面積を求める
   * @param mode AT_PER_FACE または AT_PER_VERTEX
   * @return 面積の配列、配列の個数は mode により異なる。エラー時は空の配列が返る。
   */
  std::vector<float> getAreas(VFR::AppearType mode);
#endif // OLD

  /**
   * 物理量のパネルを取得する
   */
  vsnMethodPP* getPhysPanel(wxPanel* pp);

  bool IsShearingMode(void) const;
  bool IsFricResistMode(void) const;

  /**
   *
   */
  bool allocFaces(size_t numFaces);

  /**
   * 最小値と最大値
   * getPressDragValue, getShearngValue で設定される
   * @see getPressDragValue
   * @see getShearingValue
   */
  float m_minmax[2];
  /**
   * 全ての値
   * getPressDragValue, getShearngValue, getPressResistValue で設定される
   * @see getPressDragValue
   * @see getShearngValue
   * @see getPressResistValue
   */
  std::vector<float> m_allValues;

  /**
   * 全ての値成分ごと
   * 全抵抗の成分ごとの値のために用いる
   */
  std::vector<CES::Vec3<float> > m_allValues3;

  float getTotalResistValue(size_t idx) { return m_totalResistValue[idx]; }

  bool setNumLines(const size_t nl);
  size_t getNumLines() const {return m_numLines;}

  bool getDispContourLine(void) const { return m_dispContourLine; }
  bool setDispContourLine(bool val);

  bool generateTrias(bool force=false);

  void viewByParam(void);

  void SaveStlExt(const std::string& outPath);

private:

  /**
   * 頂点にマップするときに使用する
   * 頂点毎の法線ベクトル、面積、投影面積を保持する
   * ファイルには保存しない
   */
  VertexManager m_vm;

  /**
   * 面の重心座標
   * ファイルには保存しない
   */
  VFR::vector3* m_faceGravity;
  /**
   * 面の数
   * ファイルには保存しない
   */
  size_t m_numFaces;
  /**
   * 面の面積
   * ファイルには保存しない
   */
  double* m_faceArea;
  /**
   * 面の投影面積
   * ファイルには保存しない
   */
  double** m_faceProjectionArea;

  /**
   * SPHのリスト
   * ファイルには保存しない
   */
  std::vector<vsnData_Sph*> m_sphList;

  /**
   * 三角形群
   * ファイルには保存しない
   */
  vfrTriangles*  m_trias;

  /**
   * 選択されているSPHのインデックス
   */
  int m_sphIdx;

  /**
   * RenderingMode
   */
  int m_renderingMode;

  /**
   * 内挿のモード
   * (=0) ゼロ次
   * (=1) 三重線形
   * (=2) 外挿１次
   * (=3) 外挿２次
   */
  int m_interpolateMode;

  /**
   * 物理量のインデックス
   * (=0) スカラー
   * (=1) 圧力抵抗の寄与率
   * (=2) せん断応力
   * (=3) 摩擦抵抗
   * (=4) 全抵抗
   */
  int m_physIdx;

  /**
   * 表示時にレンジを更新するかどうかのフラグ
   * false であれば更新しない
   * true であえば更新する
   */
  bool m_updateMinMax;

  /**
   * 法線の方向
   * (=0) Normal
   * (=1) Reverse
   */
  int m_normalDirection;

  /**
   * STLを細分割するか
   */
  //  bool m_divideMode;

  /**
   * 外挿で使用する点の値を求める際に、ゼロ次補間をするか三重線形内挿をするか
   * true であれば、三重線形内挿を行う
   */
  bool m_exUseTrilinear;

  // スカラー値
  //
  /**
   * スカラー値のインデックス
   */
  int m_scalarValIdx;
  /**
   * コンターラインの表示をするかどうか
   */
  bool m_dispContourLine;
  /**
   * コンターラインの本数
   */
  size_t m_numLines;
  /**
   * 未使用
   */
  size_t m_numValidLines;
  /**
   * コンターラインの線の幅
   */
  float m_lineWidth;
  /**
   * コンターラインの表示の際にカラーマップを使用するかどうか
   */
  bool m_useCMap;
  /**
   * AntiAlias の表示をするかどうか
   */
  bool m_antiAlias;



  // 圧力抵抗の寄与率
  //
  /**
   * 圧力値のインデックス
   */
  int m_pressValIdx;
  /**
   * 射影する方向
   */
  int m_pressDir;

  // せん断応力、摩擦抵抗
  //
  /**
   * せん断応力の粘性係数
   */
  float m_shearingCoeffVal;
  /**
   * せん断応力のステンシル幅
   */
  float m_shearingStencil;
  /**
   * せん断応力の表面ベクトル
   */
  CES::Vec3<float> m_shearingSurfVeloc;
  /**
   * せん断応力の方向
   */
  int m_shearingMapCompo;

#ifdef OLD
  /**
   * 摩擦抵抗の粘性係数
   */
  float m_fricResistCoeffVal;
  /**
   * 摩擦抵抗のステンシル幅
   */
  float m_fricResistStencil;
  /**
   * 摩擦抵抗の表面ベクトル
   */
  CES::Vec3<float> m_fricResistSurfVeloc;
  /**
   * 摩擦抵抗の方向
   */
  int m_fricResistMapCompo;
#endif // OLD


  // 全抵抗
  //
  /**
   * 圧力のSPHデータのインデックス
   */
  int m_totalResistPressSphIdx;
  /**
   * 圧力データの成分
   */
  int m_totalResistPressCompoIdx;


  /**
   * 全抵抗の成分
   */
  CES::Vec3<float> m_totalResistValue;

  /**
   * レンジの設定
   */
  void adjustRange(const bool updval=true);

  void deleteContourLines(void);

  static
  bool calcLineSegment(const vector3 p0, const vector3 p1, const vector3 p2,
		       const float val0, const float val1, const float val2,
		       const float val, vector3 lp1, vector3 lp2);

};

#endif // _VSN_METHOD_SV_SCALAR_MAP_HPP_
