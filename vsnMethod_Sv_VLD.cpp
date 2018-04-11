//
// vsnMethod_Sv_VLD
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

#include "vsnMethod_Sv_VLD.h"
#include "vsnIsosurf.h"
#include "vsnOctTree.h" // for decomp vecIdx
#include "vsnUiView.h"
#include "vsnColorBar.h"
#include "vsnError.h"

#include "VLDVisibilityTester.h"
#include "VLDSuggestiveContourMaker.h"
#include "VLDIntersectPlaneMaker.h"

using namespace std;
using namespace CES;
using namespace VSN;


//----------------------------------------------------------------
// class vsnMPP_Sv_VLD
//----------------------------------------------------------------

BEGIN_EVENT_TABLE(vsnMPP_Sv_VLD, wxPanel)
  EVT_COMBOBOX(MPP_Sv_VLD_SelDataLst,
               vsnMPP_Sv_VLD::OnSelDataLst)
  EVT_CHECKLISTBOX(MPP_Sv_VLD_VecDataChkLst,
                   vsnMPP_Sv_VLD::OnVecDataChkLst)
  EVT_CHECKBOX(MPP_Sv_VLD_UseCMapChk,
               vsnMPP_Sv_VLD::OnUseCMapChk)
  EVT_CHECKBOX(MPP_Sv_VLD_UpdMinMaxChk,
               vsnMPP_Sv_VLD::OnUpdMinMaxChk)
  EVT_CHECKBOX(MPP_Sv_VLD_AntiAliasChk,
               vsnMPP_Sv_VLD::OnAntiAliasChk)
  EVT_TEXT_ENTER(MPP_Sv_VLD_LineWidthTxt,
                 vsnMPP_Sv_VLD::OnLineWidthTxt)

  EVT_CHECKBOX(MPP_Sv_VLD_EnableFastMethodChk,
               vsnMPP_Sv_VLD::OnEnableFastMethodChk)
  EVT_CHECKBOX(MPP_Sv_VLD_EnableVisibilityTestingChk,
               vsnMPP_Sv_VLD::OnEnableVisibilityTestingChk)

  EVT_COMBOBOX(MPP_Sv_VLD_SelIsoValueLst,
               vsnMPP_Sv_VLD::OnSelIsoValueLst)
  EVT_TEXT_ENTER(MPP_Sv_VLD_ValueTxt,
                 vsnMPP_Sv_VLD::OnValueTxt)
  EVT_CHECKBOX(MPP_Sv_VLD_ShowContourChk,
               vsnMPP_Sv_VLD::OnShowContourChk)
  EVT_CHECKBOX(MPP_Sv_VLD_ShowSuggestiveContourChk,
               vsnMPP_Sv_VLD::OnShowSuggestiveContourChk)

  EVT_CHECKBOX(MPP_Sv_VLD_EnableCuttingPlaneChk,
               vsnMPP_Sv_VLD::OnEnableCuttingPlaneChk)
  EVT_TEXT_ENTER(MPP_Sv_VLD_SamplerTxt,
                 vsnMPP_Sv_VLD::OnEnterSamplerTxt)
  EVT_BUTTON(MPP_Sv_VLD_SetSamplerBtn,
             vsnMPP_Sv_VLD::OnSetSamplerBtn)
  EVT_COMBOBOX(MPP_Sv_VLD_SelDataLst,
               vsnMPP_Sv_VLD::OnSelDataLst)

END_EVENT_TABLE()

/* constructors / destructor */
vsnMPP_Sv_VLD::vsnMPP_Sv_VLD(wxPanel* parent, vsnMethodObj* pm)
  : vsnMethodPP(parent, pm)
{
  assert(parent);
  assert(dynamic_cast<vsnMethod_Sv_VLD*>(pm));

  wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer* sizerH;

  // data selector
  topsizer->Add(new wxStaticText(this, -1, wxT("select scalar data")),
                0, wxALIGN_LEFT|wxALL, 3);
  m_pSelDataLst = new wxComboBox(this, MPP_Sv_VLD_SelDataLst,
                                 wxT(""), wxDefaultPosition, wxDefaultSize,
                                 0, NULL, wxCB_READONLY);
  topsizer->Add(m_pSelDataLst, 0, wxEXPAND|wxALL, 3);

  // vector data selector
  topsizer->Add(new wxStaticText(this, -1, wxT("vector components")),
                0, wxALIGN_LEFT|wxALL, 3);
  m_pVecDataChkLst = new wxCheckListBox(this,
                                        MPP_Sv_VLD_VecDataChkLst);
  topsizer->Add(m_pVecDataChkLst, 0, wxEXPAND|wxALL, 3);

  // use CMap
  m_pUseCMapChk = new wxCheckBox(this, MPP_Sv_VLD_UseCMapChk,
				 wxT("use cmap"));
  m_pUseCMapChk->SetValue(TRUE);
  topsizer->Add(m_pUseCMapChk, 0, wxALL, 3);

  // update minmax mode
  m_pUpdMinMaxChk = new wxCheckBox(this, MPP_Sv_VLD_UpdMinMaxChk,
                                   wxT("update minmax"));
  m_pUpdMinMaxChk->SetValue(TRUE);
  topsizer->Add(m_pUpdMinMaxChk, 0, wxALL, 3);

  // anti-alias
  m_pAntiAliasChk = new wxCheckBox(this, MPP_Sv_VLD_AntiAliasChk,
                                   wxT("anti-alias line"));
  topsizer->Add(m_pAntiAliasChk, 0, wxALL, 3);


  // lineWidth
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxALL, 0);
  sizerH->Add(new wxStaticText(this, -1, wxT("line width")),
              0, wxALIGN_LEFT|wxALL, 3);
  m_pLineWidthTxt = new wxTextCtrl(this, MPP_Sv_VLD_LineWidthTxt,
                                   wxT(""), wxDefaultPosition, wxDefaultSize,
                                   wxTE_PROCESS_ENTER);
  sizerH->Add(m_pLineWidthTxt, 0, wxEXPAND|wxALL, 3);

  // line 
  topsizer->Add(5, 5);
  topsizer->Add(new wxStaticLine(this, -1, wxDefaultPosition, wxSize(3,3),
                                 wxHORIZONTAL), 0, wxEXPAND|wxALL, 0);
  topsizer->Add(5, 5);

  // enable fast method
  m_pEnableFastMethodChk = new wxCheckBox(this, MPP_Sv_VLD_EnableFastMethodChk,
				 wxT("enable fast method"));
  m_pEnableFastMethodChk->SetValue(FALSE);
  topsizer->Add(m_pEnableFastMethodChk, 0, wxALL, 3);

  // enable visibility testing
  m_pEnableVisibilityTestingChk = new wxCheckBox(this, MPP_Sv_VLD_EnableVisibilityTestingChk,
				 wxT("enable visibility testing"));
  m_pEnableVisibilityTestingChk->SetValue(FALSE);
  topsizer->Add(m_pEnableVisibilityTestingChk, 0, wxALL, 3);

  // iso value selector
  topsizer->Add(new wxStaticText(this, -1, wxT("select iso value")),
                0, wxALIGN_LEFT|wxALL, 3);
  m_pSelIsoValueLst = new wxComboBox(this, MPP_Sv_VLD_SelIsoValueLst,
                                 wxT(""), wxDefaultPosition, wxDefaultSize,
                                 0, NULL, wxCB_READONLY);
  topsizer->Add(m_pSelIsoValueLst, 0, wxEXPAND|wxALL, 3);

  // iso value
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxALL, 0);
  sizerH->Add(new wxStaticText(this, -1, wxT("iso value")),
              0, wxALIGN_LEFT|wxALL, 3);
  m_pValueTxt = new wxTextCtrl(this, MPP_Sv_VLD_ValueTxt, wxT(""),
                               wxDefaultPosition, wxDefaultSize,
                               wxTE_PROCESS_ENTER);
  sizerH->Add(m_pValueTxt, 0, wxEXPAND|wxALL, 3);

  // show contour 
  m_pShowContourChk = new wxCheckBox(this, MPP_Sv_VLD_ShowContourChk,
				 wxT("show contour"));
  m_pShowContourChk->SetValue(FALSE);
  topsizer->Add(m_pShowContourChk, 0, wxALL, 3);

  // show suggestive contour
  m_pShowSuggestiveContourChk
    = new wxCheckBox(this, MPP_Sv_VLD_ShowSuggestiveContourChk,
		     wxT("show suggestive contour"));
  m_pShowSuggestiveContourChk->SetValue(FALSE);
  topsizer->Add(m_pShowSuggestiveContourChk, 0, wxALL, 3);

  // enable cutting plane
  m_pEnableCuttingPlaneChk = new wxCheckBox(this, MPP_Sv_VLD_EnableCuttingPlaneChk,
				 wxT("enable cutting plane"));
  m_pEnableCuttingPlaneChk->SetValue(FALSE);
  topsizer->Add(m_pEnableCuttingPlaneChk, 0, wxALL, 3);

  // create widgets
  m_pSamplerLst = new wxListBox(this, MPP_Sv_VLD_SamplerLst);
  assert(m_pSamplerLst);
  m_pSamplerTxt = new wxTextCtrl(this, MPP_Sv_VLD_SamplerTxt,
                                 wxT(""), wxDefaultPosition, wxDefaultSize,
                                 wxTE_READONLY|wxTE_PROCESS_ENTER);
  assert(m_pSamplerTxt);
  m_pSetSamplerBtn = new wxButton(this, MPP_Sv_VLD_SetSamplerBtn,
                                  wxT("set sampler"));

  // sampler selector
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND|wxALL, 3);
  sizerH->Add(new wxStaticText(this, -1, wxT("selected")),
              0, wxALIGN_LEFT|wxALL, 3);
  sizerH->Add(m_pSamplerTxt, 0, wxEXPAND|wxALL, 3);
  topsizer->Add(m_pSamplerLst, 0, wxEXPAND|wxALL, 3);
  topsizer->Add(m_pSetSamplerBtn, 0, wxALIGN_LEFT|wxALL, 3);

  // post process
  SetSizer(topsizer);
  addTo(parent);
  topsizer->Layout();
  (void)update();
}

vsnMPP_Sv_VLD::~vsnMPP_Sv_VLD() {
}


/* interface */

