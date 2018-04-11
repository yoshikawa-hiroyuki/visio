//
// vsnMethod_Sv_orthoScalar
//
#ifndef _VSN_METHOD_SV_ORTHOSCALAR_H_
#define _VSN_METHOD_SV_ORTHOSCALAR_H_

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
#include "vfrIndexPolygons.h"

namespace VSN {
  // control ids
  enum {MPP_Sv_orthoScalar_SlicerLst = 5300,
	MPP_Sv_orthoScalar_SlicerTxt,
	MPP_Sv_orthoScalar_SetSlicerBtn,
	MPP_Sv_orthoScalar_SelDataLst,
	MPP_Sv_orthoScalar_VecDataChkLst,
	MPP_Sv_orthoScalar_UpdMinMaxChk
  };
};


//----------------------------------------------------------------
// class vsnMPP_Sv_orthoScalar
//   method parameter-panel for vsnMethod_Sv_orthoScalar
//----------------------------------------------------------------
class vsnMPP_Sv_orthoScalar : public vsnMethodPP {
public: 
  vsnMPP_Sv_orthoScalar(wxPanel* parent, vsnMethodObj* pm);
  virtual ~vsnMPP_Sv_orthoScalar();

  // interface
  virtual bool update();
  virtual void settlement();

  // event handler
  void OnSetSlicerBtn(wxCommandEvent& event);
  void OnSelDataLst(wxCommandEvent& event);
  void OnVecDataChkLst(wxCommandEvent& event);
  void OnUpdMinMaxChk(wxCommandEvent& event);
  void OnEnterSlicerTxt(wxCommandEvent& event);

private:
  wxTextCtrl*     m_pSlicerTxt;
  wxListBox*      m_pSlicerList;
  wxButton*       m_pSetSlicerBtn;
  wxComboBox*     m_pSelDataLst;
  wxCheckListBox* m_pVecDataChkLst;
  wxCheckBox*     m_pUpdMinMaxChk;

  vsnRefOrthoSlicerKeyAct gfxAct_KI;

  DECLARE_EVENT_TABLE()
};


//----------------------------------------------------------------
// class vsnMethod_Sv_orthoScalar
//----------------------------------------------------------------
class vsnMethod_Sv_orthoScalar
  : public vsnMethodObj,
    public vsnTimeSeriesMethodIF,
    public vsnRef_Sv_orthoSlicer {
public:
  vsnMethod_Sv_orthoScalar(const std::string& nm=std::string(VFR_NONAME));
  virtual ~vsnMethod_Sv_orthoScalar();

  VSN::WhichDataType getSelectedData() const {return m_selectedData;}
  bool setSelectedData(const VSN::WhichDataType sd);

  CES::Vec3<int> getVecDataIdx() const {return m_vecDataIdx;}
  bool setVecDataIdx(const CES::Vec3<int>& vdidx);
  bool isValidVecData() const;

  bool getUpdateMinMaxMode() const {return m_updateMinMax;}
  bool setUpdateMinMaxMode(const bool mode);

  // from vsnTimeSeriesMethodIF
  virtual bool updateStep(const int stp,
			  const bool force =true, const bool cascade =true);

  // from vsnMethodObj
  SV_GETDATATYPES_DEFINITION;
  virtual std::string getMethodType() const {
    return std::string("orthoScalar");
  }
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
  VSN::WhichDataType m_selectedData;
  CES::Vec3<int>     m_vecDataIdx;
  bool               m_updateMinMax;
  vfrIndexPolygons*  m_slice;

  void adjustRange();
};

#endif // _VSN_METHOD_SV_ORTHOSCALAR_H_
