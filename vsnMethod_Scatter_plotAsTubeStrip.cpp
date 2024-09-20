//
// vsnMethod_Scatter_plotAsTubeStrip
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

#include "vsnMethod_Scatter_plotAsTubeStrip.h"
#include "vsnUiView.h"
#include "vsnError.h"
#include "vsnOctTree.h" // for decomp vecIdx
#include "vsnExtrude.h"

using namespace std;
using namespace CES;
using namespace VSN;


//----------------------------------------------------------------
// class vsnMPP_Scatter_plotAsTubeStrip
//----------------------------------------------------------------
BEGIN_EVENT_TABLE(vsnMPP_Scatter_plotAsTubeStrip, wxPanel)
  EVT_TEXT_ENTER(MPP_Scatter_plotAsTubeStrip_RadiusBiasTxt,
                 vsnMPP_Scatter_plotAsTubeStrip::OnRadiusBiasTxt)
  EVT_COMBOBOX(MPP_Scatter_plotAsTubeStrip_SelColDataLst,
               vsnMPP_Scatter_plotAsTubeStrip::OnSelColDataLst)
  EVT_COMBOBOX(MPP_Scatter_plotAsTubeStrip_SelRadDataLst,
               vsnMPP_Scatter_plotAsTubeStrip::OnSelRadDataLst)
  EVT_CHECKLISTBOX(MPP_Scatter_plotAsTubeStrip_VecDataChkLst,
                   vsnMPP_Scatter_plotAsTubeStrip::OnVecDataChkLst)
  EVT_CHECKBOX(MPP_Scatter_plotAsTubeStrip_UpdMinMaxChk,
               vsnMPP_Scatter_plotAsTubeStrip::OnUpdMinMaxChk)
  EVT_CHECKBOX(MPP_Scatter_plotAsTubeStrip_SplitWithNVChk,
               vsnMPP_Scatter_plotAsTubeStrip::OnSplitWithNVChk)
END_EVENT_TABLE()


/* constructors / destructor */

vsnMPP_Scatter_plotAsTubeStrip::
vsnMPP_Scatter_plotAsTubeStrip(wxPanel* parent, vsnMethodObj* pm)
: vsnMethodPP(parent, pm)
{
  assert(parent);
  assert(dynamic_cast<vsnMethod_Scatter_plotAsTubeStrip*>(pm));

  // create widgets
  m_pRadiusBiasTxt
    = new wxTextCtrl(this, MPP_Scatter_plotAsTubeStrip_RadiusBiasTxt,
		     wxT(""), wxDefaultPosition, wxDefaultSize,
		     wxTE_PROCESS_ENTER);
  assert(m_pRadiusBiasTxt);

  m_pSelColDataLst
    = new wxComboBox(this, MPP_Scatter_plotAsTubeStrip_SelColDataLst,
		     wxT(""), wxDefaultPosition, wxDefaultSize,
		     0, NULL, wxCB_READONLY);
  assert(m_pSelColDataLst);

  m_pSelRadDataLst
    = new wxComboBox(this, MPP_Scatter_plotAsTubeStrip_SelRadDataLst,
		     wxT(""), wxDefaultPosition, wxDefaultSize,
		     0, NULL, wxCB_READONLY);
  assert(m_pSelRadDataLst);

  m_pVecDataChkLst
    = new wxCheckListBox(this, MPP_Scatter_plotAsTubeStrip_VecDataChkLst);
  assert(m_pVecDataChkLst);

  m_pUpdMinMaxChk
    = new wxCheckBox(this, MPP_Scatter_plotAsTubeStrip_UpdMinMaxChk,
		     wxT("update lut minmax"));
  assert(m_pUpdMinMaxChk);

  m_pSplitWithNVChk
    = new wxCheckBox(this, MPP_Scatter_plotAsTubeStrip_SplitWithNVChk,
                    wxT("split with NV points"));
  assert(m_pSplitWithNVChk);
 
  // prepare sizers
  wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL); assert(topsizer);
  wxBoxSizer* sizerH;

  // radius bias
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxALL, 3);
  sizerH->Add(new wxStaticText(this, -1, wxT("radius bias")),
              0, wxALIGN_LEFT|wxALL, 3);
  sizerH->Add(m_pRadiusBiasTxt, 0, wxALIGN_LEFT|wxALL, 3);

  // data selector
  topsizer->Add(new wxStaticText(this, -1,
				 wxT("select scalar data for color")),
                0, wxALIGN_LEFT|wxALL, 3);
  topsizer->Add(m_pSelColDataLst, 0, wxEXPAND|wxALL, 3);

  topsizer->Add(new wxStaticText(this, -1,
				 wxT("select scalar data for radius")),
                0, wxALIGN_LEFT|wxALL, 3);
  topsizer->Add(m_pSelRadDataLst, 0, wxEXPAND|wxALL, 3);

  // vector data selector
  topsizer->Add(new wxStaticText(this, -1, wxT("vector components")),
                0, wxALIGN_LEFT|wxALL, 3);
  topsizer->Add(m_pVecDataChkLst, 0, wxEXPAND|wxALL, 3);

  // update minmax mode
  m_pUpdMinMaxChk->SetValue(TRUE);
  topsizer->Add(m_pUpdMinMaxChk, 0, wxALL, 3);

  // split with NV
  topsizer->Add(m_pSplitWithNVChk, 0, wxALL, 3);
 
  // post process
  SetSizer(topsizer);
  addTo(parent);
  topsizer->Layout();
  (void)update();
}

