//
// vsnDataReload
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
#include "wx/dir.h"
#include "wx/busyinfo.h"

#include "vsnDataReload.h"
#include "vsnPathUtil.h"
#include "vsnError.h"

using namespace std;
using namespace CES;
using namespace VSN;


//----------------------------------------------------------------
// class vsnDataReload
//----------------------------------------------------------------

/* constructors / destructor */

vsnDataReload::vsnDataReload(vsnDataObj* pRefData)
  : wxTimer(), p_refData(NULL), m_auto(false), m_interval(600.f)
{
  setDataObj(pRefData);
}

vsnDataReload::~vsnDataReload() {
  Stop();
}


/* methods */

bool vsnDataReload::Start() {
  if ( ! p_refData ) return false;
  if ( m_auto )
    wxTimer::Start((int)(m_interval * 1000.f));
  else
    wxTimer::Start((int)(m_interval * 1000.f), TRUE);
  return true;
}

bool vsnDataReload::checkFiles() {
  if ( ! p_refData ) return false;
  if ( ! p_refData->ready() ) return true;
  string msgHdr = string("DataObj[") + p_refData->getName()
    + string("]: reload checkFiles: ");

  // create new file list
  set<string> newFileList;
  if ( m_filePattern.empty() )
    newFileList = m_fileList;
  else {
    wxDir xDir(m_dir.empty() ? wxT(".") : vsnApp::ConvSysToWx(m_dir));
    wxString resStr;
    if ( xDir.GetFirst(&resStr, vsnApp::ConvSysToWx(m_filePattern),
		       wxDIR_FILES) && ! resStr.IsEmpty() ) {
      newFileList.insert(vsnApp::ConvWxToSys(resStr));
      while ( xDir.GetNext(&resStr) )
        newFileList.insert(vsnApp::ConvWxToSys(resStr));
    }
  }
  if ( newFileList.empty() ) {
    if ( m_fileList.empty() ) return true;
    ErrMsg(MsgERR, msgHdr + string("specified file(s) not found"));
    return false;
  }

  // same file(s) ?
  if ( m_fileList == newFileList )
    return p_refData->reload();

  // get the last XML node
  static xmlChar* xs;
  xmlNodePtr xnp = p_refData->getXmlNode();
  if ( ! xnp ) {
    ErrMsg(MsgERR, msgHdr + string("invalid data(no XML node exists)"));
    return false;
  }

  // create new XML node
  string cmd;
  string xmlEncode = vsnApp::GetApp()->getXmlEncoding();
  if ( vsnApp::GetApp()->needPathEncode() && ! xmlEncode.empty() )
    cmd = string("<?xml version=\"1.0\" encoding=\"")
      + xmlEncode + string("\"?>\n");
  cmd += string("<data ");

  // type prop
  if ( xs ) xmlFree(xs);
  xs = xmlGetProp(xnp, (const xmlChar*)"type");
  if ( xs && strlen((const char*)xs) > 0 )
    cmd += string("type=\"") + (const char*)xs + string("\" ");

  // file prop
  cmd += string("file=\"") + vsn_seqfile + string("\" ");

  // name prop
  if ( xs ) xmlFree(xs);
  xs = xmlGetProp(xnp, (const xmlChar*)"name");
  if (  xs && strlen((const char*)xs) > 0 )
    cmd += string("name=\"") + (const char*)xs + string("\" ");

  // base_dir prop
  if ( ! m_dir.empty() )
    cmd += string("base_dir=\"") + m_dir + string("\" ");

  cmd += string(">\n"); 

  // seq
  set<string>::iterator it;
  for ( it = newFileList.begin(); it != newFileList.end(); it++ ) {
    cmd += string("<seq file=\"") + (*it) + string("\" />\n");
  } // end of for(i)
  cmd += string("</data>\n");

  xmlDocPtr doc = xmlParseMemory(cmd.c_str(), cmd.size());
  if ( ! doc ) return false;
  xmlNodePtr pNewNode = xmlDocGetRootElement(doc);
  if ( ! pNewNode ) {
    xmlFreeDoc(doc);
    return false;
  }

  // child nodes
  xmlNodePtr wkn;
  xmlNodePtr cur = xnp->xmlChildrenNode;
  while ( cur ) {
    if ( cur->type == XML_COMMENT_NODE ) {
      cur = cur->next; continue;
    }
    if ( !strcmp((const char*)cur->name, "xform") ||
	 !strcmp((const char*)cur->name, "param") ||
	 !strcmp((const char*)cur->name, "method") ) {
      wkn = xmlCopyNode(cur, 1);
      if ( wkn ) xmlAddChild(pNewNode, wkn);
    }
    cur = cur->next;
  } // end of while(cur)

  // re-init
  if ( ! p_refData->init(vsn_seqfile, pNewNode) ) {
    ErrMsg(MsgERR, msgHdr + string("data initialize failed"));
    xmlFreeDoc(doc);
    return false;
  }
  xmlFreeDoc(doc);

  // set to the last step
  vsnTimeSeriesDataIF* pTsData = dynamic_cast<vsnTimeSeriesDataIF*>(p_refData);
  if ( pTsData ) {
    size_t nts = pTsData->getNumSteps();
    if ( nts > 1 )
      pTsData->setCurrentStepIdx(nts - 1);
  }

  // update methods
  register size_t i, numMdt = p_refData->getNumMethod();
  for ( i = 0; i < numMdt; i++ ) {
    vsnMethodObj* pMtd = p_refData->getMethod(i);
    if ( pMtd ) pMtd->reloaded();
  } // end of for(i)

  // update TimeSeriesData refers
  vsnTimeSeriesDataIF* ptsd = dynamic_cast<vsnTimeSeriesDataIF*>(p_refData);
  if ( ptsd ) ptsd->updateTSDataRefers();

  return update();
}

