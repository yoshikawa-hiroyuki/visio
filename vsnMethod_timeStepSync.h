//
// vsnMethod_timeStepSync
//
#ifndef _VSN_METHOD_TIMESTEP_SYNC_H_
#define _VSN_METHOD_TIMESTEP_SYNC_H_

#include "wx/defs.h"
#include "wx/slider.h"
#include "wx/textctrl.h"

#include "vsnMethod_timeStep.h"
#include "vsnDataObj.h"
#include "vsnFrontLabel.h"

namespace VSN {
  // control ids
  enum {MPP_timeStepSync_RefTSDataTxt = 1950,
	MPP_timeStepSync_RefTSDataLst,
	MPP_timeStepSync_SetRefTSDataBtn,
	MPP_timeStepSync_CurStpTxt,
	MPP_timeStepSync_PauseChk,
	MPP_timeStepSync_ShowLabelChk,
        MPP_timeStepSync_LabelFmtTxt,
        MPP_timeStepSync_LabelPosXTxt,
        MPP_timeStepSync_LabelPosYTxt,
        MPP_timeStepSync_LabelSizeTxt
  };
};


//----------------------------------------------------------------
// class vsnMPP_timeStepSync
//   method parameter-panel for vsnMethod_timeStepSync
//----------------------------------------------------------------
class vsnMPP_timeStepSync : public vsnMethodPP {
public: 
  vsnMPP_timeStepSync(wxPanel* parent, vsnMethodObj* pm);
  virtual ~vsnMPP_timeStepSync();

  // interface
  virtual bool update();

  // event handler
  void OnEnterRefTSDataTxt(wxCommandEvent& event);
  void OnSetRefTSDataBtn(wxCommandEvent& event);
  void OnPauseChk(wxCommandEvent& event);
  void OnShowLabelChk(wxCommandEvent& event);
  void OnLabelTxt(wxCommandEvent& event);

private:
  wxTextCtrl*  m_pRefTSDataTxt;
  wxListBox*   m_pRefTSDataLst;
  wxButton*    m_pSetRefTSDataBtn;
  wxTextCtrl*  m_pCurStpTxt;
  wxCheckBox*  m_pPauseChk;
  wxCheckBox*  m_pShowLabelChk;
  wxTextCtrl*  m_pLabelFmtTxt;
  wxTextCtrl*  m_pLabelPosXTxt;
  wxTextCtrl*  m_pLabelPosYTxt;
  wxTextCtrl*  m_pLabelSizeTxt;

  DECLARE_EVENT_TABLE()
};


//----------------------------------------------------------------
// class vsnMethod_timeStepSync
//----------------------------------------------------------------
class vsnMethod_timeStepSync
  : public vsnMethodObj, public vsnTSDataRefer
{
public:
  vsnMethod_timeStepSync(const std::string& nm=std::string(VFR_NONAME));
  virtual ~vsnMethod_timeStepSync();

  // interface
  int  getTimeStep() const {return m_timeStep;}
  bool setPause(const bool pause);
  bool getPause() const {return m_pause;}

  // label interface
  vsnFrontLabel* getFrontLabel();
  bool showFrontLabel(const bool sfl);
  bool isShowFrontLabel() const;
  void setLabelFmt(const std::string& fmt);
  std::string getLabelFmt() const {return m_labelFmt;}

  // from vsnTSDataRefer
  virtual void noticeUpdate();

  // from vsnMethodObj
  virtual std::deque<std::string> getDataTypes() const {
    std::deque<std::string> r; r.push_back(std::string("*")); return r;
  }
  virtual std::string getMethodType() const {
    return std::string("timeStepSync");
  }
  virtual bool hasBbox() const {return false;}
  virtual bool canLighting() const {return false;}
  virtual bool update(const bool force =true);
  virtual vsnMethodPP* getParamPanel(wxPanel* pp);

  // from vsnIoObject
  virtual bool parseXML(xmlNodePtr xnp);
  virtual bool outputXML(std::ostream& os, const size_t ts =0);
  virtual bool commandXML(xmlNodePtr xnp);

private:
  bool setTimeStep(const int stp); // called by noticeUpdate()

  int m_timeStep;
  bool m_pause;

  std::string    m_labelFmt;
  vsnFrontLabel* m_pFrLbl;
  void updateLabel();

  volatile mutable bool m_onceSetTs;
};

#endif // _VSN_METHOD_TIMESTEP_SYNC_H_
