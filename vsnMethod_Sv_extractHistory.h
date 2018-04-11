//
// vsnMethod_Sv_extractHistory
//
#ifndef _VSN_METHOD_SV_EXTRACT_HISTORY_H_
#define _VSN_METHOD_SV_EXTRACT_HISTORY_H_

#include "wx/defs.h"
#include "wx/listbox.h"
#include "wx/textctrl.h"
#include "wx/button.h"
#include "wx/checkbox.h"

#include "vsnMethodObj.h"
#include "vsnMethod_sampler.h"
#include "vsnData_Sv.h"

namespace VSN {
  // control ids
  enum {MPP_Sv_extractHistory_SamplerLst = 5450,
	MPP_Sv_extractHistory_SamplerTxt,
	MPP_Sv_extractHistory_SetSamplerBtn,
	MPP_Sv_extractHistory_OutfileTxt,
	MPP_Sv_extractHistory_OutfileBrwsBtn,
	MPP_Sv_extractHistory_ExtractBtn,
	MPP_Sv_extractHistory_AlwaysOutputChk,
	MPP_Sv_extractHistory_ExtractorTxt,
	MPP_Sv_extractHistory_ExtractorBrwsBtn,
	MPP_Sv_extractHistory_DummyValTxt,
  };
};


//----------------------------------------------------------------
// class vsnMPP_Sv_extractHistory
//   method parameter-panel for vsnMethod_Sv_extractHistory
//----------------------------------------------------------------
class vsnMPP_Sv_extractHistory : public vsnMethodPP {
public:
  vsnMPP_Sv_extractHistory(wxPanel* parent, vsnMethodObj* pm);
  virtual ~vsnMPP_Sv_extractHistory();

  // interface
  virtual bool update();

  // event handler
  void OnEnterSamplerTxt(wxCommandEvent& event);
  void OnSetSamplerBtn(wxCommandEvent& event);
  void OnOutfileTxt(wxCommandEvent& event);
  void OnOutfileBrwsBtn(wxCommandEvent& event);
  void OnExtractBtn(wxCommandEvent& event);
  void OnAlwaysOutputChk(wxCommandEvent& event);
  void OnExtractorTxt(wxCommandEvent& event);
  void OnExtractorBrwsBtn(wxCommandEvent& event);
  void OnDummyValTxt(wxCommandEvent& event);

private:
  wxTextCtrl*     m_pSamplerTxt;
  wxListBox*      m_pSamplerLst;
  wxButton*       m_pSetSamplerBtn;
  wxTextCtrl*     m_pOutfileTxt;
  wxButton*       m_pOutfileBrwsBtn;
  wxButton*       m_pExtractBtn;
  wxCheckBox*     m_pAlwaysOutputChk;
  wxTextCtrl*     m_pExtractorTxt;
  wxButton*       m_pExtractorBrwsBtn;
  wxTextCtrl*     m_pDummyValTxt;

  DECLARE_EVENT_TABLE()
};


//----------------------------------------------------------------
// class vsnMethod_Sv_extractHistory
//----------------------------------------------------------------
class vsnMethod_Sv_extractHistory
  : public vsnMethodObj,
    public vsnRef_sampler {
public:
  vsnMethod_Sv_extractHistory(const std::string& nm=std::string(VFR_NONAME));
  virtual ~vsnMethod_Sv_extractHistory();

  std::string getOutfile() const {return m_outfile;}
  bool setOutfile(const std::string& outfile);

  float getDummyVal() const {return m_dummyVal;}
  bool setDummyVal(const float ob);

  bool getAlways() const {return m_always;}
  bool setAlways(const bool am);

  std::string getExtractorPath() const {return s_extractorPath;}
  bool setExtractorPath(const std::string& extractorPath);

  bool extract(const std::string& path);

  // from vsnRef_sampler
  bool setSampler(const string& samplerName); // not virtual
  virtual void noticeUpdate();

  // from vsnMethodObj
  virtual std::deque<std::string> getDataTypes() const {
    std::deque<std::string> r;
    r.push_back(std::string("Sph"));
    r.push_back(std::string("P3dF"));
    r.push_back(std::string("DfiSv"));
    return r;
  }
  virtual std::string getMethodType() const {
    return std::string("extractHistory");
  }
  virtual bool hasBbox() const {return false;}
  virtual bool canLighting() const {return false;}
  virtual bool update(const bool force =true);
  virtual vsnMethodPP* getParamPanel(wxPanel* pp);

  // from vsnIoObject
  virtual bool parseXML(xmlNodePtr xnp);
  virtual bool outputXML(std::ostream& os, const size_t ts =0);
  virtual bool commandXML(xmlNodePtr xnp);

protected:
  std::string m_outfile;
  float m_dummyVal;
  bool m_always;

  string generateInputFile() const;

  static std::string s_extractorPath;
  static bool setupExtractor();
};

#endif // _VSN_METHOD_SV_EXTRACT_HISTORY_H_
