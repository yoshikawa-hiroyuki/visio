//
// vsnMethod_Scatter_plotAsLineStrip
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

#include "vsnMethod_Scatter_plotAsLineStrip.h"
#include "vsnUiView.h"
#include "vsnError.h"
#include "vsnOctTree.h" // for decomp vecIdx

using namespace std;
using namespace CES;
using namespace VSN;


//----------------------------------------------------------------
// class vsnMPP_Scatter_plotAsLineStrip
//----------------------------------------------------------------
BEGIN_EVENT_TABLE(vsnMPP_Scatter_plotAsLineStrip, wxPanel)
  EVT_RADIOBOX(MPP_Scatter_plotAsLineStrip_LineTypeRadio,
               vsnMPP_Scatter_plotAsLineStrip::OnLineTypeRadio)
  EVT_COMBOBOX(MPP_Scatter_plotAsLineStrip_SelDataLst,
               vsnMPP_Scatter_plotAsLineStrip::OnSelDataLst)
  EVT_CHECKLISTBOX(MPP_Scatter_plotAsLineStrip_VecDataChkLst,
                   vsnMPP_Scatter_plotAsLineStrip::OnVecDataChkLst)
  EVT_CHECKBOX(MPP_Scatter_plotAsLineStrip_UpdMinMaxChk,
               vsnMPP_Scatter_plotAsLineStrip::OnUpdMinMaxChk)
  EVT_TEXT_ENTER(MPP_Scatter_plotAsLineStrip_LineWidthTxt,
                 vsnMPP_Scatter_plotAsLineStrip::OnLineWidthTxt)
  EVT_CHECKBOX(MPP_Scatter_plotAsLineStrip_AntiAliasChk,
               vsnMPP_Scatter_plotAsLineStrip::OnAntiAliasChk)
END_EVENT_TABLE()


/* constructors / destructor */

vsnMPP_Scatter_plotAsLineStrip::
vsnMPP_Scatter_plotAsLineStrip(wxPanel* parent, vsnMethodObj* pm)
: vsnMethodPP(parent, pm)
{
  assert(parent);
  assert(dynamic_cast<vsnMethod_Scatter_plotAsLineStrip*>(pm));

  // create widgets
  wxString ritems[] = {wxString(wxT("regular")), wxString(wxT("dot")),
		       wxString(wxT("dash")), wxString(wxT("dot-dash 1")),
		       wxString(wxT("dot-dash 2"))};
  m_pLineTypeRadio
    = new wxRadioBox(this, MPP_Scatter_plotAsLineStrip_LineTypeRadio,
		     wxT("line type"), wxDefaultPosition,
		     wxDefaultSize, 5, ritems, 1, wxRA_SPECIFY_COLS);
  assert(m_pLineTypeRadio);

  m_pSelDataLst
    = new wxComboBox(this, MPP_Scatter_plotAsLineStrip_SelDataLst,
		     wxT(""), wxDefaultPosition, wxDefaultSize,
		     0, NULL, wxCB_READONLY);
  assert(m_pSelDataLst);

  m_pVecDataChkLst
    = new wxCheckListBox(this, MPP_Scatter_plotAsLineStrip_VecDataChkLst);
  assert(m_pVecDataChkLst);

  m_pUpdMinMaxChk
    = new wxCheckBox(this, MPP_Scatter_plotAsLineStrip_UpdMinMaxChk,
		     wxT("update lut minmax"));
  assert(m_pUpdMinMaxChk);

  m_pLineWidthTxt
    = new wxTextCtrl(this, MPP_Scatter_plotAsLineStrip_LineWidthTxt,
		     wxT(""), wxDefaultPosition, wxDefaultSize,
		     wxTE_PROCESS_ENTER);
  assert(m_pLineWidthTxt);

  m_pAntiAliasChk
    = new wxCheckBox(this, MPP_Scatter_plotAsLineStrip_AntiAliasChk,
		     wxT("anti-alias line"));
  assert(m_pAntiAliasChk);

  // prepare sizers
  wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL); assert(topsizer);
  wxBoxSizer* sizerH;

  // line type
  topsizer->Add(m_pLineTypeRadio, 0, wxALL, 3);

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

vsnMPP_Scatter_plotAsLineStrip::~vsnMPP_Scatter_plotAsLineStrip() {
}


/* interface */

