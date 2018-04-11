//
// vsnAnchor
//
#ifndef _VSN_ANCHOR_H_
#define _VSN_ANCHOR_H_

#include "wx/defs.h"
#include "wx/dialog.h"
#include "wx/combobox.h"
#include "wx/radiobox.h"
#include "wx/checkbox.h"
#include "wx/textctrl.h"
#include "wx/button.h"
#include "wx/statline.h"
#include "wx/stattext.h"

#include "vsnIoObject.h"


//----------------------------------------------------------------
// class vsnAnchor
//----------------------------------------------------------------
class vsnAnchor {
public:
  enum ACT_Type {ACT_LoadXML =0, ACT_Movie, ACT_Image, ACT_Text};

  vsnAnchor();

  ACT_Type getActType() const {return m_actType;}
  void setActType(const ACT_Type act) {m_actType = act;}

  std::string getRefPath() const {return m_refPath;}
  void setRefPath(const std::string& path) {m_refPath = path;}

  bool getViewKeep() const {return m_viewKeep;}
  void setViewKeep(const bool vkm) {m_viewKeep = vkm;}

  float getFadeSec() const {return m_fadeSec;}
  void setFadeSec(const float fs) {m_fadeSec = fs;}

  std::string getAltImg() const {return m_altImg;}
  void setAltImg(const std::string& path) {m_altImg = path;}

  void execAction() const;

  bool outputAnchorXML(std::ostream& os, const size_t ts =0) const;
  bool parseAnchorXML(xmlNodePtr xnp);

protected:
  ACT_Type m_actType;
  std::string m_refPath;
  bool m_viewKeep;
  float m_fadeSec;
  std::string m_altImg;

  static std::string s_movShowPath;
  static std::string s_imgShowPath;
  static std::string s_txtShowPath;
};


//----------------------------------------------------------------
// class vsnEditAnchorDlg
//----------------------------------------------------------------

namespace VSN {
  enum {EditAnchorDlg_ActTypeCombo =2050,
	EditAnchorDlg_PathTxt,
	EditAnchorDlg_PathBrowsBtn,
	EditAnchorDlg_FadeTxt,
	EditAnchorDlg_KeepViewChk,
	EditAnchorDlg_AltImgChk,
	EditAnchorDlg_AltImgTxt,
	EditAnchorDlg_AltImgBrowsBtn,
	EditAnchorDlg_CancelBtn,
	EditAnchorDlg_OkBtn
  };
};

class vsnEditAnchorDlg : public wxDialog {
public:
  vsnEditAnchorDlg(wxWindow *parent, vsnAnchor *refAnchor);
  virtual ~vsnEditAnchorDlg();

  /* interface */
  bool update();

  vsnAnchor* getRefAnchor() {return p_anchor;}
  void setRefAnchor(vsnAnchor *refAnchor);

  /* event handler */
  void OnActTypeCombo(wxCommandEvent& event);
  void OnPathBrowsBtn(wxCommandEvent& event);
  void OnAltImgBrowsBtn(wxCommandEvent& event);
  void OnOkBtn(wxCommandEvent& event);
  void OnCancelBtn(wxCommandEvent& event);
  void OnClose(wxCloseEvent& event);

private:
  vsnAnchor*  p_anchor;

  wxComboBox* m_pActTypeCombo;
  wxTextCtrl* m_pPathTxt;
  wxButton*   m_pPathBrowsBtn;
  wxTextCtrl* m_pFadeTxt;
  wxCheckBox* m_pKeepViewChk;
  wxCheckBox* m_pAltImgChk;
  wxTextCtrl* m_pAltImgTxt;
  wxButton*   m_pAltImgBrowsBtn;
  wxButton*   m_pCancelBtn;
  wxButton*   m_pOkBtn;

  DECLARE_EVENT_TABLE()
};

#endif // _VSN_ANCHOR_H_
