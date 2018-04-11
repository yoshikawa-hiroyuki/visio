//
// vsnMethod_Sv_orthoVector
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

#include "vsnMethod_Sv_orthoVector.h"
#include "vsnOctTree.h" // for decomp vecIdx
#include "vsnUiView.h"
#include "vsnError.h"

using namespace std;
using namespace CES;
using namespace VSN;


//----------------------------------------------------------------
// class vsnMPP_Sv_orthoVector
//----------------------------------------------------------------

BEGIN_EVENT_TABLE(vsnMPP_Sv_orthoVector, wxPanel)
  EVT_TEXT_ENTER(MPP_Sv_orthoVector_SlicerTxt,
		 vsnMPP_Sv_orthoVector::OnEnterSlicerTxt)
  EVT_BUTTON(MPP_Sv_orthoVector_SetSlicerBtn,
	     vsnMPP_Sv_orthoVector::OnSetSlicerBtn)
  EVT_TEXT_ENTER(MPP_Sv_orthoVector_VecScaleTxt,
		 vsnMPP_Sv_orthoVector::OnVecScaleTxt)
  EVT_CHECKBOX(MPP_Sv_orthoVector_VecHeadChk,
	       vsnMPP_Sv_orthoVector::OnVecHeadChk)
  EVT_CHECKLISTBOX(MPP_Sv_orthoVector_VecDataChkLst,
                   vsnMPP_Sv_orthoVector::OnVecDataChkLst)
  EVT_COMBOBOX(MPP_Sv_orthoVector_SelDataLst,
               vsnMPP_Sv_orthoVector::OnSelDataLst)
  EVT_CHECKBOX(MPP_Sv_orthoVector_UpdMinMaxChk,
	       vsnMPP_Sv_orthoVector::OnUpdMinMaxChk)
  EVT_TEXT_ENTER(MPP_Sv_orthoVector_LineWidthTxt,
		 vsnMPP_Sv_orthoVector::OnLineWidthTxt)
  EVT_CHECKBOX(MPP_Sv_orthoVector_AntiAliasChk,
	       vsnMPP_Sv_orthoVector::OnAntiAliasChk)
END_EVENT_TABLE()


/* constructors / destructor */