bool vsnMPP_Scatter_plotAsLineStrip::update() {
  register size_t i;
  char txt[64];

  vsnMethod_Scatter_plotAsLineStrip* pm
    = dynamic_cast<vsnMethod_Scatter_plotAsLineStrip*>(p_method);
  if ( ! pm ) return false;
  vsnNumericalDataIF* pdo
    = dynamic_cast<vsnNumericalDataIF*>(pm->getRefData());
  if ( ! pdo ) return false;

  // line type
  VFR::StippleType rt = pm->getLineType();
  if ( rt < 0 || rt > 4 )
    m_pLineTypeRadio->SetSelection(ST_SOLID);
  else
    m_pLineTypeRadio->SetSelection(rt);

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

  // select scalar data
  if ( m_pSelDataLst->GetCount() < 1 ) {
    m_pSelDataLst->Append(wxT("None"));
    if ( dlen > 0 ) {
      for ( i = 0; i < dlen; i++ ) {
        sprintf(txt, "data%zd", i);
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
  float lw = pm->getLineWidth();
  sprintf(txt, "%g", lw);
  m_pLineWidthTxt->SetValue(vsnApp::ConvSysToWx(txt));  

  // anti-alias
  m_pAntiAliasChk->SetValue(pm->getAntiAliasMode());

  return true;
}


/* event handler */

void
vsnMPP_Scatter_plotAsLineStrip::OnLineTypeRadio(wxCommandEvent& event) {
  vsnMethod_Scatter_plotAsLineStrip* pm
    = dynamic_cast<vsnMethod_Scatter_plotAsLineStrip*>(p_method);
  if ( ! pm ) return;

  int val = m_pLineTypeRadio->GetSelection();
  VFR::StippleType st;
  if ( val < 0 || val > 4 ) st = VFR::ST_SOLID;
  else st = (VFR::StippleType)val;

  if ( pm->getLineType() == st ) return;
  if ( pm->setLineType(st) )
    pm->chkNotice();
}

void
vsnMPP_Scatter_plotAsLineStrip::OnVecDataChkLst(wxCommandEvent& event) {
  if ( ! m_pVecDataChkLst ) return;
  int dlen = m_pVecDataChkLst->GetCount();
  if ( dlen < 3 ) return;

  vsnMethod_Scatter_plotAsLineStrip* pm
    = dynamic_cast<vsnMethod_Scatter_plotAsLineStrip*>(p_method);
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

void vsnMPP_Scatter_plotAsLineStrip::OnSelDataLst(wxCommandEvent& event) {
  if ( ! m_pSelDataLst ) return;
  WhichDataType sel = event.GetInt();
  if ( sel < 0 ) return;

  wxString valStr = m_pSelDataLst->GetString(sel);
  if ( valStr == wxT("vector length") )
    sel = DATA_Veclen;

  vsnMethod_Scatter_plotAsLineStrip* pm
    = dynamic_cast<vsnMethod_Scatter_plotAsLineStrip*>(p_method);
  if ( ! pm ) return;
  if ( sel == pm->getSelectedData() ) return;

  if ( ! pm->setSelectedData(sel) ) {
    update();
    return;
  }
  pm->chkNotice();
}

void vsnMPP_Scatter_plotAsLineStrip::OnUpdMinMaxChk(wxCommandEvent& event) {
  if ( ! m_pUpdMinMaxChk ) return;
  bool val = m_pUpdMinMaxChk->GetValue();

  vsnMethod_Scatter_plotAsLineStrip* pm
    = dynamic_cast<vsnMethod_Scatter_plotAsLineStrip*>(p_method);
  if ( ! pm ) return;

  if ( pm->setUpdateMinMaxMode(val) )
    pm->chkNotice();
}

void vsnMPP_Scatter_plotAsLineStrip::OnLineWidthTxt(wxCommandEvent& event) {
  vsnMethod_Scatter_plotAsLineStrip* pm
    = dynamic_cast<vsnMethod_Scatter_plotAsLineStrip*>(p_method);
  if ( ! pm ) return;

  wxString valStr = m_pLineWidthTxt->GetValue();
  if ( valStr.IsEmpty() ) return;
  float value = (float)atof(vsnApp::ConvWxToSys(valStr).c_str());
  if ( value < 0.001f ) value = 0.001f;

  if ( pm->setLineWidth(value) )
    pm->chkNotice();
}

void vsnMPP_Scatter_plotAsLineStrip::OnAntiAliasChk(wxCommandEvent& event) {
  if ( ! m_pAntiAliasChk ) return;
  vsnMethod_Scatter_plotAsLineStrip* pm
    = dynamic_cast<vsnMethod_Scatter_plotAsLineStrip*>(p_method);
  if ( ! pm ) return;

  bool val = m_pAntiAliasChk->GetValue();
  if ( pm->setAntiAliasMode(val) )
    pm->chkNotice();
}


//----------------------------------------------------------------
// class vsnMethod_Scatter_plotAsLineStrip
//----------------------------------------------------------------

/* constructors / destructor */

vsnMethod_Scatter_plotAsLineStrip::
vsnMethod_Scatter_plotAsLineStrip(const string& name)
: vsnMethodObj(name),
  m_lineType(VFR::ST_SOLID), m_selectedData(DATA_None), m_vecDataIdx(0,1,2),
  m_updateMinMax(true), m_lineWidth(1.f), m_shape(NULL)
{
  m_showType = RT_WIRE;
}

vsnMethod_Scatter_plotAsLineStrip::~vsnMethod_Scatter_plotAsLineStrip() {
  if ( m_shape )
    delete m_shape;
}


/* methods */

void vsnMethod_Scatter_plotAsLineStrip::adjustRange() {
  bool needRangeUpd = false;

  vsnNumericalDataIF* pData = dynamic_cast<vsnNumericalDataIF*>(p_refData);
  if ( ! pData ) return;
  int dlen = (int)pData->getDataLen();

  float dr[2];
  if ( m_selectedData == DATA_Veclen ) {
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

bool
vsnMethod_Scatter_plotAsLineStrip::setLineType(const VFR::StippleType st) {
  if ( m_lineType == st ) return true;
  if ( st < 0 || st > 4 ) return false;
  m_lineType = st;
  if ( m_shape )
    m_shape->getPrivateMaterial()->setLineStipple(m_lineType);
  updateUI();
  return true;
}  

bool vsnMethod_Scatter_plotAsLineStrip::setLineWidth(const float lw) {
  if ( m_lineWidth == lw ) return true;
  m_lineWidth = lw;
  if ( m_shape )
    m_shape->getPrivateMaterial()->setLineWidth(m_lineWidth);
  updateUI();
  return true;
}

bool
vsnMethod_Scatter_plotAsLineStrip::setVecDataIdx(const Vec3<int>& vdidx) {
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
vsnMethod_Scatter_plotAsLineStrip::setSelectedData(const WhichDataType sd) {
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

bool vsnMethod_Scatter_plotAsLineStrip::isValidVecData() const {
  vsnNumericalDataIF* pData = dynamic_cast<vsnNumericalDataIF*>(p_refData);
  if ( ! pData ) return false;

  size_t dlen = pData->getDataLen();
  if ( m_vecDataIdx[0] < 0 || m_vecDataIdx[0] >= dlen ) return false;
  if ( m_vecDataIdx[1] < 0 || m_vecDataIdx[1] >= dlen ) return false;
  if ( m_vecDataIdx[2] < 0 || m_vecDataIdx[2] >= dlen ) return false;
  return true;
}

bool
vsnMethod_Scatter_plotAsLineStrip::setUpdateMinMaxMode(const bool mode) {
  if ( mode == m_updateMinMax ) return true;
  m_updateMinMax = mode;

  // data range
  if ( m_updateMinMax ) {
    vsnNumericalDataIF* pData
      = dynamic_cast<vsnNumericalDataIF*>(p_refData);
    if ( pData ) {
      float dr[2];
      if ( m_selectedData == DATA_Veclen ) {
	CES::Vec3<int> vdl = m_vecDataIdx;
        if ( pData->getVectorMaxLen(vdl, dr[1]) ) {
          m_lut.minVal = 0.f;
          m_lut.maxVal = dr[1];
        }
      } // end of if(DATA_Veclen)
      else if ( m_selectedData > 0 &&
		m_selectedData <= pData->getDataLen() ) {
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

bool vsnMethod_Scatter_plotAsLineStrip::update(const bool force) {
  return updateStep(m_requestedStp, force);
}

void vsnMethod_Scatter_plotAsLineStrip::reloaded() {
  adjustRange();
  vsnMethodObj::reloaded();
}

vsnMethodPP* vsnMethod_Scatter_plotAsLineStrip::getParamPanel(wxPanel* pp) {
  vsnMPP_Scatter_plotAsLineStrip* pp_plotAsLineStrip
    = new vsnMPP_Scatter_plotAsLineStrip(pp, this);
  if ( ! pp_plotAsLineStrip ) return NULL;
  return pp_plotAsLineStrip;
}


/* vsnTimeSeriesMethodIF methods */

bool vsnMethod_Scatter_plotAsLineStrip::updateStep(const int stp,
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
    m_shape = new vfrLineStrip();
    if ( ! m_shape ) {
      ErrMsg(MsgERR, getMethodType() + string("[") + getName()
             + string("]: memory allocation failed"));
      return false;
    }
    m_shape->alcMaterial();
    m_shape->getPrivateMaterial()->setLineWidth(m_lineWidth);
    addChild(m_shape);
  }
  m_shape->getPrivateMaterial()->setRenderMode(RT_NONE);
  if ( ! m_show ) {
    return true;
  }

  // check selected data
  size_t dlen = pData->getDataLen();
  if ( m_selectedData == DATA_Veclen && ! isValidVecData() ) return true;

  // alloc line datas
  int sampleSz = pData->getNumVerts();
  if ( sampleSz < 2 ) return true;
  if ( ! m_shape->alcVerts(sampleSz) ) {
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

  // set verts of linestrip
  register size_t i;
  vector3* vl = m_shape->getVerts();
  vector3* pv = pData->getVerts();
  float* pd = pData->getData();
  for ( i = 0; i < sampleSz; i++ ) {
    memcpy(vl[i], pv[i], sizeof(vector3));
  } // end of for(i)
  m_shape->generateBbox();

  // set colors
  vector4* cl = m_shape->getColors();
  Vec3<float> vv;
  register float dval;
  register int c;
  if ( m_selectedData == DATA_Veclen ) {
    for ( i = 0; i < sampleSz; i++ ) {
      vv[0] = pd[dlen*i + m_vecDataIdx[0]];
      vv[1] = pd[dlen*i + m_vecDataIdx[1]];
      vv[2] = pd[dlen*i + m_vecDataIdx[2]];
      dval = vv.Length();
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
      c = m_lut.getValIdx(dval);
      cl[i][0] = m_lut.lutEntry[c*4  ];
      cl[i][1] = m_lut.lutEntry[c*4+1];
      cl[i][2] = m_lut.lutEntry[c*4+2];
      cl[i][3] = 1.f;
    } // end of for(i)
    m_shape->setColorMode(AT_PER_VERTEX);
  }
  else { // not ref
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

bool vsnMethod_Scatter_plotAsLineStrip::parseXML(xmlNodePtr xnp) {
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

      if ( xsN == string("line_type") ) {
        VFR::StippleType st;
        if ( xsV == string("regular") ) st = ST_SOLID;
        else if ( xsV == string("dot") ) st = ST_DOT;
	else if ( xsV == string("dash") ) st = ST_DASH;
	else if ( xsV == string("ddash1") ) st = ST_DDASH1;
	else if ( xsV == string("ddash2") ) st = ST_DDASH2;
        else {
          ErrMsg(MsgERR, msgHdr + string("invalid value in param line_type"));
          goto _NEXT_XML_NODE;
        }
        if ( ! setLineType(st) ) {
          ErrMsg(MsgERR, msgHdr + string("can't set line_type"));
          goto _NEXT_XML_NODE;
        }
      } // end of "line_type"
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
        float vlw = (float)atof(xsV.c_str());
        if ( ! setLineWidth(vlw) ) {
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

bool vsnMethod_Scatter_plotAsLineStrip::outputXML(std::ostream& os,
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

  // output original params
  // line_type
  if ( m_lineType != ST_SOLID ) {
    os << idts_2 << "<param name=\"line_type\" value=\"";
    switch ( m_lineType ) {
    case ST_DOT: os << "dot"; break;
    case ST_DASH: os << "dash"; break;
    case ST_DDASH1: os << "ddash1"; break;
    case ST_DDASH2: os << "ddash2"; break;
    default: os << "regular";
    }
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

  // line_width
  if ( m_lineWidth != 1.f ) {
    os << idts_2 << "<param name=\"line_width\" value=\""
       << m_lineWidth << "\" />" << endl;
  }

  // antialias
  if ( m_antiAlias ) {
    os << idts_2 << "<param name=\"antialias\" value=\"yes\" />" << endl;
  }

  os << idts << "</method>" << endl;
  return ret;
}

bool vsnMethod_Scatter_plotAsLineStrip::commandXML(xmlNodePtr xnp) {
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
  if ( nameStr == "set_line_type" ) {
    VFR::StippleType st;
    if ( valueStr == string("regular") ) st = ST_SOLID;
    else if ( valueStr == string("dot") ) st = ST_DOT;
    else if ( valueStr == string("dash") ) st = ST_DASH;
    else if ( valueStr == string("ddash1") ) st = ST_DDASH1;
    else if ( valueStr == string("ddash2") ) st = ST_DDASH2;
    else {
      ErrMsg(MsgERR, msgHdr +
             string("command set_line_type: invalid value: ") + valueStr);
      return false;
    }
    if ( ! setLineType(st) ) {
      ErrMsg(MsgERR, msgHdr +
             string("command set_line_type: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_line_type"
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
  else if ( nameStr == "set_line_width" ) {
    float vlw = (float)atof(valueStr.c_str());
    if ( ! setLineWidth(vlw) ) {
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
