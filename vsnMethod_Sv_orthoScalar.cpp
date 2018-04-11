//
// vsnMethod_Sv_orthoScalar
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

#include "vsnMethod_Sv_orthoScalar.h"
#include "vsnOctTree.h" // for decomp vecIdx
#include "vsnUiView.h"
#include "vsnError.h"

using namespace std;
using namespace CES;
using namespace VSN;


//----------------------------------------------------------------
// class vsnMPP_Sv_orthoScalar
//----------------------------------------------------------------

BEGIN_EVENT_TABLE(vsnMPP_Sv_orthoScalar, wxPanel)
  EVT_TEXT_ENTER(MPP_Sv_orthoScalar_SlicerTxt,
		 vsnMPP_Sv_orthoScalar::OnEnterSlicerTxt)
  EVT_BUTTON(MPP_Sv_orthoScalar_SetSlicerBtn,
	     vsnMPP_Sv_orthoScalar::OnSetSlicerBtn)
  EVT_COMBOBOX(MPP_Sv_orthoScalar_SelDataLst,
               vsnMPP_Sv_orthoScalar::OnSelDataLst)
  EVT_CHECKLISTBOX(MPP_Sv_orthoScalar_VecDataChkLst,
                   vsnMPP_Sv_orthoScalar::OnVecDataChkLst)
  EVT_CHECKBOX(MPP_Sv_orthoScalar_UpdMinMaxChk,
	       vsnMPP_Sv_orthoScalar::OnUpdMinMaxChk)
END_EVENT_TABLE()


/* constructors / destructor */

vsnMPP_Sv_orthoScalar::vsnMPP_Sv_orthoScalar(wxPanel* parent, vsnMethodObj* pm)
  : vsnMethodPP(parent, pm),
    m_pSlicerList(NULL), m_pSetSlicerBtn(NULL)
{
  assert(parent);
  vsnMethod_Sv_orthoScalar* pmos = dynamic_cast<vsnMethod_Sv_orthoScalar*>(pm);
  assert(pmos);

  // setup gfxAct
  gfxAct_KI.setRefOrthoSlicer(pmos);
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
  m_pSlicerTxt = new wxTextCtrl(this, MPP_Sv_orthoScalar_SlicerTxt,
				wxT(""), wxDefaultPosition, wxDefaultSize,
				wxTE_READONLY|wxTE_PROCESS_ENTER);
  sizerH->Add(m_pSlicerTxt, 0, wxEXPAND|wxALL, 3);

  // orthoSlicer selector
  m_pSlicerList = new wxListBox(this, MPP_Sv_orthoScalar_SlicerLst);
  topsizer->Add(m_pSlicerList, 0, wxEXPAND|wxALL, 3);

  m_pSetSlicerBtn = new wxButton(this, MPP_Sv_orthoScalar_SetSlicerBtn,
				 wxT("set orthoSlicer"));
  topsizer->Add(m_pSetSlicerBtn, 0, wxALIGN_LEFT|wxALL, 3);

  // data selector
  topsizer->Add(5, 5);
  topsizer->Add(new wxStaticText(this, -1, wxT("select scalar data")),
                0, wxALIGN_LEFT|wxALL, 3);
   m_pSelDataLst = new wxComboBox(this, MPP_Sv_orthoScalar_SelDataLst,
                                 wxT(""), wxDefaultPosition, wxDefaultSize,
                                 0, NULL, wxCB_READONLY);
  topsizer->Add(m_pSelDataLst, 0, wxEXPAND|wxALL, 3);

  // vector data selector
  topsizer->Add(new wxStaticText(this, -1, wxT("vector components")),
                0, wxALIGN_LEFT|wxALL, 3);
  m_pVecDataChkLst = new wxCheckListBox(this,
					MPP_Sv_orthoScalar_VecDataChkLst);
  topsizer->Add(m_pVecDataChkLst, 0, wxEXPAND|wxALL, 3);

  // update minmax mode
  m_pUpdMinMaxChk = new wxCheckBox(this, MPP_Sv_orthoScalar_UpdMinMaxChk,
				   wxT("update lut minmax"));
  m_pUpdMinMaxChk->SetValue(TRUE);
  topsizer->Add(m_pUpdMinMaxChk, 0, wxALL, 3);

  SetSizer(topsizer);
  addTo(parent);
  topsizer->Layout();
  (void)update();
}

