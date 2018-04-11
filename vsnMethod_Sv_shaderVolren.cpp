//
// vsnMethod_Sv_shaderVolren
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

#include "vsnMethod_Sv_shaderVolren.h"
#include "vsnOctTree.h" // for decomp vecIdx
#include "vsnUiView.h"
#include "vsnColorBar.h"
#include "vsnError.h"

using namespace std;
using namespace CES;
using namespace VSN;


//----------------------------------------------------------------
// class vsnMPP_Sv_shaderVolren
//----------------------------------------------------------------

BEGIN_EVENT_TABLE(vsnMPP_Sv_shaderVolren, wxPanel)
  EVT_COMBOBOX(MPP_Sv_shaderVolren_SelDataLst,
               vsnMPP_Sv_shaderVolren::OnSelDataLst)
  EVT_CHECKLISTBOX(MPP_Sv_shaderVolren_VecDataChkLst,
                   vsnMPP_Sv_shaderVolren::OnVecDataChkLst)
  EVT_CHECKBOX(MPP_Sv_shaderVolren_UpdMinMaxChk,
               vsnMPP_Sv_shaderVolren::OnUpdMinMaxChk)
  EVT_TEXT_ENTER(MPP_Sv_shaderVolren_SliceNumTxt,
		 vsnMPP_Sv_shaderVolren::OnSliceNumTxt)
  EVT_BUTTON(MPP_Sv_shaderVolren_GradMapBtn,
	     vsnMPP_Sv_shaderVolren::OnGradMapBtn)
  EVT_BUTTON(MPP_Sv_shaderVolren_RegionResetBtn,
	     vsnMPP_Sv_shaderVolren::OnRegionResetBtn)
  EVT_TEXT_ENTER(MPP_Sv_shaderVolren_RegionX1Txt,
		 vsnMPP_Sv_shaderVolren::OnRegionTxt)
  EVT_TEXT_ENTER(MPP_Sv_shaderVolren_RegionX2Txt,
		 vsnMPP_Sv_shaderVolren::OnRegionTxt)
  EVT_TEXT_ENTER(MPP_Sv_shaderVolren_RegionY1Txt,
		 vsnMPP_Sv_shaderVolren::OnRegionTxt)
  EVT_TEXT_ENTER(MPP_Sv_shaderVolren_RegionY2Txt,
		 vsnMPP_Sv_shaderVolren::OnRegionTxt)
  EVT_TEXT_ENTER(MPP_Sv_shaderVolren_RegionZ1Txt,
		 vsnMPP_Sv_shaderVolren::OnRegionTxt)
  EVT_TEXT_ENTER(MPP_Sv_shaderVolren_RegionZ2Txt,
		 vsnMPP_Sv_shaderVolren::OnRegionTxt)
END_EVENT_TABLE()


/* constructors / destructor */

vsnMPP_Sv_shaderVolren::vsnMPP_Sv_shaderVolren(wxPanel* parent,
					       vsnMethodObj* pm)
