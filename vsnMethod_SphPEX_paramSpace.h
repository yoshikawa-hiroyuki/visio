//
// vsnMethod_SphPEX_paramSpace
//
#ifndef _VSN_METHOD_SPH_PEX_PARAMSPACE_H_
#define _VSN_METHOD_SPH_PEX_PARAMSPACE_H_

#include "wx/defs.h"
#include "wx/slider.h"
#include "wx/textctrl.h"

#include "vsnMethodObj.h"
#include "vsnData_SphPEX.h"
#include "vsnFrontLabel.h"

namespace VSN {
  // control ids
  enum {MPP_SphPEX_paramSpace_IndexSld = 5050,
	MPP_SphPEX_paramSpace_IndexTxt,
	MPP_SphPEX_paramSpace_LeftBtn,
	MPP_SphPEX_paramSpace_RightBtn,
	MPP_SphPEX_paramSpace_ShowLabelChk,
	MPP_SphPEX_paramSpace_LabelFmtTxt,
	MPP_SphPEX_paramSpace_LabelPosXTxt,
	MPP_SphPEX_paramSpace_LabelPosYTxt,
	MPP_SphPEX_paramSpace_LabelSizeTxt
  };
};


//----------------------------------------------------------------
// class vsnMPP_SphPEX_paramSpace
//   method parameter-panel for vsnMethod_SphPEX_paramSpace
//----------------------------------------------------------------
class vsnMPP_SphPEX_paramSpace : public vsnMethodPP {
public:
  vsnMPP_SphPEX_paramSpace(wxPanel* parent, vsnMethodObj* pm);
  virtual ~vsnMPP_SphPEX_paramSpace();

  // interface
  virtual bool update();

  // event handler
  void OnIndexSlider(wxScrollEvent& event);
  void OnIndexTxt(wxCommandEvent& event);
  void OnLeftBtn(wxCommandEvent& event);
  void OnRightBtn(wxCommandEvent& event);
  void OnShowLabelChk(wxCommandEvent& event);
  void OnLabelTxt(wxCommandEvent& event);

private:
  wxSlider*   m_pIndexSlider;
  wxTextCtrl* m_pIndexTxt;
  wxButton*   m_pLeftBtn;
  wxButton*   m_pRightBtn;
  wxCheckBox* m_pShowLabelChk;
  wxTextCtrl* m_pLabelFmtTxt;
  wxTextCtrl* m_pLabelPosXTxt;
  wxTextCtrl* m_pLabelPosYTxt;
  wxTextCtrl* m_pLabelSizeTxt;

  DECLARE_EVENT_TABLE()
};


//----------------------------------------------------------------
// class vsnMethod_SphPEX_paramSpace
//----------------------------------------------------------------
class vsnMethod_SphPEX_paramSpace
  : public vsnMethodObj
{
public:
  vsnMethod_SphPEX_paramSpace(const std::string& nm=std::string(VFR_NONAME));
  virtual ~vsnMethod_SphPEX_paramSpace();

  // param index interface
  size_t getNumParamIdx() const;
  size_t getCurrentParamIdx() const;
  bool setCurrentParamIdx(const size_t idx);

  // label interface
  vsnFrontLabel* getFrontLabel();
  bool showFrontLabel(const bool sfl);
  bool isShowFrontLabel() const;
  void setLabelFmt(const std::string& fmt);
  std::string getLabelFmt() const {return m_labelFmt;}

  // from vsnMethodObj
  virtual std::deque<std::string> getDataTypes() const {
    std::deque<std::string> r;
    r.push_back(std::string("SphPEX"));
    return r;
  }
  virtual std::string getMethodType() const {
    return std::string("paramSpace");
  }
  virtual bool canLighting() const {return false;}
  virtual bool update(const bool force =true);
  virtual vsnMethodPP* getParamPanel(wxPanel* pp);
  virtual void setBaseColor(const vector4 cv);
  virtual void setShow(const bool mode);

  // from vsnIoObject
  virtual bool parseXML(xmlNodePtr xnp);
  virtual bool outputXML(std::ostream& os, const size_t ts =0);
  virtual bool commandXML(xmlNodePtr xnp);

private:
  size_t m_currentIdx;

  std::string    m_labelFmt;
  vsnFrontLabel* m_pFrLbl;
  void updateLabel();
};

#endif // _VSN_METHOD_SPH_PEX_PARAMSPACE_H_