vsnMPP_Scatter_plotAsTubeStrip::~vsnMPP_Scatter_plotAsTubeStrip() {
}


/* interface */

bool vsnMPP_Scatter_plotAsTubeStrip::update() {
  register size_t i;
  char txt[64];

  vsnMethod_Scatter_plotAsTubeStrip* pm
    = dynamic_cast<vsnMethod_Scatter_plotAsTubeStrip*>(p_method);
  if ( ! pm ) return false;
  vsnNumericalDataIF* pdo
    = dynamic_cast<vsnNumericalDataIF*>(pm->getRefData());
  if ( ! pdo ) return false;

  // vector data indices
  int dlen = (int)pdo->getDataLen();
  m_pVecDataChkLst->Clear();
  if ( dlen >= 0 ) {
    for ( i = 0; i < dlen; i++ ) {
      sprintf(txt, "data%zd", i);
      m_pVecDataChkLst->Append(vsnApp::ConvSysToWx(txt));
    } // end of for(i)
    Vec3<int> vidx = pm->getVecDataIdx();
    if ( vidx[0] >= 0 && vidx[0] < dlen ) m_pVecDataChkLst->Check(vidx[0]);
    if ( vidx[1] >= 0 && vidx[1] < dlen ) m_pVecDataChkLst->Check(vidx[1]);
    if ( vidx[2] >= 0 && vidx[2] < dlen ) m_pVecDataChkLst->Check(vidx[2]);
  }

  // select scalar data for color
  if ( m_pSelColDataLst->GetCount() < 1 ) {
    m_pSelColDataLst->Append(wxT("None"));
    if ( dlen > 0 ) {
      for ( i = 0; i < dlen; i++ ) {
        sprintf(txt, "data%zd", i);
        m_pSelColDataLst->Append(vsnApp::ConvSysToWx(txt));
      } // end of for(i)
      if ( dlen >= 3 )
        m_pSelColDataLst->Append(wxT("vector length"));
    }
  }
  WhichDataType selColData = pm->getSelectedColData();
  if ( selColData >= 0 && selColData <= dlen )
    m_pSelColDataLst->SetSelection(selColData);
  else if ( selColData == DATA_Veclen && dlen >= 3 )
    m_pSelColDataLst->SetSelection(dlen + 1);
  else
    m_pSelColDataLst->SetSelection(0);

  // select scalar data for radius
  if ( m_pSelRadDataLst->GetCount() < 1 ) {
    m_pSelRadDataLst->Append(wxT("Const(1.0)"));
    if ( dlen > 0 ) {
      for ( i = 0; i < dlen; i++ ) {
        sprintf(txt, "data%zd", i);
        m_pSelRadDataLst->Append(vsnApp::ConvSysToWx(txt));
      } // end of for(i)
      if ( dlen >= 3 )
        m_pSelRadDataLst->Append(wxT("vector length"));
    }
  }
  WhichDataType selRadData = pm->getSelectedRadData();
  if ( selRadData >= 0 && selRadData <= dlen )
    m_pSelRadDataLst->SetSelection(selRadData);
  else if ( selRadData == DATA_Veclen && dlen >= 3 )
    m_pSelRadDataLst->SetSelection(dlen + 1);
  else
    m_pSelRadDataLst->SetSelection(0);

  // update minmax mode
  m_pUpdMinMaxChk->SetValue(pm->getUpdateMinMaxMode());

  // split with NV
  m_pSplitWithNVChk->SetValue(pm->getSplitWithNV());

  // radius bias
  float r = pm->getRadiusBias();
  sprintf(txt, "%g", r);
  m_pRadiusBiasTxt->SetValue(vsnApp::ConvSysToWx(txt));  

  return true;
}


