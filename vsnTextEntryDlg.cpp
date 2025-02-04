//
// vsnTextEntryDlg
//
#ifdef __GNUG__
#pragma implementation
#pragma interface
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif
#include "wx/statline.h"
#include "wx/tokenzr.h"

#include <sstream>
#include "vsnTextEntryDlg.h"
#include "vsnError.h"

using namespace std;
using namespace CES;
using namespace VSN;


//----------------------------------------------------------------
// class vsnTextEntryDlg
//----------------------------------------------------------------

BEGIN_EVENT_TABLE(vsnTextEntryDlg, wxDialog)
  EVT_BUTTON(TextEntryDlg_OkBtn, vsnTextEntryDlg::OnOkBtn)
  EVT_BUTTON(TextEntryDlg_CancelBtn, vsnTextEntryDlg::OnCancelBtn)
  EVT_CLOSE(vsnTextEntryDlg::OnClose)
END_EVENT_TABLE()


/* constructors, destructor */

vsnTextEntryDlg::vsnTextEntryDlg(wxWindow* parent,
        const wxString& msg, const wxString& caption,
        const wxString& defaultValue)
  : wxDialog(parent, -1, caption),
    m_pValueTxt(NULL), m_pOkBtn(NULL), m_pCancelBtn(NULL),
    m_modal(wxID_CANCEL)
{
  /* prepare parts */
  m_pValueTxt = new wxTextCtrl(this, -1, defaultValue);
  assert(m_pValueTxt);

  m_pOkBtn = new wxButton(this, TextEntryDlg_OkBtn, wxT("OK"));
  assert(m_pOkBtn);

  m_pCancelBtn = new wxButton(this, TextEntryDlg_CancelBtn, wxT("Cancel"));
  assert(m_pCancelBtn);

  /* top Sizer */
  wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL);
  topsizer->SetMinSize(300, 100);

  wxBoxSizer* sizerH;

  /* layout */
  topsizer->Add(new wxStaticText(this, -1, msg), 0, wxEXPAND|wxALL, 10);
  topsizer->Add(m_pValueTxt, 0, wxEXPAND|wxALL, 10);

  topsizer->Add(new wxStaticLine(this, -1, wxDefaultPosition,
                                 wxSize(1,1), wxHORIZONTAL),
                0, wxEXPAND|wxALL, 0);

  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxALIGN_RIGHT|wxALL, 5);
  sizerH->Add(m_pCancelBtn, 0, wxALIGN_RIGHT|wxALL, 3);
  sizerH->Add(m_pOkBtn, 0, wxALIGN_RIGHT|wxALL, 3);

  /* post process */
  SetAutoLayout(TRUE);
  SetSizer(topsizer);
  topsizer->SetSizeHints(this);
  topsizer->Fit(this);
}

vsnTextEntryDlg::~vsnTextEntryDlg() {
}


/* methods */

wxString vsnTextEntryDlg::GetValue() const {
  return m_value;
}

void vsnTextEntryDlg::SetValue(const wxString& value) {
  m_value = value;
  if ( m_pValueTxt ) m_pValueTxt->SetValue(m_value);
}

int vsnTextEntryDlg::ShowModal() {
  wxDialog::ShowModal();
  return m_modal;
}


/* event handler */

void vsnTextEntryDlg::OnOkBtn(wxCommandEvent& event) {
  if ( ! m_pValueTxt ) return;
  m_value = m_pValueTxt->GetValue();
  m_modal = wxID_OK;

  if ( IsModal() )
    EndModal(vsn_wxIDOK);
  else
    this->Hide();
}

void vsnTextEntryDlg::OnCancelBtn(wxCommandEvent& event) {
  if ( IsModal() )
    EndModal(vsn_wxIDCANCEL);
  else
    this->Hide();
}

void vsnTextEntryDlg::OnClose(wxCloseEvent& event) {
  if ( IsModal() )
    EndModal(vsn_wxIDCANCEL);
  else
    this->Hide();
}


//----------------------------------------------------------------
// class vsnTextEntryBrowsDlg
//----------------------------------------------------------------

BEGIN_EVENT_TABLE(vsnTextEntryBrowsDlg, wxDialog)
  EVT_BUTTON(TextEntryDlg_OkBtn, vsnTextEntryBrowsDlg::OnOkBtn)
  EVT_BUTTON(TextEntryDlg_CancelBtn, vsnTextEntryBrowsDlg::OnCancelBtn)
  EVT_BUTTON(TextEntryDlg_BrowsBtn, vsnTextEntryBrowsDlg::OnBrowsBtn)
  EVT_CLOSE(vsnTextEntryBrowsDlg::OnClose)
