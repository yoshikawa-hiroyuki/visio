//
// vsnMethod_OctVol_isoCells
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

#include "vsnMethod_OctVol_isoCells.h"
#include "vsnUiView.h"
#include "vsnColorBar.h"
#include "vsnError.h"

using namespace std;
using namespace CES;
using namespace VFR;
using namespace VSN;


//----------------------------------------------------------------
// class vsnMPP_OctVol_isoCells
//----------------------------------------------------------------

BEGIN_EVENT_TABLE(vsnMPP_OctVol_isoCells, wxPanel)
  EVT_COMBOBOX(MPP_OctVol_isoCells_SelDataLst,
               vsnMPP_OctVol_isoCells::OnSelDataLst)
  EVT_CHECKLISTBOX(MPP_OctVol_isoCells_VecDataChkLst,
                   vsnMPP_OctVol_isoCells::OnVecDataChkLst)
  EVT_TEXT_ENTER(MPP_OctVol_isoCells_ValueTxt,
                 vsnMPP_OctVol_isoCells::OnValueTxt)
  EVT_CHECKBOX(MPP_OctVol_isoCells_UseCMapChk,
               vsnMPP_OctVol_isoCells::OnUseCMapChk)
  EVT_CHECKBOX(MPP_OctVol_isoCells_UseCMapAlphaChk,
               vsnMPP_OctVol_isoCells::OnUseCMapAlphaChk)
  EVT_CHECKBOX(MPP_OctVol_isoCells_UpdMinMaxChk,
               vsnMPP_OctVol_isoCells::OnUpdMinMaxChk)
END_EVENT_TABLE()


/* constructors / destructor */

vsnMPP_OctVol_isoCells::vsnMPP_OctVol_isoCells(wxPanel* parent,
					       vsnMethodObj* pm)
  : vsnMethodPP(parent, pm)
{
  assert(parent);
  assert(dynamic_cast<vsnMethod_OctVol_isoCells*>(pm));

  wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer* sizerH;

  // iso value
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxALL, 0);
  sizerH->Add(new wxStaticText(this, -1, wxT("iso value")),
              0, wxALIGN_LEFT|wxALL, 3);
  m_pValueTxt = new wxTextCtrl(this, MPP_OctVol_isoCells_ValueTxt, wxT(""),
                               wxDefaultPosition, wxDefaultSize,
                               wxTE_PROCESS_ENTER);
  sizerH->Add(m_pValueTxt, 0, wxEXPAND|wxALL, 3);

  // data selector
  topsizer->Add(new wxStaticText(this, -1, wxT("select scalar data")),
                0, wxALIGN_LEFT|wxALL, 3);
  m_pSelDataLst = new wxComboBox(this, MPP_OctVol_isoCells_SelDataLst,
                                 wxT(""), wxDefaultPosition, wxDefaultSize,
                                 0, NULL, wxCB_READONLY);
  topsizer->Add(m_pSelDataLst, 0, wxEXPAND|wxALL, 3);

  // vector data selector
  topsizer->Add(new wxStaticText(this, -1, wxT("vector components")),
                0, wxALIGN_LEFT|wxALL, 3);
  m_pVecDataChkLst = new wxCheckListBox(this,
                                        MPP_OctVol_isoCells_VecDataChkLst);
  topsizer->Add(m_pVecDataChkLst, 0, wxEXPAND|wxALL, 3);

  // use CMap / CMap alpha
  m_pUseCMapChk = new wxCheckBox(this, MPP_OctVol_isoCells_UseCMapChk,
                                 wxT("use cmap"));
  m_pUseCMapChk->SetValue(TRUE);
  topsizer->Add(m_pUseCMapChk, 0, wxALL, 3);
  m_pUseCMapAlphaChk = new wxCheckBox(this,
				      MPP_OctVol_isoCells_UseCMapAlphaChk,
                                      wxT("use cmap alpha"));
  topsizer->Add(m_pUseCMapAlphaChk, 0, wxALL, 3);

  // update minmax mode
  m_pUpdMinMaxChk = new wxCheckBox(this, MPP_OctVol_isoCells_UpdMinMaxChk,
                                   wxT("update minmax"));
  m_pUpdMinMaxChk->SetValue(TRUE);
  topsizer->Add(m_pUpdMinMaxChk, 0, wxALL, 3);

  // post process
  SetSizer(topsizer);
  addTo(parent);
  topsizer->Layout();
  (void)update();
}

vsnMPP_OctVol_isoCells::~vsnMPP_OctVol_isoCells() {
}


/* interface */