bool vsnMPP_Sv_VLD::update() {
  if ( ! m_pSelDataLst || ! m_pVecDataChkLst || ! m_pUseCMapChk ||
       ! m_pUpdMinMaxChk || ! m_pAntiAliasChk || ! m_pLineWidthTxt ||
       ! m_pEnableFastMethodChk || ! m_pEnableVisibilityTestingChk ||
       ! m_pSelIsoValueLst || ! m_pValueTxt || ! m_pShowContourChk ||
       ! m_pShowSuggestiveContourChk || ! m_pEnableCuttingPlaneChk ||
       ! m_pSamplerLst || ! m_pSamplerTxt || ! m_pSetSamplerBtn )
    return false;

  vsnMethod_Sv_VLD* pm
    = dynamic_cast<vsnMethod_Sv_VLD*>(p_method);
  if ( ! pm ) return false;
  vsnData_Sv* pdo = dynamic_cast<vsnData_Sv*>(pm->getRefData());
  if ( ! pdo ) return false;

  register int i;
  char txt[32];

  // select scalar data
  int dlen = (int)pdo->getDataLen();
  if ( m_pSelDataLst->GetCount() < 1 ) {
    m_pSelDataLst->Append(wxT("None"));
    if ( dlen > 0 ) {
      for ( i = 0; i < dlen; i++ ) {
	sprintf(txt, "data%d", i);
	m_pSelDataLst->Append(vsnApp::ConvSysToWx(txt));
      } // end of for(i)
      if ( dlen >= 3 )
	m_pSelDataLst->Append(wxT("vector length"));
    }
  }
  WhichDataType selData = pm->getSelectedData();
  if ( selData >= 0 && selData <= dlen )
    m_pSelDataLst->SetSelection(selData);
  else if ( selData == DATA_Veclen && dlen >= 3 )
    m_pSelDataLst->SetSelection(dlen + 1);
  else
    m_pSelDataLst->SetSelection(0);

  // vector data indices
  m_pVecDataChkLst->Clear();
  if ( dlen >= 3 ) {
    for ( i = 0; i < dlen; i++ ) {
      sprintf(txt, "data%d", i);
      m_pVecDataChkLst->Append(vsnApp::ConvSysToWx(txt));
    } // end of for(i)

    Vec3<int> vidx = pm->getVecDataIdx();
    if ( vidx[0] >= 0 && vidx[0] < dlen ) m_pVecDataChkLst->Check(vidx[0]);
    if ( vidx[1] >= 0 && vidx[1] < dlen ) m_pVecDataChkLst->Check(vidx[1]);
    if ( vidx[2] >= 0 && vidx[2] < dlen ) m_pVecDataChkLst->Check(vidx[2]);
  }

  // use CMap
  m_pUseCMapChk->SetValue(pm->getUseCMap());
  // update minmax mode
  m_pUpdMinMaxChk->SetValue(pm->getUpdateMinMaxMode());
  // update anti alias mode
  m_pAntiAliasChk->SetValue(pm->getAntiAliasMode());
  // update line width
  sprintf(txt, "%g", pm->getLineWidth());
  m_pLineWidthTxt->SetValue(vsnApp::ConvSysToWx(txt));

  // upate enable first method mode
  m_pEnableFastMethodChk->SetValue(pm->getEnableFastMethod());

  // update enable visibility testing mode
  m_pEnableVisibilityTestingChk->SetValue(pm->getEnableVisibilityTesting());

  const int selIsoVal = pm->getSelectedIsoValue();
  // select iso value
  if ( m_pSelIsoValueLst->GetCount() < 1 ) {
    if ( pm->getMaxIsoValues() > 0 ) {
      for ( int iv = 0; iv < pm->getMaxIsoValues(); iv++ ) {
        sprintf(txt, "iso value %d", iv);
        m_pSelIsoValueLst->Append(vsnApp::ConvSysToWx(txt));
      } // end of for(i)
    }
  }
  if ( selIsoVal >= 0 && selIsoVal < pm->getMaxIsoValues() )
    m_pSelIsoValueLst->SetSelection(selIsoVal);

  // iso value
  float val = pm->getIsoValue(selIsoVal);
  sprintf(txt, "%g", val); m_pValueTxt->SetValue(vsnApp::ConvSysToWx(txt));

  // update show contour mode
  m_pShowContourChk->SetValue(pm->getShowContour(selIsoVal));

  // update show suggestive contour mode
  m_pShowSuggestiveContourChk->
    SetValue(pm->getShowSuggestiveContour(selIsoVal));
  if ( ! pm->getEnableFastMethod() ) 
    m_pShowSuggestiveContourChk->Enable();
  else
    m_pShowSuggestiveContourChk->Disable();

  // upate enable cutting plane mode
  m_pEnableCuttingPlaneChk->SetValue(pm->getEnableCuttingPlane(selIsoVal));

  // selected sampler
  vsnMethod_sampler* psplr = pm->getSampler();
  if ( psplr )
    m_pSamplerTxt->SetValue(vsnApp::ConvSysToWx(psplr->getName()));
  else
    m_pSamplerTxt->SetValue(wxT("none"));
  
  // sampler list
  m_pSamplerLst->Clear();
  deque<class vsnMethod_sampler*> samplerMtdLst
    = pm->getSamplerList(pm->getRefData());
  deque<class vsnMethod_sampler*>::iterator it;
  for ( it = samplerMtdLst.begin(); it != samplerMtdLst.end(); it++ ) {
    if ( ! *it ) continue;
    string samplerName = (*it)->getName();
    if ( samplerName.empty() )
      m_pSamplerLst->Append(wxT(VFR_NONAME));
    else
      m_pSamplerLst->Append(vsnApp::ConvSysToWx(samplerName));
  } // end of for(it)

  return true;
}


/* event handler */

void vsnMPP_Sv_VLD::OnSelDataLst(wxCommandEvent& event) {
  if ( ! m_pSelDataLst ) return;
  WhichDataType sel = event.GetInt();
  if ( sel < 0 ) return;

  wxString valStr = m_pSelDataLst->GetString(sel);
  if ( valStr == wxT("vector length") )
    sel = DATA_Veclen;

  vsnMethod_Sv_VLD* pm
    = dynamic_cast<vsnMethod_Sv_VLD*>(p_method);
  if ( ! pm ) return;
  if ( sel == pm->getSelectedData() ) return;

  if ( ! pm->setSelectedData(sel) ) {
    update();
    return;
  }
  pm->chkNotice();
}

void vsnMPP_Sv_VLD::OnVecDataChkLst(wxCommandEvent& event) {
  if ( ! m_pVecDataChkLst ) return;
  int dlen = m_pVecDataChkLst->GetCount();
  if ( dlen < 3 ) return;

  vsnMethod_Sv_VLD* pm
    = dynamic_cast<vsnMethod_Sv_VLD*>(p_method);
  if ( ! pm ) return;

  register int i, c = 0;
  Vec3<int> vidx(-1, -1, -1);
  for ( i = 0; i < dlen; i++ ) {
    if ( m_pVecDataChkLst->IsChecked(i) ) {
      if ( c < 3 )
        vidx[c++] = i;
      else
        m_pVecDataChkLst->Check(i, FALSE);
    }
  } // end of for(i)

  if ( pm->setVecDataIdx(vidx) )
    pm->chkNotice();
}

void vsnMPP_Sv_VLD::OnUseCMapChk(wxCommandEvent& event) {
  if ( ! m_pUseCMapChk ) return;
  bool val = m_pUseCMapChk->GetValue();

  vsnMethod_Sv_VLD* pm
    = dynamic_cast<vsnMethod_Sv_VLD*>(p_method);
  if ( ! pm ) return;

  if ( pm->setUseCMap(val) )
    pm->chkNotice();
}

void vsnMPP_Sv_VLD::OnUpdMinMaxChk(wxCommandEvent& event) {
  if ( ! m_pUpdMinMaxChk ) return;
  bool val = m_pUpdMinMaxChk->GetValue();

  vsnMethod_Sv_VLD* pm
    = dynamic_cast<vsnMethod_Sv_VLD*>(p_method);
  if ( ! pm ) return;

  if ( pm->setUpdateMinMaxMode(val) )
    pm->chkNotice();
}

void vsnMPP_Sv_VLD::OnAntiAliasChk(wxCommandEvent& event) {
  if ( ! m_pAntiAliasChk ) return;
  bool val = m_pAntiAliasChk->GetValue();

  vsnMethod_Sv_VLD* pm = dynamic_cast<vsnMethod_Sv_VLD*>(p_method);
  if ( ! pm ) return;
  if ( val == pm->getAntiAliasMode() ) return;

  if ( pm->setAntiAliasMode(val) )
    pm->chkNotice();
}

void vsnMPP_Sv_VLD::OnLineWidthTxt(wxCommandEvent& event) {
  if ( ! m_pLineWidthTxt ) return;

  vsnMethod_Sv_VLD* pm = dynamic_cast<vsnMethod_Sv_VLD*>(p_method);
  if ( ! pm ) return;

  wxString valStr = m_pLineWidthTxt->GetValue();
  if ( valStr.IsEmpty() ) return;
  float fval = (float)atof(vsnApp::ConvWxToSys(valStr).c_str());
  if ( fval == pm->getLineWidth() ) return;

  if ( pm->setLineWidth(fval) )
    pm->chkNotice();
}

void vsnMPP_Sv_VLD::OnEnableFastMethodChk(wxCommandEvent& event) {
  if ( ! m_pEnableFastMethodChk ) return;
  bool val = m_pEnableFastMethodChk->GetValue();

  vsnMethod_Sv_VLD* pm
    = dynamic_cast<vsnMethod_Sv_VLD*>(p_method);
  if ( ! pm ) return;

  pm->setEnableFastMethod(val);
  pm->chkNotice();
}

void vsnMPP_Sv_VLD::OnEnableVisibilityTestingChk(wxCommandEvent& event) {
  if ( ! m_pEnableVisibilityTestingChk ) return;
  bool val = m_pEnableVisibilityTestingChk->GetValue();

  vsnMethod_Sv_VLD* pm
    = dynamic_cast<vsnMethod_Sv_VLD*>(p_method);
  if ( ! pm ) return;

  pm->setEnableVisibilityTesting(val);
  pm->chkNotice();
}

void vsnMPP_Sv_VLD::OnSelIsoValueLst(wxCommandEvent& event) {
  if ( ! m_pSelIsoValueLst ) return;
  const int sel = event.GetInt();
  if ( sel < 0 ) return;

  vsnMethod_Sv_VLD* pm
    = dynamic_cast<vsnMethod_Sv_VLD*>(p_method);
  if ( ! pm ) return;
  if ( sel == pm->getSelectedIsoValue() ) return;

  if ( ! pm->setSelectedIsoValue(sel) ) {
    update();
    return;
  }
  pm->chkNotice();
}

void vsnMPP_Sv_VLD::OnValueTxt(wxCommandEvent& event) {
  if ( ! m_pValueTxt ) return;

  vsnMethod_Sv_VLD* pm
    = dynamic_cast<vsnMethod_Sv_VLD*>(p_method);
  if ( ! pm ) return;

  wxString valStr = m_pValueTxt->GetValue();
  const int selIsoVal = pm->getSelectedIsoValue();
  if ( valStr.IsEmpty() ) {
    char txt[32]; sprintf(txt, "%g", pm->getIsoValue(selIsoVal));
    m_pValueTxt->SetValue(vsnApp::ConvSysToWx(txt));
    return;
  }
  float val = (float)atof(vsnApp::ConvWxToSys(valStr).c_str());

  if ( pm->setIsoValue(selIsoVal, val) )
    pm->chkNotice();
}

