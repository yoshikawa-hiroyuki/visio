//
// vsnMethod_OctVol_volren
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

#include <GL/glew.h>
#include "vsnMethod_OctVol_volren.h"
#include "vsnUiView.h"
#include "vsnColorBar.h"
#include "vsnError.h"

using namespace std;
using namespace CES;
using namespace NVR;
using namespace VSN;

// do reduce ?
#undef DO_REDUCE


//----------------------------------------------------------------
// class vsnMPP_OctVol_volren
//----------------------------------------------------------------

BEGIN_EVENT_TABLE(vsnMPP_OctVol_volren, wxPanel)
  EVT_COMBOBOX(MPP_OctVol_volren_SelDataLst,
               vsnMPP_OctVol_volren::OnSelDataLst)
  EVT_CHECKLISTBOX(MPP_OctVol_volren_VecDataChkLst,
                   vsnMPP_OctVol_volren::OnVecDataChkLst)
  EVT_CHECKBOX(MPP_OctVol_volren_UpdMinMaxChk,
               vsnMPP_OctVol_volren::OnUpdMinMaxChk)
  EVT_CHECKBOX(MPP_OctVol_volren_InterpolateChk,
               vsnMPP_OctVol_volren::OnInterpolateChk)
END_EVENT_TABLE()


/* constructors / destructor */

vsnMPP_OctVol_volren::vsnMPP_OctVol_volren(wxPanel* parent, vsnMethodObj* pm)
  : vsnMethodPP(parent, pm)
{
  assert(parent);
  assert(dynamic_cast<vsnMethod_OctVol_volren*>(pm));

  wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer* sizerH;

  // data selector
  topsizer->Add(new wxStaticText(this, -1, wxT("select scalar data")),
                0, wxALIGN_LEFT|wxALL, 3);
  m_pSelDataLst = new wxComboBox(this, MPP_OctVol_volren_SelDataLst,
                                 wxT(""), wxDefaultPosition, wxDefaultSize,
                                 0, NULL, wxCB_READONLY);
  topsizer->Add(m_pSelDataLst, 0, wxEXPAND|wxALL, 3);

  // vector data selector
  topsizer->Add(new wxStaticText(this, -1, wxT("vector components")),
                0, wxALIGN_LEFT|wxALL, 3);
  m_pVecDataChkLst = new wxCheckListBox(this, MPP_OctVol_volren_VecDataChkLst);
  topsizer->Add(m_pVecDataChkLst, 0, wxEXPAND|wxALL, 3);

  // update minmax mode
  m_pUpdMinMaxChk = new wxCheckBox(this, MPP_OctVol_volren_UpdMinMaxChk,
                                   wxT("update minmax"));
  m_pUpdMinMaxChk->SetValue(TRUE);
  topsizer->Add(m_pUpdMinMaxChk, 0, wxALL, 3);

  // interpolate mode
  m_pInterpolateChk = new wxCheckBox(this, MPP_OctVol_volren_InterpolateChk,
				     wxT("interpolate"));
  m_pInterpolateChk->SetValue(FALSE);
  topsizer->Add(m_pInterpolateChk, 0, wxALL, 3);

  // post process
  SetSizer(topsizer);
  addTo(parent);
  topsizer->Layout();
  (void)update();
}

vsnMPP_OctVol_volren::~vsnMPP_OctVol_volren() {
}


/* interface */

bool vsnMPP_OctVol_volren::update() {
  if ( ! m_pSelDataLst || ! m_pVecDataChkLst ||
       ! m_pUpdMinMaxChk || ! m_pInterpolateChk )
    return false;

  vsnMethod_OctVol_volren* pm
    = dynamic_cast<vsnMethod_OctVol_volren*>(p_method);
  if ( ! pm ) return false;
  vsnData_OctVol* pdo = dynamic_cast<vsnData_OctVol*>(pm->getRefData());
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

    Vec3<int> vidx = pm->getVecDataIdx();
    if ( vidx[0] >= 0 && vidx[0] < dlen ) m_pVecDataChkLst->Check(vidx[0]);
    if ( vidx[1] >= 0 && vidx[1] < dlen ) m_pVecDataChkLst->Check(vidx[1]);
    if ( vidx[2] >= 0 && vidx[2] < dlen ) m_pVecDataChkLst->Check(vidx[2]);
  }

  // update minmax mode
  m_pUpdMinMaxChk->SetValue(pm->getUpdateMinMaxMode());

  // interpolate mode
  m_pInterpolateChk->SetValue(pm->getInterpolateMode());

  return true;
}


