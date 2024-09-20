//
// vsnMethod_Scatter_plotPoints
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

#include "vsnMethod_Scatter_plotPoints.h"
#include "vsnUiView.h"
#include "vsnError.h"
#include "vsnOctTree.h" // for decomp vecIdx
#include "vfrBall.h"

using namespace std;
using namespace CES;
using namespace VSN;


//----------------------------------------------------------------
// class vsnMPP_Scatter_plotPoints
//----------------------------------------------------------------
BEGIN_EVENT_TABLE(vsnMPP_Scatter_plotPoints, wxPanel)
  EVT_RADIOBOX(MPP_Scatter_plotPoints_PointTypeRadio,
	       vsnMPP_Scatter_plotPoints::OnPointTypeRadio)
  EVT_TEXT_ENTER(MPP_Scatter_plotPoints_DotSizeTxt,
                 vsnMPP_Scatter_plotPoints::OnDotSizeTxt)
  EVT_TEXT_ENTER(MPP_Scatter_plotPoints_RadiusMinTxt,
                 vsnMPP_Scatter_plotPoints::OnRadiusRangeTxt)
  EVT_TEXT_ENTER(MPP_Scatter_plotPoints_RadiusMaxTxt,
                 vsnMPP_Scatter_plotPoints::OnRadiusRangeTxt)
  EVT_CHECKLISTBOX(MPP_Scatter_plotPoints_VecDataChkLst,
                   vsnMPP_Scatter_plotPoints::OnVecDataChkLst)
  EVT_COMBOBOX(MPP_Scatter_plotPoints_SelDataLst,
               vsnMPP_Scatter_plotPoints::OnSelDataLst)
  EVT_CHECKBOX(MPP_Scatter_plotPoints_UpdMinMaxChk,
               vsnMPP_Scatter_plotPoints::OnUpdMinMaxChk)
  EVT_CHECKBOX(MPP_Scatter_plotPoints_AntiAliasChk,
               vsnMPP_Scatter_plotPoints::OnAntiAliasChk)
  EVT_TEXT_ENTER(MPP_Scatter_plotPoints_SubdivTxt,
                 vsnMPP_Scatter_plotPoints::OnSubdivTxt)
END_EVENT_TABLE()


/* constructors / destructor */