void vsnMPP_Sv_VLD::OnShowContourChk(wxCommandEvent& event) {
  if ( ! m_pShowContourChk ) return;
  bool val = m_pShowContourChk->GetValue();

  vsnMethod_Sv_VLD* pm
    = dynamic_cast<vsnMethod_Sv_VLD*>(p_method);
  if ( ! pm ) return;

  pm->setShowContour(pm->getSelectedIsoValue(), val);
  pm->chkNotice();
}

void vsnMPP_Sv_VLD::OnShowSuggestiveContourChk(wxCommandEvent& event) {
  if ( ! m_pShowSuggestiveContourChk ) return;
  bool val = m_pShowSuggestiveContourChk->GetValue();

  vsnMethod_Sv_VLD* pm
    = dynamic_cast<vsnMethod_Sv_VLD*>(p_method);
  if ( ! pm ) return;

  pm->setShowSuggestiveContour(pm->getSelectedIsoValue(), val);
  pm->chkNotice();
}

void vsnMPP_Sv_VLD::OnEnableCuttingPlaneChk(wxCommandEvent& event) {
  if ( ! m_pEnableCuttingPlaneChk ) return;
  bool val = m_pEnableCuttingPlaneChk->GetValue();

  vsnMethod_Sv_VLD* pm
    = dynamic_cast<vsnMethod_Sv_VLD*>(p_method);
  if ( ! pm ) return;

  pm->setEnableCuttingPlane(pm->getSelectedIsoValue(), val);
  pm->chkNotice();
}

void vsnMPP_Sv_VLD::OnSetSamplerBtn(wxCommandEvent& event) {
  if ( ! m_pSamplerLst || ! m_pSamplerTxt || ! m_pSetSamplerBtn )
    return;

  int sel = m_pSamplerLst->GetSelection();
  if ( sel < 0 ) return;
  m_pSamplerTxt->SetValue(m_pSamplerLst->GetString(sel));

  vsnMethod_Sv_VLD* pm
    = dynamic_cast<vsnMethod_Sv_VLD*>(p_method);
  if ( ! pm ) return;
  vsnDataObj* pdo = pm->getRefData();
  if ( ! pdo ) return;

  pm->setSampler(pdo, sel);
  vsnMethod_sampler* psplr = pm->getSampler();
  pm->chkNotice();
}

void vsnMPP_Sv_VLD::OnEnterSamplerTxt(wxCommandEvent& event) {
  if ( ! m_pSamplerTxt ) return;

  vsnMethod_Sv_VLD* pm
    = dynamic_cast<vsnMethod_Sv_VLD*>(p_method);
  if ( ! pm ) return;
  vsnMethod_sampler* psplr = pm->getSampler();
  if ( ! psplr ) return;
  vsnUiView* puiv = getUiView();
  if ( ! puiv ) return;

  puiv->selectObj((vfrNode*)psplr);
}

//----------------------------------------------------------------
// class vsnMethod_Sv_VLD
//----------------------------------------------------------------

/* constructors / destructor */

vsnMethod_Sv_VLD::
vsnMethod_Sv_VLD(const std::string& name)
  : vsnMethodObj(name), m_pd(NULL),
    m_selectedData(DATA_None), m_vecDataIdx(0,1,2),
    m_useCMap(true), m_updateMinMax(true), m_lineWidth(1.f),
    m_enableFastMethod(false), m_enableVisibilityTesting(false),
    m_selectedIsoValue(0), _needRemake(true), _updateVisibility(true) {

  // メンバ変数（配列）の初期化
  for ( int iv = 0; iv < MaxIsoValues; ++iv ) {
    m_isoValue[iv]              = 0.f;
    m_showContour[iv]           = false;
    m_showSuggestiveContour[iv] = false;
    m_enableCuttingPlane[iv]    = false;

    // レンダラー
    m_contours[iv]              = NULL;
    m_suggestiveContours[iv]    = NULL;
    m_intersectPlane[iv]        = NULL;

    _updateIsoValueContour[iv]           = true;
    _updateIsoValueSuggestiveContour[iv] = true;
    _updateIsoValueIntersectPlane[iv]    = true;
    _updateSampler[iv]                   = true;
    _viewPosContour[iv]                  = VLD::V3f(0.f, 0.f, 0.f);
    _viewPosSuggestiveContour[iv]        = VLD::V3f(0.f, 0.f, 0.f);
  }

}

vsnMethod_Sv_VLD::~vsnMethod_Sv_VLD() {

  for ( int iv = 0; iv < MaxIsoValues; ++iv ) {
    if ( m_contours[iv] )
      delete m_contours[iv];

    if ( m_suggestiveContours[iv] )
      delete m_suggestiveContours[iv];

    if ( m_intersectPlane[iv] )
      delete m_intersectPlane[iv];
  }

  if ( m_pd )
    DeAllocate(m_pd);
}


/* methods */
void vsnMethod_Sv_VLD::adjustRange(const bool updval) {
  bool needRangeUpd = false;

  vsnData_Sv* pData = dynamic_cast<vsnData_Sv*>(p_refData);
  if ( ! pData ) return;
  int dlen = (int)pData->getDataLen();

  float dr[2];
  for ( int iv = 0; iv < MaxIsoValues; ++iv ) {
    if ( m_selectedData == DATA_Veclen ) {
      if ( pData->getVectorMaxLen(m_vecDataIdx, dr[1]) ) {
        if ( updval )
	        m_isoValue[iv] = dr[1] * 0.5f;
        if ( m_updateMinMax ) {
	        m_lut.minVal = 0.f;
          m_lut.maxVal = dr[1];
          needRangeUpd = true;
        }
      }
      setUseLut(true);
    } // end of if(DATA_Veclen)
    else if ( m_selectedData > 0 && m_selectedData <= dlen ) {
      if ( pData->getMinMax(m_selectedData -1, dr) ) {
        if ( updval )
          m_isoValue[iv] = (dr[0] + dr[1])*0.5f;
        if ( m_updateMinMax ) {
          m_lut.minVal = dr[0];
          m_lut.maxVal = dr[1];
          needRangeUpd = true;
        }
      }
      setUseLut(true);
    } // end of if(SD_SCALAR)
    else
      setUseLut(false);
  }

  if ( needRangeUpd ) {
    set<vsnMethodLutRefer*>::iterator it;
    for ( it = m_mlrLst.begin(); it != m_mlrLst.end(); it++ )
      if ( *it ) (*it)->updateLut();
  }

}

void vsnMethod_Sv_VLD::updateColor() {

  vector4 cv;
  for ( int iv = 0; iv < MaxIsoValues; ++iv ) {
    if ( ! m_contours[iv] ) return;
    if ( ! m_suggestiveContours[iv] ) return;
    if ( ! m_intersectPlane[iv] ) return;

    int c = m_lut.getValIdx(m_isoValue[iv]);
    memcpy(cv, &m_lut.lutEntry[c*4], sizeof(float)*4);

    if ( m_useCMap ) {
      m_contours[iv]->setColor3(0, cv);
      m_suggestiveContours[iv]->setColor3(0, cv);
      m_intersectPlane[iv]->setColor3(0, cv);
    }
    else {
      m_contours[iv]->setColor3(0, m_colour);
      m_suggestiveContours[iv]->setColor3(0, m_colour);
      m_intersectPlane[iv]->setColor3(0, m_colour);
    }

 }
 chkNotice();
}

bool vsnMethod_Sv_VLD::setSelectedData(const WhichDataType sd) {
  if ( sd == m_selectedData ) return true;

  vsnData_Sv* pData = dynamic_cast<vsnData_Sv*>(p_refData);
  if ( ! pData ) return false;

  int dlen = (int)pData->getDataLen();
  if ( sd > dlen ) return false;
  if ( sd == DATA_Veclen && dlen < 3 ) return false;
  m_selectedData = sd;

  // data range
  adjustRange();

  for ( int iv = 0; iv < MaxIsoValues; ++iv ) {
    _updateIsoValueContour[iv]           = true;
    _updateIsoValueSuggestiveContour[iv] = true;
    _updateIsoValueIntersectPlane[iv]    = true;
  }
  _updateVisibility = true;
  

  if ( ! update() ) return false;
  updateUI();
  return true;
}

bool vsnMethod_Sv_VLD::setVecDataIdx(const CES::Vec3<int>& vdidx) {
  if ( vdidx[0] == m_vecDataIdx[0] &&
       vdidx[1] == m_vecDataIdx[1] &&
       vdidx[2] == m_vecDataIdx[2] ) return true;
  m_vecDataIdx = vdidx;

  // data range
  adjustRange();

  for ( int iv = 0; iv < MaxIsoValues; ++iv ) {
    _updateIsoValueContour[iv]           = true;
    _updateIsoValueSuggestiveContour[iv] = true;
    _updateIsoValueIntersectPlane[iv]    = true;
  }
  _updateVisibility = true;

  if ( ! update() ) return false;
  updateUI();
  return true;
}

bool vsnMethod_Sv_VLD::isValidVecData() const {
  vsnData_Sv* pData = dynamic_cast<vsnData_Sv*>(p_refData);
  if ( ! pData ) return false;

  size_t dlen = pData->getDataLen();
  if ( m_vecDataIdx[0] < 0 || m_vecDataIdx[0] >= dlen ) return false;
  if ( m_vecDataIdx[1] < 0 || m_vecDataIdx[1] >= dlen ) return false;
  if ( m_vecDataIdx[2] < 0 || m_vecDataIdx[2] >= dlen ) return false;
  return true;
}

bool vsnMethod_Sv_VLD::setUseCMap(const bool ucm) {
  if ( m_useCMap == ucm ) return true;
  m_useCMap = ucm;

  updateColor();
  updateUI();
  return true;
}

bool vsnMethod_Sv_VLD::setUpdateMinMaxMode(const bool mode) {
  if ( m_updateMinMax == mode ) return true;
  m_updateMinMax = mode;

  // data range
  adjustRange(false);

  updateColor();
  updateUI();
  return true;
}

bool vsnMethod_Sv_VLD::setLineWidth(const float lw) {
  if ( lw <= 0.f ) return false;
  if ( m_lineWidth == lw ) return true;
  m_lineWidth = lw;

  for ( register int c = 0; c < nChild; c++ ) {
    _children[c]->getPrivateMaterial()->setLineWidth(m_lineWidth);
  }

  updateUI();
  return true;
}

