//
// vsnTextEntryDlg
//
#ifndef _VSN_TEXT_ENTRY_DLG_H_
#define _VSN_TEXT_ENTRY_DLG_H_

#include "wx/defs.h"
#include "wx/dialog.h"
#include "wx/button.h"
#include "wx/textctrl.h"

namespace VSN {
  // control ids
  enum {TextEntryDlg_OkBtn=1650,
	TextEntryDlg_CancelBtn,
	TextEntryDlg_BrowsBtn
  };
};


//----------------------------------------------------------------
// class vsnTextEntryDlg
//----------------------------------------------------------------
class vsnTextEntryDlg : public wxDialog {
public:
  vsnTextEntryDlg(wxWindow *parent, const wxString& msg,
		  const wxString& caption = wxT("Please entertext"), 
		  const wxString& defaultValue = wxT(""));
  virtual ~vsnTextEntryDlg();

  wxString GetValue() const;
  void SetValue(const wxString& value);
  int ShowModal();

  // event handler
  void OnOkBtn(wxCommandEvent& event);
  void OnCancelBtn(wxCommandEvent& event);
  void OnClose(wxCloseEvent& event);

private:
  wxTextCtrl*        m_pValueTxt;
  wxButton*          m_pOkBtn;
  wxButton*          m_pCancelBtn;

  wxString           m_value;
  int                m_modal;

  DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------
// class vsnTextEntryBrowsDlg
//----------------------------------------------------------------
class vsnTextEntryBrowsDlg : public wxDialog {
public:
  vsnTextEntryBrowsDlg(wxWindow *parent, const wxString& msg,
		       const wxString& caption = wxT("Please enter text"), 
		       const wxString& defaultValue = wxT(""));
  virtual ~vsnTextEntryBrowsDlg();

  wxString GetValue() const;
  void SetValue(const wxString& value);
  int ShowModal();

  // event handler
  void OnOkBtn(wxCommandEvent& event);
  void OnCancelBtn(wxCommandEvent& event);
  void OnBrowsBtn(wxCommandEvent& event);
  void OnClose(wxCloseEvent& event);

private:
  wxTextCtrl*        m_pValueTxt;
  wxButton*          m_pBrowsBtn;
  wxButton*          m_pOkBtn;
  wxButton*          m_pCancelBtn;

  wxString           m_value;
  int                m_modal;

  DECLARE_EVENT_TABLE()
};

#endif // _VSN_TEXT_ENTRY_DLG_H_

