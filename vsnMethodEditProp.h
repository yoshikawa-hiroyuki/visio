//
// vsnMethodEditProp
//
#ifndef _VSN_METHOD_EDIT_PROP_DLG_H_
#define _VSN_METHOD_EDIT_PROP_DLG_H_

#include "wx/defs.h"
#include "wx/dialog.h"
#include "wx/slider.h"
#include "wx/textctrl.h"
#include "wx/button.h"
#include "wx/statline.h"

#include "vsnMethodObj.h"
#include "vsnCMap.h"

namespace VSN {
  // control ids
  enum {MtdEditPropDlg_EditColorBtn = 1250,
	MtdEditPropDlg_ResetBtn,
	MtdEditPropDlg_CopyBtn,
	MtdEditPropDlg_OpacitySld,
	MtdEditPropDlg_OpacityTxt,
	MtdEditPropDlg_HilightSld,
	MtdEditPropDlg_HilightTxt,
	MtdEditPropDlg_CancelBtn,
	MtdEditPropDlg_CloseBtn
  };
};


//----------------------------------------------------------------
// class vsnMethodEditPropDlg
//----------------------------------------------------------------
class vsnMethodEditPropDlg : public wxDialog {
public:
  vsnMethodEditPropDlg(wxWindow *parent, vsnMethodObj* pmtd);
  ~vsnMethodEditPropDlg();

  // interface
  bool update(); // copy values from p_refMethod to this
  vsnMethodObj* getRefMethod() {return p_refMethod;}
  void setRefMethod(vsnMethodObj* pmtd);

  // event handler
  void OnEditColorBtn(wxCommandEvent& event);
  void OnResetBtn(wxCommandEvent& event);
  void OnCopyBtn(wxCommandEvent& event);
  void OnOpacitySld(wxScrollEvent& event);
  void OnOpacityTxt(wxCommandEvent& event);
  void OnHilightSld(wxScrollEvent& event);
  void OnHilightTxt(wxCommandEvent& event);
  void OnCancelBtn(wxCommandEvent& event);
  void OnCloseBtn(wxCommandEvent& event);
  void OnClose(wxCloseEvent& event);

private:
  wxButton*      m_pEditColorBtn;
  wxButton*      m_pResetBtn;
  wxButton*      m_pCopyBtn;
  wxSlider*      m_pOpacitySld;
  wxTextCtrl*    m_pOpacityTxt;
  wxSlider*      m_pHilightSld;
  wxTextCtrl*    m_pHilightTxt;
  wxButton*      m_pCancelBtn;
  wxButton*      m_pCloseBtn;

  vsnMethodObj*  p_refMethod;

  vector4        m_color_bak;
  float          m_hilight_bak;

  DECLARE_EVENT_TABLE()
};

#endif // _VSN_METHOD_EDIT_PROP_DLG_H_

