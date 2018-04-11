//
// vsnMethod_plotContour
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

#include "vsnMethod_plotContour.h"
#include "vsnError.h"
#include "vsnOctTree.h" // for decomp vecIdx

using namespace std;
using namespace CES;
using namespace VSN;


//----------------------------------------------------------------
// class vsnMPP_plotContour
//----------------------------------------------------------------

BEGIN_EVENT_TABLE(vsnMPP_plotContour, wxPanel)
  EVT_TEXT_ENTER(MPP_plotContour_SamplerTxt,
                 vsnMPP_plotContour::OnEnterSamplerTxt)
  EVT_BUTTON(MPP_plotContour_SetSamplerBtn,
             vsnMPP_plotContour::OnSetSamplerBtn)
  EVT_COMBOBOX(MPP_plotContour_SelDataLst,
	       vsnMPP_plotContour::OnSelDataLst)
  EVT_CHECKLISTBOX(MPP_plotContour_VecDataChkLst,
                   vsnMPP_plotContour::OnVecDataChkLst)
  EVT_TEXT_ENTER(MPP_plotContour_NumLinesTxt,
                 vsnMPP_plotContour::OnNumLinesTxt)
  EVT_TEXT_ENTER(MPP_plotContour_MinTxt,
                 vsnMPP_plotContour::OnMinMaxTxt)
  EVT_TEXT_ENTER(MPP_plotContour_MaxTxt,
                 vsnMPP_plotContour::OnMinMaxTxt)
  EVT_CHECKBOX(MPP_plotContour_UseCMapChk,
               vsnMPP_plotContour::OnUseCMapChk)
  EVT_CHECKBOX(MPP_plotContour_UpdMinMaxChk,
               vsnMPP_plotContour::OnUpdMinMaxChk)
  EVT_TEXT_ENTER(MPP_plotContour_LineWidthTxt,
                 vsnMPP_plotContour::OnLineWidthTxt)
  EVT_CHECKBOX(MPP_plotContour_AntiAliasChk,
               vsnMPP_plotContour::OnAntiAliasChk)
END_EVENT_TABLE()


/* constructors / destructor */

