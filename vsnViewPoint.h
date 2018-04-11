//
// vsnViewPoint
//
#ifndef _VSN_VIEW_POINT_H_
#define _VSN_VIEW_POINT_H_

#include "wx/defs.h"
#include "wx/dialog.h"
#include "wx/bmpbuttn.h"
#include "wx/textctrl.h"
#include "wx/listbox.h"
#include "wx/button.h"
#include "wx/statline.h"

#include "utilMath.h"
#include <string>
#include <iostream>
#include <libxml/tree.h>

class vsnViewFrame;

namespace VSN {
  // control ids
  enum {ViewPointDlg_PxBtn =1600,
	ViewPointDlg_MxBtn,
	ViewPointDlg_PyBtn, ViewPointDlg_MyBtn,
	ViewPointDlg_PzBtn, ViewPointDlg_MzBtn,
	ViewPointDlg_ViewNameTxt,
	ViewPointDlg_ViewNameLst,
	ViewPointDlg_RegViewBtn,
	ViewPointDlg_ResViewBtn,
	ViewPointDlg_DelViewBtn,
	ViewPointDlg_CloseBtn
  };
};


//----------------------------------------------------------------
// struct vsnViewPoint
//----------------------------------------------------------------
struct vsnViewPoint {
  std::string      name;
  CES::Vec3<float> vT, vC, vS;
  CES::Mat4<float> mR;

  vsnViewPoint() {}
  vsnViewPoint(const vsnViewPoint& org) {*this = org;}

  void operator=(const vsnViewPoint& org) {
    name = org.name; vT = org.vT; vC = org.vC; vS = org.vS; mR = org.mR;
  }

  // quaternions util
  CES::Quat4<float> getRotQuat() const;

  // XML util
  bool importXMLNode(xmlNodePtr xnp);
  bool exportXMLNode(std::ostream& os, const size_t ts =0) const;
};


//----------------------------------------------------------------
// class vsnViewPointDlg
//----------------------------------------------------------------
class vsnViewPointDlg : public wxDialog {
public:
  vsnViewPointDlg(wxWindow *parent, vsnViewFrame* refViewFrame);
  ~vsnViewPointDlg();

  // interface
  void setRefViewFrame(vsnViewFrame* pvf);
  vsnViewFrame* getRefViewFrame() {return p_viewFrame;}

  // event handler
  void OnLeftViewBtn(wxCommandEvent& event);
  void OnRightViewBtn(wxCommandEvent& event);
  void OnFrontViewBtn(wxCommandEvent& event);
  void OnBackViewBtn(wxCommandEvent& event);
  void OnTopViewBtn(wxCommandEvent& event);
  void OnBottomViewBtn(wxCommandEvent& event);

  void OnViewNameTxt(wxCommandEvent& event);
  void OnRegViewBtn(wxCommandEvent& event);
  void OnViewNameLstClick(wxCommandEvent& event);
  void OnViewNameLstDblClick(wxCommandEvent& event);
  void OnResViewBtn(wxCommandEvent& event);
  void OnDelViewBtn(wxCommandEvent& event);
  void OnCloseBtn(wxCommandEvent& event);
  void OnClose(wxCloseEvent& event);

private:
  wxBitmapButton *m_pPxBtn, *m_pMxBtn;
  wxBitmapButton *m_pPyBtn, *m_pMyBtn;
  wxBitmapButton *m_pPzBtn, *m_pMzBtn;
  wxTextCtrl     *m_pViewNameTxt;
  wxButton       *m_pRegViewBtn;
  wxListBox      *m_pViewNameLst;
  wxButton       *m_pResViewBtn;
  wxButton       *m_pDelViewBtn;
  wxButton       *m_pCloseBtn;

  vsnViewFrame   *p_viewFrame;

  bool updateViewList(); // create ViewNameLst from p_viewFrame

  DECLARE_EVENT_TABLE()
};

#endif // _VSN_VIEW_POINT_H_