bool vsnMPP_OctVol_isoCells::update() {
  if ( ! m_pSelDataLst || ! m_pVecDataChkLst || ! m_pValueTxt ||
       ! m_pUseCMapChk || ! m_pUseCMapAlphaChk || ! m_pUpdMinMaxChk )
    return false;

  vsnMethod_OctVol_isoCells* pm
    = dynamic_cast<vsnMethod_OctVol_isoCells*>(p_method);
  if ( ! pm ) return false;
  vsnData_OctVol* pdo = dynamic_cast<vsnData_OctVol*>(pm->getRefData());
  if ( ! pdo ) return false;

  register int i;
  char txt[64];

  // iso value
  float val = pm->getIsoValue();
  sprintf(txt, "%g", val); m_pValueTxt->SetValue(vsnApp::ConvSysToWx(txt));

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

  // use CMap / CMap alpha
  m_pUseCMapChk->SetValue(pm->getUseCMap());
  m_pUseCMapAlphaChk->SetValue(pm->getUseCMapAlpha());

  // update minmax mode
  m_pUpdMinMaxChk->SetValue(pm->getUpdateMinMaxMode());

  return true;
}


/* event handler */

void vsnMPP_OctVol_isoCells::OnSelDataLst(wxCommandEvent& event) {
  if ( ! m_pSelDataLst ) return;
  WhichDataType sel = event.GetInt();
  if ( sel < 0 ) return;

  wxString valStr = m_pSelDataLst->GetString(sel);
  if ( valStr == wxT("vector length") )
    sel = DATA_Veclen;

  vsnMethod_OctVol_isoCells* pm
    = dynamic_cast<vsnMethod_OctVol_isoCells*>(p_method);
  if ( ! pm ) return;
  if ( sel == pm->getSelectedData() ) return;

  if ( ! pm->setSelectedData(sel) ) {
    update();
    return;
  }
  pm->chkNotice();
}

