//
// vsnMethod_Sv_setRectCoord
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

#include "vsnMethod_Sv_setRectCoord.h"
#include "vsnOctTree.h" // for decomp probeIdx
#include "vsnApp.h"
#include "vsnError.h"

using namespace std;
using namespace CES;
using namespace VSN;


//----------------------------------------------------------------
// class vsnMPP_Sv_setRectCoord
//----------------------------------------------------------------

BEGIN_EVENT_TABLE(vsnMPP_Sv_setRectCoord, wxPanel)
  EVT_BUTTON(MPP_Sv_setRectCoord_BrowsBtn,
	     vsnMPP_Sv_setRectCoord::OnBrowsBtn)
  EVT_BUTTON(MPP_Sv_setRectCoord_SetRectCoordBtn,
	     vsnMPP_Sv_setRectCoord::OnSetRectCoordBtn)
  EVT_TEXT_ENTER(MPP_Sv_setRectCoord_OffsetXTxt,
		 vsnMPP_Sv_setRectCoord::OnOffsetTxts)
  EVT_TEXT_ENTER(MPP_Sv_setRectCoord_OffsetYTxt,
		 vsnMPP_Sv_setRectCoord::OnOffsetTxts)
  EVT_TEXT_ENTER(MPP_Sv_setRectCoord_OffsetZTxt,
		 vsnMPP_Sv_setRectCoord::OnOffsetTxts)
END_EVENT_TABLE()


/* constructors / destructor */

vsnMPP_Sv_setRectCoord::vsnMPP_Sv_setRectCoord(wxPanel* parent,
					       vsnMethodObj* pm)
  : vsnMethodPP(parent, pm),
    m_pFileTxt(NULL), m_pBrowsBtn(NULL), m_pSetRectCoordBtn(NULL),
    m_pOffsetXTxt(NULL), m_pOffsetYTxt(NULL), m_pOffsetZTxt(NULL)
{
  assert(parent);
  assert(dynamic_cast<vsnMethod_Sv_setRectCoord*>(pm));

  wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer* sizerH;

  // file
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND);
  sizerH->Add(new wxStaticText(this, -1, wxT("coord file")),
              0, wxEXPAND|wxALL, 3);
  m_pFileTxt = new wxTextCtrl(this, MPP_Sv_setRectCoord_FileTxt, wxT(""),
			      wxDefaultPosition, wxDefaultSize);
  sizerH->Add(m_pFileTxt, 1, wxEXPAND|wxALL, 3);
  m_pBrowsBtn = new wxButton(this, MPP_Sv_setRectCoord_BrowsBtn,
			     wxT("..."), wxDefaultPosition, wxDefaultSize,
			     wxBU_EXACTFIT);
  sizerH->Add(m_pBrowsBtn, 0, wxEXPAND|wxALL, 3);

  // set rect coord button
  m_pSetRectCoordBtn = new wxButton(this, MPP_Sv_setRectCoord_SetRectCoordBtn,
				    wxT("set rect coord"));
  topsizer->Add(m_pSetRectCoordBtn, 0, wxEXPAND|wxALL, 5);

  // Offset
  topsizer->Add(new wxStaticText(this, -1, wxT("shift index")),
                0, wxALIGN_LEFT|wxALL, 3);
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND);
  sizerH->Add(new wxStaticText(this,-1,wxT(" I ")), 0, wxALIGN_LEFT|wxALL, 3);
  m_pOffsetXTxt = new wxTextCtrl(this, MPP_Sv_setRectCoord_OffsetXTxt, wxT(""),
                                 wxDefaultPosition, wxDefaultSize,
                                 wxTE_PROCESS_ENTER);
  sizerH->Add(m_pOffsetXTxt, 0, wxALL, 3);

  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND);
  sizerH->Add(new wxStaticText(this, -1, wxT(" J")), 0, wxALIGN_LEFT|wxALL, 3);
  m_pOffsetYTxt = new wxTextCtrl(this, MPP_Sv_setRectCoord_OffsetYTxt, wxT(""),
                                 wxDefaultPosition, wxDefaultSize,
                                 wxTE_PROCESS_ENTER);
  sizerH->Add(m_pOffsetYTxt, 0, wxALL, 3);

  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND);
  sizerH->Add(new wxStaticText(this, -1, wxT(" K")), 0, wxALIGN_LEFT|wxALL, 3);
  m_pOffsetZTxt = new wxTextCtrl(this, MPP_Sv_setRectCoord_OffsetZTxt, wxT(""),
                                 wxDefaultPosition, wxDefaultSize,
                                 wxTE_PROCESS_ENTER);
  sizerH->Add(m_pOffsetZTxt, 0, wxALL, 3);

  // post process
  SetSizer(topsizer);
  addTo(parent);
  topsizer->Layout();
  (void)update();
}