: vsnMethodPP(parent, pm)
{
  assert(parent);
  assert(dynamic_cast<vsnMethod_Sv_shaderVolren*>(pm));

  wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer* sizerH;

  // data selector
  topsizer->Add(new wxStaticText(this, -1, wxT("select scalar data")),
                0, wxALIGN_LEFT|wxALL, 3);
  m_pSelDataLst = new wxComboBox(this, MPP_Sv_shaderVolren_SelDataLst,
                                 wxT(""), wxDefaultPosition, wxDefaultSize,
                                 0, NULL, wxCB_READONLY);
  topsizer->Add(m_pSelDataLst, 0, wxEXPAND|wxALL, 3);

  // vector data selector
  topsizer->Add(new wxStaticText(this, -1, wxT("vector components")),
                0, wxALIGN_LEFT|wxALL, 3);
  m_pVecDataChkLst = new wxCheckListBox(this,
                                        MPP_Sv_shaderVolren_VecDataChkLst);
  topsizer->Add(m_pVecDataChkLst, 0, wxEXPAND|wxALL, 3);

  // update minmax mode
  m_pUpdMinMaxChk = new wxCheckBox(this, MPP_Sv_shaderVolren_UpdMinMaxChk,
                                   wxT("update minmax"));
  m_pUpdMinMaxChk->SetValue(TRUE);
  topsizer->Add(m_pUpdMinMaxChk, 0, wxALL, 3);

  // #of slices
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxALL, 0);
  sizerH->Add(new wxStaticText(this, -1, wxT("#of slices")),
              0, wxALIGN_LEFT|wxALL, 3);
  m_pSliceNumTxt = new wxTextCtrl(this, MPP_Sv_shaderVolren_SliceNumTxt,
				  wxT(""), wxDefaultPosition, wxDefaultSize,
				  wxTE_PROCESS_ENTER);
  sizerH->Add(m_pSliceNumTxt, 1, wxEXPAND|wxALL, 3);

  // regions
  topsizer->Add(5, 5);
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND|wxALL, 3);
  sizerH->Add(new wxStaticText(this, -1, wxT("crop region")),
	      0, wxALIGN_LEFT|wxALL, 3);
  m_pRegionResetBtn = new wxButton(this, MPP_Sv_shaderVolren_RegionResetBtn,
			       wxT("reset region"),
			       wxDefaultPosition, wxDefaultSize);
  sizerH->Add(10, 5);
  sizerH->Add(m_pRegionResetBtn, 0, wxALIGN_LEFT|wxALL, 3);

  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND|(wxALL & ~wxTOP), 5);
  sizerH->Add(new wxStaticText(this, -1, wxT("x1")), 0, wxALIGN_LEFT|wxALL, 3);
  m_pRegionX1Txt = new wxTextCtrl(this, MPP_Sv_shaderVolren_RegionX1Txt,
				  wxT("0"), wxDefaultPosition, wxSize(60,-1),
				  wxTE_PROCESS_ENTER);
  sizerH->Add(m_pRegionX1Txt, 1, wxEXPAND|wxALL, 3);
  sizerH->Add(new wxStaticText(this, -1, wxT("x2")), 0, wxALIGN_LEFT|wxALL, 3);
  m_pRegionX2Txt = new wxTextCtrl(this, MPP_Sv_shaderVolren_RegionX2Txt,
				  wxT("1"), wxDefaultPosition, wxSize(60,-1),
				  wxTE_PROCESS_ENTER);
  sizerH->Add(m_pRegionX2Txt, 1, wxEXPAND|wxALL, 3);

  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND|(wxALL & ~wxTOP), 5);
  sizerH->Add(new wxStaticText(this, -1, wxT("y1")), 0, wxALIGN_LEFT|wxALL, 3);
  m_pRegionY1Txt = new wxTextCtrl(this, MPP_Sv_shaderVolren_RegionY1Txt,
				  wxT("0"), wxDefaultPosition, wxSize(60,-1),
				  wxTE_PROCESS_ENTER);
  sizerH->Add(m_pRegionY1Txt, 1, wxEXPAND|wxALL, 3);
  sizerH->Add(new wxStaticText(this, -1, wxT("y2")), 0, wxALIGN_LEFT|wxALL, 3);
  m_pRegionY2Txt = new wxTextCtrl(this, MPP_Sv_shaderVolren_RegionY2Txt,
				  wxT("1"), wxDefaultPosition, wxSize(60,-1),
				  wxTE_PROCESS_ENTER);
  sizerH->Add(m_pRegionY2Txt, 1, wxEXPAND|wxALL, 3);

  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND|(wxALL & ~wxTOP), 5);
  sizerH->Add(new wxStaticText(this, -1, wxT("z1")), 0, wxALIGN_LEFT|wxALL, 3);
  m_pRegionZ1Txt = new wxTextCtrl(this, MPP_Sv_shaderVolren_RegionZ1Txt,
				  wxT("0"), wxDefaultPosition, wxSize(60,-1),
				  wxTE_PROCESS_ENTER);
  sizerH->Add(m_pRegionZ1Txt, 1, wxEXPAND|wxALL, 3);
  sizerH->Add(new wxStaticText(this, -1, wxT("z2")), 0, wxALIGN_LEFT|wxALL, 3);
  m_pRegionZ2Txt = new wxTextCtrl(this, MPP_Sv_shaderVolren_RegionZ2Txt,
				  wxT("1"), wxDefaultPosition, wxSize(60,-1),
				  wxTE_PROCESS_ENTER);
  sizerH->Add(m_pRegionZ2Txt, 1, wxEXPAND|wxALL, 3);

  // grad-map button
  topsizer->Add(5, 5);
  m_pGradMapBtn = new wxButton(this, MPP_Sv_shaderVolren_GradMapBtn,
			       wxT("edit gradient-map"),
			       wxDefaultPosition, wxDefaultSize);
  topsizer->Add(m_pGradMapBtn, 0, wxEXPAND|wxALL, 3);

  // post process
  SetSizer(topsizer);
  addTo(parent);
  topsizer->Layout();
  (void)update();
}

vsnMPP_Sv_shaderVolren::~vsnMPP_Sv_shaderVolren() {
}

/* interface */

