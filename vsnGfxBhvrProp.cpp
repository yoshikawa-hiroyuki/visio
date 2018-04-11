//
// vsnGfxBhvrProp
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

#include "vsnGfxBhvrProp.h"
#include "vsnPropDlg.h"
#include "vsnError.h"

using namespace std;
using namespace VFR;
using namespace VSN;


//----------------------------------------------------------------
// class vsnGfxBhvrProp
//----------------------------------------------------------------

/* constructors, destructor */

vsnGfxBhvrProp::vsnGfxBhvrProp(wxWindow* parent)
  : wxPanel(parent, wxID_ANY)
{
  assert(parent);

  wxBoxSizer *sizerTop = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *sizerH;

  // prepare widgets
  m_pXformAnimChk = new wxCheckBox(this, GfxBhvrProp_XformAnimChk,
				   wxT("animation in transformation"));
  assert(m_pXformAnimChk);
  m_pXformAnimDrtnTxt = new wxTextCtrl(this, GfxBhvrProp_XformAnimDrtnTxt,
				       wxT(""));
  assert(m_pXformAnimDrtnTxt);
  m_pXformSimpleRenderChk
    = new wxCheckBox(this, GfxBhvrProp_XformSimpleRenderChk,
		     wxT("simple rendering in transformation"));
  assert(m_pXformSimpleRenderChk);

  m_pZkeyZoonRatioTxt
    = new wxTextCtrl(this, GfxBhvrProp_ZkeyZoonRatioTxt, wxT(""));
  assert(m_pZkeyZoonRatioTxt);
  m_pShftZkeyZoonRatioTxt
    = new wxTextCtrl(this, GfxBhvrProp_ShftZkeyZoonRatioTxt, wxT(""));
  assert(m_pShftZkeyZoonRatioTxt);
  m_pXkeyZoonRatioTxt
    = new wxTextCtrl(this, GfxBhvrProp_XkeyZoonRatioTxt, wxT(""));
  assert(m_pXkeyZoonRatioTxt);
  m_pShftXkeyZoonRatioTxt
    = new wxTextCtrl(this, GfxBhvrProp_ShftXkeyZoonRatioTxt, wxT(""));
  assert(m_pShftXkeyZoonRatioTxt);

  // layout
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  sizerTop->Add(sizerH, 0, wxALIGN_RIGHT|wxALL, 5);
  sizerH->Add(m_pXformAnimChk, 0, wxALL, 3);
  sizerH->Add(5, 5);
  sizerH->Add(new wxStaticText(this, -1, wxT("duration")), 0, wxALL, 3);
  sizerH->Add(m_pXformAnimDrtnTxt, 0, wxALL, 3);
  sizerH->Add(new wxStaticText(this, -1, wxT("sec.")), 0, wxALL, 3);

  sizerH = new wxBoxSizer(wxHORIZONTAL);
  sizerTop->Add(sizerH, 0, wxALIGN_RIGHT|wxALL, 5);
  sizerH->Add(m_pXformSimpleRenderChk, 0, wxALL, 3);

  sizerH = new wxBoxSizer(wxHORIZONTAL);
  sizerTop->Add(sizerH, 0, wxALIGN_RIGHT|wxALL, 5);
  sizerH->Add(new wxStaticText(this, -1, wxT("z-key zoom-in ratio")),
	      0, wxALL, 3);
  sizerH->Add(m_pZkeyZoonRatioTxt, 0, wxALL, 3);
  sizerH->Add(5, 5);
  sizerH->Add(new wxStaticText(this, -1, wxT("shifted")), 0, wxALL, 3);
  sizerH->Add(m_pShftZkeyZoonRatioTxt, 0, wxALL, 3);

  sizerH = new wxBoxSizer(wxHORIZONTAL);
  sizerTop->Add(sizerH, 0, wxALIGN_RIGHT|wxALL, 5);
  sizerH->Add(new wxStaticText(this, -1, wxT("x-key zoom-out ratio")),
	      0, wxALL, 3);
  sizerH->Add(m_pXkeyZoonRatioTxt, 0, wxALL, 3);
  sizerH->Add(5, 5);
  sizerH->Add(new wxStaticText(this, -1, wxT("shifted")), 0, wxALL, 3);
  sizerH->Add(m_pShftXkeyZoonRatioTxt, 0, wxALL, 3);

  // post process
  SetAutoLayout(TRUE);
  SetSizer(sizerTop);
  sizerTop->SetSizeHints(this);
  sizerTop->Fit(this);

  (void)update();
}

