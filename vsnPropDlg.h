#pragma once
//
// vsnPropDlg
//
#ifndef _VSN_PROP_DLG_H_
#define _VSN_PROP_DLG_H_

#include "wx/defs.h"
#include "wx/dialog.h"
#include "wx/notebook.h"
#include "wx/button.h"

#include "vsnGfxOprProp.h"
#include "vsnGfxBhvrProp.h"

namespace VSN {
  // menu ids
  enum {PropDlg_Notebook = 1040,
	PropDlg_OkBtn,
	PropDlg_CancelBtn
  };
};


//----------------------------------------------------------------
// class vsnPropDlg
//----------------------------------------------------------------
class vsnPropDlg : public wxDialog
{
public:
  vsnPropDlg(wxWindow *parent);
  virtual ~vsnPropDlg();

  /* interface */
  bool update();

  /* event handler */
  void OnNotebook(wxNotebookEvent& event);
  void OnOkBtn(wxCommandEvent& event);
  void OnCancelBtn(wxCommandEvent& event);

private:
  wxNotebook *m_notebook;
  wxButton   *m_pOkBtn, *m_pCancelBtn;

  vsnGfxOprProp*  m_pGfxOprProp;
  vsnGfxBhvrProp* m_pGfxBhvrProp;

  DECLARE_EVENT_TABLE()
};

#endif // _VSN_PROP_DLG_H_
