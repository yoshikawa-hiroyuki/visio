//
// vsnMethod_minmaxGlyph
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

#include "vsnMethod_minmaxGlyph.h"
#include "vsnColorBar.h"
#include "vsnOctTree.h" // for decomp vecIdx
#include "vsnError.h"

using namespace std;
using namespace CES;
using namespace VSN;


//----------------------------------------------------------------
// class vsnMPP_minmaxGlyph
//----------------------------------------------------------------

BEGIN_EVENT_TABLE(vsnMPP_minmaxGlyph, wxPanel)
  EVT_COMBOBOX(MPP_minmaxGlyph_SelDataLst,
	       vsnMPP_minmaxGlyph::OnSelDataLst)
  EVT_CHECKLISTBOX(MPP_minmaxGlyph_VecDataChkLst,
		   vsnMPP_minmaxGlyph::OnVecDataChkLst)
  EVT_TEXT_ENTER(MPP_minmaxGlyph_ToleranceTxt,
		 vsnMPP_minmaxGlyph::OnToleranceTxt)
  EVT_CHECKBOX(MPP_minmaxGlyph_ShowMinChk,
	       vsnMPP_minmaxGlyph::OnShowMinChk)
  EVT_CHECKBOX(MPP_minmaxGlyph_ShowMaxChk,
	       vsnMPP_minmaxGlyph::OnShowMaxChk)
END_EVENT_TABLE()


/* constructors / destructor */

vsnMPP_minmaxGlyph::vsnMPP_minmaxGlyph(wxPanel* parent, vsnMethodObj* pm)
  : vsnMethodPP(parent, pm),
    m_pToleranceTxt(NULL), m_pShowMinChk(NULL), m_pShowMaxChk(NULL)
{
  assert(parent);
  assert(dynamic_cast<vsnMethod_minmaxGlyph*>(pm));

  wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer* sizerH;

  // min
  m_pShowMinChk = new wxCheckBox(this, MPP_minmaxGlyph_ShowMinChk,
				 wxT("show min glyph"));
  topsizer->Add(m_pShowMinChk, 0, wxALL, 3);

  // max
  m_pShowMaxChk = new wxCheckBox(this, MPP_minmaxGlyph_ShowMaxChk,
				 wxT("show max glyph"));
  topsizer->Add(m_pShowMaxChk, 0, wxALL, 3);

  // tolerance
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxALL, 0);
  sizerH->Add(new wxStaticText(this, -1, wxT("tolerance")),
              0, wxALIGN_LEFT|wxALL, 3);

  m_pToleranceTxt = new wxTextCtrl(this, MPP_minmaxGlyph_ToleranceTxt, wxT(""),
				   wxDefaultPosition, wxDefaultSize,
				   wxTE_PROCESS_ENTER);
  sizerH->Add(m_pToleranceTxt, 0, wxEXPAND|wxALL, 3);
  sizerH->Add(new wxStaticText(this, -1, wxT("%")), 0, wxALIGN_LEFT|wxALL, 3);

  topsizer->Add(new wxStaticLine(this, -1, wxDefaultPosition,
				 wxSize(3,3), wxHORIZONTAL),
		0, wxEXPAND|wxALL, 0);

  // data selector
  topsizer->Add(new wxStaticText(this, -1, wxT("select scalar data")),
                0, wxALIGN_LEFT|wxALL, 3);
  m_pSelDataLst = new wxComboBox(this, MPP_minmaxGlyph_SelDataLst,
                                 wxT(""), wxDefaultPosition, wxDefaultSize,
                                 0, NULL, wxCB_READONLY);
  topsizer->Add(m_pSelDataLst, 0, wxEXPAND|wxALL, 3);

  // vector data selector
  topsizer->Add(new wxStaticText(this, -1, wxT("vector components")),
                0, wxALIGN_LEFT|wxALL, 3);
  m_pVecDataChkLst = new wxCheckListBox(this, MPP_minmaxGlyph_VecDataChkLst);
  topsizer->Add(m_pVecDataChkLst, 0, wxEXPAND|wxALL, 3);

  // post process
  SetSizer(topsizer);
  addTo(parent);
  topsizer->Layout();
  (void)update();
}

