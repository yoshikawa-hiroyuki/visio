//
// vsnMethod_Sv_orthoContour
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

#include "vsnMethod_Sv_orthoContour.h"
#include "vsnOctTree.h" // for decomp vecIdx
#include "vsnUiView.h"
#include "vsnError.h"

using namespace std;
using namespace CES;
using namespace VSN;


//----------------------------------------------------------------
// class vsnMPP_Sv_orthoContour
//----------------------------------------------------------------

BEGIN_EVENT_TABLE(vsnMPP_Sv_orthoContour, wxPanel)
  EVT_TEXT_ENTER(MPP_Sv_orthoContour_SlicerTxt,
		 vsnMPP_Sv_orthoContour::OnSlicerTxt)
  EVT_BUTTON(MPP_Sv_orthoContour_SetSlicerBtn,
	     vsnMPP_Sv_orthoContour::OnSetSlicerBtn)
  EVT_COMBOBOX(MPP_Sv_orthoContour_SelDataLst,
               vsnMPP_Sv_orthoContour::OnSelDataLst)
  EVT_CHECKLISTBOX(MPP_Sv_orthoContour_VecDataChkLst,
                   vsnMPP_Sv_orthoContour::OnVecDataChkLst)
  EVT_TEXT_ENTER(MPP_Sv_orthoContour_NumLinesTxt,
		 vsnMPP_Sv_orthoContour::OnNumLinesTxt)
  EVT_CHECKBOX(MPP_Sv_orthoContour_BiLinearChk,
	       vsnMPP_Sv_orthoContour::OnBiLinearChk)
  EVT_TEXT_ENTER(MPP_Sv_orthoContour_MinTxt,
		 vsnMPP_Sv_orthoContour::OnMinMaxTxt)
  EVT_TEXT_ENTER(MPP_Sv_orthoContour_MaxTxt,
		 vsnMPP_Sv_orthoContour::OnMinMaxTxt)
  EVT_CHECKBOX(MPP_Sv_orthoContour_UseCMapChk,
	       vsnMPP_Sv_orthoContour::OnUseCMapChk)
  EVT_CHECKBOX(MPP_Sv_orthoContour_UpdMinMaxChk,
	       vsnMPP_Sv_orthoContour::OnUpdMinMaxChk)
  EVT_TEXT_ENTER(MPP_Sv_orthoContour_LineWidthTxt,
		 vsnMPP_Sv_orthoContour::OnLineWidthTxt)
  EVT_CHECKBOX(MPP_Sv_orthoContour_AntiAliasChk,
	       vsnMPP_Sv_orthoContour::OnAntiAliasChk)
END_EVENT_TABLE()


/* constructors / destructor */

