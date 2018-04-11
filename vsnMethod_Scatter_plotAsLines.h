//
// vsnMethod_Scatter_plotAsLines
//
#ifndef _VSN_METHOD_SCATTER_PLOTASLINES_H_
#define _VSN_METHOD_SCATTER_PLOTASLINES_H_

#include "wx/defs.h"
#include "wx/listbox.h"
#include "wx/combobox.h"
#include "wx/checklst.h"
#include "wx/textctrl.h"
#include "wx/button.h"
#include "wx/checkbox.h"

#include "vsnMethodObj.h"
#include "vsnData_Scatter.h"

#include "vfrLines.h"
#include "vsnPtSet.h"

namespace VSN {
  // control ids
  enum {MPP_Scatter_plotAsLines_LineTypeRadio = 3700,
	MPP_Scatter_plotAsLines_SelDataLst,
	MPP_Scatter_plotAsLines_VecDataChkLst,
	MPP_Scatter_plotAsLines_UpdMinMaxChk,
	MPP_Scatter_plotAsLines_LineWidthTxt,
	MPP_Scatter_plotAsLines_AntiAliasChk,

	MPP_Scatter_plotAsLines2_LineTypeRadio,
	MPP_Scatter_plotAsLines2_LineWidthTxt,
	MPP_Scatter_plotAsLines2_AntiAliasChk
  };
};


//----------------------------------------------------------------
// class vsnMPP_Scatter_plotAsLines
//   method parameter-panel for vsnMethod_Scatter_plotAsLines
//----------------------------------------------------------------
class vsnMPP_Scatter_plotAsLines : public vsnMethodPP {
public:
  vsnMPP_Scatter_plotAsLines(wxPanel* parent, vsnMethodObj* pm);
  virtual ~vsnMPP_Scatter_plotAsLines();

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
// class vsnMethod_Scatter_plotAsLines
//----------------------------------------------------------------
class vsnMethod_Scatter_plotAsLines
  : public vsnMethodObj,
    public vsnTimeSeriesMethodIF
{
public:
  vsnMethod_Scatter_plotAsLines(const std::string& nm=std::string(VFR_NONAME));
  virtual ~vsnMethod_Scatter_plotAsLines();

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
    return std::string("plotAsLines");
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
  vfrLines*          m_shape;

  void adjustRange();
};


//----------------------------------------------------------------
// class vsnMPP_Scatter_plotAsLines2
//   method parameter-panel for vsnMethod_Scatter_plotAsLines2
//----------------------------------------------------------------
class vsnMPP_Scatter_plotAsLines2 : public vsnMethodPP {
public:
  vsnMPP_Scatter_plotAsLines2(wxPanel* parent, vsnMethodObj* pm);
  virtual ~vsnMPP_Scatter_plotAsLines2();

  // interface
  virtual bool update();

  // event handler
  void OnLineTypeRadio(wxCommandEvent& event);
  void OnLineWidthTxt(wxCommandEvent& event);
  void OnAntiAliasChk(wxCommandEvent& event);

private:
  wxRadioBox*     m_pLineTypeRadio;
  wxTextCtrl*     m_pLineWidthTxt;
  wxCheckBox*     m_pAntiAliasChk;

  DECLARE_EVENT_TABLE()
};


//----------------------------------------------------------------
// class vsnMethod_Scatter_plotAsLines2
//----------------------------------------------------------------
class vsnMethod_Scatter_plotAsLines2
  : public vsnMethodObj,
    public vsnTimeSeriesMethodIF
{
public:
  vsnMethod_Scatter_plotAsLines2(const std::string& nm=std::string(VFR_NONAME));
  virtual ~vsnMethod_Scatter_plotAsLines2();

  // methods
  VFR::StippleType getLineType() const {return m_lineType;}
  virtual bool setLineType(const VFR::StippleType st);

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
    return std::string("plotAsLines2");
  }
  virtual bool update(const bool force =true);
  virtual vsnMethodPP* getParamPanel(wxPanel* pp);

  // from vsnIoObject
  virtual bool parseXML(xmlNodePtr xnp);
  virtual bool outputXML(std::ostream& os, const size_t ts =0);
  virtual bool commandXML(xmlNodePtr xnp);

protected:
  VFR::StippleType   m_lineType;
  float              m_lineWidth;
  vsnPtSet*          m_shape;
};

#endif // _VSN_METHOD_SCATTER_PLOTASLINES_H_