/* event handler */

void vsnMPP_Scatter_plotAsTubeStrip::OnRadiusBiasTxt(wxCommandEvent& event) {
  vsnMethod_Scatter_plotAsTubeStrip* pm
    = dynamic_cast<vsnMethod_Scatter_plotAsTubeStrip*>(p_method);
  if ( ! pm ) return;

  wxString valStr = m_pRadiusBiasTxt->GetValue();
  if ( valStr.IsEmpty() ) return;
  float value = (float)atof(vsnApp::ConvWxToSys(valStr).c_str());
  if ( value < 0.001f ) value = 0.001f;

  if ( pm->setRadiusBias(value) )
    pm->chkNotice();
}

void
vsnMPP_Scatter_plotAsTubeStrip::OnVecDataChkLst(wxCommandEvent& event) {
  if ( ! m_pVecDataChkLst ) return;
  int dlen = m_pVecDataChkLst->GetCount();
  if ( dlen < 3 ) return;

  vsnMethod_Scatter_plotAsTubeStrip* pm
    = dynamic_cast<vsnMethod_Scatter_plotAsTubeStrip*>(p_method);
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

void vsnMPP_Scatter_plotAsTubeStrip::OnSelColDataLst(wxCommandEvent& event) {
  if ( ! m_pSelColDataLst ) return;
  WhichDataType sel = event.GetInt();
  if ( sel < 0 ) return;

  wxString valStr = m_pSelColDataLst->GetString(sel);
  if ( valStr == wxT("vector length") )
    sel = DATA_Veclen;

  vsnMethod_Scatter_plotAsTubeStrip* pm
    = dynamic_cast<vsnMethod_Scatter_plotAsTubeStrip*>(p_method);
  if ( ! pm ) return;
  if ( sel == pm->getSelectedColData() ) return;

  if ( ! pm->setSelectedColData(sel) ) {
    update();
    return;
  }
  pm->chkNotice();
}

void vsnMPP_Scatter_plotAsTubeStrip::OnSelRadDataLst(wxCommandEvent& event) {
  if ( ! m_pSelRadDataLst ) return;
  WhichDataType sel = event.GetInt();
  if ( sel < 0 ) return;

  wxString valStr = m_pSelRadDataLst->GetString(sel);
  if ( valStr == wxT("vector length") )
    sel = DATA_Veclen;

  vsnMethod_Scatter_plotAsTubeStrip* pm
    = dynamic_cast<vsnMethod_Scatter_plotAsTubeStrip*>(p_method);
  if ( ! pm ) return;
  if ( sel == pm->getSelectedRadData() ) return;

  if ( ! pm->setSelectedRadData(sel) ) {
    update();
    return;
  }
  pm->chkNotice();
}

void vsnMPP_Scatter_plotAsTubeStrip::OnUpdMinMaxChk(wxCommandEvent& event) {
  if ( ! m_pUpdMinMaxChk ) return;
  bool val = m_pUpdMinMaxChk->GetValue();

  vsnMethod_Scatter_plotAsTubeStrip* pm
    = dynamic_cast<vsnMethod_Scatter_plotAsTubeStrip*>(p_method);
  if ( ! pm ) return;

  if ( pm->setUpdateMinMaxMode(val) )
    pm->chkNotice();
}

void vsnMPP_Scatter_plotAsTubeStrip::OnSplitWithNVChk(wxCommandEvent& event)
{
  if ( ! m_pSplitWithNVChk ) return;
  vsnMethod_Scatter_plotAsTubeStrip* pm
    = dynamic_cast<vsnMethod_Scatter_plotAsTubeStrip*>(p_method);
  if ( ! pm ) return;

  bool val = m_pSplitWithNVChk->GetValue();
  if ( pm->setSplitWithNV(val) )
    pm->chkNotice();
}


//----------------------------------------------------------------
// class vsnMethod_Scatter_plotAsTubeStrip
//----------------------------------------------------------------

/* constructors / destructor */

vsnMethod_Scatter_plotAsTubeStrip::
vsnMethod_Scatter_plotAsTubeStrip(const string& name)
: vsnMethodObj(name),
  m_selectedColData(DATA_None), m_selectedRadData(DATA_None),
  m_vecDataIdx(0,1,2), m_updateMinMax(true), m_radiusBias(1.f),
  m_splitWithNV(true), m_shape(NULL)
{
  m_showType = RT_SMOOTH;
}

vsnMethod_Scatter_plotAsTubeStrip::~vsnMethod_Scatter_plotAsTubeStrip() {
  if ( m_shape ) {
    m_shape->remAllChildren();
    delete m_shape;
    m_shape = NULL;
  }
}


/* methods */

void vsnMethod_Scatter_plotAsTubeStrip::adjustRange() {
  bool needRangeUpd = false;

  vsnNumericalDataIF* pData = dynamic_cast<vsnNumericalDataIF*>(p_refData);
  if ( ! pData ) return;
  int dlen = (int)pData->getDataLen();

  float dr[2];
  if ( m_selectedColData == DATA_Veclen ) {
    CES::Vec3<int> vdl = m_vecDataIdx;
    if ( pData->getVectorMaxLen(vdl, dr[1]) ) {
      if ( m_updateMinMax ) {
        m_lut.minVal = 0.f;
        m_lut.maxVal = dr[1];
        needRangeUpd = true;
      }
    }
    setUseLut(true);
  } // end of if(DATA_Veclen)
  else if ( m_selectedColData > 0 && m_selectedColData <= dlen ) {
    if ( pData->getMinMax(m_selectedColData -1, dr) ) {
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

bool vsnMethod_Scatter_plotAsTubeStrip::setRadiusBias(const float rb) {
  if ( m_radiusBias == rb ) return true;
  m_radiusBias = rb;
  if ( ! update() ) return false;
  updateUI();
  return true;
}

bool
vsnMethod_Scatter_plotAsTubeStrip::setVecDataIdx(const Vec3<int>& vdidx) {
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

bool
vsnMethod_Scatter_plotAsTubeStrip::setSelectedColData(const WhichDataType sd)
{
  if ( sd == m_selectedColData ) return true;

  vsnNumericalDataIF* pData = dynamic_cast<vsnNumericalDataIF*>(p_refData);
  if ( ! pData ) return false;

  int dlen = (int)pData->getDataLen();
  if ( sd > dlen ) return false;
  if ( sd == DATA_Veclen && dlen < 3 ) return false;
  m_selectedColData = sd;

  // data range
  adjustRange();

  if ( ! update() ) return false;
  updateUI();
  return true;
}

bool
vsnMethod_Scatter_plotAsTubeStrip::setSelectedRadData(const WhichDataType sd)
{
  if ( sd == m_selectedRadData ) return true;

  vsnNumericalDataIF* pData = dynamic_cast<vsnNumericalDataIF*>(p_refData);
  if ( ! pData ) return false;

  int dlen = (int)pData->getDataLen();
  if ( sd > dlen ) return false;
  if ( sd == DATA_Veclen && dlen < 3 ) return false;
  m_selectedRadData = sd;

  if ( ! update() ) return false;
  updateUI();
  return true;
}

bool vsnMethod_Scatter_plotAsTubeStrip::isValidVecData() const {
  vsnNumericalDataIF* pData = dynamic_cast<vsnNumericalDataIF*>(p_refData);
  if ( ! pData ) return false;

  size_t dlen = pData->getDataLen();
  if ( m_vecDataIdx[0] < 0 || m_vecDataIdx[0] >= dlen ) return false;
  if ( m_vecDataIdx[1] < 0 || m_vecDataIdx[1] >= dlen ) return false;
  if ( m_vecDataIdx[2] < 0 || m_vecDataIdx[2] >= dlen ) return false;
  return true;
}

bool
vsnMethod_Scatter_plotAsTubeStrip::setUpdateMinMaxMode(const bool mode) {
  if ( mode == m_updateMinMax ) return true;
  m_updateMinMax = mode;

  // data range
  if ( m_updateMinMax ) {
    vsnNumericalDataIF* pData
      = dynamic_cast<vsnNumericalDataIF*>(p_refData);
    if ( pData ) {
      float dr[2];
      if ( m_selectedColData == DATA_Veclen ) {
	CES::Vec3<int> vdl = m_vecDataIdx;
        if ( pData->getVectorMaxLen(vdl, dr[1]) ) {
          m_lut.minVal = 0.f;
          m_lut.maxVal = dr[1];
        }
      } // end of if(DATA_Veclen)
      else if ( m_selectedColData > 0 &&
		m_selectedColData <= pData->getDataLen() ) {
        if ( pData->getMinMax(m_selectedColData -1, dr) ) {
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

bool vsnMethod_Scatter_plotAsTubeStrip::setSplitWithNV(const bool split) {
  if ( m_splitWithNV == split ) return true;
  m_splitWithNV = split;

  if ( ! update() ) return false;
  updateUI();
  return true;
}


/* from vsnMethodObj */

bool vsnMethod_Scatter_plotAsTubeStrip::update(const bool force) {
  return updateStep(m_requestedStp, force);
}

void vsnMethod_Scatter_plotAsTubeStrip::reloaded() {
  adjustRange();
  vsnMethodObj::reloaded();
}

vsnMethodPP* vsnMethod_Scatter_plotAsTubeStrip::getParamPanel(wxPanel* pp) {
  vsnMPP_Scatter_plotAsTubeStrip* pp_plotAsTubeStrip
    = new vsnMPP_Scatter_plotAsTubeStrip(pp, this);
  if ( ! pp_plotAsTubeStrip ) return NULL;
  return pp_plotAsTubeStrip;
}


/* vsnTimeSeriesMethodIF methods */

bool vsnMethod_Scatter_plotAsTubeStrip::updateStep(const int stp,
						   const bool force,
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
    m_shape = new vfrGroup();
    if ( ! m_shape ) {
      ErrMsg(MsgERR, getMethodType() + string("[") + getName()
             + string("]: memory allocation failed"));
      return false;
    }
    m_shape->alcMaterial();
    addChild(m_shape);
  }
  m_shape->getPrivateMaterial()->setRenderMode(RT_NONE);
  m_shape->remAllChildren();
  if ( ! m_show ) {
    return true;
  }

  // check selected data
  size_t dlen = pData->getDataLen();
  if ( (m_selectedColData == DATA_Veclen || m_selectedRadData == DATA_Veclen)
       && ! isValidVecData() ) return true;

  // get point data
  int numVtx = pData->getNumVerts();
  if ( numVtx < 2 ) return true;
  vector3* pv = pData->getVerts();
  float* pd = pData->getData();

  // adjust split NV list
  deque<int> nv_list;
  if ( getSplitWithNV() ) {
    nv_list = pData->getNvList(m_requestedStp);
    int totalNv = 0;
    deque<int>::iterator nvit = nv_list.begin();
    for ( ; nvit != nv_list.end(); nvit++ ) totalNv += *nvit;
    if ( numVtx < totalNv ) {
      while ( nv_list.size() > 0 ) {
	int lastNv = nv_list.back();
	nv_list.pop_back();
	totalNv -= lastNv;
	if ( numVtx >= totalNv ) break;
      }
    }
    if ( numVtx > totalNv ) {
      int lastNv = numVtx - totalNv;
      nv_list.push_back(lastNv);
    }
  } else {
    nv_list.push_back(numVtx);
  }
  size_t numStrip = nv_list.size();

  // alloc TubeLineStrip set
  size_t idxOfst = 0;
  for ( int idxL = 0; idxL < numStrip; idxL++ ) {
    if ( nv_list[idxL] < 2 ) continue;
    vsnTubeLineStrip* ptls = new vsnTubeLineStrip();
    if ( ! ptls ) {
      ErrMsg(MsgERR, getMethodType() + string("[") + getName()
           + string("]: memory allocation for TubeLineStrip failed"));
      return false;
    }
    if ( ! ptls->alcVerts(nv_list[idxL]) ) {
      ErrMsg(MsgERR, getMethodType() + string("[") + getName()
	     + string("]: memory allocation for vertex failed"));
      return false;
    }
    if ( m_selectedColData == DATA_Veclen ||
	 (m_selectedColData > 0 && m_selectedColData <= dlen) ) {
      if ( ! ptls->alcColors(nv_list[idxL]) ) {
	ErrMsg(MsgERR, getMethodType() + string("[") + getName()
	       + string("]: memory allocation for colors failed"));
	return false;
      }
    }
    if ( ! ptls->alcRadiusList(nv_list[idxL]) ) {
      ErrMsg(MsgERR, getMethodType() + string("[") + getName()
	     + string("]: memory allocation for radius failed"));
      return false;
    }

    // set verts of TubeLineStrip
    register size_t i;
    vector3* vl = ptls->getVerts();
    for ( i = 0; i < nv_list[idxL]; i++ ) {
      memcpy(vl[i], pv[i+idxOfst], sizeof(vector3));
    } // end of for(i)
    ptls->generateBbox();

    // set colors
    vector4* cl = ptls->getColors();
    Vec3<float> vv;
    register float dval;
    register int c;
    if ( m_selectedColData == DATA_Veclen ) {
      for ( i = 0; i < nv_list[idxL]; i++ ) {
	vv[0] = pd[dlen*(i+idxOfst) + m_vecDataIdx[0]];
	vv[1] = pd[dlen*(i+idxOfst) + m_vecDataIdx[1]];
	vv[2] = pd[dlen*(i+idxOfst) + m_vecDataIdx[2]];
	dval = vv.Length();
	c = m_lut.getValIdx(dval);
	cl[i][0] = m_lut.lutEntry[c*4  ];
	cl[i][1] = m_lut.lutEntry[c*4+1];
	cl[i][2] = m_lut.lutEntry[c*4+2];
	cl[i][3] = 1.f;
      } // end of for(i)
      ptls->setColorMode(AT_PER_VERTEX);
    }
    else if ( m_selectedColData > 0 && m_selectedColData <= dlen ) {
      for ( i = 0; i < nv_list[idxL]; i++ ) {
	dval = pd[dlen*(i+idxOfst) + m_selectedColData -1];
	c = m_lut.getValIdx(dval);
	cl[i][0] = m_lut.lutEntry[c*4  ];
	cl[i][1] = m_lut.lutEntry[c*4+1];
	cl[i][2] = m_lut.lutEntry[c*4+2];
	cl[i][3] = 1.f;
      } // end of for(i)
      ptls->setColorMode(AT_PER_VERTEX);
    }
    else { // not ref
      cl[0][0] = m_colour[0]; cl[0][1] = m_colour[1];
      cl[0][2] = m_colour[2]; cl[0][3] = 1.f;
      ptls->setColorMode(AT_WHOLE);
    }

    // set radius
    deque<double>& rl = ptls->getRadiusList();
    if ( m_selectedRadData == DATA_Veclen ) {
      for ( i = 0; i < nv_list[idxL]; i++ ) {
	vv[0] = pd[dlen*(i+idxOfst) + m_vecDataIdx[0]];
	vv[1] = pd[dlen*(i+idxOfst) + m_vecDataIdx[1]];
	vv[2] = pd[dlen*(i+idxOfst) + m_vecDataIdx[2]];
	rl[i] = vv.Length() * m_radiusBias;
      } // end of for(i)
    }
    else if ( m_selectedRadData > 0 && m_selectedRadData <= dlen ) {
      for ( i = 0; i < nv_list[idxL]; i++ ) {
	rl[i] = pd[dlen*(i+idxOfst) + m_selectedColData -1] * m_radiusBias;
      } // end of for(i)
    }
    else { // not ref
      for ( i = 0; i < nv_list[idxL]; i++ ) {
	rl[i] = m_radiusBias;
      } // end of for(i)
    }

    m_shape->addChild(ptls);
    idxOfst += nv_list[idxL];
  } // end of for(idxL)  

  // ok
  m_shape->getPrivateMaterial()->setRenderMode(m_showType);
  setBaseColor(m_colour);
  setHilight(m_hilight);
  m_updatedStp = m_requestedStp;
  return true;
}


/* from vsnIoObject */

bool vsnMethod_Scatter_plotAsTubeStrip::parseXML(xmlNodePtr xnp) {
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

      if ( xsN == string("use_color_data") ) {
        WhichDataType sdt = -2;
        if ( xsV == string("none") ) sdt = DATA_None;
        else if ( xsV == string("veclen")  ) sdt = DATA_Veclen;
        else if ( xsV.substr(0, 4) == string("data") ) {
          string numStr = xsV.substr(4);
          if ( ! numStr.empty() ) sdt = atoi(numStr.c_str()) + 1;
        }
        if ( ! setSelectedColData(sdt) ) {
          ErrMsg(MsgERR, msgHdr + string("failed to select ") +xsV);
          goto _NEXT_XML_NODE;
        }
      } // end of "use_color_data"
      else if ( xsN == string("use_radius_data") ) {
        WhichDataType sdt = -2;
        if ( xsV == string("none") ) sdt = DATA_None;
        else if ( xsV == string("veclen")  ) sdt = DATA_Veclen;
        else if ( xsV.substr(0, 4) == string("data") ) {
          string numStr = xsV.substr(4);
          if ( ! numStr.empty() ) sdt = atoi(numStr.c_str()) + 1;
        }
        if ( ! setSelectedRadData(sdt) ) {
          ErrMsg(MsgERR, msgHdr + string("failed to select ") +xsV);
          goto _NEXT_XML_NODE;
        }
      } // end of "use_radius_data"
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
      else if ( xsN == string("radius_bias") ) {
        float rb = (float)atof(xsV.c_str());
        if ( ! setRadiusBias(rb) ) {
          ErrMsg(MsgERR, msgHdr + string("invalid value in param radius_bias"));
          goto _NEXT_XML_NODE;
        }
      } // end of "radius_bias"
      else if ( xsN == string("split_with_NV") ) {
        bool aam;
        if ( xsV == string("yes") ) aam = true;
        else if ( xsV == string("no") ) aam = false;
        else {
          ErrMsg(MsgERR, msgHdr
                 + string("invalid value in param split_with_NV"));
          goto _NEXT_XML_NODE;
        }
        if ( ! setSplitWithNV(aam) ) {
          ErrMsg(MsgERR, msgHdr + string("can't set split_with_NV"));
          goto _NEXT_XML_NODE;
        }
      } // end of "split_with_NV"
    } // end of param

  _NEXT_XML_NODE:
    cur = cur->next;
  } // end of while(cur)

  return true;
}

bool vsnMethod_Scatter_plotAsTubeStrip::outputXML(std::ostream& os,
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
  // use_color_data
  if ( m_selectedColData != DATA_None ) {
    os << idts_2 << "<param name=\"use_color_data\" value=\"";
    if ( m_selectedColData == DATA_Veclen )
      os << "veclen";
    else if ( m_selectedColData > 0 )
      os << "data" << m_selectedColData -1;
    else {
      os << "none";
      ErrMsg(MsgWARN, msgHdr
             + string("invalid use_color_data has set, so don't output\n"));
    }
    os << "\" />" << endl;
  }

  // use_radius_data
  if ( m_selectedRadData != DATA_None ) {
    os << idts_2 << "<param name=\"use_radius_data\" value=\"";
    if ( m_selectedRadData == DATA_Veclen )
      os << "veclen";
    else if ( m_selectedRadData > 0 )
      os << "data" << m_selectedRadData -1;
    else {
      os << "none";
      ErrMsg(MsgWARN, msgHdr
             + string("invalid use_radius_data has set, so don't output\n"));
    }
    os << "\" />" << endl;
  }

  // vec_idx
  if ( m_vecDataIdx[0] != 0 || m_vecDataIdx[1] != 1 || m_vecDataIdx[2] != 2 ) {
    os << idts_2 << "<param name=\"vec_idx\" value=\"";
    os << m_vecDataIdx[0] << "/" << m_vecDataIdx[1] << "/" << m_vecDataIdx[2];
    os << "\" />" << endl;
  }

  // upd_minmax
  if ( ! m_updateMinMax ) {
    os << idts_2 << "<param name=\"upd_minmax\" value=\"no\" />" << endl;
  }

  // radius_bias
  if ( m_radiusBias != 1.f ) {
    os << idts_2 << "<param name=\"radius_bias\" value=\""
       << m_radiusBias << "\" />" << endl;
  }

  // split with NV
  if ( m_splitWithNV != true ) {
    os << idts_2 << "<param name=\"split_with_NV\" value=\"no\" />" << endl;
  }

  os << idts << "</method>" << endl;
  return ret;
}

bool vsnMethod_Scatter_plotAsTubeStrip::commandXML(xmlNodePtr xnp) {
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
  if ( nameStr == "set_use_color_data" ) {
    WhichDataType sdt = -2;
    if ( valueStr == string("none") ) sdt = DATA_None;
    else if ( valueStr == string("veclen")  ) sdt = DATA_Veclen;
    else if ( valueStr.substr(0, 4) == string("data") ) {
      string numStr = valueStr.substr(4);
      if ( ! numStr.empty() ) sdt = atoi(numStr.c_str()) + 1;
    }
    if ( ! setSelectedColData(sdt) ) {
      ErrMsg(MsgERR, msgHdr + "command " + nameStr
             + ": set failed: " + valueStr);
      return false;
    }
  } // end of "set_use_color_data"
  else if ( nameStr == "set_use_radius_data" ) {
    WhichDataType sdt = -2;
    if ( valueStr == string("none") ) sdt = DATA_None;
    else if ( valueStr == string("veclen")  ) sdt = DATA_Veclen;
    else if ( valueStr.substr(0, 4) == string("data") ) {
      string numStr = valueStr.substr(4);
      if ( ! numStr.empty() ) sdt = atoi(numStr.c_str()) + 1;
    }
    if ( ! setSelectedRadData(sdt) ) {
      ErrMsg(MsgERR, msgHdr + "command " + nameStr
             + ": set failed: " + valueStr);
      return false;
    }
  } // end of "set_use_radius_data"
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
  else if ( nameStr == "set_upd_minmax" ) {
    bool mmupd;
    if ( valueStr == string("yes") ) mmupd = true;
    else if ( valueStr == string("no") ) mmupd = false;
    else {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_upd_minmax: invalid value"));
      return false;
    }
    if ( ! setUpdateMinMaxMode(mmupd) ) {
      ErrMsg(MsgERR, msgHdr +
             string("command set_upd_minmax: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_upd_minmax"
  else if ( nameStr == "set_radius_bias" ) {
    float rb = (float)atof(valueStr.c_str());
    if ( ! setRadiusBias(rb) ) {
      ErrMsg(MsgERR, msgHdr +
             string("command set_radius_bias: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_radius_bias"
  else if ( nameStr == "set_split_with_NV" ) {
    bool split;
    if ( valueStr == string("yes") ) split = true;
    else if ( valueStr == string("no") ) split = false;
    else {
      ErrMsg(MsgERR, msgHdr +
             string("invalid command: set_split_with_NV: invalid value"));
      return false;
    }
    if ( ! setSplitWithNV(split) ) {
      ErrMsg(MsgERR, msgHdr +
             string("command set_split_with_NV: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_split_with_NV"
  else {
    // not my command
    ErrMsg(MsgERR, msgHdr + string("unknown command: ") + nameStr);
    return false;
  }

  return true;
}