vsnMPP_Sv_orthoVector::vsnMPP_Sv_orthoVector(wxPanel* parent, vsnMethodObj* pm)
  : vsnMethodPP(parent, pm),
    m_pSlicerTxt(NULL), m_pSlicerLst(NULL), m_pSetSlicerBtn(NULL),
    m_pVecScaleTxt(NULL), m_pVecHeadChk(NULL),
    m_pVecDataChkLst(NULL), m_pSelDataLst(NULL),
    m_pUpdMinMaxChk(NULL), m_pLineWidthTxt(NULL), m_pAntiAliasChk(NULL)
{
  assert(parent);
  vsnMethod_Sv_orthoVector* pmov = dynamic_cast<vsnMethod_Sv_orthoVector*>(pm);
  assert(pmov);

  // setup gfxAct
  gfxAct_KI.setRefOrthoSlicer(pmov);
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


  wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer* sizerH;

  // selected slicer
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND|wxALL, 3);
  sizerH->Add(new wxStaticText(this, -1, wxT("selected")),
	      0, wxALIGN_LEFT|wxALL, 3);
  m_pSlicerTxt = new wxTextCtrl(this, MPP_Sv_orthoVector_SlicerTxt,
				wxT(""), wxDefaultPosition, wxDefaultSize,
				wxTE_READONLY|wxTE_PROCESS_ENTER);
  sizerH->Add(m_pSlicerTxt, 0, wxEXPAND|wxALL, 3);

  // orthoSlicer selector
  m_pSlicerLst = new wxListBox(this, MPP_Sv_orthoVector_SlicerLst);
  topsizer->Add(m_pSlicerLst, 0, wxEXPAND|wxALL, 3);

  m_pSetSlicerBtn = new wxButton(this, MPP_Sv_orthoVector_SetSlicerBtn,
				 wxT("set orthoSlicer"));
  topsizer->Add(m_pSetSlicerBtn, 0, wxALIGN_LEFT|wxALL, 3);

  // vector params
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxALL, 3);
  sizerH->Add(new wxStaticText(this, -1, wxT("vector scale")),
	      0, wxALIGN_LEFT|wxALL, 3);
  m_pVecScaleTxt = new wxTextCtrl(this, MPP_Sv_orthoVector_VecScaleTxt,
				  wxT(""), wxDefaultPosition, wxDefaultSize,
				  wxTE_PROCESS_ENTER);
  sizerH->Add(m_pVecScaleTxt, 0, wxALIGN_LEFT|wxALL, 3);

  m_pVecHeadChk = new wxCheckBox(this, MPP_Sv_orthoVector_VecHeadChk,
				 wxT("arrow head"));
  topsizer->Add(m_pVecHeadChk, 0, wxALL, 3);

  // vector data selector
  topsizer->Add(new wxStaticText(this, -1, wxT("vector components")),
                0, wxALIGN_LEFT|wxALL, 3);
  m_pVecDataChkLst = new wxCheckListBox(this,
                                        MPP_Sv_orthoVector_VecDataChkLst);
  topsizer->Add(m_pVecDataChkLst, 0, wxEXPAND|wxALL, 3);

  // data selector
  topsizer->Add(new wxStaticText(this, -1, wxT("select scalar data")),
                0, wxALIGN_LEFT|wxALL, 3);
  m_pSelDataLst = new wxComboBox(this, MPP_Sv_orthoVector_SelDataLst,
                                 wxT(""), wxDefaultPosition, wxDefaultSize,
                                 0, NULL, wxCB_READONLY);
  topsizer->Add(m_pSelDataLst, 0, wxEXPAND|wxALL, 3);

  // update minmax mode
  m_pUpdMinMaxChk = new wxCheckBox(this, MPP_Sv_orthoVector_UpdMinMaxChk,
				   wxT("update lut minmax"));
  m_pUpdMinMaxChk->SetValue(TRUE);
  topsizer->Add(m_pUpdMinMaxChk, 0, wxALL, 3);

  // line width
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxALL, 3);
  sizerH->Add(new wxStaticText(this, -1, wxT("line width")),
	      0, wxALIGN_LEFT|wxALL, 3);
  m_pLineWidthTxt = new wxTextCtrl(this, MPP_Sv_orthoVector_LineWidthTxt,
				   wxT(""), wxDefaultPosition, wxDefaultSize,
				   wxTE_PROCESS_ENTER);
  sizerH->Add(m_pLineWidthTxt, 0, wxALIGN_LEFT|wxALL, 3);

  // anti-alias
  m_pAntiAliasChk = new wxCheckBox(this, MPP_Sv_orthoVector_AntiAliasChk,
				   wxT("anti-alias line"));
  topsizer->Add(m_pAntiAliasChk, 0, wxALL, 3);

  // post process
  SetSizer(topsizer);
  addTo(parent);
  topsizer->Layout();
  (void)update();
}

vsnMPP_Sv_orthoVector::~vsnMPP_Sv_orthoVector() {
}


/* interface */

