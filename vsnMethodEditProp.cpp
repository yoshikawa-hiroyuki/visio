//
// vsnMethodEditProp
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
#include "wx/statline.h"
#include "wx/colordlg.h"

#include "vsnMethodEditProp.h"
#include "vsnError.h"

using namespace std;
using namespace CES;
using namespace VSN;


//----------------------------------------------------------------
// class vsnMethodEditPropDlg
//----------------------------------------------------------------

BEGIN_EVENT_TABLE(vsnMethodEditPropDlg, wxDialog)
  EVT_BUTTON(MtdEditPropDlg_EditColorBtn, vsnMethodEditPropDlg::OnEditColorBtn)
  EVT_BUTTON(MtdEditPropDlg_ResetBtn, vsnMethodEditPropDlg::OnResetBtn)
  EVT_BUTTON(MtdEditPropDlg_CopyBtn, vsnMethodEditPropDlg::OnCopyBtn)
  EVT_COMMAND_SCROLL(MtdEditPropDlg_OpacitySld,
		     vsnMethodEditPropDlg::OnOpacitySld)
  EVT_TEXT_ENTER(MtdEditPropDlg_OpacityTxt, vsnMethodEditPropDlg::OnOpacityTxt)
  EVT_COMMAND_SCROLL(MtdEditPropDlg_HilightSld,
		     vsnMethodEditPropDlg::OnHilightSld)
  EVT_TEXT_ENTER(MtdEditPropDlg_HilightTxt, vsnMethodEditPropDlg::OnHilightTxt)
  EVT_BUTTON(MtdEditPropDlg_CancelBtn, vsnMethodEditPropDlg::OnCancelBtn)
  EVT_BUTTON(MtdEditPropDlg_CloseBtn, vsnMethodEditPropDlg::OnCloseBtn)
  EVT_CLOSE(vsnMethodEditPropDlg::OnClose)
END_EVENT_TABLE()


/* constructors / destructor */

vsnMethodEditPropDlg::vsnMethodEditPropDlg(wxWindow *parent,
					   vsnMethodObj* pmtd)
  : wxDialog(parent, -1, wxString(wxT("MethodEditPropDlg"))),
    p_refMethod(pmtd)
{
  assert(parent);

  m_color_bak[0] = m_color_bak[1] = m_color_bak[2] = m_color_bak[3] = 1.f;
  m_hilight_bak = 0.f;

  /* prepare parts */
  m_pEditColorBtn = new wxButton(this, MtdEditPropDlg_EditColorBtn,
				 wxT("Edit Base Color"));
  assert(m_pEditColorBtn);

  m_pResetBtn = new wxButton(this, MtdEditPropDlg_ResetBtn, wxT("Reset"));
  assert(m_pResetBtn);

  m_pCopyBtn = new wxButton(this, MtdEditPropDlg_CopyBtn, wxT("Copy"));
  assert(m_pCopyBtn);

  m_pOpacitySld = new wxSlider(this, MtdEditPropDlg_OpacitySld,
			       100 /* val */, 0 /* min */, 100 /* max */,
			       wxDefaultPosition, wxSize(120, -1),
			       wxSL_HORIZONTAL|wxSL_LABELS);
  assert(m_pOpacitySld);

  m_pOpacityTxt = new wxTextCtrl(this, MtdEditPropDlg_OpacityTxt, wxT("100"),
				 wxDefaultPosition, wxDefaultSize,
				 wxTE_PROCESS_ENTER);
  assert(m_pOpacityTxt);

  m_pHilightSld = new wxSlider(this, MtdEditPropDlg_HilightSld,
			       0 /* val */, 0 /* min */, 100 /* max */,
			       wxDefaultPosition, wxSize(120, -1),
			       wxSL_HORIZONTAL|wxSL_LABELS);
  assert(m_pHilightSld);

  m_pHilightTxt =new wxTextCtrl(this, MtdEditPropDlg_HilightTxt, wxT("0"),
				 wxDefaultPosition, wxDefaultSize,
				 wxTE_PROCESS_ENTER);
  assert(m_pHilightTxt);

  m_pCancelBtn = new wxButton(this, MtdEditPropDlg_CancelBtn, wxT("Cancel"));
  assert(m_pCancelBtn);

  m_pCloseBtn = new wxButton(this, MtdEditPropDlg_CloseBtn, wxT("Close"));
  assert(m_pCloseBtn);

  /* top Sizer */
  wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL);

  /* upper buttons */
  wxBoxSizer* sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxALL, 0);
  sizerH->Add(m_pResetBtn, 0, wxALIGN_LEFT|wxALL, 3);
  sizerH->Add(m_pCopyBtn, 0, wxALIGN_LEFT|wxALL, 3);
  sizerH->Add(5, 5);
  sizerH->Add(m_pEditColorBtn, 0, wxALIGN_RIGHT|wxALL, 3);

  /* opacity */
  topsizer->Add(new wxStaticText(this, -1, wxT("Opacity")),
                0, wxALIGN_LEFT|wxALL, 3);
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND|wxALL, 0);
  sizerH->Add(m_pOpacitySld, 1, wxEXPAND|wxALL, 3);
  sizerH->Add(m_pOpacityTxt, 1, wxEXPAND|wxALL, 3);

  /* hilight */
  topsizer->Add(new wxStaticText(this, -1, wxT("Hilight")),
                0, wxALIGN_LEFT|wxALL, 3);
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND|wxALL, 0);
  sizerH->Add(m_pHilightSld, 1, wxEXPAND|wxALL, 3);
  sizerH->Add(m_pHilightTxt, 1, wxEXPAND|wxALL, 3);

  /* bottom buttons */
  topsizer->Add(
     new wxStaticLine(this, -1, wxDefaultPosition, wxSize(3,3), wxHORIZONTAL),
     0, wxEXPAND|wxALL, 0);
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxALIGN_RIGHT|wxALL, 0);
  sizerH->Add(m_pCancelBtn, 0, wxALL, 3);
  sizerH->Add(10, 0);
  sizerH->Add(m_pCloseBtn, 0, wxALIGN_RIGHT|wxALL, 3);

  /* post process */
  SetAutoLayout(TRUE);
  SetSizer(topsizer);
  topsizer->SetSizeHints(this);
  topsizer->Fit(this);

  if ( p_refMethod ) {
    p_refMethod->getBaseColor(m_color_bak);
    m_hilight_bak = p_refMethod->getHilight();
    (void)update();
  } // end of if(p_refMethod)
}

