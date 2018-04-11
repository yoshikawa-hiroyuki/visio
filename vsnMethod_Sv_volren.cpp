//
// vsnMethod_Sv_volren
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
#include "vsnMethod_Sv_volren.h"
#include "vsnOctTree.h" // for decomp vecIdx
#include "vsnUiView.h"
#include "vsnColorBar.h"
#include "vsnError.h"

using namespace std;
using namespace CES;
using namespace NVR;
using namespace VSN;


//----------------------------------------------------------------
// class vsnMPP_Sv_volren
//----------------------------------------------------------------

BEGIN_EVENT_TABLE(vsnMPP_Sv_volren, wxPanel)
  EVT_COMBOBOX(MPP_Sv_volren_SelDataLst,
               vsnMPP_Sv_volren::OnSelDataLst)
  EVT_CHECKLISTBOX(MPP_Sv_volren_VecDataChkLst,
                   vsnMPP_Sv_volren::OnVecDataChkLst)
  EVT_CHECKBOX(MPP_Sv_volren_UpdMinMaxChk,
               vsnMPP_Sv_volren::OnUpdMinMaxChk)
  EVT_CHECKBOX(MPP_Sv_volren_ReduceChk,
               vsnMPP_Sv_volren::OnReduceChk)
END_EVENT_TABLE()


/* constructors / destructor */

vsnMPP_Sv_volren::vsnMPP_Sv_volren(wxPanel* parent, vsnMethodObj* pm)
  : vsnMethodPP(parent, pm)
{
  assert(parent);
  assert(dynamic_cast<vsnMethod_Sv_volren*>(pm));

  wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL);

  // data selector
  topsizer->Add(new wxStaticText(this, -1, wxT("select scalar data")),
                0, wxALIGN_LEFT|wxALL, 3);
  m_pSelDataLst = new wxComboBox(this, MPP_Sv_volren_SelDataLst,
                                 wxT(""), wxDefaultPosition, wxDefaultSize,
                                 0, NULL, wxCB_READONLY);
  topsizer->Add(m_pSelDataLst, 0, wxEXPAND|wxALL, 3);

  // vector data selector
  topsizer->Add(new wxStaticText(this, -1, wxT("vector components")),
                0, wxALIGN_LEFT|wxALL, 3);
  m_pVecDataChkLst = new wxCheckListBox(this,
                                        MPP_Sv_volren_VecDataChkLst);
  topsizer->Add(m_pVecDataChkLst, 0, wxEXPAND|wxALL, 3);

  // update minmax mode
  m_pUpdMinMaxChk = new wxCheckBox(this, MPP_Sv_volren_UpdMinMaxChk,
                                   wxT("update minmax"));
  m_pUpdMinMaxChk->SetValue(TRUE);
  topsizer->Add(m_pUpdMinMaxChk, 0, wxALL, 3);

  // reduce mode
  m_pReduceChk = new wxCheckBox(this, MPP_Sv_volren_ReduceChk,
				wxT("data reduce"));
  m_pReduceChk->SetValue(TRUE);
  topsizer->Add(m_pReduceChk, 0, wxALL, 3);

  // post process
  SetSizer(topsizer);
  addTo(parent);
  topsizer->Layout();
  (void)update();
}

vsnMPP_Sv_volren::~vsnMPP_Sv_volren() {
}


/* interface */

bool vsnMPP_Sv_volren::update() {
  if ( ! m_pSelDataLst || ! m_pVecDataChkLst ||
       ! m_pUpdMinMaxChk || ! m_pReduceChk )
    return false;

  vsnMethod_Sv_volren* pm
    = dynamic_cast<vsnMethod_Sv_volren*>(p_method);
  if ( ! pm ) return false;
  vsnData_Sv* pdo = dynamic_cast<vsnData_Sv*>(pm->getRefData());
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

  // reduce mode
  m_pReduceChk->SetValue(pm->getReduceMode());

  return true;
}


/* event handler */

void vsnMPP_Sv_volren::OnSelDataLst(wxCommandEvent& event) {
  if ( ! m_pSelDataLst ) return;
  WhichDataType sel = event.GetInt();
  if ( sel < 0 ) return;

  wxString valStr = m_pSelDataLst->GetString(sel);
  if ( valStr == wxT("vector length") )
    sel = DATA_Veclen;

  vsnMethod_Sv_volren* pm
    = dynamic_cast<vsnMethod_Sv_volren*>(p_method);
  if ( ! pm ) return;
  if ( sel == pm->getSelectedData() ) return;

  if ( ! pm->setSelectedData(sel) ) {
    update();
    return;
  }
  pm->chkNotice();
}