vsnMPP_minmaxGlyph::~vsnMPP_minmaxGlyph() {
}


/* interface */

bool vsnMPP_minmaxGlyph::update() {
  if ( ! m_pSelDataLst || ! m_pVecDataChkLst ||
       ! m_pToleranceTxt || ! m_pShowMinChk || ! m_pShowMaxChk )
    return false;

  vsnMethod_minmaxGlyph* pm
    = dynamic_cast<vsnMethod_minmaxGlyph*>(p_method);
  if ( ! pm ) return false;

  vsnNumericalDataIF* pdo
    = dynamic_cast<vsnNumericalDataIF*>(pm->getRefData());
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
  Vec3<int> vidx;
  m_pVecDataChkLst->Clear();
  if ( dlen >= 3 ) {
    for ( i = 0; i < dlen; i++ ) {
      sprintf(txt, "data%d", i);
      m_pVecDataChkLst->Append(vsnApp::ConvSysToWx(txt));
    } // end of for(i)

    vidx = pm->getVecDataIdx();
    if ( vidx[0] >= 0 && vidx[0] < dlen ) m_pVecDataChkLst->Check(vidx[0]);
    if ( vidx[1] >= 0 && vidx[1] < dlen ) m_pVecDataChkLst->Check(vidx[1]);
    if ( vidx[2] >= 0 && vidx[2] < dlen ) m_pVecDataChkLst->Check(vidx[2]);
  }

  // show min/max
  m_pShowMinChk->SetValue(pm->getShowMin());
  m_pShowMaxChk->SetValue(pm->getShowMax());

  // tolerance
  float tval = (float)fabs(pm->getTolerance());
  char mlv[32];
  sprintf(mlv, "%g", tval);
  m_pToleranceTxt->SetValue(vsnApp::ConvSysToWx(mlv));

  return true;
}


/* event handler */

void vsnMPP_minmaxGlyph::OnSelDataLst(wxCommandEvent& event) {
  if ( ! m_pSelDataLst ) return;
  WhichDataType sel = event.GetInt();
  if ( sel < 0 ) return;

  wxString valStr = m_pSelDataLst->GetString(sel);
  if ( valStr == wxT("vector length") )
    sel = DATA_Veclen;

  vsnMethod_minmaxGlyph* pm
    = dynamic_cast<vsnMethod_minmaxGlyph*>(p_method);
  if ( ! pm ) return;
  if ( sel == pm->getSelectedData() ) return;

  if ( ! pm->setSelectedData(sel) ) {
    update();
    return;
  }
  pm->chkNotice();
}

