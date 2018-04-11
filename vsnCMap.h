//
// vsnCMap
//
#ifndef _VSN_CMAP_H_
#define _VSN_CMAP_H_

#include "wx/defs.h"
#include "wx/dialog.h"
#include "wx/glcanvas.h"
#include "wx/radiobox.h"
#include "wx/stattext.h"

#include "vsnLut.h"

namespace VSN {
  enum RGBAcnlType {cnlRED =0, cnlGREEN, cnlBLUE, cnlALPHA};

  // control ids
  enum {CMapDlg_ChannelRadio =1400,
	CMapDlg_ImportBtn,
	CMapDlg_ExportBtn,
	CMapDlg_RampBtn,
	CMapDlg_InvertBtn,
	CMapDlg_CopyBtn,
	CMapDlg_ResetBtn,
	CMapDlg_DisplayChk,
	CMapDlg_LayoutBtn,
	CMapDlg_CancelBtn,
	CMapDlg_CloseBtn,

	AMapDlg_ImportBtn,
	AMapDlg_ExportBtn,
	AMapDlg_RampBtn,
	AMapDlg_InvertBtn,
	AMapDlg_ResetBtn,
	AMapDlg_CancelBtn,
	AMapDlg_CloseBtn,

	CMapConfirmDlg_OkBtn,
	CMapConfirmDlg_CancelBtn
  };
};

class vsnMethodObj;
class vsnCMapDlg;


//----------------------------------------------------------------
// class vsnExtLutRefer
//----------------------------------------------------------------
class vsnExtLutRefer {
public:
  // interface
  virtual void ext_setLut(const vsnLut& lut) =0;
  virtual vsnLut ext_getLut() const =0;
};


//----------------------------------------------------------------
// class vsnCMapBar
//----------------------------------------------------------------
class vsnCMapBar : public wxWindow {
public:
  enum {CMBshowRGB =1, CMBshowALP =2, CMBshowRGBA =3};

  vsnCMapBar(wxWindow* parent,
	     const wxPoint& pos =wxDefaultPosition,
	     const wxSize& size =wxDefaultSize);
  virtual ~vsnCMapBar();

  void setUseLut(const bool ulm);
  bool getUseLut() const {return m_useLut;}
  void setShowLutFlag(const long flg);
  long getShowLutFlg() const {return m_showLutFlg;}

  void setBaseColor(const vector4 cv);
  void getBaseColor(vector4 cv) const {
    memcpy(cv, m_colour, sizeof(vector4));
  }
  void setHilight(const float hl);
  float getHilight() const {return m_hilight;}

  void setLut(const vsnLut& lut);
  vsnLut getLut() const {return m_lut;}

  // event handler
  void OnPaint(wxPaintEvent& event);

private:
  bool    m_useLut;
  long    m_showLutFlg;
  vector4 m_colour;
  float   m_hilight;
  vsnLut  m_lut;

  DECLARE_EVENT_TABLE()
};


//----------------------------------------------------------------
// class vsnCMapCanvas
//----------------------------------------------------------------
class vsnCMapCanvas : public wxGLCanvas {
public:
  vsnCMapCanvas(wxWindow* parent, vsnExtLutRefer* pExtRef,
		const wxWindowID id =-1,
		const wxPoint& pos =wxDefaultPosition,
		const wxSize& size =wxSize(256,128), long style =0,
		const wxString& name =wxT("vsnCMapCanvas"));
  ~vsnCMapCanvas();

  // interface
  vsnLut getLut() const {return m_lut;}
  void setLut(const vsnLut& lut);
  VSN::RGBAcnlType getRGBAChannel() const {return m_rgbaCnl;}
  void setRGBAChannel(const VSN::RGBAcnlType cnl);

  // event handler
  void OnPaint(wxPaintEvent& event);
  void OnSize(wxSizeEvent& event);
  void OnEraseBackground(wxEraseEvent& event);
  void OnMouse(wxMouseEvent& event);

private:
  vsnLut            m_lut;
  VSN::RGBAcnlType  m_rgbaCnl;
  mutable long      m_lastX;

  vsnExtLutRefer*   p_extRefer;

  DECLARE_EVENT_TABLE()
};


