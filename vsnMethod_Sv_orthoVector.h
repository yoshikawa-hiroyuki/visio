//
// vsnMethod_Sv_orthoVector
//
#ifndef _VSN_METHOD_SV_ORTHOVECTOR_H_
#define _VSN_METHOD_SV_ORTHOVECTOR_H_

#include "wx/defs.h"
#include "wx/listbox.h"
#include "wx/checklst.h"
#include "wx/textctrl.h"
#include "wx/button.h"
#include "wx/combobox.h"
#include "wx/checkbox.h"

#include "vsnMethodObj.h"
#include "vsnMethod_Sv_orthoSlicer.h"
#include "vsnData_Sv.h"
#include "vfrVectors.h"

namespace VSN {
  // control ids
  enum {MPP_Sv_orthoVector_SlicerLst = 5400,
	MPP_Sv_orthoVector_SlicerTxt,
	MPP_Sv_orthoVector_SetSlicerBtn,
	MPP_Sv_orthoVector_VecScaleTxt,
	MPP_Sv_orthoVector_VecHeadChk,
	MPP_Sv_orthoVector_VecDataChkLst,
	MPP_Sv_orthoVector_SelDataLst,
	MPP_Sv_orthoVector_UpdMinMaxChk,
	MPP_Sv_orthoVector_LineWidthTxt,
	MPP_Sv_orthoVector_AntiAliasChk
  };
};


//----------------------------------------------------------------
// class vsnMPP_Sv_orthoVector
//   method parameter-panel for vsnMethod_Sv_orthoVector
//----------------------------------------------------------------
class vsnMPP_Sv_orthoVector : public vsnMethodPP {
public: 
  vsnMPP_Sv_orthoVector(wxPanel* parent, vsnMethodObj* pm);
  virtual ~vsnMPP_Sv_orthoVector();

  // interface
  virtual bool update();
  virtual void settlement();

  // event handler
  void OnSetSlicerBtn(wxCommandEvent& event);
  void OnEnterSlicerTxt(wxCommandEvent& event);
  void OnVecScaleTxt(wxCommandEvent& event);
  void OnVecHeadChk(wxCommandEvent& event);
  void OnVecDataChkLst(wxCommandEvent& event);
  void OnSelDataLst(wxCommandEvent& event);
  void OnUpdMinMaxChk(wxCommandEvent& event);
  void OnLineWidthTxt(wxCommandEvent& event);
  void OnAntiAliasChk(wxCommandEvent& event);

private:
  wxTextCtrl*     m_pSlicerTxt;
  wxListBox*      m_pSlicerLst;
  wxButton*       m_pSetSlicerBtn;
  wxTextCtrl*     m_pVecScaleTxt;
  wxCheckBox*     m_pVecHeadChk;
  wxCheckListBox* m_pVecDataChkLst;
  wxComboBox*     m_pSelDataLst;
  wxCheckBox*     m_pUpdMinMaxChk;
  wxTextCtrl*     m_pLineWidthTxt;
  wxCheckBox*     m_pAntiAliasChk;

  vsnRefOrthoSlicerKeyAct gfxAct_KI;

  DECLARE_EVENT_TABLE()
};


//----------------------------------------------------------------
// class vsnMethod_Sv_orthoVector
//----------------------------------------------------------------
class vsnMethod_Sv_orthoVector
  : public vsnMethodObj,
    public vsnRef_Sv_orthoSlicer,
    public vsnTimeSeriesMethodIF {
public:
  vsnMethod_Sv_orthoVector(const std::string& nm=std::string(VFR_NONAME));
  virtual ~vsnMethod_Sv_orthoVector();

  float getVecScale() const {return m_vecScale;}
  bool setVecScale(const float vs);

  bool getVecHeadMode() const {return m_vecHead;}
  bool setVecHeadMode(const bool vhm);

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
    return std::string("orthoVector");
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
  float          m_vecScale;
  bool           m_vecHead;
  VSN::WhichDataType
                 m_selectedData;
  CES::Vec3<int> m_vecDataIdx;
  bool           m_updateMinMax;
  float          m_lineWidth;

  vfrVectors*    m_vectors;

  void adjustRange();
};

#endif // _VSN_METHOD_SV_ORTHOVECTOR_H_

