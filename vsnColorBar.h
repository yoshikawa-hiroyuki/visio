//
// vsnColorBar
//
#ifndef _VSN_COLOR_BAR_H_
#define _VSN_COLOR_BAR_H_

#include "wx/defs.h"
#include "wx/dialog.h"
#include "wx/radiobox.h"
#include "wx/textctrl.h"

#include "vsnMethodObj.h"
#include "vsnFrontObj.h"
#include "vsnLut.h"
#include "vfrMesh2D.h"
#include "vfrLineText.h"
#include <iostream>
#include <libxml/tree.h>

namespace VSN {
  // control ids
  enum {ColorBarDlg_DirRadio =1500,
	ColorBarDlg_AlphaChk,
	ColorBarDlg_TitleTxt,
	ColorBarDlg_NumLblsTxt,
	ColorBarDlg_NumDeciTxt,
	ColorBarDlg_PosXTxt,
	ColorBarDlg_PosYTxt,
	ColorBarDlg_SizeXTxt,
	ColorBarDlg_SizeYTxt,
	ColorBarDlg_CloseBtn
  };
};


//----------------------------------------------------------------
// class vsnColorBar
//----------------------------------------------------------------
class vsnColorBar : public vsnFrontObj, public vsnMethodLutRefer {
public:
  enum CBDirType {CBD_VERTICAL, CBD_HORIZONTAL};

  vsnColorBar(vsnMethodObj* pm,
	      const std::string& name =std::string(VFR_NONAME));
  virtual ~vsnColorBar();

  // from vsnMethdLutRefer
  virtual bool updateLut();

  // interface
  void setLut(const vsnLut& lut);
  vsnLut getLut() const {return m_lut;}

  void setShowAlpha(const bool sa);
  bool getShowAlpha() const {return m_showAlpha;}

  void setTitle(const std::string& tstr);
  std::string getTitle() const;

  bool setNumLabels(const size_t nl);
  size_t getNumLabels() const {return m_numLbls;}

  void setLabelPrecision(const size_t lp);
  size_t getLabelPrecision() const {return m_precLbls;}

  void setBaseColor(const vector4 cv);

  // geometric interface
  void setDirection(const CBDirType dir);
  CBDirType getDirection() const {return m_direction;}
  bool setSize(const vector2 s);
  void getSize(vector2 s) const {s[0] = m_sz[0]; s[1] = m_sz[1];}

  // invalidate scale methods
  virtual void scale(const vector3 v) {}
  virtual void scale(const float s) {}
  virtual void scale(const float x, const float y, const float z) {}

  // XML util
  bool importXMLNode(xmlNodePtr xnp);
  bool exportXMLNode(std::ostream& os, const bool show,
		     const size_t ts =0) const;

private:
  vsnLut       m_lut;
  vfrMesh2D*   m_pBar;
  vfrLineText* m_pTitleLbl;
  vfrGroup*    m_pLbls;
  size_t       m_numLbls;
  size_t       m_precLbls;
  CBDirType    m_direction;
  bool         m_showAlpha;
  vector2      m_sz;

  bool updateColorBar();
};


//----------------------------------------------------------------
// class vsnColorBarDlg
//----------------------------------------------------------------
class vsnColorBarDlg : public wxDialog {
public:
  vsnColorBarDlg(wxWindow *parent, vsnColorBar* pcb);
  ~vsnColorBarDlg();

  // interface
  bool update(); // copy values from p_colorBar to this
  void setColorBar(vsnColorBar* pcb);
  vsnColorBar* getColorBar() {return p_colorBar;}

  // callbacks
  void OnDirRadio(wxCommandEvent& event);
  void OnAlphaChk(wxCommandEvent& event);
  void OnTitleTxt(wxCommandEvent& event);
  void OnNumLblsTxt(wxCommandEvent& event);
  void OnNumDeciTxt(wxCommandEvent& event);
  void OnChangePos(wxCommandEvent& event);
  void OnChangeSize(wxCommandEvent& event);
  void OnCloseBtn(wxCommandEvent& event);
  void OnClose(wxCloseEvent& event);

private:
  wxRadioBox*  m_pDirection;
  wxCheckBox*  m_pAlphaChk;
  wxTextCtrl*  m_pTitle;
  wxTextCtrl*  m_pNumLbls;
  wxTextCtrl*  m_pNumDeci;
  wxTextCtrl*  m_pPosX;
  wxTextCtrl*  m_pPosY;
  wxTextCtrl*  m_pSizeX;
  wxTextCtrl*  m_pSizeY;
  wxButton*    m_pClose;
  vsnColorBar* p_colorBar;

  DECLARE_EVENT_TABLE()
};

#endif // _VSN_COLOR_BAR_H_
