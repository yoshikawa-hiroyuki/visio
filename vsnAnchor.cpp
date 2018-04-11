//
// vsnAnchor
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

#include "vsnAnchor.h"
#include "vsnApp.h"
#include <sstream>

using namespace std;
using namespace CES;
using namespace VSN;


//----------------------------------------------------------------
// class vsnAnchor
//----------------------------------------------------------------
#ifdef WINDOWS
std::string vsnAnchor::s_movShowPath("\"C:/Program Files/Windows Media Player/wmplayer.exe\" /play");
std::string vsnAnchor::s_imgShowPath("\"C:/Program Files/Windows Media Player/wmplayer.exe\" /play");
std::string vsnAnchor::s_txtShowPath("C:/WINNT/system32/notepad.exe");
#else
# ifdef MacOSX
std::string vsnAnchor::s_movShowPath("open");
std::string vsnAnchor::s_imgShowPath("open");
std::string vsnAnchor::s_txtShowPath("open");
# else // Linux
std::string vsnAnchor::s_movShowPath("/usr/bin/totem");
std::string vsnAnchor::s_imgShowPath("/usr/bin/eog");
std::string vsnAnchor::s_txtShowPath("/usr/bin/gedit");
# endif
#endif

vsnAnchor::vsnAnchor()
  : m_actType(ACT_LoadXML), m_viewKeep(true), m_fadeSec(0.f)
{
  static bool firstTouch = true;
  if ( firstTouch ) {
    char* pEnvMov = getenv("VSN_MOVIE_PLAYER");
    if ( pEnvMov ) s_movShowPath = pEnvMov;

    char* pEnvImg = getenv("VSN_IMAGE_VIEWER");
    if ( pEnvImg ) s_imgShowPath = pEnvImg;

    char* pEnvTxt = getenv("VSN_TEXT_VIEWER");
    if ( pEnvTxt ) s_txtShowPath = pEnvTxt;

    firstTouch = false;
  }
}


void vsnAnchor::execAction() const {
  if ( m_refPath.empty() ) return;
  vsnApp* pApp = vsnApp::GetApp();

  if ( m_actType == ACT_LoadXML ) {
    string cmd = string("<command target=\"App\" name=\"open2\""
			" value=\"" + m_refPath + "\"");
    char sbuff[64];
    sprintf(sbuff, " fade=\"%g\"", m_fadeSec);
    cmd += string(sbuff);
    
    sprintf(sbuff, " viewkeep=\"%s\"", m_viewKeep ? "yes" : "no");
    cmd += string(sbuff);
    
    if ( ! m_altImg.empty() ) {
      sprintf(sbuff, " altimg=\"%s\"", m_altImg.c_str());
      cmd += string(sbuff);
    }
    
    cmd += string("/>");
    pApp->parseXMLCommand(cmd);
  }
  else if ( m_actType == ACT_Movie ) {
    std::string cmd = s_movShowPath + " " + m_refPath + " &";
    system(cmd.c_str());
  }
  else if ( m_actType == ACT_Image ) {
    std::string cmd = s_imgShowPath + " " + m_refPath + " &";
    system(cmd.c_str());
  }
  else if ( m_actType == ACT_Text ) {
    std::string cmd = s_txtShowPath + " " + m_refPath + " &";
    system(cmd.c_str());
  }
}


bool vsnAnchor::outputAnchorXML(std::ostream& os, const size_t ts) const {
  if ( m_refPath.empty() ) return true;

  string idts, idts2, idts4;
  register int i;
  for ( i = 0; i < ts; i++ ) idts.push_back(' ');
  idts2 = idts; idts2 += "  ";
  idts4 = idts2; idts4 += "  ";
  char sbuff[64];

  os << idts << "<anchor>" << endl;

  os << idts2  << "<action type=\"";
  if ( m_actType == ACT_LoadXML ) os << "load";
  else if ( m_actType == ACT_Movie ) os << "movie";
  else if ( m_actType == ACT_Image ) os << "image";
  else if ( m_actType == ACT_Text ) os << "text";
  else {
    os << "\"/></anchor>" << endl;
    return false;
  }
  os << "\" path=\"" << m_refPath << "\"";
  if ( m_fadeSec > 0.f ) {
    sprintf(sbuff, "fade=\"%g\"", m_fadeSec);
    os << endl << idts4 << sbuff;
  }
  if ( m_viewKeep ) {
    os << endl << idts4 << "viewkeep=\"yes\"";
  } else {
    os << endl << idts4 << "viewkeep=\"no\"";
  }
  os << "/>" << endl;

  if ( ! m_altImg.empty() ) {
    os << idts2  << "<alt type=\"image\" path=\"" << m_altImg << "\"/>"
       << endl;
  }

  os << idts << "</anchor>" << endl;

  return true;
}

