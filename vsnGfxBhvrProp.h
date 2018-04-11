//
// vsnGfxBhvrProp
//
#ifndef _VSN_GFX_BHVR_PROP_H_
#define _VSN_GFX_BHVR_PROP_H_

#include "wx/defs.h"
#include "wx/panel.h"
#include "wx/checkbox.h"
#include "wx/textctrl.h"

namespace VSN {
  // control ids
  enum {GfxBhvrProp_XformAnimChk =1080,
	GfxBhvrProp_XformAnimDrtnTxt,
	GfxBhvrProp_XformSimpleRenderChk,
	GfxBhvrProp_ZkeyZoonRatioTxt,
	GfxBhvrProp_ShftZkeyZoonRatioTxt,
	GfxBhvrProp_XkeyZoonRatioTxt,
	GfxBhvrProp_ShftXkeyZoonRatioTxt

  };
};


//----------------------------------------------------------------
// class vsnGfxBhvrProp
//----------------------------------------------------------------
class vsnGfxBhvrProp : public wxPanel {
public:
  vsnGfxBhvrProp(wxWindow* parent);
  virtual ~vsnGfxBhvrProp();

  /* interface */
  bool update();
  bool apply();

  /* event handler */
  // none

private:
  wxCheckBox *m_pXformAnimChk;
  wxTextCtrl *m_pXformAnimDrtnTxt;
  wxCheckBox *m_pXformSimpleRenderChk;
  wxTextCtrl *m_pZkeyZoonRatioTxt, *m_pShftZkeyZoonRatioTxt;
  wxTextCtrl *m_pXkeyZoonRatioTxt, *m_pShftXkeyZoonRatioTxt;

  //DECLARE_EVENT_TABLE()
};  

#endif // _VSN_GFX_BFVR_PROP_H_