void vsnMPP_OctVol_isoCells::OnVecDataChkLst(wxCommandEvent& event) {
  if ( ! m_pVecDataChkLst ) return;
  int dlen = m_pVecDataChkLst->GetCount();
  if ( dlen < 3 ) return;

  vsnMethod_OctVol_isoCells* pm
    = dynamic_cast<vsnMethod_OctVol_isoCells*>(p_method);
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

void vsnMPP_OctVol_isoCells::OnValueTxt(wxCommandEvent& event) {
  if ( ! m_pValueTxt ) return;

  vsnMethod_OctVol_isoCells* pm
    = dynamic_cast<vsnMethod_OctVol_isoCells*>(p_method);
  if ( ! pm ) return;

  wxString valStr = m_pValueTxt->GetValue();
  if ( valStr.IsEmpty() ) {
    char txt[64]; sprintf(txt, "%g", pm->getIsoValue());
    m_pValueTxt->SetValue(vsnApp::ConvSysToWx(txt));
    return;
  }
  float val = (float)atof(vsnApp::ConvWxToSys(valStr).c_str());

  if ( pm->setIsoValue(val) )
    pm->chkNotice();
}

void vsnMPP_OctVol_isoCells::OnUseCMapChk(wxCommandEvent& event) {
  if ( ! m_pUseCMapChk ) return;
  bool val = m_pUseCMapChk->GetValue();

  vsnMethod_OctVol_isoCells* pm
    = dynamic_cast<vsnMethod_OctVol_isoCells*>(p_method);
  if ( ! pm ) return;

  if ( pm->setUseCMap(val) )
    pm->chkNotice();
}

void vsnMPP_OctVol_isoCells::OnUseCMapAlphaChk(wxCommandEvent& event) {
  if ( ! m_pUseCMapAlphaChk ) return;
  bool val = m_pUseCMapAlphaChk->GetValue();

  vsnMethod_OctVol_isoCells* pm
    = dynamic_cast<vsnMethod_OctVol_isoCells*>(p_method);
  if ( ! pm ) return;

  if ( pm->setUseCMapAlpha(val) )
    pm->chkNotice();
}

void vsnMPP_OctVol_isoCells::OnUpdMinMaxChk(wxCommandEvent& event) {
  if ( ! m_pUpdMinMaxChk ) return;
  bool val = m_pUpdMinMaxChk->GetValue();

  vsnMethod_OctVol_isoCells* pm
    = dynamic_cast<vsnMethod_OctVol_isoCells*>(p_method);
  if ( ! pm ) return;

  if ( pm->setUpdateMinMaxMode(val) )
    pm->chkNotice();
}


//----------------------------------------------------------------
// class vsnMethod_OctVol_isoCells
//----------------------------------------------------------------

/* constructors / destructor */

vsnMethod_OctVol_isoCells::
vsnMethod_OctVol_isoCells(const std::string& name)
  : vsnMethodObj(name), m_isoTrias(NULL), m_pd(NULL),
    m_selectedData(DATA_None), m_vecDataIdx(0,1,2), m_isoValue(0.f),
    m_useCMap(true), m_useCMapAlpha(false), m_updateMinMax(true)
{
}

vsnMethod_OctVol_isoCells::~vsnMethod_OctVol_isoCells() {
  if ( m_isoTrias )
    delete m_isoTrias;
  if ( m_pd )
    DeAllocate(m_pd);
}


/* methods */

void vsnMethod_OctVol_isoCells::adjustRange(const bool updval) {
  bool needRangeUpd = false;

  vsnData_OctVol* pData = dynamic_cast<vsnData_OctVol*>(p_refData);
  if ( ! pData ) return;
  int dlen = (int)pData->getDataLen();

  float dr[2];
  if ( m_selectedData == DATA_Veclen ) {
    if ( pData->getVectorMaxLen(m_vecDataIdx, dr[1]) ) {
      if ( updval )
        m_isoValue = dr[1] * 0.5f;
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
        m_isoValue = (dr[0] + dr[1])*0.5f;
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

void vsnMethod_OctVol_isoCells::updateColor() {
  if ( ! m_isoTrias ) return;

  vector4 cv;
  int c = m_lut.getValIdx(m_isoValue);
  memcpy(cv, &m_lut.lutEntry[c*4], sizeof(float)*4);

  if ( m_useCMap )
    m_isoTrias->setColor3(0, cv);
  else
    m_isoTrias->setColor3(0, m_colour);

  if ( m_useCMapAlpha )
    m_isoTrias->setAlpha(TRUE, cv[3]);
  else
    m_isoTrias->setAlpha((m_colour[3]<OPAC_THRESH), m_colour[3]);

  chkNotice();
}

bool vsnMethod_OctVol_isoCells::setSelectedData(const WhichDataType sd) {
  if ( sd == m_selectedData ) return true;

  vsnData_OctVol* pData = dynamic_cast<vsnData_OctVol*>(p_refData);
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

bool vsnMethod_OctVol_isoCells::setVecDataIdx(const CES::Vec3<int>& vdidx) {
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

bool vsnMethod_OctVol_isoCells::isValidVecData() const {
  vsnData_OctVol* pData = dynamic_cast<vsnData_OctVol*>(p_refData);
  if ( ! pData ) return false;

  size_t dlen = pData->getDataLen();
  if ( m_vecDataIdx[0] < 0 || m_vecDataIdx[0] >= dlen ) return false;
  if ( m_vecDataIdx[1] < 0 || m_vecDataIdx[1] >= dlen ) return false;
  if ( m_vecDataIdx[2] < 0 || m_vecDataIdx[2] >= dlen ) return false;
  return true;
}

bool vsnMethod_OctVol_isoCells::setIsoValue(const float val) {
  if ( m_isoValue == val ) return true;
  m_isoValue = val;

  if ( ! update() ) return false;
  updateUI();
  return true;
}

bool vsnMethod_OctVol_isoCells::setUseCMap(const bool ucm) {
  if ( m_useCMap == ucm ) return true;
  m_useCMap = ucm;

  updateColor();
  updateUI();
  return true;
}

bool vsnMethod_OctVol_isoCells::setUseCMapAlpha(const bool ucma) {
  if ( m_useCMapAlpha == ucma ) return true;
  m_useCMapAlpha = ucma;

  updateColor();
  updateUI();
  return true;
}

bool vsnMethod_OctVol_isoCells::setUpdateMinMaxMode(const bool mode) {
  if ( m_updateMinMax == mode ) return true;
  m_updateMinMax = mode;

  // data range
  adjustRange(false);

  if ( ! update() ) return false;
  updateUI();
  return true;
}


/* vsnTimeSeriesMethodIF methods */

#define VEC_LEN(x) \
Vec3<float>((x)->m_pData[m_vecDataIdx[0]], \
  (x)->m_pData[m_vecDataIdx[1]], (x)->m_pData[m_vecDataIdx[2]]).Length()


bool vsnMethod_OctVol_isoCells::updateStep(const int stp, const bool force,
					   const bool cascade)
{
  vsnData_OctVol* pData = dynamic_cast<vsnData_OctVol*>(p_refData);
  if ( ! pData ) return false;
  m_requestedStp = stp;
  if ( m_requestedStp < 0 )
    m_requestedStp = pData->getCurrentStepIdx();

  if ( force ) m_updatedStp = -1;
  if ( m_updatedStp == m_requestedStp ) return true;
  m_updatedStp = -1;

  // tria data
  if ( ! m_isoTrias ) {
    m_isoTrias = new vfrTriangles();
    if ( ! m_isoTrias ) {
      ErrMsg(MsgERR, getMethodType() + string("[") + getName()
             + string("]: memory allocation failed"));
      return false;
    }
    m_isoTrias->alcMaterial();
    m_isoTrias->setNormalMode(AT_PER_FACE);
    addChild(m_isoTrias);
  }
  m_isoTrias->getPrivateMaterial()->setRenderMode(RT_NONE);
  if ( ! m_show ) {
    return true;
  }

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
  if ( m_isoValue <= dr[0] || m_isoValue >= dr[1] ) return true;

  vsnOctTree* otv = (vsnOctTree*)pData->getOctTree();
  Vec3<float> rootPitch = otv->m_pitch;
  deque<FaceWall> wallLst;
  set<vsnOctTree::Node*>::const_iterator nit;
  if ( m_selectedData == DATA_Veclen ) {
    for ( nit = otv->m_pLeafLst.begin();
	  nit != otv->m_pLeafLst.end(); nit++ ) {
      if ( ! (*nit) ) continue;
      vsnOctTree::Node* pnn;
      if ( VEC_LEN(*nit) < m_isoValue ) {
	if ( (pnn = otv->GetNeighbor(*nit, Vec3<int>(-1,0,0))) ) {
	  if ( ! pnn->m_hasChildren && VEC_LEN(pnn) >= m_isoValue )
	    wallLst.push_back(FaceWall(*nit, FaceWall::FaceMX));
	}
	if ( (pnn = otv->GetNeighbor(*nit, Vec3<int>(1,0,0))) ) {
	  if ( ! pnn->m_hasChildren && VEC_LEN(pnn) >= m_isoValue )
	    wallLst.push_back(FaceWall(*nit, FaceWall::FacePX));
	}
	if ( (pnn = otv->GetNeighbor(*nit, Vec3<int>(0,-1,0))) ) {
	  if ( ! pnn->m_hasChildren && VEC_LEN(pnn) >= m_isoValue )
	    wallLst.push_back(FaceWall(*nit, FaceWall::FaceMY));
	}
	if ( (pnn = otv->GetNeighbor(*nit, Vec3<int>(0,1,0))) ) {
	  if ( ! pnn->m_hasChildren && VEC_LEN(pnn) >= m_isoValue )
	    wallLst.push_back(FaceWall(*nit, FaceWall::FacePY));
	}
	if ( (pnn = otv->GetNeighbor(*nit, Vec3<int>(0,0,-1))) ) {
	  if ( ! pnn->m_hasChildren && VEC_LEN(pnn) >= m_isoValue )
	    wallLst.push_back(FaceWall(*nit, FaceWall::FaceMZ));
	}
	if ( (pnn = otv->GetNeighbor(*nit, Vec3<int>(0,0,1))) ) {
	  if ( ! pnn->m_hasChildren && VEC_LEN(pnn) >= m_isoValue )
	    wallLst.push_back(FaceWall(*nit, FaceWall::FacePZ));
	}
      }
      else {
	if ( (pnn = otv->GetNeighbor(*nit, Vec3<int>(-1,0,0))) ) {
	  if ( ! pnn->m_hasChildren && VEC_LEN(pnn) < m_isoValue )
	    wallLst.push_back(FaceWall(*nit, FaceWall::FaceMX));
	}
	if ( (pnn = otv->GetNeighbor(*nit, Vec3<int>(1,0,0))) ) {
	  if ( ! pnn->m_hasChildren && VEC_LEN(pnn) < m_isoValue )
	    wallLst.push_back(FaceWall(*nit, FaceWall::FacePX));
	}
	if ( (pnn = otv->GetNeighbor(*nit, Vec3<int>(0,-1,0))) ) {
	  if ( ! pnn->m_hasChildren && VEC_LEN(pnn) < m_isoValue )
	    wallLst.push_back(FaceWall(*nit, FaceWall::FaceMY));
	}
	if ( (pnn = otv->GetNeighbor(*nit, Vec3<int>(0,1,0))) ) {
	  if ( ! pnn->m_hasChildren && VEC_LEN(pnn) < m_isoValue )
	    wallLst.push_back(FaceWall(*nit, FaceWall::FacePY));
	}
	if ( (pnn = otv->GetNeighbor(*nit, Vec3<int>(0,0,-1))) ) {
	  if ( ! pnn->m_hasChildren && VEC_LEN(pnn) < m_isoValue )
	    wallLst.push_back(FaceWall(*nit, FaceWall::FaceMZ));
	}
	if ( (pnn = otv->GetNeighbor(*nit, Vec3<int>(0,0,1))) ) {
	  if ( ! pnn->m_hasChildren && VEC_LEN(pnn) < m_isoValue )
	    wallLst.push_back(FaceWall(*nit, FaceWall::FacePZ));
	}
      }
    } // end of for(nit)
  } // end of if(selectedData==DATA_Veclen)
  else {
    for ( nit = otv->m_pLeafLst.begin();
	  nit != otv->m_pLeafLst.end(); nit++ ) {
      if ( ! (*nit) ) continue;
      vsnOctTree::Node* pnn;
      if ( (*nit)->m_pData[m_selectedData -1] < m_isoValue ) {
	if ( (pnn = otv->GetNeighbor(*nit, Vec3<int>(-1,0,0))) ) {
	  if ( ! pnn->m_hasChildren &&
	       pnn->m_pData[m_selectedData -1] >= m_isoValue )
	    wallLst.push_back(FaceWall(*nit, FaceWall::FaceMX));
	}
	if ( (pnn = otv->GetNeighbor(*nit, Vec3<int>(1,0,0))) ) {
	  if ( ! pnn->m_hasChildren &&
	       pnn->m_pData[m_selectedData -1] >= m_isoValue )
	    wallLst.push_back(FaceWall(*nit, FaceWall::FacePX));
	}
	if ( (pnn = otv->GetNeighbor(*nit, Vec3<int>(0,-1,0))) ) {
	  if ( ! pnn->m_hasChildren &&
	       pnn->m_pData[m_selectedData -1] >= m_isoValue )
	    wallLst.push_back(FaceWall(*nit, FaceWall::FaceMY));
	}
	if ( (pnn = otv->GetNeighbor(*nit, Vec3<int>(0,1,0))) ) {
	  if ( ! pnn->m_hasChildren &&
	       pnn->m_pData[m_selectedData -1] >= m_isoValue )
	    wallLst.push_back(FaceWall(*nit, FaceWall::FacePY));
	}
	if ( (pnn = otv->GetNeighbor(*nit, Vec3<int>(0,0,-1))) ) {
	  if ( ! pnn->m_hasChildren &&
	       pnn->m_pData[m_selectedData -1] >= m_isoValue )
	    wallLst.push_back(FaceWall(*nit, FaceWall::FaceMZ));
	}
	if ( (pnn = otv->GetNeighbor(*nit, Vec3<int>(0,0,1))) ) {
	  if ( ! pnn->m_hasChildren &&
	       pnn->m_pData[m_selectedData -1] >= m_isoValue )
	    wallLst.push_back(FaceWall(*nit, FaceWall::FacePZ));
	}
      }
      else {
	if ( (pnn = otv->GetNeighbor(*nit, Vec3<int>(-1,0,0))) ) {
	  if ( ! pnn->m_hasChildren &&
	       pnn->m_pData[m_selectedData -1] < m_isoValue )
	    wallLst.push_back(FaceWall(*nit, FaceWall::FaceMX));
	}
	if ( (pnn = otv->GetNeighbor(*nit, Vec3<int>(1,0,0))) ) {
	  if ( ! pnn->m_hasChildren &&
	       pnn->m_pData[m_selectedData -1] < m_isoValue )
	    wallLst.push_back(FaceWall(*nit, FaceWall::FacePX));
	}
	if ( (pnn = otv->GetNeighbor(*nit, Vec3<int>(0,-1,0))) ) {
	  if ( ! pnn->m_hasChildren &&
	       pnn->m_pData[m_selectedData -1] < m_isoValue )
	    wallLst.push_back(FaceWall(*nit, FaceWall::FaceMY));
	}
	if ( (pnn = otv->GetNeighbor(*nit, Vec3<int>(0,1,0))) ) {
	  if ( ! pnn->m_hasChildren &&
	       pnn->m_pData[m_selectedData -1] < m_isoValue )
	    wallLst.push_back(FaceWall(*nit, FaceWall::FacePY));
	}
	if ( (pnn = otv->GetNeighbor(*nit, Vec3<int>(0,0,-1))) ) {
	  if ( ! pnn->m_hasChildren &&
	       pnn->m_pData[m_selectedData -1] < m_isoValue )
	    wallLst.push_back(FaceWall(*nit, FaceWall::FaceMZ));
	}
	if ( (pnn = otv->GetNeighbor(*nit, Vec3<int>(0,0,1))) ) {
	  if ( ! pnn->m_hasChildren &&
	       pnn->m_pData[m_selectedData -1] < m_isoValue )
	    wallLst.push_back(FaceWall(*nit, FaceWall::FacePZ));
	}
      }
    } // end of for(nit)
  } // end of if(selectedData!=DATA_Veclen)

  m_isoTrias->alcVerts(wallLst.size()*2*3);
  m_isoTrias->alcNormals(wallLst.size()*2);

  register size_t iVtx = 0, iFcs = 0;
  vector3* pv = m_isoTrias->getVerts();
  vector3* pn = m_isoTrias->getNormals();
  CES::Vec3<float>* dbb = otv->m_bbox;
  deque<FaceWall>::iterator it;
  for ( it = wallLst.begin(); it != wallLst.end(); it++ ) {
    register size_t l, lvl = it->p_node->GetLevel();
    register float scaleFac = 1.f;
    for ( l = 0; l < lvl; l++ ) scaleFac *= 0.5f;
    vsnOctTree::Node* pnode = it->p_node;

    Vec3<float> orig, giro, size = rootPitch;
    PedigType pedig = pnode->m_pedigree;
    while ( pedig && pnode ) {
      register int relp = pedig % 10 -1;
      if ( relp & (0x1)    ) orig.m_v[0] += rootPitch.m_v[0] * scaleFac;
      if ( relp & (0x1<<1) ) orig.m_v[1] += rootPitch.m_v[1] * scaleFac;
      if ( relp & (0x1<<2) ) orig.m_v[2] += rootPitch.m_v[2] * scaleFac;
      size = size * 0.5f;
      scaleFac *= 2.f;
      pedig = pedig / 10;
      pnode = pnode->p_parent;
    } // end of while(pedig && pnode)

    vsnOctTree::RootNode* proot = dynamic_cast<vsnOctTree::RootNode*>(pnode);
    if ( ! proot ) continue;
    orig.m_v[0] += rootPitch.m_v[0] * (float)proot->m_idx[0];
    orig.m_v[1] += rootPitch.m_v[1] * (float)proot->m_idx[1];
    orig.m_v[2] += rootPitch.m_v[2] * (float)proot->m_idx[2];
    orig = orig + dbb[0];
    giro = orig + size;

    // -X face
    if ( it->m_wall == FaceWall::FaceMX ) {
      pv[iVtx][0] = orig[0]; pv[iVtx][1] = orig[1]; pv[iVtx++][2] = orig[2];
      pv[iVtx][0] = orig[0]; pv[iVtx][1] = orig[1]; pv[iVtx++][2] = giro[2];
      pv[iVtx][0] = orig[0]; pv[iVtx][1] = giro[1]; pv[iVtx++][2] = giro[2];
      pn[iFcs][0] = -1.f; pn[iFcs][1] = 0.f; pn[iFcs++][2] = 0.f;
      pv[iVtx][0] = orig[0]; pv[iVtx][1] = giro[1]; pv[iVtx++][2] = giro[2];
      pv[iVtx][0] = orig[0]; pv[iVtx][1] = giro[1]; pv[iVtx++][2] = orig[2];
      pv[iVtx][0] = orig[0]; pv[iVtx][1] = orig[1]; pv[iVtx++][2] = orig[2];
      pn[iFcs][0] = -1.f; pn[iFcs][1] = 0.f; pn[iFcs++][2] = 0.f;
    }
    // +X face
    if ( it->m_wall == FaceWall::FacePX ) {
      pv[iVtx][0] = giro[0]; pv[iVtx][1] = orig[1]; pv[iVtx++][2] = orig[2];
      pv[iVtx][0] = giro[0]; pv[iVtx][1] = giro[1]; pv[iVtx++][2] = orig[2];
      pv[iVtx][0] = giro[0]; pv[iVtx][1] = giro[1]; pv[iVtx++][2] = giro[2];
      pn[iFcs][0] = 1.f; pn[iFcs][1] = 0.f; pn[iFcs++][2] = 0.f;
      pv[iVtx][0] = giro[0]; pv[iVtx][1] = giro[1]; pv[iVtx++][2] = giro[2];
      pv[iVtx][0] = giro[0]; pv[iVtx][1] = orig[1]; pv[iVtx++][2] = giro[2];
      pv[iVtx][0] = giro[0]; pv[iVtx][1] = orig[1]; pv[iVtx++][2] = orig[2];
      pn[iFcs][0] = 1.f; pn[iFcs][1] = 0.f; pn[iFcs++][2] = 0.f;
    }
    // -Y face
    if ( it->m_wall == FaceWall::FaceMY ) {
      pv[iVtx][0] = orig[0]; pv[iVtx][1] = orig[1]; pv[iVtx++][2] = orig[2];
      pv[iVtx][0] = giro[0]; pv[iVtx][1] = orig[1]; pv[iVtx++][2] = orig[2];
      pv[iVtx][0] = giro[0]; pv[iVtx][1] = orig[1]; pv[iVtx++][2] = giro[2];
      pn[iFcs][0] = 0.f; pn[iFcs][1] = -1.f; pn[iFcs++][2] = 0.f;
      pv[iVtx][0] = giro[0]; pv[iVtx][1] = orig[1]; pv[iVtx++][2] = giro[2];
      pv[iVtx][0] = orig[0]; pv[iVtx][1] = orig[1]; pv[iVtx++][2] = giro[2];
      pv[iVtx][0] = orig[0]; pv[iVtx][1] = orig[1]; pv[iVtx++][2] = orig[2];
      pn[iFcs][0] = 0.f; pn[iFcs][1] = -1.f; pn[iFcs++][2] = 0.f;
    }
    // +Y face
    if ( it->m_wall == FaceWall::FacePY ) {
      pv[iVtx][0] = orig[0]; pv[iVtx][1] = giro[1]; pv[iVtx++][2] = orig[2];
      pv[iVtx][0] = orig[0]; pv[iVtx][1] = giro[1]; pv[iVtx++][2] = giro[2];
      pv[iVtx][0] = giro[0]; pv[iVtx][1] = giro[1]; pv[iVtx++][2] = giro[2];
      pn[iFcs][0] = 0.f; pn[iFcs][1] = 1.f; pn[iFcs++][2] = 0.f;
      pv[iVtx][0] = giro[0]; pv[iVtx][1] = giro[1]; pv[iVtx++][2] = giro[2];
      pv[iVtx][0] = giro[0]; pv[iVtx][1] = giro[1]; pv[iVtx++][2] = orig[2];
      pv[iVtx][0] = orig[0]; pv[iVtx][1] = giro[1]; pv[iVtx++][2] = orig[2];
      pn[iFcs][0] = 0.f; pn[iFcs][1] = 1.f; pn[iFcs++][2] = 0.f;
    }
    // -Z face
    if ( it->m_wall == FaceWall::FaceMZ ) {
      pv[iVtx][0] = orig[0]; pv[iVtx][1] = orig[1]; pv[iVtx++][2] = orig[2];
      pv[iVtx][0] = orig[0]; pv[iVtx][1] = giro[1]; pv[iVtx++][2] = orig[2];
      pv[iVtx][0] = giro[0]; pv[iVtx][1] = giro[1]; pv[iVtx++][2] = orig[2];
      pn[iFcs][0] = 0.f; pn[iFcs][1] = 0.f; pn[iFcs++][2] = -1.f;
      pv[iVtx][0] = giro[0]; pv[iVtx][1] = giro[1]; pv[iVtx++][2] = orig[2];
      pv[iVtx][0] = giro[0]; pv[iVtx][1] = orig[1]; pv[iVtx++][2] = orig[2];
      pv[iVtx][0] = orig[0]; pv[iVtx][1] = orig[1]; pv[iVtx++][2] = orig[2];
      pn[iFcs][0] = 0.f; pn[iFcs][1] = 0.f; pn[iFcs++][2] = -1.f;
    }
    // +Z face
    if ( it->m_wall == FaceWall::FacePZ ) {
      pv[iVtx][0] = orig[0]; pv[iVtx][1] = orig[1]; pv[iVtx++][2] = giro[2];
      pv[iVtx][0] = giro[0]; pv[iVtx][1] = orig[1]; pv[iVtx++][2] = giro[2];
      pv[iVtx][0] = giro[0]; pv[iVtx][1] = giro[1]; pv[iVtx++][2] = giro[2];
      pn[iFcs][0] = 0.f; pn[iFcs][1] = 0.f; pn[iFcs++][2] = 1.f;
      pv[iVtx][0] = giro[0]; pv[iVtx][1] = giro[1]; pv[iVtx++][2] = giro[2];
      pv[iVtx][0] = orig[0]; pv[iVtx][1] = giro[1]; pv[iVtx++][2] = giro[2];
      pv[iVtx][0] = orig[0]; pv[iVtx][1] = orig[1]; pv[iVtx++][2] = giro[2];
      pn[iFcs][0] = 0.f; pn[iFcs][1] = 0.f; pn[iFcs++][2] = 1.f;
    }
  } // end of for(it)

  // set color
  updateColor();

  // ok
  m_isoTrias->getPrivateMaterial()->setRenderMode(m_showType);
  setHilight(m_hilight);
  m_updatedStp = m_requestedStp;
  return true;
}

#undef VEC_LEN


/* vsnMethodObj methods */

bool vsnMethod_OctVol_isoCells::update(const bool force) {
  return updateStep(m_requestedStp, force);
}

void vsnMethod_OctVol_isoCells::reloaded() {
  adjustRange(false);
  vsnMethodObj::reloaded();
}

vsnMethodPP* vsnMethod_OctVol_isoCells::getParamPanel(wxPanel* pp) {
  vsnMPP_OctVol_isoCells* pp_isoCells
    = new vsnMPP_OctVol_isoCells(pp, this);
  if ( ! pp_isoCells ) return NULL;
  return pp_isoCells;
}

void vsnMethod_OctVol_isoCells::setBaseColor(const vector4 cv) {
  memcpy(m_colour, cv, sizeof(vector4));
  //m_colour[3] = (float)((int)(m_colour[3] * 10) / 10.f);
  updateUI();
  
  if ( m_pColorBar )
    m_pColorBar->setBaseColor(m_colour);

  updateColor();
}


/* serialize : from vsnIoObject */

bool vsnMethod_OctVol_isoCells::parseXML(xmlNodePtr xnp) {
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
      else if ( xsN == string("iso_value") ) {
        float val = (float)atof(xsV.c_str());
        if ( ! setIsoValue(val) ) {
          ErrMsg(MsgERR, msgHdr + string("invalid value in param iso_value"));
          goto _NEXT_XML_NODE;
        }
      } // end of "iso_value"
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
      else if ( xsN == string("use_cmap_alpha") ) {
        bool ucma;
        if ( xsV == string("yes") ) ucma = true;
        else if ( xsV == string("no") ) ucma = false;
        else {
          ErrMsg(MsgERR, msgHdr
                 + string("invalid value in param use_cmap_alpha"));
          goto _NEXT_XML_NODE;
        }
        if ( ! setUseCMapAlpha(ucma) ) {
          ErrMsg(MsgERR, msgHdr + string("can't update use_cmap_alpha mode"));
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
    } // end of param

  _NEXT_XML_NODE:
    cur = cur->next;
  } // end of while(cur)

  return true;
}

bool vsnMethod_OctVol_isoCells::outputXML(std::ostream& os, const size_t ts) {
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

  // iso_value
  os << idts_2 << "<param name=\"iso_value\" value=\""
     << m_isoValue << "\" />" << endl;

  // use_cmap / use_cmap_alpha
  if ( ! m_useCMap ) {
    os << idts_2 << "<param name=\"use_cmap\" value=\"no\" />" << endl;
  }
  if ( m_useCMapAlpha ) {
    os << idts_2 << "<param name=\"use_cmap_alpha\" value=\"yes\" />" << endl;
  }

  // output base method params
  if ( ! vsnMethodObj::exportXMLNode(os, ts+2) ) {
    ErrMsg(MsgERR, msgHdr + string("base MethodObj exportXMLNode failed"));
    ret = false;
  }

  os << idts << "</method>" << endl;
  return true;
}

bool vsnMethod_OctVol_isoCells::commandXML(xmlNodePtr xnp) {
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
  else if ( nameStr == "set_iso_value" ) {
    float val = (float)atof(valueStr.c_str());
    if ( ! setIsoValue(val) ) {
      ErrMsg(MsgERR, msgHdr +
             string("command set_iso_value: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_iso_value"
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
  else if ( nameStr == "set_use_cmap_alpha" ) {
    bool ucma;
    if ( valueStr == string("yes") ) ucma = true;
    else if ( valueStr == string("no") ) ucma = false;
    else {
      ErrMsg(MsgERR, msgHdr
             + string("command set_use_cmap_alpha: invalid value"));
      return false;
    }
    if ( ! setUseCMapAlpha(ucma) ) {
      ErrMsg(MsgERR, msgHdr +
             string("command set_use_cmap_alpha: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_use_cmap_alpha"
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
  else {
    // not my command
    ErrMsg(MsgERR, msgHdr + string("unknown command: ") + nameStr);
    return false;
  }

  return true;
}

/* override vfrGroup method */

void vsnMethod_OctVol_isoCells::generateBbox() {
  vsnData_OctVol* pData = dynamic_cast<vsnData_OctVol*>(p_refData);
  if ( ! pData ) return;
  const CES::Vec3<float>* pbb = pData->getBbox();
  if ( ! pbb ) return;
  _bbox[0] = pbb[0];
  _bbox[1] = pbb[1];
}
