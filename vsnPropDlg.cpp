//
// vsnPropDlg
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

#include "vsnPropDlg.h"
#include "vsnError.h"

using namespace std;
using namespace VFR;
using namespace VSN;


//----------------------------------------------------------------
// class vsnPropDlg
//----------------------------------------------------------------

BEGIN_EVENT_TABLE(vsnPropDlg, wxDialog)
  EVT_NOTEBOOK_PAGE_CHANGED(PropDlg_Notebook, vsnPropDlg::OnNotebook)
  EVT_BUTTON(PropDlg_OkBtn, vsnPropDlg::OnOkBtn)
  EVT_BUTTON(PropDlg_CancelBtn, vsnPropDlg::OnCancelBtn)
END_EVENT_TABLE()

/* constructor, destructor */

vsnPropDlg::vsnPropDlg(wxWindow *parent)
  : wxDialog(parent, -1, wxString(wxT("properties")),
	     wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE),
    m_notebook(NULL), m_pOkBtn(NULL), m_pCancelBtn(NULL),
    m_pGfxOprProp(NULL), m_pGfxBhvrProp(NULL)
{
  assert(parent);
  wxBoxSizer *sizerTop = new wxBoxSizer(wxVERTICAL);

  m_notebook = new wxNotebook(this, PropDlg_Notebook);
  assert(m_notebook);
  sizerTop->Add(m_notebook, 1, wxGROW);

  // buttons
  wxBoxSizer *sizerH = new wxBoxSizer(wxHORIZONTAL);
  sizerTop->Add(sizerH, 0, wxALIGN_RIGHT|wxALL, 5);
  m_pOkBtn = new wxButton(this, PropDlg_OkBtn, wxT("OK"));
  sizerH->Add(m_pOkBtn);
  sizerH->Add(5, 5);
  m_pCancelBtn = new wxButton(this, PropDlg_CancelBtn, wxT("Cancel"));
  sizerH->Add(m_pCancelBtn);

  // notebook pages
  m_pGfxBhvrProp = new vsnGfxBhvrProp(m_notebook);
  assert(m_pGfxBhvrProp);
  m_notebook->AddPage(m_pGfxBhvrProp, wxT("Gfx behavior"));

  m_pGfxOprProp = new vsnGfxOprProp(m_notebook);
  assert(m_pGfxOprProp);
  m_notebook->AddPage(m_pGfxOprProp, wxT("Gfx mouse operation"));

  // post process
  SetAutoLayout(TRUE);
  SetSizer(sizerTop);
  sizerTop->SetSizeHints(this);
  sizerTop->Layout();
  sizerTop->Fit(this);

  (void)update();
}

vsnPropDlg::~vsnPropDlg()
{
}


/* interface */

bool vsnPropDlg::update() {
  if ( ! m_pGfxOprProp || ! m_pGfxBhvrProp ) return false;
  if ( ! m_pGfxOprProp->update() ) return false;
  if ( ! m_pGfxBhvrProp->update() ) return false;
  return true;
}


/* event handler */

void vsnPropDlg::OnNotebook(wxNotebookEvent& event) {
}

void vsnPropDlg::OnOkBtn(wxCommandEvent& event) {
  if ( ! m_pGfxOprProp || ! m_pGfxBhvrProp ) return;
  m_pGfxOprProp->apply();
  m_pGfxBhvrProp->apply();
  if ( IsModal() )
    EndModal(vsn_wxIDOK);
  else
    this->Hide();
}

void vsnPropDlg::OnCancelBtn(wxCommandEvent& event) {
  if ( IsModal() )
    EndModal(vsn_wxIDCANCEL);
  else
    this->Hide();
}