void vsnMPP_Sv_volren::OnVecDataChkLst(wxCommandEvent& event) {
  if ( ! m_pVecDataChkLst ) return;
  int dlen = m_pVecDataChkLst->GetCount();
  if ( dlen < 3 ) return;

  vsnMethod_Sv_volren* pm
    = dynamic_cast<vsnMethod_Sv_volren*>(p_method);
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

void vsnMPP_Sv_volren::OnUpdMinMaxChk(wxCommandEvent& event) {
  if ( ! m_pUpdMinMaxChk ) return;
  bool val = m_pUpdMinMaxChk->GetValue();

  vsnMethod_Sv_volren* pm
    = dynamic_cast<vsnMethod_Sv_volren*>(p_method);
  if ( ! pm ) return;

  if ( pm->setUpdateMinMaxMode(val) )
    pm->chkNotice();
}

void vsnMPP_Sv_volren::OnReduceChk(wxCommandEvent& event) {
  if ( ! m_pReduceChk ) return;
  bool val = m_pReduceChk->GetValue();

  vsnMethod_Sv_volren* pm
    = dynamic_cast<vsnMethod_Sv_volren*>(p_method);
  if ( ! pm ) return;

  if ( pm->setReduceMode(val) )
    pm->chkNotice();
}


//----------------------------------------------------------------
// class vsnMethod_Sv_volren
//----------------------------------------------------------------

/* constructors / destructor */

vsnMethod_Sv_volren::
vsnMethod_Sv_volren(const std::string& name)
  : vsnMethodObj(name), m_pRender(NULL), m_inited(false), m_pd(NULL),
    m_selectedData(DATA_None), m_vecDataIdx(0,1,2), m_crdWarned(false),
    m_updateMinMax(true), m_doReduce(true)
{
  m_pRender = new vsnNvrVolumeRender();
  assert(m_pRender);
  m_pRender->alcMaterial();
  if ( ! m_pRender->getOglChkd() ) {
    ErrMsg(MsgERR, getMethodType() + string("[") + getName()
	   + string("]: required OpenGL Extensions not supported"));
    return;
  }
  addChild(m_pRender);

  setPickMode(PT_NONE);
}

vsnMethod_Sv_volren::~vsnMethod_Sv_volren() {
  if ( m_pRender ) {
    if ( p_refData ) {
      vsnScene* psc = p_refData->getAncestorScene();
      if ( psc ) psc->delVolRender(m_pRender, this);
    }
    delete m_pRender;
  }
  if ( m_pd )
    VFR::DeAllocate(m_pd);
}


/* methods */

void vsnMethod_Sv_volren::adjustRange() {
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

bool vsnMethod_Sv_volren::setSelectedData(const WhichDataType sd) {
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

bool vsnMethod_Sv_volren::setVecDataIdx(const CES::Vec3<int>& vdidx) {
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

bool vsnMethod_Sv_volren::isValidVecData() const {
  vsnData_Sv* pData = dynamic_cast<vsnData_Sv*>(p_refData);
  if ( ! pData ) return false;

  size_t dlen = pData->getDataLen();
  if ( m_vecDataIdx[0] < 0 || m_vecDataIdx[0] >= dlen ) return false;
  if ( m_vecDataIdx[1] < 0 || m_vecDataIdx[1] >= dlen ) return false;
  if ( m_vecDataIdx[2] < 0 || m_vecDataIdx[2] >= dlen ) return false;
  return true;
}

bool vsnMethod_Sv_volren::setUpdateMinMaxMode(const bool mode) {
  if ( m_updateMinMax == mode ) return true;
  m_updateMinMax = mode;

  // data range
  adjustRange();

  if ( ! update() ) return false;
  updateUI();
  return true;
}

bool vsnMethod_Sv_volren::setReduceMode(const bool mode) {
  if ( m_doReduce == mode ) return true;
  m_doReduce = mode;

  if ( m_pRender ) {
    if ( ! m_pRender->setReduceMode(m_doReduce) )
      return false;
  }

  if ( ! update() ) return false;
  updateUI();
  return true;
}


/* vsnTimeSeriesMethodIF methods */

bool vsnMethod_Sv_volren::updateStep(const int stp,
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

  // rederer
  if ( ! m_pRender ) return false;
  m_pRender->getPrivateMaterial()->setRenderMode(RT_NONE);
  if ( ! m_show ) {
    return true;
  }

  // check data
  Vec3<size_t> dims = pData->getDims();
  size_t dimSz = dims[0] * dims[1] * dims[2];
  if ( dimSz < 1 ) return false;
  if ( ! m_crdWarned && ! pData->isUniformCoord() ) {
    ErrMsg(MsgWARN, getMethodType() + string("[") + getName()
           + string("]: non-uniform grid data"));
    m_crdWarned = true;
  }
  if ( ! m_inited ) {
    if ( ! m_pRender->Initialize(dims) ) {
      ErrMsg(MsgERR, getMethodType() + string("[") + getName()
           + string("]: volume render initialization failed"));
      return false;
    }
    m_inited = true;
  }

  // check selected data
  size_t dlen = pData->getDataLen();
  float dr[2] = {m_lut.minVal, m_lut.maxVal};
  if ( m_selectedData == DATA_None ) return true;
  else if ( m_selectedData == DATA_Veclen ) {
    if ( ! isValidVecData() ) return true;
  } else if ( m_selectedData > dlen || m_selectedData < -1 ) {
    return true;
  }

  // get Sv data
  const float* dptr = pData->getData(m_requestedStp);
  if ( ! dptr ) {
    return false;
  }

  // prepare data
  m_pd = (float*)VFR::ReAllocate(m_pd, sizeof(float)*dimSz);
  float* sptr = m_pd;
  if ( ! sptr ) {
    ErrMsg(MsgERR, getMethodType() + string("[") + getName()
           + string("]: memory allocation failed"));
    return false;
  }
  register size_t i, idx;
  if ( m_selectedData == DATA_Veclen ) {
    Vec3<float> vv;
    for ( i = 0; i < dimSz; i++ ) {
      idx = i * dlen;
      vv.m_v[0] = dptr[idx + m_vecDataIdx.m_v[0]];
      vv.m_v[1] = dptr[idx + m_vecDataIdx.m_v[1]];
      vv.m_v[2] = dptr[idx + m_vecDataIdx.m_v[2]];
      sptr[i] = vv.Length();
    } // end of for(i)
  }
  else {
    for ( i = 0; i < dimSz; i++ ) {
      idx = i * dlen + m_selectedData -1;
      sptr[i] = dptr[idx];
    } // end of for(i)
  }

  // set lut
  if ( ! m_pRender->SetLut(m_lut) ) {
    ErrMsg(MsgERR, getMethodType() + string("[") + getName()
           + string("]: can't set LUT to the volume render"));
    return false;
  }

  // set volume and geometory
  if ( ! m_pRender->SetVolume(sptr, dr, pData->getBbox(), pData->getMask()) ) {
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

bool vsnMethod_Sv_volren::update(const bool force) {
  return updateStep(m_requestedStp, force);
}

void vsnMethod_Sv_volren::reloaded() {
  adjustRange();
  vsnMethodObj::reloaded();
}

vsnMethodPP* vsnMethod_Sv_volren::getParamPanel(wxPanel* pp) {
  vsnMPP_Sv_volren* pp_volren
    = new vsnMPP_Sv_volren(pp, this);
  if ( ! pp_volren ) return NULL;
  return pp_volren;
}

void vsnMethod_Sv_volren::setBaseColor(const vector4 cv) {
  memcpy(m_colour, cv, sizeof(vector4));
  updateUI();
  
  if ( m_pColorBar )
    m_pColorBar->setBaseColor(m_colour);

  chkNotice();
}

void vsnMethod_Sv_volren::setRefData(vsnDataObj* prd) {
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

bool vsnMethod_Sv_volren::parseXML(xmlNodePtr xnp) {
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
      else if ( xsN == string("data_reduce") ) {
	bool drm;
	if ( xsV == string("yes") ) drm = true;
        else if ( xsV == string("no") ) drm = false;
        else {
          ErrMsg(MsgERR, msgHdr+string("invalid value in param data_reduce"));
          goto _NEXT_XML_NODE;
        }
	if ( ! setReduceMode(drm) ) {
          ErrMsg(MsgERR, msgHdr + string("can't update data_reduce mode"));
          goto _NEXT_XML_NODE;
	}
      } // // end of "data_reduce"
    } // end of param

  _NEXT_XML_NODE:
    cur = cur->next;
  } // end of while(cur)

  return true;
}

bool vsnMethod_Sv_volren::outputXML(std::ostream& os, const size_t ts) {
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

  // data_reduce
  if ( ! m_doReduce ) {
    os << idts_2 << "<param name=\"data_reduce\" value=\"no\" />" << endl;
  }

  // output base method params
  if ( ! vsnMethodObj::exportXMLNode(os, ts+2) ) {
    ErrMsg(MsgERR, msgHdr + string("base MethodObj exportXMLNode failed"));
    ret = false;
  }

  os << idts << "</method>" << endl;
  return true;
}

bool vsnMethod_Sv_volren::commandXML(xmlNodePtr xnp) {
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
  else if ( nameStr == "set_data_reduce" ) {
    bool drm;
    if ( valueStr == string("yes") ) drm = true;
    else if ( valueStr == string("no") ) drm = false;
    else {
      ErrMsg(MsgERR, msgHdr+string("command set_data_reduce: invalid value"));
      return false;
    }
    if ( ! setReduceMode(drm) ) {
      ErrMsg(MsgERR, msgHdr +
             string("command set_data_reduce: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_data_reduce"
  else {
    // not my command
    ErrMsg(MsgERR, msgHdr + string("unknown command: ") + nameStr);
    return false;
  }

  return true;
}
