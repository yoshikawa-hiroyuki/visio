//
// vsnMethod_staggeredVector
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

#include "vsnMethod_Sv_staggeredVector.h"
#include "vsnOctTree.h" // for decomp vecIdx
#include "vsnUiView.h"
#include "vsnError.h"

using namespace std;
using namespace CES;
using namespace VSN;


//----------------------------------------------------------------
// class vsnMPP_Sv_staggeredVector
//----------------------------------------------------------------

BEGIN_EVENT_TABLE(vsnMPP_Sv_staggeredVector, wxPanel)
  EVT_TEXT_ENTER(MPP_Sv_staggeredVector_SlicerTxt,
                 vsnMPP_Sv_staggeredVector::OnEnterSlicerTxt)
  EVT_BUTTON(MPP_Sv_staggeredVector_SetSlicerBtn,
             vsnMPP_Sv_staggeredVector::OnSetSlicerBtn)
  EVT_RADIOBOX(MPP_Sv_staggeredVector_TypeRadio,
               vsnMPP_Sv_staggeredVector::OnTypeRadio)
  EVT_TEXT_ENTER(MPP_Sv_staggeredVector_VecScaleTxt,
                 vsnMPP_Sv_staggeredVector::OnVecScaleTxt)
  EVT_CHECKBOX(MPP_Sv_staggeredVector_VecHeadChk,
               vsnMPP_Sv_staggeredVector::OnVecHeadChk)
  EVT_CHECKBOX(MPP_Sv_staggeredVector_ShowVecXChk,
               vsnMPP_Sv_staggeredVector::OnShowVecCompChk)
  EVT_CHECKBOX(MPP_Sv_staggeredVector_ShowVecYChk,
               vsnMPP_Sv_staggeredVector::OnShowVecCompChk)
  EVT_CHECKBOX(MPP_Sv_staggeredVector_ShowVecZChk,
               vsnMPP_Sv_staggeredVector::OnShowVecCompChk)
  EVT_CHECKLISTBOX(MPP_Sv_staggeredVector_VecDataChkLst,
                   vsnMPP_Sv_staggeredVector::OnVecDataChkLst)
  EVT_COMBOBOX(MPP_Sv_staggeredVector_SelDataLst,
               vsnMPP_Sv_staggeredVector::OnSelDataLst)
  EVT_CHECKBOX(MPP_Sv_staggeredVector_UpdMinMaxChk,
               vsnMPP_Sv_staggeredVector::OnUpdMinMaxChk)
  EVT_TEXT_ENTER(MPP_Sv_staggeredVector_LineWidthTxt,
                 vsnMPP_Sv_staggeredVector::OnLineWidthTxt)
  EVT_CHECKBOX(MPP_Sv_staggeredVector_AntiAliasChk,
               vsnMPP_Sv_staggeredVector::OnAntiAliasChk)
END_EVENT_TABLE()


/* constructors / destructor */

vsnMPP_Sv_staggeredVector::vsnMPP_Sv_staggeredVector(wxPanel* parent,
						     vsnMethodObj* pm)
  : vsnMethodPP(parent, pm),
    m_pSlicerTxt(NULL), m_pSlicerLst(NULL), m_pSetSlicerBtn(NULL),
    m_pTypeRadio(NULL), m_pVecScaleTxt(NULL), m_pVecHeadChk(NULL),
    m_pShowVecXChk(NULL), m_pShowVecYChk(NULL), m_pShowVecZChk(NULL),
    m_pVecDataChkLst(NULL), m_pSelDataLst(NULL),
    m_pUpdMinMaxChk(NULL), m_pLineWidthTxt(NULL), m_pAntiAliasChk(NULL)
{
  assert(parent);
  vsnMethod_Sv_staggeredVector* psvm
    = dynamic_cast<vsnMethod_Sv_staggeredVector*>(pm);
  assert(psvm);

  // setup gfxAct
  gfxAct_KI.setRefOrthoSlicer(psvm);
  vsnUiView* puiv = getUiView();
  if ( puiv ) {
    vsnViewFrame* pvf = dynamic_cast<vsnViewFrame*>(puiv->GetParent());
    if ( pvf ) {
      vsnGfxView* pgv = pvf->getGfxView();
      if ( pgv ) {
        vfrDrawAreaWx* pda = pgv->getDrawArea();
        if ( pda ) {
          vfrDispatch& dispatcher = vfrDispatch::instance(*pda);
          gfxAct_KI.setGfxView(pgv);
          pgv->pushActions(this);
          pgv->setStandardActions();
          vfrEvKeyIn::instance(dispatcher).regist(&gfxAct_KI);
        } // end of if(pda)
      } // end of if(pgv)
    } // end of if(pvf)
  } // end of if(puvi)

  // widgets layout
  wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer* sizerH;

  // selected slicer
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND|wxALL, 3);
  sizerH->Add(new wxStaticText(this, -1, wxT("selected")),
              0, wxALIGN_LEFT|wxALL, 3);
  m_pSlicerTxt = new wxTextCtrl(this, MPP_Sv_staggeredVector_SlicerTxt,
                                wxT(""), wxDefaultPosition, wxDefaultSize,
                                wxTE_READONLY|wxTE_PROCESS_ENTER);
  sizerH->Add(m_pSlicerTxt, 0, wxEXPAND|wxALL, 3);

  // orthoSlicer selector
  m_pSlicerLst = new wxListBox(this, MPP_Sv_staggeredVector_SlicerLst);
  topsizer->Add(m_pSlicerLst, 0, wxEXPAND|wxALL, 3);

  m_pSetSlicerBtn = new wxButton(this, MPP_Sv_staggeredVector_SetSlicerBtn,
                                 wxT("set orthoSlicer"));
  topsizer->Add(m_pSetSlicerBtn, 0, wxALIGN_LEFT|wxALL, 3);

  // vector layout type
  wxString ritems[] = {wxString(wxT("Regular")),
		       wxString(wxT("Colocated")),
		       wxString(wxT("Staggered1")),
		       wxString(wxT("Staggered2"))};
  m_pTypeRadio = new wxRadioBox(this, MPP_Sv_staggeredVector_TypeRadio,
                                wxT("vector layout"),
                                wxDefaultPosition, wxDefaultSize,
                                4, ritems, 1, wxRA_SPECIFY_COLS);
  topsizer->Add(m_pTypeRadio, 0, wxEXPAND|wxALL, 3);

  // vector params
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxALL, 3);
  sizerH->Add(new wxStaticText(this, -1, wxT("vector scale")),
              0, wxALIGN_LEFT|wxALL, 3);
  m_pVecScaleTxt = new wxTextCtrl(this, MPP_Sv_staggeredVector_VecScaleTxt,
                                  wxT(""), wxDefaultPosition, wxDefaultSize,
                                  wxTE_PROCESS_ENTER);
  sizerH->Add(m_pVecScaleTxt, 0, wxALIGN_LEFT|wxALL, 3);

  m_pVecHeadChk = new wxCheckBox(this, MPP_Sv_staggeredVector_VecHeadChk,
                                 wxT("arrow head"));
  topsizer->Add(m_pVecHeadChk, 0, wxALL, 3);

  m_pShowVecXChk = new wxCheckBox(this, MPP_Sv_staggeredVector_ShowVecXChk,
				  wxT("show X component"));
  topsizer->Add(m_pShowVecXChk, 0, wxALL, 3);
  m_pShowVecYChk = new wxCheckBox(this, MPP_Sv_staggeredVector_ShowVecYChk,
				  wxT("show Y component"));
  topsizer->Add(m_pShowVecYChk, 0, wxALL, 3);
  m_pShowVecZChk = new wxCheckBox(this, MPP_Sv_staggeredVector_ShowVecZChk,
				  wxT("show Z component"));
  topsizer->Add(m_pShowVecZChk, 0, wxALL, 3);

  // vector data selector
  topsizer->Add(new wxStaticText(this, -1, wxT("vector components")),
                0, wxALIGN_LEFT|wxALL, 3);
  m_pVecDataChkLst = new wxCheckListBox(this,
                                        MPP_Sv_staggeredVector_VecDataChkLst);
  topsizer->Add(m_pVecDataChkLst, 0, wxEXPAND|wxALL, 3);

  // data selector
  topsizer->Add(new wxStaticText(this, -1, wxT("select scalar data")),
                0, wxALIGN_LEFT|wxALL, 3);
  m_pSelDataLst = new wxComboBox(this, MPP_Sv_staggeredVector_SelDataLst,
                                 wxT(""), wxDefaultPosition, wxDefaultSize,
                                 0, NULL, wxCB_READONLY);
  topsizer->Add(m_pSelDataLst, 0, wxEXPAND|wxALL, 3);

  // update minmax mode
  m_pUpdMinMaxChk = new wxCheckBox(this, MPP_Sv_staggeredVector_UpdMinMaxChk,
                                   wxT("update lut minmax"));
  m_pUpdMinMaxChk->SetValue(TRUE);
  topsizer->Add(m_pUpdMinMaxChk, 0, wxALL, 3);

  // line width
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxALL, 3);
  sizerH->Add(new wxStaticText(this, -1, wxT("line width")),
              0, wxALIGN_LEFT|wxALL, 3);
  m_pLineWidthTxt = new wxTextCtrl(this, MPP_Sv_staggeredVector_LineWidthTxt,
                                   wxT(""), wxDefaultPosition, wxDefaultSize,
                                   wxTE_PROCESS_ENTER);
  sizerH->Add(m_pLineWidthTxt, 0, wxALIGN_LEFT|wxALL, 3);

  // anti-alias
  m_pAntiAliasChk = new wxCheckBox(this, MPP_Sv_staggeredVector_AntiAliasChk,
                                   wxT("anti-alias line"));
  topsizer->Add(m_pAntiAliasChk, 0, wxALL, 3);

  // post process
  SetSizer(topsizer);
  addTo(parent);
  topsizer->Layout();
  (void)update();
}