vsnMPP_Sv_orthoContour::vsnMPP_Sv_orthoContour(wxPanel* parent,
					       vsnMethodObj* pm)
  : vsnMethodPP(parent, pm)
{
  assert(parent);
  vsnMethod_Sv_orthoContour* pmoc
    = dynamic_cast<vsnMethod_Sv_orthoContour*>(pm);
  assert(pmoc);

  // setup gfxAct
  gfxAct_KI.setRefOrthoSlicer(pmoc);
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
  m_pSlicerTxt = new wxTextCtrl(this, MPP_Sv_orthoContour_SlicerTxt,
				wxT(""), wxDefaultPosition, wxDefaultSize,
                                wxTE_READONLY|wxTE_PROCESS_ENTER);
  sizerH->Add(m_pSlicerTxt, 0, wxEXPAND|wxALL, 3);

  // orthoSlicer selector
  m_pSlicerList = new wxListBox(this, MPP_Sv_orthoContour_SlicerLst);
  topsizer->Add(m_pSlicerList, 0, wxEXPAND|wxALL, 3);

  m_pSetSlicerBtn = new wxButton(this, MPP_Sv_orthoContour_SetSlicerBtn,
				 wxT("set orthoSlicer"));
  topsizer->Add(m_pSetSlicerBtn, 0, wxALIGN_LEFT|wxALL, 3);

  // data selector
  topsizer->Add(5, 5);
  topsizer->Add(new wxStaticText(this, -1, wxT("select scalar data")),
                0, wxALIGN_LEFT|wxALL, 3);
  m_pSelDataLst = new wxComboBox(this, MPP_Sv_orthoContour_SelDataLst,
                                 wxT(""), wxDefaultPosition, wxDefaultSize,
                                 0, NULL, wxCB_READONLY);
  topsizer->Add(m_pSelDataLst, 0, wxEXPAND|wxALL, 3);

  // vector data selector
  topsizer->Add(new wxStaticText(this, -1, wxT("vector components")),
                0, wxALIGN_LEFT|wxALL, 3);
  m_pVecDataChkLst = new wxCheckListBox(this,
                                        MPP_Sv_orthoContour_VecDataChkLst);
  topsizer->Add(m_pVecDataChkLst, 0, wxEXPAND|wxALL, 3);

  // num lines, bilinear
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxALL, 3);
  sizerH->Add(new wxStaticText(this, -1, wxT("#of lines")),
	      0, wxALIGN_LEFT|wxALL, 3);
  m_pNumLinesTxt = new wxTextCtrl(this, MPP_Sv_orthoContour_NumLinesTxt,
				  wxT(""), wxDefaultPosition, wxDefaultSize,
				  wxTE_PROCESS_ENTER);
  sizerH->Add(m_pNumLinesTxt, 0, wxALIGN_LEFT|wxALL, 3);

  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxALL, 3);
  m_pBiLinearChk = new wxCheckBox(this, MPP_Sv_orthoContour_BiLinearChk,
				  wxT("bilinear"));
  sizerH->Add(m_pBiLinearChk, 0, wxALIGN_LEFT|wxALL, 3);

  // range
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxALL, 3);
  sizerH->Add(new wxStaticText(this, -1, wxT("range")),
	      0, wxALIGN_LEFT|wxALL, 3);
  m_pMinTxt = new wxTextCtrl(this, MPP_Sv_orthoContour_MinTxt,
			     wxT(""), wxDefaultPosition, wxSize(60,-1),
			     wxTE_PROCESS_ENTER);
  sizerH->Add(m_pMinTxt, 1, wxALIGN_LEFT|wxALL, 3);
  sizerH->Add(new wxStaticText(this, -1, wxT("to")),
	      0, wxALIGN_LEFT|wxALL, 3);
  m_pMaxTxt = new wxTextCtrl(this, MPP_Sv_orthoContour_MaxTxt,
			     wxT(""), wxDefaultPosition, wxSize(60,-1),
			     wxTE_PROCESS_ENTER);
  sizerH->Add(m_pMaxTxt, 1, wxALIGN_LEFT|wxALL, 3);

  // use CMap / update minmax mode
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxALL, 3);
  m_pUseCMapChk = new wxCheckBox(this, MPP_Sv_orthoContour_UseCMapChk,
				 wxT("use cmap"));
  m_pUseCMapChk->SetValue(TRUE);
  sizerH->Add(m_pUseCMapChk, 0, wxALL, 3);

  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxALL, 3);
  m_pUpdMinMaxChk = new wxCheckBox(this, MPP_Sv_orthoContour_UpdMinMaxChk,
                                   wxT("update minmax"));
  m_pUpdMinMaxChk->SetValue(TRUE);
  sizerH->Add(m_pUpdMinMaxChk, 0, wxALL, 3);

  // line width
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxALL, 3);
  sizerH->Add(new wxStaticText(this, -1, wxT("line width")),
	      0, wxALIGN_LEFT|wxALL, 3);
  m_pLineWidthTxt = new wxTextCtrl(this, MPP_Sv_orthoContour_LineWidthTxt,
				   wxT(""), wxDefaultPosition, wxDefaultSize,
                                   wxTE_PROCESS_ENTER);
  sizerH->Add(m_pLineWidthTxt, 0, wxALIGN_LEFT|wxALL, 3);

  // anti-alias
  m_pAntiAliasChk = new wxCheckBox(this, MPP_Sv_orthoContour_AntiAliasChk,
				   wxT("anti-alias line"));
  topsizer->Add(m_pAntiAliasChk, 0, wxALL, 3);

  // post process
  SetSizer(topsizer);
  addTo(parent);
  topsizer->Layout();
  (void)update();
}

vsnMPP_Sv_orthoContour::~vsnMPP_Sv_orthoContour() {
}


/* interface */