vsnMPP_plotContour::vsnMPP_plotContour(wxPanel* parent, vsnMethodObj* pm)
  : vsnMethodPP(parent, pm)
{
  assert(parent);
  vsnMethod_plotContour* pMtd = dynamic_cast<vsnMethod_plotContour*>(pm);
  assert(pMtd);

  // create widgets
  m_pSamplerLst = new wxListBox(this, MPP_plotContour_SamplerLst);
  assert(m_pSamplerLst);
  m_pSamplerTxt = new wxTextCtrl(this, MPP_plotContour_SamplerTxt,
                                 wxT(""), wxDefaultPosition, wxDefaultSize,
                                 wxTE_READONLY|wxTE_PROCESS_ENTER);
  assert(m_pSamplerTxt);
  m_pSetSamplerBtn = new wxButton(this, MPP_plotContour_SetSamplerBtn,
                                  wxT("set sampler"));
  assert(m_pSetSamplerBtn);
  m_pSelDataLst = new wxComboBox(this, MPP_plotContour_SelDataLst,
				 wxT(""), wxDefaultPosition, wxDefaultSize,
				 0, NULL, wxCB_READONLY);
  assert(m_pSelDataLst);
  m_pVecDataChkLst = new wxCheckListBox(this, MPP_plotContour_VecDataChkLst);
  assert(m_pVecDataChkLst);
  m_pNumLinesTxt = new wxTextCtrl(this, MPP_plotContour_NumLinesTxt,
                                  wxT(""), wxDefaultPosition, wxDefaultSize,
                                  wxTE_PROCESS_ENTER);
  assert(m_pNumLinesTxt);
  m_pMinTxt = new wxTextCtrl(this, MPP_plotContour_MinTxt,
                             wxT(""), wxDefaultPosition, wxSize(60,-1),
                             wxTE_PROCESS_ENTER);
  assert(m_pMinTxt);
  m_pMaxTxt = new wxTextCtrl(this, MPP_plotContour_MaxTxt,
                             wxT(""), wxDefaultPosition, wxSize(60,-1),
                             wxTE_PROCESS_ENTER);
  assert(m_pMaxTxt);
  m_pUseCMapChk = new wxCheckBox(this, MPP_plotContour_UseCMapChk,
                                 wxT("use cmap"));
  assert(m_pUseCMapChk);
  m_pUpdMinMaxChk = new wxCheckBox(this, MPP_plotContour_UpdMinMaxChk,
                                   wxT("update lut minmax"));
  assert(m_pUpdMinMaxChk);
  m_pLineWidthTxt = new wxTextCtrl(this, MPP_plotContour_LineWidthTxt,
                                   wxT(""), wxDefaultPosition, wxDefaultSize,
                                   wxTE_PROCESS_ENTER);
  assert(m_pLineWidthTxt);
  m_pAntiAliasChk = new wxCheckBox(this, MPP_plotContour_AntiAliasChk,
                                   wxT("anti-alias line"));
  assert(m_pAntiAliasChk);

  // prepare sizers
  wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL); assert(topsizer);
  wxBoxSizer* sizerH;

  // sampler selector
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND|wxALL, 3);
  sizerH->Add(new wxStaticText(this, -1, wxT("selected")),
	      0, wxALIGN_LEFT|wxALL, 3);
  sizerH->Add(m_pSamplerTxt, 0, wxEXPAND|wxALL, 3);
  topsizer->Add(m_pSamplerLst, 0, wxEXPAND|wxALL, 3);
  topsizer->Add(m_pSetSamplerBtn, 0, wxALIGN_LEFT|wxALL, 3);
  topsizer->Add(new wxStaticLine(this, -1, wxDefaultPosition, wxSize(3,3),
				 wxHORIZONTAL), 0, wxEXPAND|wxALL, 0);
  topsizer->Add(5, 5);
  if ( ! pMtd->getUseSampler() ) {
    m_pSamplerTxt->Disable();
    m_pSamplerLst->Disable();
    m_pSetSamplerBtn->Disable();
  }

  // data selector
  topsizer->Add(new wxStaticText(this, -1, wxT("select scalar data")),
                0, wxALIGN_LEFT|wxALL, 3);
  topsizer->Add(m_pSelDataLst, 0, wxEXPAND|wxALL, 3);

  // vector data selector
  topsizer->Add(new wxStaticText(this, -1, wxT("vector components")),
                0, wxALIGN_LEFT|wxALL, 3);
  topsizer->Add(m_pVecDataChkLst, 0, wxEXPAND|wxALL, 3);

  // num lines
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxALL, 3);
  sizerH->Add(new wxStaticText(this, -1, wxT("#of lines")),
              0, wxALIGN_LEFT|wxALL, 3);
  sizerH->Add(m_pNumLinesTxt, 0, wxALIGN_LEFT|wxALL, 3);

  // range
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxALL, 3);
  sizerH->Add(new wxStaticText(this, -1, wxT("range")),
              0, wxALIGN_LEFT|wxALL, 3);
  sizerH->Add(m_pMinTxt, 1, wxALIGN_LEFT|wxALL, 3);
  sizerH->Add(new wxStaticText(this, -1, wxT("to")),
              0, wxALIGN_LEFT|wxALL, 3);
  sizerH->Add(m_pMaxTxt, 1, wxALIGN_LEFT|wxALL, 3);

  // use CMap / update minmax mode
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxALL, 3);
  sizerH->Add(m_pUseCMapChk, 0, wxALL, 3);
  m_pUseCMapChk->SetValue(TRUE);

  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxALL, 3);
  sizerH->Add(m_pUpdMinMaxChk, 0, wxALL, 3);
  m_pUpdMinMaxChk->SetValue(TRUE);

  // line width
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxALL, 3);
  sizerH->Add(new wxStaticText(this, -1, wxT("line width")),
              0, wxALIGN_LEFT|wxALL, 3);
  sizerH->Add(m_pLineWidthTxt, 0, wxALIGN_LEFT|wxALL, 3);

  // anti-alias
  topsizer->Add(m_pAntiAliasChk, 0, wxALL, 3);

  // post process
  SetSizer(topsizer);
  addTo(parent);
  topsizer->Layout();
  (void)update();
}