vsnMPP_Scatter_plotPoints::vsnMPP_Scatter_plotPoints(wxPanel* parent,
						     vsnMethodObj* pm)
  : vsnMethodPP(parent, pm)
{
  assert(parent);
  assert(dynamic_cast<vsnMethod_Scatter_plotPoints*>(pm));

  // create widgets
  wxString ritems[] = {wxString(wxT("dots")), wxString(wxT("balls"))};
  m_pPointTypeRadio = new wxRadioBox(this,
				     MPP_Scatter_plotPoints_PointTypeRadio,
				     wxT("point type"), wxDefaultPosition,
				     wxDefaultSize,
				     2, ritems, 1, wxRA_SPECIFY_ROWS);
  assert(m_pPointTypeRadio);
  m_pDotSizeTxt = new wxTextCtrl(this, MPP_Scatter_plotPoints_DotSizeTxt,
				 wxT(""), wxDefaultPosition, wxDefaultSize,
				 wxTE_PROCESS_ENTER);
  assert(m_pDotSizeTxt);
  m_pRadiusMinTxt = new wxTextCtrl(this, MPP_Scatter_plotPoints_RadiusMinTxt,
				   wxT(""), wxDefaultPosition, wxSize(60,-1),
				   wxTE_PROCESS_ENTER);
  assert(m_pRadiusMinTxt);
  m_pRadiusMaxTxt = new wxTextCtrl(this, MPP_Scatter_plotPoints_RadiusMaxTxt,
				   wxT(""), wxDefaultPosition, wxSize(60,-1),
				   wxTE_PROCESS_ENTER);
  assert(m_pRadiusMaxTxt);
  m_pVecDataChkLst = new wxCheckListBox(this,
                                        MPP_Scatter_plotPoints_VecDataChkLst);
  assert(m_pVecDataChkLst);
  m_pSelDataLst = new wxComboBox(this, MPP_Scatter_plotPoints_SelDataLst,
                                 wxT(""), wxDefaultPosition, wxDefaultSize,
                                 0, NULL, wxCB_READONLY);
  assert(m_pSelDataLst);
  m_pUpdMinMaxChk = new wxCheckBox(this, MPP_Scatter_plotPoints_UpdMinMaxChk,
                                   wxT("update lut minmax"));
  assert(m_pUpdMinMaxChk);
  m_pAntiAliasChk = new wxCheckBox(this, MPP_Scatter_plotPoints_AntiAliasChk,
                                   wxT("anti-alias dot"));
  assert(m_pAntiAliasChk);
  m_pSubdivTxt = new wxTextCtrl(this, MPP_Scatter_plotPoints_SubdivTxt,
				wxT(""), wxDefaultPosition, wxDefaultSize,
				wxTE_PROCESS_ENTER);
  assert(m_pSubdivTxt);

  // prepare sizers
  wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL); assert(topsizer);
  wxBoxSizer* sizerH;

  // point type
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND|wxALL, 3);
  sizerH->Add(m_pPointTypeRadio, 1, wxEXPAND, 0);

  // size params
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND|wxALL, 3);
  sizerH->Add(new wxStaticText(this, -1, wxT("dot size")),
              0, wxALIGN_LEFT|wxALL, 3);
  sizerH->Add(m_pDotSizeTxt, 1, wxALIGN_LEFT|wxEXPAND|wxALL, 3);

  topsizer->Add(new wxStaticText(this, -1, wxT("ball radius range")),
              0, wxALIGN_LEFT|wxALL, 3);
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND|wxALL, 3);
  sizerH->Add(new wxStaticText(this, -1, wxT("min")),
              0, wxALIGN_LEFT|wxALL, 3);
  sizerH->Add(m_pRadiusMinTxt, 1, wxALIGN_LEFT|wxEXPAND|wxALL, 3);
  sizerH->Add(new wxStaticText(this, -1, wxT(" max")),
              0, wxALIGN_LEFT|wxALL, 3);
  sizerH->Add(m_pRadiusMaxTxt, 1, wxALIGN_LEFT|wxEXPAND|wxALL, 3);

  // data selector
  topsizer->Add(new wxStaticText(this, -1, wxT("select scalar data")),
                0, wxALIGN_LEFT|wxALL, 3);
  topsizer->Add(m_pSelDataLst, 0, wxEXPAND|wxALL, 3);

  // vector data selector
  topsizer->Add(new wxStaticText(this, -1, wxT("vector components")),
                0, wxALIGN_LEFT|wxALL, 3);
  topsizer->Add(m_pVecDataChkLst, 0, wxEXPAND|wxALL, 3);

  // update minmax mode
  m_pUpdMinMaxChk->SetValue(TRUE);
  topsizer->Add(m_pUpdMinMaxChk, 0, wxALL, 3);

  // anti-alias
  topsizer->Add(m_pAntiAliasChk, 0, wxALL, 3);

  // subdiv
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxALL, 3);
  sizerH->Add(new wxStaticText(this, -1, wxT("ball subdiv")),
              0, wxALIGN_LEFT|wxALL, 3);
  sizerH->Add(m_pSubdivTxt, 0, wxALIGN_LEFT|wxALL, 3);  

  // post process
  SetSizer(topsizer);
  addTo(parent);
  topsizer->Layout();
  (void)update();
}

vsnMPP_Scatter_plotPoints::~vsnMPP_Scatter_plotPoints() {
}


/* interface */