void vsnMPP_minmaxGlyph::OnVecDataChkLst(wxCommandEvent& event) {
  if ( ! m_pVecDataChkLst ) return;
  int dlen = m_pVecDataChkLst->GetCount();
  if ( dlen < 3 ) return;

  vsnMethod_minmaxGlyph* pm
    = dynamic_cast<vsnMethod_minmaxGlyph*>(p_method);
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

void vsnMPP_minmaxGlyph::OnToleranceTxt(wxCommandEvent& event) {
  if ( ! m_pToleranceTxt ) return;

  vsnMethod_minmaxGlyph* pm
    = dynamic_cast<vsnMethod_minmaxGlyph*>(p_method);
  if ( ! pm ) return;

  wxString valStr = m_pToleranceTxt->GetValue();
  if ( valStr.IsEmpty() ) {
    char txt[64]; sprintf(txt, "%g", pm->getTolerance());
    m_pToleranceTxt->SetValue(vsnApp::ConvSysToWx(txt));
    return;
  }
  float val = (float)atof(vsnApp::ConvWxToSys(valStr).c_str());

  if ( pm->setTolerance(val) )
    pm->chkNotice();
}

void vsnMPP_minmaxGlyph::OnShowMinChk(wxCommandEvent& event) {
  if ( ! m_pShowMinChk ) return;
  bool val = m_pShowMinChk->GetValue();

  vsnMethod_minmaxGlyph* pm
    = dynamic_cast<vsnMethod_minmaxGlyph*>(p_method);
  if ( ! pm ) return;

  if ( pm->setShowMin(val) )
    pm->chkNotice();
}

void vsnMPP_minmaxGlyph::OnShowMaxChk(wxCommandEvent& event) {
  if ( ! m_pShowMaxChk ) return;
  bool val = m_pShowMaxChk->GetValue();

  vsnMethod_minmaxGlyph* pm
    = dynamic_cast<vsnMethod_minmaxGlyph*>(p_method);
  if ( ! pm ) return;

  if ( pm->setShowMax(val) )
    pm->chkNotice();
}


//----------------------------------------------------------------
// class vsnMethod_minmaxGlyph
//----------------------------------------------------------------

/* constructors / destructor */

vsnMethod_minmaxGlyph::vsnMethod_minmaxGlyph(const std::string& name)
  : vsnMethodObj(name), m_minGlyph(NULL), m_maxGlyph(NULL),
    m_selectedData(DATA_None), m_vecDataIdx(0,1,2),
    m_tolerance(0.1f), m_showMin(true), m_showMax(true)
{
  m_showType = RT_POINT;
  m_useLut = true;
}

vsnMethod_minmaxGlyph::~vsnMethod_minmaxGlyph() {
  if ( m_minGlyph )
    delete m_minGlyph;
  if ( m_maxGlyph )
    delete m_maxGlyph;
}


/* methods */

bool vsnMethod_minmaxGlyph::setSelectedData(const VSN::WhichDataType sd) {
  if ( sd == m_selectedData ) return true;

  vsnNumericalDataIF* pData = dynamic_cast<vsnNumericalDataIF*>(p_refData);
  if ( ! pData ) return false;

  int dlen = (int)pData->getDataLen();
  if ( sd > dlen ) return false;
  if ( sd == DATA_Veclen && dlen < 3 ) return false;
  m_selectedData = sd;

  if ( ! update() ) return false;
  updateUI();
  return true;
}

bool vsnMethod_minmaxGlyph::setVecDataIdx(const CES::Vec3<int>& vdidx) {
  if ( vdidx[0] == m_vecDataIdx[0] &&
       vdidx[1] == m_vecDataIdx[1] &&
       vdidx[2] == m_vecDataIdx[2] ) return true;
  m_vecDataIdx = vdidx;

  if ( ! update() ) return false;
  updateUI();
  return true;
}

bool vsnMethod_minmaxGlyph::isValidVecData() const {
  vsnNumericalDataIF* pData = dynamic_cast<vsnNumericalDataIF*>(p_refData);
  if ( ! pData ) return false;

  size_t dlen = pData->getDataLen();
  if ( m_vecDataIdx[0] < 0 || m_vecDataIdx[0] >= dlen ) return false;
  if ( m_vecDataIdx[1] < 0 || m_vecDataIdx[1] >= dlen ) return false;
  if ( m_vecDataIdx[2] < 0 || m_vecDataIdx[2] >= dlen ) return false;
  return true;
}

bool vsnMethod_minmaxGlyph::setTolerance(const float val) {
  if ( m_tolerance == val ) return true;
  m_tolerance = val;

  if ( ! update() ) return false;
  updateUI();
  return true;
}

bool vsnMethod_minmaxGlyph::setShowMin(const bool sm) {
  if ( m_showMin == sm ) return true;
  m_showMin = sm;

  updateShowGlyph();
  updateUI();
  return true;
}

bool vsnMethod_minmaxGlyph::setShowMax(const bool sm) {
  if ( m_showMax == sm ) return true;
  m_showMax = sm;

  updateShowGlyph();
  updateUI();
  return true;
}

void vsnMethod_minmaxGlyph::updateShowGlyph() {
  if ( m_minGlyph ) m_minGlyph->getPrivateMaterial()->setRenderMode(RT_NONE);
  if ( m_maxGlyph ) m_maxGlyph->getPrivateMaterial()->setRenderMode(RT_NONE);
  if ( ! m_show ) return;

  if ( m_showMin && m_minGlyph )
    m_minGlyph->getPrivateMaterial()->setRenderMode(m_showType);
  if ( m_showMax && m_maxGlyph )
    m_maxGlyph->getPrivateMaterial()->setRenderMode(m_showType);
}

void vsnMethod_minmaxGlyph::updateColor() {
  vector4 cv;
  if ( m_minGlyph ) {
    memcpy(cv, &m_lut.lutEntry[0], sizeof(float)*4);
    m_minGlyph->setColor3(0, cv);
  }
  if ( m_maxGlyph ) {
    memcpy(cv, &m_lut.lutEntry[(m_lut.numEntry-1)*4], sizeof(float)*4);
    m_maxGlyph->setColor3(0, cv);
  }

  chkNotice();
}

void vsnMethod_minmaxGlyph::updateLutRange() {

  // min/max value of lut
  float minmax[2] = {0.f, 1.f};
  if ( getMinMaxCurStp(minmax) ) {
    m_lut.minVal = minmax[0];
    m_lut.maxVal = minmax[1];
    updateUI();

    set<vsnMethodLutRefer*>::iterator it;
    for ( it = m_mlrLst.begin(); it != m_mlrLst.end(); it++ )
      if ( *it ) (*it)->updateLut();
  }

  chkNotice();
}


/* vsnMethodObj methods */

void vsnMethod_minmaxGlyph::setShow(const bool mode) {
  if ( m_show == mode ) return;
  m_show = mode;
  updateShowGlyph();
}

void vsnMethod_minmaxGlyph::setBaseColor(const vector4 cv) {
  memcpy(m_colour, cv, sizeof(vector4));
  updateUI();

  if ( m_pColorBar )
    m_pColorBar->setBaseColor(m_colour);

  chkNotice();
}

bool vsnMethod_minmaxGlyph::update(const bool force) {
  // allocate glyphs
  if ( ! m_minGlyph ) {
    m_minGlyph = new vfrTriangles("min_glyph");
    if ( ! m_minGlyph ) {
      ErrMsg(MsgERR, getMethodType() + string("[") + getName()
             + string("]: memory allocation failed"));
      return false;
    }
    vfrMaterial* pmat = m_minGlyph->alcMaterial();
    if ( pmat ) pmat->setPointSymbol(SYM_TRIANGLE);
    addChild(m_minGlyph);
  }
  if ( ! m_maxGlyph ) {
    m_maxGlyph = new vfrTriangles("max_glyph");
    if ( ! m_maxGlyph ) {
      ErrMsg(MsgERR, getMethodType() + string("[") + getName()
             + string("]: memory allocation failed"));
      return false;
    }
    vfrMaterial* pmat = m_maxGlyph->alcMaterial();
    if ( pmat ) pmat->setPointSymbol(SYM_CIRCLE);
    addChild(m_maxGlyph);
  }

  updateColor();
  return updateStep(m_requestedStp, force);
}

vsnMethodPP* vsnMethod_minmaxGlyph::getParamPanel(wxPanel* pp) {
  vsnMPP_minmaxGlyph* pp_minmaxGlyph
    = new vsnMPP_minmaxGlyph(pp, this);
  if ( ! pp_minmaxGlyph ) return NULL;
  return pp_minmaxGlyph;
}


/* serialize : from vsnIoObject */

bool vsnMethod_minmaxGlyph::parseXML(xmlNodePtr xnp) {
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
       else if ( xsN == string("tolerance") ) {
        float val = (float)atof(xsV.c_str());
        if ( ! setTolerance(val) ) {
          ErrMsg(MsgERR, msgHdr + string("invalid value in param tolerance"));
          goto _NEXT_XML_NODE;
        }
      } // end of "tolerance"
      else if ( xsN == string("show_min") ) {
        bool smm;
        if ( xsV == string("yes") ) smm = true;
        else if ( xsV == string("no") ) smm = false;
        else {
          ErrMsg(MsgERR, msgHdr + string("invalid value in param show_min"));
          goto _NEXT_XML_NODE;
        }
        if ( ! setShowMin(smm) ) {
          ErrMsg(MsgERR, msgHdr + string("can't update show_min mode"));
          goto _NEXT_XML_NODE;
        }
      } // end of "show_min"
      else if ( xsN == string("show_max") ) {
        bool smm;
        if ( xsV == string("yes") ) smm = true;
        else if ( xsV == string("no") ) smm = false;
        else {
          ErrMsg(MsgERR, msgHdr + string("invalid value in param show_max"));
          goto _NEXT_XML_NODE;
        }
        if ( ! setShowMax(smm) ) {
          ErrMsg(MsgERR, msgHdr + string("can't update show_max mode"));
          goto _NEXT_XML_NODE;
        }
      } // end of "show_max"
    } // end of param

  _NEXT_XML_NODE:
    cur = cur->next;
  } // end of while(cur)

  return true;
}

