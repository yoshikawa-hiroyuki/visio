//
// vsnMethod_plotScalar
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

#include "vsnMethod_plotScalar.h"
#include "vsnDataObj.h"
#include "vsnUiView.h"
#include "vsnError.h"
#include "vsnOctTree.h" // for decomp vecIdx

using namespace std;
using namespace CES;
using namespace VSN;


//----------------------------------------------------------------
// class vsnMPP_plotScalar
//----------------------------------------------------------------
BEGIN_EVENT_TABLE(vsnMPP_plotScalar, wxPanel)
  EVT_TEXT_ENTER(MPP_plotScalar_SamplerTxt,
		 vsnMPP_plotScalar::OnEnterSamplerTxt)
  EVT_BUTTON(MPP_plotScalar_SetSamplerBtn,
	     vsnMPP_plotScalar::OnSetSamplerBtn)
  EVT_COMBOBOX(MPP_plotScalar_SelDataLst,
	       vsnMPP_plotScalar::OnSelDataLst)
  EVT_CHECKLISTBOX(MPP_plotScalar_VecDataChkLst,
		   vsnMPP_plotScalar::OnVecDataChkLst)
  EVT_CHECKBOX(MPP_plotScalar_UpdMinMaxChk,
	       vsnMPP_plotScalar::OnUpdMinMaxChk)
END_EVENT_TABLE()


/* constructors / destructor */

vsnMPP_plotScalar::vsnMPP_plotScalar(wxPanel* parent, vsnMethodObj* pm)
  : vsnMethodPP(parent, pm)
{
  assert(parent);
  vsnMethod_plotScalar* pMtd = dynamic_cast<vsnMethod_plotScalar*>(pm);
  assert(pMtd);

  // create widgets
  m_pSamplerLst = new wxListBox(this, MPP_plotScalar_SamplerLst);
  assert(m_pSamplerLst);
  m_pSamplerTxt = new wxTextCtrl(this, MPP_plotScalar_SamplerTxt,
				 wxT(""), wxDefaultPosition, wxDefaultSize,
				 wxTE_READONLY|wxTE_PROCESS_ENTER);
  assert(m_pSamplerTxt);
  m_pSetSamplerBtn = new wxButton(this, MPP_plotScalar_SetSamplerBtn,
				  wxT("set sampler"));
  assert(m_pSetSamplerBtn);
  m_pSelDataLst = new wxComboBox(this, MPP_plotScalar_SelDataLst,
				 wxT(""), wxDefaultPosition, wxDefaultSize,
				 0, NULL, wxCB_READONLY);
  assert(m_pSelDataLst);
  m_pVecDataChkLst = new wxCheckListBox(this, MPP_plotScalar_VecDataChkLst);
  assert(m_pVecDataChkLst);
  m_pUpdMinMaxChk = new wxCheckBox(this, MPP_plotScalar_UpdMinMaxChk,
                                   wxT("update lut minmax"));
  assert(m_pUpdMinMaxChk);

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

  // update minmax mode
  m_pUpdMinMaxChk->SetValue(TRUE);
  topsizer->Add(m_pUpdMinMaxChk, 0, wxALL, 3);

  // post process
  SetSizer(topsizer);
  addTo(parent);
  topsizer->Layout();
  (void)update();
}

vsnMPP_plotScalar::~vsnMPP_plotScalar() {
}


/* interface */

bool vsnMPP_plotScalar::update() {
  if ( ! m_pSamplerLst || ! m_pSamplerTxt || ! m_pSetSamplerBtn ||
       ! m_pSelDataLst || ! m_pVecDataChkLst || ! m_pUpdMinMaxChk )
    return false;

  register size_t i;
  char txt[16];

  vsnMethod_plotScalar* pm
    = dynamic_cast<vsnMethod_plotScalar*>(p_method);
  if ( ! pm ) return false;
  vsnNumericalDataIF* pdo
    = dynamic_cast<vsnNumericalDataIF*>(pm->getRefData());
  if ( ! pdo ) return false;

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

  // update minmax mode
  m_pUpdMinMaxChk->SetValue(pm->getUpdateMinMaxMode());

  return true;
}


/* event handler */

void vsnMPP_plotScalar::OnSetSamplerBtn(wxCommandEvent& event) {
  if ( ! m_pSamplerLst || ! m_pSamplerTxt || ! m_pSetSamplerBtn )
    return;

  int sel = m_pSamplerLst->GetSelection();
  if ( sel < 0 ) return;
  m_pSamplerTxt->SetValue(m_pSamplerLst->GetString(sel));

  vsnMethod_plotScalar* pm = dynamic_cast<vsnMethod_plotScalar*>(p_method);
  if ( ! pm ) return;
  vsnDataObj* pdo = pm->getRefData();
  if ( ! pdo ) return;

  pm->setSampler(pdo, sel);
  pm->chkNotice();
}

void vsnMPP_plotScalar::OnEnterSamplerTxt(wxCommandEvent& event) {
  if ( ! m_pSamplerTxt ) return;

  vsnMethod_plotScalar* pm = dynamic_cast<vsnMethod_plotScalar*>(p_method);
  if ( ! pm ) return;
  vsnMethod_sampler* psplr = pm->getSampler();
  if ( ! psplr ) return;
  vsnUiView* puiv = getUiView();
  if ( ! puiv ) return;

  puiv->selectObj((vfrNode*)psplr);
}