vsnMethodEditPropDlg::~vsnMethodEditPropDlg() {
}


/* interface */

bool vsnMethodEditPropDlg::update() {
  if ( ! p_refMethod ) return false;
  int val; char txt[64]; vector4 cv;

  // opacity
  p_refMethod->getBaseColor(cv);
  val = (int)(cv[3] * 100.5f);
  m_pOpacitySld->SetValue(val);
  sprintf(txt, "%d", val);
  m_pOpacityTxt->SetValue(vsnApp::ConvSysToWx(txt));

  // hilight
  cv[0] = p_refMethod->getHilight();
  val = (int)(cv[0] * 100.f);
  m_pHilightSld->SetValue(val);
  sprintf(txt, "%d", val);
  m_pHilightTxt->SetValue(vsnApp::ConvSysToWx(txt));

  return true;
}

void vsnMethodEditPropDlg::setRefMethod(vsnMethodObj* pmtd) {
  p_refMethod = pmtd;
  if ( p_refMethod ) {
    p_refMethod->getBaseColor(m_color_bak);
    m_hilight_bak = p_refMethod->getHilight();
    (void)update();
  }
}


/* event handler */

void vsnMethodEditPropDlg::OnEditColorBtn(wxCommandEvent& event) {
  if ( ! p_refMethod ) return;

  // current base-color
  vector4 cv; p_refMethod->getBaseColor(cv);
  wxColour ccol((unsigned char)(cv[0]*255.f),
                (unsigned char)(cv[1]*255.f),
                (unsigned char)(cv[2]*255.f));
  wxColourData cdata;
  cdata.SetColour(ccol);
  cdata.SetChooseFull(TRUE);

  vsnViewFrame* pvf = NULL;
  vsnUiView* puiv = dynamic_cast<vsnUiView*>(GetParent());
  if ( puiv && (pvf = dynamic_cast<vsnViewFrame*>(puiv->GetParent())) ) {
    deque<wxColor>& wcl = pvf->getWxColorList();
    for ( register int c = 0; c < wcl.size(); c++ )
      cdata.SetCustomColour(c%16, wcl[c]);
  } // end of if(puiv && pvf)

  // invoke color dialog
  wxColourDialog cdlg(this, &cdata);
  cdlg.SetTitle(wxT("Choose base color of the method"));
  if ( cdlg.ShowModal() != wxID_OK ) return;

  // update base-color
  cdata = cdlg.GetColourData();
  wxColor rcol = cdata.GetColour();
  if ( ccol == rcol ) return;
  if ( pvf ) {
    pvf->getWxColorList().push_back(rcol);
  }
  cv[0] = rcol.Red() / 255.f;
  cv[1] = rcol.Green() / 255.f;
  cv[2] = rcol.Blue() / 255.f;
  p_refMethod->setBaseColor(cv);
}

void vsnMethodEditPropDlg::OnResetBtn(wxCommandEvent& event) {
  if ( ! p_refMethod ) return;

  // restore backups
  p_refMethod->setBaseColor(m_color_bak);
  p_refMethod->setHilight(m_hilight_bak);

  // update
  (void)update();
}