bool vsnAnchor::parseAnchorXML(xmlNodePtr xnp) {
  static xmlChar* xs;
  xmlNodePtr cur;

  if ( ! xnp ) return false;

  // is 'anchor' node?
  if ( strcmp((const char*)xnp->name, "anchor") ) return false;

  // get children node
  cur = xnp->xmlChildrenNode;
  while ( cur ) {
    if ( cur->type == XML_COMMENT_NODE )
      goto _NEXT_XML_NODE;

    // traverse 'action' node
    if ( !strcmp((const char*)cur->name, "action") ) {
      // type
      if ( xs ) xmlFree(xs);
      xs = xmlGetProp(cur, (const xmlChar*)"type");
      if ( ! xs || strlen((const char*)xs) < 1 )
	goto _NEXT_XML_NODE;
      string typeStr = (const char*)xs;
      if ( typeStr == string("load") )
	m_actType = ACT_LoadXML;
      else if ( typeStr == string("movie") )
	m_actType = ACT_Movie;
      else if ( typeStr == string("image") )
	m_actType = ACT_Image;
      else if ( typeStr == string("text") )
	m_actType = ACT_Text;
      else
	goto _NEXT_XML_NODE;

      // path
      if ( xs ) xmlFree(xs);
      xs = xmlGetProp(cur, (const xmlChar*)"path");
      if ( xs && strlen((const char*)xs) > 0 )
	m_refPath = (const char*)xs;

      // fade
      if ( xs ) xmlFree(xs);
      xs = xmlGetProp(cur, (const xmlChar*)"fade");
      if ( xs && strlen((const char*)xs) > 0 )
	m_fadeSec = (float)atof((const char*)xs);

      // viewkeep
      if ( xs ) xmlFree(xs);
      xs = xmlGetProp(cur, (const xmlChar*)"viewkeep");
      if ( xs && strlen((const char*)xs) > 0 )
	m_viewKeep = (strcmp((const char*)xs, "yes") == 0);

    } // end of 'action' node
    // traverse 'alt' node
    else if ( !strcmp((const char*)cur->name, "alt") ) {
      if ( xs ) xmlFree(xs);
      xs = xmlGetProp(cur, (const xmlChar*)"type");
      if ( ! xs || strlen((const char*)xs) < 1 ) {
	goto _NEXT_XML_NODE;
      }
      string typeStr = (const char*)xs;

      if ( typeStr == string("image") ) {
	// path
	if ( xs ) xmlFree(xs);
	xs = xmlGetProp(cur, (const xmlChar*)"path");
	if ( xs && strlen((const char*)xs) > 0 )
	  m_altImg = (const char*)xs;
      } // end of 'image' type
    } // end of 'alt' node

  _NEXT_XML_NODE:
    cur = cur->next;
  } // end of while(cur)

  return true;
}


//----------------------------------------------------------------
// class vsnEditAnchorDlg
//----------------------------------------------------------------

BEGIN_EVENT_TABLE(vsnEditAnchorDlg, wxDialog)
EVT_COMBOBOX(EditAnchorDlg_ActTypeCombo, vsnEditAnchorDlg::OnActTypeCombo)
EVT_BUTTON(EditAnchorDlg_PathBrowsBtn, vsnEditAnchorDlg::OnPathBrowsBtn)
EVT_BUTTON(EditAnchorDlg_AltImgBrowsBtn, vsnEditAnchorDlg::OnAltImgBrowsBtn)
EVT_BUTTON(EditAnchorDlg_CancelBtn, vsnEditAnchorDlg::OnCancelBtn)
EVT_BUTTON(EditAnchorDlg_OkBtn, vsnEditAnchorDlg::OnOkBtn)
EVT_CLOSE(vsnEditAnchorDlg::OnClose)
END_EVENT_TABLE()