bool vsnMethod_Sv_VLD::setEnableFastMethod(const bool efm) {
  if ( m_enableFastMethod == efm ) return true;
  // fast methodがオフからオンになった場合は、全てのセルを検索し抽出を行わせる
  _needRemake        = ( ! efm && m_enableFastMethod ) ? true : false;
  m_enableFastMethod = efm;

  updateUI();
  return true;
}

bool vsnMethod_Sv_VLD::setSelectedIsoValue(const int siv) {
  if ( siv < 0 || siv >= MaxIsoValues ) return false;
  if ( siv == m_selectedIsoValue ) return true;
  m_selectedIsoValue = siv;
  // visibility(陰線消去)が選択されているiso valueに依存するため
  _updateVisibility  = true;

  updateUI();
  return true;
}

bool vsnMethod_Sv_VLD::setIsoValue(const int iv, const float val) {
  if ( iv < 0 || iv >= MaxIsoValues ) return false;

  if ( m_isoValue[iv] == val ) return true;
  m_isoValue[iv] = val;
  _updateIsoValueContour[iv]           = true;
  _updateIsoValueSuggestiveContour[iv] = true;
  _updateIsoValueIntersectPlane[iv]    = true;
  // iv 以外でshowがオンになっているものの visibility(陰線消去)を更新させるため
  _updateVisibility                    = true;

  updateColor();
  updateUI();
  return true;
}

bool vsnMethod_Sv_VLD::setShowContour(const int iv, const bool sc) {
  if ( iv < 0 || iv >= MaxIsoValues ) return false;
  if ( m_showContour[iv] == sc ) return true;
  m_showContour[iv] = sc;
  // (showがオフになっていた間の)cutting planeの変更を反映させるため
  _updateVisibility = true;

  updateUI();
  return true;
}

bool vsnMethod_Sv_VLD::setShowSuggestiveContour(const int iv, const bool ssc) {
  if ( iv < 0 || iv >= MaxIsoValues ) return false;
  if ( m_showSuggestiveContour[iv] == ssc ) return true;
  m_showSuggestiveContour[iv] = ssc;
  // (showがオフになっていた間の)cutting planeの変更を反映させるため
  _updateVisibility           = true;
  
  updateUI();
  return true;
}

bool vsnMethod_Sv_VLD::setEnableVisibilityTesting(const bool evt) {
  if ( m_enableVisibilityTesting == evt ) return true;
  m_enableVisibilityTesting = evt;
  // (showがオフになっていた間の)cutting planeの変更を反映させるため
  _updateVisibility         = true;

  updateUI();
  return true;
}

bool vsnMethod_Sv_VLD::setEnableCuttingPlane(const int iv, const bool ecp) {
  if ( iv < 0 || iv >= MaxIsoValues ) return false;
  if ( m_enableCuttingPlane[iv] == ecp ) return true;
  m_enableCuttingPlane[iv] = ecp;
  // (showがオフになっていた間の)cutting planeの変更を反映させるため
  _updateVisibility        = true;
  
  updateUI();
  return true;
}

/* vsnTimeSeriesMethodIF methods */

bool vsnMethod_Sv_VLD::updateStep(const int stp,
				      const bool force, const bool cascade)
{
  vsnData_Sv* pData = dynamic_cast<vsnData_Sv*>(p_refData);
  if ( ! pData ) return false;
  m_requestedStp = stp;
  if ( m_requestedStp < 0 )
    m_requestedStp = pData->getCurrentStepIdx();

  if ( force ) m_updatedStp = -1;
  if ( m_updatedStp == m_requestedStp ) return true;
  m_updatedStp = -1;

  // レンダラーを子（コンポーネント）として追加
  // for contour
  if ( MaxIsoValues < 1 ) return false;
  if ( ! m_contours[0] ) {
    for ( int iv = 0; iv < MaxIsoValues; ++iv ) {
      m_contours[iv] = new vfrLines();
      if ( ! m_contours[iv] ) {
        ErrMsg(MsgERR, getMethodType() + string("[") + getName()
              + string("]: memory allocation failed"));
        return false;
      }
      m_contours[iv]->alcMaterial();
      m_contours[iv]->setNormalMode(AT_PER_VERTEX);
      addChild(m_contours[iv]);
    }
  }

  // レンダラーを子（コンポーネント）として追加
  // for suggestive contour
  if ( ! m_suggestiveContours[0] ) {
    for ( int iv = 0; iv < MaxIsoValues; ++iv ) {
      m_suggestiveContours[iv] = new vfrLines();
      if ( ! m_suggestiveContours[iv] ) {
        ErrMsg(MsgERR, getMethodType() + string("[") + getName()
              + string("]: memory allocation failed"));
        return false;
      }
      m_suggestiveContours[iv]->alcMaterial();
      m_suggestiveContours[iv]->setNormalMode(AT_PER_VERTEX);
      addChild(m_suggestiveContours[iv]);
    }
  }

  // レンダラーを子（コンポーネント）として追加
  // for intersect plane
  if ( ! m_intersectPlane[0] ) {
    for ( int iv = 0; iv < MaxIsoValues; ++iv ) {
      m_intersectPlane[iv] = new vfrLines();
      if ( ! m_intersectPlane[iv] ) {
        ErrMsg(MsgERR, getMethodType() + string("[") + getName()
              + string("]: memory allocation failed"));
        return false;
      }
      m_intersectPlane[iv]->alcMaterial();
      m_intersectPlane[iv]->setNormalMode(AT_PER_VERTEX);
      addChild(m_intersectPlane[iv]);
    }
  }

  for ( register int c = 0; c < nChild; c++ ) {
    _children[c]->getPrivateMaterial()->setRenderMode(RT_NONE);
  }

  if ( ! m_show ) {
    return true;
  }

  // initialize prepar data
  DeAllocate(m_pd);
  m_pd = 0;

  // check selected data
  size_t dlen = pData->getDataLen();
  float dr[2] = {0.f, 1.f};
  if ( m_selectedData == DATA_None ) return true;
  else if ( m_selectedData == DATA_Veclen ) {
    if ( ! isValidVecData() ) return true;
    if ( ! pData->getVectorMaxLen(m_vecDataIdx, dr[1]) ) return false;
  }
  else if ( m_selectedData > 0 && m_selectedData <= dlen ) {
    if ( ! pData->getMinMax(m_selectedData -1, dr) ) return false;
  }
  else return true;

  // get Sv data
  const vector3* gptr = (const vector3*)pData->getCoord(m_requestedStp);
  
  if ( ! gptr ) {
    return false;
  }
  const float* dptr = pData->getData(m_requestedStp);
  if ( ! dptr ) {
    return false;
  }
  const unsigned char* maskptr = pData->getMask();

  // prepare data (set)
  Vec3<size_t> dims = pData->getDims();
  size_t dimSz = dims[0] * dims[1] * dims[2];
  if ( dimSz < 1 ) return false;
  m_pd = (float*)ReAllocate(m_pd, sizeof(float)*dimSz);
  
  float* sptr = m_pd;
  if ( ! sptr ) {
    ErrMsg(MsgERR, getMethodType() + string("[") + getName()
	   + string("]: memory allocation failed"));
    return false;
  }

  if ( ! _isEqualIntervals() ) {
    ErrMsg(MsgERR, getMethodType() + string("[") + getName()
      + string("]: ") + pData->getName() + string(" is not equal intervals"));
    return false;
  }

  register size_t i, idx;
  if ( m_selectedData == DATA_Veclen ) {
    Vec3<float> vv;
    for ( i = 0; i < dimSz; i++ ) {
      if ( maskptr && maskptr[i] ) {sptr[i] = 0.f; continue;}
      idx = i * dlen;
      vv.m_v[0] = dptr[idx + m_vecDataIdx.m_v[0]];
      vv.m_v[1] = dptr[idx + m_vecDataIdx.m_v[1]];
      vv.m_v[2] = dptr[idx + m_vecDataIdx.m_v[2]];
      sptr[i] = vv.Length();
    } // end of for(i)
  }
  else {
    for ( i = 0; i < dimSz; i++ ) {
      if ( maskptr && maskptr[i] ) {sptr[i] = dr[0]; continue;}
      idx = i * dlen + m_selectedData -1;
      sptr[i] = dptr[idx];
    } // end of for(i)
  }

  // set color
  updateColor();

  for ( register int c = 0; c < nChild; c++ ) {
    _children[c]->getPrivateMaterial()->setRenderMode(RT_WIRE);
    _children[c]->getPrivateMaterial()->setLineWidth(m_lineWidth);
  }

  setHilight(m_hilight);
  m_updatedStp = m_requestedStp;
  return true;
}

/* vsnMethodObj methods */

bool vsnMethod_Sv_VLD::update(const bool force) {
  return updateStep(m_requestedStp, force);
}

void vsnMethod_Sv_VLD::reloaded() {
  adjustRange(false);

  // m_updateMinMaxがfalseの場合でも再度抽出する
  for ( int iv = 0; iv < MaxIsoValues; ++iv ) {
    _updateIsoValueContour[iv]           = true;
    _updateIsoValueSuggestiveContour[iv] = true;
    _updateIsoValueIntersectPlane[iv]    = true;
  }
  _updateVisibility = true;

  vsnMethodObj::reloaded();
}

vsnMethodPP* vsnMethod_Sv_VLD::getParamPanel(wxPanel* pp) {
  vsnMPP_Sv_VLD* pp_VLD
    = new vsnMPP_Sv_VLD(pp, this);
  if ( ! pp_VLD ) return NULL;
  return pp_VLD;
}

void vsnMethod_Sv_VLD::setBaseColor(const vector4 cv) {
  memcpy(m_colour, cv, sizeof(vector4));
  updateUI();
  
  if ( m_pColorBar )
    m_pColorBar->setBaseColor(m_colour);

  updateColor();
}

/* from vsnRef_sampler */

void vsnMethod_Sv_VLD::noticeUpdate() {
  for ( int iv = 0; iv < MaxIsoValues; ++iv )
    _updateSampler[iv] = true;

  _updateVisibility  = true;
  chkNotice();
}

/* serialize : from vsnIoObject */