bool vsnMPP_Scatter_plotPoints::update() {
  register size_t i;
  char txt[64];

  vsnMethod_Scatter_plotPoints* pm
    = dynamic_cast<vsnMethod_Scatter_plotPoints*>(p_method);
  if ( ! pm ) return false;
  vsnNumericalDataIF* pdo
    = dynamic_cast<vsnNumericalDataIF*>(pm->getRefData());
  if ( ! pdo ) return false;

  // point type
  VFR::RenderType rt = pm->getRenderType();
  if ( rt == RT_SMOOTH )
    m_pPointTypeRadio->SetSelection(1);
  else
    m_pPointTypeRadio->SetSelection(0);

  // size params
  float ds = pm->getDotSize();
  sprintf(txt, "%g", ds);
  m_pDotSizeTxt->SetValue(vsnApp::ConvSysToWx(txt));
  float rr[2];
  pm->getRadiusRange(rr);
  sprintf(txt, "%g", rr[0]);
  m_pRadiusMinTxt->SetValue(vsnApp::ConvSysToWx(txt));
  sprintf(txt, "%g", rr[1]);
  m_pRadiusMaxTxt->SetValue(vsnApp::ConvSysToWx(txt));

  // vector data indices
  int dlen = (int)pdo->getDataLen();
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

  // select scalar data
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

  // update minmax mode
  m_pUpdMinMaxChk->SetValue(pm->getUpdateMinMaxMode());

  // anti-alias
  m_pAntiAliasChk->SetValue(pm->getAntiAliasMode());

  // subdiv
  int sdv = pm->getSubdiv();
  sprintf(txt, "%d", sdv);
  m_pSubdivTxt->SetValue(vsnApp::ConvSysToWx(txt));  

  return true;
}


/* event handler */

void vsnMPP_Scatter_plotPoints::OnPointTypeRadio(wxCommandEvent& event) {
  vsnMethod_Scatter_plotPoints* pm
    = dynamic_cast<vsnMethod_Scatter_plotPoints*>(p_method);
  if ( ! pm ) return;

  int val = m_pPointTypeRadio->GetSelection();
  RenderType rt;
  if ( val == 1 ) rt = RT_SMOOTH;
  else rt = RT_POINT;

  if ( pm->getRenderType() == rt ) return;
  if ( pm->setRenderType(rt) )
    pm->chkNotice();
}

void vsnMPP_Scatter_plotPoints::OnDotSizeTxt(wxCommandEvent& event) {
  vsnMethod_Scatter_plotPoints* pm
    = dynamic_cast<vsnMethod_Scatter_plotPoints*>(p_method);
  if ( ! pm ) return;

  wxString valStr = m_pDotSizeTxt->GetValue();
  if ( valStr.IsEmpty() ) return;
  string xstr = vsnApp::ConvWxToSys(valStr);
  float value = (float)atof(xstr.c_str());
  if ( value < 1e-6f ) value = 1e-6f;

  if ( pm->setDotSize(value) )
    pm->chkNotice();
}

void vsnMPP_Scatter_plotPoints::OnRadiusRangeTxt(wxCommandEvent& event) {
  vsnMethod_Scatter_plotPoints* pm
    = dynamic_cast<vsnMethod_Scatter_plotPoints*>(p_method);
  if ( ! pm ) return;

  char txt[64];
  float rr[2], orr[2];
  pm->getRadiusRange(orr);
  wxString valStr = m_pRadiusMinTxt->GetValue();
  if ( valStr.IsEmpty() ) {
    rr[0] = orr[0];
    sprintf(txt, "%g", rr[0]);
    m_pRadiusMinTxt->SetValue(vsnApp::ConvSysToWx(txt));
  }
  else {
    rr[0] = (float)atof(vsnApp::ConvWxToSys(valStr).c_str());
  }
  valStr = m_pRadiusMaxTxt->GetValue();
  if ( valStr.IsEmpty() ) {
    rr[1] = orr[1];
    sprintf(txt, "%g", rr[1]);
    m_pRadiusMaxTxt->SetValue(vsnApp::ConvSysToWx(txt));
  } else {
    rr[1] = (float)atof(vsnApp::ConvWxToSys(valStr).c_str());
  }
  if ( rr[0] == orr[0] && rr[1] == orr[1] ) return;
  if ( rr[0] < 0.f ) {
    sprintf(txt, "%g", orr[0]);
    m_pRadiusMinTxt->SetValue(vsnApp::ConvSysToWx(txt));
  }
  if ( rr[1] < 0.f ) {
    sprintf(txt, "%g", orr[1]);
    m_pRadiusMaxTxt->SetValue(vsnApp::ConvSysToWx(txt));
  }

  if ( rr[0] > rr[1] ) {
    float x = rr[0]; rr[0] = rr[1]; rr[1] = x;
    sprintf(txt, "%g", rr[0]);
    m_pRadiusMinTxt->SetValue(vsnApp::ConvSysToWx(txt));
    sprintf(txt, "%g", rr[1]);
    m_pRadiusMaxTxt->SetValue(vsnApp::ConvSysToWx(txt));
  }

  if ( pm->setRadiusRange(rr) )
    pm->chkNotice();
}