bool vsnMPP_Sv_shaderVolren::update() {
  if ( ! m_pSelDataLst || ! m_pVecDataChkLst ||
       ! m_pUpdMinMaxChk || ! m_pSliceNumTxt ||
       ! m_pRegionX1Txt || ! m_pRegionY1Txt || ! m_pRegionZ1Txt ||
       ! m_pRegionX2Txt || ! m_pRegionY2Txt || ! m_pRegionZ2Txt )
    return false;

  vsnMethod_Sv_shaderVolren* pm
    = dynamic_cast<vsnMethod_Sv_shaderVolren*>(p_method);
  if ( ! pm ) return false;
  vsnData_Sv* pdo = dynamic_cast<vsnData_Sv*>(pm->getRefData());
  if ( ! pdo ) return false;

  register int i;
  char txt[64];

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

    CES::Vec3<int> vidx = pm->getVecDataIdx();
    if ( vidx[0] >= 0 && vidx[0] < dlen ) m_pVecDataChkLst->Check(vidx[0]);
    if ( vidx[1] >= 0 && vidx[1] < dlen ) m_pVecDataChkLst->Check(vidx[1]);
    if ( vidx[2] >= 0 && vidx[2] < dlen ) m_pVecDataChkLst->Check(vidx[2]);
  }

  // update minmax mode
  m_pUpdMinMaxChk->SetValue(pm->getUpdateMinMaxMode());

  // #of slices
  sprintf(txt, "%lu", pm->getNumSlices());
  m_pSliceNumTxt->SetValue(vsnApp::ConvSysToWx(txt));

  // regions
  CES::Vec3<float> region[2];
  if ( pm->getRegion(region) ) {
    sprintf(txt, "%g", region[0][0]);
    m_pRegionX1Txt->SetValue(vsnApp::ConvSysToWx(txt));
    sprintf(txt, "%g", region[1][0]);
    m_pRegionX2Txt->SetValue(vsnApp::ConvSysToWx(txt));
    sprintf(txt, "%g", region[0][1]);
    m_pRegionY1Txt->SetValue(vsnApp::ConvSysToWx(txt));
    sprintf(txt, "%g", region[1][1]);
    m_pRegionY2Txt->SetValue(vsnApp::ConvSysToWx(txt));
    sprintf(txt, "%g", region[0][2]);
    m_pRegionZ1Txt->SetValue(vsnApp::ConvSysToWx(txt));
    sprintf(txt, "%g", region[1][2]);
    m_pRegionZ2Txt->SetValue(vsnApp::ConvSysToWx(txt));
  }

  return true;
}


/* event handler */

void vsnMPP_Sv_shaderVolren::OnSelDataLst(wxCommandEvent& event) {
  if ( ! m_pSelDataLst ) return;
  WhichDataType sel = event.GetInt();
  if ( sel < 0 ) return;

  wxString valStr = m_pSelDataLst->GetString(sel);
  if ( valStr == wxT("vector length") )
    sel = DATA_Veclen;

  vsnMethod_Sv_shaderVolren* pm
    = dynamic_cast<vsnMethod_Sv_shaderVolren*>(p_method);
  if ( ! pm ) return;
  if ( sel == pm->getSelectedData() ) return;

  if ( pm->setSelectedData(sel) )
    pm->chkNotice();
  else
    update();
}

