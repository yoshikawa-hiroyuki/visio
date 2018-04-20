//
// vsnMethod_Scatter_plotAsLineStrip
//
#ifndef _VSN_METHOD_SCATTER_PLOTASLINESTRIP_H_
#define _VSN_METHOD_SCATTER_PLOTASLINESTRIP_H_

#include "wx/defs.h"
#include "wx/listbox.h"
#include "wx/combobox.h"
#include "wx/checklst.h"
#include "wx/textctrl.h"
#include "wx/button.h"
#include "wx/checkbox.h"

#include "vsnMethodObj.h"
#include "vsnData_Scatter.h"

#include "vfrLineStrip.h"

namespace VSN {
  // control ids
  enum {MPP_Scatter_plotAsLineStrip_LineTypeRadio = 3750,
	MPP_Scatter_plotAsLineStrip_SelDataLst,
	MPP_Scatter_plotAsLineStrip_VecDataChkLst,
	MPP_Scatter_plotAsLineStrip_UpdMinMaxChk,
	MPP_Scatter_plotAsLineStrip_LineWidthTxt,
	MPP_Scatter_plotAsLineStrip_AntiAliasChk,
  };
};


//----------------------------------------------------------------
// class vsnMPP_Scatter_plotAsLineStrip
//   method parameter-panel for vsnMethod_Scatter_plotAsLineStrip
//----------------------------------------------------------------
class vsnMPP_Scatter_plotAsLineStrip : public vsnMethodPP {
public:
  vsnMPP_Scatter_plotAsLineStrip(wxPanel* parent, vsnMethodObj* pm);
  virtual ~vsnMPP_Scatter_plotAsLineStrip();

  // interface
  virtual bool update();

  // event handler
  void OnLineTypeRadio(wxCommandEvent& event);
  void OnSelDataLst(wxCommandEvent& event);
  void OnVecDataChkLst(wxCommandEvent& event);
  void OnUpdMinMaxChk(wxCommandEvent& event);
  void OnLineWidthTxt(wxCommandEvent& event);
  void OnAntiAliasChk(wxCommandEvent& event);

private:
  wxRadioBox*     m_pLineTypeRadio;
  wxComboBox*     m_pSelDataLst;
  wxCheckListBox* m_pVecDataChkLst;
  wxCheckBox*     m_pUpdMinMaxChk;
  wxTextCtrl*     m_pLineWidthTxt;
  wxCheckBox*     m_pAntiAliasChk;

  DECLARE_EVENT_TABLE()
};


//----------------------------------------------------------------
// class vsnMethod_Scatter_plotAsLineStrip
//----------------------------------------------------------------
class vsnMethod_Scatter_plotAsLineStrip
  : public vsnMethodObj,
    public vsnTimeSeriesMethodIF
{
public:
  vsnMethod_Scatter_plotAsLineStrip(const std::string& nm
				    = std::string(VFR_NONAME));
  virtual ~vsnMethod_Scatter_plotAsLineStrip();

  // methods
  VFR::StippleType getLineType() const {return m_lineType;}
  virtual bool setLineType(const VFR::StippleType st);

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
  virtual std::deque<std::string> getDataTypes() const {
    std::deque<std::string> r; r.push_back(std::string("Scatter")); return r;
  }
  virtual std::string getMethodType() const {
    return std::string("plotAsLineStrip");
  }
  virtual bool update(const bool force =true);
  virtual void reloaded();
  virtual vsnMethodPP* getParamPanel(wxPanel* pp);

  // from vsnIoObject
  virtual bool parseXML(xmlNodePtr xnp);
  virtual bool outputXML(std::ostream& os, const size_t ts =0);
  virtual bool commandXML(xmlNodePtr xnp);

protected:
  VFR::StippleType   m_lineType;
  float              m_lineWidth;
  VSN::WhichDataType m_selectedData;
  CES::Vec3<int>     m_vecDataIdx;
  bool               m_updateMinMax;
  vfrLineStrip*      m_shape;

  void adjustRange();
};

#endif // _VSN_METHOD_SCATTER_PLOTASLINESTRIP_H_