bool vsnMPP_Sv_orthoVector::update() {
  if ( ! m_pSlicerTxt || ! m_pSlicerLst || ! m_pSetSlicerBtn ||
       ! m_pVecScaleTxt || ! m_pVecHeadChk ||
       ! m_pVecDataChkLst || ! m_pSelDataLst ||
       ! m_pUpdMinMaxChk || ! m_pLineWidthTxt || ! m_pAntiAliasChk )
    return false;

  vsnMethod_Sv_orthoVector* pm
    = dynamic_cast<vsnMethod_Sv_orthoVector*>(p_method);
  if ( ! pm ) return false;
  vsnData_Sv* pdo = dynamic_cast<vsnData_Sv*>(pm->getRefData());
  if ( ! pdo ) return false;

  // selected slicer
  vsnMethod_Sv_orthoSlicer* osr = pm->getOrthoSlicer();
  if ( osr )
    m_pSlicerTxt->SetValue(vsnApp::ConvSysToWx(osr->getName()));
  else
    m_pSlicerTxt->SetValue(wxT("none"));

  // slicer list
  m_pSlicerLst->Clear();
  deque<vsnMethod_Sv_orthoSlicer*> slicerMtdLst
    = pm->getOrthoSlicerList(pdo);
  deque<vsnMethod_Sv_orthoSlicer*>::iterator it;
  for ( it = slicerMtdLst.begin(); it != slicerMtdLst.end(); it++ ) {
    if ( ! *it ) continue;
    string slicerName = (*it)->getName();
    if ( slicerName.empty() )
      m_pSlicerLst->Append(wxT(VFR_NONAME));
    else
      m_pSlicerLst->Append(vsnApp::ConvSysToWx(slicerName));
  } // end of for(it)

  // vector params
  float vs = pm->getVecScale();
  char txt[64];
  sprintf(txt, "%g", vs);
  m_pVecScaleTxt->SetValue(vsnApp::ConvSysToWx(txt));
  bool vhm = pm->getVecHeadMode();
  m_pVecHeadChk->SetValue(vhm);

  // vector data indices
  register int i;
  int dlen = (int)pdo->getDataLen();
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

void vsnMPP_Sv_orthoVector::settlement() {
  vsnGfxView* pgv = gfxAct_KI.getGfxView();
  if ( pgv ) {
    pgv->popActions(this);
    gfxAct_KI.setGfxView(NULL);
  }
}


/* event handler */

void vsnMPP_Sv_orthoVector::OnSetSlicerBtn(wxCommandEvent& event) {
  if ( ! m_pSlicerLst || ! m_pSetSlicerBtn ) return;
  WhichDataType sel = m_pSlicerLst->GetSelection();
  if ( sel < 0 ) return;

  if ( m_pSlicerTxt )
    m_pSlicerTxt->SetValue(m_pSlicerLst->GetString(sel));

  vsnMethod_Sv_orthoVector* pm
    = dynamic_cast<vsnMethod_Sv_orthoVector*>(p_method);
  if ( ! pm ) return;
  vsnData_Sv* pdo = dynamic_cast<vsnData_Sv*>(pm->getRefData());
  if ( ! pdo ) return;

  pm->setOrthoSlicer(pdo, sel);
  pm->chkNotice();
}

void vsnMPP_Sv_orthoVector::OnEnterSlicerTxt(wxCommandEvent& event) {
  if ( ! m_pSlicerTxt ) return;

  vsnMethod_Sv_orthoVector* pm
    = dynamic_cast<vsnMethod_Sv_orthoVector*>(p_method);
  if ( ! pm ) return;
  vsnMethod_Sv_orthoSlicer* posr = pm->getOrthoSlicer();
  if ( ! posr ) return;
  vsnUiView* puiv = getUiView();
  if ( ! puiv ) return;

  puiv->selectObj((vfrNode*)posr);
}

void vsnMPP_Sv_orthoVector::OnVecScaleTxt(wxCommandEvent& event) {
  if ( ! m_pVecScaleTxt ) return;

  vsnMethod_Sv_orthoVector* pm
    = dynamic_cast<vsnMethod_Sv_orthoVector*>(p_method);
  if ( ! pm ) return;

  wxString valStr = m_pVecScaleTxt->GetValue();
  if ( valStr.IsEmpty() ) return;
  float value = (float)atof(vsnApp::ConvWxToSys(valStr).c_str());

  if ( pm->setVecScale(value) )
    pm->chkNotice();
}

void vsnMPP_Sv_orthoVector::OnVecHeadChk(wxCommandEvent& event) {
  if ( ! m_pVecHeadChk ) return;
  bool val = m_pVecHeadChk->GetValue();

  vsnMethod_Sv_orthoVector* pm
    = dynamic_cast<vsnMethod_Sv_orthoVector*>(p_method);
  if ( ! pm ) return;

  if ( pm->setVecHeadMode(val) )
    pm->chkNotice();
}

void vsnMPP_Sv_orthoVector::OnVecDataChkLst(wxCommandEvent& event) {
  if ( ! m_pVecDataChkLst ) return;
  int dlen = m_pVecDataChkLst->GetCount();
  if ( dlen < 3 ) return;

  vsnMethod_Sv_orthoVector* pm
    = dynamic_cast<vsnMethod_Sv_orthoVector*>(p_method);
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

void vsnMPP_Sv_orthoVector::OnSelDataLst(wxCommandEvent& event) {
  if ( ! m_pSelDataLst ) return;
  WhichDataType sel = event.GetInt();
  if ( sel < 0 ) return;

  wxString valStr = m_pSelDataLst->GetString(sel);
  if ( valStr == wxT("vector length") )
    sel = DATA_Veclen;

  vsnMethod_Sv_orthoVector* pm
    = dynamic_cast<vsnMethod_Sv_orthoVector*>(p_method);
  if ( ! pm ) return;
  if ( sel == pm->getSelectedData() ) return;

  if ( ! pm->setSelectedData(sel) ) {
    update();
    return;
  }
  pm->chkNotice();
}

void vsnMPP_Sv_orthoVector::OnUpdMinMaxChk(wxCommandEvent& event) {
  if ( ! m_pUpdMinMaxChk ) return;
  bool val = m_pUpdMinMaxChk->GetValue();

  vsnMethod_Sv_orthoVector* pm
    = dynamic_cast<vsnMethod_Sv_orthoVector*>(p_method);
  if ( ! pm ) return;

  if ( pm->setUpdateMinMaxMode(val) )
    pm->chkNotice();
}

void vsnMPP_Sv_orthoVector::OnLineWidthTxt(wxCommandEvent& event) {
  if ( ! m_pLineWidthTxt ) return;
  vsnMethod_Sv_orthoVector* pm
    = dynamic_cast<vsnMethod_Sv_orthoVector*>(p_method);
  if ( ! pm ) return;

  wxString valStr = m_pLineWidthTxt->GetValue();
  if ( valStr.IsEmpty() ) return;
  float value = (float)atof(vsnApp::ConvWxToSys(valStr).c_str());
  if ( value <= 0.f ) return;

  if ( pm->setLineWidth(value) )
    pm->chkNotice();
}

void vsnMPP_Sv_orthoVector::OnAntiAliasChk(wxCommandEvent& event) {
  if ( ! m_pAntiAliasChk ) return;
  vsnMethod_Sv_orthoVector* pm
    = dynamic_cast<vsnMethod_Sv_orthoVector*>(p_method);
  if ( ! pm ) return;

  bool val = m_pAntiAliasChk->GetValue();
  if ( pm->setAntiAliasMode(val) )
    pm->chkNotice();
}


//----------------------------------------------------------------
// class vsnMethod_Sv_orthoVector
//----------------------------------------------------------------

/* constructors / destructor */

vsnMethod_Sv_orthoVector::vsnMethod_Sv_orthoVector(const string& name)
  : vsnMethodObj(name),
    m_vecScale(1.f), m_vecHead(true), m_lineWidth(1.f),
    m_selectedData(DATA_None), m_updateMinMax(true),
    m_vecDataIdx(0,1,2), m_vectors(NULL)
{
}

vsnMethod_Sv_orthoVector::~vsnMethod_Sv_orthoVector() {
  if ( m_vectors )
    delete m_vectors;
}


/* methods */

void vsnMethod_Sv_orthoVector::adjustRange() {
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

bool vsnMethod_Sv_orthoVector::setVecScale(const float vs) {
  if ( m_vecScale == vs ) return true;
  m_vecScale = vs;

  if ( m_vectors )
    m_vectors->setScaleFac(m_vecScale);

  updateUI();
  return true;
}

bool vsnMethod_Sv_orthoVector::setVecHeadMode(const bool vhm) {
  if ( m_vecHead == vhm ) return true;
  m_vecHead = vhm;

  if ( m_vectors )
    m_vectors->setHeadMode(m_vecHead);

  updateUI();
  return true;
}

bool vsnMethod_Sv_orthoVector::setVecDataIdx(const Vec3<int>& vdidx) {
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

bool vsnMethod_Sv_orthoVector::isValidVecData() const {
  vsnData_Sv* pData = dynamic_cast<vsnData_Sv*>(p_refData);
  if ( ! pData ) return false;

  size_t dlen = pData->getDataLen();
  if ( m_vecDataIdx[0] < 0 || m_vecDataIdx[0] >= dlen ) return false;
  if ( m_vecDataIdx[1] < 0 || m_vecDataIdx[1] >= dlen ) return false;
  if ( m_vecDataIdx[2] < 0 || m_vecDataIdx[2] >= dlen ) return false;
  return true;
}

bool vsnMethod_Sv_orthoVector::setSelectedData(const WhichDataType sd) {
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

bool vsnMethod_Sv_orthoVector::setUpdateMinMaxMode(const bool mode) {
  if ( mode == m_updateMinMax ) return true;
  m_updateMinMax = mode;

  // data range
  adjustRange();

  if ( ! update() ) return false;
  updateUI();
  return true;
}

bool vsnMethod_Sv_orthoVector::setLineWidth(const float lw) {
  if ( m_lineWidth == lw ) return true;
  if ( lw <= 0.f ) return false;

  m_lineWidth = lw;
  if ( m_vectors ) {
    m_vectors->getPrivateMaterial()->setLineWidth(m_lineWidth);
    m_vectors->notice();
  }

  updateUI();
  return true;
}


/* vsnTimeSeriesMethodIF method */

bool vsnMethod_Sv_orthoVector::updateStep(const int stp,
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

  // vectors data
  if ( ! m_vectors ) {
    m_vectors = new vfrVectors();
    if ( ! m_vectors ) {
      ErrMsg(MsgERR, getMethodType() + string("[") + getName()
	     + string("]: memory allocation failed"));
      return false;
    }
    m_vectors->alcMaterial();
    m_vectors->setHeadWidth(0.03f);
    m_vectors->setShowZero(FALSE);
    addChild(m_vectors);
  }
  m_vectors->getPrivateMaterial()->setRenderMode(RT_NONE);
  if ( ! m_show ) {
    return true;
  }

  // check selected data
  size_t dlen = pData->getDataLen();
  if ( ! isValidVecData() ) return true;  

  // check slicer
  if ( ! p_osr ) return true;
  Point2 sliceSize = p_osr->getSliceSize();
  size_t sliceSz = sliceSize.x * sliceSize.y;
  if ( sliceSz < 1 ) return true;
  const int* const sliceIndices = p_osr->getSliceIndices();
  if ( ! sliceIndices ) {
    ErrMsg(MsgERR, getMethodType() + string("[") + getName()
	   + string("]: can't get slice indices data"));
    return false;
  }

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
  if ( ! m_vectors->alcVerts(sliceSz) ||
       ! m_vectors->alcNormals(sliceSz) ) {
    return false;
  }
  if ( m_selectedData == DATA_Veclen ||
       (m_selectedData > 0 && m_selectedData <= dlen) ) {
    if ( ! m_vectors->alcColors(sliceSz) ) {
      return false;
    }
  }

  // set verts of vectors
  register int i, idx, c;
  vector3* vl = m_vectors->getVerts();
  for ( i = 0; i < sliceSz; i++ ) {
    idx = sliceIndices[i] * 3;
    vl[i][0] = gptr[idx   ];
    vl[i][1] = gptr[idx +1];
    vl[i][2] = gptr[idx +2];
  } // end of for(i)
  m_vectors->generateBbox();

  // set normals / colors of vectors
  vector3* nl = m_vectors->getNormals();
  vector4* cl = m_vectors->getColors();
  Vec3<float> vv;
  if ( m_selectedData == DATA_Veclen ) {
    for ( i = 0; i < sliceSz; i++ ) {
      if ( maskptr && maskptr[sliceIndices[i]] ) {
	nl[i][0] = nl[i][1] = nl[i][2] = 0.f;
	cl[i][0] = cl[i][1] = cl[i][2] = cl[i][3] = 0.f;
	continue;
      }
      idx = sliceIndices[i] * dlen;
      vv.m_v[0] = vptr[idx + m_vecDataIdx.m_v[0]];
      vv.m_v[1] = vptr[idx + m_vecDataIdx.m_v[1]];
      vv.m_v[2] = vptr[idx + m_vecDataIdx.m_v[2]];
      memcpy(nl[i], vv.m_v, sizeof(vector3));
      c = m_lut.getValIdx(vv.Length());
      cl[i][0] = m_lut.lutEntry[c*4  ];
      cl[i][1] = m_lut.lutEntry[c*4+1];
      cl[i][2] = m_lut.lutEntry[c*4+2];
      cl[i][3] = 1.f;
    } // end of for(i)
    m_vectors->setColorMode(AT_PER_VERTEX);
  }
  else if ( m_selectedData > 0 && m_selectedData <= dlen ) {
    for ( i = 0; i < sliceSz; i++ ) {
      if ( maskptr && maskptr[sliceIndices[i]] ) {
	nl[i][0] = nl[i][1] = nl[i][2] = 0.f;
	cl[i][0] = cl[i][1] = cl[i][2] = cl[i][3] = 0.f;
	continue;
      }
      idx = sliceIndices[i] * dlen;
      nl[i][0] = vptr[idx + m_vecDataIdx.m_v[0]];
      nl[i][1] = vptr[idx + m_vecDataIdx.m_v[1]];
      nl[i][2] = vptr[idx + m_vecDataIdx.m_v[2]];
      c = m_lut.getValIdx(vptr[idx  + m_selectedData -1]);
      cl[i][0] = m_lut.lutEntry[c*4  ];
      cl[i][1] = m_lut.lutEntry[c*4+1];
      cl[i][2] = m_lut.lutEntry[c*4+2];
      cl[i][3] = 1.f;
    } // end of for(i)
    m_vectors->setColorMode(AT_PER_VERTEX);
  }
  else { // no color
    for ( i = 0; i < sliceSz; i++ ) {
      if ( maskptr && maskptr[sliceIndices[i]] ) {
	nl[i][0] = nl[i][1] = nl[i][2] = 0.f;
	continue;
      }
      idx = sliceIndices[i] * dlen;
      nl[i][0] = vptr[idx + m_vecDataIdx.m_v[0]];
      nl[i][1] = vptr[idx + m_vecDataIdx.m_v[1]];
      nl[i][2] = vptr[idx + m_vecDataIdx.m_v[2]];
    } // end of for(i)
    cl[0][0] = m_colour[0]; cl[0][1] = m_colour[1];
    cl[0][2] = m_colour[2]; cl[0][3] = 1.f;
    m_vectors->setColorMode(AT_WHOLE);
  }

  // vectors' attr
  m_vectors->setTransparency(m_antiAlias);
  m_vectors->setScaleFac(m_vecScale);
  m_vectors->setHeadMode(m_vecHead);
  m_vectors->getPrivateMaterial()->setLineWidth(m_lineWidth);

  // ok
  m_vectors->getPrivateMaterial()->setRenderMode(m_showType);
  setBaseColor(m_colour);
  setHilight(m_hilight);
  m_updatedStp = m_requestedStp;
  return true;
}


/* from vsnMethodObj */

bool vsnMethod_Sv_orthoVector::update(const bool force) {
  return updateStep(m_requestedStp, force);
}

void vsnMethod_Sv_orthoVector::reloaded() {
  adjustRange();
  vsnMethodObj::reloaded();
}

vsnMethodPP* vsnMethod_Sv_orthoVector::getParamPanel(wxPanel* pp) {
  vsnMPP_Sv_orthoVector* pp_orthoVector
    = new vsnMPP_Sv_orthoVector(pp, this);
  if ( ! pp_orthoVector ) return NULL;
  return pp_orthoVector;
}


/* serialize : from vsnIoObject */

bool vsnMethod_Sv_orthoVector::parseXML(xmlNodePtr xnp) {
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

bool vsnMethod_Sv_orthoVector::outputXML(std::ostream& os, const size_t ts)
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

  // vec_scale
  if ( m_vecScale != 1.f ) {
    os << idts_2 << "<param name=\"vec_scale\" value=\""
       << m_vecScale << "\" />" << endl;
  }

  // vec_head
  if ( ! m_vecHead ) {
    os << idts_2 << "<param name=\"vec_head\" value=\"no\" />" << endl;
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
  return ret;
}

bool vsnMethod_Sv_orthoVector::commandXML(xmlNodePtr xnp) {
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

void vsnMethod_Sv_orthoVector::noticeUpdate() {
  update();
}