vsnMPP_Sv_staggeredVector::~vsnMPP_Sv_staggeredVector() {
  settlement();
}


/* interface */

bool vsnMPP_Sv_staggeredVector::update() {
  if ( ! m_pSlicerTxt || ! m_pSlicerLst ||
       ! m_pTypeRadio || ! m_pVecScaleTxt || ! m_pVecHeadChk ||
       ! m_pShowVecXChk || ! m_pShowVecYChk || ! m_pShowVecZChk ||
       ! m_pVecDataChkLst || ! m_pSelDataLst ||
       ! m_pUpdMinMaxChk || ! m_pLineWidthTxt || ! m_pAntiAliasChk )
    return false;
  int val; char txt[64];

  vsnMethod_Sv_staggeredVector* pm
    = dynamic_cast<vsnMethod_Sv_staggeredVector*>(p_method);
  if ( ! pm ) return false;
  vsnData_Sv* pdata = dynamic_cast<vsnData_Sv*>(pm->getRefData());
  if ( ! pdata ) return false;

  // selected slicer
  vsnMethod_Sv_orthoSlicer* osr = pm->getOrthoSlicer();
  if ( osr )
    m_pSlicerTxt->SetValue(vsnApp::ConvSysToWx(osr->getName()));
  else
    m_pSlicerTxt->SetValue(wxT("none"));

  // slicer list
  m_pSlicerLst->Clear();
  deque<vsnMethod_Sv_orthoSlicer*> slicerMtdLst
    = pm->getOrthoSlicerList(pdata);
  deque<vsnMethod_Sv_orthoSlicer*>::iterator it;
  for ( it = slicerMtdLst.begin(); it != slicerMtdLst.end(); it++ ) {
    if ( ! *it ) continue;
    string slicerName = (*it)->getName();
    if ( slicerName.empty() )
      m_pSlicerLst->Append(wxT(VFR_NONAME));
    else
      m_pSlicerLst->Append(vsnApp::ConvSysToWx(slicerName));
  } // end of for(it)

  // vector layout
  val = (int)pm->getVecLayout();
  m_pTypeRadio->SetSelection(val);

  // vector scale
  float vs = pm->getVecScale();
  sprintf(txt, "%g", vs);
  m_pVecScaleTxt->SetValue(vsnApp::ConvSysToWx(txt));

  // show vector head
  bool vhm = pm->getVecHeadMode();
  m_pVecHeadChk->SetValue(vhm);

  // show vector components
  bool svx, svy, svz;
  pm->getShowVecComp(svx, svy, svz);
  m_pShowVecXChk->SetValue(svx);
  m_pShowVecYChk->SetValue(svy);
  m_pShowVecZChk->SetValue(svz);

  // vector data indices
  register int i;
  int dlen = (int)pdata->getDataLen();
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

  // select scalar data
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

  // update minmax mode
  m_pUpdMinMaxChk->SetValue(pm->getUpdateMinMaxMode());

  // line width
  sprintf(txt, "%g", pm->getLineWidth());
  m_pLineWidthTxt->SetValue(vsnApp::ConvSysToWx(txt));

  // anti-alias
  m_pAntiAliasChk->SetValue(pm->getAntiAliasMode());

  return true;
}

void vsnMPP_Sv_staggeredVector::settlement() {
  vsnGfxView* pgv = gfxAct_KI.getGfxView();
  if ( pgv ) {
    pgv->popActions(this);
    gfxAct_KI.setGfxView(NULL);
  }
}


/* event handler */

void vsnMPP_Sv_staggeredVector::OnSetSlicerBtn(wxCommandEvent& event) {
  if ( ! m_pSlicerLst || ! m_pSetSlicerBtn ) return;
  WhichDataType sel = m_pSlicerLst->GetSelection();
  if ( sel < 0 ) return;

  if ( m_pSlicerTxt )
    m_pSlicerTxt->SetValue(m_pSlicerLst->GetString(sel));

  vsnMethod_Sv_staggeredVector* pm
    = dynamic_cast<vsnMethod_Sv_staggeredVector*>(p_method);
  if ( ! pm ) return;
  vsnData_Sv* pdo = dynamic_cast<vsnData_Sv*>(pm->getRefData());
  if ( ! pdo ) return;

  pm->setOrthoSlicer(pdo, sel);
  pm->chkNotice();
}

void vsnMPP_Sv_staggeredVector::OnEnterSlicerTxt(wxCommandEvent& event) {
  if ( ! m_pSlicerTxt ) return;

  vsnMethod_Sv_staggeredVector* pm
    = dynamic_cast<vsnMethod_Sv_staggeredVector*>(p_method);
  if ( ! pm ) return;
  vsnMethod_Sv_orthoSlicer* posr = pm->getOrthoSlicer();
  if ( ! posr ) return;
  vsnUiView* puiv = getUiView();
  if ( ! puiv ) return;

  puiv->selectObj((vfrNode*)posr);
}

void vsnMPP_Sv_staggeredVector::OnTypeRadio(wxCommandEvent& event) {
  if ( ! m_pTypeRadio ) return;
  vsnMethod_Sv_staggeredVector* pm
    = dynamic_cast<vsnMethod_Sv_staggeredVector*>(p_method);
  if ( ! pm ) return;

  int val = m_pTypeRadio->GetSelection();
  int oval = (int)pm->getVecLayout();
  if ( val == oval ) return;

  if ( pm->setVecLayout((vsnMethod_Sv_staggeredVector::VecLayoutType)val) )
    pm->chkNotice();
}

void vsnMPP_Sv_staggeredVector::OnVecScaleTxt(wxCommandEvent& event) {
  if ( ! m_pVecScaleTxt ) return;

  vsnMethod_Sv_staggeredVector* pm
    = dynamic_cast<vsnMethod_Sv_staggeredVector*>(p_method);
  if ( ! pm ) return;

  wxString valStr = m_pVecScaleTxt->GetValue();
  if ( valStr.IsEmpty() ) return;
  float value = (float)atof(vsnApp::ConvWxToSys(valStr).c_str());

  if ( pm->setVecScale(value) )
    pm->chkNotice();
}

void vsnMPP_Sv_staggeredVector::OnVecHeadChk(wxCommandEvent& event) {
  if ( ! m_pVecHeadChk ) return;
  bool val = m_pVecHeadChk->GetValue();

  vsnMethod_Sv_staggeredVector* pm
    = dynamic_cast<vsnMethod_Sv_staggeredVector*>(p_method);
  if ( ! pm ) return;

  if ( pm->setVecHeadMode(val) )
    pm->chkNotice();
}

