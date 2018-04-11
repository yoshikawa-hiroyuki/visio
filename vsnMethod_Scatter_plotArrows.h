//
// vsnMethod_Scatter_plotArrows
//
#ifndef _VSN_METHOD_SCATTER_PLOTARROWS_H_
#define _VSN_METHOD_SCATTER_PLOTARROWS_H_

#include "wx/defs.h"
#include "wx/listbox.h"
#include "wx/combobox.h"
#include "wx/checklst.h"
#include "wx/textctrl.h"
#include "wx/button.h"
#include "wx/checkbox.h"

#include "vsnMethodObj.h"
#include "vsnData_Scatter.h"
#include "vfrVectors.h"

namespace VSN {
  // control ids
  enum {MPP_Scatter_plotArrows_VecScaleTxt = 3500,
	MPP_Scatter_plotArrows_VecHeadChk,
	MPP_Scatter_plotArrows_VecDataChkLst,
	MPP_Scatter_plotArrows_SelDataLst,
        MPP_Scatter_plotArrows_UpdMinMaxChk,
        MPP_Scatter_plotArrows_LineWidthTxt,
        MPP_Scatter_plotArrows_AntiAliasChk
  };
};


//----------------------------------------------------------------
// class vsnMPP_Scatter_plotArrows
//   method parameter-panel for vsnMethod_Scatter_plotArrows
//----------------------------------------------------------------
class vsnMPP_Scatter_plotArrows : public vsnMethodPP {
public:
  vsnMPP_Scatter_plotArrows(wxPanel* parent, vsnMethodObj* pm);
  virtual ~vsnMPP_Scatter_plotArrows();

  // interface
  virtual bool update();

  // event handler
  void OnVecScaleTxt(wxCommandEvent& event);
  void OnVecHeadChk(wxCommandEvent& event);
  void OnVecDataChkLst(wxCommandEvent& event);
  void OnSelDataLst(wxCommandEvent& event);
  void OnUpdMinMaxChk(wxCommandEvent& event);
  void OnLineWidthTxt(wxCommandEvent& event);
  void OnAntiAliasChk(wxCommandEvent& event);

private:
  wxTextCtrl*     m_pVecScaleTxt;
  wxCheckBox*     m_pVecHeadChk;
  wxCheckListBox* m_pVecDataChkLst;
  wxComboBox*     m_pSelDataLst;
  wxCheckBox*     m_pUpdMinMaxChk;
  wxTextCtrl*     m_pLineWidthTxt;
  wxCheckBox*     m_pAntiAliasChk;

  DECLARE_EVENT_TABLE()
};


//----------------------------------------------------------------
// class vsnMethod_Scatter_plotArrows
//----------------------------------------------------------------
class vsnMethod_Scatter_plotArrows
  : public vsnMethodObj,
    public vsnTimeSeriesMethodIF {
public:
  vsnMethod_Scatter_plotArrows(const std::string& nm=std::string(VFR_NONAME));
  virtual ~vsnMethod_Scatter_plotArrows();

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
  virtual std::deque<std::string> getDataTypes() const {
    std::deque<std::string> r; r.push_back(std::string("Scatter")); return r;
  }
  virtual std::string getMethodType() const {
    return std::string("plotArrows");
  }
  virtual bool canLighting() const {return false;}
  virtual bool update(const bool force =true);
  virtual void reloaded();
  virtual vsnMethodPP* getParamPanel(wxPanel* pp);

  // from vsnIoObject
  virtual bool parseXML(xmlNodePtr xnp);
  virtual bool outputXML(std::ostream& os, const size_t ts =0);
  virtual bool commandXML(xmlNodePtr xnp);

protected:
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

#endif // _VSN_METHOD_SCATTER_PLOTARROWS_H_