bool vsnMethod_Sv_VLD::parseXML(xmlNodePtr xnp) {
  static xmlChar* xs;
  xmlNodePtr cur;

  if ( ! vsnMethodObj::parseXML(xnp) ) return false;
  string msgHdr
    = getMethodType() +string("[") +getName() +string("]: parseXML: ");

  // get children node
  cur = xnp->xmlChildrenNode;

  // traverse
  while ( cur ) {
    if ( (cur = vsnIoObject::SkipCommentXML(cur)) == NULL ) break;

    if ( !strcmp((const char*)cur->name, "param") ) {
      string xsN, xsV;
      if ( xs ) xmlFree(xs);
      xs = xmlGetProp(cur, (const xmlChar*)"name");
      if ( ! xs || strlen((const char*)xs) < 1 ) goto _NEXT_XML_NODE;
      xsN = (const char*)xs;
      if ( xs ) xmlFree(xs);
      xs = xmlGetProp(cur, (const xmlChar*)"value");
      if ( xs && strlen((const char*)xs) > 0 ) xsV = (const char*)xs;
      else xsV = "";

      if ( xsV.empty() ) {
        ErrMsg(MsgERR, msgHdr +string("no value in param ") +xsN);
        goto _NEXT_XML_NODE;
      }

      if ( xsN == string("use_data") ) {
        WhichDataType sdt = -2;
        if ( xsV == string("none") ) sdt = DATA_None;
        else if ( xsV == string("veclen")  ) sdt = DATA_Veclen;
        else if ( xsV.substr(0, 4) == string("data") ) {
          string numStr = xsV.substr(4);
          if ( ! numStr.empty() ) sdt = atoi(numStr.c_str()) + 1;
        } else {
          ErrMsg(MsgERR, msgHdr + string("invalid value in param use_data"));
          goto _NEXT_XML_NODE;
        }
        if ( ! setSelectedData(sdt) ) {
          ErrMsg(MsgERR, msgHdr + string("failed to select ") +xsV);
          goto _NEXT_XML_NODE;
        }
      } // end of "use_data"
      else if ( xsN == string("vec_idx") ) {
        Vec3<int> idcs(-1, -1, -1);
        istringstream iss(xsV);
        iss >> idcs;
        if ( idcs[0] < 0 || idcs[1] < 0 ||  idcs[2] < 0 ) {
          ErrMsg(MsgERR, msgHdr + string("bad value format ") +xsV);
          goto _NEXT_XML_NODE;
        }
        if ( ! setVecDataIdx(idcs) ) {
          ErrMsg(MsgERR, msgHdr + string("failed to set ") +xsV);
          goto _NEXT_XML_NODE;
        }
      } // end of "vec_idx"
      else if ( xsN == string("use_cmap") ) {
        bool ucm;
        if ( xsV == string("yes") ) ucm = true;
        else if ( xsV == string("no") ) ucm = false;
        else {
          ErrMsg(MsgERR, msgHdr + string("invalid value in param use_cmap"));
          goto _NEXT_XML_NODE;
        }
        if ( ! setUseCMap(ucm) ) {
          ErrMsg(MsgERR, msgHdr + string("can't update use_cmap mode"));
          goto _NEXT_XML_NODE;
        }
      } // end of "use_cmap"
      else if ( xsN == string("upd_minmax") ) {
        bool mmupd;
        if ( xsV == string("yes") ) mmupd = true;
        else if ( xsV == string("no") ) mmupd = false;
        else {
          ErrMsg(MsgERR, msgHdr + string("invalid value in param upd_minmax"));
          goto _NEXT_XML_NODE;
        }
        if ( ! setUpdateMinMaxMode(mmupd) ) {
          ErrMsg(MsgERR, msgHdr + string("can't update upd_minmax mode"));
          goto _NEXT_XML_NODE;
        }
      } // end of "upd_minmax"
      else if ( xsN == string("antialias") ) {
        bool aam;
        if ( xsV == string("yes") ) aam = true;
        else if ( xsV == string("no") ) aam = false;
        else {
          ErrMsg(MsgERR, msgHdr + string("invalid value in param antialias"));
          goto _NEXT_XML_NODE;
        }
        if ( ! setAntiAliasMode(aam) ) {
          ErrMsg(MsgERR, msgHdr + string("can't set antialias"));
          goto _NEXT_XML_NODE;
        }
      } // end of "antialias"
      else if ( xsN == string("line_width") ) {
        float lw = (float)atof(xsV.c_str());
        if ( ! setLineWidth(lw) ) {
          ErrMsg(MsgERR, msgHdr + string("invalid value in param line_width"));
          goto _NEXT_XML_NODE;
        }
      } // end of "line_width"
      else if ( xsN == string("enable_fast_method") ) {
        bool efm;
        if ( xsV == string("yes") ) efm = true;
        else if ( xsV == string("no") ) efm = false;
        else {
          ErrMsg(MsgERR, msgHdr +
		 string("invalid value in param enable_fast_method"));
          goto _NEXT_XML_NODE;
        }
        if ( ! setEnableFastMethod(efm) ) {
          ErrMsg(MsgERR, msgHdr + string("can't update enable_fast_method"));
          goto _NEXT_XML_NODE;
        }
      } // end of "enable_fast_method"
      else if ( xsN == string("enable_visibility_testing") ) {
        bool evt;
        if ( xsV == string("yes") ) evt = true;
        else if ( xsV == string("no") ) evt = false;
        else {
          ErrMsg(MsgERR, msgHdr +
		 string("invalid value in param enable_visibility_testing"));
          goto _NEXT_XML_NODE;
        }
        if ( ! setEnableVisibilityTesting(evt) ) {
          ErrMsg(MsgERR, msgHdr +
		 string("can't update enable_visibility_testing mode"));
          goto _NEXT_XML_NODE;
        }
      } // end of "enable_visibility_testing"
      else if ( xsN == string("selected_iso_value") ) {
        int siv = (int)atoi(xsV.c_str());
        if ( ! setSelectedIsoValue(siv) ) {
          ErrMsg(MsgERR, msgHdr +
		 string("invalid value in param selected_iso_value"));
          goto _NEXT_XML_NODE;
        }
      } // end of "selected_iso_value"
      else if ( xsN == string("iso_value 0") ||
                xsN == string("iso_value 1") ||
                xsN == string("iso_value 2")    ) {
        int iv;
        char name[32];
        const int nscan = sscanf(xsN.c_str(), "%s %d", name, &iv);
        if ( nscan < 2 ) {
          ErrMsg(MsgERR, msgHdr + string("invalid value in param iso_value"));
          goto _NEXT_XML_NODE;
        }
        float val = (float)atof(xsV.c_str());
        if ( ! setIsoValue(iv, val) ) {
          ErrMsg(MsgERR, msgHdr + string("invalid value in param iso_value"));
          goto _NEXT_XML_NODE;
        }
      } // end of "iso_value"
      else if ( xsN == string("show_contour 0") ||
                xsN == string("show_contour 1") ||
                xsN == string("show_contour 2")    ) {
        int iv;
        char name[32];
        const int nscan = sscanf(xsN.c_str(), "%s %d", name, &iv);
        if ( nscan < 2 ) {
          ErrMsg(MsgERR, msgHdr +
		 string("invalid value in param show_contour"));
          goto _NEXT_XML_NODE;        
        }
        bool sc;
        if ( xsV == string("yes") ) sc = true;
        else if ( xsV == string("no") ) sc = false;
        else {
          ErrMsg(MsgERR, msgHdr +
		 string("invalid value in param show_contour"));
          goto _NEXT_XML_NODE;
        }
        if ( ! setShowContour(iv, sc) ) {
          ErrMsg(MsgERR, msgHdr + string("can't update show_contour mode"));
          goto _NEXT_XML_NODE;
        }
      } // end of "show_contour"
      else if ( xsN == string("show_suggestive_contour 0") ||
                xsN == string("show_suggestive_contour 1") ||
                xsN == string("show_suggestive_contour 2")    ) {
        int iv;
        char name[32];
        const int nscan = sscanf(xsN.c_str(), "%s %d", name, &iv);
        if ( nscan < 2 ) {
          ErrMsg(MsgERR, msgHdr +
		 string("invalid value in param show_suggestive_contour"));
          goto _NEXT_XML_NODE;        
        }
        bool ssc;
        if ( xsV == string("yes") ) ssc = true;
        else if ( xsV == string("no") ) ssc = false;
        else {
          ErrMsg(MsgERR, msgHdr +
		 string("invalid value in param show_suggestive_contour"));
          goto _NEXT_XML_NODE;
        }
        if ( ! setShowSuggestiveContour(iv, ssc) ) {
          ErrMsg(MsgERR, msgHdr +
		 string("can't update show_suggestive_contour mode"));
          goto _NEXT_XML_NODE;
        }
      } // end of "show_suggestive_contour"
      else if ( xsN == string("enable_cutting_plane 0") ||
                xsN == string("enable_cutting_plane 1") ||
                xsN == string("enable_cutting_plane 2")    ) {
        int iv;
        char name[32];
        const int nscan = sscanf(xsN.c_str(), "%s %d", name, &iv);
        if ( nscan < 2 ) {
          ErrMsg(MsgERR, msgHdr +
		 string("invalid value in param enable_cutting_plane"));
          goto _NEXT_XML_NODE;        
        }
        bool ecp;
        if ( xsV == string("yes") ) ecp = true;
        else if ( xsV == string("no") ) ecp = false;
        else {
          ErrMsg(MsgERR, msgHdr +
		 string("invalid value in param enable_cutting_plane"));
          goto _NEXT_XML_NODE;
        }
        if ( ! setEnableCuttingPlane(iv, ecp) ) {
          ErrMsg(MsgERR, msgHdr +
		 string("can't update enable_cutting_plane mode"));
          goto _NEXT_XML_NODE;
        }
      } // end of "enable_cutting_plane"
      if ( xsN == string("sampler") ) {
        if ( ! setSampler(p_refData, xsV) ) {
          ErrMsg(MsgERR, msgHdr + string("failed to set sampler: ") + xsV);
          goto _NEXT_XML_NODE;
        }
      } // end of "sampler"
    } // end of param

  _NEXT_XML_NODE:
    cur = cur->next;
  } // end of while(cur)

  return true;
}

