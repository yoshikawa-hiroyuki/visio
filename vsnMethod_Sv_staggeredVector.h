
//
// vsnMethod_staggeredVector
//
#ifndef _VSN_METHOD_SV_STAGGERED_VECTOR_H_
#define _VSN_METHOD_SV_STAGGERED_VECTOR_H_

#include "wx/defs.h"
#include "wx/listbox.h"
#include "wx/checkbox.h"
#include "wx/button.h"
#include "wx/textctrl.h"

#include "vsnMethodObj.h"
#include "vsnMethod_Sv_orthoSlicer.h"
#include "vsnData_Sv.h"
#include "vfrVectors.h"

class vsnMethod_Sv_staggeredVector;

namespace VSN {
  // control ids
  enum {MPP_Sv_staggeredVector_SlicerLst = 5150,
        MPP_Sv_staggeredVector_SlicerTxt,
        MPP_Sv_staggeredVector_SetSlicerBtn,
        MPP_Sv_staggeredVector_TypeRadio,
        MPP_Sv_staggeredVector_VecScaleTxt,
        MPP_Sv_staggeredVector_VecHeadChk,
	MPP_Sv_staggeredVector_ShowVecXChk,
	MPP_Sv_staggeredVector_ShowVecYChk,
	MPP_Sv_staggeredVector_ShowVecZChk,
        MPP_Sv_staggeredVector_VecDataChkLst,
        MPP_Sv_staggeredVector_SelDataLst,
        MPP_Sv_staggeredVector_UpdMinMaxChk,
        MPP_Sv_staggeredVector_LineWidthTxt,
        MPP_Sv_staggeredVector_AntiAliasChk,
  };
};


//----------------------------------------------------------------
// class vsnMPP_Sv_staggeredVector
//   method parameter-panel for vsnMethod_Sv_staggeredVector
//----------------------------------------------------------------
class vsnMPP_Sv_staggeredVector : public vsnMethodPP {
public: 
  vsnMPP_Sv_staggeredVector(wxPanel* parent, vsnMethodObj* pm);
  virtual ~vsnMPP_Sv_staggeredVector();

  // interface
  virtual bool update();
  virtual void settlement();

  // event handler
  void OnSetSlicerBtn(wxCommandEvent& event);
  void OnEnterSlicerTxt(wxCommandEvent& event);
  void OnTypeRadio(wxCommandEvent& event);
  void OnVecScaleTxt(wxCommandEvent& event);
  void OnVecHeadChk(wxCommandEvent& event);
  void OnShowVecCompChk(wxCommandEvent& event);
  void OnVecDataChkLst(wxCommandEvent& event);
  void OnSelDataLst(wxCommandEvent& event);
  void OnUpdMinMaxChk(wxCommandEvent& event);
  void OnLineWidthTxt(wxCommandEvent& event);
  void OnAntiAliasChk(wxCommandEvent& event);

private:
  wxTextCtrl*     m_pSlicerTxt;
  wxListBox*      m_pSlicerLst;
  wxButton*       m_pSetSlicerBtn;
  wxRadioBox*     m_pTypeRadio;
  wxTextCtrl*     m_pVecScaleTxt;
  wxCheckBox*     m_pVecHeadChk;
  wxCheckBox*     m_pShowVecXChk;
  wxCheckBox*     m_pShowVecYChk;
  wxCheckBox*     m_pShowVecZChk;
  wxCheckListBox* m_pVecDataChkLst;
  wxComboBox*     m_pSelDataLst;
  wxCheckBox*     m_pUpdMinMaxChk;
  wxTextCtrl*     m_pLineWidthTxt;
  wxCheckBox*     m_pAntiAliasChk;

  vsnRefOrthoSlicerKeyAct gfxAct_KI;

  DECLARE_EVENT_TABLE()
};


//----------------------------------------------------------------
// class vsnMethod_Sv_staggeredVector
//----------------------------------------------------------------
class vsnMethod_Sv_staggeredVector
  : public vsnMethodObj,
    public vsnRef_Sv_orthoSlicer,
    public vsnTimeSeriesMethodIF {
public:
  enum VecLayoutType {Regular, Colocated, Staggered1, Staggered2};

  vsnMethod_Sv_staggeredVector(const std::string& nm=std::string(VFR_NONAME));
  virtual ~vsnMethod_Sv_staggeredVector();

  VecLayoutType getVecLayout() const {return m_vecLayoutType;}
  bool setVecLayout(const VecLayoutType vlt);

  float getVecScale() const {return m_vecScale;}
  bool setVecScale(const float vs);

  bool getVecHeadMode() const {return m_vecHead;}
  bool setVecHeadMode(const bool vhm);

  void getShowVecComp(bool& svx, bool& svy, bool& svz) const;
  bool setShowVecComp(const bool svx, const bool svy, const bool svz);

  CES::Vec3<int> getVecDataIdx() const {return m_vecDataIdx;}
  bool setVecDataIdx(const CES::Vec3<int>& vdidx);
  bool isValidVecData() const;

  VSN::WhichDataType getSelectedData() const {return m_selectedData;}
  bool setSelectedData(const VSN::WhichDataType sd);

  bool getUpdateMinMaxMode() const {return m_updateMinMax;}
  bool setUpdateMinMaxMode(const bool mode);

  float getLineWidth() const {return m_lineWidth;}
  bool setLineWidth(const float lw);

  // from vsnTimeSeriesMethodIF
  virtual bool updateStep(const int stp,
                          const bool force =true, const bool cascade =true);

  // from vsnMethodObj
  SV_GETDATATYPES_DEFINITION;
  virtual std::string getMethodType() const {
    return std::string("staggeredVector");
  }
  virtual bool canLighting() const {return false;}
  virtual bool update(const bool force =true);
  virtual void reloaded();
  virtual vsnMethodPP* getParamPanel(wxPanel* pp);

  // from vsnIoObject
  virtual bool parseXML(xmlNodePtr xnp);
  virtual bool outputXML(std::ostream& os, const size_t ts =0);
  virtual bool commandXML(xmlNodePtr xnp);

  // from vsnRef_Sv_orthoSlicer
  virtual void noticeUpdate();

private:
  VecLayoutType  m_vecLayoutType;
  float          m_vecScale;
  bool           m_vecHead;
  bool           m_showVecX, m_showVecY, m_showVecZ;
  VSN::WhichDataType
                 m_selectedData;
  CES::Vec3<int> m_vecDataIdx;
  bool           m_updateMinMax;
  float          m_lineWidth;

  vfrVectors*    m_vectorsX;
  vfrVectors*    m_vectorsY;
  vfrVectors*    m_vectorsZ;

  void adjustRange();
};

#endif // _VSN_METHOD_SV_STAGGERED_VECTOR_H_