vsnEditAnchorDlg::vsnEditAnchorDlg(wxWindow *parent, vsnAnchor *refAnchor)
: wxDialog(parent, -1, wxString(wxT("edit anchor"))), p_anchor(refAnchor)
{
  assert(parent);

  wxBoxSizer *sizerTop = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer* sizerH;

  // action type
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  sizerTop->Add(sizerH, 0, wxEXPAND|wxALL, 5);
  sizerH->Add(new wxStaticText(this, -1, wxT("action type")),
              0, wxALIGN_LEFT|wxEXPAND|wxALL, 3);
  m_pActTypeCombo = new wxComboBox(this, EditAnchorDlg_ActTypeCombo, wxT(""),
				   wxDefaultPosition, wxDefaultSize, 0, NULL,
				   wxCB_READONLY);
  sizerH->Add(m_pActTypeCombo, wxEXPAND|wxALL, 3);

  m_pActTypeCombo->Append(wxT("load XML"));
  m_pActTypeCombo->Append(wxT("show movie"));
  m_pActTypeCombo->Append(wxT("show image"));
  m_pActTypeCombo->Append(wxT("show text"));

  // path
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  sizerTop->Add(sizerH, 0, wxEXPAND|wxALL, 5);
  sizerH->Add(new wxStaticText(this, -1, wxT("path")),
              0, wxALIGN_LEFT|wxEXPAND|wxALL, 3);
  m_pPathTxt = new wxTextCtrl(this, EditAnchorDlg_PathTxt, wxT(""),
			      wxDefaultPosition, wxDefaultSize);
  sizerH->Add(m_pPathTxt, 1, wxEXPAND|wxALL, 3);
  m_pPathBrowsBtn= new wxButton(this, EditAnchorDlg_PathBrowsBtn, wxT("..."),
				wxDefaultPosition, wxSize(-1,-1),
				wxBU_EXACTFIT);
  sizerH->Add(m_pPathBrowsBtn, 0, wxALIGN_LEFT|wxALL, 3);

  // alt image
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  sizerTop->Add(sizerH, 0, wxEXPAND|wxALL, 5);
  m_pAltImgChk = new wxCheckBox(this, EditAnchorDlg_AltImgChk,
				wxT("use alt image"));
  sizerH->Add(m_pAltImgChk, 0, wxEXPAND|wxALL, 5);
  m_pAltImgTxt = new wxTextCtrl(this, EditAnchorDlg_PathTxt, wxT(""),
				wxDefaultPosition, wxSize(200,-1));
  sizerH->Add(m_pAltImgTxt, 1, wxEXPAND|wxALL, 3);
  m_pAltImgBrowsBtn= new wxButton(this, EditAnchorDlg_AltImgBrowsBtn,
				  wxT("..."), wxDefaultPosition,
				  wxDefaultSize, wxBU_EXACTFIT);
  sizerH->Add(m_pAltImgBrowsBtn, 0, wxALIGN_LEFT|wxALL, 3);

  // fade
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  sizerTop->Add(sizerH, 0, wxEXPAND|wxALL, 5);
  sizerH->Add(new wxStaticText(this, -1, wxT("fade in/out time(sec)")),
              0, wxALIGN_LEFT|wxEXPAND|wxALL, 3);
  m_pFadeTxt = new wxTextCtrl(this, EditAnchorDlg_FadeTxt, wxT("0.0"),
			      wxDefaultPosition, wxDefaultSize);
  sizerH->Add(m_pFadeTxt, 1, wxEXPAND|wxALL, 3);

  // keep view
  m_pKeepViewChk = new wxCheckBox(this, EditAnchorDlg_KeepViewChk,
				  wxT("keep view point"));
  sizerTop->Add(m_pKeepViewChk, 0, wxEXPAND|wxALL, 5);

  // buttons
  sizerTop->Add(new wxStaticLine(this, -1, wxDefaultPosition,
				 wxSize(3,3), wxHORIZONTAL),
		0, wxEXPAND|wxALL, 0);

  sizerH = new wxBoxSizer(wxHORIZONTAL);
  sizerTop->Add(sizerH, 0, wxALIGN_RIGHT|wxALL, 5);
  m_pCancelBtn = new wxButton(this, EditAnchorDlg_CancelBtn, wxT("Cancel"));
  sizerH->Add(m_pCancelBtn, 0, wxALL, 3);
  sizerH->Add(10, 10);
  m_pOkBtn = new wxButton(this, EditAnchorDlg_OkBtn, wxT("OK"));
  sizerH->Add(m_pOkBtn, 0, wxALL, 3);

  // postprocess
  SetAutoLayout(TRUE);
  SetSizer(sizerTop);
  sizerTop->SetSizeHints(this);
  sizerTop->Layout();
  sizerTop->Fit(this);

  (void)update();
}