bool vsnDataReload::update() {
  if ( ! p_refData ) return false;
  m_dir = string("");
  m_fileList.clear();

  if ( ! p_refData->hasMultiFiles() ) {
    string filePath = p_refData->getFilePath();
    m_dir = DirName(filePath, vsnPath_getDelimChar());
    string fileName = BaseName(filePath, string(""), vsnPath_getDelimChar());
    if ( ! fileName.empty() )
      m_fileList.insert(fileName.c_str());
  }
  else {
    deque<string> fileList = p_refData->getFileList();
    if ( ! fileList.empty() ) {
      m_dir = DirName(fileList[0], vsnPath_getDelimChar());
      register int i;
      for ( i = 0; i < fileList.size(); i++ ) {
	string fileName = BaseName(fileList[i],
				   string(""), vsnPath_getDelimChar());
	if ( fileName.empty() ) continue;
	m_fileList.insert(fileName.c_str());
      } // end of for(i)
    }
  }
  return true;
}

bool vsnDataReload::setDataObj(vsnDataObj* pRefData) {
  if ( p_refData == pRefData ) return true;
  p_refData = pRefData;
  return update();
}

void vsnDataReload::setAuto(const bool am) {
  if ( m_auto == am ) return;
  m_auto = am;
  if ( ! m_auto && IsRunning() )
    Stop();
}

bool vsnDataReload::setInterval(const float its) {
  if ( m_interval == its ) return true;
  m_interval = its;
  if ( m_interval < 0.001f ) {
    m_interval = 0.f;
    Stop();
  }

  if ( m_auto && IsRunning() ) {
    Stop();
    Start();
  }
  return true;
}

bool vsnDataReload::setPattern(const std::string& patStr) {
  if ( m_filePattern == patStr ) return true;
  m_filePattern = patStr;
  return true;
}


/* from wxTimer */

