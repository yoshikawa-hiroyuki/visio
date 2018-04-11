//
// vsnMethod_plotVector
//
#ifndef _VSN_METHOD_PLOTVECTOR_H_
#define _VSN_METHOD_PLOTVECTOR_H_

#include "wx/defs.h"
#include "wx/listbox.h"
#include "wx/combobox.h"
#include "wx/checklst.h"
#include "wx/textctrl.h"
#include "wx/button.h"
#include "wx/checkbox.h"

#include "vsnMethodObj.h"
#include "vsnMethod_sampler.h"
#include "vsnDataObj.h"
#include "vfrVectors.h"

namespace VSN {
  // control ids
  enum {MPP_plotVector_SamplerLst = 2200,
	MPP_plotVector_SamplerTxt,
	MPP_plotVector_SetSamplerBtn,
	MPP_plotVector_VecScaleTxt,
	MPP_plotVector_VecHeadChk,
	MPP_plotVector_VecDataChkLst,
	MPP_plotVector_SelDataLst,
	MPP_plotVector_UpdMinMaxChk,
	MPP_plotVector_LineWidthTxt,
	MPP_plotVector_AntiAliasChk
  };
};


//----------------------------------------------------------------
// class vsnMPP_plotVector
//   method parameter-panel for vsnMethod_plotVector
//----------------------------------------------------------------
class vsnMPP_plotVector : public vsnMethodPP {
public: 
  vsnMPP_plotVector(wxPanel* parent, vsnMethodObj* pm);
  virtual ~vsnMPP_plotVector();

  // interface
  virtual bool update();

  // event handler
  void OnSetSamplerBtn(wxCommandEvent& event);
  void OnEnterSamplerTxt(wxCommandEvent& event);
  void OnVecScaleTxt(wxCommandEvent& event);
  void OnVecHeadChk(wxCommandEvent& event);
  void OnVecDataChkLst(wxCommandEvent& event);
  void OnSelDataLst(wxCommandEvent& event);
  void OnUpdMinMaxChk(wxCommandEvent& event);
  void OnLineWidthTxt(wxCommandEvent& event);
  void OnAntiAliasChk(wxCommandEvent& event);

private:
  wxListBox*      m_pSamplerLst;
  wxTextCtrl*     m_pSamplerTxt;
  wxButton*       m_pSetSamplerBtn;
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
// class vsnMethod_plotVector
//----------------------------------------------------------------
class vsnMethod_plotVector
  : public vsnMethodObj,
    public vsnTimeSeriesMethodIF,
    public vsnRef_sampler {
public:
  vsnMethod_plotVector(const std::string& nm=std::string(VFR_NONAME));
  virtual ~vsnMethod_plotVector();

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

  bool getUseSampler() const {return m_useSampler;}

  // from vsnTimeSeriesMethodIF
  //  need to implement
  //virtual bool updateStep(const int stp,
  //                        const bool force =true, const bool cascade =true);

  // from vsnMethodObj
  virtual std::deque<std::string> getDataTypes() const {
    std::deque<std::string> r; r.push_back(std::string("*")); return r;
  }
  virtual std::string getMethodType() const {
    return std::string("plotVector");
  }
  virtual bool canLighting() const {return false;}
  virtual bool update(const bool force =true);
  virtual void reloaded();
  virtual vsnMethodPP* getParamPanel(wxPanel* pp);

  // from vsnIoObject
  virtual bool parseXML(xmlNodePtr xnp);
  virtual bool outputXML(std::ostream& os, const size_t ts =0);
  virtual bool commandXML(xmlNodePtr xnp);

  // from vsnRef_sampler
  virtual void noticeUpdate();

protected:
  float          m_vecScale;
  bool           m_vecHead;
  VSN::WhichDataType
                 m_selectedData;
  CES::Vec3<int> m_vecDataIdx;
  bool           m_updateMinMax;
  float          m_lineWidth;
  bool           m_useSampler;

  vfrVectors*    m_vectors;

  void adjustRange();
};

#endif // _VSN_METHOD_PLOTVECTOR_H_