vsnEditAnchorDlg::~vsnEditAnchorDlg() {
}


/* interface */

bool vsnEditAnchorDlg::update() {
  if ( ! p_anchor ) return false;
  if ( ! m_pActTypeCombo || ! m_pPathTxt || ! m_pFadeTxt || ! m_pKeepViewChk ||
       ! m_pAltImgChk || ! m_pAltImgTxt )
    return false;
  string val;

  // action type
  vsnAnchor::ACT_Type actType = p_anchor->getActType();
  switch ( actType ) {
  case vsnAnchor::ACT_Movie:
    m_pActTypeCombo->SetSelection(1); break;
  case vsnAnchor::ACT_Image:
    m_pActTypeCombo->SetSelection(2); break;
  case vsnAnchor::ACT_Text:
    m_pActTypeCombo->SetSelection(3); break;
  case vsnAnchor::ACT_LoadXML: default:
    m_pActTypeCombo->SetSelection(0);
  }

  // path
  val = p_anchor->getRefPath();
  m_pPathTxt->SetValue(vsnApp::ConvSysToWx(val));

  // fade
  char buff[64];
  sprintf(buff, "%g", p_anchor->getFadeSec());
  m_pFadeTxt->SetValue(vsnApp::ConvSysToWx(string(buff)));

  // keep view
  m_pKeepViewChk->SetValue(p_anchor->getViewKeep());

  // alt image
  val = p_anchor->getAltImg();
  m_pAltImgChk->SetValue(! val.empty());
  m_pAltImgTxt->SetValue(vsnApp::ConvSysToWx(val));

  // enable / diable
  bool mode = true;
  if ( actType != 0 ) mode = false;
  m_pFadeTxt->Enable(mode);
  m_pKeepViewChk->Enable(mode);
  m_pAltImgChk->Enable(mode);
  m_pAltImgTxt->Enable(mode);
  m_pAltImgBrowsBtn->Enable(mode);

  return true;
}

void vsnEditAnchorDlg::setRefAnchor(vsnAnchor *refAnchor) {
  if ( p_anchor == refAnchor ) return;
  p_anchor = refAnchor;

  (void)update();
}


/* event handler */

void vsnEditAnchorDlg::OnActTypeCombo(wxCommandEvent& event) {
  if ( ! p_anchor ) return;
  if ( ! m_pActTypeCombo || ! m_pFadeTxt || ! m_pKeepViewChk ||
       ! m_pAltImgChk || ! m_pAltImgTxt || ! m_pAltImgBrowsBtn ) return;

  bool mode = true;
  int actType = m_pActTypeCombo->GetSelection();
  if ( actType != 0 ) mode = false;

  m_pFadeTxt->Enable(mode);
  m_pKeepViewChk->Enable(mode);
  m_pAltImgChk->Enable(mode);
  m_pAltImgTxt->Enable(mode);
  m_pAltImgBrowsBtn->Enable(mode);
}

void vsnEditAnchorDlg::OnPathBrowsBtn(wxCommandEvent& event) {
  if ( ! p_anchor ) return;
  if ( ! m_pPathTxt || ! m_pActTypeCombo ) return;
  vsnApp* pApp = vsnApp::GetApp();

  wxFileDialog fileDlg(this, wxT("select file for anchor action"),
                       wxT(""), wxT(""), // default Dir / File
		       wxT("XML file (*.xml;*.vsn)|*.xml;*.vsn|")
		       wxT("Movie file (*.mpg;*.mov;*.avi)|*.mpg;*.mov;*.avi|")
		       wxT("Image file (*.png;*.jpg;*.tif;*.bmp)|")
			   wxT("*.png;*.jpg;*.tif;*.bmp|")
		       wxT("Text file (*.txt)|*.txt|")
		       wxT("(*)|*"),
                       wxFD_OPEN);
  wxString prevPath = m_pPathTxt->GetValue();
  if ( prevPath.IsEmpty() ) {
    string appCurDir = pApp->getCwd();
    string appCurFile = pApp->getCurrentFilename();
    if ( ! appCurFile.empty() )
      fileDlg.SetDirectory(vsnApp::ConvSysToWx(DirName(appCurFile,
				vsnPath_getDelimChar())));
    else if ( ! appCurDir.empty() )
      fileDlg.SetDirectory(vsnApp::ConvSysToWx(appCurDir));
  } else {
    string tmpPath = vsnPath_normalize(vsnApp::ConvWxToSys(prevPath));
    string tmpDir = DirName(tmpPath, vsnPath_getDelimChar());
    string tmpFile = BaseName(tmpPath, "", vsnPath_getDelimChar());
    if ( ! tmpDir.empty() ) fileDlg.SetDirectory(vsnApp::ConvSysToWx(tmpDir));
    if ( ! tmpFile.empty() ) fileDlg.SetFilename(vsnApp::ConvSysToWx(tmpFile));
  }
  int actType = m_pActTypeCombo->GetSelection();
  fileDlg.SetFilterIndex(actType);

  if ( fileDlg.ShowModal() == wxID_OK ) {
    wxString newPath = fileDlg.GetPath();
    m_pPathTxt->SetValue(newPath);
  } 
}

