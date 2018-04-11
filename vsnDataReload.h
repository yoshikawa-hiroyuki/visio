//
// vsnDataReload
//
#ifndef _VSN_DATA_RELOAD_H_
#define _VSN_DATA_RELOAD_H_

#include "wx/defs.h"
#include "wx/dialog.h"
#include "wx/textctrl.h"
#include "wx/listbox.h"
#include "wx/button.h"
#include "wx/statline.h"
#include "wx/timer.h"
#include "wx/thread.h"

#include "vsnDataObj.h"
#include <string>
#include <set>
#include <libxml/tree.h>

namespace VSN {
  // control ids
  enum {DataReloadDlg_AutoChk =1450,
	DataReloadDlg_IntervalTxt,
	DataReloadDlg_DirTxt,
	DataReloadDlg_FileList,
	DataReloadDlg_PatternTxt,
	DataReloadDlg_OkBtn,
	DataReloadDlg_CancelBtn
  };
};


//----------------------------------------------------------------
// class vsnDataReload
//----------------------------------------------------------------
class vsnDataReload : public wxTimer {
public:
  vsnDataReload(vsnDataObj* pRefData);
  virtual ~vsnDataReload();

  bool Start();
  bool checkFiles();
  bool update();
  bool setDataObj(vsnDataObj* pRefData);
  vsnDataObj* getDataObj() {return p_refData;}

  bool isAuto() const {return m_auto;}
  void setAuto(const bool am);
  float getInterval() const {return m_interval;}
  bool setInterval(const float its);
  std::string getPattern() const {return m_filePattern;}
  bool setPattern(const std::string& patStr);

  std::string getDir() const {return m_dir;}
  std::set<std::string> getFileList() const {return m_fileList;}

  // from wxTimer
  virtual void Notify();

protected:
  bool m_auto;
  float m_interval;
  std::string m_dir;
  std::set<std::string> m_fileList;
  std::string m_filePattern;
  wxMutex m_mutexTimerJob;
  vsnDataObj* p_refData;
};


//----------------------------------------------------------------
// class vsnDataReloadDlg
//----------------------------------------------------------------
class vsnDataReloadDlg : public wxDialog {
public:
  vsnDataReloadDlg(wxWindow *parent, vsnDataObj* pRefData);
  virtual ~vsnDataReloadDlg();

  // interface
  void update();
  void setRefData(vsnDataObj* pRefData);
  vsnDataObj* getRefData() {return p_refData;}

  // event handler
  void OnOkBtn(wxCommandEvent& event);
  void OnCancelBtn(wxCommandEvent& event);
  void OnClose(wxCloseEvent& event);

private:
  wxCheckBox* m_pAutoChk;
  wxTextCtrl* m_pIntervalTxt;
  wxTextCtrl* m_pDirTxt;
  wxListBox*  m_pFileList;
  wxTextCtrl* m_pPatternTxt;
  wxButton*   m_pOkBtn;
  wxButton*   m_pCancelBtn;
  vsnDataObj* p_refData;

  DECLARE_EVENT_TABLE()
};

#endif // _VSN_DATA_RELOAD_H_
