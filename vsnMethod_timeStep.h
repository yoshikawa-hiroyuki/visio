//
// vsnMethod_timeStep
//
#ifndef _VSN_METHOD_TIMESTEP_H_
#define _VSN_METHOD_TIMESTEP_H_

#include "wx/defs.h"
#include "wx/timer.h"
#include "wx/thread.h"
#include "wx/slider.h"
#include "wx/textctrl.h"

#include "vsnMethodObj.h"
#include "vsnGfxActions.h"
#include "vsnFrontLabel.h"
#include "vsnAnimFileCtx.h"

class vsnMethod_timeStep;

namespace VSN {
  // control ids
  enum {MPP_timeStep_StepSld = 1900,
	MPP_timeStep_StepTxt,
	MPP_timeStep_StartTxt,
	MPP_timeStep_EndTxt,
	MPP_timeStep_SkipTxt,
	MPP_timeStep_LeftBtn,
	MPP_timeStep_RightBtn,
	MPP_timeStep_ShowLabelChk,
	MPP_timeStep_LabelFmtTxt,
	MPP_timeStep_LabelPosXTxt,
	MPP_timeStep_LabelPosYTxt,
	MPP_timeStep_LabelSizeTxt,
	MPP_timeStep_AnimPlayBtn,
	MPP_timeStep_AnimStopBtn,
	MPP_timeStep_AnimRewBtn,
	MPP_timeStep_AnimFwdBtn,
	MPP_timeStep_AnimLoopChk,
	MPP_timeStep_AnimSshotChk,
	MPP_timeStep_AnimSshotFileTxt,
	MPP_timeStep_AnimSshotFileBtn,
	MPP_timeStep_AnimSshotViewTxt,
	MPP_timeStep_AnimSshotViewBtn
  };
};


//----------------------------------------------------------------
// class vsnMPP_timeStep
//   method parameter-panel for vsnMethod_timeStep
//----------------------------------------------------------------
class vsnMPP_timeStep : public vsnMethodPP {
public: 
  vsnMPP_timeStep(wxPanel* parent, vsnMethodObj* pm);
  virtual ~vsnMPP_timeStep();

  // interface
  virtual bool update();
  virtual void settlement();

  // event handler
  void OnStepSld(wxScrollEvent& event);
  void OnStepTxt(wxCommandEvent& event);
  void OnSkipTxt(wxCommandEvent& event);
  void OnStartTxt(wxCommandEvent& event);
  void OnEndTxt(wxCommandEvent& event);
  void OnLeftBtn(wxCommandEvent& event);
  void OnRightBtn(wxCommandEvent& event);
  void OnShowLabelChk(wxCommandEvent& event);
  void OnLabelTxt(wxCommandEvent& event);
  void OnAnimPlayBtn(wxCommandEvent& event);
  void OnAnimStopBtn(wxCommandEvent& event);
  void OnAnimRewBtn(wxCommandEvent& event);
  void OnAnimFwdBtn(wxCommandEvent& event);
  void OnAnimLoopChk(wxCommandEvent& event);
  void OnAnimSshotChk(wxCommandEvent& event);
  void OnAnimSshotFileTxt(wxCommandEvent& event);
  void OnAnimSshotFileBtn(wxCommandEvent& event);
  void OnAnimSshotViewTxt(wxCommandEvent& event);
  void OnAnimSshotViewBtn(wxCommandEvent& event);
  void OnChar(wxKeyEvent& event);

private:
  wxSlider*    m_pStepSld;
  wxTextCtrl*  m_pStepTxt;
  wxTextCtrl*  m_pStartTxt;
  wxTextCtrl*  m_pEndTxt;
  wxTextCtrl*  m_pSkipTxt;
  wxButton*    m_pLeftBtn;
  wxButton*    m_pRightBtn;
  wxCheckBox*  m_pShowLabelChk;
  wxTextCtrl*  m_pLabelFmtTxt;
  wxTextCtrl*  m_pLabelPosXTxt;
  wxTextCtrl*  m_pLabelPosYTxt;
  wxTextCtrl*  m_pLabelSizeTxt;
  wxButton*    m_pAnimPlayBtn;
  wxButton*    m_pAnimStopBtn;
  wxButton*    m_pAnimRewBtn;
  wxButton*    m_pAnimFwdBtn;
  wxCheckBox*  m_pAnimLoopChk;
  wxCheckBox*  m_pAnimSshotChk;
  wxTextCtrl*  m_pAnimSshotFileTxt;
  wxButton*    m_pAnimSshotFileBtn;
  wxTextCtrl*  m_pAnimSshotViewTxt;
  wxButton*    m_pAnimSshotViewBtn;