vsnMPP_Sv_setRectCoord::~vsnMPP_Sv_setRectCoord() {
}


/* interface */

bool vsnMPP_Sv_setRectCoord::update() {
  if ( ! m_pFileTxt || ! m_pOffsetXTxt || ! m_pOffsetYTxt || ! m_pOffsetZTxt )
    return false;

  vsnMethod_Sv_setRectCoord*
    pm = dynamic_cast<vsnMethod_Sv_setRectCoord*>(p_method);
  if ( ! pm ) return false;
  char txt[64];

  // path
  string path = pm->getPath();
  if ( path.empty() ) m_pFileTxt->SetValue(wxT(""));
  else m_pFileTxt->SetValue(vsnApp::ConvSysToWx(path));

  // offset
  size_t ofst[3]; pm->getOffset(ofst);
  sprintf(txt, "%lu", ofst[0]);
  m_pOffsetXTxt->SetValue(vsnApp::ConvSysToWx(txt));
  sprintf(txt, "%lu", ofst[1]);
  m_pOffsetYTxt->SetValue(vsnApp::ConvSysToWx(txt));
  sprintf(txt, "%lu", ofst[2]);
  m_pOffsetZTxt->SetValue(vsnApp::ConvSysToWx(txt));

  return true;
}


/* event handler */

void vsnMPP_Sv_setRectCoord::OnBrowsBtn(wxCommandEvent& event) {
  if ( ! m_pFileTxt ) return;
  vsnMethod_Sv_setRectCoord*
    pm = dynamic_cast<vsnMethod_Sv_setRectCoord*>(p_method);
  if ( ! pm ) return;

  wxFileDialog fileDlg(this, wxT("specify rect coord file"),
                       wxT(""), wxT(""), // default Dir / File
		       wxT("Rect Coord file (*.crd)|*.crd|")
                       wxT("(*)|*"), wxFD_OPEN);

  wxString prevPath = m_pFileTxt->GetValue();
  if ( prevPath.IsEmpty() ) {
    string appImpDir = vsnApp::GetApp()->getImportDir();
    string appCurDir = vsnApp::GetApp()->getCwd();
    if ( ! appImpDir.empty() )
      fileDlg.SetDirectory(vsnApp::ConvSysToWx(appImpDir));
    else if ( ! appCurDir.empty() )
      fileDlg.SetDirectory(vsnApp::ConvSysToWx(appCurDir));
  } else {
    string tmpPath = vsnPath_normalize(vsnApp::ConvWxToSys(prevPath));
    string tmpDir = DirName(tmpPath, vsnPath_getDelimChar());
    string tmpFile = BaseName(tmpPath, "", vsnPath_getDelimChar());
    if ( ! tmpDir.empty() ) fileDlg.SetDirectory(vsnApp::ConvSysToWx(tmpDir));
    if ( ! tmpFile.empty() ) fileDlg.SetFilename(vsnApp::ConvSysToWx(tmpFile));
  }

  if ( fileDlg.ShowModal() == wxID_OK ) {
    wxString newPath = fileDlg.GetPath();
    m_pFileTxt->SetValue(newPath);
  }
}