void vsnDataReload::Notify() {
  if ( m_mutexTimerJob.TryLock() == wxMUTEX_BUSY )
    return;
  //-------------------- LOCK BEGIN --------------------
  if ( ! checkFiles() ) {
    setAuto(false);
  }
  //-------------------- LOCK END --------------------
  m_mutexTimerJob.Unlock();

  vsnApp::GetApp()->Yield();
}


//----------------------------------------------------------------
// class vsnDataReloadDlg
//----------------------------------------------------------------

BEGIN_EVENT_TABLE(vsnDataReloadDlg, wxDialog)
  EVT_BUTTON(DataReloadDlg_OkBtn, vsnDataReloadDlg::OnOkBtn)
  EVT_BUTTON(DataReloadDlg_CancelBtn, vsnDataReloadDlg::OnCancelBtn)
  EVT_CLOSE(vsnDataReloadDlg::OnClose)
END_EVENT_TABLE()


/* constructors, destructor */

vsnDataReloadDlg::vsnDataReloadDlg(wxWindow *parent, vsnDataObj* pRefData)
  : wxDialog(parent, -1, wxString(wxT("setup auto reload"))),
    p_refData(NULL)
{
  assert(parent);

  wxBoxSizer* sizerTop = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer* sizerH;

  // auto / interval
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  sizerTop->Add(sizerH, 0, wxEXPAND|wxALL, 0);

  m_pAutoChk = new wxCheckBox(this, DataReloadDlg_AutoChk,
			      wxT("auto reload"));
  sizerH->Add(m_pAutoChk, 0, wxALL, 3);
  sizerH->Add(5, 5);

  wxStaticText* label0 = new wxStaticText(this, -1, wxT("reload interval"));
  wxSize lblSz0 = label0->GetSize();
  sizerH->Add(label0, 0, wxALIGN_LEFT|wxALL, 3);
  m_pIntervalTxt = new wxTextCtrl(this, DataReloadDlg_IntervalTxt, wxT("0.0"),
                                  wxDefaultPosition, wxDefaultSize);
  sizerH->Add(m_pIntervalTxt, 1, wxEXPAND|wxALL, 3);

  // current dir/files
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  sizerTop->Add(sizerH, 0, wxEXPAND|wxALL, 0);
  sizerH->Add(new wxStaticText(this, -1, wxT("base dir"),
			       wxDefaultPosition, lblSz0),
	      0, wxALIGN_LEFT|wxALL, 3);
  m_pDirTxt = new wxTextCtrl(this, DataReloadDlg_DirTxt, wxT(""),
			     wxDefaultPosition, wxDefaultSize,
			     wxTE_READONLY);
  sizerH->Add(m_pDirTxt, 1, wxEXPAND|wxALL, 3);

  sizerH = new wxBoxSizer(wxHORIZONTAL);
  sizerTop->Add(sizerH, 0, wxEXPAND|wxALL, 0);
  sizerH->Add(new wxStaticText(this, -1, wxT("file list"),
			       wxDefaultPosition, lblSz0),
	      0, wxALIGN_LEFT|wxALL, 3);
  m_pFileList = new wxListBox(this, DataReloadDlg_FileList,
			      wxDefaultPosition, wxSize(200,150));
  sizerH->Add(m_pFileList, 1, wxEXPAND|wxALL, 3);

  // pattern
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  sizerTop->Add(sizerH, 0, wxEXPAND|wxALL, 0);
  sizerH->Add(new wxStaticText(this, -1, wxT("file pattern"),
			       wxDefaultPosition, lblSz0),
	      0, wxALIGN_LEFT|wxALL, 3);
  m_pPatternTxt = new wxTextCtrl(this, DataReloadDlg_PatternTxt,wxT(""),
				 wxDefaultPosition, wxDefaultSize);
  sizerH->Add(m_pPatternTxt, 1, wxEXPAND|wxALL, 3);

  // buttons
  sizerTop->Add(
    new wxStaticLine(this, -1, wxDefaultPosition, wxSize(300,3), wxHORIZONTAL),
    0, wxEXPAND|wxALL, 0);
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  sizerTop->Add(sizerH, 0, wxALIGN_RIGHT|wxALL, 5);

  m_pOkBtn = new wxButton(this, DataReloadDlg_OkBtn, wxT("OK"));
  sizerH->Add(m_pOkBtn);
  sizerH->Add(5, 5);
  m_pCancelBtn = new wxButton(this, DataReloadDlg_CancelBtn, wxT("Cancel"));
  sizerH->Add(m_pCancelBtn);

  // post process
  SetAutoLayout(TRUE);
  SetSizer(sizerTop);
  sizerTop->SetSizeHints(this);
  sizerTop->Fit(this);

  setRefData(pRefData);
}

