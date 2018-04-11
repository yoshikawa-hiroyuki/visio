//
// vsnMethod_OctVol_shaderVolren
//
#ifndef _VSN_METHOD_OCTVOL_SHADER_VOLREN_H_
#define _VSN_METHOD_OCTVOL_SHADER_VOLREN_H_

#include "wx/defs.h"
#include "wx/textctrl.h"
#include "wx/listbox.h"
#include "wx/button.h"
#include "wx/checklst.h"
#include "wx/checkbox.h"

#include "vsnRvrVolumeRender.h"
#include "vsnMethodObj.h"
#include "vsnData_OctVol.h"
#include "vsnCMap.h"

namespace VSN {
  // control ids
  enum {MPP_OctVol_shaderVolren_SelDataLst = 4750,
        MPP_OctVol_shaderVolren_VecDataChkLst,
        MPP_OctVol_shaderVolren_UpdMinMaxChk,
	MPP_OctVol_shaderVolren_SliceNumTxt,
        MPP_OctVol_shaderVolren_InterpolateChk,
        MPP_OctVol_shaderVolren_GradMapBtn,
        MPP_OctVol_shaderVolren_RegionResetBtn,
        MPP_OctVol_shaderVolren_RegionX1Txt,
        MPP_OctVol_shaderVolren_RegionX2Txt,
        MPP_OctVol_shaderVolren_RegionY1Txt,
        MPP_OctVol_shaderVolren_RegionY2Txt,
        MPP_OctVol_shaderVolren_RegionZ1Txt,
        MPP_OctVol_shaderVolren_RegionZ2Txt
  };
};

//----------------------------------------------------------------
// class vsnMPP_OctVol_shaderVolren
//   method parameter-panel for vsnMethod_OctVol_shaderVolren
//----------------------------------------------------------------
class vsnMPP_OctVol_shaderVolren : public vsnMethodPP {
public:
  vsnMPP_OctVol_shaderVolren(wxPanel* parent, vsnMethodObj* pm);
  virtual ~vsnMPP_OctVol_shaderVolren();

  // interface
  virtual bool update();

  // event handler
  void OnSelDataLst(wxCommandEvent& event);
  void OnVecDataChkLst(wxCommandEvent& event);
  void OnUpdMinMaxChk(wxCommandEvent& event);
  void OnSliceNumTxt(wxCommandEvent& event);
  void OnInterpolateChk(wxCommandEvent& event);
  void OnGradMapBtn(wxCommandEvent& event);
  void OnRegionResetBtn(wxCommandEvent& event);
  void OnRegionTxt(wxCommandEvent& event);

private:
  wxComboBox*     m_pSelDataLst;
  wxCheckListBox* m_pVecDataChkLst;
  wxCheckBox*     m_pUpdMinMaxChk;
  wxTextCtrl*     m_pSliceNumTxt;
  wxCheckBox*     m_pInterpolateChk;
  wxButton*       m_pGradMapBtn;
  wxButton*       m_pRegionResetBtn;
  wxTextCtrl*     m_pRegionX1Txt;
  wxTextCtrl*     m_pRegionX2Txt;
  wxTextCtrl*     m_pRegionY1Txt;
  wxTextCtrl*     m_pRegionY2Txt;
  wxTextCtrl*     m_pRegionZ1Txt;
  wxTextCtrl*     m_pRegionZ2Txt;

  DECLARE_EVENT_TABLE()
};


//----------------------------------------------------------------
// class vsnMethod_OctVol_shaderVolren
//----------------------------------------------------------------
class vsnMethod_OctVol_shaderVolren
: public vsnMethodObj,
  public vsnExtLutRefer,
  public vsnTimeSeriesMethodIF
{
public:
  vsnMethod_OctVol_shaderVolren(const std::string& nm=std::string(VFR_NONAME));
  virtual ~vsnMethod_OctVol_shaderVolren();

  VSN::WhichDataType getSelectedData() const {return m_selectedData;}
  bool setSelectedData(const VSN::WhichDataType sd);

  CES::Vec3<int> getVecDataIdx() const {return m_vecDataIdx;}
  bool setVecDataIdx(const CES::Vec3<int>& vdidx);
  bool isValidVecData() const;

  bool setUpdateMinMaxMode(const bool mode);
  bool getUpdateMinMaxMode() const {return m_updateMinMax;}

  size_t getNumSlices() const;
  bool setNumSlices(const size_t nsl);

  bool setInterpolateMode(const bool mode);
  bool getInterpolateMode() const {return m_interpolate;}

  bool setRegion(const CES::Vec3<float>* preg);
  bool getRegion(CES::Vec3<float>* preg) const;

  // from vsnTimeSeriesMethodIF
  virtual bool updateStep(const int stp,
                          const bool force =true, const bool cascade =true);

  // from vsnExtLutRefer : for gradient-map
  virtual void ext_setLut(const vsnLut& lut);
  virtual vsnLut ext_getLut() const;

  // from vsnMethodObj
  virtual std::deque<std::string> getDataTypes() const {
    std::deque<std::string> r; r.push_back(std::string("OctVol")); return r;
  }
  virtual std::string getMethodType() const {
    return std::string("shaderVolren");
  }
  virtual bool update(const bool force =true);
  virtual void reloaded();
  virtual vsnMethodPP* getParamPanel(wxPanel* pp);
  virtual void setBaseColor(const vector4 cv);
  virtual void setRefData(vsnDataObj* prd);
  virtual void setLighting(const bool mode);

  // from vsnIoObject
  virtual bool parseXML(xmlNodePtr xnp);
  virtual bool outputXML(std::ostream& os, const size_t ts =0);
  virtual bool commandXML(xmlNodePtr xnp);

private:
  VSN::WhichDataType  m_selectedData;
  CES::Vec3<int>      m_vecDataIdx;
  bool                m_updateMinMax;
  bool                m_interpolate;
  vsnRvrVolumeRender* m_pRender;

  void adjustRange();
};

#endif // _VSN_METHOD_OCTVOL_SHADER_VOLREN_H_