bool vsnMethod_minmaxGlyph::outputXML(std::ostream& os, const size_t ts) {
  string idts;
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
  if ( ! vsnMethodObj::exportXMLNode(os, ts+2) ) {
    ErrMsg(MsgERR, msgHdr + string("base MethodObj exportXMLNode failed"));
    ret = false;
  }

  // output original params
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

  // tolerance
  if ( m_tolerance != 0.1f ) {
    os << idts_2 << "<param name=\"tolerance\" value=\""
       << m_tolerance << "\" />" << endl;
  }

  // show_min/show_max
  if ( ! m_showMin ) {
    os << idts_2 << "<param name=\"show_min\" value=\"no\" />" << endl;
  }
  if ( ! m_showMax ) {
    os << idts_2 << "<param name=\"show_max\" value=\"no\" />" << endl;
  }

  os << idts << "</method>" << endl;
  return true;
}

bool vsnMethod_minmaxGlyph::commandXML(xmlNodePtr xnp) {
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
  else if ( nameStr == "set_tolerance" ) {
    float val = (float)atof(valueStr.c_str());
    if ( ! setTolerance(val) ) {
      ErrMsg(MsgERR, msgHdr +
             string("command set_tolerance: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_tolerance"
  else if ( nameStr == "set_show_min" ) {
    bool smm;
    if ( valueStr == string("yes") ) smm = true;
    else if ( valueStr == string("no") ) smm = false;
    else {
      ErrMsg(MsgERR, msgHdr + string("command set_show_min: invalid value"));
      return false;
    }
    if ( ! setShowMin(smm) ) {
      ErrMsg(MsgERR, msgHdr +
             string("command set_show_min: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_show_min"
  else if ( nameStr == "set_show_max" ) {
    bool smm;
    if ( valueStr == string("yes") ) smm = true;
    else if ( valueStr == string("no") ) smm = false;
    else {
      ErrMsg(MsgERR, msgHdr + string("command set_show_max: invalid value"));
      return false;
    }
    if ( ! setShowMax(smm) ) {
      ErrMsg(MsgERR, msgHdr +
             string("command set_show_max: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_show_max"
  else {
    // not my command
    ErrMsg(MsgERR, msgHdr + string("unknown command: ") + nameStr);
    return false;
  }

  return true;
}