void vsnMPP_plotScalar::OnSelDataLst(wxCommandEvent& event) {
  if ( ! m_pSelDataLst ) return;
  WhichDataType sel = event.GetInt();
  if ( sel < 0 ) return;

  wxString valStr = m_pSelDataLst->GetString(sel);
  if ( valStr == wxT("vector length") )
    sel = DATA_Veclen;

  vsnMethod_plotScalar* pm = dynamic_cast<vsnMethod_plotScalar*>(p_method);
  if ( ! pm ) return;
  if ( sel == pm->getSelectedData() ) return;

  if ( ! pm->setSelectedData(sel) ) {
    update();
    return;
  }
  pm->chkNotice();
}

void vsnMPP_plotScalar::OnVecDataChkLst(wxCommandEvent& event) {
  if ( ! m_pVecDataChkLst ) return;
  int dlen = m_pVecDataChkLst->GetCount();
  if ( dlen < 3 ) return;

  vsnMethod_plotScalar* pm = dynamic_cast<vsnMethod_plotScalar*>(p_method);
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

void vsnMPP_plotScalar::OnUpdMinMaxChk(wxCommandEvent& event) {
  if ( ! m_pUpdMinMaxChk ) return;
  bool val = m_pUpdMinMaxChk->GetValue();

  vsnMethod_plotScalar* pm = dynamic_cast<vsnMethod_plotScalar*>(p_method);
  if ( ! pm ) return;

  if ( pm->setUpdateMinMaxMode(val) )
    pm->chkNotice();
}


//----------------------------------------------------------------
// class vsnMethod_plotScalar
//----------------------------------------------------------------

/* constructors / destructor */

vsnMethod_plotScalar::vsnMethod_plotScalar(const string& name)
  : vsnMethodObj(name), m_selectedData(DATA_None), m_updateMinMax(true),
    m_vecDataIdx(0,1,2), m_mesh(NULL), m_useSampler(true)
{
}

vsnMethod_plotScalar::~vsnMethod_plotScalar() {
  if ( m_mesh )
    delete m_mesh;
}


/* methods */

void vsnMethod_plotScalar::adjustRange() {
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

bool vsnMethod_plotScalar::setSelectedData(const WhichDataType sd) {
  if ( sd == m_selectedData ) return true;
 
  vsnNumericalDataIF* pData = dynamic_cast<vsnNumericalDataIF*>(p_refData);
  if ( ! pData ) return false;

  int dlen = (int)pData->getDataLen();
  if ( sd > dlen ) return false;
  if ( sd == DATA_Veclen && dlen < 3 ) return false;
  m_selectedData = sd;

  // data range
  if ( m_updateMinMax )
    adjustRange();

  if ( ! update() ) return false;
  updateUI();
  return true;
}

bool vsnMethod_plotScalar::setVecDataIdx(const Vec3<int>& vdidx) {
  if ( vdidx[0] == m_vecDataIdx[0] &&
       vdidx[1] == m_vecDataIdx[1] &&
       vdidx[2] == m_vecDataIdx[2] ) return true;
  m_vecDataIdx = vdidx;

  // data range
  if ( m_updateMinMax )
    adjustRange();

  if ( ! update() ) return false;
  updateUI();
  return true;
}

bool vsnMethod_plotScalar::isValidVecData() const {
  vsnNumericalDataIF* pData = dynamic_cast<vsnNumericalDataIF*>(p_refData);
  if ( ! pData ) return false;

  size_t dlen = pData->getDataLen();
  if ( m_vecDataIdx[0] < 0 || m_vecDataIdx[0] >= dlen ) return false;
  if ( m_vecDataIdx[1] < 0 || m_vecDataIdx[1] >= dlen ) return false;
  if ( m_vecDataIdx[2] < 0 || m_vecDataIdx[2] >= dlen ) return false;
  return true;
}

bool vsnMethod_plotScalar::setUpdateMinMaxMode(const bool mode) {
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

bool vsnMethod_plotScalar::update(const bool force) {
  return updateStep(m_requestedStp, force);
}

void vsnMethod_plotScalar::reloaded() {
  adjustRange();
  vsnMethodObj::reloaded();
}

vsnMethodPP* vsnMethod_plotScalar::getParamPanel(wxPanel* pp) {
  vsnMPP_plotScalar* pp_plotScalar = new vsnMPP_plotScalar(pp, this);
  if ( ! pp_plotScalar ) return NULL;
  return pp_plotScalar;
}


/* from vsnRef_sampler */

void vsnMethod_plotScalar::noticeUpdate() {
  update();
}


/* from vsnIoObject */

bool vsnMethod_plotScalar::parseXML(xmlNodePtr xnp) {
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

bool vsnMethod_plotScalar::outputXML(std::ostream& os, const size_t ts)
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

  // output base method params
  if ( ! vsnMethodObj::exportXMLNode(os, ts+2) ) {
    ErrMsg(MsgERR, msgHdr + string("base MethodObj exportXMLNode failed"));
    ret = false;
  }

  os << idts << "</method>" << endl;
  return ret;
}

bool vsnMethod_plotScalar::commandXML(xmlNodePtr xnp) {
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
