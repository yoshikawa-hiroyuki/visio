#ifdef __GNUG__
#pragma implementation
#pragma interface
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <wx/listctrl.h>
#include <wx/regex.h>

#include <fstream>
#include <stdexcept>

#include "vsnApp.h"
#include "vsnParaXvxSelectDlg.h"
#include "vsnViewFrame.h"

BEGIN_EVENT_TABLE(vsnParaXvxSelectDlg, wxDialog)
  EVT_BUTTON(VSN::ParaXvxSelectDlg_OkBtn, vsnParaXvxSelectDlg::OnOK)
  EVT_BUTTON(VSN::ParaXvxSelectDlg_CancelBtn, vsnParaXvxSelectDlg::OnCancel)
  EVT_BUTTON(VSN::ParaXvxSelectDlg_SelectAllBtn,
	     vsnParaXvxSelectDlg::OnSelectAll)
  EVT_BUTTON(VSN::ParaXvxSelectDlg_DeselectAllBtn,
	     vsnParaXvxSelectDlg::OnDeselectAll)
  EVT_TEXT_ENTER(VSN::ParaXvxSelectDlg_FilterTxt,
		 vsnParaXvxSelectDlg::OnEnterFilter)
END_EVENT_TABLE()

vsnParaXvxSelectDlg::vsnParaXvxSelectDlg(vsnViewFrame* parent)
  : wxDialog(parent, -1, wxString(wxT("Select Parallel XVX Result Data")),
	     wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE),
  m_filterTxt(NULL), m_listCtrl(NULL)
{
  wxBoxSizer* pBaseSizer = new wxBoxSizer(wxVERTICAL);
  assert(pBaseSizer);

  createLayout(pBaseSizer);

  SetAutoLayout(TRUE);
  SetSizer(pBaseSizer);
  pBaseSizer->Layout();
  pBaseSizer->Fit(this);
}

void
vsnParaXvxSelectDlg::LoadIndexFile(const std::string& indexPath)
{
  m_allNames.clear();
  std::ifstream ifs(indexPath.c_str());
  if (ifs.fail()) throw std::runtime_error("Can't Open File");
  int n;
  ifs >> n;
  for (int i = 0; i < n; ++i) {
    std::string str;
    ifs >> str;
    m_listCtrl -> InsertItem(i, vsnApp::ConvSysToWx(str));
    m_allNames.push_back(str);
  }
  ifs.close();
}

void
vsnParaXvxSelectDlg::createLayout(wxBoxSizer* pBaseSizer)
{
  wxBoxSizer* pTopSizer = new wxBoxSizer(wxVERTICAL);
  assert(pTopSizer);
  pTopSizer->SetMinSize(400, 200);

  m_listCtrl = new wxListCtrl(this, -1, wxDefaultPosition, 
			      wxDefaultSize, wxLC_LIST);
  //  m_listCtrl = new wxListCtrl(this); 
  assert(m_listCtrl);
  pTopSizer -> Add(m_listCtrl, 1, wxEXPAND | wxALL, 5);

  {
    // Filter
    wxBoxSizer* btnSizer = new wxBoxSizer(wxHORIZONTAL);
    assert(btnSizer);

    btnSizer -> Add(new wxStaticText(this, -1, wxT("  Filter")));
    
    m_filterTxt = new wxTextCtrl(this, ParaXvxSelectDlg_FilterTxt, wxT(""),
				 wxDefaultPosition, wxDefaultSize,
				 wxTE_PROCESS_ENTER);
    assert(m_filterTxt);
    btnSizer -> Add(m_filterTxt, 1, wxEXPAND | wxALL, 3);

    wxButton* selectAllBtn = new wxButton(this, ParaXvxSelectDlg_SelectAllBtn,
					  wxT("Select All"));
    assert(selectAllBtn);
    btnSizer -> Add(selectAllBtn, 0, wxALL, 3);
    wxButton* deselectAllBtn = new wxButton(this,
					    ParaXvxSelectDlg_DeselectAllBtn,
					    wxT("Deselect All"));
    assert(deselectAllBtn);
    btnSizer -> Add(deselectAllBtn, 0, wxALL, 3);

    pTopSizer -> Add(btnSizer, 0, wxEXPAND | wxALL, 5);
  }

  {
    wxBoxSizer* btnSizer = new wxBoxSizer(wxHORIZONTAL);
    assert(btnSizer);

    wxButton* pOkBtn = new wxButton(this, ParaXvxSelectDlg_OkBtn, wxT("OK"));
    assert(pOkBtn);
    btnSizer -> Add(pOkBtn, 0, wxALL, 3);
    wxButton* pCancelBtn = new wxButton(this, ParaXvxSelectDlg_CancelBtn,
					wxT("Cancel"));
    assert(pCancelBtn);
    btnSizer -> Add(pCancelBtn, 0, wxALL, 3);

    pTopSizer -> Add(btnSizer, 0, wxALL | wxALIGN_CENTER, 5);
  }

  pBaseSizer -> Add(pTopSizer);
}

void
vsnParaXvxSelectDlg::OnOK(wxCommandEvent& event)
{
  m_selectedNames.clear();

  int itemIndex = -1;
  for (;;) {
    itemIndex = m_listCtrl -> GetNextItem(itemIndex,
					  wxLIST_NEXT_ALL,
					  wxLIST_STATE_SELECTED);
    if (itemIndex == -1) break;

    m_selectedNames.push_back(m_allNames[itemIndex]);
  }

  if ( IsModal() )
    EndModal(vsn_wxIDOK);
  else
    this->Hide();
}

void
vsnParaXvxSelectDlg::OnCancel(wxCommandEvent& event)
{
  if ( IsModal() )
    EndModal(vsn_wxIDCANCEL);
  else
    this->Hide();
}

void
vsnParaXvxSelectDlg::OnSelectAll(wxCommandEvent& event)
{
  if (m_listCtrl) {
    for (int i = 0; i < m_allNames.size(); ++i) {
      m_listCtrl -> SetItemState(i, wxLIST_STATE_SELECTED,
				 wxLIST_STATE_SELECTED);
    }
  }
}

void
vsnParaXvxSelectDlg::OnDeselectAll(wxCommandEvent& event)
{
  if (m_listCtrl) {
    for (int i = 0; i < m_allNames.size(); ++i) {
      //      m_listCtrl -> SetItemState(i, 0, wxLIST_STATE_SELECTED);
      m_listCtrl -> SetItemState(i, 0, wxLIST_STATE_SELECTED);
      m_listCtrl -> SetItemState(i, wxLIST_STATE_FOCUSED, wxLIST_STATE_FOCUSED);
    }
  }
}

void
vsnParaXvxSelectDlg::OnEnterFilter(wxCommandEvent& event)
{
  if (m_filterTxt && m_listCtrl) {
    wxString str = m_filterTxt -> GetValue();
    wxRegEx pat(str);
    
    for (int i = 0; i < m_allNames.size(); ++i) {
      if (pat.Matches(vsnApp::ConvSysToWx(m_allNames[i]))) {
	m_listCtrl -> SetItemState(i, wxLIST_STATE_SELECTED,
				   wxLIST_STATE_SELECTED);
      } else {
	m_listCtrl -> SetItemState(i, 0, wxLIST_STATE_SELECTED);
	m_listCtrl -> SetItemState(i, wxLIST_STATE_FOCUSED,
				   wxLIST_STATE_FOCUSED);
      }
    }

  }
}
