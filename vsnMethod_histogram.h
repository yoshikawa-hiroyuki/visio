//
// vsnMethod_histogram
//
#ifndef _VSN_METHOD_HISTOGRAM_H_
#define _VSN_METHOD_HISTOGRAM_H_

#include "wx/defs.h"
#include "wx/checklst.h"
#include "wx/textctrl.h"
#include "wx/button.h"
#include "wx/combobox.h"
#include "wx/checkbox.h"

#include "vsnMethodObj.h"
#include "vsnDataObj.h"
#include "vsnGnuplotIF.h"

namespace VSN {
  // control ids
  enum {MPP_histogram_ProgramTxt = 2500,
	MPP_histogram_ProgBrwsBtn,
	MPP_histogram_PreCommandTxt,
	MPP_histogram_SelDataLst,
	MPP_histogram_VecDataChkLst,
	MPP_histogram_NumClassTxt,
	MPP_histogram_LogscaleChk,
	MPP_histogram_ShowTitleChk
  };
};


//----------------------------------------------------------------
// class vsnMPP_histogram
//   method parameter-panel for vsnMethod_histogram
//----------------------------------------------------------------
class vsnMPP_histogram : public vsnMethodPP {
public:
  vsnMPP_histogram(wxPanel* parent, vsnMethodObj* pm);
  virtual ~vsnMPP_histogram();

  // interface
  virtual bool update();

  // event handler
  void OnEnterProgramTxt(wxCommandEvent& event);
  void OnProgBrwsBtn(wxCommandEvent& event);
  void OnEnterPreCommandTxt(wxCommandEvent& event);
  void OnSelDataLst(wxCommandEvent& event);
  void OnVecDataChkLst(wxCommandEvent& event);
  void OnEnterNumClassTxt(wxCommandEvent& event);
  void OnLogscaleChk(wxCommandEvent& event);
  void OnShowTitleChk(wxCommandEvent& event);

private:
  wxTextCtrl*     m_pProgramTxt;
  wxButton*       m_pProgBrwsBtn;
  wxTextCtrl*     m_pPreCommandTxt;
  wxComboBox*     m_pSelDataLst;
  wxCheckListBox* m_pVecDataChkLst;
  wxTextCtrl*     m_pNumClassTxt;
  wxCheckBox*     m_pLogscaleChk;
  wxCheckBox*     m_pShowTitleChk;

  DECLARE_EVENT_TABLE()
};


//----------------------------------------------------------------
// class vsnMethod_histogram
//----------------------------------------------------------------
class vsnMethod_histogram : public vsnMethodObj {
public:
  vsnMethod_histogram(const std::string& nm=std::string(VFR_NONAME));
  virtual ~vsnMethod_histogram();

  std::string getProgramName() const {return m_progName;}
  bool setProgramName(const std::string& progName);

  std::string getPreCommand() const {return m_preCommand;}
  bool setPreCommand(const std::string& preCmd);

  VSN::WhichDataType getSelectedData() const {return m_selectedData;}
  bool setSelectedData(const VSN::WhichDataType sd);

  CES::Vec3<int> getVecDataIdx() const {return m_vecDataIdx;}
  bool setVecDataIdx(const CES::Vec3<int>& vdidx);
  bool isValidVecData() const;

  bool setNumClasses(const size_t nc);
  size_t getNumClasses() const {return m_numClasses;}

  bool setLogscale(const bool lm);
  bool getLogscale() const {return m_logscale;}

  bool setShowTitle(const bool stm);
  bool getShowTitle() const {return m_showTitle;}

  // from vsnMethodObj
  //   need to implement getDataTypes() in derived class
  //   need to implement update() in derived class
  virtual std::string getMethodType() const {
    return std::string("histogram");
  }
  virtual bool canLighting() const {return false;}
  virtual vsnMethodPP* getParamPanel(wxPanel* pp);

  // from vsnIoObject
  virtual bool parseXML(xmlNodePtr xnp);
  virtual bool outputXML(std::ostream& os, const size_t ts =0);
  virtual bool commandXML(xmlNodePtr xnp);

protected:
  vsnGnuplotIF*  m_pGnuPlotIF;

  std::string    m_progName;
  std::string    m_preCommand;
  VSN::WhichDataType
                 m_selectedData;
  CES::Vec3<int> m_vecDataIdx;
  size_t         m_numClasses;
  bool           m_logscale;
  bool           m_showTitle;
  std::string    m_tmpPath;

  bool DoPlot(const size_t sampleSize, const vector2* sampleVals,
              const float range[2], const std::string& titleStr) const;
};

#endif // _VSN_METHOD_HISTOGRAM_H_