void vsnMPP_Sv_shaderVolren::OnVecDataChkLst(wxCommandEvent& event) {
  if ( ! m_pVecDataChkLst ) return;
  int dlen = m_pVecDataChkLst->GetCount();
  if ( dlen < 3 ) return;

  vsnMethod_Sv_shaderVolren* pm
    = dynamic_cast<vsnMethod_Sv_shaderVolren*>(p_method);
  if ( ! pm ) return;

  register int i, c = 0;
  CES::Vec3<int> vidx(-1, -1, -1);
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

void vsnMPP_Sv_shaderVolren::OnUpdMinMaxChk(wxCommandEvent& event) {
  if ( ! m_pUpdMinMaxChk ) return;
  bool val = m_pUpdMinMaxChk->GetValue();

  vsnMethod_Sv_shaderVolren* pm
    = dynamic_cast<vsnMethod_Sv_shaderVolren*>(p_method);
  if ( ! pm ) return;

  if ( pm->setUpdateMinMaxMode(val) )
    pm->chkNotice();
}

void vsnMPP_Sv_shaderVolren::OnSliceNumTxt(wxCommandEvent& event) {
  if ( ! m_pSliceNumTxt ) return;
  vsnMethod_Sv_shaderVolren* pm
    = dynamic_cast<vsnMethod_Sv_shaderVolren*>(p_method);
  if ( ! pm ) return;

  wxString valStr = m_pSliceNumTxt->GetValue();
  if ( valStr.IsEmpty() ) {
    char txt[64];
    sprintf(txt, "%lu", pm->getNumSlices());
    m_pSliceNumTxt->SetValue(vsnApp::ConvSysToWx(txt));
    return;
  }
  size_t val = (size_t)atoi(vsnApp::ConvWxToSys(valStr).c_str());
  if ( val == pm->getNumSlices() ) return;
  if ( pm->setNumSlices(val) )
    pm->chkNotice();
}

void vsnMPP_Sv_shaderVolren::OnGradMapBtn(wxCommandEvent& event) {
  vsnMethod_Sv_shaderVolren* pm
    = dynamic_cast<vsnMethod_Sv_shaderVolren*>(p_method);
  if ( ! pm ) return;

  vsnAMapDlg dlg(this, pm);
  dlg.SetTitle(wxT("gradient map"));
  dlg.ShowModal();
}

void vsnMPP_Sv_shaderVolren::OnRegionResetBtn(wxCommandEvent& event) {
  vsnMethod_Sv_shaderVolren* pm
    = dynamic_cast<vsnMethod_Sv_shaderVolren*>(p_method);
  if ( ! pm ) return;
  CES::Vec3<float> region[2];
  region[0] = CES::Vec3<float>(0.0f, 0.0f, 0.0f);
  region[1] = CES::Vec3<float>(1.0f, 1.0f, 1.0f);
  if ( pm->setRegion(region) )
    pm->chkNotice();  
}

void vsnMPP_Sv_shaderVolren::OnRegionTxt(wxCommandEvent& event) {
  if ( ! m_pRegionX1Txt || ! m_pRegionY1Txt || ! m_pRegionZ1Txt ||
       ! m_pRegionX2Txt || ! m_pRegionY2Txt || ! m_pRegionZ2Txt ) return;
  vsnMethod_Sv_shaderVolren* pm
    = dynamic_cast<vsnMethod_Sv_shaderVolren*>(p_method);
  if ( ! pm ) return;
  CES::Vec3<float> region[2];
  if ( ! pm->getRegion(region) ) return;

  wxString valStr; float val;
  valStr = m_pRegionX1Txt->GetValue();
  if ( ! valStr.IsEmpty() ) {
    val = (float)atof(vsnApp::ConvWxToSys(valStr).c_str());
    if ( val < 0.f ) val = 0.f;
    else if ( val > 1.f ) val = 1.f;
    region[0][0] = val;
  }
  valStr = m_pRegionX2Txt->GetValue();
  if ( ! valStr.IsEmpty() ) {
    val = (float)atof(vsnApp::ConvWxToSys(valStr).c_str());
    if ( val < 0.f ) val = 0.f;
    else if ( val > 1.f ) val = 1.f;
    region[1][0] = val;
  }
  valStr = m_pRegionY1Txt->GetValue();
  if ( ! valStr.IsEmpty() ) {
    val = (float)atof(vsnApp::ConvWxToSys(valStr).c_str());
    if ( val < 0.f ) val = 0.f;
    else if ( val > 1.f ) val = 1.f;
    region[0][1] = val;
  }
  valStr = m_pRegionY2Txt->GetValue();
  if ( ! valStr.IsEmpty() ) {
    val = (float)atof(vsnApp::ConvWxToSys(valStr).c_str());
    if ( val < 0.f ) val = 0.f;
    else if ( val > 1.f ) val = 1.f;
    region[1][1] = val;
  }
  valStr = m_pRegionZ1Txt->GetValue();
  if ( ! valStr.IsEmpty() ) {
    val = (float)atof(vsnApp::ConvWxToSys(valStr).c_str());
    if ( val < 0.f ) val = 0.f;
    else if ( val > 1.f ) val = 1.f;
    region[0][2] = val;
  }
  valStr = m_pRegionZ2Txt->GetValue();
  if ( ! valStr.IsEmpty() ) {
    val = (float)atof(vsnApp::ConvWxToSys(valStr).c_str());
    if ( val < 0.f ) val = 0.f;
    else if ( val > 1.f ) val = 1.f;
    region[1][2] = val;
  }

  if ( pm->setRegion(region) )
    pm->chkNotice();
}


//----------------------------------------------------------------
// class vsnMethod_Sv_shaderVolren
//----------------------------------------------------------------

/* constructors / destructor */

vsnMethod_Sv_shaderVolren::
vsnMethod_Sv_shaderVolren(const std::string& name)
  : vsnMethodObj(name), m_pRender(NULL), m_pd(NULL), m_crdWarned(false),
    m_selectedData(DATA_None), m_vecDataIdx(0,1,2), m_updateMinMax(true)
{
  m_pRender = new vsnRvrVolumeRender();
  assert(m_pRender);
  if ( ! m_pRender->Initialize() ) {
    ErrMsg(MsgERR, getMethodType() + string("[") + getName()
           + string("]: can't initialize RVR Volume renderer"));
  } else {
    addChild(m_pRender);
  }
  setPickMode(PT_NONE);
}

vsnMethod_Sv_shaderVolren::~vsnMethod_Sv_shaderVolren() {
  if ( m_pRender ) {
    if ( p_refData ) {
      vsnScene* psc = p_refData->getAncestorScene();
      if ( psc ) psc->delVolRender(m_pRender, this);
    }
    delete m_pRender;
  }
  if ( m_pd )
    VFR::DeAllocate(m_pd);
}


/* methods */

void vsnMethod_Sv_shaderVolren::adjustRange() {
  bool needRangeUpd = false;

  vsnData_Sv* pData = dynamic_cast<vsnData_Sv*>(p_refData);
  if ( ! pData ) return;
  int dlen = (int)pData->getDataLen();

  float dr[2];
  if ( m_selectedData == DATA_Veclen ) {
    if ( pData->getVectorMaxLen(m_vecDataIdx, dr[1]) ) {
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

  if ( needRangeUpd ) {
    set<vsnMethodLutRefer*>::iterator it;
    for ( it = m_mlrLst.begin(); it != m_mlrLst.end(); it++ )
      if ( *it ) (*it)->updateLut();
  }
}

bool vsnMethod_Sv_shaderVolren::setSelectedData(const WhichDataType sd) {
  if ( sd == m_selectedData ) return true;

  vsnData_Sv* pData = dynamic_cast<vsnData_Sv*>(p_refData);
  if ( ! pData ) return false;

  int dlen = (int)pData->getDataLen();
  if ( sd > dlen ) return false;
  if ( sd == DATA_Veclen && dlen < 3 ) return false;
  m_selectedData = sd;

  // data range
  adjustRange();

  if ( ! update() ) return false;
  updateUI();
  return true;
}

bool vsnMethod_Sv_shaderVolren::setVecDataIdx(const CES::Vec3<int>& vdidx) {
  if ( vdidx[0] == m_vecDataIdx[0] &&
       vdidx[1] == m_vecDataIdx[1] &&
       vdidx[2] == m_vecDataIdx[2] ) return true;
  m_vecDataIdx = vdidx;

  // data range
  adjustRange();

  if ( ! update() ) return false;
  updateUI();
  return true;
}

bool vsnMethod_Sv_shaderVolren::isValidVecData() const {
  vsnData_Sv* pData = dynamic_cast<vsnData_Sv*>(p_refData);
  if ( ! pData ) return false;

  size_t dlen = pData->getDataLen();
  if ( m_vecDataIdx[0] < 0 || m_vecDataIdx[0] >= dlen ) return false;
  if ( m_vecDataIdx[1] < 0 || m_vecDataIdx[1] >= dlen ) return false;
  if ( m_vecDataIdx[2] < 0 || m_vecDataIdx[2] >= dlen ) return false;
  return true;
}

bool vsnMethod_Sv_shaderVolren::setUpdateMinMaxMode(const bool mode) {
  if ( m_updateMinMax == mode ) return true;
  m_updateMinMax = mode;

  // data range
  adjustRange();

  if ( ! update() ) return false;
  updateUI();
  return true;
}

size_t vsnMethod_Sv_shaderVolren::getNumSlices() const {
  if ( ! m_pRender ) return 0;
  return m_pRender->GetSliceNum();
}

bool vsnMethod_Sv_shaderVolren::setNumSlices(const size_t nsl) {
  if ( ! m_pRender ) return false;
  if ( ! m_pRender->SetSliceNum(nsl) ) return false;
  updateUI();
  return true;
}

bool vsnMethod_Sv_shaderVolren::setRegion(const CES::Vec3<float>* preg) {
  if ( ! preg ) return false;
  if ( ! m_pRender ) return false;
  m_pRender->SetRegion(preg);
  updateUI();
  return true;
}

bool vsnMethod_Sv_shaderVolren::getRegion(CES::Vec3<float>* preg) const {
  if ( ! preg ) return false;
  if ( ! m_pRender ) return false;
  m_pRender->GetRegion(preg);
  return true;
}


/* vsnTimeSeriesMethodIF methods */

bool vsnMethod_Sv_shaderVolren::updateStep(const int stp,
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

  // check renderer
  if ( ! m_pRender ) return false;
  m_pRender->getPrivateMaterial()->setRenderMode(RT_NONE);
  if ( ! m_show ) {
    return true;
  }

  // check data
  CES::Vec3<size_t> dims = pData->getDims();
  size_t dimSz = dims[0] * dims[1] * dims[2];
  if ( dimSz < 1 ) return false;
  if ( ! m_crdWarned && ! pData->isUniformCoord() ) {
    ErrMsg(MsgWARN, getMethodType() + string("[") + getName()
           + string("]: non-uniform grid data"));
    m_crdWarned = true;
  }

  // check selected data
  size_t dlen = pData->getDataLen();
  float dr[2] = {m_lut.minVal, m_lut.maxVal};
  if ( m_selectedData == DATA_None ) return true;
  else if ( m_selectedData == DATA_Veclen ) {
    if ( ! isValidVecData() ) return true;
  } else if ( m_selectedData > dlen || m_selectedData < -1 ) {
    return true;
  }

  // get Sv data
  const float* dptr = pData->getData(m_requestedStp);
  if ( ! dptr ) {
    return false;
  }
  const unsigned char* maskptr = pData->getMask();

  // prepare data
  size_t tgt;
  float* sptr = NULL;
  if ( m_selectedData == DATA_Veclen ) {
    m_pd = (float*)VFR::ReAllocate(m_pd, sizeof(float)*dimSz);
    if ( ! m_pd ) {
      ErrMsg(MsgERR, getMethodType() + string("[") + getName()
	     + string("]: memory allocation failed"));
      return false;
    }
    register size_t i, idx;
    CES::Vec3<float> vv;
    for ( i = 0; i < dimSz; i++ ) {
      idx = i * dlen;
      vv.m_v[0] = dptr[idx + m_vecDataIdx.m_v[0]];
      vv.m_v[1] = dptr[idx + m_vecDataIdx.m_v[1]];
      vv.m_v[2] = dptr[idx + m_vecDataIdx.m_v[2]];
      m_pd[i] = vv.Length();
    } // end of for(i)
    sptr = m_pd;
    tgt = 0;
    dlen = 1;
  }
  else {
    sptr = const_cast<float*>(dptr);
    tgt = m_selectedData -1;
  }

  // set geometory
  const CES::Vec3<float>* pbb = pData->getBbox();
  m_pRender->SetGeom(pbb[0].m_v, pbb[1].m_v);

  // set lut
  if ( ! m_pRender->SetLut(m_lut) ) {
    ErrMsg(MsgERR, getMethodType() + string("[") + getName()
           + string("]: can't set LUT to the volume render"));
    return false;
  }

  // set volume data
  if ( ! m_pRender->SetData(dims.m_v, dlen, tgt, sptr, dr, maskptr) ) {
    ErrMsg(MsgERR, getMethodType() + string("[") + getName()
           + string("]: can't set data to the volume render"));
    return false;
  }

  // ok
  m_pRender->getPrivateMaterial()->setRenderMode(m_showType);
  m_updatedStp = m_requestedStp;
  return true;
}


/* vsnExtLutRefer methods */

void vsnMethod_Sv_shaderVolren::ext_setLut(const vsnLut& lut) {
  if ( ! m_pRender ) return;
  if ( m_pRender->SetDerivLut(lut) )
    chkNotice();
}

vsnLut vsnMethod_Sv_shaderVolren::ext_getLut() const {
  vsnLut lut;
  if ( m_pRender ) lut = m_pRender->GetDerivLut();
  return lut;
}


/* vsnMethodObj methods */

bool vsnMethod_Sv_shaderVolren::update(const bool force) {
  return updateStep(m_requestedStp, force);
}

void vsnMethod_Sv_shaderVolren::reloaded() {
  adjustRange();
  vsnMethodObj::reloaded();
}

vsnMethodPP* vsnMethod_Sv_shaderVolren::getParamPanel(wxPanel* pp) {
  vsnMPP_Sv_shaderVolren* pp_shaderVolren
    = new vsnMPP_Sv_shaderVolren(pp, this);
  if ( ! pp_shaderVolren ) return NULL;
  return pp_shaderVolren;
}

void vsnMethod_Sv_shaderVolren::setBaseColor(const vector4 cv) {
  memcpy(m_colour, cv, sizeof(vector4));
  updateUI();

  if ( m_pColorBar )
    m_pColorBar->setBaseColor(m_colour);

  chkNotice();
}

void vsnMethod_Sv_shaderVolren::setRefData(vsnDataObj* prd) {
  if ( p_refData == prd ) return;

  if ( p_refData ) {
    vsnScene* psc = p_refData->getAncestorScene();
    if ( psc ) psc->delVolRender(m_pRender, this);
  }

  p_refData = prd;
  if ( p_refData ) {
    vsnScene* psc = p_refData->getAncestorScene();
    if ( psc ) psc->addVolRender(m_pRender, this);
  }

  update();
}

void vsnMethod_Sv_shaderVolren::setLighting(const bool mode) {
  if ( m_pRender )
    m_pRender->SetShadingMode(mode);

  vsnMethodObj::setLighting(mode);
}

/* serialize : from vsnIoObject */

bool vsnMethod_Sv_shaderVolren::parseXML(xmlNodePtr xnp) {
  static xmlChar* xs;
  xmlNodePtr cur;

  if ( ! vsnMethodObj::parseXML(xnp) ) return false;
  string msgHdr
    = getMethodType() +string("[") +getName() +string("]: parseXML: ");

  // crop-region param
  CES::Vec3<float> region[2];
  getRegion(region);

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
        CES::Vec3<int> idcs(-1, -1, -1);
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
      else if ( xsN == string("num_slices") ) {
        int nsls = atoi(xsV.c_str());
        if ( nsls < 1 ) {
          ErrMsg(MsgERR, msgHdr+string("invalid value in param num_slices"));
          goto _NEXT_XML_NODE;
        }
        if ( ! setNumSlices((size_t)nsls) ) {
          ErrMsg(MsgERR, msgHdr + string("can't set num_slices"));
          goto _NEXT_XML_NODE;
        }
      } // end of "num_slices"
      else if ( xsN == string("region_x1") ) {
	float val = (float)atof(xsV.c_str());
	if ( val < 0.f && val > 1.f ) {
	  ErrMsg(MsgERR, msgHdr + string("invalid value in param region_x1"));
          goto _NEXT_XML_NODE;
	}
	region[0][0] = val;
      } // end of "region_x1"
      else if ( xsN == string("region_x2") ) {
	float val = (float)atof(xsV.c_str());
	if ( val < 0.f && val > 1.f ) {
	  ErrMsg(MsgERR, msgHdr + string("invalid value in param region_x2"));
          goto _NEXT_XML_NODE;
	}
	region[1][0] = val;
      } // end of "region_x2"
      else if ( xsN == string("region_y1") ) {
	float val = (float)atof(xsV.c_str());
	if ( val < 0.f && val > 1.f ) {
	  ErrMsg(MsgERR, msgHdr + string("invalid value in param region_y1"));
          goto _NEXT_XML_NODE;
	}
	region[0][1] = val;
      } // end of "region_y1"
      else if ( xsN == string("region_y2") ) {
	float val = (float)atof(xsV.c_str());
	if ( val < 0.f && val > 1.f ) {
	  ErrMsg(MsgERR, msgHdr + string("invalid value in param region_y2"));
          goto _NEXT_XML_NODE;
	}
	region[1][1] = val;
      } // end of "region_y2"
      else if ( xsN == string("region_z1") ) {
	float val = (float)atof(xsV.c_str());
	if ( val < 0.f && val > 1.f ) {
	  ErrMsg(MsgERR, msgHdr + string("invalid value in param region_z1"));
          goto _NEXT_XML_NODE;
	}
	region[0][2] = val;
      } // end of "region_z1"
      else if ( xsN == string("region_z2") ) {
	float val = (float)atof(xsV.c_str());
	if ( val < 0.f && val > 1.f ) {
	  ErrMsg(MsgERR, msgHdr + string("invalid value in param region_z2"));
          goto _NEXT_XML_NODE;
	}
	region[1][2] = val;
      } // end of "region_z2"
    } // end of param
    else if ( !strcmp((const char*)cur->name, "grad_map") ) {
      xmlNodePtr gmapNode = cur->xmlChildrenNode;
      if ( ! gmapNode || gmapNode->type != XML_TEXT_NODE )
        goto _NEXT_XML_NODE;
      if ( ! gmapNode->content || strlen((const char*)gmapNode->content) < 1 )
        goto _NEXT_XML_NODE;
      istringstream iss((const char*)gmapNode->content);
      vsnLut gmap;
      if ( ! gmap.ImportStream(iss) ) {
        ErrMsg(MsgERR, msgHdr + string("invalid grad_map node, ignore"));
        goto _NEXT_XML_NODE;
      }
      gmap.normalize();
      ext_setLut(gmap);
    } // end of grad_map

  _NEXT_XML_NODE:
    cur = cur->next;
  } // end of while(cur)

  // set crop-region
  if ( ! setRegion(region) ) {
    ErrMsg(MsgERR, msgHdr + string("can't set crop-region"));
  }

  return true;
}

bool vsnMethod_Sv_shaderVolren::outputXML(std::ostream& os, const size_t ts) {
  size_t i;
  std::string idts;
  for ( i = 0; i < ts; i++ ) idts.push_back(' ');
  string idts_2 = idts; idts_2.push_back(' '); idts_2.push_back(' ');
  bool ret= true;
  string msgHdr
    = getMethodType() +string("[") +getName() +string("]: outputXML: ");

  // output
  os << idts << "<method type=\"" << getMethodType() << "\"";
  if ( !_name.empty() && _name != string(VFR_NONAME) )
    os << " name=\"" << _name << "\"";
  os << " >" << endl;

  // output original params
  // upd_minmax
  if ( ! m_updateMinMax ) {
    os << idts_2 << "<param name=\"upd_minmax\" value=\"no\" />" << endl;
  }

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

  // num_slices
  size_t nsls = getNumSlices();
  os << idts_2 << "<param name=\"num_slices\" value=\""
     << nsls << "\" />" << endl;

  // regions
  CES::Vec3<float> region[2];
  getRegion(region);
  if ( region[0][0] != 0.0f ) {
    os << idts_2 << "<param name=\"region_x1\" value=\""
       << region[0][0] << "\" />" << endl;
  }
  if ( region[1][0] != 1.0f ) {
    os << idts_2 << "<param name=\"region_x2\" value=\""
       << region[1][0] << "\" />" << endl;
  }
  if ( region[0][1] != 0.0f ) {
    os << idts_2 << "<param name=\"region_y1\" value=\""
       << region[0][1] << "\" />" << endl;
  }
  if ( region[1][1] != 1.0f ) {
    os << idts_2 << "<param name=\"region_y2\" value=\""
       << region[1][1] << "\" />" << endl;
  }
  if ( region[0][2] != 0.0f ) {
    os << idts_2 << "<param name=\"region_z1\" value=\""
       << region[0][2] << "\" />" << endl;
  }
  if ( region[1][2] != 1.0f ) {
    os << idts_2 << "<param name=\"region_z2\" value=\""
       << region[1][2] << "\" />" << endl;
  }

  // output base method params
  if ( ! vsnMethodObj::exportXMLNode(os, ts+2) ) {
    ErrMsg(MsgERR, msgHdr + string("base MethodObj exportXMLNode failed"));
    ret = false;
  }

  // output grad_map node
  bool gmNeedOutput = false;
  vsnLut gmap = ext_getLut();
  for ( i = 0; i < VSN::LUT_MAX_ENTRY; i++ )
    if ( gmap.lutEntry[i*4+3] != 1.f ) {gmNeedOutput = true; break;}
  if ( gmNeedOutput ) {
    os << idts_2 << "<grad_map>" << endl;
    if ( ! gmap.ExportStream(os, ts+4) )
      ret = false;
    os << idts_2 << "</grad_map>" << endl;
  }

  os << idts << "</method>" << endl;
  return ret;
}

bool vsnMethod_Sv_shaderVolren::commandXML(xmlNodePtr xnp) {
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
    CES::Vec3<int> idcs(-1, -1, -1);
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
  else if ( nameStr == "set_num_slices" ) {
    int nsls = atoi(valueStr.c_str());
    if ( nsls < 1 ) {
      ErrMsg(MsgERR, msgHdr+string("command set_num_slices: invalid value"));
      return false;
    }
    if ( ! setNumSlices((size_t)nsls) ) {
      ErrMsg(MsgERR, msgHdr +
             string("command set_num_slices: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_num_slices"
  else if ( nameStr == "set_region_x1" ) {
    CES::Vec3<float> region[2];
    getRegion(region);
    region[0][0] = (float)atof(valueStr.c_str());
    if ( ! setRegion(region) ) {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_region_x1: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_region_x1"
  else if ( nameStr == "set_region_x2" ) {
    CES::Vec3<float> region[2];
    getRegion(region);
    region[1][0] = (float)atof(valueStr.c_str());
    if ( ! setRegion(region) ) {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_region_x2: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_region_x2"
  else if ( nameStr == "set_region_y1" ) {
    CES::Vec3<float> region[2];
    getRegion(region);
    region[0][1] = (float)atof(valueStr.c_str());
    if ( ! setRegion(region) ) {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_region_y1: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_region_y1"
  else if ( nameStr == "set_region_y2" ) {
    CES::Vec3<float> region[2];
    getRegion(region);
    region[1][1] = (float)atof(valueStr.c_str());
    if ( ! setRegion(region) ) {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_region_y2: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_region_y2"
  else if ( nameStr == "set_region_z1" ) {
    CES::Vec3<float> region[2];
    getRegion(region);
    region[0][2] = (float)atof(valueStr.c_str());
    if ( ! setRegion(region) ) {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_region_z1: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_region_z1"
  else if ( nameStr == "set_region_z2" ) {
    CES::Vec3<float> region[2];
    getRegion(region);
    region[1][2] = (float)atof(valueStr.c_str());
    if ( ! setRegion(region) ) {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_region_z2: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_region_z2"
  else if ( nameStr == string("set_grad_map") ) {
    vsnLut gmap;
    xmlNodePtr gmapNode = xnp->xmlChildrenNode;
    for ( ; gmapNode; gmapNode = gmapNode->next ) {
      if ( ! gmapNode || gmapNode->type != XML_TEXT_NODE )
        continue;
      if ( ! gmapNode->content || strlen((const char*)gmapNode->content) < 1 )
        continue;
      istringstream iss((const char*)gmapNode->content);
      if ( ! gmap.ImportStream(iss) ) {
        ErrMsg(MsgERR, msgHdr + string("command ") + nameStr +
               string(": invalid lut node for grad_map, ignore"));
        return false;
      }
      gmap.normalize();
      ext_setLut(gmap);
      break;
    } // end of for(gmapNode)
  } // end of "set_grad_map"
  else {
    // not my command
    ErrMsg(MsgERR, msgHdr + string("unknown command: ") + nameStr);
    return false;
  }

  return true;
}