vsnMPP_Sv_orthoScalar::~vsnMPP_Sv_orthoScalar() {
}


/* interface */

bool vsnMPP_Sv_orthoScalar::update() {
  if ( ! m_pSlicerTxt || ! m_pSlicerList || ! m_pSetSlicerBtn ||
       ! m_pSelDataLst || ! m_pVecDataChkLst || ! m_pUpdMinMaxChk )
    return false;

  register size_t i;
  char txt[16];

  vsnMethod_Sv_orthoScalar* pm
    = dynamic_cast<vsnMethod_Sv_orthoScalar*>(p_method);
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

  // update minmax mode
  m_pUpdMinMaxChk->SetValue(pm->getUpdateMinMaxMode());

  return true;
}

void vsnMPP_Sv_orthoScalar::settlement() {
  vsnGfxView* pgv = gfxAct_KI.getGfxView();
  if ( pgv ) {
    pgv->popActions(this);
    gfxAct_KI.setGfxView(NULL);
  }
}


/* event handler */

void vsnMPP_Sv_orthoScalar::OnEnterSlicerTxt(wxCommandEvent& event) {
  if ( ! m_pSlicerTxt ) return;

  vsnMethod_Sv_orthoScalar* pm
    = dynamic_cast<vsnMethod_Sv_orthoScalar*>(p_method);
  if ( ! pm ) return;
  vsnMethod_Sv_orthoSlicer* posr = pm->getOrthoSlicer();
  if ( ! posr ) return;
  vsnUiView* puiv = getUiView();
  if ( ! puiv ) return;

  puiv->selectObj((vfrNode*)posr);
}

void vsnMPP_Sv_orthoScalar::OnSetSlicerBtn(wxCommandEvent& event) {
  if ( ! m_pSlicerList || ! m_pSetSlicerBtn || ! m_pSlicerTxt )
    return;

  WhichDataType sel = m_pSlicerList->GetSelection();
  if ( sel < 0 ) return;
  m_pSlicerTxt->SetValue(m_pSlicerList->GetString(sel));

  vsnMethod_Sv_orthoScalar* pm
    = dynamic_cast<vsnMethod_Sv_orthoScalar*>(p_method);
  if ( ! pm ) return;
  vsnData_Sv* pdo = dynamic_cast<vsnData_Sv*>(pm->getRefData());
  if ( ! pdo ) return;

  pm->setOrthoSlicer(pdo, sel);
  pm->chkNotice();
}

void vsnMPP_Sv_orthoScalar::OnSelDataLst(wxCommandEvent& event) {
  if ( ! m_pSelDataLst ) return;
  WhichDataType sel = event.GetInt();
  if ( sel < 0 ) return;

  wxString valStr = m_pSelDataLst->GetString(sel);
  if ( valStr == wxT("vector length") )
    sel = DATA_Veclen;

  vsnMethod_Sv_orthoScalar* pm
    = dynamic_cast<vsnMethod_Sv_orthoScalar*>(p_method);
  if ( ! pm ) return;
  if ( sel == pm->getSelectedData() ) return;

  if ( ! pm->setSelectedData(sel) ) {
    update();
    return;
  }
  pm->chkNotice();
}