void vsnMethodEditPropDlg::OnCopyBtn(wxCommandEvent& event) {
  if ( ! p_refMethod ) return;

  register size_t i, k;
  deque<vsnMethodObj*> mtdLst;
  size_t numScene = vsnApp::GetApp()->getNumScene();
  for ( i = 0; i < numScene; i++ ) {
    vsnScene* psc = vsnApp::GetApp()->getScene(i);
    if ( ! psc ) continue;
    size_t numData = psc->getNumDataObj();
    for ( k = 0; k < numData; k++ ) {
      vsnDataObj* pdt = psc->getDataObj(k);
      if ( ! pdt ) continue;
      vsnMethodObj* pmtd = pdt->getNextMethod(NULL);
      while ( pmtd ) {
	if ( pmtd != p_refMethod )
	  mtdLst.push_back(pmtd);
	pmtd = pdt->getNextMethod(pmtd);
      } // end of while(pmtd)
    } // end of for(k)
  } // end of for(i)

  if ( mtdLst.size() < 1 ) {
    ErrMsg(MsgINFO, "MethodEditPropDlg:"
	   " there is no other method to reference Properties");
    return;
  }

  wxString* choices = new wxString[mtdLst.size()];
  for ( i = 0; i < mtdLst.size(); i++ ) {
    wxString item = vsnApp::ConvSysToWx(mtdLst[i]->getMethodType());
    item += wxT("[");
    item += vsnApp::ConvSysToWx(mtdLst[i]->getName());
    item += wxT("] of data ");
    vsnDataObj* pdt = mtdLst[i]->getRefData();
    if ( pdt ) {
      item += vsnApp::ConvSysToWx(pdt->getDataType());
      item += wxT("[");
      item += vsnApp::ConvSysToWx(pdt->getName());
      item += wxT("]");
    }
    choices[i] = item;
  } // end of for(i)

  wxSingleChoiceDialog
    dlg(this, wxT("select the method of the copy origin"),
        wxT("reference copy of Properties"),
        mtdLst.size(), choices);
  delete [] choices;
  if ( dlg.ShowModal() != wxID_OK ) return;

  int val = dlg.GetSelection();
  if ( val < 0 || val >= mtdLst.size() ) {
    ErrMsg(MsgERR, "MethodEditPropDlg: invalid selection");
    return;
  }

  vector4 cv; float hl;
  mtdLst[val]->getBaseColor(cv);
  hl = mtdLst[val]->getHilight();
  p_refMethod->setBaseColor(cv);
  p_refMethod->setHilight(hl);

  (void)update();
}

void vsnMethodEditPropDlg::OnOpacitySld(wxScrollEvent& event) {
  if ( ! p_refMethod ) return;
  vector4 cv; p_refMethod->getBaseColor(cv);
  int val = m_pOpacitySld->GetValue();
  float alp = (float)val / 100.f;
  if ( cv[3] == alp ) return;

  cv[3] = alp;
  p_refMethod->setBaseColor(cv);
  (void)update();
}

void vsnMethodEditPropDlg::OnOpacityTxt(wxCommandEvent& event) {
  if ( ! p_refMethod ) return;
  vector4 cv; p_refMethod->getBaseColor(cv);
  wxString valStr = m_pOpacityTxt->GetValue();
  if ( valStr.IsEmpty() ) return;
  int val = atoi(vsnApp::ConvWxToSys(valStr).c_str());
  float alp = (float)val / 100.f;
  if ( cv[3] == alp ) return;

  cv[3] = alp;
  p_refMethod->setBaseColor(cv);
  (void)update();
}

void vsnMethodEditPropDlg::OnHilightSld(wxScrollEvent& event) {
  if ( ! p_refMethod ) return;
  int val = m_pHilightSld->GetValue();
  float hl = (float)val / 100.f;
  if ( hl == p_refMethod->getHilight() ) return;

  p_refMethod->setHilight(hl);
  (void)update();
}

void vsnMethodEditPropDlg::OnHilightTxt(wxCommandEvent& event) {
  if ( ! p_refMethod ) return;
  wxString valStr = m_pHilightTxt->GetValue();
  if ( valStr.IsEmpty() ) return;
  int val = atoi(vsnApp::ConvWxToSys(valStr).c_str());
  float hl = (float)val / 100.f;
  if ( hl == p_refMethod->getHilight() ) return;

  p_refMethod->setHilight(hl);
  (void)update();
}

void vsnMethodEditPropDlg::OnCancelBtn(wxCommandEvent& event) {
  if ( p_refMethod ) {
    p_refMethod->setBaseColor(m_color_bak);
    p_refMethod->setHilight(m_hilight_bak);
  }
  if ( IsModal() )
    EndModal(vsn_wxIDCANCEL);
  else
    this->Hide();
}

void vsnMethodEditPropDlg::OnCloseBtn(wxCommandEvent& event) {
  if ( IsModal() )
    EndModal(vsn_wxIDOK);
  else
    this->Hide();
}

void vsnMethodEditPropDlg::OnClose(wxCloseEvent& event) {
}
