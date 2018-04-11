//
// vsnMethod_keyFrameAnim
//
#ifndef _VSN_METHOD_KEYFRAMEANIM_H_
#define _VSN_METHOD_KEYFRAMEANIM_H_

#include "wx/defs.h"
#include "wx/timer.h"
#include "wx/thread.h"
#include "wx/textctrl.h"

#include "vsnMethodObj.h"
#include "vsnKeyFrameAnim.h"
#include "vsnAnimFileCtx.h"


//----------------------------------------------------------------
namespace VSN {
  // control ids
  enum {MPP_keyFrameAnim_RewindBtn = 2600,
	MPP_keyFrameAnim_StopBtn,
	MPP_keyFrameAnim_PlayBtn,
	MPP_keyFrameAnim_LastBtn,
	MPP_keyFrameAnim_LoopChk,
	MPP_keyFrameAnim_WithTimeStepChk,
	MPP_keyFrameAnim_PlayerModeChk,
	MPP_keyFrameAnim_SshotChk,
	MPP_keyFrameAnim_SshotFileTxt,
	MPP_keyFrameAnim_SshotFileBtn,
	MPP_keyFrameAnim_TargetViewTxt,
	MPP_keyFrameAnim_TargetViewBtn,
	MPP_keyFrameAnim_EditBtn
  };
};

//----------------------------------------------------------------
// class vsnMPP_keyFrameAnim
//   method parameter-panel for vsnMethod_keyFrameAnim
//----------------------------------------------------------------
class vsnMPP_keyFrameAnim : public vsnMethodPP {
public: 
  vsnMPP_keyFrameAnim(wxPanel* parent, vsnMethodObj* pm);
  virtual ~vsnMPP_keyFrameAnim();

  // interface
  virtual bool update();

  // event handler
  void OnRewindBtn(wxCommandEvent& event);
  void OnStopBtn(wxCommandEvent& event);
  void OnPlayBtn(wxCommandEvent& event);
  void OnLastBtn(wxCommandEvent& event);
  void OnLoopChk(wxCommandEvent& event);
  void OnWithTimeStepChk(wxCommandEvent& event);
  void OnPlayerModeChk(wxCommandEvent& event);
  void OnSshotChk(wxCommandEvent& event);
  void OnSshotFileTxt(wxCommandEvent& event);
  void OnSshotFileBtn(wxCommandEvent& event);
  void OnTargetViewTxt(wxCommandEvent& event);
  void OnTargetViewBtn(wxCommandEvent& event);
  void OnEditBtn(wxCommandEvent& event);

private:
  wxButton*    m_pRewindBtn;
  wxButton*    m_pStopBtn;
  wxButton*    m_pPlayBtn;
  wxButton*    m_pLastBtn;
  wxCheckBox*  m_pLoopChk;
  wxCheckBox*  m_pWithTimeStepChk;
  wxCheckBox*  m_pPlayerModeChk;
  wxCheckBox*  m_pSshotChk;
  wxTextCtrl*  m_pSshotFileTxt;
  wxButton*    m_pSshotFileBtn;
  wxTextCtrl*  m_pTargetViewTxt;
  wxButton*    m_pTargetViewBtn;
  wxButton*    m_pEditBtn;

  DECLARE_EVENT_TABLE()
};


//----------------------------------------------------------------
// class vsnMethod_keyFrameAnim
//----------------------------------------------------------------
class vsnMethod_keyFrameAnim
  : public vsnMethodObj,
    public wxTimer
{
  friend class vsnKeyFrameAnimEditDlg;

public:
  vsnMethod_keyFrameAnim(const std::string& nm=std::string(VFR_NONAME));
  virtual ~vsnMethod_keyFrameAnim();

  // interface
  const vsnKeyFrameAnim& getKFA() const {return m_KFA;}
  vsnKeyFrameAnim& getKFA() {return m_KFA;}

  void reset();
  void play(const bool rewind =true);
  void stop();
  bool addKeyframe(const double tm);
  bool delKeyframe(const size_t idx);
  bool delKeyframe(const double tm);
  bool replaceKeyframe(const size_t idx);
  bool setCurrentTime(const double tm);
  double getCurrentTime() const {return m_timeKFA;}

  void setPlayerMode(const bool pm);
  bool getPlayerMode() const {return m_playerMode;}

  void setShotMode(const bool mode);
  bool getShotMode() const {return m_scrShot;}
  void setShotPath(const std::string& spath);
  std::string getShotPath() const {return m_scrShotPath;}

  void setTargetView(const std::string& tview);
  std::string getTargetView() const {return m_targetView;}

  void showEditDlg(const bool show);
  void updateEditDlg();

  // from vsnMethodObj
  virtual std::deque<std::string> getDataTypes() const {
    std::deque<std::string> r; r.push_back(std::string("*")); return r;
  }
  virtual std::string getMethodType() const {
    return std::string("keyFrameAnim");
  }
  virtual bool hasBbox() const {return false;}
  virtual bool canLighting() const {return false;}
  virtual bool update(const bool force =true);
  virtual vsnMethodPP* getParamPanel(wxPanel* pp);

  // from vsnIoObject
  virtual bool parseXML(xmlNodePtr xnp);
  virtual bool outputXML(std::ostream& os, const size_t ts =0);
  virtual bool commandXML(xmlNodePtr xnp);

  // from wxTimer
  virtual void Notify() {animTimerJob();}

private:
  std::string m_targetView;

  vsnKeyFrameAnim m_KFA;
  double m_timeKFA;
  volatile mutable bool m_needRewind;

  bool m_scrShot;
  std::string m_scrShotPath;

  vsnAnimFileCtx m_animFileCtx;
  int m_animFileIdx;
  bool m_playerMode;

  class vsnKeyFrameAnimEditDlg* m_pEditDlg;
  void clearEditDlg();

  wxMutex m_inProcMutex;
  void animTimerJob();
};

#endif // _VSN_METHOD_KEYFRAMEANIM_H_