void vsnMPP_Sv_setRectCoord::OnSetRectCoordBtn(wxCommandEvent& event) {
  if ( ! m_pFileTxt || ! m_pOffsetXTxt || ! m_pOffsetYTxt || ! m_pOffsetZTxt)
    return;
  vsnMethod_Sv_setRectCoord*
    pm = dynamic_cast<vsnMethod_Sv_setRectCoord*>(p_method);
  if ( ! pm ) return;

  wxString valStr;
  valStr = m_pFileTxt->GetValue();
  string rcPath = vsnPath_normalize(vsnApp::ConvWxToSys(valStr));

  int ofst[3];
  valStr = m_pOffsetXTxt->GetValue();
  ofst[0] = atoi(vsnApp::ConvWxToSys(valStr).c_str());
  valStr = m_pOffsetYTxt->GetValue();
  ofst[1] = atoi(vsnApp::ConvWxToSys(valStr).c_str());
  valStr = m_pOffsetZTxt->GetValue();
  ofst[2] = atoi(vsnApp::ConvWxToSys(valStr).c_str());
  if ( ofst[0] < 0 ) ofst[0] = 0;
  if ( ofst[1] < 0 ) ofst[1] = 0;
  if ( ofst[2] < 0 ) ofst[2] = 0;

  if ( pm->setRectCoord(rcPath, (size_t*)ofst) )
    pm->chkNotice();
  else
    update();
}

void vsnMPP_Sv_setRectCoord::OnOffsetTxts(wxCommandEvent& event) {
  if ( ! m_pFileTxt || ! m_pOffsetXTxt || ! m_pOffsetYTxt || ! m_pOffsetZTxt)
    return;
  vsnMethod_Sv_setRectCoord*
    pm = dynamic_cast<vsnMethod_Sv_setRectCoord*>(p_method);
  if ( ! pm ) return;

  wxString valStr;
  int ofst[3];
  valStr = m_pOffsetXTxt->GetValue();
  ofst[0] = atoi(vsnApp::ConvWxToSys(valStr).c_str());
  valStr = m_pOffsetYTxt->GetValue();
  ofst[1] = atoi(vsnApp::ConvWxToSys(valStr).c_str());
  valStr = m_pOffsetZTxt->GetValue();
  ofst[2] = atoi(vsnApp::ConvWxToSys(valStr).c_str());
  if ( ofst[0] < 0 ) {m_pOffsetXTxt->SetValue(wxT("0")); ofst[0] = 0;}
  if ( ofst[1] < 0 ) {m_pOffsetYTxt->SetValue(wxT("0")); ofst[1] = 0;}
  if ( ofst[2] < 0 ) {m_pOffsetZTxt->SetValue(wxT("0")); ofst[2] = 0;}

  valStr = m_pFileTxt->GetValue();
  string rcPath = vsnPath_normalize(vsnApp::ConvWxToSys(valStr));

  if ( pm->setRectCoord(rcPath, (size_t*)ofst) )
    pm->chkNotice();
  else
    update();
}


//----------------------------------------------------------------
// class vsnMethod_Sv_setRectCoord
//----------------------------------------------------------------

/* constructors / destructor */

vsnMethod_Sv_setRectCoord::vsnMethod_Sv_setRectCoord(const std::string& name)
  : vsnMethodObj(name)
{
  m_offset[0] = m_offset[1] = m_offset[2] = 0;

  vsnData_Sph* pdSph = dynamic_cast<vsnData_Sph*>(p_refData);
  if ( pdSph && pdSph->hasRectCoord() ) {
    m_rectCrdFile = pdSph->getRectCoordPath();
    size_t* pofst = pdSph->getRectCoordOffset();
    if ( pofst ) {
      m_offset[0] = pofst[0]; m_offset[1] = pofst[1]; m_offset[2] = pofst[2];
    }
  }
}

vsnMethod_Sv_setRectCoord::~vsnMethod_Sv_setRectCoord() {
  if ( ! m_rectCrdFile.empty() ) {
    vsnData_Sph* pdSph = dynamic_cast<vsnData_Sph*>(p_refData);
    if ( pdSph && pdSph->hasRectCoord() )
      pdSph->loadRectCoord(string());
  }
}


/* methods */

bool vsnMethod_Sv_setRectCoord::setRectCoord(const std::string& rcPath,
					     const size_t* ofst) {
  vsnData_Sph* pData = dynamic_cast<vsnData_Sph*>(p_refData);
  if ( ! pData ) {
    ErrMsg(MsgERR, string("setRectCoord: invalid data (not Sph ?)"));
    return false;
  }

  bool ret = pData->loadRectCoord(rcPath, ofst);

  m_rectCrdFile = rcPath;
  if ( ofst ) {
    m_offset[0] = ofst[0]; m_offset[1] = ofst[1]; m_offset[2] = ofst[2];
  } else {
    m_offset[0] = m_offset[1] = m_offset[2] = 0;
  }
  return ret;
}

