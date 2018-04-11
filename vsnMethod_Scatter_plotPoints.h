//
// vsnMethod_Scatter_plotPoints
//
#ifndef _VSN_METHOD_SCATTER_PLOTPOINTS_H_
#define _VSN_METHOD_SCATTER_PLOTPOINTS_H_

#include "wx/defs.h"
#include "wx/listbox.h"
#include "wx/combobox.h"
#include "wx/checklst.h"
#include "wx/textctrl.h"
#include "wx/button.h"
#include "wx/checkbox.h"

#include "vsnMethodObj.h"
#include "vsnData_Scatter.h"
#include "vfrPrimSet.h"

namespace VSN {
  // control ids
  enum {MPP_Scatter_plotPoints_PointTypeRadio = 3600,
	MPP_Scatter_plotPoints_DotSizeTxt,
        MPP_Scatter_plotPoints_RadiusMinTxt,
        MPP_Scatter_plotPoints_RadiusMaxTxt,
	MPP_Scatter_plotPoints_VecDataChkLst,
	MPP_Scatter_plotPoints_SelDataLst,
        MPP_Scatter_plotPoints_UpdMinMaxChk,
        MPP_Scatter_plotPoints_AntiAliasChk,
	MPP_Scatter_plotPoints_SubdivTxt
  };
};


//----------------------------------------------------------------
// class vsnMPP_Scatter_plotPoints
//   method parameter-panel for vsnMethod_Scatter_plotPoints
//----------------------------------------------------------------
class vsnMPP_Scatter_plotPoints : public vsnMethodPP {
public:
  vsnMPP_Scatter_plotPoints(wxPanel* parent, vsnMethodObj* pm);
  virtual ~vsnMPP_Scatter_plotPoints();

  // interface
  virtual bool update();

  // event handler
  void OnPointTypeRadio(wxCommandEvent& event);
  void OnDotSizeTxt(wxCommandEvent& event);
  void OnRadiusRangeTxt(wxCommandEvent& event);
  void OnVecDataChkLst(wxCommandEvent& event);
  void OnSelDataLst(wxCommandEvent& event);
  void OnUpdMinMaxChk(wxCommandEvent& event);
  void OnAntiAliasChk(wxCommandEvent& event);
  void OnSubdivTxt(wxCommandEvent& event);

private:
  wxRadioBox*     m_pPointTypeRadio;
  wxTextCtrl*     m_pDotSizeTxt;
  wxTextCtrl*     m_pRadiusMinTxt;
  wxTextCtrl*     m_pRadiusMaxTxt;
  wxCheckListBox* m_pVecDataChkLst;
  wxComboBox*     m_pSelDataLst;
  wxCheckBox*     m_pUpdMinMaxChk;
  wxCheckBox*     m_pAntiAliasChk;
  wxTextCtrl*     m_pSubdivTxt;

  DECLARE_EVENT_TABLE()
};


//----------------------------------------------------------------
// class vsnMethod_Scatter_plotPoints
//----------------------------------------------------------------
class vsnMethod_Scatter_plotPoints
  : public vsnMethodObj,
    public vsnTimeSeriesMethodIF {
public:
  vsnMethod_Scatter_plotPoints(const std::string& nm=std::string(VFR_NONAME));
  virtual ~vsnMethod_Scatter_plotPoints();

  VFR::RenderType getRenderType() const {return m_renType;}
  virtual bool setRenderType(const VFR::RenderType rt);

  float getDotSize() const {return m_dotSize;}
  bool setDotSize(const float ds);
  void getRadiusRange(float rr[2]) const {
    rr[0] = m_radius[0]; rr[1] = m_radius[1];
  }
  bool setRadiusRange(const float rr[2]);

  int getSubdiv() const {return m_subdiv;}
  bool setSubdiv(const int sdv);

  CES::Vec3<int> getVecDataIdx() const {return m_vecDataIdx;}
  bool setVecDataIdx(const CES::Vec3<int>& vdidx);
  bool isValidVecData() const;

  VSN::WhichDataType getSelectedData() const {return m_selectedData;}
  bool setSelectedData(const VSN::WhichDataType sd);

  bool getUpdateMinMaxMode() const {return m_updateMinMax;}
  bool setUpdateMinMaxMode(const bool mode);

  // from vsnTimeSeriesMethodIF
  virtual bool updateStep(const int stp,
                          const bool force =true, const bool cascade =true);

  // from vsnMethodObj
  virtual std::deque<std::string> getDataTypes() const {
    std::deque<std::string> r; r.push_back(std::string("Scatter")); return r;
  }
  virtual std::string getMethodType() const {
    return std::string("plotPoints");
  }
  virtual bool update(const bool force =true);
  virtual void reloaded();
  virtual vsnMethodPP* getParamPanel(wxPanel* pp);

  // from vsnIoObject
  virtual bool parseXML(xmlNodePtr xnp);
  virtual bool outputXML(std::ostream& os, const size_t ts =0);
  virtual bool commandXML(xmlNodePtr xnp);

protected:
  VFR::RenderType m_renType;
  float           m_dotSize;
  float           m_radius[2];
  int             m_subdiv;
  VSN::WhichDataType
                  m_selectedData;
  CES::Vec3<int>  m_vecDataIdx;
  bool            m_updateMinMax;

  vfrPrimSet*     m_shape;

  void adjustRange();
};

#endif // _VSN_METHOD_SCATTER_PLOTPOINTS_H_