  //-------- TimeStepKeyAct : key-in action class --------
  class TimeStepKeyAct : public vsnGfxAct_KeyIn {
  public:
    TimeStepKeyAct(vsnGfxView* pgv =NULL, vsnMethod_timeStep* pm =NULL);
    void setTimeStepMethod(vsnMethod_timeStep* pm);
    void execute(vfrEvent& e);
  private:
    vsnMethod_timeStep* p_method;
    friend class vsnMPP_timeStep;
  } gfxAct_KI;

  DECLARE_EVENT_TABLE()
};


//----------------------------------------------------------------
// class vsnMethod_timeStep
//----------------------------------------------------------------
class vsnMethod_timeStep
  : public vsnMethodObj,
    public vsnTimeSeriesMethodIF,
    public wxTimer
{
public:
  vsnMethod_timeStep(const std::string& nm=std::string(VFR_NONAME));
  virtual ~vsnMethod_timeStep();

  // interface
  bool setTimeStep(const int stp);
  int  getTimeStep() const {return m_timeStep;}
  bool getStepRange(int* range) const;
  void setSkipSteps(const int skip);
  int  getSkipSteps(bool initialVal =false) const;

  // label interface
  vsnFrontLabel* getFrontLabel();
  bool showFrontLabel(const bool sfl);
  bool isShowFrontLabel() const;
  void setLabelFmt(const std::string& fmt);
  std::string getLabelFmt() const {return m_labelFmt;}

  // animation interface
  void anim_play(const bool rewind =true);
  void anim_stop();
  void anim_setLoop(const bool mode);
  bool anim_getLoop() const {return m_animLoop;}
  void anim_setShotMode(const bool mode);
  bool anim_getShotMode() const {return m_animSshot;}
  void anim_setShotPath(const std::string& spath);
  std::string anim_getShotPath() const {return m_animShotPath;}
  void anim_setShotView(const std::string& sview);
  std::string anim_getShotView() const {return m_animShotView;}

  // from vsnMethodObj
  virtual std::deque<std::string> getDataTypes() const {
    std::deque<std::string> r; r.push_back(std::string("*")); return r;
  }
  virtual std::string getMethodType() const {return std::string("timeStep");}
  virtual bool hasBbox() const {return false;}
  virtual bool canLighting() const {return false;}
  virtual bool update(const bool force =true);
  virtual vsnMethodPP* getParamPanel(wxPanel* pp);
  virtual void setBaseColor(const vector4 cv);
  virtual void setShow(const bool mode);

  // from vsnTimeSeriesMethodIF
  virtual bool updateStep(const int stp,
                          const bool force =true, const bool cascade =true);

  // from vsnIoObject
  virtual bool parseXML(xmlNodePtr xnp);
  virtual bool outputXML(std::ostream& os, const size_t ts =0);
  virtual bool commandXML(xmlNodePtr xnp);

  // from wxTimer
  virtual void Notify();

private:
  int m_timeStep;
  int m_skipSteps;

  std::string    m_labelFmt;
  vsnFrontLabel* m_pFrLbl;
  void updateLabel();

  volatile mutable bool m_onceSetTs;
  volatile mutable bool m_needRewind;

  // members for animation
  vsnAnimFileCtx
              m_animCtx;
  int         m_animIdx;
  wxMutex     m_animInProcMutex;
  bool        m_animLoop;
  bool        m_animSshot;
  std::string m_animShotPath;
  std::string m_animShotView;

  void animTimerJob();
};

#endif // _VSN_METHOD_TIMESTEP_H_