void vsnMethod_Sv_setRectCoord::getOffset(size_t* ofst) const {
  if ( ! ofst ) return;
  ofst[0] = m_offset[0]; ofst[1] = m_offset[1]; ofst[2] = m_offset[2];
}


/* vsnMethodObj methods */

bool vsnMethod_Sv_setRectCoord::update(const bool force) {
  return true;
}

vsnMethodPP* vsnMethod_Sv_setRectCoord::getParamPanel(wxPanel* pp) {
  vsnMPP_Sv_setRectCoord*
    pp_setRectCoord = new vsnMPP_Sv_setRectCoord(pp, this);
  if ( ! pp_setRectCoord ) return NULL;
  return pp_setRectCoord;
}


/* serialize : from vsnIoObject */

bool vsnMethod_Sv_setRectCoord::parseXML(xmlNodePtr xnp) {
  static xmlChar* xs;
  xmlNodePtr cur;
  string rcFile;
  size_t ofst[3] = {0, 0, 0};

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

      if ( xsN == string("coord_file") ) {
	rcFile = xsV;
      } // end of "coord_file"
      else if ( xsN == string("offset") ) {
	Vec3<int> idcs(-1, -1, -1);
        istringstream iss(xsV);
        iss >> idcs;
        if ( idcs[0] < 0 || idcs[1] < 0 ||  idcs[2] < 0 ) {
          ErrMsg(MsgERR, msgHdr + string("bad value format: ") +xsV);
          goto _NEXT_XML_NODE;
        }
	ofst[0] = idcs[0]; ofst[1] = idcs[1]; ofst[2] = idcs[2];
      } // end of "offset"
    } // end of param

  _NEXT_XML_NODE:
    cur = cur->next;
  } // end of while(cur)

  // set params
  if ( ! rcFile.empty() ) {
    if ( ! setRectCoord(rcFile, ofst) ) {
      ErrMsg(MsgERR, msgHdr + string("setRectCoord failed"));
      return false;
    }
  }

  return true;
}

bool vsnMethod_Sv_setRectCoord::outputXML(std::ostream& os, const size_t ts) {
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
  if ( ! vsnMethodObj::exportXMLNode(os, ts+2) ) {
    ErrMsg(MsgERR, msgHdr + string("base MethodObj exportXMLNode failed"));
    ret = false;
  }

  // output original params
  // coord_file
  if ( ! m_rectCrdFile.empty() ) {
    os << idts_2 << "<param name=\"coord_file\" value=\"" << m_rectCrdFile
       << "\" />" << endl;
  }

  // offset
  if ( m_offset[0] > 0 || m_offset[1] > 0 || m_offset[2] > 0 ) {
    os << idts_2 << "<param name=\"offset\" value=\""
       << m_offset[0] << "/" << m_offset[1] << "/" << m_offset[2]
       << "\" />" << endl;
  }

  os << idts << "</method>" << endl;

  return true;
}

bool vsnMethod_Sv_setRectCoord::commandXML(xmlNodePtr xnp) {
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

  if ( nameStr == "set_coord_file" ) {
    if ( ! setRectCoord(valueStr, m_offset) ) {
      ErrMsg(MsgERR, msgHdr +
             string("command set_coord_file: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_coord_file"
  else if ( nameStr == "set_offset" ) {
    Vec3<int> idcs(-1, -1, -1);
    istringstream iss(valueStr);
    iss >> idcs;
    if ( idcs[0] < 0 || idcs[1] < 0 ||  idcs[2] < 0 ) {
      ErrMsg(MsgERR, msgHdr
	     + string("command set_offset: bad value format: ") + valueStr);
      return false;
    }
    size_t ofst[3]; ofst[0] = idcs[0]; ofst[1] = idcs[1]; ofst[2] = idcs[2];
    if ( ! setRectCoord(m_rectCrdFile, ofst) ) {
      ErrMsg(MsgERR, msgHdr +
             string("command set_offset: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_offset"

  return true;
}
