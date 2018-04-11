//
// vsnMethod_streamLines
//
#ifndef _VSN_METHOD_STREAMLINES_H_
#define _VSN_METHOD_STREAMLINES_H_

#include "wx/defs.h"
#include "wx/listbox.h"
#include "wx/textctrl.h"
#include "wx/button.h"
#include "wx/checkbox.h"

#include "vsnMethodObj.h"
#include "vsnMethod_sampler.h"
#include "vsnData_Sv.h"
#include "vsnPtSet.h"
#include "vsnGridUtilSv.h"

namespace VSN {
  // control ids
  enum {MPP_streamLines_SamplerLst = 2400,
        MPP_streamLines_SamplerTxt,
        MPP_streamLines_SetSamplerBtn,
        MPP_streamLines_VecDataChkLst,
        MPP_streamLines_DivTimeTxt,
        MPP_streamLines_SkipOutTxt,
        MPP_streamLines_MaxPtsTxt,
        MPP_streamLines_LineWidthTxt,
        MPP_streamLines_ColoredChk,
        MPP_streamLines_AntiAliasChk,
        MPP_streamLines_AlwaysChk,
        MPP_streamLines_RunBtn,
	MPP_streamLines_ExportBtn,
	MPP_streamLines_AutoExportChk
  };
};


//----------------------------------------------------------------
// class vsnMPP_streamLines
//   method parameter-panel for vsnMethod_streamLines
//----------------------------------------------------------------
class vsnMPP_streamLines : public vsnMethodPP {
public:
  vsnMPP_streamLines(wxPanel* parent, vsnMethodObj* pm);
  virtual ~vsnMPP_streamLines();

  // interface
  virtual bool update();

  // event handler
  void OnEnterSamplerTxt(wxCommandEvent& event);
  void OnSetSamplerBtn(wxCommandEvent& event);
  void OnVecDataChkLst(wxCommandEvent& event);
  void OnDivTimeTxt(wxCommandEvent& event);
  void OnSkipOutTxt(wxCommandEvent& event);
  void OnMaxPtsTxt(wxCommandEvent& event);
  void OnLineWidthTxt(wxCommandEvent& event);
  void OnColoredChk(wxCommandEvent& event);
  void OnAntiAliasChk(wxCommandEvent& event);
  void OnAlwaysChk(wxCommandEvent& event);
  void OnRunBtn(wxCommandEvent& event);
  void OnExportBtn(wxCommandEvent& event);
  void OnAutoExportChk(wxCommandEvent& event);

private:
  wxTextCtrl*     m_pSamplerTxt;
  wxListBox*      m_pSamplerLst;
  wxButton*       m_pSetSamplerBtn;
  wxCheckListBox* m_pVecDataChkLst;
  wxTextCtrl*     m_pDivTimeTxt;
  wxTextCtrl*     m_pSkipOutTxt;
  wxTextCtrl*     m_pMaxPtsTxt;
  wxTextCtrl*     m_pLineWidthTxt;
  wxCheckBox*     m_pColoredChk;
  wxCheckBox*     m_pAntiAliasChk;
  wxCheckBox*     m_pAlwaysChk;
  wxButton*       m_pRunBtn;
  wxButton*       m_pExportBtn;
  wxCheckBox*     m_pAutoExportChk;

  DECLARE_EVENT_TABLE()
};


//----------------------------------------------------------------
// class vsnMethod_streamLines
//----------------------------------------------------------------
class vsnMethod_streamLines
  : public vsnMethodObj,
    public vsnRef_sampler,
    public vsnTimeSeriesMethodIF {
public:
  vsnMethod_streamLines(const std::string& nm=std::string(VFR_NONAME));
  virtual ~vsnMethod_streamLines();

  CES::Vec3<int> getVecDataIdx() const {return m_vecDataIdx;}
  bool setVecDataIdx(const CES::Vec3<int>& vdidx);
  bool isValidVecData() const;

  int getDivTime() const {return m_divTime;}
  bool setDivTime(const int divt);

  int getNumSkip() const {return m_numSkip;}
  bool setNumSkip(const int ns);

  int getMaxPts() const {return m_maxPts;}
  bool setMaxPts(const int mpt);

  float getLineWidth() const {return m_lineWidth;}
  bool setLineWidth(const float lw);

  bool getColored() const {return m_colored;}
  bool setColored(const bool cm);

  bool getAlways() const {return m_always;}
  bool setAlways(const bool am);

  bool requestCalc() {
    m_needUpd = true;
    return update();
  }

  bool exportLines(const std::string& path);
  std::string getExportedPath() const {return m_exportPath;}
  bool setAutoExport(const bool aem);
  bool getAutoExport() const {return m_autoExport;}

  // from vsnRef_sampler
  bool setSampler(const string& samplerName);

  // from vsnTimeSeriesMethodIF
  //  need to implement
  //virtual bool updateStep(const int stp,
  //                        const bool force =true, const bool cascade =true);

  // from vsnMethodObj
  virtual std::deque<std::string> getDataTypes() const {
    std::deque<std::string> r; r.push_back(std::string("*")); return r;
  }
  virtual std::string getMethodType() const {
    return std::string("streamLines");
  }
  virtual bool canLighting() const {return false;}
  virtual bool update(const bool force =true);
  virtual vsnMethodPP* getParamPanel(wxPanel* pp);

  // from vsnIoObject
  virtual bool parseXML(xmlNodePtr xnp);
  virtual bool outputXML(std::ostream& os, const size_t ts =0);
  virtual bool commandXML(xmlNodePtr xnp);

  // from vsnRef_sampler
  virtual void noticeUpdate();

protected:
  vsnPtSet* m_lines;

  CES::Vec3<int>
            m_vecDataIdx;
  int       m_divTime;
  int       m_numSkip;
  int       m_maxPts;
  float     m_lineWidth;
  bool      m_colored;
  bool      m_always;
  bool      m_needUpd;
  std::string
            m_exportPath;
  bool      m_autoExport;
};

#endif // _VSN_METHOD_STREAMLINES_H_