vsnMPP_plotContour::~vsnMPP_plotContour() {
}


/* interface */

bool vsnMPP_plotContour::update() {
  if ( ! m_pSamplerLst || ! m_pSamplerTxt || ! m_pSetSamplerBtn ||
       ! m_pSelDataLst || ! m_pVecDataChkLst || 
       ! m_pNumLinesTxt || ! m_pMinTxt || ! m_pMaxTxt ||
       ! m_pUseCMapChk || ! m_pUpdMinMaxChk ||
       ! m_pLineWidthTxt || ! m_pAntiAliasChk )
    return false;

  vsnMethod_plotContour* pm
    = dynamic_cast<vsnMethod_plotContour*>(p_method);
  if ( ! pm ) return false;
  vsnNumericalDataIF* pdo
    = dynamic_cast<vsnNumericalDataIF*>(pm->getRefData());
  if ( ! pdo ) return false;

  register size_t i;
  char txt[64];

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

  // select scalar data
  int dlen = (int)pdo->getDataLen();
  if ( m_pSelDataLst->GetCount() < 1 ) {
    m_pSelDataLst->Append(wxT("None"));
    if ( dlen > 0 ) {
      for ( i = 0; i < dlen; i++ ) {
	sprintf(txt, "data%lu", i);
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
      sprintf(txt, "data%lu", i);
      m_pVecDataChkLst->Append(vsnApp::ConvSysToWx(txt));
    } // end of for(i)

    Vec3<int> vidx = pm->getVecDataIdx();
    if ( vidx[0] >= 0 && vidx[0] < dlen ) m_pVecDataChkLst->Check(vidx[0]);
    if ( vidx[1] >= 0 && vidx[1] < dlen ) m_pVecDataChkLst->Check(vidx[1]);
    if ( vidx[2] >= 0 && vidx[2] < dlen ) m_pVecDataChkLst->Check(vidx[2]);
  }

  // num lines
  size_t nl = pm->getNumLines();
  sprintf(txt, "%lu", nl); m_pNumLinesTxt->SetValue(vsnApp::ConvSysToWx(txt));

  // range
  float min, max; pm->getRange(min, max);
  sprintf(txt, "%g", min); m_pMinTxt->SetValue(vsnApp::ConvSysToWx(txt));
  sprintf(txt, "%g", max); m_pMaxTxt->SetValue(vsnApp::ConvSysToWx(txt));

  // use CMap
  m_pUseCMapChk->SetValue(pm->getUseCMap());

  // update minmax mode
  m_pUpdMinMaxChk->SetValue(pm->getUpdateMinMaxMode());

  // line width
  sprintf(txt, "%g", pm->getLineWidth());
  m_pLineWidthTxt->SetValue(vsnApp::ConvSysToWx(txt));

  // anti-alias
  m_pAntiAliasChk->SetValue(pm->getAntiAliasMode());

  return true;
}


/* event handler */

void vsnMPP_plotContour::OnSetSamplerBtn(wxCommandEvent& event) {
  if ( ! m_pSamplerLst || ! m_pSamplerTxt || ! m_pSetSamplerBtn )
    return;

  int sel = m_pSamplerLst->GetSelection();
  if ( sel < 0 ) return;
  m_pSamplerTxt->SetValue(m_pSamplerLst->GetString(sel));

  vsnMethod_plotContour* pm
    = dynamic_cast<vsnMethod_plotContour*>(p_method);
  if ( ! pm ) return;
  vsnDataObj* pdo = pm->getRefData();
  if ( ! pdo ) return;

  pm->setSampler(pdo, sel);
  pm->chkNotice();
}

void vsnMPP_plotContour::OnEnterSamplerTxt(wxCommandEvent& event) {
  if ( ! m_pSamplerTxt ) return;

  vsnMethod_plotContour* pm
    = dynamic_cast<vsnMethod_plotContour*>(p_method);
  if ( ! pm ) return;
  vsnMethod_sampler* psplr = pm->getSampler();
  if ( ! psplr ) return;
  vsnUiView* puiv = getUiView();
  if ( ! puiv ) return;

  puiv->selectObj((vfrNode*)psplr);
}

void vsnMPP_plotContour::OnSelDataLst(wxCommandEvent& event) {
  if ( ! m_pSelDataLst ) return;
  WhichDataType sel = event.GetInt();
  if ( sel < 0 ) return;

  wxString valStr = m_pSelDataLst->GetString(sel);
  if ( valStr == wxT("vector length") )
    sel = DATA_Veclen;

  vsnMethod_plotContour* pm
    = dynamic_cast<vsnMethod_plotContour*>(p_method);
  if ( ! pm ) return;
  if ( sel == pm->getSelectedData() ) return;

  if ( ! pm->setSelectedData(sel) ) {
    update();
    return;
  }
  pm->chkNotice();
}

void vsnMPP_plotContour::OnVecDataChkLst(wxCommandEvent& event) {
  if ( ! m_pVecDataChkLst ) return;
  int dlen = m_pVecDataChkLst->GetCount();
  if ( dlen < 3 ) return;

  vsnMethod_plotContour* pm
    = dynamic_cast<vsnMethod_plotContour*>(p_method);
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

void vsnMPP_plotContour::OnNumLinesTxt(wxCommandEvent& event) {
  if ( ! m_pNumLinesTxt ) return;

  vsnMethod_plotContour* pm
    = dynamic_cast<vsnMethod_plotContour*>(p_method);
  if ( ! pm ) return;

  wxString valStr = m_pNumLinesTxt->GetValue();
  if ( valStr.IsEmpty() ) return;
  int val = atoi(vsnApp::ConvWxToSys(valStr).c_str());
  if ( val < 1 ) {
    char txt[64]; sprintf(txt, "%lu", pm->getNumLines());
    m_pNumLinesTxt->SetValue(vsnApp::ConvSysToWx(txt));
    return;
  }

  if ( pm->setNumLines((size_t)val) )
    pm->chkNotice();
}

void vsnMPP_plotContour::OnMinMaxTxt(wxCommandEvent& event) {
  if ( ! m_pMinTxt || ! m_pMaxTxt ) return;

  vsnMethod_plotContour* pm
    = dynamic_cast<vsnMethod_plotContour*>(p_method);
  if ( ! pm ) return;

  wxString valStr = m_pMinTxt->GetValue();
  if ( valStr.IsEmpty() ) return;
  float min = (float)atof(vsnApp::ConvWxToSys(valStr).c_str());
  valStr = m_pMaxTxt->GetValue();
  if ( valStr.IsEmpty() ) return;
  float max = (float)atof(vsnApp::ConvWxToSys(valStr).c_str());

  if ( pm->setRange(min, max) )
    pm->chkNotice();
  else {
    pm->getRange(min, max);
    char txt[64];
    sprintf(txt, "%g", min); m_pMinTxt->SetValue(vsnApp::ConvSysToWx(txt));
    sprintf(txt, "%g", max); m_pMaxTxt->SetValue(vsnApp::ConvSysToWx(txt));
  }
}

void vsnMPP_plotContour::OnUseCMapChk(wxCommandEvent& event) {
  if ( ! m_pUseCMapChk ) return;
  bool val = m_pUseCMapChk->GetValue();

  vsnMethod_plotContour* pm
    = dynamic_cast<vsnMethod_plotContour*>(p_method);
  if ( ! pm ) return;

  if ( pm->setUseCMap(val) )
    pm->chkNotice();
}

void vsnMPP_plotContour::OnUpdMinMaxChk(wxCommandEvent& event) {
  if ( ! m_pUpdMinMaxChk ) return;
  bool val = m_pUpdMinMaxChk->GetValue();

  vsnMethod_plotContour* pm
    = dynamic_cast<vsnMethod_plotContour*>(p_method);
  if ( ! pm ) return;

  if ( pm->setUpdateMinMaxMode(val) )
    pm->chkNotice();
}

void vsnMPP_plotContour::OnLineWidthTxt(wxCommandEvent& event) {
  if ( ! m_pLineWidthTxt ) return;
  vsnMethod_plotContour* pm
    = dynamic_cast<vsnMethod_plotContour*>(p_method);
  if ( ! pm ) return;

  wxString valStr = m_pLineWidthTxt->GetValue();
  if ( valStr.IsEmpty() ) return;
  float val = (float)atof(vsnApp::ConvWxToSys(valStr).c_str());
  if ( val <= 0.f ) return;

  if ( pm->setLineWidth(val) )
    pm->chkNotice();
}

void vsnMPP_plotContour::OnAntiAliasChk(wxCommandEvent& event) {
  if ( ! m_pAntiAliasChk ) return;
  vsnMethod_plotContour* pm
    = dynamic_cast<vsnMethod_plotContour*>(p_method);
  if ( ! pm ) return;

  bool val = m_pAntiAliasChk->GetValue();
  if ( pm->setAntiAliasMode(val) )
    pm->chkNotice();
}


//----------------------------------------------------------------
// class vsnMethod_plotContour
//----------------------------------------------------------------

/* constructors / destructor */

vsnMethod_plotContour::
vsnMethod_plotContour(const std::string& name)
  : vsnMethodObj(name), m_selectedData(DATA_None), m_vecDataIdx(0,1,2),
    m_numLines(5), m_min(0.f), m_max(1.f),
    m_useCMap(true), m_updateMinMax(true),
    m_lineWidth(1.f), m_numValidLines(0), m_useSampler(true)
{
  m_useLut = m_useCMap;
}

vsnMethod_plotContour::~vsnMethod_plotContour() {
}


/* methods */

void vsnMethod_plotContour::adjustRange() {
  bool needRangeUpd = false;

  vsnNumericalDataIF* pData = dynamic_cast<vsnNumericalDataIF*>(p_refData);
  if ( ! pData ) return;
  int dlen = (int)pData->getDataLen();

  float dr[2];
  if ( m_selectedData == DATA_Veclen ) {
    if ( pData->getVectorMaxLen(m_vecDataIdx, dr[1]) ) {
      m_lut.minVal = 0.f;
      m_lut.maxVal = dr[1];
      needRangeUpd = true;
    }
  } // end of if(DATA_Veclen)
  else if ( m_selectedData > 0 && m_selectedData <= dlen ) {
    if ( pData->getMinMax(m_selectedData -1, dr) ) {
      m_lut.minVal = dr[0];
      m_lut.maxVal = dr[1];
      needRangeUpd = true;
    }
  }

  setUseLut(m_useCMap);
  
  if ( m_updateMinMax ) {
    m_min = m_lut.minVal;
    m_max = m_lut.maxVal;
  }

  if ( needRangeUpd ) {
    set<vsnMethodLutRefer*>::iterator it;
    for ( it = m_mlrLst.begin(); it != m_mlrLst.end(); it++ )
      if ( *it ) (*it)->updateLut();
  }
}

// STATIC
bool vsnMethod_plotContour::
calcLineSegment(const vector3 p0, const vector3 p1, const vector3 p2,
                const float val0, const float val1, const float val2,
                const float val, vector3 lp1, vector3 lp2) {
  register int i, j, idx;
  char vFlg[3] = {0, 0, 0};

  j = 0;
  if ( val0 > val ) {vFlg[0] = 1; j++;}
  if ( val1 > val ) {vFlg[1] = 1; j++;}
  if ( val2 > val ) {vFlg[2] = 1; j++;}
  if ( j == 0 || j == 3 ) return false; // contour line not crossed

  Vec3<float> pos[3] = {Vec3<float>(p0), Vec3<float>(p1), Vec3<float>(p2)};
  Vec3<float> lp;
  float vals[3] = {val0, val1, val2};
  float t;
  int top = -1; int bot[2] = {-1, -1};
  if ( j == 1 ) {
    for ( idx = 0, i = 0; i < 3; i++ )
      if ( vFlg[i] == 1 ) top = i;
      else bot[idx++] = i;
  } else {
    for ( idx = 0, i = 0; i < 3; i++ )
      if ( vFlg[i] == 0 ) top = i;
      else bot[idx++] = i;
  }

  // lp1: top---bot[0]
  if ( vals[top] == vals[bot[0]] ) return false; // something wrong
  t = (val - vals[top]) / (vals[bot[0]] - vals[top]);
  lp = pos[top] + (pos[bot[0]] - pos[top]) * t;
  lp1[0] = lp[0]; lp1[1] = lp[1]; lp1[2] = lp[2];

  // lp2 : top---bot[1]
  if ( vals[top] == vals[bot[1]] ) return false; // something wrong
  t = (val - vals[top]) / (vals[bot[1]] - vals[top]);
  lp = pos[top] + (pos[bot[1]] - pos[top]) * t;
  lp2[0] = lp[0]; lp2[1] = lp[1]; lp2[2] = lp[2];

  return true;
}

bool vsnMethod_plotContour::setSelectedData(const WhichDataType sd) {
  if ( sd == m_selectedData ) return true;
  m_selectedData = sd;

  if ( m_updateMinMax )
    adjustRange();

  if ( ! update() ) return false;
  updateUI();
  return true;
}

bool vsnMethod_plotContour::setVecDataIdx(const Vec3<int>& vdidx) {
  if ( vdidx[0] == m_vecDataIdx[0] &&
       vdidx[1] == m_vecDataIdx[1] &&
       vdidx[2] == m_vecDataIdx[2] ) return true;
  m_vecDataIdx = vdidx;

  if ( m_updateMinMax )
    adjustRange();

  if ( ! update() ) return false;
  updateUI();
  return true;
}

bool vsnMethod_plotContour::isValidVecData() const {
  vsnNumericalDataIF* pData = dynamic_cast<vsnNumericalDataIF*>(p_refData);
  if ( ! pData ) return false;

  size_t dlen = pData->getDataLen();
  if ( m_vecDataIdx[0] < 0 || m_vecDataIdx[0] >= dlen ) return false;
  if ( m_vecDataIdx[1] < 0 || m_vecDataIdx[1] >= dlen ) return false;
  if ( m_vecDataIdx[2] < 0 || m_vecDataIdx[2] >= dlen ) return false;
  return true;
}

bool vsnMethod_plotContour::setNumLines(const size_t nl) {
  if ( m_numLines == nl ) return true;
  if ( nl < 1 ) return false;
  m_numLines = nl;

  if ( ! update() ) return false;
  updateUI();
  return true;
}

bool vsnMethod_plotContour::setRange(const float min, const float max) {
  if ( m_min == min && m_max == max ) return true;
  if ( min > max ) return false;
  m_min = min; m_max = max;

  if ( ! update() ) return false;
  updateUI();
  return true;
}

bool vsnMethod_plotContour::setUseCMap(const bool ucm) {
  if ( m_useCMap == ucm ) return true;
  m_useCMap = ucm;

  setUseLut(m_useCMap);
  updateUI();
  return true;
}

bool vsnMethod_plotContour::setUpdateMinMaxMode(const bool mode) {
  if ( m_updateMinMax == mode ) return true;
  m_updateMinMax = mode;

  // data range
  if ( m_updateMinMax )
    adjustRange();

  if ( ! update() ) return false;
  updateUI();
  return true;
}

bool vsnMethod_plotContour::setLineWidth(const float lw) {
  if ( m_lineWidth == lw ) return true;
  if ( lw <= 0.f ) return false;
  m_lineWidth = lw;

  if ( _material )
    _material->setLineWidth(m_lineWidth);

  updateUI();
  return true;
}


/* from vsnMethodObj */

bool vsnMethod_plotContour::update(const bool force) {
  return updateStep(m_requestedStp, force);
}

void vsnMethod_plotContour::reloaded() {
  adjustRange();
  vsnMethodObj::reloaded();
}

vsnMethodPP* vsnMethod_plotContour::getParamPanel(wxPanel* pp) {
  vsnMPP_plotContour* pp_plotContour
    = new vsnMPP_plotContour(pp, this);
  if ( ! pp_plotContour ) return NULL;
  return pp_plotContour;
}

void vsnMethod_plotContour::setUseLut(const bool ulm) {
  if ( m_useLut == ulm ) return;
  m_useLut = ulm;

  if ( m_useLut ) {
    if ( m_numLines == m_numValidLines ) {
      register float val,
        dv = (m_numLines < 2) ? 0.f : (m_max - m_min)/(float)(m_numLines - 1);
      register int k;
      for ( val = m_min, k = 0; k < m_numLines; val += dv, k++ ) {
        vfrLines* plns = dynamic_cast<vfrLines*>(getChild(k));
        if ( ! plns ) continue;
        plns->setColor3(0, &m_lut.lutEntry[m_lut.getValIdx(val) * 4]);
      } // end of for(val, k)
    }
    else {
      update();
    }
  } else {
    setBaseColor(m_colour);
  }

  chkNotice();
}

void vsnMethod_plotContour::setLut(const vsnLut& ol) {
  m_lut = ol;
  updateUI();

  set<vsnMethodLutRefer*>::iterator it;
  for ( it = m_mlrLst.begin(); it != m_mlrLst.end(); it++ )
    if ( *it ) (*it)->updateLut();

  if ( m_useLut ) {
    if ( m_numLines == m_numValidLines ) {
      register float val,
        dv = (m_numLines < 2) ? 0.f : (m_max - m_min)/(float)(m_numLines - 1);
      register int k;
      for ( val = m_min, k = 0; k < m_numLines; val += dv, k++ ) {
        vfrLines* plns = dynamic_cast<vfrLines*>(getChild(k));
        if ( ! plns ) continue;
        plns->setColor3(0, &m_lut.lutEntry[m_lut.getValIdx(val) * 4]);
      } // end of for(val, k)
    }
    else {
      update();
    }
  } // end of if(useLut)

  if ( m_useLut || m_mlrLst.size() > 0 )
    chkNotice();
}


/* from vsnRef_sampler */

void vsnMethod_plotContour::noticeUpdate() {
  update();
}


/* from vsnIoObject */

bool vsnMethod_plotContour::parseXML(xmlNodePtr xnp) {
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

      if ( xsN == string("sampler") ) {
        if ( ! setSampler(p_refData, xsV) ) {
          ErrMsg(MsgERR, msgHdr + string("failed to set sampler: ") + xsV);
          goto _NEXT_XML_NODE;
        }
      } // end of "sampler"
      else if ( xsN == string("use_data") ) {
        WhichDataType sdt = -2;
        if ( xsV == string("none") ) sdt = DATA_None;
        else if ( xsV == string("veclen")  ) sdt = DATA_Veclen;
        else if ( xsV.substr(0, 4) == string("data") ) {
          string numStr = xsV.substr(4);
          if ( ! numStr.empty() ) sdt = atoi(numStr.c_str()) + 1;
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
      else if ( xsN == string("num_lines") ) {
        int nl = atoi(xsV.c_str());
        if ( ! setNumLines(nl) ) {
          ErrMsg(MsgERR,msgHdr + string("invalid value in param num_lines"));
          goto _NEXT_XML_NODE;
        }
      } // end of "num_lines"
      else if ( xsN == string("data_range") ) {
        string minStr, maxStr; char c;
        istringstream iss(xsV);
        while( iss.get(c) && c != ':' ) minStr.push_back(c);
        while( iss.get(c) ) maxStr.push_back(c);
        if ( minStr.empty() || maxStr.empty() ) {
          ErrMsg(MsgERR, msgHdr
                 + string("invalid format in param data_range"));
          goto _NEXT_XML_NODE;
        }
        float min = (float)atof(minStr.c_str());
        float max = (float)atof(maxStr.c_str());
        if ( ! setRange(min, max) ) {
          ErrMsg(MsgERR, msgHdr + string("invalid value in param data_range"));
          goto _NEXT_XML_NODE;
        }
      } // end of "data_range"
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
          ErrMsg(MsgERR, msgHdr + string("can't set upd_minmax"));
          goto _NEXT_XML_NODE;
        }
      } // end of "upd_minmax"
      else if ( xsN == string("line_width") ) {
        float lw = (float)atof(xsV.c_str());
        if ( ! setLineWidth(lw) ) {
          ErrMsg(MsgERR, msgHdr + string("invalid value in param line_width"));
          goto _NEXT_XML_NODE;
        }
      } // end of "line_width"
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
    } // end of param

  _NEXT_XML_NODE:
    cur = cur->next;
  } // end of while(cur)

  return true;
}