vsnDataReloadDlg::~vsnDataReloadDlg() {
}


/* methods */

void vsnDataReloadDlg::update() {
  if ( ! m_pAutoChk || ! m_pIntervalTxt ||
       ! m_pDirTxt || ! m_pFileList || ! m_pPatternTxt ) return;
  if ( ! p_refData ) return;
  vsnDataReload* pReloader = p_refData->getReloader();
  if ( ! pReloader ) return;

  m_pAutoChk->SetValue(pReloader->isAuto());

  char txt[64];
  sprintf(txt, "%g", pReloader->getInterval());
  m_pIntervalTxt->SetValue(vsnApp::ConvSysToWx(txt));

  string baseDir = pReloader->getDir();
  m_pDirTxt->SetValue(baseDir.empty() ?
		      wxT("") : vsnApp::ConvSysToWx(baseDir));

  m_pFileList->Clear();
  set<string> fileList = pReloader->getFileList();
  if ( ! fileList.empty() ) {
    set<string>::iterator it;
    for ( it = fileList.begin(); it != fileList.end(); it++ ) {
      if ( it->empty() ) continue;
      m_pFileList->Append(vsnApp::ConvSysToWx(*it));
    } // end of for(it)
  }

  string pattern = pReloader->getPattern();
  m_pPatternTxt->SetValue(pattern.empty() ?
			  wxT("") : vsnApp::ConvSysToWx(pattern));
}

void vsnDataReloadDlg::setRefData(vsnDataObj* pRefData) {
  if ( p_refData == pRefData ) return;
  p_refData = pRefData;

  vsnDataReload* pReloader = p_refData->getReloader();
  if ( pReloader ) pReloader->Stop();

  update();
}


/* event handler */

void vsnDataReloadDlg::OnOkBtn(wxCommandEvent& event) {
  if ( ! m_pAutoChk || ! m_pIntervalTxt ||
       ! m_pDirTxt || ! m_pFileList || ! m_pPatternTxt ) return;
  if ( ! p_refData ) return;
  vsnDataReload* pReloader = p_refData->getReloader();
  if ( ! pReloader ) return;
  wxString valStr;

  bool am = m_pAutoChk->GetValue();
  valStr = m_pIntervalTxt->GetValue();
  float interval = (float)atof(vsnApp::ConvWxToSys(valStr).c_str());
  if ( interval < 0.001f ) {
    interval = 0.f;
    am = false;
  }
  pReloader->setInterval(interval);
  pReloader->setAuto(am);

  valStr = m_pPatternTxt->GetValue();
  pReloader->setPattern(valStr.IsEmpty() ?
			string("") : string(vsnApp::ConvWxToSys(valStr)));

  // start timer if need
  if ( am )
    pReloader->Start();

  if ( IsModal() )
    EndModal(vsn_wxIDOK);
  else
    this->Hide();}

void vsnDataReloadDlg::OnCancelBtn(wxCommandEvent& event) {
  // restart timer if auto-mode is true
  vsnDataReload* pReloader = p_refData->getReloader();
  if ( ! pReloader ) return;
  if ( pReloader->isAuto() )
    pReloader->Start();

  if ( IsModal() )
    EndModal(vsn_wxIDCANCEL);
  else
    this->Hide();
}

void vsnDataReloadDlg::OnClose(wxCloseEvent& event) {
}