bool vsnMethod_Sv_VLD::outputXML(std::ostream& os, const size_t ts) {
  std::string idts;
  for ( int i = 0; i < ts; i++ ) idts.push_back(' ');
  string idts_2 = idts; idts_2.push_back(' '); idts_2.push_back(' ');
  bool ret= true;
  string msgHdr
    = getMethodType() +string("[") +getName() +string("]: outputXML: ");

  // output
  os << idts << "<method type=\"" << getMethodType() << "\"";
  if ( !_name.empty() && _name != string(VFR_NONAME) )
    os << " name=\"" << _name << "\"";
  os << " >" << endl;

  // output base method params
  if ( ! vsnMethodObj::exportXMLNode(os, ts+2) ) {
    ErrMsg(MsgERR, msgHdr + string("base MethodObj exportXMLNode failed"));
    ret = false;
  }

  // output original params
  // use_data
  if ( m_selectedData != DATA_None ) {
    os << idts_2 << "<param name=\"use_data\" value=\"";
    if ( m_selectedData == DATA_Veclen ) 
      os << "veclen";
    else if ( m_selectedData > 0 )
      os << "data" << m_selectedData -1;
    else {
      os << "none";
      ErrMsg(MsgWARN, msgHdr
             + string("invalid use_data has set, so don't output\n"));
    }
    os << "\" />" << endl;
  }

  // vec_idx
  if ( m_vecDataIdx[0] != 0 || m_vecDataIdx[1] != 1 || m_vecDataIdx[2] != 2 ) {
    os << idts_2 << "<param name=\"vec_idx\" value=\"";
    os << m_vecDataIdx[0] << "/" << m_vecDataIdx[1] << "/" << m_vecDataIdx[2];
    os << "\" />" << endl;
  }

  // use_cmap / use_cmap_alpha
  if ( ! m_useCMap ) {
    os << idts_2 << "<param name=\"use_cmap\" value=\"no\" />" << endl;
  }

  // upd_minmax
  if ( ! m_updateMinMax ) {
    os << idts_2 << "<param name=\"upd_minmax\" value=\"no\" />" << endl;
  }

  // antialias
  if ( m_antiAlias ) {
    os << idts_2 << "<param name=\"antialias\" value=\"yes\" />" << endl;
  }

  // line_width
  if ( m_lineWidth != 1.f ) {
    os << idts_2 << "<param name=\"line_width\" value=\""
       << m_lineWidth << "\" />" << endl;
  }

  // enable_fast_method
  if ( m_enableFastMethod ) {
    os << idts_2 << "<param name=\"enable_fast_method\" value=\"yes\" />"
       << endl;
  }

  // enable_visibility_testing
  if ( m_enableVisibilityTesting ) {
    os << idts_2 << "<param name=\"enable_visibility_testing\" value=\"yes\" />"
       << endl;
  }

  //  selected_iso_value
  if ( m_selectedData != DATA_None ) {
    os << idts_2 << "<param name=\"selected_iso_value\" value=\""; 
    if ( m_selectedIsoValue >= 0 && m_selectedIsoValue < MaxIsoValues )
      os << m_selectedIsoValue;
    else {
      os << "none";
      ErrMsg(MsgWARN, msgHdr
             + string("invalid selected_iso_value has set, so don't output\n"));
    }
    os << "\" />" << endl;
  }

  char txt[64];
  // iso_value
  for ( int iv = 0; iv < MaxIsoValues; ++iv ) {
    sprintf(txt, "iso_value %d", iv);
    os << idts_2 << "<param name=\"" << txt << "\" value=\""
      << m_isoValue[iv] << "\" />" << endl;
  }

  // show_contour
  for ( int iv = 0; iv < MaxIsoValues; ++iv ) {
    sprintf(txt, "show_contour %d", iv);
    if ( m_showContour[iv] ) 
      os << idts_2 << "<param name=\""<< txt << "\" value=\"yes\" />" << endl;
  }

  // show_suggestive_contour
  for ( int iv = 0; iv < MaxIsoValues; ++iv ) {
    sprintf(txt, "show_suggestive_contour %d", iv);
    if ( m_showSuggestiveContour[iv] ) 
      os << idts_2 << "<param name=\""<< txt << "\" value=\"yes\" />" << endl;
  }

  // enable_cutting_plane
  for ( int iv = 0; iv < MaxIsoValues; ++iv ) {
    sprintf(txt, "enable_cutting_plane %d", iv);
    if ( m_enableCuttingPlane[iv] ) 
      os << idts_2 << "<param name=\""<< txt << "\" value=\"yes\" />" << endl;
  }

  // sampler
  if ( p_splr ) {
    if ( ! vsnRef_sampler::exportXMLNode(os, ts+2) ) {
      ErrMsg(MsgWARN, msgHdr
             + string("sampler has set, but the sampler has no name,\n")
             + string("so don't output sampler param node"));
    }
  }

  os << idts << "</method>" << endl;
  return ret;
}