bool vsnMethod_plotContour::outputXML(std::ostream& os, const size_t ts)
{
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

  // output original params
  // upd_minmax
  if ( ! m_updateMinMax ) {
    os << idts_2 << "<param name=\"upd_minmax\" value=\"no\" />" << endl;
    os << idts_2 << "<param name=\"data_range\" value=\""
       << m_min << ':' << m_max << "\" />" << endl;
  }

  // sampler
  if ( p_splr ) {
    if ( ! vsnRef_sampler::exportXMLNode(os, ts+2) ) {
      ErrMsg(MsgWARN, msgHdr
             + string("sampler has set, but the sampler has no name,\n")
             + string("so don't output sampler param node"));
    }
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

  // num_lines
  if ( m_numLines != 1 ) {
    os << idts_2 << "<param name=\"num_lines\" value=\""
       << m_numLines << "\" />" << endl;
  }

  // use_cmap
  if ( ! m_useCMap ) {
    os << idts_2 << "<param name=\"use_cmap\" value=\"no\" />" << endl;
  }

  // line_width
  if ( m_lineWidth != 1.f ) {
    os << idts_2 << "<param name=\"line_width\" value=\""
       << m_lineWidth << "\" />" << endl;
  }

  // antialias
  if ( m_antiAlias ) {
    os << idts_2 << "<param name=\"antialias\" value=\"yes\" />" << endl;
  }

  // output base method params
  if ( ! vsnMethodObj::exportXMLNode(os, ts+2) ) {
    ErrMsg(MsgERR, msgHdr + string("base MethodObj exportXMLNode failed"));
    ret = false;
  }

  os << idts << "</method>" << endl;
  return true;
}

bool vsnMethod_plotContour::commandXML(xmlNodePtr xnp) {
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
  else if ( nameStr == "set_use_data" ) {
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
  else if ( nameStr == "set_num_lines" ) {
    int nl = atoi(valueStr.c_str());
    if ( ! setNumLines(nl) ) {
      ErrMsg(MsgERR, msgHdr +
             string("command set_num_lines: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_num_lines"
  else if ( nameStr == "set_data_range" ) {
    string minStr, maxStr; char c;
    istringstream iss(valueStr);
    while( iss.get(c) && c != ':' ) minStr.push_back(c);
    while( iss.get(c) ) maxStr.push_back(c);
    if ( minStr.empty() || maxStr.empty() ) {
      ErrMsg(MsgERR, msgHdr
             + string("command set_data_range: invalid format"));
      return false;
    }
    float min = (float)atof(minStr.c_str());
    float max = (float)atof(maxStr.c_str());
    if ( ! setRange(min, max) ) {
      ErrMsg(MsgERR, msgHdr +
             string("command set_data_range: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_data_range"
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
  else if ( nameStr == "set_line_width" ) {
    float lw = (float)atof(valueStr.c_str());
    if ( ! setLineWidth(lw) ) {
      ErrMsg(MsgERR, msgHdr +
             string("command set_line_width: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_line_width"
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
  else {
    // not my command
    ErrMsg(MsgERR, msgHdr + string("unknown command: ") + nameStr);
    return false;
  }

  return true;
}