void vsnMPP_Sv_staggeredVector::OnShowVecCompChk(wxCommandEvent& event) {
  if ( ! m_pShowVecXChk || ! m_pShowVecYChk || ! m_pShowVecZChk )
    return;
  bool svx, svy, svz;
  svx = m_pShowVecXChk->GetValue();
  svy = m_pShowVecYChk->GetValue();
  svz = m_pShowVecZChk->GetValue();

  vsnMethod_Sv_staggeredVector* pm
    = dynamic_cast<vsnMethod_Sv_staggeredVector*>(p_method);
  if ( ! pm ) return;

  if ( pm->setShowVecComp(svx, svy, svz) )
    pm->chkNotice();
}

void vsnMPP_Sv_staggeredVector::OnVecDataChkLst(wxCommandEvent& event) {
  if ( ! m_pVecDataChkLst ) return;
  int dlen = m_pVecDataChkLst->GetCount();
  if ( dlen < 3 ) return;

  vsnMethod_Sv_staggeredVector* pm
    = dynamic_cast<vsnMethod_Sv_staggeredVector*>(p_method);
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

void vsnMPP_Sv_staggeredVector::OnSelDataLst(wxCommandEvent& event) {
  if ( ! m_pSelDataLst ) return;
  WhichDataType sel = event.GetInt();
  if ( sel < 0 ) return;

  wxString valStr = m_pSelDataLst->GetString(sel);
  if ( valStr == wxT("vector length") )
    sel = DATA_Veclen;

  vsnMethod_Sv_staggeredVector* pm
    = dynamic_cast<vsnMethod_Sv_staggeredVector*>(p_method);
  if ( ! pm ) return;
  if ( sel == pm->getSelectedData() ) return;

  if ( ! pm->setSelectedData(sel) ) {
    update();
    return;
  }
  pm->chkNotice();
}

void vsnMPP_Sv_staggeredVector::OnUpdMinMaxChk(wxCommandEvent& event) {
  if ( ! m_pUpdMinMaxChk ) return;
  bool val = m_pUpdMinMaxChk->GetValue();

  vsnMethod_Sv_staggeredVector* pm
    = dynamic_cast<vsnMethod_Sv_staggeredVector*>(p_method);
  if ( ! pm ) return;

  if ( pm->setUpdateMinMaxMode(val) )
    pm->chkNotice();
}

void vsnMPP_Sv_staggeredVector::OnLineWidthTxt(wxCommandEvent& event) {
  if ( ! m_pLineWidthTxt ) return;
  vsnMethod_Sv_staggeredVector* pm
    = dynamic_cast<vsnMethod_Sv_staggeredVector*>(p_method);
  if ( ! pm ) return;

  wxString valStr = m_pLineWidthTxt->GetValue();
  if ( valStr.IsEmpty() ) return;
  float value = (float)atof(vsnApp::ConvWxToSys(valStr).c_str());
  if ( value <= 0.f ) return;

  if ( pm->setLineWidth(value) )
    pm->chkNotice();
}

void vsnMPP_Sv_staggeredVector::OnAntiAliasChk(wxCommandEvent& event) {
  if ( ! m_pAntiAliasChk ) return;
  vsnMethod_Sv_staggeredVector* pm
    = dynamic_cast<vsnMethod_Sv_staggeredVector*>(p_method);
  if ( ! pm ) return;

  bool val = m_pAntiAliasChk->GetValue();
  if ( pm->setAntiAliasMode(val) )
    pm->chkNotice();
}


//----------------------------------------------------------------
// class vsnMethod_Sv_staggeredVector
//----------------------------------------------------------------

/* constructors / destructor */

vsnMethod_Sv_staggeredVector::vsnMethod_Sv_staggeredVector(const string& name)
  : vsnMethodObj(name), m_vecLayoutType(Staggered2),
    m_vecScale(1.f), m_vecHead(true), m_lineWidth(1.f),
    m_showVecX(true), m_showVecY(true), m_showVecZ(true),
    m_selectedData(DATA_None), m_updateMinMax(true), m_vecDataIdx(0,1,2),
    m_vectorsX(NULL), m_vectorsY(NULL), m_vectorsZ(NULL)
{
}

vsnMethod_Sv_staggeredVector::~vsnMethod_Sv_staggeredVector() {
  if ( m_vectorsX )
    delete m_vectorsX;
  if ( m_vectorsY )
    delete m_vectorsY;
  if ( m_vectorsZ )
    delete m_vectorsZ;
}


/* vsnMethodObj method */

bool vsnMethod_Sv_staggeredVector::update(const bool force) {
  return updateStep(m_requestedStp, force);
}

void vsnMethod_Sv_staggeredVector::reloaded() {
  adjustRange();
  vsnMethodObj::reloaded();
}

vsnMethodPP* vsnMethod_Sv_staggeredVector::getParamPanel(wxPanel* pp) {
  vsnMPP_Sv_staggeredVector* pp_staggeredVector
    = new vsnMPP_Sv_staggeredVector(pp, this);
  if ( ! pp_staggeredVector ) return NULL;
  return pp_staggeredVector;
}


/* methods */

void vsnMethod_Sv_staggeredVector::adjustRange() {
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

bool vsnMethod_Sv_staggeredVector::setVecLayout(const VecLayoutType vlt) {
  if ( m_vecLayoutType == vlt ) return true;
  m_vecLayoutType = vlt;

  if ( ! update() ) return false;
  updateUI();
  return true;
}

bool vsnMethod_Sv_staggeredVector::setVecScale(const float vs) {
  if ( m_vecScale == vs ) return true;
  m_vecScale = vs;

  if ( m_vectorsX )
    m_vectorsX->setScaleFac(m_vecScale);
  if ( m_vectorsY )
    m_vectorsY->setScaleFac(m_vecScale);
  if ( m_vectorsZ )
    m_vectorsZ->setScaleFac(m_vecScale);

  updateUI();
  return true;
}

bool vsnMethod_Sv_staggeredVector::setVecHeadMode(const bool vhm) {
  if ( m_vecHead == vhm ) return true;
  m_vecHead = vhm;

  if ( m_vectorsX )
    m_vectorsX->setHeadMode(m_vecHead);
  if ( m_vectorsY )
    m_vectorsY->setHeadMode(m_vecHead);
  if ( m_vectorsZ )
    m_vectorsZ->setHeadMode(m_vecHead);

  updateUI();
  return true;
}

void vsnMethod_Sv_staggeredVector::getShowVecComp(bool& svx,
						  bool& svy, bool& svz) const {
  svx = m_showVecX;
  svy = m_showVecY;
  svz = m_showVecZ;
}

bool vsnMethod_Sv_staggeredVector::setShowVecComp(const bool svx,
						  const bool svy,
						  const bool svz) {
  if ( m_showVecX == svx && m_showVecY == svy && m_showVecZ == svz )
    return true;
  m_showVecX = svx; m_showVecY = svy; m_showVecZ = svz;

  if ( m_vectorsX ) {
    if ( m_showVecX )
      m_vectorsX->getPrivateMaterial()->setRenderMode(m_showType);
    else
      m_vectorsX->getPrivateMaterial()->setRenderMode(RT_NONE);
  }
  if ( m_vectorsY ) {
    if ( m_showVecY )
      m_vectorsY->getPrivateMaterial()->setRenderMode(m_showType);
    else
      m_vectorsY->getPrivateMaterial()->setRenderMode(RT_NONE);
  }
  if ( m_vectorsZ ) {
    if ( m_showVecZ )
      m_vectorsZ->getPrivateMaterial()->setRenderMode(m_showType);
    else
      m_vectorsZ->getPrivateMaterial()->setRenderMode(RT_NONE);
  }

  updateUI();
  return true;
}

bool vsnMethod_Sv_staggeredVector::setVecDataIdx(const Vec3<int>& vdidx) {
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

bool vsnMethod_Sv_staggeredVector::isValidVecData() const {
  vsnData_Sv* pData = dynamic_cast<vsnData_Sv*>(p_refData);
  if ( ! pData ) return false;

  size_t dlen = pData->getDataLen();
  if ( m_vecDataIdx[0] < 0 || m_vecDataIdx[0] >= dlen ) return false;
  if ( m_vecDataIdx[1] < 0 || m_vecDataIdx[1] >= dlen ) return false;
  if ( m_vecDataIdx[2] < 0 || m_vecDataIdx[2] >= dlen ) return false;
  return true;
}

bool vsnMethod_Sv_staggeredVector::setSelectedData(const WhichDataType sd) {
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

bool vsnMethod_Sv_staggeredVector::setUpdateMinMaxMode(const bool mode) {
  if ( mode == m_updateMinMax ) return true;
  m_updateMinMax = mode;

  // data range
  adjustRange();

  if ( ! update() ) return false;
  updateUI();
  return true;
}

bool vsnMethod_Sv_staggeredVector::setLineWidth(const float lw) {
  if ( m_lineWidth == lw ) return true;
  if ( lw <= 0.f ) return false;

  m_lineWidth = lw;
  if ( m_vectorsX ) {
    m_vectorsX->getPrivateMaterial()->setLineWidth(m_lineWidth);
    m_vectorsX->notice();
  }
  if ( m_vectorsY ) {
    m_vectorsY->getPrivateMaterial()->setLineWidth(m_lineWidth);
    m_vectorsY->notice();
  }
  if ( m_vectorsZ ) {
    m_vectorsZ->getPrivateMaterial()->setLineWidth(m_lineWidth);
    m_vectorsZ->notice();
  }

  updateUI();
  return true;
}


/* vsnTimeSeriesMethodIF method */

#define IDX1(d,i,j,k) ((k)*(d)[0]*(d)[1] +(j)*(d)[0] +(i))
#define IDX3(d,i,j,k) (3*((k)*(d)[0]*(d)[1] +(j)*(d)[0] +(i)))

bool vsnMethod_Sv_staggeredVector::updateStep(const int stp, const bool force,
					      const bool cascade)
{
  vsnData_Sv* pData = dynamic_cast<vsnData_Sv*>(p_refData);
  if ( ! pData ) return false;
  m_requestedStp = stp;
  if ( m_requestedStp < 0 )
    m_requestedStp = pData->getCurrentStepIdx();

  if ( force ) m_updatedStp = -1;
  if ( m_updatedStp == m_requestedStp ) return true;
  m_updatedStp = -1;

  // vectors data
  if ( ! m_vectorsX ) {
    m_vectorsX = new vfrVectors();
    if ( ! m_vectorsX ) {
      ErrMsg(MsgERR, getMethodType() + string("[") + getName()
             + string("]: memory allocation failed"));
      return false;
    }
    m_vectorsX->alcMaterial();
    m_vectorsX->setHeadWidth(0.03f);
    m_vectorsX->setShowZero(FALSE);
    addChild(m_vectorsX);
  }
  if ( ! m_vectorsY ) {
    m_vectorsY = new vfrVectors();
    if ( ! m_vectorsY ) {
      ErrMsg(MsgERR, getMethodType() + string("[") + getName()
             + string("]: memory allocation failed"));
      return false;
    }
    m_vectorsY->alcMaterial();
    m_vectorsY->setHeadWidth(0.03f);
    m_vectorsY->setShowZero(FALSE);
    addChild(m_vectorsY);
  }
  if ( ! m_vectorsZ ) {
    m_vectorsZ = new vfrVectors();
    if ( ! m_vectorsZ ) {
      ErrMsg(MsgERR, getMethodType() + string("[") + getName()
             + string("]: memory allocation failed"));
      return false;
    }
    m_vectorsZ->alcMaterial();
    m_vectorsZ->setHeadWidth(0.03f);
    m_vectorsZ->setShowZero(FALSE);
    addChild(m_vectorsZ);
  }
  m_vectorsX->getPrivateMaterial()->setRenderMode(RT_NONE);
  m_vectorsY->getPrivateMaterial()->setRenderMode(RT_NONE);
  m_vectorsZ->getPrivateMaterial()->setRenderMode(RT_NONE);
  if ( ! m_show ) {
    return true;
  }

  // check selected data
  size_t dlen = pData->getDataLen();
  if ( ! isValidVecData() ) return true;

  // check slice size
  Vec3<size_t> dims = pData->getDims();
  if ( ! p_osr ) return true;
  int sliceAxis = p_osr->getSliceAxis() % 3;
  int slicePlane = p_osr->getSlicePlane();
  if ( sliceAxis < 0 ) sliceAxis = 0;
  if ( slicePlane < 0 ) slicePlane = 0;
  Point2 sliceSize = p_osr->getSliceSize();
  size_t sliceSz = sliceSize.x * sliceSize.y;
  if ( sliceSz < 1 ) return true;

  // get Sv data
  const float* gptr = pData->getCoord(m_requestedStp);
  if ( ! gptr ) {
    return false;
  }
  const float* vptr = pData->getData(m_requestedStp);
  if ( ! vptr ) {
    return false;
  }
  const unsigned char* maskptr = pData->getMask();

  // alloc vector datas
  if ( ! m_vectorsX->alcVerts(sliceSz) || ! m_vectorsX->alcNormals(sliceSz) ||
       ! m_vectorsY->alcVerts(sliceSz) || ! m_vectorsY->alcNormals(sliceSz) ||
       ! m_vectorsZ->alcVerts(sliceSz) || ! m_vectorsZ->alcNormals(sliceSz) ) {
    return false;
  }
  vector3* vlX = m_vectorsX->getVerts();
  vector3* vlY = m_vectorsY->getVerts();
  vector3* vlZ = m_vectorsZ->getVerts();
  vector3* nlX = m_vectorsX->getNormals();
  vector3* nlY = m_vectorsY->getNormals();
  vector3* nlZ = m_vectorsZ->getNormals();
  vector4* clX = m_vectorsX->getColors();
  vector4* clY = m_vectorsY->getColors();
  vector4* clZ = m_vectorsZ->getColors();
  if ( m_selectedData == DATA_Veclen ||
       (m_selectedData > 0 && m_selectedData <= dlen) ) {
    if ( ! m_vectorsX->alcColors(sliceSz) ||
	 ! m_vectorsY->alcColors(sliceSz) ||
	 ! m_vectorsZ->alcColors(sliceSz) ) {
      return false;
    }
    m_vectorsX->setColorMode(AT_PER_VERTEX);
    m_vectorsY->setColorMode(AT_PER_VERTEX);
    m_vectorsZ->setColorMode(AT_PER_VERTEX);
    clX = m_vectorsX->getColors();
    clY = m_vectorsY->getColors();
    clZ = m_vectorsZ->getColors();
  }
  else { // no color
    memcpy(clX[0], m_colour, sizeof(vector3));
    memcpy(clY[0], m_colour, sizeof(vector3));
    memcpy(clZ[0], m_colour, sizeof(vector3));
    clX[0][3] = clY[0][3] = clZ[0][3] = 1.f;
    m_vectorsX->setColorMode(AT_WHOLE);
    m_vectorsY->setColorMode(AT_WHOLE);
    m_vectorsZ->setColorMode(AT_WHOLE);
  }

  // setup vectors
  Vec3<float> p0, p_i, p_j, p_k, p_w;
  Vec3<float> vv;
  register int ii, i, j, k, idx0, idx, c;
  ii = 0;
  switch ( sliceAxis % 3 ) {
  case 0: // YZ
    i = slicePlane;
    for ( k = 0; k < sliceSize.y; k++ )
      for ( j = 0; j < sliceSize.x; j++ ) {
	idx0 = IDX1(dims, i, j, k);
	idx = idx0 * dlen;
	vv.m_v[0] = vptr[idx + m_vecDataIdx.m_v[0]];
	vv.m_v[1] = vptr[idx + m_vecDataIdx.m_v[1]];
	vv.m_v[2] = vptr[idx + m_vecDataIdx.m_v[2]];
	if ( maskptr && maskptr[idx0] ) {
	  nlX[ii][0] = nlY[ii][1] = nlZ[ii][2] = 0.f;
	} else {
	  nlX[ii][0] = vv.m_v[0];
	  nlY[ii][1] = vv.m_v[1];
	  nlZ[ii][2] = vv.m_v[2];
	}
	nlX[ii][1] = 0.f; nlX[ii][2] = 0.f;
	nlY[ii][0] = 0.f; nlY[ii][2] = 0.f;
	nlZ[ii][0] = 0.f; nlZ[ii][1] = 0.f;

	p0 = &gptr[IDX3(dims, i, j, k)];
	if ( m_vecLayoutType == Staggered1 ) {
	  if ( i == 0 ) {
	    if ( dims[0] == 1 ) p_i = p0;
	    else {
	      p_w = &gptr[IDX3(dims, i+1, j, k)];
	      p_i = p0 - (p_w - p0);
	    }
	  } else
	    p_i = &gptr[IDX3(dims, i-1, j, k)];
	  if ( j == 0 ) {
	    if ( dims[1] == 1 ) p_j = p0;
	    else {
	      p_w = &gptr[IDX3(dims, i, j+1, k)];
	      p_j = p0 - (p_w - p0);
	    }
	  } else
	    p_j = &gptr[IDX3(dims, i, j-1, k)];
	  if ( k == 0 ) {
	    if ( dims[2] == 1 ) p_k = p0;
	    else {
	      p_w = &gptr[IDX3(dims, i, j, k+1)];
	      p_k = p0 - (p_w - p0);
	    }
	  } else
	    p_k = &gptr[IDX3(dims, i, j, k-1)];
	  memcpy(vlX[ii], ((p0 + p_i) * 0.5f).m_v, sizeof(vector3));
	  memcpy(vlY[ii], ((p0 + p_j) * 0.5f).m_v, sizeof(vector3));
	  memcpy(vlZ[ii], ((p0 + p_k) * 0.5f).m_v, sizeof(vector3));
	}
	else if ( m_vecLayoutType == Staggered2 ) {
	  if ( i == dims[0]-1 ) {
	    if ( dims[0] == 1 ) p_i = p0;
	    else {
	      p_w = &gptr[IDX3(dims, i-1, j, k)];
	      p_i = p0 + (p0 - p_w);
	    }
	  } else
	    p_i = &gptr[IDX3(dims, i+1, j, k)];
	  if ( j == dims[1]-1 ) {
	    if ( dims[1] == 1 ) p_j = p0;
	    else {
	      p_w = &gptr[IDX3(dims, i, j-1, k)];
	      p_j = p0 + (p0 - p_w);
	    }
	  } else
	    p_j = &gptr[IDX3(dims, i, j+1, k)];
	  if ( k == dims[2]-1 ) {
	    if ( dims[2] == 1 ) p_k = p0;
	    else {
	      p_w = &gptr[IDX3(dims, i, j, k-1)];
	      p_k = p0 + (p0 - p_w);
	    }
	  } else
	    p_k = &gptr[IDX3(dims, i, j, k+1)];
	  memcpy(vlX[ii], ((p0 + p_i) * 0.5f).m_v, sizeof(vector3));
	  memcpy(vlY[ii], ((p0 + p_j) * 0.5f).m_v, sizeof(vector3));
	  memcpy(vlZ[ii], ((p0 + p_k) * 0.5f).m_v, sizeof(vector3));
	}
	else if ( m_vecLayoutType == Colocated ) {
	  if ( i == dims[0]-1 ) {
	    if ( dims[0] == 1 ) p_i = p0;
	    else {
	      p_w = &gptr[IDX3(dims, i-1, j, k)];
	      p_i = p0 + (p0 - p_w);
	    }
	  } else
	    p_i = &gptr[IDX3(dims, i+1, j, k)];
	  if ( j == dims[1]-1 ) {
	    if ( dims[1] == 1 ) p_j = p0;
	    else {
	      p_w = &gptr[IDX3(dims, i, j-1, k)];
	      p_j = p0 + (p0 - p_w);
	    }
	  } else
	    p_j = &gptr[IDX3(dims, i, j+1, k)];
	  if ( k == dims[2]-1 ) {
	    if ( dims[2] == 1 ) p_k = p0;
	    else {
	      p_w = &gptr[IDX3(dims, i, j, k-1)];
	      p_k = p0 + (p0 - p_w);
	    }
	  } else
	    p_k = &gptr[IDX3(dims, i, j, k+1)];
	  p_w[0] = p_i[0]; p_w[1] = p_j[1]; p_w[2] = p_k[2];
	  memcpy(vlX[ii], ((p0 + p_w) * 0.5f).m_v, sizeof(vector3));
	  memcpy(vlY[ii], ((p0 + p_w) * 0.5f).m_v, sizeof(vector3));
	  memcpy(vlZ[ii], ((p0 + p_w) * 0.5f).m_v, sizeof(vector3));
	}
	else { // Regular
	  memcpy(vlX[ii], p0.m_v, sizeof(vector3));
	  memcpy(vlY[ii], p0.m_v, sizeof(vector3));
	  memcpy(vlZ[ii], p0.m_v, sizeof(vector3));
	}

	if ( m_selectedData == DATA_Veclen ) {
#if 0
	  c = m_lut.getValIdx(vv.m_v[0]);
	  memcpy(clX[ii], &m_lut.lutEntry[c*4], sizeof(vector3));
	  c = m_lut.getValIdx(vv.m_v[1]);
	  memcpy(clY[ii], &m_lut.lutEntry[c*4], sizeof(vector3));
	  c = m_lut.getValIdx(vv.m_v[2]);
	  memcpy(clZ[ii], &m_lut.lutEntry[c*4], sizeof(vector3));
#else
	  c = m_lut.getValIdx(vv.Length());
	  memcpy(clX[ii], &m_lut.lutEntry[c*4], sizeof(vector3));
	  memcpy(clY[ii], &m_lut.lutEntry[c*4], sizeof(vector3));
	  memcpy(clZ[ii], &m_lut.lutEntry[c*4], sizeof(vector3));
#endif
	  clX[ii][3] = clY[ii][3] = clZ[ii][3] = 1.f;
	}
	else if ( m_selectedData > 0 && m_selectedData <= dlen ) {
	  c = m_lut.getValIdx(vptr[idx  + m_selectedData -1]);
	  memcpy(clX[ii], &m_lut.lutEntry[c*4], sizeof(vector3));
	  memcpy(clY[ii], &m_lut.lutEntry[c*4], sizeof(vector3));
	  memcpy(clZ[ii], &m_lut.lutEntry[c*4], sizeof(vector3));
	  clX[ii][3] = clY[ii][3] = clZ[ii][3] = 1.f;
	}

	ii ++;
      } // end of for(j)
    break;
  case 1: // ZX
    j = slicePlane;
    for ( i = 0; i < sliceSize.y; i++ )
      for ( k = 0; k < sliceSize.x; k++ ) {
	idx0 = IDX1(dims, i, j, k);
	idx = idx0 * dlen;
	vv.m_v[0] = vptr[idx + m_vecDataIdx.m_v[0]];
	vv.m_v[1] = vptr[idx + m_vecDataIdx.m_v[1]];
	vv.m_v[2] = vptr[idx + m_vecDataIdx.m_v[2]];
	if ( maskptr && maskptr[idx0] ) {
	  nlX[ii][0] = nlY[ii][1] = nlZ[ii][2] = 0.f;
	} else {
	  nlX[ii][0] = vv.m_v[0];
	  nlY[ii][1] = vv.m_v[1];
	  nlZ[ii][2] = vv.m_v[2];
	}
	nlX[ii][1] = 0.f; nlX[ii][2] = 0.f;
	nlY[ii][0] = 0.f; nlY[ii][2] = 0.f;
	nlZ[ii][0] = 0.f; nlZ[ii][1] = 0.f;

	p0 = &gptr[IDX3(dims, i, j, k)];
	if ( m_vecLayoutType == Staggered1 ) {
	  if ( i == 0 ) {
	    if ( dims[0] == 1 ) p_i = p0;
	    else {
	      p_w = &gptr[IDX3(dims, i+1, j, k)];
	      p_i = p0 - (p_w - p0);
	    }
	  } else
	    p_i = &gptr[IDX3(dims, i-1, j, k)];
	  if ( j == 0 ) {
	    if ( dims[1] == 1 ) p_j = p0;
	    else {
	      p_w = &gptr[IDX3(dims, i, j+1, k)];
	      p_j = p0 - (p_w - p0);
	    }
	  } else
	    p_j = &gptr[IDX3(dims, i, j-1, k)];
	  if ( k == 0 ) {
	    if ( dims[2] == 1 ) p_k = p0;
	    else {
	      p_w = &gptr[IDX3(dims, i, j, k+1)];
	      p_k = p0 - (p_w - p0);
	    }
	  } else
	    p_k = &gptr[IDX3(dims, i, j, k-1)];
	  memcpy(vlX[ii], ((p0 + p_i) * 0.5f).m_v, sizeof(vector3));
	  memcpy(vlY[ii], ((p0 + p_j) * 0.5f).m_v, sizeof(vector3));
	  memcpy(vlZ[ii], ((p0 + p_k) * 0.5f).m_v, sizeof(vector3));
	}
	else if ( m_vecLayoutType == Staggered2 ) {
	  if ( i == dims[0]-1 ) {
	    if ( dims[0] == 1 ) p_i = p0;
	    else {
	      p_w = &gptr[IDX3(dims, i-1, j, k)];
	      p_i = p0 + (p0 - p_w);
	    }
	  } else
	    p_i = &gptr[IDX3(dims, i+1, j, k)];
	  if ( j == dims[1]-1 ) {
	    if ( dims[1] == 1 ) p_j = p0;
	    else {
	      p_w = &gptr[IDX3(dims, i, j-1, k)];
	      p_j = p0 + (p0 - p_w);
	    }
	  } else
	    p_j = &gptr[IDX3(dims, i, j+1, k)];
	  if ( k == dims[2]-1 ) {
	    if ( dims[2] == 1 ) p_k = p0;
	    else {
	      p_w = &gptr[IDX3(dims, i, j, k-1)];
	      p_k = p0 + (p0 - p_w);
	    }
	  } else
	    p_k = &gptr[IDX3(dims, i, j, k+1)];
	  memcpy(vlX[ii], ((p0 + p_i) * 0.5f).m_v, sizeof(vector3));
	  memcpy(vlY[ii], ((p0 + p_j) * 0.5f).m_v, sizeof(vector3));
	  memcpy(vlZ[ii], ((p0 + p_k) * 0.5f).m_v, sizeof(vector3));
	}
	else if ( m_vecLayoutType == Colocated ) {
	  if ( i == dims[0]-1 ) {
	    if ( dims[0] == 1 ) p_i = p0;
	    else {
	      p_w = &gptr[IDX3(dims, i-1, j, k)];
	      p_i = p0 + (p0 - p_w);
	    }
	  } else
	    p_i = &gptr[IDX3(dims, i+1, j, k)];
	  if ( j == dims[1]-1 ) {
	    if ( dims[1] == 1 ) p_j = p0;
	    else {
	      p_w = &gptr[IDX3(dims, i, j-1, k)];
	      p_j = p0 + (p0 - p_w);
	    }
	  } else
	    p_j = &gptr[IDX3(dims, i, j+1, k)];
	  if ( k == dims[2]-1 ) {
	    if ( dims[2] == 1 ) p_k = p0;
	    else {
	      p_w = &gptr[IDX3(dims, i, j, k-1)];
	      p_k = p0 + (p0 - p_w);
	    }
	  } else
	    p_k = &gptr[IDX3(dims, i, j, k+1)];
	  p_w[0] = p_i[0]; p_w[1] = p_j[1]; p_w[2] = p_k[2];
	  memcpy(vlX[ii], ((p0 + p_w) * 0.5f).m_v, sizeof(vector3));
	  memcpy(vlY[ii], ((p0 + p_w) * 0.5f).m_v, sizeof(vector3));
	  memcpy(vlZ[ii], ((p0 + p_w) * 0.5f).m_v, sizeof(vector3));
	}
	else { // Regular
	  memcpy(vlX[ii], p0.m_v, sizeof(vector3));
	  memcpy(vlY[ii], p0.m_v, sizeof(vector3));
	  memcpy(vlZ[ii], p0.m_v, sizeof(vector3));
	}

	if ( m_selectedData == DATA_Veclen ) {
#if 0
	  c = m_lut.getValIdx(vv.m_v[0]);
	  memcpy(clX[ii], &m_lut.lutEntry[c*4], sizeof(vector3));
	  c = m_lut.getValIdx(vv.m_v[1]);
	  memcpy(clY[ii], &m_lut.lutEntry[c*4], sizeof(vector3));
	  c = m_lut.getValIdx(vv.m_v[2]);
	  memcpy(clZ[ii], &m_lut.lutEntry[c*4], sizeof(vector3));
#else
	  c = m_lut.getValIdx(vv.Length());
	  memcpy(clX[ii], &m_lut.lutEntry[c*4], sizeof(vector3));
	  memcpy(clY[ii], &m_lut.lutEntry[c*4], sizeof(vector3));
	  memcpy(clZ[ii], &m_lut.lutEntry[c*4], sizeof(vector3));
#endif
	  clX[ii][3] = clY[ii][3] = clZ[ii][3] = 1.f;
	}
	else if ( m_selectedData > 0 && m_selectedData <= dlen ) {
	  c = m_lut.getValIdx(vptr[idx  + m_selectedData -1]);
	  memcpy(clX[ii], &m_lut.lutEntry[c*4], sizeof(vector3));
	  memcpy(clY[ii], &m_lut.lutEntry[c*4], sizeof(vector3));
	  memcpy(clZ[ii], &m_lut.lutEntry[c*4], sizeof(vector3));
	  clX[ii][3] = clY[ii][3] = clZ[ii][3] = 1.f;
	}

	ii ++;
      } // end of for(k)
    break;
  case 2: // XY
    k = slicePlane;
    for ( j = 0; j < sliceSize.y; j++ )
      for ( i = 0; i < sliceSize.x; i++ ) {
	idx0 = IDX1(dims, i, j, k);
	idx = idx0 * dlen;
	vv.m_v[0] = vptr[idx + m_vecDataIdx.m_v[0]];
	vv.m_v[1] = vptr[idx + m_vecDataIdx.m_v[1]];
	vv.m_v[2] = vptr[idx + m_vecDataIdx.m_v[2]];
	if ( maskptr && maskptr[idx0] ) {
	  nlX[ii][0] = nlY[ii][1] = nlZ[ii][2] = 0.f;
	} else {
	  nlX[ii][0] = vv.m_v[0];
	  nlY[ii][1] = vv.m_v[1];
	  nlZ[ii][2] = vv.m_v[2];
	}
	nlX[ii][1] = 0.f; nlX[ii][2] = 0.f;
	nlY[ii][0] = 0.f; nlY[ii][2] = 0.f;
	nlZ[ii][0] = 0.f; nlZ[ii][1] = 0.f;

	p0 = &gptr[IDX3(dims, i, j, k)];
	if ( m_vecLayoutType == Staggered1 ) {
	  if ( i == 0 ) {
	    if ( dims[0] == 1 ) p_i = p0;
	    else {
	      p_w = &gptr[IDX3(dims, i+1, j, k)];
	      p_i = p0 - (p_w - p0);
	    }
	  } else
	    p_i = &gptr[IDX3(dims, i-1, j, k)];
	  if ( j == 0 ) {
	    if ( dims[1] == 1 ) p_j = p0;
	    else {
	      p_w = &gptr[IDX3(dims, i, j+1, k)];
	      p_j = p0 - (p_w - p0);
	    }
	  } else
	    p_j = &gptr[IDX3(dims, i, j-1, k)];
	  if ( k == 0 ) {
	    if ( dims[2] == 1 ) p_k = p0;
	    else {
	      p_w = &gptr[IDX3(dims, i, j, k+1)];
	      p_k = p0 - (p_w - p0);
	    }
	  } else
	    p_k = &gptr[IDX3(dims, i, j, k-1)];
	  memcpy(vlX[ii], ((p0 + p_i) * 0.5f).m_v, sizeof(vector3));
	  memcpy(vlY[ii], ((p0 + p_j) * 0.5f).m_v, sizeof(vector3));
	  memcpy(vlZ[ii], ((p0 + p_k) * 0.5f).m_v, sizeof(vector3));
	}
	else if ( m_vecLayoutType == Staggered2 ) {
	  if ( i == dims[0]-1 ) {
	    if ( dims[0] == 1 ) p_i = p0;
	    else {
	      p_w = &gptr[IDX3(dims, i-1, j, k)];
	      p_i = p0 + (p0 - p_w);
	    }
	  } else
	    p_i = &gptr[IDX3(dims, i+1, j, k)];
	  if ( j == dims[1]-1 ) {
	    if ( dims[1] == 1 ) p_j = p0;
	    else {
	      p_w = &gptr[IDX3(dims, i, j-1, k)];
	      p_j = p0 + (p0 - p_w);
	    }
	  } else
	    p_j = &gptr[IDX3(dims, i, j+1, k)];
	  if ( k == dims[2]-1 ) {
	    if ( dims[2] == 1 ) p_k = p0;
	    else {
	      p_w = &gptr[IDX3(dims, i, j, k-1)];
	      p_k = p0 + (p0 - p_w);
	    }
	  } else
	    p_k = &gptr[IDX3(dims, i, j, k+1)];
	  memcpy(vlX[ii], ((p0 + p_i) * 0.5f).m_v, sizeof(vector3));
	  memcpy(vlY[ii], ((p0 + p_j) * 0.5f).m_v, sizeof(vector3));
	  memcpy(vlZ[ii], ((p0 + p_k) * 0.5f).m_v, sizeof(vector3));
	}
	else if ( m_vecLayoutType == Colocated ) {
	  if ( i == dims[0]-1 ) {
	    if ( dims[0] == 1 ) p_i = p0;
	    else {
	      p_w = &gptr[IDX3(dims, i-1, j, k)];
	      p_i = p0 + (p0 - p_w);
	    }
	  } else
	    p_i = &gptr[IDX3(dims, i+1, j, k)];
	  if ( j == dims[1]-1 ) {
	    if ( dims[1] == 1 ) p_j = p0;
	    else {
	      p_w = &gptr[IDX3(dims, i, j-1, k)];
	      p_j = p0 + (p0 - p_w);
	    }
	  } else
	    p_j = &gptr[IDX3(dims, i, j+1, k)];
	  if ( k == dims[2]-1 ) {
	    if ( dims[2] == 1 ) p_k = p0;
	    else {
	      p_w = &gptr[IDX3(dims, i, j, k-1)];
	      p_k = p0 + (p0 - p_w);
	    }
	  } else
	    p_k = &gptr[IDX3(dims, i, j, k+1)];
	  p_w[0] = p_i[0]; p_w[1] = p_j[1]; p_w[2] = p_k[2];
	  memcpy(vlX[ii], ((p0 + p_w) * 0.5f).m_v, sizeof(vector3));
	  memcpy(vlY[ii], ((p0 + p_w) * 0.5f).m_v, sizeof(vector3));
	  memcpy(vlZ[ii], ((p0 + p_w) * 0.5f).m_v, sizeof(vector3));
	}
	else { // Regular
	  memcpy(vlX[ii], p0.m_v, sizeof(vector3));
	  memcpy(vlY[ii], p0.m_v, sizeof(vector3));
	  memcpy(vlZ[ii], p0.m_v, sizeof(vector3));
	}

	if ( m_selectedData == DATA_Veclen ) {
#if 0
	  c = m_lut.getValIdx(vv.m_v[0]);
	  memcpy(clX[ii], &m_lut.lutEntry[c*4], sizeof(vector3));
	  c = m_lut.getValIdx(vv.m_v[1]);
	  memcpy(clY[ii], &m_lut.lutEntry[c*4], sizeof(vector3));
	  c = m_lut.getValIdx(vv.m_v[2]);
	  memcpy(clZ[ii], &m_lut.lutEntry[c*4], sizeof(vector3));
#else
	  c = m_lut.getValIdx(vv.Length());
	  memcpy(clX[ii], &m_lut.lutEntry[c*4], sizeof(vector3));
	  memcpy(clY[ii], &m_lut.lutEntry[c*4], sizeof(vector3));
	  memcpy(clZ[ii], &m_lut.lutEntry[c*4], sizeof(vector3));
#endif
	  clX[ii][3] = clY[ii][3] = clZ[ii][3] = 1.f;
	}
	else if ( m_selectedData > 0 && m_selectedData <= dlen ) {
	  c = m_lut.getValIdx(vptr[idx  + m_selectedData -1]);
	  memcpy(clX[ii], &m_lut.lutEntry[c*4], sizeof(vector3));
	  memcpy(clY[ii], &m_lut.lutEntry[c*4], sizeof(vector3));
	  memcpy(clZ[ii], &m_lut.lutEntry[c*4], sizeof(vector3));
	  clX[ii][3] = clY[ii][3] = clZ[ii][3] = 1.f;
	}

	ii ++;
      } // end of for(i)
    break;
  } // end of switch(sliceAxis)
  m_vectorsX->generateBbox();
  m_vectorsY->generateBbox();
  m_vectorsZ->generateBbox();

  // vectors' attr
  m_vectorsX->setTransparency(m_antiAlias);
  m_vectorsX->setScaleFac(m_vecScale);
  m_vectorsX->setHeadMode(m_vecHead);
  m_vectorsX->getPrivateMaterial()->setLineWidth(m_lineWidth);
  m_vectorsY->setTransparency(m_antiAlias);
  m_vectorsY->setScaleFac(m_vecScale);
  m_vectorsY->setHeadMode(m_vecHead);
  m_vectorsY->getPrivateMaterial()->setLineWidth(m_lineWidth);
  m_vectorsZ->setTransparency(m_antiAlias);
  m_vectorsZ->setScaleFac(m_vecScale);
  m_vectorsZ->setHeadMode(m_vecHead);
  m_vectorsZ->getPrivateMaterial()->setLineWidth(m_lineWidth);

  // ok
  if ( m_showVecX )
    m_vectorsX->getPrivateMaterial()->setRenderMode(m_showType);
  if ( m_showVecY )
    m_vectorsY->getPrivateMaterial()->setRenderMode(m_showType);
  if ( m_showVecZ )
    m_vectorsZ->getPrivateMaterial()->setRenderMode(m_showType);
  setBaseColor(m_colour);
  setHilight(m_hilight);
  m_updatedStp = m_requestedStp;
  return true;
}
#undef IDX1
#undef IDX3


/* serialize : from vsnIoObject */

bool vsnMethod_Sv_staggeredVector::parseXML(xmlNodePtr xnp) {
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

      if ( xsN == string("ortho_slicer") ) {
        if ( ! setOrthoSlicer(dynamic_cast<vsnData_Sv*>(p_refData),xsV) ) {
          ErrMsg(MsgERR, msgHdr + string("failed to set orthoSlicer: ") + xsV);
          goto _NEXT_XML_NODE;
        }
      } // end of "ortho_slicer"
      else if ( xsN == string("vec_layout") ) {
	VecLayoutType vlt;
	if ( xsV  == string("staggered1") ) vlt = Staggered1;
	else if ( xsV  == string("staggered2") ) vlt = Staggered2;
	else if ( xsV  == string("colocated") ) vlt = Colocated;
	else if ( xsV  == string("regular") ) vlt = Regular;
	else {
          ErrMsg(MsgERR, msgHdr + string("invalid value in param vec_layout"));
          goto _NEXT_XML_NODE;
        }
        if ( ! setVecLayout(vlt) ) {
          ErrMsg(MsgERR, msgHdr + string("can't set param vec_layout"));
          goto _NEXT_XML_NODE;
        }
      } // end of "vec_layout"
      else if ( xsN == string("vec_scale") ) {
        float vsc = (float)atof(xsV.c_str());
        if ( ! setVecScale(vsc) ) {
          ErrMsg(MsgERR, msgHdr + string("invalid value in param vec_scale"));
          goto _NEXT_XML_NODE;
        }
      } // end of "vec_scale"
      else if ( xsN == string("vec_head") ) {
        bool vhm;
        if ( xsV == string("yes") ) vhm = true;
        else if ( xsV == string("no") ) vhm = false;
        else {
          ErrMsg(MsgERR, msgHdr + string("invalid value in param vec_head"));
          goto _NEXT_XML_NODE;
        }
        if ( ! setVecHeadMode(vhm) ) {
          ErrMsg(MsgERR, msgHdr + string("can't update vec_head mode"));
          goto _NEXT_XML_NODE;
        }
      } // end of "vec_head"
      else if ( xsN == string("show_x") ) {
        bool sxm, sym, szm;
	getShowVecComp(sxm, sym, szm);
        if ( xsV == string("yes") ) sxm = true;
        else if ( xsV == string("no") ) sxm = false;
        else {
          ErrMsg(MsgERR, msgHdr + string("invalid value in param show_x"));
          goto _NEXT_XML_NODE;
        }
        if ( ! setShowVecComp(sxm, sym, szm) ) {
          ErrMsg(MsgERR, msgHdr + string("can't set param show_x"));
          goto _NEXT_XML_NODE;
        }
      } // end of "show_x"
      else if ( xsN == string("show_y") ) {
        bool sxm, sym, szm;
	getShowVecComp(sxm, sym, szm);
        if ( xsV == string("yes") ) sym = true;
        else if ( xsV == string("no") ) sym = false;
        else {
          ErrMsg(MsgERR, msgHdr + string("invalid value in param show_y"));
          goto _NEXT_XML_NODE;
        }
        if ( ! setShowVecComp(sxm, sym, szm) ) {
          ErrMsg(MsgERR, msgHdr + string("can't set param show_y"));
          goto _NEXT_XML_NODE;
        }
      } // end of "show_y"
      else if ( xsN == string("show_z") ) {
        bool sxm, sym, szm;
	getShowVecComp(sxm, sym, szm);
        if ( xsV == string("yes") ) szm = true;
        else if ( xsV == string("no") ) szm = false;
        else {
          ErrMsg(MsgERR, msgHdr + string("invalid value in param show_z"));
          goto _NEXT_XML_NODE;
        }
        if ( ! setShowVecComp(sxm, sym, szm) ) {
          ErrMsg(MsgERR, msgHdr + string("can't set param show_z"));
          goto _NEXT_XML_NODE;
        }
      } // end of "show_z"
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
      else if ( xsN == string("upd_minmax") ) {
        bool mmupd;
        if ( xsV == string("yes") ) mmupd = true;
        else if ( xsV == string("no") ) mmupd = false;
        else {
          ErrMsg(MsgERR, msgHdr + string("invalid value in param upd_minmax"));
          goto _NEXT_XML_NODE;
        }
        if ( ! setUpdateMinMaxMode(mmupd) ) {
          ErrMsg(MsgERR, msgHdr + string("failed to set upd_minmax"));
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

bool vsnMethod_Sv_staggeredVector::outputXML(std::ostream& os, const size_t ts)
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
  }

  // ortho_slicer
  if ( p_osr ) {
    if ( ! vsnRef_Sv_orthoSlicer::exportXMLNode(os, ts+2) ) {
      ErrMsg(MsgWARN, msgHdr
             + string("ortho_slice has set, but the slicer has no name,\n")
             + string("so don't output ortho_slicer param node"));
    }
  }

  // vec_layout
  if ( m_vecLayoutType != Staggered2 ) {
    os << idts_2 << "<param name=\"vec_layout\" value=\"";
    switch ( m_vecLayoutType ) {
    case Staggered1: os << "staggered1"; break;
    case Colocated: os << "colocated"; break;
    case Regular: os << "regular"; break;
    default: os << "staggered2";
    } // end of switch
    os << "\" />" << endl;
  }

  // vec_scale
  if ( m_vecScale != 1.f ) {
    os << idts_2 << "<param name=\"vec_scale\" value=\""
       << m_vecScale << "\" />" << endl;
  }

  // vec_head
  if ( ! m_vecHead ) {
    os << idts_2 << "<param name=\"vec_head\" value=\"no\" />" << endl;
  }

  // show_x/y/z
  if ( ! m_showVecX ) {
    os << idts_2 << "<param name=\"show_x\" value=\"no\" />" << endl;
  }
  if ( ! m_showVecY ) {
    os << idts_2 << "<param name=\"show_y\" value=\"no\" />" << endl;
  }
  if ( ! m_showVecZ ) {
    os << idts_2 << "<param name=\"show_z\" value=\"no\" />" << endl;
  }

  // vec_idx
  if ( m_vecDataIdx[0] != 0 || m_vecDataIdx[1] != 1 || m_vecDataIdx[2] != 2 ) {
    os << idts_2 << "<param name=\"vec_idx\" value=\"";
    os << m_vecDataIdx[0] << "/" << m_vecDataIdx[1] << "/" << m_vecDataIdx[2];
    os << "\" />" << endl;
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

bool vsnMethod_Sv_staggeredVector::commandXML(xmlNodePtr xnp) {
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

  if ( nameStr == "set_ortho_slicer" ) {
    if ( valueStr == VFR_NONAME ) {
      ErrMsg(MsgERR, msgHdr + "command set_ortho_slicer: " +
             "can't set orthoSlicer with no name");
      return false;
    }
    if ( ! setOrthoSlicer(dynamic_cast<vsnData_Sv*>(p_refData),
                          valueStr) ) {
      ErrMsg(MsgERR, msgHdr +
             string("command set_ortho_slicer: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_ortho_slicer"
  else if ( nameStr == "set_vec_layout" ) {
    VecLayoutType vlt;
    if ( valueStr  == string("staggered1") ) vlt = Staggered1;
    else if ( valueStr  == string("staggered2") ) vlt = Staggered2;
    else if ( valueStr  == string("colocated") ) vlt = Colocated;
    else if ( valueStr  == string("regular") ) vlt = Regular;
    else {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_vec_layout: invalid value: ") + valueStr);
      return false;
    }
    if ( ! setVecLayout(vlt) ) {
      ErrMsg(MsgERR, msgHdr +
             string("command set_vec_scale: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_vec_layout"
  else if ( nameStr == "set_vec_scale" ) {
    float vsc = (float)atof(valueStr.c_str());
    if ( ! setVecScale(vsc) ) {
      ErrMsg(MsgERR, msgHdr +
             string("command set_vec_scale: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_vec_scale"
  else if ( nameStr == "set_vec_head" ) {
    bool vhm;
    if ( valueStr == string("yes") ) vhm = true;
    else if ( valueStr == string("no") ) vhm = false;
    else {
      ErrMsg(MsgERR, msgHdr +
             string("command set_vec_head: invalid value: ") + valueStr);
      return false;
    }
    if ( ! setVecHeadMode(vhm) ) {
      ErrMsg(MsgERR, msgHdr +
             string("command set_vec_head: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_vec_head"
  else if ( nameStr == "set_show_x" ) {
    bool sxm, sym, szm;
    getShowVecComp(sxm, sym, szm);
    if ( valueStr == string("yes") ) sxm = true;
    else if ( valueStr == string("no") ) sxm = false;
    else {
      ErrMsg(MsgERR, msgHdr +
             string("command set_show_x: invalid value: ") + valueStr);
      return false;
    }
    if ( ! setShowVecComp(sxm, sym, szm) ) {
      ErrMsg(MsgERR, msgHdr +
             string("command set_show_x: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_show_x"
  else if ( nameStr == "set_show_y" ) {
    bool sxm, sym, szm;
    getShowVecComp(sxm, sym, szm);
    if ( valueStr == string("yes") ) sym = true;
    else if ( valueStr == string("no") ) sym = false;
    else {
      ErrMsg(MsgERR, msgHdr +
             string("command set_show_y: invalid value: ") + valueStr);
      return false;
    }
    if ( ! setShowVecComp(sxm, sym, szm) ) {
      ErrMsg(MsgERR, msgHdr +
             string("command set_show_y: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_show_y"
  else if ( nameStr == "set_show_z" ) {
    bool sxm, sym, szm;
    getShowVecComp(sxm, sym, szm);
    if ( valueStr == string("yes") ) szm = true;
    else if ( valueStr == string("no") ) szm = false;
    else {
      ErrMsg(MsgERR, msgHdr +
             string("command set_show_z: invalid value: ") + valueStr);
      return false;
    }
    if ( ! setShowVecComp(sxm, sym, szm) ) {
      ErrMsg(MsgERR, msgHdr +
             string("command set_show_z: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_show_z"
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


/* from vsnRef_Sv_orthoSlicer */

void vsnMethod_Sv_staggeredVector::noticeUpdate() {
  update();
}