/* event handler */

void vsnMPP_OctVol_volren::OnSelDataLst(wxCommandEvent& event) {
  if ( ! m_pSelDataLst ) return;
  WhichDataType sel = event.GetInt();
  if ( sel < 0 ) return;

  wxString valStr = m_pSelDataLst->GetString(sel);
  if ( valStr == wxT("vector length") )
    sel = DATA_Veclen;

  vsnMethod_OctVol_volren* pm
    = dynamic_cast<vsnMethod_OctVol_volren*>(p_method);
  if ( ! pm ) return;
  if ( sel == pm->getSelectedData() ) return;

  if ( ! pm->setSelectedData(sel) ) {
    update();
    return;
  }
  pm->chkNotice();
}

void vsnMPP_OctVol_volren::OnVecDataChkLst(wxCommandEvent& event) {
  if ( ! m_pVecDataChkLst ) return;
  int dlen = m_pVecDataChkLst->GetCount();
  if ( dlen < 3 ) return;

  vsnMethod_OctVol_volren* pm
    = dynamic_cast<vsnMethod_OctVol_volren*>(p_method);
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

void vsnMPP_OctVol_volren::OnUpdMinMaxChk(wxCommandEvent& event) {
  if ( ! m_pUpdMinMaxChk ) return;
  bool val = m_pUpdMinMaxChk->GetValue();

  vsnMethod_OctVol_volren* pm
    = dynamic_cast<vsnMethod_OctVol_volren*>(p_method);
  if ( ! pm ) return;

  if ( pm->setUpdateMinMaxMode(val) )
    pm->chkNotice();
}

void vsnMPP_OctVol_volren::OnInterpolateChk(wxCommandEvent& event) {
  if ( ! m_pInterpolateChk ) return;
  bool val = m_pInterpolateChk->GetValue();

  vsnMethod_OctVol_volren* pm
    = dynamic_cast<vsnMethod_OctVol_volren*>(p_method);
  if ( ! pm ) return;

  if ( pm->setInterpolateMode(val) )
    pm->chkNotice();
}


//----------------------------------------------------------------
// class vsnMethod_OctVol_volren
//----------------------------------------------------------------

/* constructors / destructor */

vsnMethod_OctVol_volren::
vsnMethod_OctVol_volren(const std::string& name)
  : vsnMethodObj(name), m_pRender(NULL), m_rsmpl_updStamp(-1),
    m_selectedData(DATA_None), m_vecDataIdx(0,1,2),
    m_updateMinMax(true), m_interpolate(false)
{
  m_pRender = new vsnNvrVolumeRender();
  assert(m_pRender);
#if defined(DO_REDUCE)
  m_pRender->setReduceMode(true);
#else
  m_pRender->setReduceMode(false);
#endif
  m_pRender->alcMaterial();
#if 0
  if ( ! m_pRender->getOglChkd() ) {
    ErrMsg(MsgERR, getMethodType() + string("[") + getName()
           + string("]: required OpenGL Extensions not supported"));
    return;
  }
#endif
  addChild(m_pRender);

  setPickMode(PT_NONE);
}

vsnMethod_OctVol_volren::~vsnMethod_OctVol_volren() {
  if ( m_pRender ) {
    if ( p_refData ) {
      vsnScene* psc = p_refData->getAncestorScene();
      if ( psc ) psc->delVolRender(m_pRender, this);
    }
    delete m_pRender;
  }
  if ( ! m_pdl.empty() ) {
    deque<unsigned char*>::iterator it;
    for ( it = m_pdl.begin(); it != m_pdl.end(); it++ )
      if ( *it ) VFR::DeAllocate(*it);
    m_pdl.clear();
  }
}


/* methods */

void vsnMethod_OctVol_volren::adjustRange() {
  bool needRangeUpd = false;

  vsnData_OctVol* pData = dynamic_cast<vsnData_OctVol*>(p_refData);
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

bool vsnMethod_OctVol_volren::setSelectedData(const WhichDataType sd) {
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

bool vsnMethod_OctVol_volren::setVecDataIdx(const CES::Vec3<int>& vdidx) {
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

bool vsnMethod_OctVol_volren::isValidVecData() const {
  vsnData_OctVol* pData = dynamic_cast<vsnData_OctVol*>(p_refData);
  if ( ! pData ) return false;

  size_t dlen = pData->getDataLen();
  if ( m_vecDataIdx[0] < 0 || m_vecDataIdx[0] >= dlen ) return false;
  if ( m_vecDataIdx[1] < 0 || m_vecDataIdx[1] >= dlen ) return false;
  if ( m_vecDataIdx[2] < 0 || m_vecDataIdx[2] >= dlen ) return false;
  return true;
}

bool vsnMethod_OctVol_volren::setUpdateMinMaxMode(const bool mode) {
  if ( m_updateMinMax == mode ) return true;
  m_updateMinMax = mode;

  // data range
  adjustRange();

  if ( ! update() ) return false;
  updateUI();
  return true;
}

bool vsnMethod_OctVol_volren::setInterpolateMode(const bool mode) {
  if ( m_interpolate == mode ) return true;
  m_interpolate = mode;
  m_rsmpl_updStamp = -1;

  if ( ! update() ) return false;
  updateUI();
  return true;
}

bool vsnMethod_OctVol_volren::doResample(vsnOctTree* poct,
					 deque<vsnRsmplOctVol::RsmplBlk*>& bl)
{
  if ( ! poct || bl.size() < 1 ) return false;

  float minMax[2];
  if ( poct->m_dataLen > 0 ) {
    int i;
    m_minLst.clear();
    for ( i = 0; i < poct->m_dataLen; i++ ) {
      poct->GetMinMax(i, minMax);
      m_minLst.push_back(minMax[0] - 1e-3f);
    } // end of for(i)
  }

  // reduce to (MaxTextureSize * 2)
  register size_t texMemSize = nvrRender::MaxTextureSize() * 2;
  if ( texMemSize < 1 ) return false;
#if defined(DO_REDUCE)
  register size_t totalSize = m_rsmpl.GetTotalProxySize();
#else
  register size_t totalSize = 0;
#endif
  while ( totalSize > texMemSize ) {
    /* sort by level */
    printf("Sort: "); fflush(stdout);
    m_rsmpl.SortBlkByLevel();
    if ( ! bl[0] ) break;
    printf("level of bl[0] = %lu\n", bl[0]->m_level); fflush(stdout);
    
    /* test reduce the block of the max level */
    if ( bl[0]->m_level < 1 ) break;
    if ( bl[0]->m_dims.m_v[0] < 2 ||
	 bl[0]->m_dims.m_v[1] < 2 || bl[0]->m_dims.m_v[2] < 2 ) break;
    bl[0]->m_level --;
    bl[0]->m_dims.m_v[0] >>= 1;
    bl[0]->m_dims.m_v[1] >>= 1;
    bl[0]->m_dims.m_v[2] >>= 1;
    
    /* calculate total size */
    totalSize = m_rsmpl.GetTotalProxySize();
  } // end of while()

  // resample
  if ( poct->m_dataLen < 1 ) return true;
  deque<vsnRsmplOctVol::RsmplBlk*>::iterator it;
  for ( it = bl.begin(); it != bl.end(); it++ ) {
    if ( ! *it ) continue;
    vsnRsmplOctVol::RsmplBlk* b = (*it);
    Vec3<float> pos, dpos;
    dpos[0] = (b->m_bbox[1][0]-b->m_bbox[0][0])/(float)(b->m_dims[0]-1);
    dpos[1] = (b->m_bbox[1][1]-b->m_bbox[0][1])/(float)(b->m_dims[1]-1);
    dpos[2] = (b->m_bbox[1][2]-b->m_bbox[0][2])/(float)(b->m_dims[2]-1);
    register size_t ii, jj, kk, ll, idx2;
    idx2 = 0;
    for ( kk = 0; kk < b->m_dims[2]; kk++ ) {
      if ( kk == b->m_dims[2] - 1 ) pos[2] = b->m_bbox[1][2];
      else pos[2] = b->m_bbox[0][2] + (float)dpos[2] * kk;
      for ( jj = 0; jj < b->m_dims[1]; jj++ ) {
	if ( jj == b->m_dims[1] - 1 ) pos[1] = b->m_bbox[1][1];
	else pos[1] = b->m_bbox[0][1] + (float)dpos[1] * jj;
	for ( ii = 0; ii < b->m_dims[0]; ii++ ) {
	  if ( ii == b->m_dims[0] - 1 ) pos[0] = b->m_bbox[1][0];
	  else pos[0] = b->m_bbox[0][0] + (float)dpos[0] * ii;

	  vsnOctTree::Node* pnode = poct->FindByPos(pos);
	  if ( ! pnode ) {
	    for ( ll = 0; ll < poct->m_dataLen; ll++ ) {
	      b->m_pData[idx2] = m_minLst[ll];
	      idx2 ++;
	    } // end of for(ll)
	    continue;
	  }
	  if ( m_interpolate ) {
	    for ( ll = 0; ll < poct->m_dataLen; ll++ ) {
	      float dval;
	      if ( poct->InterpolateData(pos, pnode, ll, dval) )
		b->m_pData[idx2] = dval;
	      idx2 ++;
	    } // end of for(ll)
	  } else {
	    memcpy(&b->m_pData[idx2], pnode->m_pData,
		   sizeof(float)*poct->m_dataLen);
	    idx2 += poct->m_dataLen;
	  }
	} // end of for(ii)
      } // end of for(jj)
    } // end of for(kk)

  } // end of for(it)
  return true;
}


/* vsnTimeSeriesMethodIF methods */

bool vsnMethod_OctVol_volren::updateStep(const int stp,
					 const bool force, const bool cascade)
{
  vsnData_OctVol* pData = dynamic_cast<vsnData_OctVol*>(p_refData);
  if ( ! pData ) return false;
  m_requestedStp = stp;
  if ( m_requestedStp < 0 )
    m_requestedStp = pData->getCurrentStepIdx();

  if ( force ) m_updatedStp = -1;
  if ( m_updatedStp == m_requestedStp ) return true;
  m_updatedStp = -1;

  // rederer
  if ( ! m_pRender ) return false;
  m_pRender->getPrivateMaterial()->setRenderMode(RT_NONE);
  if ( ! m_show ) {
    return true;
  }
  nvrRender& render = m_pRender->getRender();
  deque<nvrBrick*>& bl = render.GetBrickList();

  // check selected data
  size_t dlen = pData->getDataLen();
  float dr[2] = {m_lut.minVal, m_lut.maxVal};
  float minMax[2];
  if ( m_selectedData == DATA_None ) return true;
  else if ( m_selectedData == DATA_Veclen ) {
    if ( ! isValidVecData() ) return true;
    pData->getOctTree()->GetMinMax(m_vecDataIdx.m_v[0], minMax);  
  }
  else if ( m_selectedData > 0 && m_selectedData <= dlen ) {
    minMax[0] = dr[0];
  }
  else return true;

  // resample and renderer initialize
  Vec3<size_t> maxDims;
  if ( m_rsmpl_updStamp != pData->getOctTreeUpdatedStamp() ) {
    vsnOctTree* pot = const_cast<vsnOctTree*>(pData->getOctTree());
    if ( ! m_rsmpl.Setup(pot, true, false) ) {
      ErrMsg(MsgERR, getMethodType() + string("[") + getName()
             + string("]: resample failed"));
      return false;
    }

    maxDims = m_rsmpl.getMaxDims();
    if ( ! m_pRender->Initialize(maxDims, false) ) {
      ErrMsg(MsgERR, getMethodType() + string("[") + getName()
           + string("]: volume render initialization failed"));
      return false;
    }

    if ( ! render.GetBspTree()->AdjustBbox(m_rsmpl.getBbox()) ) {
      ErrMsg(MsgERR, getMethodType() + string("[") + getName()
           + string("]: adjust bbox failed"));
      return false;
    }

    if ( ! doResample(pot, m_rsmpl.getRsmplBlkList()) ) {
      ErrMsg(MsgERR, getMethodType() + string("[") + getName()
           + string("]: resample OctTree failed"));
      return false;
    }

    m_rsmpl_updStamp = pData->getOctTreeUpdatedStamp();
  }

  // prepare data pool
  const std::deque<vsnRsmplOctVol::RsmplBlk*>& rsmplBlkLst
    = m_rsmpl.getRsmplBlkList();
  if ( rsmplBlkLst.empty() ) {
    ErrMsg(MsgERR, getMethodType() + string("[") + getName()
           + string("]: resample failed, no valid resample block"));
    return false;
  }

  m_pdl.resize(rsmplBlkLst.size(), NULL);

  deque<nvrBrick*>::iterator itB;
  deque<unsigned char*>::iterator itPD;
  vsnRsmplOctVol::RsmplBlk* pRB;

  register float facA, facB;
  if ( fabs(dr[1] - dr[0]) < 1e-8 ) {facA = facB = 0.f;}
  else {
    facA = 254.f / (dr[1] - dr[0]);
    facB = facA * dr[0];
  }

  for ( itB = bl.begin(), itPD = m_pdl.begin();
	itB != bl.end() && itPD != m_pdl.end(); itB++, itPD++ ) {
    if ( ! *itB ) continue;
    const Vec3<float>* bbRB = (*itB)->GetBbox();
    pRB = m_rsmpl.FindByPos((bbRB[1] + bbRB[0]) * 0.5f);
    if ( ! pRB ) continue;

    Vec3<size_t> rbDim = pRB->m_dims;
    register size_t rbSz = rbDim[0] * rbDim[1] * rbDim[2];
    if ( rbSz < 1 ) continue;
    const Dim3 dm3(rbDim[0],rbDim[1],rbDim[2]);
    (*itB)->SetDims(dm3);
    (*itPD) = (unsigned char*)VFR::ReAllocate(*itPD, rbSz);
    if ( ! *itPD ) {
      ErrMsg(MsgERR, getMethodType() + string("[") + getName()
	     + string("]: memory allocation failed"));
      return false;
    }
    unsigned char* sptr = *itPD;
    const float* dptr = pRB->m_pData;

    register size_t i, idx;
    if ( m_selectedData == DATA_Veclen ) {
      Vec3<float> vv;
      for ( i = 0; i < rbSz; i++ ) {
	idx = i * dlen;
	if ( minMax[0] > dptr[idx+m_vecDataIdx.m_v[0]] ) {
	  sptr[i] = 0;
	} else {
	  vv.m_v[0] = dptr[idx + m_vecDataIdx.m_v[0]];
	  vv.m_v[1] = dptr[idx + m_vecDataIdx.m_v[1]];
	  vv.m_v[2] = dptr[idx + m_vecDataIdx.m_v[2]];
	  sptr[i] = (unsigned char)(vv.Length() * facA - facB) + 1;
	}
      } // end of for(i)
    }
    else {
      for ( i = 0; i < rbSz; i++ ) {
	idx = i * dlen + m_selectedData -1;
	if ( minMax[0] > dptr[idx] )
	  sptr[i] = 0;
	else
	  sptr[i] = (unsigned char)(dptr[idx] * facA - facB) + 1;
      } // end of for(i)
    }
  } // end of for(itB,itPD)

  // set lut
  if ( ! m_pRender->SetLut(m_lut) ) {
    ErrMsg(MsgERR, getMethodType() + string("[") + getName()
           + string("]: can't set LUT to the volume render"));
    return false;
  }

  // set volume and geometory
  if ( ! m_pRender->SetVolume(m_pdl, pData->getOctTree()->m_bbox) ) {
    ErrMsg(MsgERR, getMethodType() + string("[") + getName()
           + string("]: can't set data to the volume render"));
    return false;
  }

  // ok
  m_pRender->getPrivateMaterial()->setRenderMode(m_showType);
  m_updatedStp = m_requestedStp;
  return true;
}


/* vsnMethodObj methods */

bool vsnMethod_OctVol_volren::update(const bool force) {
  return updateStep(m_requestedStp, force);
}

void vsnMethod_OctVol_volren::reloaded() {
  adjustRange();
  vsnMethodObj::reloaded();
}

vsnMethodPP* vsnMethod_OctVol_volren::getParamPanel(wxPanel* pp) {
  vsnMPP_OctVol_volren* pp_volren
    = new vsnMPP_OctVol_volren(pp, this);
  if ( ! pp_volren ) return NULL;
  return pp_volren;
}

void vsnMethod_OctVol_volren::setBaseColor(const vector4 cv) {
  memcpy(m_colour, cv, sizeof(vector4));
  updateUI();
  
  if ( m_pColorBar )
    m_pColorBar->setBaseColor(m_colour);

  chkNotice();
}

void vsnMethod_OctVol_volren::setRefData(vsnDataObj* prd) {
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


/* serialize : from vsnIoObject */

bool vsnMethod_OctVol_volren::parseXML(xmlNodePtr xnp) {
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
      else if ( xsN == string("interpolate") ) {
        bool mintp;
        if ( xsV == string("yes") ) mintp = true;
        else if ( xsV == string("no") ) mintp = false;
        else {
          ErrMsg(MsgERR, msgHdr +string("invalid value in param interpolate"));
          goto _NEXT_XML_NODE;
        }
        if ( ! setInterpolateMode(mintp) ) {
          ErrMsg(MsgERR, msgHdr + string("can't update interpolate mode"));
          goto _NEXT_XML_NODE;
        }
      } // end of "interpolate"
    } // end of param

  _NEXT_XML_NODE:
    cur = cur->next;
  } // end of while(cur)

  return true;
}

bool vsnMethod_OctVol_volren::outputXML(std::ostream& os, const size_t ts) {
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

  // interpolate
  if ( m_interpolate ) {
    os << idts_2 << "<param name=\"interpolate\" value=\"yes\" />" << endl;
  }

  // output base method params
  if ( ! vsnMethodObj::exportXMLNode(os, ts+2) ) {
    ErrMsg(MsgERR, msgHdr + string("base MethodObj exportXMLNode failed"));
    ret = false;
  }

  os << idts << "</method>" << endl;
  return true;
}

bool vsnMethod_OctVol_volren::commandXML(xmlNodePtr xnp) {
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
  else if ( nameStr == "set_interpolate" ) {
    bool mintp;
    if ( valueStr == string("yes") ) mintp = true;
    else if ( valueStr == string("no") ) mintp = false;
    else {
      ErrMsg(MsgERR, msgHdr +string("command set_interpolate: invalid value"));
      return false;
    }
    if ( ! setInterpolateMode(mintp) ) {
      ErrMsg(MsgERR, msgHdr +
             string("command set_interpolate: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_interpolate"
  else {
    // not my command
    ErrMsg(MsgERR, msgHdr + string("unknown command: ") + nameStr);
    return false;
  }

  return true;
}