bool vsnMPP_Sv_orthoContour::update() {
  if ( ! m_pSlicerTxt || ! m_pSlicerList || ! m_pSetSlicerBtn ||
       ! m_pSelDataLst || ! m_pVecDataChkLst || ! m_pNumLinesTxt ||
       ! m_pMinTxt || ! m_pMaxTxt || ! m_pUseCMapChk || ! m_pUpdMinMaxChk ||
       ! m_pLineWidthTxt || ! m_pAntiAliasChk || ! m_pBiLinearChk )
    return false;

  vsnMethod_Sv_orthoContour* pm
    = dynamic_cast<vsnMethod_Sv_orthoContour*>(p_method);
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
  m_pSlicerList->Clear();
  deque<vsnMethod_Sv_orthoSlicer*> slicerMtdLst
    = pm->getOrthoSlicerList(pdo);
  deque<vsnMethod_Sv_orthoSlicer*>::iterator it;
  for ( it = slicerMtdLst.begin(); it != slicerMtdLst.end(); it++ ) {
    if ( ! *it ) continue;
    string slicerName = (*it)->getName();
    if ( slicerName.empty() )
      m_pSlicerList->Append(wxT(VFR_NONAME));
    else
      m_pSlicerList->Append(vsnApp::ConvSysToWx(slicerName));
  } // end of for(it)

  // select scalar data
  register int i;
  char txt[64];
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

  // num lines
  size_t nl = pm->getNumLines();
  sprintf(txt, "%lu", nl); m_pNumLinesTxt->SetValue(vsnApp::ConvSysToWx(txt));

  // bilinear
  m_pBiLinearChk->SetValue(pm->getBiLinearMode());

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

void vsnMPP_Sv_orthoContour::settlement() {
  vsnGfxView* pgv = gfxAct_KI.getGfxView();
  if ( pgv ) {
    pgv->popActions(this);
    gfxAct_KI.setGfxView(NULL);
  }
}


/* event handler */

void vsnMPP_Sv_orthoContour::OnSlicerTxt(wxCommandEvent& event) {
  if ( ! m_pSlicerTxt ) return;

  vsnMethod_Sv_orthoContour* pm
    = dynamic_cast<vsnMethod_Sv_orthoContour*>(p_method);
  if ( ! pm ) return;
  vsnMethod_Sv_orthoSlicer* posr = pm->getOrthoSlicer();
  if ( ! posr ) return;
  vsnUiView* puiv = getUiView();
  if ( ! puiv ) return;

  puiv->selectObj((vfrNode*)posr);
}

void vsnMPP_Sv_orthoContour::OnSetSlicerBtn(wxCommandEvent& event) {
  if ( ! m_pSlicerList || ! m_pSetSlicerBtn || ! m_pSlicerTxt )
    return;

  int sel = m_pSlicerList->GetSelection();
  if ( sel < 0 ) return;
  m_pSlicerTxt->SetValue(m_pSlicerList->GetString(sel));

  vsnMethod_Sv_orthoContour* pm
    = dynamic_cast<vsnMethod_Sv_orthoContour*>(p_method);
  if ( ! pm ) return;
  vsnData_Sv* pdo = dynamic_cast<vsnData_Sv*>(pm->getRefData());
  if ( ! pdo ) return;

  pm->vsnRef_Sv_orthoSlicer::setOrthoSlicer(pdo, sel);
  pm->chkNotice();
}

void vsnMPP_Sv_orthoContour::OnSelDataLst(wxCommandEvent& event) {
  if ( ! m_pSelDataLst ) return;
  WhichDataType sel = event.GetInt();
  if ( sel < 0 ) return;

  wxString valStr = m_pSelDataLst->GetString(sel);
  if ( valStr == wxT("vector length") )
    sel = DATA_Veclen;

  vsnMethod_Sv_orthoContour* pm
    = dynamic_cast<vsnMethod_Sv_orthoContour*>(p_method);
  if ( ! pm ) return;
  if ( sel == pm->getSelectedData() ) return;

  if ( ! pm->setSelectedData(sel) ) {
    update();
    return;
  }
  pm->chkNotice();
}

void vsnMPP_Sv_orthoContour::OnVecDataChkLst(wxCommandEvent& event) {
  if ( ! m_pVecDataChkLst ) return;
  int dlen = m_pVecDataChkLst->GetCount();
  if ( dlen < 3 ) return;

  vsnMethod_Sv_orthoContour* pm
    = dynamic_cast<vsnMethod_Sv_orthoContour*>(p_method);
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

void vsnMPP_Sv_orthoContour::OnNumLinesTxt(wxCommandEvent& event) {
  if ( ! m_pNumLinesTxt ) return;

  vsnMethod_Sv_orthoContour* pm
    = dynamic_cast<vsnMethod_Sv_orthoContour*>(p_method);
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

void vsnMPP_Sv_orthoContour::OnBiLinearChk(wxCommandEvent& event) {
  if ( ! m_pBiLinearChk ) return;
  bool val = m_pBiLinearChk->GetValue();

  vsnMethod_Sv_orthoContour* pm
    = dynamic_cast<vsnMethod_Sv_orthoContour*>(p_method);
  if ( ! pm ) return;

  if ( pm->setBiLinearMode(val) )
    pm->chkNotice();
}

void vsnMPP_Sv_orthoContour::OnMinMaxTxt(wxCommandEvent& event) {
  if ( ! m_pMinTxt || ! m_pMaxTxt ) return;

  vsnMethod_Sv_orthoContour* pm
    = dynamic_cast<vsnMethod_Sv_orthoContour*>(p_method);
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

void vsnMPP_Sv_orthoContour::OnUseCMapChk(wxCommandEvent& event) {
  if ( ! m_pUseCMapChk ) return;
  bool val = m_pUseCMapChk->GetValue();

  vsnMethod_Sv_orthoContour* pm
    = dynamic_cast<vsnMethod_Sv_orthoContour*>(p_method);
  if ( ! pm ) return;

  if ( pm->setUseCMap(val) )
    pm->chkNotice();
}

void vsnMPP_Sv_orthoContour::OnUpdMinMaxChk(wxCommandEvent& event) {
  if ( ! m_pUpdMinMaxChk ) return;
  bool val = m_pUpdMinMaxChk->GetValue();

  vsnMethod_Sv_orthoContour* pm
    = dynamic_cast<vsnMethod_Sv_orthoContour*>(p_method);
  if ( ! pm ) return;

  if ( pm->setUpdateMinMaxMode(val) )
    pm->chkNotice();
}

void vsnMPP_Sv_orthoContour::OnLineWidthTxt(wxCommandEvent& event) {
  if ( ! m_pLineWidthTxt ) return;
  vsnMethod_Sv_orthoContour* pm
    = dynamic_cast<vsnMethod_Sv_orthoContour*>(p_method);
  if ( ! pm ) return;

  wxString valStr = m_pLineWidthTxt->GetValue();
  if ( valStr.IsEmpty() ) return;
  float val = (float)atof(vsnApp::ConvWxToSys(valStr).c_str());
  if ( val <= 0.f ) return;

  if ( pm->setLineWidth(val) )
    pm->chkNotice();
}

void vsnMPP_Sv_orthoContour::OnAntiAliasChk(wxCommandEvent& event) {
  if ( ! m_pAntiAliasChk ) return;
  vsnMethod_Sv_orthoContour* pm
    = dynamic_cast<vsnMethod_Sv_orthoContour*>(p_method);
  if ( ! pm ) return;

  bool val = m_pAntiAliasChk->GetValue();
  if ( pm->setAntiAliasMode(val) )
    pm->chkNotice();
}


//----------------------------------------------------------------
// class vsnMethod_Sv_orthoContour
//----------------------------------------------------------------

/* constructors / destructor */

vsnMethod_Sv_orthoContour::
vsnMethod_Sv_orthoContour(const std::string& name)
  : vsnMethodObj(name), m_selectedData(DATA_None), m_numLines(5),
    m_min(0.f), m_max(1.f), m_useCMap(true), m_updateMinMax(true),
    m_vecDataIdx(0,1,2), m_lineWidth(1.f), m_numValidLines(0),
    m_biLinear(true)
{
  m_useLut = m_useCMap;
}

vsnMethod_Sv_orthoContour::~vsnMethod_Sv_orthoContour() {
}


/* methods */

void vsnMethod_Sv_orthoContour::adjustRange() {
  bool needRangeUpd = false;

  vsnData_Sv* pData = dynamic_cast<vsnData_Sv*>(p_refData);
  if ( ! pData ) return;
  int dlen = (int)pData->getDataLen();

  float dr[2];
  if ( m_selectedData == DATA_Veclen ) {
    if ( m_updateMinMax )
      if ( pData->getVectorMaxLen(m_vecDataIdx, dr[1]) ) {
	m_lut.minVal = 0.f;
	m_lut.maxVal = dr[1];
	needRangeUpd = true;
      }
  } // end of if(DATA_Veclen)
  else if ( m_selectedData > 0 && m_selectedData <= dlen ) {
    if ( m_updateMinMax )
      if ( pData->getMinMax(m_selectedData -1, dr) ) {
	m_lut.minVal = dr[0];
	m_lut.maxVal = dr[1];
	needRangeUpd = true;
      }
  } // end of if(SD_SCALAR)

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

#define VIDX(i, j, k) (sliceIndices[sliceSz.x*(j)+(i)]*dlen +(k))
size_t
vsnMethod_Sv_orthoContour::countValidCells(const float val, const float* dptr,
					   const VFR::Point2& sliceSz,
					   const int* const sliceIndices,
					   const size_t dlen) const {
  if ( ! dptr || sliceSz.x * sliceSz.y < 4 || dlen < 1 ) return 0;

  register int i, j, k, cnt = 0;
  if ( m_selectedData == DATA_None )
    return 0;
  else if ( m_selectedData == DATA_Veclen ) {
    Vec3<float> v;
    for ( j = 0; j < sliceSz.y -1; j++ ) {
      for ( i = 0; i < sliceSz.x -1; i++ ) {
	k = 0;
	v = Vec3<float>(dptr[VIDX(i, j, m_vecDataIdx.m_v[0])],
			dptr[VIDX(i, j, m_vecDataIdx.m_v[1])],
			dptr[VIDX(i, j, m_vecDataIdx.m_v[2])]);
	if ( v.Length() > val ) k++;
	v = Vec3<float>(dptr[VIDX(i+1, j, m_vecDataIdx.m_v[0])],
			dptr[VIDX(i+1, j, m_vecDataIdx.m_v[1])],
			dptr[VIDX(i+1, j, m_vecDataIdx.m_v[2])]);
	if ( v.Length() > val ) k++;
	v = Vec3<float>(dptr[VIDX(i, j+1, m_vecDataIdx.m_v[0])],
			dptr[VIDX(i, j+1, m_vecDataIdx.m_v[1])],
			dptr[VIDX(i, j+1, m_vecDataIdx.m_v[2])]);
	if ( v.Length() > val ) k++;
	v = Vec3<float>(dptr[VIDX(i+1, j+1, m_vecDataIdx.m_v[0])],
			dptr[VIDX(i+1, j+1, m_vecDataIdx.m_v[1])],
			dptr[VIDX(i+1, j+1, m_vecDataIdx.m_v[2])]);
	if ( v.Length() > val ) k++;
	if ( k != 0 && k != 4 ) cnt++;
      } // end of for(i)
    } // end of for(j)
  } // end of DATA_Veclen
  else {
    register int vordr = m_selectedData - 1;
    for ( j = 0; j < sliceSz.y -1; j++ ) {
      for ( i = 0; i < sliceSz.x -1; i++ ) {
	k = 0;
	if ( dptr[VIDX(i, j, vordr)] > val ) k++;
	if ( dptr[VIDX(i+1, j, vordr)] > val ) k++;
	if ( dptr[VIDX(i, j+1, vordr)] > val ) k++;
	if ( dptr[VIDX(i+1, j+1, vordr)] > val ) k++;
	if ( k != 0 && k != 4 ) cnt++;
      } // end of for(i)
    } // end of for(j)
  } // end of datan
  return cnt;
}
#undef VIDX


// STATIC
bool vsnMethod_Sv_orthoContour::
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

// STATIC
void vsnMethod_Sv_orthoContour::
calcCP(const vector3 p1, const vector3 p2,
       const vector3 p3, const vector3 p4, vector3 cp) {
  //  p4+----+p2
  //    |    |    calc cp as cross point of (p1,p2) and (p3,p4)
  //  p1+----+p3
  float D = -(p2[0]-p1[0])*(p4[1]-p3[1]) + (p4[0]-p3[0])*(p2[1]-p1[1]);
  if ( fabs(D) < 1e-6 ) {
    cp[0] = (p1[0] + p2[0]) * 0.5f;
    cp[1] = (p1[1] + p2[1]) * 0.5f;
    cp[2] = (p1[2] + p2[2]) * 0.5f;
  } else {
    float s =(-(p3[0]-p1[0])*(p4[1]-p3[1]) + (p4[0]-p3[0])*(p3[1]-p1[1])) / D;
    if ( s < 0.f || s > 1.f ) {
      cp[0] = (p1[0] + p2[0]) * 0.5f;
      cp[1] = (p1[1] + p2[1]) * 0.5f;
      cp[2] = (p1[2] + p2[2]) * 0.5f;
    } else {
      cp[0] = p1[0] + (p2[0] - p1[0]) * s;
      cp[1] = p1[1] + (p2[1] - p1[1]) * s;
      cp[2] = p1[2] + (p2[2] - p1[2]) * s;
    }
  }
}


bool vsnMethod_Sv_orthoContour::setSelectedData(const WhichDataType sd) {
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

bool vsnMethod_Sv_orthoContour::setVecDataIdx(const CES::Vec3<int>& vdidx) {
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

bool vsnMethod_Sv_orthoContour::isValidVecData() const {
  vsnData_Sv* pData = dynamic_cast<vsnData_Sv*>(p_refData);
  if ( ! pData ) return false;

  size_t dlen = pData->getDataLen();
  if ( m_vecDataIdx[0] < 0 || m_vecDataIdx[0] >= dlen ) return false;
  if ( m_vecDataIdx[1] < 0 || m_vecDataIdx[1] >= dlen ) return false;
  if ( m_vecDataIdx[2] < 0 || m_vecDataIdx[2] >= dlen ) return false;
  return true;
}

bool vsnMethod_Sv_orthoContour::setNumLines(const size_t nl) {
  if ( m_numLines == nl ) return true;
  if ( nl < 1 ) return false;
  m_numLines = nl;

  if ( ! update() ) return false;
  updateUI();
  return true;
}

bool vsnMethod_Sv_orthoContour::setBiLinearMode(const bool blm) {
  if ( m_biLinear == blm ) return true;
  m_biLinear = blm;
  
  if ( ! update() ) return false;
  updateUI();
  return true;
}

bool vsnMethod_Sv_orthoContour::setRange(const float min, const float max) {
  if ( m_min == min && m_max == max ) return true;
  if ( min > max ) return false;
  m_min = min; m_max = max;

  if ( ! update() ) return false;
  updateUI();
  return true;
}

bool vsnMethod_Sv_orthoContour::setUseCMap(const bool ucm) {
  if ( m_useCMap == ucm ) return true;
  m_useCMap = ucm;

  setUseLut(m_useCMap);
  updateUI();
  return true;
}

bool vsnMethod_Sv_orthoContour::setUpdateMinMaxMode(const bool mode) {
  if ( m_updateMinMax == mode ) return true;
  m_updateMinMax = mode;

  // data range
  adjustRange();

  if ( ! update() ) return false;
  updateUI();
  return true;
}

bool vsnMethod_Sv_orthoContour::setLineWidth(const float lw) {
  if ( m_lineWidth == lw ) return true;
  if ( lw <= 0.f ) return false;
  m_lineWidth = lw;

  if ( _material )
    _material->setLineWidth(m_lineWidth);

  updateUI();
  return true;
}


/* vsnTimeSeriesMethodIF methods */

#define IDX(i, j) (sliceIndices[sliceSize.x*(j)+(i)])

bool vsnMethod_Sv_orthoContour::updateStep(const int stp,
					   const bool force,
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

  // lines data
  register int i, j, k, idx;
  int n = getNumChildren();
  if ( m_numLines > n ) {
    for ( i = n; i < m_numLines; i++ ) {
      vfrLines* plns = new vfrLines("contour_line");
      if ( ! plns ) {
	ErrMsg(MsgERR, getMethodType() + string("[") + getName()
	       + string("]: memory allocation failed"));
	return false;
      }
      addChild(plns);
    } // end of for(i)
  }

  // invalidate all lines
  m_numValidLines = 0;
  for ( i = 0; i < n; i++ ) {
    vfrNode* p = getChild(i);
    if ( ! p ) continue;
    p->alcVerts(0);
    p->notice();
  } // end of for(i)

  getPrivateMaterial()->setRenderMode(RT_NONE);
  if ( ! m_show ) {
    return true;
  }

  // check slicer
  if ( ! p_osr ) return true;
  Point2 sliceSize = p_osr->getSliceSize();
  size_t sliceSz = sliceSize.x * sliceSize.y;
  if ( sliceSz < 4 ) return true;
  const int* const sliceIndices = p_osr->getSliceIndices();
  if ( ! sliceIndices ) {
    ErrMsg(MsgERR, getMethodType() + string("[") + getName()
           + string("]: can't get slice indices data"));
    return false;
  }

  // check selected data
  size_t dlen = pData->getDataLen();
  if ( m_selectedData == DATA_None ) return true;
  else if ( m_selectedData == DATA_Veclen ) {
    if ( ! isValidVecData() ) return true;
  }
  else if ( m_selectedData > dlen ) return true;

  // get Sv data
  const float* gptr = pData->getCoord(m_requestedStp);
  if ( ! gptr ) {
    return false;
  }
  const float* dptr = pData->getData(m_requestedStp);
  if ( ! dptr ) {
    return false;
  }
  const unsigned char* maskptr = pData->getMask();

  // calc lines
  vector3 lp1, lp2;
  register float val;
  register float dv = (m_numLines < 2) ? 0.f
    : (m_max - m_min)/(float)(m_numLines - 1);

  for ( val = m_min, k = 0; k < m_numLines; val += dv, k++ ) {
    vfrLines* plns = dynamic_cast<vfrLines*>(getChild(k));
    if ( ! plns ) continue;

    // alloc lines' data
    size_t maxLineSegs
      = countValidCells(val, dptr, sliceSize, sliceIndices, dlen) * 2;
    if ( maxLineSegs == 0 ) continue;
    if ( m_biLinear ) maxLineSegs *= 2;
    vfrNode* p = getChild(k);
    if ( ! p->alcPools(maxLineSegs * 2, -1, -1, -1) ) {
      ErrMsg(MsgERR, getMethodType() + string("[") + getName()
	     + string("]: memory allocation failed"));
      return false;
    }

    idx = 0;
    float vv[5]; vector3 vc;
    if ( m_selectedData == DATA_Veclen ) {
      for ( j = 0; j < sliceSize.y -1; j++ ) {
	for ( i = 0; i < sliceSize.x -1; i++ ) {
	  if ( maskptr &&
	       (maskptr[IDX(i,j)] || maskptr[IDX(i+1,j)] ||
		maskptr[IDX(i,j+1)] || maskptr[IDX(i+1,j+1)]) ) continue;

	  vv[0] = Vec3<float>(dptr[IDX(i,j)*dlen +m_vecDataIdx.m_v[0]],
		      dptr[IDX(i,j)*dlen +m_vecDataIdx.m_v[1]],
		      dptr[IDX(i,j)*dlen +m_vecDataIdx.m_v[2]]).Length();
	  vv[1] = Vec3<float>(dptr[IDX(i+1,j)*dlen +m_vecDataIdx.m_v[0]],
		      dptr[IDX(i+1,j)*dlen +m_vecDataIdx.m_v[1]],
		      dptr[IDX(i+1,j)*dlen +m_vecDataIdx.m_v[2]]).Length();
	  vv[2] = Vec3<float>(dptr[IDX(i+1,j+1)*dlen +m_vecDataIdx.m_v[0]],
		      dptr[IDX(i+1,j+1)*dlen +m_vecDataIdx.m_v[1]],
		      dptr[IDX(i+1,j+1)*dlen +m_vecDataIdx.m_v[2]]).Length();
	  vv[3] = Vec3<float>(dptr[IDX(i,j+1)*dlen +m_vecDataIdx.m_v[0]],
		      dptr[IDX(i,j+1)*dlen +m_vecDataIdx.m_v[1]],
		      dptr[IDX(i,j+1)*dlen +m_vecDataIdx.m_v[2]]).Length();

	  if ( m_biLinear ) {
	    vv[4] = (vv[0] + vv[1] + vv[2] +vv[3]) * 0.25f;
#if 0
	    calcCP(&gptr[IDX(i,j)*3], &gptr[IDX(i+1,j+1)*3],
		   &gptr[IDX(i+1,j)*3], &gptr[IDX(i,j+1)*3], vc);
#else
	    vc[0] = (gptr[IDX(i,j)*3] +gptr[IDX(i+1,j)*3]
		     +gptr[IDX(i+1,j+1)*3] +gptr[IDX(i,j+1)*3]) * 0.25f;
	    vc[1] = (gptr[IDX(i,j)*3 +1] +gptr[IDX(i+1,j)*3 +1]
		     +gptr[IDX(i+1,j+1)*3 +1] +gptr[IDX(i,j+1)*3 +1]) * 0.25f;
	    vc[2] = (gptr[IDX(i,j)*3 +2] +gptr[IDX(i+1,j)*3 +2]
		     +gptr[IDX(i+1,j+1)*3 +2] +gptr[IDX(i,j+1)*3 +2]) * 0.25f;
#endif
	    // triangle(0, 1, C)
	    if ( calcLineSegment(&gptr[IDX(i,j)*3], &gptr[IDX(i+1,j)*3], vc,
				 vv[0], vv[1], vv[4], val, lp1, lp2) ) {
	      plns->alcVerts(idx + 2);
	      plns->setVert(idx++, lp1, FALSE);
	      plns->setVert(idx++, lp2, FALSE);
	    }

	    // triangle(1, 2, C)
	    if ( calcLineSegment(&gptr[IDX(i+1,j)*3],&gptr[IDX(i+1,j+1)*3],vc,
				 vv[1], vv[2], vv[4], val, lp1, lp2) ) {
	      plns->alcVerts(idx + 2);
	      plns->setVert(idx++, lp1, FALSE);
	      plns->setVert(idx++, lp2, FALSE);
	    }

	    // triangle(2, 3, C)
	    if ( calcLineSegment(&gptr[IDX(i+1,j+1)*3],&gptr[IDX(i,j+1)*3],vc,
				 vv[2], vv[3], vv[4], val, lp1, lp2) ) {
	      plns->alcVerts(idx + 2);
	      plns->setVert(idx++, lp1, FALSE);
	      plns->setVert(idx++, lp2, FALSE);
	    }

	    // triangle(3, 0, C)
	    if ( calcLineSegment(&gptr[IDX(i,j+1)*3], &gptr[IDX(i,j)*3], vc,
				 vv[3], vv[0], vv[4], val, lp1, lp2) ) {
	      plns->alcVerts(idx + 2);
	      plns->setVert(idx++, lp1, FALSE);
	      plns->setVert(idx++, lp2, FALSE);
	    }
	  }
	  else {
	    // lower triangle(0, 1, 2)
	    if ( calcLineSegment(&gptr[IDX(i,j)*3],
				 &gptr[IDX(i+1,j)*3], &gptr[IDX(i+1,j+1)*3],
				 vv[0], vv[1], vv[2], val, lp1, lp2) ) {
	      plns->alcVerts(idx + 2);
	      plns->setVert(idx++, lp1, FALSE);
	      plns->setVert(idx++, lp2, FALSE);
	    }

	    // upper triangle(0, 2, 3)
	    if ( calcLineSegment(&gptr[IDX(i,j)*3],
				 &gptr[IDX(i+1,j+1)*3], &gptr[IDX(i,j+1)*3],
				 vv[0], vv[2], vv[3], val, lp1, lp2) ) {
	      plns->alcVerts(idx + 2);
	      plns->setVert(idx++, lp1, FALSE);
	      plns->setVert(idx++, lp2, FALSE);
	    }
	  }
        } // end of for(i)
      } // end of for(j)
    } // end of DATA_Veclen
    else {
      register int vordr = m_selectedData - 1;

      for ( j = 0; j < sliceSize.y -1; j++ ) {
	for ( i = 0; i < sliceSize.x -1; i++ ) {
	  if ( maskptr &&
	       (maskptr[IDX(i,j)] || maskptr[IDX(i+1,j)] ||
		maskptr[IDX(i,j+1)] || maskptr[IDX(i+1,j+1)]) ) continue;

	  vv[0] = dptr[IDX(i,j)*dlen + vordr];
	  vv[1] = dptr[IDX(i+1,j)*dlen + vordr];
	  vv[2] = dptr[IDX(i+1,j+1)*dlen + vordr];
	  vv[3] = dptr[IDX(i,j+1)*dlen + vordr];

	  if ( m_biLinear ) {
	    vv[4] = (vv[0] + vv[1] + vv[2] +vv[3]) * 0.25f;
#if 0
	    calcCP(&gptr[IDX(i,j)*3], &gptr[IDX(i+1,j+1)*3],
		   &gptr[IDX(i+1,j)*3], &gptr[IDX(i,j+1)*3], vc);
#else
	    vc[0] = (gptr[IDX(i,j)*3] +gptr[IDX(i+1,j)*3]
		     +gptr[IDX(i+1,j+1)*3] +gptr[IDX(i,j+1)*3]) * 0.25f;
	    vc[1] = (gptr[IDX(i,j)*3 +1] +gptr[IDX(i+1,j)*3 +1]
		     +gptr[IDX(i+1,j+1)*3 +1] +gptr[IDX(i,j+1)*3 +1]) * 0.25f;
	    vc[2] = (gptr[IDX(i,j)*3 +2] +gptr[IDX(i+1,j)*3 +2]
		     +gptr[IDX(i+1,j+1)*3 +2] +gptr[IDX(i,j+1)*3 +2]) * 0.25f;
#endif
	    // triangle(0, 1, C)
	    if ( calcLineSegment(&gptr[IDX(i,j)*3], &gptr[IDX(i+1,j)*3], vc,
				 vv[0], vv[1], vv[4], val, lp1, lp2) ) {
	      plns->alcVerts(idx + 2);
	      plns->setVert(idx++, lp1, FALSE);
	      plns->setVert(idx++, lp2, FALSE);
	    }

	    // triangle(1, 2, C)
	    if ( calcLineSegment(&gptr[IDX(i+1,j)*3],&gptr[IDX(i+1,j+1)*3],vc,
				 vv[1], vv[2], vv[4], val, lp1, lp2) ) {
	      plns->alcVerts(idx + 2);
	      plns->setVert(idx++, lp1, FALSE);
	      plns->setVert(idx++, lp2, FALSE);
	    }

	    // triangle(2, 3, C)
	    if ( calcLineSegment(&gptr[IDX(i+1,j+1)*3],&gptr[IDX(i,j+1)*3],vc,
				 vv[2], vv[3], vv[4], val, lp1, lp2) ) {
	      plns->alcVerts(idx + 2);
	      plns->setVert(idx++, lp1, FALSE);
	      plns->setVert(idx++, lp2, FALSE);
	    }

	    // triangle(3, 0, C)
	    if ( calcLineSegment(&gptr[IDX(i,j+1)*3], &gptr[IDX(i,j)*3], vc,
				 vv[3], vv[0], vv[4], val, lp1, lp2) ) {
	      plns->alcVerts(idx + 2);
	      plns->setVert(idx++, lp1, FALSE);
	      plns->setVert(idx++, lp2, FALSE);
	    }
	  }
	  else {
	    // lower triangle(0, 1, 2)
	    if ( calcLineSegment(&gptr[IDX(i,j)*3],
				 &gptr[IDX(i+1,j)*3], &gptr[IDX(i+1,j+1)*3],
				 vv[0], vv[1], vv[2], val, lp1, lp2) ) {
	      plns->alcVerts(idx + 2);
	      plns->setVert(idx++, lp1, FALSE);
	      plns->setVert(idx++, lp2, FALSE);
	    }

	    // upper triangle(0, 2, 3)
	    if ( calcLineSegment(&gptr[IDX(i,j)*3],
				 &gptr[IDX(i+1,j+1)*3], &gptr[IDX(i,j+1)*3],
				 vv[0], vv[2], vv[3], val, lp1, lp2) ) {
	      plns->alcVerts(idx + 2);
	      plns->setVert(idx++, lp1, FALSE);
	      plns->setVert(idx++, lp2, FALSE);
	    }
	  }
        } // end of for(i)
      } // end of for(j)
    } // end of datan

    // lines' attr
    if ( m_useCMap )
      plns->setColor3(0, &m_lut.lutEntry[m_lut.getValIdx(val) * 4]);
    else
      plns->setColor4(0, m_colour);
    plns->setTransparency(m_antiAlias);
  } // end of for(val, k)

  // ok
  m_numValidLines = m_numLines;
  getPrivateMaterial()->setRenderMode(RT_WIRE);
  m_updatedStp = m_requestedStp;
  return true;
}
#undef IDX


/* vsnMethodObj methods */

bool vsnMethod_Sv_orthoContour::update(const bool force) {
  return updateStep(m_requestedStp, force);
}

void vsnMethod_Sv_orthoContour::reloaded() {
  adjustRange();
  vsnMethodObj::reloaded();
}

vsnMethodPP* vsnMethod_Sv_orthoContour::getParamPanel(wxPanel* pp) {
  vsnMPP_Sv_orthoContour* pp_orthoContour
    = new vsnMPP_Sv_orthoContour(pp, this);
  if ( ! pp_orthoContour ) return NULL;
  return pp_orthoContour;
}

void vsnMethod_Sv_orthoContour::setUseLut(const bool ulm) {
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

void vsnMethod_Sv_orthoContour::setLut(const vsnLut& ol) {
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


/* serialize : from vsnIoObject */

bool vsnMethod_Sv_orthoContour::parseXML(xmlNodePtr xnp) {
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
      else if ( xsN == string("use_data") ) {
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
      else if ( xsN == string("num_lines") ) {
        int nl = atoi(xsV.c_str());
        if ( ! setNumLines(nl) ) {
          ErrMsg(MsgERR, msgHdr + string("invalid value in param num_lines"));
          goto _NEXT_XML_NODE;
        }
      } // end of "num_lines"
      else if ( xsN == string("bilinear") ) {
        bool blm;
        if ( xsV == string("yes") ) blm = true;
        else if ( xsV == string("no") ) blm = false;
        else {
          ErrMsg(MsgERR, msgHdr + string("invalid value in param bilinear"));
          goto _NEXT_XML_NODE;
        }
        if ( ! setBiLinearMode(blm) ) {
          ErrMsg(MsgERR, msgHdr + string("can't update bilinear mode"));
          goto _NEXT_XML_NODE;
        }
      } // end of "bilinear"
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
          ErrMsg(MsgERR, msgHdr + string("can't update upd_minmax mode"));
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

bool vsnMethod_Sv_orthoContour::outputXML(std::ostream& os,
					      const size_t ts) {
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
  if ( ! m_updateMinMax ) {
    os << idts_2 << "<param name=\"upd_minmax\" value=\"no\" />" << endl;
    os << idts_2 << "<param name=\"data_range\" value=\""
       << m_min << ':' << m_max << "\" />" << endl;
  }

  // ortho_slicer
  if ( p_osr ) {
    if ( ! vsnRef_Sv_orthoSlicer::exportXMLNode(os, ts+2) ) {
      ErrMsg(MsgWARN, msgHdr
	     + string("ortho_slice has set, but the slicer has no name,\n")
	     + string("so don't output ortho_slicer param node"));
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

  // bilinear
  if ( m_biLinear ) {
    os << idts_2 << "<param name=\"bilinear\" value=\"yes\" />" << endl;
  } else {
    os << idts_2 << "<param name=\"bilinear\" value=\"no\" />" << endl;
  }

  // upd_minmax, data_range
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
  return ret;
}

bool vsnMethod_Sv_orthoContour::commandXML(xmlNodePtr xnp) {
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
  else if ( nameStr == "set_bilinear" ) {
    bool blm;
    if ( valueStr == string("yes") ) blm = true;
    else if ( valueStr == string("no") ) blm = false;
    else {
      ErrMsg(MsgERR, msgHdr + string("command set_bilinear: invalid value"));
      return false;
    }
    if ( ! setBiLinearMode(blm) ) {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_bilinear: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_bilinear"
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


/* from vsnRef_Sv_orthoSlicer */

void vsnMethod_Sv_orthoContour::noticeUpdate() {
  update();
}