void vsnMPP_Sv_orthoScalar::OnVecDataChkLst(wxCommandEvent& event) {
  if ( ! m_pVecDataChkLst ) return;
  int dlen = m_pVecDataChkLst->GetCount();
  if ( dlen < 3 ) return;

  vsnMethod_Sv_orthoScalar* pm
    = dynamic_cast<vsnMethod_Sv_orthoScalar*>(p_method);
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

void vsnMPP_Sv_orthoScalar::OnUpdMinMaxChk(wxCommandEvent& event) {
  if ( ! m_pUpdMinMaxChk ) return;
  bool val = m_pUpdMinMaxChk->GetValue();

  vsnMethod_Sv_orthoScalar* pm
    = dynamic_cast<vsnMethod_Sv_orthoScalar*>(p_method);
  if ( ! pm ) return;

  if ( pm->setUpdateMinMaxMode(val) )
    pm->chkNotice();
}


//----------------------------------------------------------------
// class vsnMethod_Sv_orthoScalar
//----------------------------------------------------------------

/* constructors / destructor */

vsnMethod_Sv_orthoScalar::vsnMethod_Sv_orthoScalar(const string& name)
  : vsnMethodObj(name), m_selectedData(DATA_None), m_updateMinMax(true),
    m_vecDataIdx(0,1,2), m_slice(NULL)
{
}

vsnMethod_Sv_orthoScalar::~vsnMethod_Sv_orthoScalar() {
  if ( m_slice )
    delete m_slice;
}


/* vsnTimeSeriesMethodIF methods */

#define IDX(i, j) (sliceIndices[sliceSize.x*(j)+(i)])

bool vsnMethod_Sv_orthoScalar::updateStep(const int stp,
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

  // slice data
  if ( ! m_slice ) {
    m_slice = new vfrIndexPolygons();
    if ( ! m_slice ) {
      ErrMsg(MsgERR, getMethodType() + string("[") + getName()
	     + string("]: memory allocation failed"));
      return false;
    }
    m_slice->alcMaterial();
    m_slice->setNormalMode(AT_PER_FACE);
    addChild(m_slice);
  }
  m_slice->getPrivateMaterial()->setRenderMode(RT_NONE);
  if ( ! m_show ) {
    return true;
  }

  // check slicer
  if ( ! p_osr ) return true;
  Point2 sliceSize = p_osr->getSliceSize();
  register size_t vtxNm = sliceSize.x * sliceSize.y;
  register size_t faceNm = (sliceSize.x -1) * (sliceSize.y -1);
  register size_t idcNm = faceNm * 5;
  if ( faceNm < 1 ) return true;
  const int* const sliceIndices = p_osr->getSliceIndices();
  if ( ! sliceIndices ) {
    ErrMsg(MsgERR, getMethodType() + string("[") + getName()
	   + string("]: can't get slice indices data"));
    return false;
  }

  // check selected data
  size_t dlen = pData->getDataLen();
  if ( m_selectedData == DATA_Veclen ) {
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

  // alloc slice datas
  if ( ! m_slice->alcVerts(vtxNm) || ! m_slice->alcColors(vtxNm) ||
       ! m_slice->alcIndices(idcNm) ) {
    return false;
  }
  vector3* vl = m_slice->getVerts();
  vector4* cl = m_slice->getColors();
  int* il = m_slice->getIndices();
  register int i, j, idx, c;

  // set slice verts
  for ( i = 0; i < vtxNm; i++ ) {
    idx = sliceIndices[i] * 3;
    vl[i][0] = gptr[idx   ];
    vl[i][1] = gptr[idx +1];
    vl[i][2] = gptr[idx +2];
  } // end of for(i)

  // set slice colors
  if ( m_selectedData == DATA_Veclen ) {
    for ( i = 0; i < vtxNm; i++ ) {
      idx = sliceIndices[i] * dlen;
      Vec3<float> vv;
      vv.m_v[0] = dptr[idx + m_vecDataIdx.m_v[0]];
      vv.m_v[1] = dptr[idx + m_vecDataIdx.m_v[1]];
      vv.m_v[2] = dptr[idx + m_vecDataIdx.m_v[2]];
      c = m_lut.getValIdx(vv.Length());
      cl[i][0] = m_lut.lutEntry[c*4  ];
      cl[i][1] = m_lut.lutEntry[c*4+1];
      cl[i][2] = m_lut.lutEntry[c*4+2];
      cl[i][3] = 1.f;
    } // end of for(i)
    m_slice->setColorMode(AT_PER_VERTEX);
  }
  else if ( m_selectedData > 0 && m_selectedData <= dlen ) {
    for ( i = 0; i < vtxNm; i++ ) {
      idx = sliceIndices[i] * dlen + m_selectedData -1;
      c = m_lut.getValIdx(dptr[idx]);
      cl[i][0] = m_lut.lutEntry[c*4  ];
      cl[i][1] = m_lut.lutEntry[c*4+1];
      cl[i][2] = m_lut.lutEntry[c*4+2];
      cl[i][3] = 1.f;
    } // end of for(i)
    m_slice->setColorMode(AT_PER_VERTEX);
  }
  else {
    m_slice->setColorMode(AT_WHOLE);
  }

  // set slice indices
  idx = 0;
  for ( j = 0; j < sliceSize.y -1; j++ ) {
    for ( i = 0; i < sliceSize.x -1; i++ ) {
      if ( maskptr &&
	   (maskptr[IDX(i,j)] || maskptr[IDX(i+1,j)] ||
	    maskptr[IDX(i,j+1)] || maskptr[IDX(i+1,j+1)]) ) continue;
      il[idx++] = sliceSize.x*(j)+(i);
      il[idx++] = sliceSize.x*(j)+(i+1);
      il[idx++] = sliceSize.x*(j+1)+(i+1);
      il[idx++] = sliceSize.x*(j+1)+(i);
      il[idx++] = VFR_END_OF_ELEM;
    } // end of for(i)
  } // end of for(j)

  m_slice->alcIndices(idx);
  m_slice->generateBbox();
  m_slice->generateNormals();

  // ok
  m_slice->getPrivateMaterial()->setRenderMode(m_showType);
  setBaseColor(m_colour);
  setHilight(m_hilight);
  m_updatedStp = m_requestedStp;
  return true;
}


/* methods */

void vsnMethod_Sv_orthoScalar::adjustRange() {
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

bool vsnMethod_Sv_orthoScalar::setSelectedData(const WhichDataType sd) {
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

bool vsnMethod_Sv_orthoScalar::setVecDataIdx(const CES::Vec3<int>& vdidx) {
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

bool vsnMethod_Sv_orthoScalar::isValidVecData() const {
  vsnData_Sv* pData = dynamic_cast<vsnData_Sv*>(p_refData);
  if ( ! pData ) return false;

  size_t dlen = pData->getDataLen();
  if ( m_vecDataIdx[0] < 0 || m_vecDataIdx[0] >= dlen ) return false;
  if ( m_vecDataIdx[1] < 0 || m_vecDataIdx[1] >= dlen ) return false;
  if ( m_vecDataIdx[2] < 0 || m_vecDataIdx[2] >= dlen ) return false;
  return true;
}

bool vsnMethod_Sv_orthoScalar::setUpdateMinMaxMode(const bool mode) {
  if ( mode == m_updateMinMax ) return true;
  m_updateMinMax = mode;

  // data range
  adjustRange();

  if ( ! update() ) return false;
  updateUI();
  return true;
}


/* from vsnMethodObj */

bool vsnMethod_Sv_orthoScalar::update(const bool force) {
  return updateStep(m_requestedStp, force);
}

void vsnMethod_Sv_orthoScalar::reloaded() {
  adjustRange();
  vsnMethodObj::reloaded();
}

vsnMethodPP* vsnMethod_Sv_orthoScalar::getParamPanel(wxPanel* pp) {
  vsnMPP_Sv_orthoScalar* pp_orthoScalar = new vsnMPP_Sv_orthoScalar(pp, this);
  if ( ! pp_orthoScalar ) return NULL;
  return pp_orthoScalar;
}


/* serialize : from vsnIoObject */

bool vsnMethod_Sv_orthoScalar::parseXML(xmlNodePtr xnp) {
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
    } // end of param

  _NEXT_XML_NODE:
    cur = cur->next;
  } // end of while(cur)

  return true;
}

bool vsnMethod_Sv_orthoScalar::outputXML(std::ostream& os, const size_t ts)
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

  // output base method params
  if ( ! vsnMethodObj::exportXMLNode(os, ts+2) ) {
    ErrMsg(MsgERR, msgHdr + string("base MethodObj exportXMLNode failed"));
    ret = false;
  }

  os << idts << "</method>" << endl;
  return ret;
}

bool vsnMethod_Sv_orthoScalar::commandXML(xmlNodePtr xnp) {
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


/* from vsnRef_Sv_orthoSlicer */

void vsnMethod_Sv_orthoScalar::noticeUpdate() {
  update();
}