void vsnMPP_Scatter_plotPoints::OnVecDataChkLst(wxCommandEvent& event) {
  if ( ! m_pVecDataChkLst ) return;
  int dlen = m_pVecDataChkLst->GetCount();
  if ( dlen < 3 ) return;

  vsnMethod_Scatter_plotPoints* pm
    = dynamic_cast<vsnMethod_Scatter_plotPoints*>(p_method);
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

void vsnMPP_Scatter_plotPoints::OnSelDataLst(wxCommandEvent& event) {
  if ( ! m_pSelDataLst ) return;
  WhichDataType sel = event.GetInt();
  if ( sel < 0 ) return;

  wxString valStr = m_pSelDataLst->GetString(sel);
  if ( valStr == wxT("vector length") )
    sel = DATA_Veclen;

  vsnMethod_Scatter_plotPoints* pm
    = dynamic_cast<vsnMethod_Scatter_plotPoints*>(p_method);
  if ( ! pm ) return;
  if ( sel == pm->getSelectedData() ) return;

  if ( ! pm->setSelectedData(sel) ) {
    update();
    return;
  }
  pm->chkNotice();
}

void vsnMPP_Scatter_plotPoints::OnUpdMinMaxChk(wxCommandEvent& event) {
  if ( ! m_pUpdMinMaxChk ) return;
  bool val = m_pUpdMinMaxChk->GetValue();

  vsnMethod_Scatter_plotPoints* pm
    = dynamic_cast<vsnMethod_Scatter_plotPoints*>(p_method);
  if ( ! pm ) return;

  if ( pm->setUpdateMinMaxMode(val) )
    pm->chkNotice();
}

void vsnMPP_Scatter_plotPoints::OnAntiAliasChk(wxCommandEvent& event) {
  if ( ! m_pAntiAliasChk ) return;
  vsnMethod_Scatter_plotPoints* pm
    = dynamic_cast<vsnMethod_Scatter_plotPoints*>(p_method);
  if ( ! pm ) return;

  bool val = m_pAntiAliasChk->GetValue();
  if ( pm->setAntiAliasMode(val) )
    pm->chkNotice();
}

void vsnMPP_Scatter_plotPoints::OnSubdivTxt(wxCommandEvent& event) {
  vsnMethod_Scatter_plotPoints* pm
    = dynamic_cast<vsnMethod_Scatter_plotPoints*>(p_method);
  if ( ! pm ) return;

  wxString valStr = m_pSubdivTxt->GetValue();
  if ( valStr.IsEmpty() ) return;
  int value = atoi(vsnApp::ConvWxToSys(valStr).c_str());
  if ( value < 0 ) value = 0;

  if ( pm->setSubdiv(value) )
    pm->chkNotice();
}


//----------------------------------------------------------------
// class vsnMethod_Scatter_plotPoints
//----------------------------------------------------------------

/* constructors / destructor */

vsnMethod_Scatter_plotPoints::vsnMethod_Scatter_plotPoints(const string& name)
  : vsnMethodObj(name),
    m_renType(RT_POINT), m_dotSize(3.f), m_subdiv(2),
    m_selectedData(DATA_None), m_updateMinMax(true),
    m_vecDataIdx(0,1,2), m_shape(NULL)
{
  m_showType = RT_POINT;
  m_radius[0] = m_radius[1] = 1.f;
}

vsnMethod_Scatter_plotPoints::~vsnMethod_Scatter_plotPoints() {
  if ( m_shape )
    delete m_shape;
}


/* methods */

void vsnMethod_Scatter_plotPoints::adjustRange() {
  bool needRangeUpd = false;

  vsnNumericalDataIF* pData = dynamic_cast<vsnNumericalDataIF*>(p_refData);
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

bool vsnMethod_Scatter_plotPoints::setRenderType(const VFR::RenderType rt) {
  if ( m_renType == rt ) return true;
  if ( rt != RT_SMOOTH && rt != RT_POINT ) return false;
  m_renType = rt;
  m_showType = m_renType;

  if ( m_shape ) {
    if ( m_renType == RT_SMOOTH ) {
      m_shape->setTransparency(m_colour[3]<0.991f);
      m_shape->setAlpha(m_colour[3]);
    } else {
      m_shape->setTransparency(m_antiAlias);
      m_shape->setAlpha(1.f);
    }
    vfrMaterial* pmate = m_shape->getPrivateMaterial();
    if ( pmate->getRenderMode() != RT_NONE ) {
      pmate->setRenderMode(m_renType);
    }
  } // end of if(m_shape)

  updateUI();
  return true;
}  

bool vsnMethod_Scatter_plotPoints::setDotSize(const float ds) {
  if ( m_dotSize == ds ) return true;
  m_dotSize = ds;

  if ( m_shape )
    m_shape->getPrivateMaterial()->setPointSize(m_dotSize);

  updateUI();
  return true;
}

bool vsnMethod_Scatter_plotPoints::setRadiusRange(const float rr[2]) {
  if ( m_radius[0] == rr[0] && m_radius[1] == rr[1] ) return true;
  if ( rr[0] < 0.f || rr[1] < 0.f ) return false;
  if ( rr[0] > rr[1] ) return false;
  m_radius[0] = rr[0];
  m_radius[1] = rr[1];

  if ( m_shape ) update();

  updateUI();
  return true;
}

bool vsnMethod_Scatter_plotPoints::setSubdiv(const int sdv) {
  if ( sdv < 0 ) return false;
  if ( m_subdiv == sdv ) return true;
  m_subdiv = sdv;

  if ( m_shape ) {
    vfrBall* pball = dynamic_cast<vfrBall*>(m_shape->getPrimitive());
    if ( pball ) pball->setSubdiv(m_subdiv);
  }

  updateUI();
  return true;
}

bool vsnMethod_Scatter_plotPoints::setVecDataIdx(const Vec3<int>& vdidx) {
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

bool vsnMethod_Scatter_plotPoints::setSelectedData(const WhichDataType sd) {
  if ( sd == m_selectedData ) return true;

  vsnNumericalDataIF* pData = dynamic_cast<vsnNumericalDataIF*>(p_refData);
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

bool vsnMethod_Scatter_plotPoints::isValidVecData() const {
  vsnNumericalDataIF* pData = dynamic_cast<vsnNumericalDataIF*>(p_refData);
  if ( ! pData ) return false;

  size_t dlen = pData->getDataLen();
  if ( m_vecDataIdx[0] < 0 || m_vecDataIdx[0] >= dlen ) return false;
  if ( m_vecDataIdx[1] < 0 || m_vecDataIdx[1] >= dlen ) return false;
  if ( m_vecDataIdx[2] < 0 || m_vecDataIdx[2] >= dlen ) return false;
  return true;
}

bool vsnMethod_Scatter_plotPoints::setUpdateMinMaxMode(const bool mode) {
  if ( mode == m_updateMinMax ) return true;
  m_updateMinMax = mode;

  // data range
  if ( m_updateMinMax ) {
    vsnNumericalDataIF* pData = dynamic_cast<vsnNumericalDataIF*>(p_refData);
    if ( pData ) {
      float dr[2];
      if ( m_selectedData == DATA_Veclen ) {
        if ( pData->getVectorMaxLen(m_vecDataIdx, dr[1]) ) {
          m_lut.minVal = 0.f;
          m_lut.maxVal = dr[1];
        }
      } // end of if(DATA_Veclen)
      else if ( m_selectedData > 0 && m_selectedData <= pData->getDataLen() ) {
        if ( pData->getMinMax(m_selectedData -1, dr) ) {
          m_lut.minVal = dr[0];
          m_lut.maxVal = dr[1];
        }
      }
    } // end of if(pData)
  } // end of if(m_updateMinMax)

  if ( ! update() ) return false;
  updateUI();
  return true;
}


/* from vsnMethodObj */

bool vsnMethod_Scatter_plotPoints::update(const bool force) {
  return updateStep(m_requestedStp, force);
}

void vsnMethod_Scatter_plotPoints::reloaded() {
  adjustRange();
  vsnMethodObj::reloaded();
}

vsnMethodPP* vsnMethod_Scatter_plotPoints::getParamPanel(wxPanel* pp) {
  vsnMPP_Scatter_plotPoints* pp_plotPoints
    = new vsnMPP_Scatter_plotPoints(pp, this);
  if ( ! pp_plotPoints ) return NULL;
  return pp_plotPoints;
}


/* vsnTimeSeriesMethodIF methods */

bool vsnMethod_Scatter_plotPoints::updateStep(const int stp, const bool force,
					      const bool cascade)
{
  vsnData_Scatter* pData = dynamic_cast<vsnData_Scatter*>(p_refData);
  if ( ! pData ) return false;
  m_requestedStp = stp;
  if ( m_requestedStp < 0 )
    m_requestedStp = pData->getCurrentStepIdx();

  if ( force ) m_updatedStp = -1;
  if ( m_updatedStp == m_requestedStp ) return true;
  m_updatedStp = -1;

  // shape
  if ( ! m_shape ) {
    m_shape = new vfrPrimSet();
    if ( ! m_shape ) {
      ErrMsg(MsgERR, getMethodType() + string("[") + getName()
             + string("]: memory allocation failed"));
      return false;
    }
    m_shape->alcMaterial();
    m_shape->getPrivateMaterial()->setPointSize(m_dotSize);
    m_shape->setRotScaleMode(VFR_PRIMSET_SCALE);
    vfrBall* pball = new vfrBall(1.f, "SCAT_PLOTPTS_BALL", TRUE);
    pball->setSubdiv(m_subdiv);
    m_shape->setPrimitive(pball);
    addChild(m_shape);
  }
  m_shape->getPrivateMaterial()->setRenderMode(RT_NONE);
  if ( ! m_show ) {
    return true;
  }

  // check selected data
  size_t dlen = pData->getDataLen();
  if ( dlen < 1 ) return true;
  if ( m_selectedData == DATA_Veclen && ! isValidVecData() ) return true;

  // alloc vector datas
  int sampleSz = pData->getNumVerts();
  if ( sampleSz < 1 ) return true;
  if ( ! m_shape->alcVerts(sampleSz) || ! m_shape->alcNormals(sampleSz) ) {
    ErrMsg(MsgERR, getMethodType() + string("[") + getName()
	   + string("]: memory allocation for vertex failed"));
    return false;
  }
  if ( m_selectedData == DATA_Veclen ||
       (m_selectedData > 0 && m_selectedData <= dlen) ) {
    if ( ! m_shape->alcColors(sampleSz) ) {
      ErrMsg(MsgERR, getMethodType() + string("[") + getName()
	     + string("]: memory allocation for colors failed"));
      return false;
    }
  }

  // set verts of vectors
  vector3* vl = m_shape->getVerts();
  memcpy(vl, pData->getVerts(), sizeof(vector3)*sampleSz);
  m_shape->generateBbox();

  // set normals / colors of vectors
  register float a, b, dval;
  if ( fabs(m_lut.maxVal - m_lut.minVal) < 1e-8 ) {
    a = 0.f; b = m_radius[0];
  } else {
    a = (m_radius[1] - m_radius[0]) / (m_lut.maxVal - m_lut.minVal);
    b = m_radius[0] - a * m_lut.minVal;
  }

  register int i, c;
  vector3* nl = m_shape->getNormals();
  vector4* cl = m_shape->getColors();
  float* pd = pData->getData();
  Vec3<float> vv;
  if ( m_selectedData == DATA_Veclen ) {
    for ( i = 0; i < sampleSz; i++ ) {
      vv[0] = pd[dlen*i + m_vecDataIdx[0]];
      vv[1] = pd[dlen*i + m_vecDataIdx[1]];
      vv[2] = pd[dlen*i + m_vecDataIdx[2]];
      dval = vv.Length();
      nl[i][2] = dval * a + b;
      c = m_lut.getValIdx(dval);
      cl[i][0] = m_lut.lutEntry[c*4  ];
      cl[i][1] = m_lut.lutEntry[c*4+1];
      cl[i][2] = m_lut.lutEntry[c*4+2];
      cl[i][3] = 1.f;
    } // end of for(i)
    m_shape->setColorMode(AT_PER_VERTEX);
  }
  else if ( m_selectedData > 0 && m_selectedData <= dlen ) {
    for ( i = 0; i < sampleSz; i++ ) {
      dval = pd[dlen*i + m_selectedData -1];
      nl[i][2] = dval * a + b;
      c = m_lut.getValIdx(dval);
      cl[i][0] = m_lut.lutEntry[c*4  ];
      cl[i][1] = m_lut.lutEntry[c*4+1];
      cl[i][2] = m_lut.lutEntry[c*4+2];
      cl[i][3] = 1.f;
    } // end of for(i)
    m_shape->setColorMode(AT_PER_VERTEX);
  }
  else { // not ref
    for ( i = 0; i < sampleSz; i++ ) {
      nl[i][2] = m_radius[0];
    } // end of for(i)
    cl[0][0] = m_colour[0]; cl[0][1] = m_colour[1];
    cl[0][2] = m_colour[2]; cl[0][3] = 1.f;
    m_shape->setColorMode(AT_WHOLE);
  }

  // ok
  m_shape->getPrivateMaterial()->setRenderMode(m_showType);
  setBaseColor(m_colour);
  setHilight(m_hilight);
  m_updatedStp = m_requestedStp;
  return true;
}


/* from vsnIoObject */

bool vsnMethod_Scatter_plotPoints::parseXML(xmlNodePtr xnp) {
  static xmlChar* xs;
  xmlNodePtr cur;

  if ( ! vsnMethodObj::parseXML(xnp) ) return false;
  m_showType = m_renType;
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

      if ( xsN == string("point_type") ) {
	VFR::RenderType rt;
        if ( xsV == string("ball") ) rt = RT_SMOOTH;
	else if ( xsV == string("dot") ) rt = RT_POINT;
	else {
          ErrMsg(MsgERR, msgHdr + string("invalid value in param point_type"));
          goto _NEXT_XML_NODE;
        }
	if ( ! setRenderType(rt) ) {
	  ErrMsg(MsgERR, msgHdr + string("can't set point_type"));
          goto _NEXT_XML_NODE;
	}
      } // end of "point_type"
      else if ( xsN == string("dot_size") ) {
        float vds = (float)atof(xsV.c_str());
        if ( ! setDotSize(vds) ) {
          ErrMsg(MsgERR, msgHdr + string("invalid value in param dot_size"));
          goto _NEXT_XML_NODE;
        }
      } // end of "dot_size"
      else if ( xsN == string("radius_range") ) {
	string minStr, maxStr; char c;
        istringstream iss(xsV);
        while( iss.get(c) && c != ':' ) minStr.push_back(c);
        while( iss.get(c) ) maxStr.push_back(c);
        if ( minStr.empty() || maxStr.empty() ) {
          ErrMsg(MsgERR, msgHdr
                 + string("invalid format in param radius_range"));
          goto _NEXT_XML_NODE;
        }
	float rr[2];
        rr[0] = (float)atof(minStr.c_str());
        rr[1] = (float)atof(maxStr.c_str());
        if ( ! setRadiusRange(rr) ) {
          ErrMsg(MsgERR, msgHdr+string("invalid value in param radius_range"));
          goto _NEXT_XML_NODE;
        }
      } // end of "radius_range"
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
      else if ( xsN == string("subdiv") ) {
        int sdv = atoi(xsV.c_str());
        if ( ! setSubdiv(sdv) ) {
          ErrMsg(MsgERR, msgHdr + string("invalid value in param subdiv"));
          goto _NEXT_XML_NODE;
        }
      } // end of "subdiv"
    } // end of param

  _NEXT_XML_NODE:
    cur = cur->next;
  } // end of while(cur)

  return true;
}

bool vsnMethod_Scatter_plotPoints::outputXML(std::ostream& os,
					     const size_t ts)
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

  // output base method params
  if (!vsnMethodObj::exportXMLNode(os, ts + 2)) {
      ErrMsg(MsgERR, msgHdr + string("base MethodObj exportXMLNode failed"));
      ret = false;
  }

  // output original params
  // upd_minmax
  if ( ! m_updateMinMax ) {
    os << idts_2 << "<param name=\"upd_minmax\" value=\"no\" />" << endl;
  }

  // point_type
  if ( m_renType != RT_POINT ) {
    os << idts_2 << "<param name=\"point_type\" value=\"ball\" />" << endl;
  }

  // dot_size
  if ( m_dotSize != 3.f ) {
    os << idts_2 << "<param name=\"dot_size\" value=\""
       << m_dotSize << "\" />" << endl;
  }

  // radius_range
  if ( m_radius[0] != 1.f || m_radius[1] != 1.f ) {
    os << idts_2 << "<param name=\"radius_range\" value=\""
       << m_radius[0] << ':' << m_radius[1] << "\" />" << endl;
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

  // antialias
  if ( m_antiAlias ) {
    os << idts_2 << "<param name=\"antialias\" value=\"yes\" />" << endl;
  }

  // subdiv
  if ( m_subdiv != 2 ) {
    os << idts_2 << "<param name=\"subdiv\" value=\""
       << m_subdiv << "\" />" << endl;
  }

  // output base method params
  if ( ! vsnMethodObj::exportXMLNode(os, ts+2) ) {
    ErrMsg(MsgERR, msgHdr + string("base MethodObj exportXMLNode failed"));
    ret = false;
  }

  os << idts << "</method>" << endl;
  return true;
}

bool vsnMethod_Scatter_plotPoints::commandXML(xmlNodePtr xnp) {
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
  if ( nameStr == "set_point_type" ) {
    VFR::RenderType rt;
    if ( valueStr == string("ball") ) rt = RT_SMOOTH;
    else if ( valueStr == string("dot") ) rt = RT_POINT;
    else {
      ErrMsg(MsgERR, msgHdr +
             string("command set_point_type: invalid value: ") + valueStr);
      return false;
    }
    if ( ! setRenderType(rt) ) {
      ErrMsg(MsgERR, msgHdr +
             string("command set_point_type: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_point_type"
  else if ( nameStr == "set_dot_size" ) {
    float vds = (float)atof(valueStr.c_str());
    if ( ! setDotSize(vds) ) {
      ErrMsg(MsgERR, msgHdr +
             string("command set_dot_size: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_dot_size"
  else if ( nameStr == "set_radius_range" ) {
    string minStr, maxStr; char c;
    istringstream iss(valueStr);
    while( iss.get(c) && c != ':' ) minStr.push_back(c);
    while( iss.get(c) ) maxStr.push_back(c);
    if ( minStr.empty() || maxStr.empty() ) {
      ErrMsg(MsgERR, msgHdr
             + string("command set_radius_range: invalid format"));
      return false;
    }
    float rr[2];
    rr[0] = (float)atof(minStr.c_str());
    rr[1] = (float)atof(maxStr.c_str());
    if ( ! setRadiusRange(rr) ) {
      ErrMsg(MsgERR, msgHdr +
             string("command set_radius_range: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_radius_range"
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
  else if ( nameStr == "set_subdiv" ) {
    int sdv = atoi(valueStr.c_str());
    if ( ! setSubdiv(sdv) ) {
      ErrMsg(MsgERR, msgHdr +
             string("command set_subdiv: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_subdiv"
  else {
    // not my command
    ErrMsg(MsgERR, msgHdr + string("unknown command: ") + nameStr);
    return false;
  }

  return true;
}