END_EVENT_TABLE()


/* constructors, destructor */

vsnTextEntryBrowsDlg::vsnTextEntryBrowsDlg(wxWindow* parent,
        const wxString& msg, const wxString& caption,
        const wxString& defaultValue)
  : wxDialog(parent, -1, caption),
  m_pValueTxt(NULL), m_pOkBtn(NULL), m_pCancelBtn(NULL), m_pBrowsBtn(NULL),
    m_modal(wxID_CANCEL)
{
  /* prepare parts */
  m_pValueTxt = new wxTextCtrl(this, -1, defaultValue);
  assert(m_pValueTxt);

  m_pOkBtn = new wxButton(this, TextEntryDlg_OkBtn, wxT("OK"));
  assert(m_pOkBtn);

  m_pBrowsBtn = new wxButton(this, TextEntryDlg_BrowsBtn, wxT("Brows"));
  assert(m_pBrowsBtn);

  m_pCancelBtn = new wxButton(this, TextEntryDlg_CancelBtn, wxT("Cancel"));
  assert(m_pCancelBtn);

  /* top Sizer */
  wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL);
  topsizer->SetMinSize(350, 100);

  wxBoxSizer* sizerH;

  /* layout */
  topsizer->Add(new wxStaticText(this, -1, msg), 0, wxEXPAND|wxALL, 10);

  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND|wxALL, 5);
  sizerH->Add(m_pValueTxt, 1, wxEXPAND|wxALL, 5);
  sizerH->Add(m_pBrowsBtn, 0, wxALIGN_RIGHT|wxALL, 5);

  topsizer->Add(new wxStaticLine(this, -1, wxDefaultPosition,
                                 wxSize(1,1), wxHORIZONTAL),
                0, wxEXPAND|wxALL, 0);

  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxALIGN_RIGHT|wxALL, 5);
  sizerH->Add(m_pCancelBtn, 0, wxALIGN_RIGHT|wxALL, 3);
  sizerH->Add(m_pOkBtn, 0, wxALIGN_RIGHT|wxALL, 3);

  /* post process */
  SetAutoLayout(TRUE);
  SetSizer(topsizer);
  topsizer->SetSizeHints(this);
  topsizer->Fit(this);
}

vsnTextEntryBrowsDlg::~vsnTextEntryBrowsDlg() {
}


/* methods */

wxString vsnTextEntryBrowsDlg::GetValue() const {
  return m_value;
}

void vsnTextEntryBrowsDlg::SetValue(const wxString& value) {
  m_value = value;
  if ( m_pValueTxt ) m_pValueTxt->SetValue(m_value);
}

int vsnTextEntryBrowsDlg::ShowModal() {
  wxDialog::ShowModal();
  return m_modal;
}


/* event handler */

void vsnTextEntryBrowsDlg::OnBrowsBtn(wxCommandEvent& event) {
  m_value = m_pValueTxt->GetValue();
  wxStringTokenizer tokenizer(m_value);
  wxString firstStr, followStr;
  if ( tokenizer.HasMoreTokens() )
    firstStr = tokenizer.GetNextToken();
  while ( tokenizer.HasMoreTokens() )
    followStr +=  wxString(wxT(" ")) + tokenizer.GetNextToken();

  wxFileDialog fileDlg(this, wxT("select a file"),
		       wxT(""), wxT(""), // default Dir / File
                       wxT("(*)|*"), wxFD_OPEN);
  if ( ! firstStr.IsEmpty() )
    fileDlg.SetPath(firstStr);
  if ( fileDlg.ShowModal() != wxID_OK )
    return;
  m_value = fileDlg.GetPath() + followStr;

  m_pValueTxt->SetValue(m_value);
  return;
}

void vsnTextEntryBrowsDlg::OnOkBtn(wxCommandEvent& event) {
  m_value = m_pValueTxt->GetValue();

  m_modal = wxID_OK;
  if ( IsModal() )
    EndModal(vsn_wxIDOK);
  else
    this->Hide();
}

void vsnTextEntryBrowsDlg::OnCancelBtn(wxCommandEvent& event) {
  if ( IsModal() )
    EndModal(vsn_wxIDCANCEL);
  else
    this->Hide();
}

void vsnTextEntryBrowsDlg::OnClose(wxCloseEvent& event) {
  if ( IsModal() )
    EndModal(vsn_wxIDCANCEL);
  else
    this->Hide();
}