//----------------------------------------------------------------
// class vsnCMapDlg
//----------------------------------------------------------------
class vsnCMapDlg
  : public wxDialog,
    public vsnExtLutRefer
{
public:
  vsnCMapDlg(wxWindow *parent, vsnMethodObj* pmtd);
  ~vsnCMapDlg();

  // interface
  void setLut(const vsnLut& lut, const bool updRef =false);
  vsnLut getLut() const {return m_lut;}

  vsnMethodObj* getRefMethod() {return p_refMethod;}
  void setRefMethod(vsnMethodObj* pMtd);

  void setDispCBar(const bool disp);
  bool getDispCBar() const {return m_dispCBar;}

  // from vsnExtLutRefer
  virtual void ext_setLut(const vsnLut& lut) {setLut(lut, true);}
  virtual vsnLut ext_getLut() const {return getLut();}

  // event handler
  void OnChannelRadio(wxCommandEvent& event);
  void OnImportBtn(wxCommandEvent& event);
  void OnExportBtn(wxCommandEvent& event);
  void OnRampBtn(wxCommandEvent& event);
  void OnInvertBtn(wxCommandEvent& event);
  void OnCopyBtn(wxCommandEvent& event);
  void OnResetBtn(wxCommandEvent& event);
  void OnDisplayChk(wxCommandEvent& event);
  void OnLayoutBtn(wxCommandEvent& event);
  void OnCancelBtn(wxCommandEvent& event);
  void OnCloseBtn(wxCommandEvent& event);
  void OnClose(wxCloseEvent& event);

  // static members
  static std::string s_lutImpDir;

private:
  vsnCMapCanvas* m_pGlCanvas;
  vsnCMapBar*    m_pCMapBar;
  wxRadioBox*    m_pChannel;
  wxButton*      m_pImport;
  wxButton*      m_pExport;
  wxButton*      m_pRamp;
  wxButton*      m_pInvert;
  wxButton*      m_pCopy;
  wxButton*      m_pReset;
  wxCheckBox*    m_pDisplay;
  wxButton*      m_pLayout;
  wxButton*      m_pCancel;
  wxButton*      m_pClose;

  vsnLut         m_lut;
  vsnLut         m_lut_back;
  vsnMethodObj*  p_refMethod;
  bool           m_dispCBar;

  void updateRefMethod(); // copy m_lut to p_refMethod

  DECLARE_EVENT_TABLE()
};


//----------------------------------------------------------------
// class vsnAMapDlg
//----------------------------------------------------------------
class vsnAMapDlg
  : public wxDialog,
    public vsnExtLutRefer
{
public:
  vsnAMapDlg(wxWindow *parent, vsnExtLutRefer* pref,
	     const VSN::RGBAcnlType cnl =VSN::cnlALPHA);
  ~vsnAMapDlg();

  // interface
  void setLut(const vsnLut& lut, const bool updRef =false);
  vsnLut getLut() const {return m_lut;}

  vsnExtLutRefer* getRefer() {return p_refer;}
  void setRefer(vsnExtLutRefer* pref);

  // from vsnExtLutRefer
  virtual void ext_setLut(const vsnLut& lut) {setLut(lut, true);}
  virtual vsnLut ext_getLut() const {return getLut();}

  // event handler
  void OnImportBtn(wxCommandEvent& event);
  void OnExportBtn(wxCommandEvent& event);
  void OnRampBtn(wxCommandEvent& event);
  void OnInvertBtn(wxCommandEvent& event);
  void OnResetBtn(wxCommandEvent& event);
  void OnCancelBtn(wxCommandEvent& event);
  void OnCloseBtn(wxCommandEvent& event);
  void OnClose(wxCloseEvent& event);

private:
  vsnCMapCanvas*   m_pGlCanvas;
  vsnCMapBar*      m_pCMapBar;
  wxButton*        m_pImport;
  wxButton*        m_pExport;
  wxButton*        m_pRamp;
  wxButton*        m_pInvert;
  wxButton*        m_pReset;
  wxButton*        m_pCancel;
  wxButton*        m_pClose;

  vsnLut           m_lut;
  vsnLut           m_lut_back;
  VSN::RGBAcnlType m_useChannel;

  vsnExtLutRefer*  p_refer;
  void updateRefer(); // copy m_lut to p_refer

  DECLARE_EVENT_TABLE()
};


//----------------------------------------------------------------
// class vsnCMapConfirmDlg
//----------------------------------------------------------------

class vsnCMapConfirmDlg : public wxDialog {
public:
  vsnCMapConfirmDlg(wxWindow *parent, const std::string& path=std::string(""));
  ~vsnCMapConfirmDlg();

  // interface
  bool setPath(const std::string& path);
  std::string getPath() const {return m_path;}
  bool getStatus() const {return m_status;}
  vsnLut getLut() const {return m_lut;}

  // event handler
  void OnOkBtn(wxCommandEvent& event);
  void OnCancelBtn(wxCommandEvent& event);

private:
  std::string m_path;
  bool m_status;
  vsnLut m_lut;

  wxStaticText* m_pDirTxt;
  wxStaticText* m_pFileTxt;
  vsnCMapBar*   m_pCMapBar;

  DECLARE_EVENT_TABLE()
};

#endif // _VSN_CMAP_H_
