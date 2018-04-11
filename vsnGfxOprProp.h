//
// vsnGfxOprProp
//
#ifndef _VSN_GFX_OPR_DLG_H_
#define _VSN_GFX_OPR_DLG_H_

#include "wx/defs.h"
#include "wx/dialog.h"
#include "wx/checkbox.h"
#include "wx/radiobox.h"
#include "wx/statline.h"
#include "wx/button.h"

namespace VSN {
  // menu ids
  enum {GfxOprProp_Rotate_CtrlChk =1050,
	GfxOprProp_Rotate_ShiftChk,
	GfxOprProp_Rotate_MBtnRadio,
	GfxOprProp_Roll_CtrlChk,
	GfxOprProp_Roll_ShiftChk,
	GfxOprProp_Roll_MBtnRadio,
	GfxOprProp_Scale_CtrlChk,
	GfxOprProp_Scale_ShiftChk,
	GfxOprProp_Scale_MBtnRadio,
	GfxOprProp_Translate_CtrlChk,
	GfxOprProp_Translate_ShiftChk,
	GfxOprProp_Translate_MBtnRadio,
	GfxOprProp_Select_CtrlChk,
	GfxOprProp_Select_ShiftChk,
	GfxOprProp_Select_MBtnRadio,
	GfxOprProp_SweepZoom_CtrlChk,
	GfxOprProp_SweepZoom_ShiftChk,
	GfxOprProp_SweepZoom_MBtnRadio
  };
};

class vsnViewFrame;


//----------------------------------------------------------------
// class vsnGfxOprProp
//----------------------------------------------------------------
class vsnGfxOprProp : public wxPanel {
public:
  vsnGfxOprProp(wxWindow* parent);
  virtual ~vsnGfxOprProp();

  /* interface */
  bool update();
  bool apply();

private:
  wxCheckBox *m_pRotate_CtrlChk, *m_pRotate_ShiftChk;
  wxRadioBox *m_pRotate_MBtnRadio;
  wxCheckBox *m_pRoll_CtrlChk, *m_pRoll_ShiftChk;
  wxRadioBox *m_pRoll_MBtnRadio;
  wxCheckBox *m_pScale_CtrlChk, *m_pScale_ShiftChk;
  wxRadioBox *m_pScale_MBtnRadio;
  wxCheckBox *m_pTranslate_CtrlChk, *m_pTranslate_ShiftChk;
  wxRadioBox *m_pTranslate_MBtnRadio;
  wxCheckBox *m_pSelect_CtrlChk, *m_pSelect_ShiftChk;
  wxRadioBox *m_pSelect_MBtnRadio;
  wxCheckBox *m_pSweepZoom_CtrlChk, *m_pSweepZoom_ShiftChk;
  wxRadioBox *m_pSweepZoom_MBtnRadio;
};  

#endif // _VSN_GFX_OPR_DLG_H_