void vsnEditAnchorDlg::OnAltImgBrowsBtn(wxCommandEvent& event) {
  if ( ! p_anchor ) return;
  if ( ! m_pAltImgTxt ) return;
  vsnApp* pApp = vsnApp::GetApp();

  wxFileDialog fileDlg(this, wxT("select file for anchor action"),
                       wxT(""), wxT(""), // default Dir / File
#if defined(WINDOWS)
                       wxT("BMP files (*.bmp)|*.bmp|")
#endif
                       wxT("PNG files (*.png)|*.png|")
                       wxT("JPEG files (*.jpg)|*.jpg|")
//                     wxT("GIF files (*.gif)|*.gif|")
                       wxT("TIFF files (*.tif)|*.tif|")
#if !defined(WINDOWS)
                       wxT("BMP files (*.bmp)|*.bmp|")
#endif
		       wxT("(*)|*"),
                       wxFD_OPEN);
  wxString prevPath = m_pAltImgTxt->GetValue();
  if ( prevPath.IsEmpty() ) {
    string appCurDir = pApp->getCwd();
    string appCurFile = pApp->getCurrentFilename();
    if ( ! appCurFile.empty() )
      fileDlg.SetDirectory(vsnApp::ConvSysToWx(DirName(appCurFile,
				vsnPath_getDelimChar())));
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
    m_pAltImgTxt->SetValue(newPath);
  } 
}

void vsnEditAnchorDlg::OnOkBtn(wxCommandEvent& event) {
  if ( ! p_anchor ) return;
  if ( ! m_pActTypeCombo || ! m_pPathTxt || ! m_pFadeTxt || ! m_pKeepViewChk ||
       ! m_pAltImgChk || ! m_pAltImgTxt )
    return;

  int actType = m_pActTypeCombo->GetSelection();
  if ( actType < 0 || actType > 3 ) {
    wxMessageDialog dlg(this, wxT("Edit Anchor: ")
				  wxT("invalid action type is specified."),
			wxT("Edit Anchor"), wxOK|wxICON_INFORMATION);
    dlg.ShowModal();
    return;
  }
  p_anchor->setActType((vsnAnchor::ACT_Type)actType);

  wxString loadPath = m_pPathTxt->GetValue();
  if ( ! loadPath.IsEmpty() ) {
    string path = vsnPath_normalize(vsnApp::ConvWxToSys(loadPath));
    p_anchor->setRefPath(path);
  } else
    p_anchor->setRefPath(string(""));

  float fadeSec = 0.f;
  wxString valStr = m_pFadeTxt->GetValue();
  if ( ! valStr.IsEmpty() )
    fadeSec = (float)atof(vsnApp::ConvWxToSys(valStr).c_str());
  p_anchor->setFadeSec(fadeSec);

  p_anchor->setViewKeep(m_pKeepViewChk->GetValue());

  if ( m_pAltImgChk->GetValue() ) {
    wxString altImgPath = m_pAltImgTxt->GetValue();
    if ( ! altImgPath.IsEmpty() ) {
      string path = vsnPath_normalize(vsnApp::ConvWxToSys(altImgPath));
      p_anchor->setAltImg(path);
    } else
      p_anchor->setAltImg(string(""));
  } else
    p_anchor->setAltImg(string(""));

  EndModal(vsn_wxIDOK);
}

void vsnEditAnchorDlg::OnCancelBtn(wxCommandEvent& event) {
  EndModal(vsn_wxIDCANCEL);
}

void vsnEditAnchorDlg::OnClose(wxCloseEvent& event) {
}