vsnGfxBhvrProp::~vsnGfxBhvrProp() {
}


/* interface */

bool vsnGfxBhvrProp::update() {
  if ( ! m_pXformAnimChk || ! m_pXformAnimDrtnTxt || 
       ! m_pXformSimpleRenderChk ||
       ! m_pZkeyZoonRatioTxt || ! m_pShftZkeyZoonRatioTxt ||
       ! m_pXkeyZoonRatioTxt || ! m_pShftXkeyZoonRatioTxt )
    return false;

  char txt[64];
  float r1, r2;

  m_pXformAnimChk->SetValue(vsnGfxView::GetXformAnim());

  sprintf(txt, "%g", vsnGfxView::GetXformAnimDuration());
  m_pXformAnimDrtnTxt->SetValue(vsnApp::ConvSysToWx(txt));

  m_pXformSimpleRenderChk->SetValue(vsnScene::GetXformSimpleRender());

  vsnGfxView::GetKeyZoomInRatio(r1, r2);
  sprintf(txt, "%g", r1);
  m_pZkeyZoonRatioTxt->SetValue(vsnApp::ConvSysToWx(txt));
  sprintf(txt, "%g", r2);
  m_pShftZkeyZoonRatioTxt->SetValue(vsnApp::ConvSysToWx(txt));

  vsnGfxView::GetKeyZoomOutRatio(r1, r2);
  sprintf(txt, "%g", r1);
  m_pXkeyZoonRatioTxt->SetValue(vsnApp::ConvSysToWx(txt));
  sprintf(txt, "%g", r2);
  m_pShftXkeyZoonRatioTxt->SetValue(vsnApp::ConvSysToWx(txt));

  return true;
}

bool vsnGfxBhvrProp::apply() {
  if ( ! m_pXformAnimChk || ! m_pXformAnimDrtnTxt || 
       ! m_pXformSimpleRenderChk ||
       ! m_pZkeyZoonRatioTxt || ! m_pShftZkeyZoonRatioTxt ||
       ! m_pXkeyZoonRatioTxt || ! m_pShftXkeyZoonRatioTxt )
    return false;

  wxString wkStr;
  volatile float r1, r2;

  r1 = -1.f;
  wkStr = m_pXformAnimDrtnTxt->GetValue();
  if ( ! wkStr.IsEmpty() ) r1 = (float)atof(vsnApp::ConvWxToSys(wkStr).c_str());
  vsnGfxView::SetXformAnim(m_pXformAnimChk->GetValue(), r1);

  vsnScene::SetXformSimpleRender(m_pXformSimpleRenderChk->GetValue());

  r1 = r2 = -1.f;
  wkStr = m_pZkeyZoonRatioTxt->GetValue();
  if ( ! wkStr.IsEmpty() ) r1 = (float)atof(vsnApp::ConvWxToSys(wkStr).c_str());
  wkStr = m_pShftZkeyZoonRatioTxt->GetValue();
  if ( ! wkStr.IsEmpty() ) r2 = (float)atof(vsnApp::ConvWxToSys(wkStr).c_str());
  vsnGfxView::SetKeyZoomInRatio(r1, r2);

  r1 = r2 = -1.f;
  wkStr = m_pXkeyZoonRatioTxt->GetValue();
  if ( ! wkStr.IsEmpty() ) r1 = (float)atof(vsnApp::ConvWxToSys(wkStr).c_str());
  wkStr = m_pShftXkeyZoonRatioTxt->GetValue();
  if ( ! wkStr.IsEmpty() ) r2 = (float)atof(vsnApp::ConvWxToSys(wkStr).c_str());
  vsnGfxView::SetKeyZoomOutRatio(r1, r2);

  return true;
}