bool vsnMethod_Sv_VLD::commandXML(xmlNodePtr xnp) {
  static xmlChar* xs;
  string msgHdr
    = getMethodType() + string("[") + getName() + string("]: commandXML: ");

  // is 'command' node?
  if ( ! xnp ) return false;
  if ( strcmp((const char*)xnp->name, "command") ) return false;

  // is my command?
  if ( xs ) xmlFree(xs);
  xs = xmlGetProp(xnp, (const xmlChar*)"target");
  if ( ! xs || strlen((const char*)xs) < 1 ) return false;
  if ( string((const char*)xs) != getName() ) return false;

  // get command name
  string nameStr, valueStr;
  if ( xs ) xmlFree(xs);
  xs = xmlGetProp(xnp, (const xmlChar*)"name");
  if ( ! xs || strlen((const char*)xs) < 1 ) {
    ErrMsg(MsgERR, msgHdr + "invalid command: no 'name' property");
    return false;
  }
  nameStr = string((const char*)xs);

  // traverse base MethdObj command
  if ( vsnMethodObj::commandXML(xnp) )
    return true; // this is a base MethdObj command, ok

  // get command value (if there)
  if ( xs ) xmlFree(xs);
  xs = xmlGetProp(xnp, (const xmlChar*)"value");
  if ( xs && strlen((const char*)xs) > 0 )
    valueStr = string((const char*)xs);

  // do the command
  if ( valueStr.empty() ) {
    ErrMsg(MsgERR, msgHdr + "invalid command: " + nameStr + ": no value");
    return false;
  }

  if ( nameStr == "set_use_data" ) {
    WhichDataType sdt = -2;
    if ( valueStr == string("none") ) sdt = DATA_None;
    else if ( valueStr == string("veclen")  ) sdt = DATA_Veclen;
    else if ( valueStr.substr(0, 4) == string("data") ) {
      string numStr = valueStr.substr(4);
      if ( ! numStr.empty() ) sdt = atoi(numStr.c_str()) + 1;
    }
    if ( ! setSelectedData(sdt) ) {
      ErrMsg(MsgERR, msgHdr + "command " + nameStr
             + ": set failed: " + valueStr);
      return false;
    }
  } // end of "set_use_data"
  else if ( nameStr == string("set_vec_idx") ) {
    Vec3<int> idcs(-1, -1, -1);
    istringstream iss(valueStr);
    iss >> idcs;
    if ( idcs[0] < 0 || idcs[1] < 0 ||  idcs[2] < 0 ) {
      ErrMsg(MsgERR, msgHdr + "command " + nameStr
             + string(": bad value format: ") + valueStr);
      return false;
    }
    if ( ! setVecDataIdx(idcs) ) {
      ErrMsg(MsgERR, msgHdr + "command " + nameStr
             + string(": failed to set: ") + valueStr);
      return false;
    }
  } // end of "set_vec_idx"
  else if ( nameStr == "set_use_cmap" ) {
    bool ucm;
    if ( valueStr == string("yes") ) ucm = true;
    else if ( valueStr == string("no") ) ucm = false;
    else {
      ErrMsg(MsgERR, msgHdr + string("command set_use_cmap: invalid value"));
      return false;
    }
    if ( ! setUseCMap(ucm) ) {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_use_cmap: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_use_cmap"
  else if ( nameStr == "set_upd_minmax" ) {
    bool mmupd;
    if ( valueStr == string("yes") ) mmupd = true;
    else if ( valueStr == string("no") ) mmupd = false;
    else {
      ErrMsg(MsgERR, msgHdr + string("command set_upd_minmax: invalid value"));
      return false;
    }
    if ( ! setUpdateMinMaxMode(mmupd) ) {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_upd_minmax: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_upd_minmax"
  else if ( nameStr == "set_antialias" ) {
    bool am;
    if ( valueStr == string("yes") ) am = true;
    else if ( valueStr == string("no") ) am = false;
    else {
      ErrMsg(MsgERR, msgHdr +
             string("invalid command: set_antialias: invalid value"));
      return false;
    }
    if ( ! setAntiAliasMode(am) ) {
      ErrMsg(MsgERR, msgHdr +
             string("command set_antialias: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_antialias"
  else if ( nameStr == string("set_line_width") ) {
    float lw = (float)atof(valueStr.c_str());
    if ( ! setLineWidth(lw) ) {
      ErrMsg(MsgERR, msgHdr +
	    string("command set_line_width: set failed: ") + valueStr);
      return false;
    }
  } // end of "line_width"
  else if ( nameStr == "set_enable_fast_method" ) {
    bool efm;
    if ( valueStr == string("yes") ) efm = true;
    else if ( valueStr == string("no") ) efm = false;
    else {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_enable_fast_method: invalid value"));
      return false;
    }
    if ( ! setEnableFastMethod(efm) ) {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_enable_fast_method: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_enable_fast_method"
  else if ( nameStr == "set_enable_visibility_testing" ) {
    bool evt;
    if ( valueStr == string("yes") ) evt = true;
    else if ( valueStr == string("no") ) evt = false;
    else {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_enable_test_visibility: invalid value"));
      return false;
    }
    if ( ! setEnableVisibilityTesting(evt) ) {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_enable_visibility_testing: set failed: ") +
	     valueStr);
      return false;
    }
  } // end of "enable_visibility_testing"
  else if ( nameStr == "set_selected_iso_value" ) {
    int siv = (int)atoi(valueStr.c_str());
    if ( ! setSelectedIsoValue(siv) ) {
      ErrMsg(MsgERR, msgHdr +
	    string("command set_selected_iso_value: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_selected_iso_value"
  else if ( nameStr == "set_iso_value 0" ||
            nameStr == "set_iso_value 1" ||
            nameStr == "set_iso_value 2"    ) {
    int iv;
    char name[32];
    const int nscan = sscanf(nameStr.c_str(), "%s %d", name, &iv);
    if ( nscan < 2 ) {
      ErrMsg(MsgERR, msgHdr + string("command set_iso_value: invalid value"));
      return false;
    }
    float val = (float)atof(valueStr.c_str());
    if ( ! setIsoValue(iv, val) ) {
      ErrMsg(MsgERR, msgHdr +
	    string("command set_iso_value: set failed: ") + valueStr);
      return false;
    }
  } // end of "iso_value"
  else if ( nameStr == "set_show_contour 0" ||
            nameStr == "set_show_contour 1" ||
            nameStr == "set_show_contour 2"    ) {
    int iv;
    char name[32];
    const int nscan = sscanf(nameStr.c_str(), "%s %d", name, &iv);
    if ( nscan < 2 ) {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_show_contour: invalid value"));
      return false;
    }

    bool sc;
    if ( valueStr == string("yes") ) sc = true;
    else if ( valueStr == string("no") ) sc = false;
    else {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_show_contour: invalid value"));
      return false;
    }
    if ( ! setShowContour(iv, sc) ) {
      ErrMsg(MsgERR, msgHdr +
	    string("command set_show_contour: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_show_contour" 
  else if ( nameStr == "set_show_suggestive_contour 0" ||
            nameStr == "set_show_suggestive_contour 1" ||
            nameStr == "set_show_suggestive_contour 2"    ) {
    int iv;
    char name[32];
    const int nscan = sscanf(nameStr.c_str(), "%s %d", name, &iv);
    if ( nscan < 2 ) {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_show_suggestive_contour: invalid value"));
      return false;
    }
    bool ssc;
    if ( valueStr == string("yes") ) ssc = true;
    else if ( valueStr == string("no") ) ssc = false;
    else {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_show_suggestive_contour: invalid value"));
      return false;
    }
    if ( ! setShowSuggestiveContour(iv, ssc) ) {
      ErrMsg(MsgERR, msgHdr +
	    string("command set_show_suggestive_contour: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_show_suggestive_contour"
  else if ( nameStr == "enable_cutting_plane 0" ||
            nameStr == "enable_cutting_plane 1" ||
            nameStr == "enable_cutting_plane 2"    ) {
    int iv;
    char name[32];
    const int nscan = sscanf(nameStr.c_str(), "%s %d", name, &iv);
    if ( nscan < 2 ) {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_cutting_plane: invalid value"));
      return false;
    }
    bool ecp;
    if ( valueStr == string("yes") ) ecp = true;
    else if ( valueStr == string("no") ) ecp = false;
    else {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_enable_cutting_plane: invalid value"));
      return false;
    }
    if ( ! setEnableCuttingPlane(iv, ecp) ) {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_enable_cutting_plane: set failed: ") +
	     valueStr);
      return false;
    }
  } // end of "enable_cutting_plane"
  if ( nameStr == "set_sampler" ) {
    if ( valueStr == VFR_NONAME ) {
      ErrMsg(MsgERR, msgHdr + "command " + nameStr
	    + ": can't set sampler with no name");
      return false;
    }
    if ( ! setSampler(p_refData, valueStr) ) {
      ErrMsg(MsgERR, msgHdr + "command " + nameStr
	    + ": set failed: " + valueStr);
      return false;
    }
  } // end of "set_sampler"
  else {
    // not my command
    ErrMsg(MsgERR, msgHdr + string("unknown command: ") + nameStr);
    return false;
  }

  return true;
}

/* for VLD */


bool vsnMethod_Sv_VLD::_isEqualIntervals(){

  vsnData_Sv* pData = dynamic_cast<vsnData_Sv*>(p_refData);
  if ( ! pData ) return 0.f;

  Vec3<size_t> dims = pData->getDims();

  // get bbox
  const Vec3<float>* pbb = pData->getBbox();
  const float dx = (fabs(pbb[1].m_v[0] - pbb[0].m_v[0]) / (dims[0] - 1));
  const float dy = (fabs(pbb[1].m_v[1] - pbb[0].m_v[1]) / (dims[1] - 1));
  const float dz = (fabs(pbb[1].m_v[2] - pbb[0].m_v[2]) / (dims[2] - 1));

  return ( (fabs(dx - dy) <= VLD::ACCEPTABLE_DELTA) &&
           (fabs(dy - dz) <= VLD::ACCEPTABLE_DELTA) &&
           (fabs(dx - dz) <= VLD::ACCEPTABLE_DELTA)    );
}

void vsnMethod_Sv_VLD::_outPutErrorMessage(const std::string &msg){
  const std::string emsg = getMethodType() + 
                           std::string("[") + 
                           getName() + 
                           std::string("]:") + msg;
  const std::size_t svf = vsnApp::GetApp()->getNumViewFrame();
  for ( std::size_t ivf = 0; ivf < svf; ++ivf ) {
    vsnViewFrame* pvf = vsnApp::GetApp()->getViewFrame(ivf);
    pvf->setStatusText(emsg);
  }
}


void vsnMethod_Sv_VLD::_updateContour(){

  const VLD::V3f viewPos   = _getViewPosition();
  const bool enableSampler = _enableSampler();

  for ( int iv = 0; iv < MaxIsoValues; ++ iv ) {
    m_contours[iv]->getPrivateMaterial()->setRenderMode(RT_NONE);
    m_suggestiveContours[iv]->getPrivateMaterial()->setRenderMode(RT_NONE);
    m_intersectPlane[iv]->getPrivateMaterial()->setRenderMode(RT_NONE);

    if ( ! m_pd ) continue;

    const bool updateViewPosContour
      = (viewPos - _viewPosContour[iv]).length() > VLD::ACCEPTABLE_DELTA;
    const bool updateViewPosSuggestiveContour
      = (viewPos - _viewPosSuggestiveContour[iv]).length()
      > VLD::ACCEPTABLE_DELTA;
    const bool showContour = m_showContour[iv];
    const bool showSuggestiveContour
      = m_showSuggestiveContour[iv] && ! m_enableFastMethod;
    const bool enableCuttingPlane
      = m_enableCuttingPlane[iv] && enableSampler;
    const bool updateVisibility
      = _updateVisibility || ( _updateSampler[iv] && enableCuttingPlane ); 

    // contour / suggestive contour / intersect plane を表示するか否かを決定===
    if ( showContour )
      m_contours[iv]->getPrivateMaterial()->setRenderMode(RT_WIRE);
    if ( showSuggestiveContour )
      m_suggestiveContours[iv]->getPrivateMaterial()->setRenderMode(RT_WIRE);
    if ( enableCuttingPlane )
      m_intersectPlane[iv]->getPrivateMaterial()->setRenderMode(RT_WIRE);

    // --------------- contour に関して更新するか否かを決定 ---------------
    if ( showContour ) {
      if ( _updateIsoValueContour[iv] || _needRemake ) { 
        // iso-value が変化したか、もしくは強制的にリメイクする場合
        _updateContour(iv);
        _setLinesContour(iv, m_enableVisibilityTesting, enableCuttingPlane);
        _updateIsoValueContour[iv] = false;
        _viewPosContour[iv]        = viewPos;
      }
      else 
      if ( updateViewPosContour ) { 
      // 視点が変化した場合
        if ( ! m_enableFastMethod ) {
          // fast methodが有効でない場合
          _updateContour(iv);
          _setLinesContour(iv, m_enableVisibilityTesting, enableCuttingPlane);
          _updateIsoValueContour[iv] = false;
          _viewPosContour[iv]        = viewPos;
        }
        else {
          // fast methodが有効な場合
          _updateContourByFastMethod(iv);
          _setLinesContour(iv, m_enableVisibilityTesting, enableCuttingPlane);
          _updateIsoValueContour[iv] = false;
          _viewPosContour[iv]        = viewPos;
        }
      } 
      else
      if ( updateVisibility ) {
        _setLinesContour(iv, m_enableVisibilityTesting, enableCuttingPlane);
      }
    } // end of if ( showContour )

    // ----------- suggestive contour に関して更新するか否かを決定 -----------
    if ( showSuggestiveContour ) {
      if ( _updateIsoValueSuggestiveContour[iv] || _needRemake ) { 
        // iso-value が変化したか、もしくは強制的にリメイクする場合
        _updateSuggestiveContour(iv);
        _setLinesSuggestiveContour(iv, m_enableVisibilityTesting,
				   enableCuttingPlane);
        _updateIsoValueSuggestiveContour[iv] = false;
        _viewPosSuggestiveContour[iv]        = viewPos;
      }
      else 
      if ( updateViewPosSuggestiveContour ) { 
        // 視点が変化した場合
        _updateSuggestiveContour(iv);
        _setLinesSuggestiveContour(iv, m_enableVisibilityTesting,
				   enableCuttingPlane);
        _updateIsoValueSuggestiveContour[iv] = false;
        _viewPosSuggestiveContour[iv]        = viewPos;
      } 
      else
      if ( updateVisibility ) {
        _setLinesSuggestiveContour(iv, m_enableVisibilityTesting,
				   enableCuttingPlane);
      }
    } // end of if ( showSuggestiveContour )

    // ----------- intersect plane に関して更新するか否かを決定 -------------
    if ( enableCuttingPlane ) {
      if ( _updateIsoValueIntersectPlane[iv] || _needRemake ) {
        // iso-valueが変化した場合 、もしくは強制的にリメイクする場合
        _updateIntersectPlane(iv);
        _setLinesIntersectPlane(iv);
        _updateIsoValueIntersectPlane[iv] = false;
        _updateSampler[iv]                = false;
      }
      else
      if ( _updateSampler[iv] ) {
        // iso-valueが変化した場合 、もしくは強制的にリメイクする場合
        _updateIntersectPlane(iv);
        _setLinesIntersectPlane(iv);
        _updateIsoValueIntersectPlane[iv] = false;
        _updateSampler[iv]                = false;
      }
    }  // end of if ( enableCuttingPlane )

  } 
  _updateVisibility = false;
  _needRemake       = false;
}

void vsnMethod_Sv_VLD::_updateContour(const int iv){
  float *sptr = m_pd;
  if ( ! sptr ) return;
 
  const float isoValue     = m_isoValue[iv];
  const VLD::V3i dataSize  = _getDataSize();
  const VLD::V3f origin    = _getOrigin();
  const float cellScale    = _getCellScale();
  const VLD::V3f viewPos   = _getViewPosition();

  _contourMaker[iv].setData(isoValue, sptr, dataSize, cellScale,
			    origin, viewPos);
  if ( ! _contourMaker[iv].make(_linesContour[iv]) ) {
    _outPutErrorMessage(std::string("make contour failed"));
    _linesContour[iv].clear();
    return;
  }

}

void vsnMethod_Sv_VLD::_updateContourByFastMethod(const int iv){
  float *sptr = m_pd;
  if ( ! sptr ) return;

  const int previousFound = (int)_linesContour[iv].size();

  const float isoValue     = m_isoValue[iv];
  const VLD::V3i dataSize  = _getDataSize();
  const VLD::V3f origin    = _getOrigin();
  const float cellScale    = _getCellScale();
  const VLD::V3f viewPos   = _getViewPosition();

  _contourMaker[iv].setData(isoValue, sptr, dataSize, cellScale,
			    origin, viewPos);
  if ( ! _contourMaker[iv].makeBySeedTraverseMethod(_linesContour[iv]) ) {
    _outPutErrorMessage(std::string("make contour(fast method) failed"));
    _linesContour[iv].clear();
    return;
  }

  // 高速化手法により見つけたコンタの数/前フレームのコンタの比がある値
  // (ACCEPTABLE_FOUND_RATE)よりも低かった場合、
  // 全てのセルを検索し、コンタを抽出する
  const int presentFound = (int)_linesContour[iv].size();
  const int rate         = (float)presentFound / previousFound * 100.f;
  if ( previousFound == 0 || presentFound == 0 || rate < ACCEPTABLE_FOUND_RATE )
    {
      _updateContour(iv);
      return;
    }

}

void vsnMethod_Sv_VLD::_updateSuggestiveContour(const int iv){
  float *sptr = m_pd;
  if ( ! sptr ) return;
  
  const float isoValue     = m_isoValue[iv];
  const VLD::V3i dataSize  = _getDataSize();
  const VLD::V3f origin    = _getOrigin();
  const float cellScale    = _getCellScale();
  const VLD::V3f viewPos   = _getViewPosition();

  VLD::SuggestiveContourMaker suggestiveContourMaker;
  suggestiveContourMaker.setData(isoValue, sptr, dataSize, cellScale,
				 origin, viewPos);
  if ( ! suggestiveContourMaker.make(_linesSuggestiveContour[iv]) ) {
    _outPutErrorMessage(std::string("make suggestive contour failed"));
    _linesSuggestiveContour[iv].clear();
    return;
  }

  return;
}

void vsnMethod_Sv_VLD::_updateIntersectPlane(const int iv){
  float *sptr = m_pd;
  if ( ! sptr ) return;

  const float isoValue       = m_isoValue[iv];
  const VLD::V3i dataSize    = _getDataSize();
  const VLD::V3f origin      = _getOrigin();
  const float cellScale      = _getCellScale();
  const VLD::V3f planeNormal = _getCuttingPlaneNormal();
  const VLD::V3f planePos    = _getCuttingPlanePosition();

  VLD::IntersectPlaneMaker intersectPlaneMaker;
  intersectPlaneMaker.setData(isoValue, sptr, dataSize, cellScale,
			      origin, planeNormal, planePos);
  if ( ! intersectPlaneMaker.make(_linesIntersectPlane[iv]) ) {
    _outPutErrorMessage(std::string("make intersect plane failed"));
    _linesIntersectPlane[iv].clear();
    return;
  }

}

void vsnMethod_Sv_VLD::_setLinesContour(const int iv,
					const bool enableVisibilityTesting,
					const bool enableCuttingPlane){
  float *sptr = m_pd;
  if (  !sptr || ! m_contours[iv] ) return;

  const float isoValue       = m_isoValue[m_selectedIsoValue];
  const VLD::V3i dataSize    = _getDataSize();
  const VLD::V3f origin      = _getOrigin();
  const float cellScale      = _getCellScale();
  const VLD::V3f viewPos     = _getViewPosition();
  const VLD::V3f planeNormal = _getCuttingPlaneNormal();
  const VLD::V3f planePos    = _getCuttingPlanePosition();

  VLD::VisibilityTester visTester;
  visTester.setData(isoValue, sptr, dataSize, cellScale, origin, viewPos,
		    planeNormal, planePos, enableCuttingPlane);

  VLD::Lines visibleLines;
  if ( enableCuttingPlane ) {

    VLD::Lines lines;
    if ( ! visTester.testCuttingPlane(_linesContour[iv], lines) ) {
      _outPutErrorMessage(std::string("visibility testing(cutting plane) "
				      "failed"));
      lines.clear();
      return;
    }
    
    if ( enableVisibilityTesting ) {
      if ( ! visTester.testIsoValue(lines, visibleLines) ) {
        _outPutErrorMessage(std::string("visibility testing failed"));
        visibleLines.clear();
        return;
      }
    }
    else
      visibleLines = lines;

  } 
  else {

    if ( enableVisibilityTesting ) { 
      if ( ! visTester.testIsoValue(_linesContour[iv], visibleLines) ) {
        _outPutErrorMessage(std::string("visibility testing failed"));
        visibleLines.clear();
        return;
      }
    }
    else 
      visibleLines = _linesContour[iv];
  }

  if ( ! m_contours[iv]->alcVerts((int)visibleLines.size()) ) {
    _outPutErrorMessage(std::string("memory allocation failed"));
    return;
  }

  const int sv = m_contours[iv]->getNumVerts();
  vector3 *vl = m_contours[iv]->getVerts();

  for ( int ver = 0; ver < sv; ++ver ) {
    vl[ver][0] = visibleLines[ver].x; 
    vl[ver][1] = visibleLines[ver].y; 
    vl[ver][2] = visibleLines[ver].z; 
  }

  const vector3 ori = {origin.x   , origin.y   , origin.z   };
  const vector3 sca = {cellScale, cellScale, cellScale};
  m_contours[iv]->identity();
  m_contours[iv]->trans(ori);
  m_contours[iv]->scale(sca);

  return;
}

void
vsnMethod_Sv_VLD::_setLinesSuggestiveContour(const int iv,
					     const bool enableVisibilityTesting,
					     const bool enableCuttingPlane){
  float *sptr = m_pd;
  if ( ! sptr || ! m_suggestiveContours[iv] ) return;

  const clock_t t0 = clock();

  const float isoValue       = m_isoValue[m_selectedIsoValue];
  const VLD::V3i dataSize    = _getDataSize();
  const VLD::V3f origin      = _getOrigin();
  const float cellScale      = _getCellScale();
  const VLD::V3f viewPos     = _getViewPosition();
  const VLD::V3f planeNormal = _getCuttingPlaneNormal();
  const VLD::V3f planePos    = _getCuttingPlanePosition();

  VLD::VisibilityTester visTester;
  visTester.setData(isoValue, sptr, dataSize, cellScale, origin, viewPos,
		    planeNormal, planePos, enableCuttingPlane);

  VLD::Lines visibleLines;
  if ( enableCuttingPlane ) {

    VLD::Lines lines;
    if ( ! visTester.testCuttingPlane(_linesSuggestiveContour[iv], lines) ) {
      _outPutErrorMessage(std::string("visibility testing(cutting plane) "
				      "failed"));
      lines.clear();
      return;
    }
    
    if ( enableVisibilityTesting ) {
      if ( ! visTester.testIsoValue(lines, visibleLines) ) {
        _outPutErrorMessage(std::string("visibility testing failed"));
        visibleLines.clear();
        return;
      }
    }
    else
      visibleLines = lines;

  } 
  else {

    if ( enableVisibilityTesting ) { 
      if ( ! visTester.testIsoValue(_linesSuggestiveContour[iv], visibleLines) )
	{
	  _outPutErrorMessage(std::string("visibility testing failed"));
	  visibleLines.clear();
	  return;
	}
    }
    else 
      visibleLines = _linesSuggestiveContour[iv];
  }

  if ( ! m_suggestiveContours[iv]->alcVerts((int)visibleLines.size()) ) {
    _outPutErrorMessage(std::string("memory allocation failed"));
    return;
  }

  const int sv = m_suggestiveContours[iv]->getNumVerts();
  vector3 *vl  = m_suggestiveContours[iv]->getVerts();

  for ( int ver = 0; ver < sv; ++ver ) {
    vl[ver][0] = visibleLines[ver].x; 
    vl[ver][1] = visibleLines[ver].y; 
    vl[ver][2] = visibleLines[ver].z; 
  }

  const vector3 ori = {origin.x   , origin.y   , origin.z   };
  const vector3 sca = {cellScale, cellScale, cellScale};
  m_suggestiveContours[iv]->identity();
  m_suggestiveContours[iv]->trans(ori);
  m_suggestiveContours[iv]->scale(sca);

  return;
}

void vsnMethod_Sv_VLD::_setLinesIntersectPlane(const int iv){
  float *sptr = m_pd;
  if ( ! sptr || ! m_intersectPlane[iv] ) return;

  if ( ! m_intersectPlane[iv]->alcVerts((int)_linesIntersectPlane[iv].size()) )
    {
      _outPutErrorMessage(std::string("memory allocation failed"));
      return;
    }
  const int sv = m_intersectPlane[iv]->getNumVerts();
  vector3 *vl  = m_intersectPlane[iv]->getVerts();

  for ( int ver = 0; ver < sv; ++ver ) {
    vl[ver][0] = _linesIntersectPlane[iv][ver].x; 
    vl[ver][1] = _linesIntersectPlane[iv][ver].y; 
    vl[ver][2] = _linesIntersectPlane[iv][ver].z; 
  }

  const VLD::V3f origin    = _getOrigin();
  const float cellScale    = _getCellScale();

  const vector3 ori = {origin.x   , origin.y   , origin.z};
  const vector3 sca = {cellScale, cellScale, cellScale};
  m_intersectPlane[iv]->identity();
  m_intersectPlane[iv]->trans(ori);
  m_intersectPlane[iv]->scale(sca);

  return;
}


VLD::V3i vsnMethod_Sv_VLD::_getDataSize(){
  vsnData_Sv* pData = dynamic_cast<vsnData_Sv*>(p_refData);
  if ( ! pData ) return VLD::V3i(0, 0, 0);

  Vec3<size_t> dims = pData->getDims();
  return VLD::V3i((int)dims[0], (int)dims[1], (int)dims[2]);
}

VLD::V3f vsnMethod_Sv_VLD::_getOrigin(){
  vsnData_Sv* pData = dynamic_cast<vsnData_Sv*>(p_refData);
  if ( ! pData ) return VLD::V3f(0.f, 0.f, 0.f);

  // get bbox
  const Vec3<float>* pbb = pData->getBbox();
  return VLD::V3f(pbb[0].m_v[0], pbb[0].m_v[1], pbb[0].m_v[2]);
}

float vsnMethod_Sv_VLD::_getCellScale(){
  vsnData_Sv* pData = dynamic_cast<vsnData_Sv*>(p_refData);
  if ( ! pData ) return 0.f;

  Vec3<size_t> dims = pData->getDims();

  // get bbox
  const Vec3<float>* pbb = pData->getBbox();
  const float dx = (fabs(pbb[1].m_v[0] - pbb[0].m_v[0]) / (dims[0] - 1));

  return dx;
}


VLD::V3f vsnMethod_Sv_VLD::_getViewPosition(){
  GLfloat m_v[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, m_v);
  VLD::M44f mv(m_v);
  return VLD::gg::inverse(mv) * VLD::V4f(0.f, 0.f, 0.f, 1.f);
}

VLD::V3f vsnMethod_Sv_VLD::_getCuttingPlaneNormal(){
  // check sampler
  if ( ! p_splr ) return VLD::V3f(0.f, 0.f, 0.f);
  if ( ! p_splr->getSamplePoints() ) 
    return VLD::V3f(0.f, 0.f, 0.f);

  const Vec3<float> n = p_splr->getNormalVec();
  return VLD::V3f(n[0], n[1], n[2]);
}

VLD::V3f vsnMethod_Sv_VLD::_getCuttingPlanePosition(){
  // check sampler
  if ( ! p_splr ) return VLD::V3f(0.f, 0.f, 0.f);
  if ( ! p_splr->getSamplePoints() ) 
    return VLD::V3f(0.f, 0.f, 0.f);

  const Vec3<float> pos = p_splr->getT();
  return VLD::V3f(pos[0], pos[1], pos[2]);
}


bool vsnMethod_Sv_VLD::_enableSampler(){
  // check sampler
  if ( ! p_splr ) return false;
  if ( ! p_splr->getSamplePoints() ) return false;
  return true;
}
