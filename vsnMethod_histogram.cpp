//
// vsnMethod_histogram
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
#include "wx/filename.h"

#include "vsnMethod_histogram.h"
#include "vsnDataObj.h"
#include "vsnData_OctVol.h"
#include "vsnUiView.h"
#include "vsnError.h"
#include <fstream>
#include <sstream>

using namespace std;
using namespace CES;
using namespace VSN;


//----------------------------------------------------------------
// class vsnMPP_histogram
//----------------------------------------------------------------
BEGIN_EVENT_TABLE(vsnMPP_histogram, wxPanel)
  EVT_TEXT_ENTER(MPP_histogram_ProgramTxt,
		 vsnMPP_histogram::OnEnterProgramTxt)
  EVT_BUTTON(MPP_histogram_ProgBrwsBtn,
	     vsnMPP_histogram::OnProgBrwsBtn)
  EVT_TEXT_ENTER(MPP_histogram_PreCommandTxt,
		 vsnMPP_histogram::OnEnterPreCommandTxt)
  EVT_COMBOBOX(MPP_histogram_SelDataLst,
	       vsnMPP_histogram::OnSelDataLst)
  EVT_CHECKLISTBOX(MPP_histogram_VecDataChkLst,
		   vsnMPP_histogram::OnVecDataChkLst)
  EVT_TEXT_ENTER(MPP_histogram_NumClassTxt,
		 vsnMPP_histogram::OnEnterNumClassTxt)
  EVT_CHECKBOX(MPP_histogram_LogscaleChk,
	       vsnMPP_histogram::OnLogscaleChk)
  EVT_CHECKBOX(MPP_histogram_ShowTitleChk,
	       vsnMPP_histogram::OnShowTitleChk)
END_EVENT_TABLE()


/* constructors / destructor */

vsnMPP_histogram::vsnMPP_histogram(wxPanel* parent, vsnMethodObj* pm)
  : vsnMethodPP(parent, pm)
{
  assert(parent);
  assert(dynamic_cast<vsnMethod_histogram*>(pm));

  // create widgets
  m_pProgramTxt = new wxTextCtrl(this, MPP_histogram_ProgramTxt, wxT(""),
                                 wxDefaultPosition, wxSize(60,-1),
                                 wxTE_PROCESS_ENTER);
  assert(m_pProgramTxt);
  m_pProgBrwsBtn = new wxButton(this, MPP_histogram_ProgBrwsBtn,
				wxT("..."), wxDefaultPosition,
                                wxDefaultSize, wxBU_EXACTFIT);
  assert(m_pProgBrwsBtn);

  m_pPreCommandTxt = new wxTextCtrl(this, MPP_histogram_PreCommandTxt, wxT(""),
                                    wxDefaultPosition, wxDefaultSize,
                                    wxTE_PROCESS_ENTER);
  assert(m_pPreCommandTxt);

  m_pSelDataLst = new wxComboBox(this, MPP_histogram_SelDataLst, wxT(""),
                                 wxDefaultPosition, wxDefaultSize,
                                 0, NULL, wxCB_READONLY);
  assert(m_pSelDataLst);
  m_pVecDataChkLst = new wxCheckListBox(this, MPP_histogram_VecDataChkLst);
  assert(m_pVecDataChkLst);

  m_pNumClassTxt = new wxTextCtrl(this, MPP_histogram_NumClassTxt, wxT(""),
				  wxDefaultPosition, wxSize(60,-1),
				  wxTE_PROCESS_ENTER);
  assert(m_pNumClassTxt);
  m_pLogscaleChk = new wxCheckBox(this, MPP_histogram_LogscaleChk,
				  wxT("logscale"));
  assert(m_pLogscaleChk);
  m_pShowTitleChk = new wxCheckBox(this, MPP_histogram_ShowTitleChk,
				   wxT("show title"));
  assert(m_pShowTitleChk);

  // prepare sizers
  wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL); assert(topsizer);
  wxBoxSizer* sizerH;

  // program selector
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND|wxALL, 3);
  sizerH->Add(new wxStaticText(this, -1, wxT("program")),
              0, wxALIGN_LEFT|wxALL, 3);
  sizerH->Add(m_pProgramTxt, 1, wxEXPAND|wxALL, 3);
  sizerH->Add(m_pProgBrwsBtn, 0, wxALIGN_RIGHT|wxALL, 3);

  // pre-command
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND|wxALL, 3);
  sizerH->Add(new wxStaticText(this, -1, wxT("pre command")),
              0, wxALIGN_LEFT|wxALL, 3);
  sizerH->Add(m_pPreCommandTxt, 1, wxEXPAND|wxALL, 3);

  topsizer->Add(new wxStaticLine(this, -1, wxDefaultPosition, wxSize(3,3),
                                 wxHORIZONTAL), 0, wxEXPAND|wxALL, 0);

  // data selector
  topsizer->Add(new wxStaticText(this, -1, wxT("select scalar data")),
                0, wxALIGN_LEFT|wxALL, 3);
  topsizer->Add(m_pSelDataLst, 0, wxEXPAND|wxALL, 3);

  // vector data selector
  topsizer->Add(new wxStaticText(this, -1, wxT("vector components")),
                0, wxALIGN_LEFT|wxALL, 3);
  topsizer->Add(m_pVecDataChkLst, 0, wxEXPAND|wxALL, 3);

  // #of classes, logscale
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND|wxALL, 3);
  sizerH->Add(new wxStaticText(this, -1, wxT("#of classes")),
              0, wxALIGN_LEFT|wxALL, 3);
  sizerH->Add(m_pNumClassTxt, 1, wxEXPAND|wxALL, 3);

  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND|wxALL, 3);
  sizerH->Add(m_pLogscaleChk, 0, wxALL, 3);

  // show title
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND|wxALL, 3);
  sizerH->Add(m_pShowTitleChk, 0, wxALL, 3);

  // post process
  SetSizer(topsizer);
  addTo(parent);
  topsizer->Layout();
  (void)update();
}

vsnMPP_histogram::~vsnMPP_histogram() {
}


/* interface */

bool vsnMPP_histogram::update() {
  if ( ! m_pProgramTxt || ! m_pProgBrwsBtn || ! m_pPreCommandTxt ||
       ! m_pSelDataLst || ! m_pVecDataChkLst || ! m_pNumClassTxt ||
       ! m_pLogscaleChk || ! m_pShowTitleChk )
    return false;

  register size_t i;
  char txt[64];

  vsnMethod_histogram* pm = dynamic_cast<vsnMethod_histogram*>(p_method);
  if ( ! pm ) return false;
  vsnDataObj* pdo = pm->getRefData();
  if ( ! pdo ) return false;
  vsnNumericalDataIF* pndo = dynamic_cast<vsnNumericalDataIF*>(pdo);
  if ( ! pndo ) return false;

  // program name
  m_pProgramTxt->SetValue(vsnApp::ConvSysToWx(pm->getProgramName()));

  // pre-command
  m_pPreCommandTxt->SetValue(vsnApp::ConvSysToWx(pm->getPreCommand()));

  // select scalar data
  int dlen = (int)pndo->getDataLen();
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

  // #of classes
  sprintf(txt, "%lu", pm->getNumClasses());
  m_pNumClassTxt->SetValue(vsnApp::ConvSysToWx(txt));

  // logscale mode
  m_pLogscaleChk->SetValue(pm->getLogscale());

  // show title
  m_pShowTitleChk->SetValue(pm->getShowTitle());

  return true;
}


/* event handler */

void vsnMPP_histogram::OnEnterProgramTxt(wxCommandEvent& event) {
  if ( ! m_pProgramTxt ) return;
  vsnMethod_histogram* pm = dynamic_cast<vsnMethod_histogram*>(p_method);
  if ( ! pm ) return;

  wxString valStr = m_pProgramTxt->GetValue();
  string pgName = vsnApp::ConvWxToSys(valStr);
  if ( ! pm->setProgramName(pgName) ) {
    update();
    return;
  }
}

void vsnMPP_histogram::OnProgBrwsBtn(wxCommandEvent& event) {
  if ( ! m_pProgBrwsBtn || ! m_pProgramTxt ) return;
  vsnMethod_histogram* pm = dynamic_cast<vsnMethod_histogram*>(p_method);
  if ( ! pm ) return;

  wxFileDialog fileDlg(this, wxT("select gnuplot program"),
                       wxT(""), wxT(""), // default Dir / File
                       wxT("(*)|*"), wxFD_OPEN);

  wxString valStr = m_pProgramTxt->GetValue();
  if ( ! valStr.IsEmpty() )
    fileDlg.SetPath(valStr);
  else
    fileDlg.SetDirectory(vsnApp::ConvSysToWx(vsnApp::GetApp()->getCwd()));

  if ( fileDlg.ShowModal() != wxID_OK ) return;
  string progPath = vsnApp::ConvWxToSys(fileDlg.GetPath());
  if ( ! pm->setProgramName(progPath) ) return;

  update(); // update ProgramTxt
}

void vsnMPP_histogram::OnEnterPreCommandTxt(wxCommandEvent& event) {
  if ( ! m_pPreCommandTxt ) return;
  vsnMethod_histogram* pm = dynamic_cast<vsnMethod_histogram*>(p_method);
  if ( ! pm ) return;

  wxString valStr = m_pPreCommandTxt->GetValue();
  string pcName = vsnApp::ConvWxToSys(valStr);
  if ( ! pm->setPreCommand(pcName) ) {
    update();
    return;
  }
}

void vsnMPP_histogram::OnSelDataLst(wxCommandEvent& event) {
  if ( ! m_pSelDataLst ) return;
  WhichDataType sel = event.GetInt();
  if ( sel < 0 ) return;

  wxString valStr = m_pSelDataLst->GetString(sel);
  if ( valStr == wxT("vector length") )
    sel = DATA_Veclen;

  vsnMethod_histogram* pm = dynamic_cast<vsnMethod_histogram*>(p_method);
  if ( ! pm ) return;
  if ( sel == pm->getSelectedData() ) return;

  if ( ! pm->setSelectedData(sel) )
    update();
}

void vsnMPP_histogram::OnVecDataChkLst(wxCommandEvent& event) {
  if ( ! m_pVecDataChkLst ) return;
  int dlen = m_pVecDataChkLst->GetCount();
  if ( dlen < 3 ) return;

  vsnMethod_histogram* pm = dynamic_cast<vsnMethod_histogram*>(p_method);
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

  if ( ! pm->setVecDataIdx(vidx) )
    update();
}

void vsnMPP_histogram::OnEnterNumClassTxt(wxCommandEvent& event) {
  if ( ! m_pNumClassTxt ) return;
  vsnMethod_histogram* pm = dynamic_cast<vsnMethod_histogram*>(p_method);
  if ( ! pm ) return;

  wxString valStr = m_pNumClassTxt->GetValue();
  if ( valStr.IsEmpty() ) {
    char txt[64]; sprintf(txt, "%lu", pm->getNumClasses());
    m_pNumClassTxt->SetValue(vsnApp::ConvSysToWx(txt));
    return;
  }
  int val = atoi(vsnApp::ConvWxToSys(valStr).c_str());
  if ( ! pm->setNumClasses(val) ) {
    update();
    return;
  }
}

void vsnMPP_histogram::OnLogscaleChk(wxCommandEvent& event) {
  if ( ! m_pLogscaleChk) return;
  vsnMethod_histogram* pm = dynamic_cast<vsnMethod_histogram*>(p_method);
  if ( ! pm ) return;

  bool val = m_pLogscaleChk->GetValue();
  if ( ! pm->setLogscale(val) )
    update();
}

void vsnMPP_histogram::OnShowTitleChk(wxCommandEvent& event) {
  if ( ! m_pShowTitleChk ) return;
  vsnMethod_histogram* pm = dynamic_cast<vsnMethod_histogram*>(p_method);
  if ( ! pm ) return;

  bool val = m_pShowTitleChk->GetValue();
  if ( ! pm->setShowTitle(val) )
    update();
}


//----------------------------------------------------------------
// class vsnMethod_histogram
//----------------------------------------------------------------

/* constructors / destructor */

vsnMethod_histogram::vsnMethod_histogram(const string& name)
  : vsnMethodObj(name), m_pGnuPlotIF(NULL),
    m_selectedData(DATA_None), m_vecDataIdx(0,1,2), m_numClasses(100),
    m_logscale(false), m_showTitle(true)
{
  m_pGnuPlotIF = VSN::GetGnuplotIF();
  assert(m_pGnuPlotIF);
  if ( ! m_pGnuPlotIF->Open(m_progName) )
    m_progName = string(VFR_NONAME);

  wxString tmpPfx = wxT("vsn_histogram_");
  wxString tmpPath = wxFileName::CreateTempFileName(tmpPfx);
  if ( tmpPath.IsEmpty() ) {
    tmpPfx = vsnApp::ConvSysToWx(vsnApp::GetApp()->getCwd());
    tmpPath = wxFileName::CreateTempFileName(tmpPfx);
  }
  if ( ! tmpPath.IsEmpty() )
    m_tmpPath = vsnApp::ConvWxToSys(tmpPath);
}

vsnMethod_histogram::~vsnMethod_histogram() {
  if ( m_pGnuPlotIF )
    delete m_pGnuPlotIF;

  if ( ! m_tmpPath.empty() ) {
    wxRemoveFile(vsnApp::ConvSysToWx(m_tmpPath));
  }
}


/* methods */

bool vsnMethod_histogram::setProgramName(const std::string& progName) {
  if ( m_progName == progName ) return true;
  m_progName = progName;

  if ( m_pGnuPlotIF->IsConnected() )
    m_pGnuPlotIF->Close();

  if ( ! m_pGnuPlotIF->Open(m_progName) ) {
    ErrMsg(MsgERR, getMethodType() + string("[") + getName()
           + string("]: can't connect to Gnuplot: ") + m_progName);
    m_progName = string(VFR_NONAME);
    return false;
  }

  if ( ! update() ) return false;
  return true;
}

bool vsnMethod_histogram::setPreCommand(const std::string& preCmd) {
  if ( m_preCommand == preCmd ) return true;
  m_preCommand = preCmd;

  if ( ! update() ) return false;
  return true;
}

bool vsnMethod_histogram::setSelectedData(const WhichDataType sd) {
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

bool vsnMethod_histogram::setVecDataIdx(const CES::Vec3<int>& vdidx) {
  if ( vdidx[0] == m_vecDataIdx[0] &&
       vdidx[1] == m_vecDataIdx[1] &&
       vdidx[2] == m_vecDataIdx[2] ) return true;
  m_vecDataIdx = vdidx;

  if ( ! update() ) return false;
  updateUI();
  return true;
}

bool vsnMethod_histogram::isValidVecData() const {
  vsnNumericalDataIF* pData = dynamic_cast<vsnNumericalDataIF*>(p_refData);
  if ( ! pData ) return false;

  size_t dlen = pData->getDataLen();
  if ( m_vecDataIdx[0] < 0 || m_vecDataIdx[0] >= dlen ) return false;
  if ( m_vecDataIdx[1] < 0 || m_vecDataIdx[1] >= dlen ) return false;
  if ( m_vecDataIdx[2] < 0 || m_vecDataIdx[2] >= dlen ) return false;
  return true;
}

bool vsnMethod_histogram::setNumClasses(const size_t nc) {
  if ( m_numClasses == nc ) return true;
  if ( nc < 1 ) {
    ErrMsg(MsgERR, getMethodType() + string("[") + getName()
           + string("]: invalid number of classes"));
    return false;
  }
  m_numClasses = nc;

  if ( ! update() ) return false;
  updateUI();
  return true;
}

bool vsnMethod_histogram::setLogscale(const bool lm) {
  if ( m_logscale == lm ) return true;
  m_logscale = lm;

  if ( ! update() ) return false;
  updateUI();
  return true;
}

bool vsnMethod_histogram::setShowTitle(const bool stm) {
  if ( m_showTitle == stm ) return true;
  m_showTitle = stm;

  if ( ! update() ) return false;
  updateUI();
  return true;
}

bool vsnMethod_histogram::DoPlot(const size_t sampleSize,
                                 const vector2* sampleVals,
                                 const float range[2],
                                 const std::string& titleStr) const {
  // check gnuplot interface
  if ( ! m_pGnuPlotIF || ! m_pGnuPlotIF->IsConnected() )
    return false;

  // check args
  if ( sampleSize < 1 || ! sampleVals )
    return false;

  // open output file
  ofstream fout(m_tmpPath.c_str());
  if ( ! fout ) {
    ErrMsg(MsgERR, getMethodType() + string("[") + getName()
           + string("]: can't open temporary file: ") + m_tmpPath);
    return false;
  }

  // write to file
  register int i;
  for ( i = 0; i < sampleSize; i++ ) {
    fout << sampleVals[i][0] << '\t' << sampleVals[i][1] << endl;
  } // end of for(i)
  fout.close();

  // send commands to gnuplot
  char txt[64];
  string command;

  command = string("set title '");
  if ( m_showTitle ) command += titleStr;
  command += string("'");
  if ( ! m_pGnuPlotIF->Command(command) ) {
    ErrMsg(MsgERR, getMethodType() + string("[") + getName()
           + string("]: can't send command to GNUPLOT"));
    return false;
  }

  if ( ! m_preCommand.empty() ) {
    if ( ! m_pGnuPlotIF->Command(m_preCommand) ) {
      ErrMsg(MsgERR, getMethodType() + string("[") + getName()
             + string("]: can't send pre-command to GNUPLOT"));
      return false;
    }
  } // end of if(m_preCommand)

  if ( m_logscale && sampleSize > 1 )
    command = "set logscale y";
  else
    command = "unset logscale";
  if ( ! m_pGnuPlotIF->Command(command) ) {
    ErrMsg(MsgERR, getMethodType() + string("[") + getName()
           + string("]: can't send command to GNUPLOT"));
    return false;
  }

  command = "plot ";
  if ( sampleSize < 1 ) command += "[-1:1]";
  else if ( sampleSize == 1 ) {
    sprintf(txt, "[%g:%g] ",
	    floor(sampleVals[0][0]-1.), ceil(sampleVals[0][1]+1.));
    command += txt;
  } else command += "[]";
  if ( range[0] == range[1] ) {
    float yr[2]; yr[0] = floor(range[0]-1.); yr[1] = ceil(range[1]+1.);
    if ( m_logscale && yr[0] < 0.f ) {
      yr[0] = 0.0001f;
      if ( yr[1] < yr[0] ) yr[1] = yr[0] + 1.f;
    }
    sprintf(txt, "[%g:%g] ", yr[0], yr[1]);
  } else
    sprintf(txt, "[] ");
  command += txt;
  command += string("\"");
#ifdef WINDOWS
  istringstream tiss(m_tmpPath); char xtc;
  while ( tiss.get(xtc) ) {
    if ( xtc == '\\' ) command += "\\\\";
    else command.push_back(xtc);
  }
#else
  command += m_tmpPath;
#endif
  command += string("\" ");
  command += string("using 1:2 title \"\" ");
  command += string("with steps");

  if ( ! m_pGnuPlotIF->Command(command) ) {
    ErrMsg(MsgERR, getMethodType() + string("[") + getName()
	   + string("]: can't send command to GNUPLOT"));
    return false;
  }

  return true;
}


/* from vsnMethodObj */

vsnMethodPP* vsnMethod_histogram::getParamPanel(wxPanel* pp) {
  vsnMPP_histogram* pp_histogram = new vsnMPP_histogram(pp, this);
  if ( ! pp_histogram ) return NULL;
  return pp_histogram;
}


/* from vsnIoObject */

bool vsnMethod_histogram::parseXML(xmlNodePtr xnp) {
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

      if ( xsN == string("program") ) {
        if ( ! setProgramName(xsV) ) {
          ErrMsg(MsgERR, msgHdr + string("failed to set program: ") + xsV);
          goto _NEXT_XML_NODE;
        }
      } // end of "program"
      else if ( xsN == string("pre_command") ) {
        if ( ! setPreCommand(xsV) ) {
          ErrMsg(MsgERR, msgHdr + string("failed to set pre_command: ") + xsV);
          goto _NEXT_XML_NODE;
        }
      } // end of "pre_command"
      else if ( xsN == string("use_data") ) {
        WhichDataType sdt = -2;
        if ( xsV == string("none") ) sdt = DATA_None;
        else if ( xsV == string("veclen")  ) sdt = DATA_Veclen;
        else if ( xsV.substr(0, 4) == string("data") ) {
          string numStr = xsV.substr(4);
          if ( ! numStr.empty() ) sdt = atoi(numStr.c_str()) + 1;
        }
        if ( ! setSelectedData(sdt) ) {
          ErrMsg(MsgERR, msgHdr + string("failed to select use_data: ") +xsV);
          goto _NEXT_XML_NODE;
        }
      } // end of "use_data"
      else if ( xsN == string("vec_idx") ) {
        Vec3<int> idcs(-1, -1, -1);
        istringstream iss(xsV);
        iss >> idcs;
        if ( idcs[0] < 0 || idcs[1] < 0 ||  idcs[2] < 0 ) {
          ErrMsg(MsgERR, msgHdr +string("bad value format of vec_idx: ") +xsV);
          goto _NEXT_XML_NODE;
        }
        if ( ! setVecDataIdx(idcs) ) {
          ErrMsg(MsgERR, msgHdr + string("failed to set vec_idx: ") +xsV);
          goto _NEXT_XML_NODE;
        }
      } // end of "vec_idx"
      else if ( xsN == string("num_class") ) {
	int nc = atoi(xsV.c_str());
	if ( ! setNumClasses(nc) ) {
	  ErrMsg(MsgERR, msgHdr + string("failed to set num_class: ") +xsV);
          goto _NEXT_XML_NODE;
        }
      } // end of "num_class"
      else if ( xsN == string("logscale") ) {
        bool lm;
        if ( xsV == string("yes") ) lm = true;
        else if ( xsV == string("no") ) lm = false;
        else {
          ErrMsg(MsgERR, msgHdr + string("invalid value in param logscale"));
          goto _NEXT_XML_NODE;
        }
        if ( ! setLogscale(lm) ) {
          ErrMsg(MsgERR, msgHdr + string("can't set logscale"));
          goto _NEXT_XML_NODE;
        }
      } // end of "logscale"
      else if ( xsN == string("show_title") ) {
        bool stm;
        if ( xsV == string("yes") ) stm = true;
        else if ( xsV == string("no") ) stm = false;
        else {
          ErrMsg(MsgERR, msgHdr + string("invalid value in param show_title"));
          goto _NEXT_XML_NODE;
        }
        if ( ! setShowTitle(stm) ) {
          ErrMsg(MsgERR, msgHdr + string("can't set show_title"));
          goto _NEXT_XML_NODE;
        }
      } // end of "show_title"
    } // end of param

  _NEXT_XML_NODE:
    cur = cur->next;
  } // end of while(cur)

  return true;
}

bool vsnMethod_histogram::outputXML(std::ostream& os, const size_t ts) {
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
  // program
  if ( ! m_progName.empty() ) {
    os << idts_2 << "<param name=\"program\" value=\"";
    os << m_progName;
    os << "\" />" << endl;
  }

  // pre_command
  if ( ! m_preCommand.empty() ) {
    os << idts_2 << "<param name=\"pre_command\" value=\"";
    os << m_preCommand;
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

  // num_classes
  if ( m_numClasses != 100 ) {
    os << idts_2 << "<param name=\"num_class\" value=\""
       << m_numClasses << "\" />" << endl;
  }

  // logscale
  if ( m_logscale ) {
    os << idts_2 << "<param name=\"logscale\" value=\"yes\" />" << endl;
  }

  // show_title
  if ( ! m_showTitle ) {
    os << idts_2 << "<param name=\"show_title\" value=\"no\" />" << endl;
  }

  os << idts << "</method>" << endl;
  return true;
}

bool vsnMethod_histogram::commandXML(xmlNodePtr xnp) {
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
  if ( nameStr == "set_program" ) {
    if ( ! setProgramName(valueStr) ) {
      ErrMsg(MsgERR, msgHdr + "command " + nameStr
             + ": set failed: " + valueStr);
      return false;
    }
  } // end of "set_program"
  else if ( nameStr == "set_pre_command" ) {
    if ( ! setPreCommand(valueStr) ) {
      ErrMsg(MsgERR, msgHdr + "command " + nameStr
             + ": set failed: " + valueStr);
      return false;
    }
  } // end of "set_pre_command"
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
  else if ( nameStr == "set_num_class" ) {
    int nc = atoi(valueStr.c_str());
    if ( ! setNumClasses(nc) ) {
      ErrMsg(MsgERR, msgHdr + "command " + nameStr
             + string(": failed to set: ") + valueStr);
      return false;
    }
  } // end of "set_num_class"
  else if ( nameStr == "set_logscale" ) {
    bool lm;
    if ( valueStr == string("yes") ) lm = true;
    else if ( valueStr == string("no") ) lm = false;
    else {
      ErrMsg(MsgERR, msgHdr + string("command set_logscale: invalid value"));
      return false;
    }
    if ( ! setLogscale(lm) ) {
      ErrMsg(MsgERR, msgHdr +
             string("command set_logscale: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_logscale"
  else if ( nameStr == "set_show_title" ) {
    bool stm;
    if ( valueStr == string("yes") ) stm = true;
    else if ( valueStr == string("no") ) stm = false;
    else {
      ErrMsg(MsgERR, msgHdr + string("command set_show_title: invalid value"));
      return false;
    }
    if ( ! setShowTitle(stm) ) {
      ErrMsg(MsgERR, msgHdr +
             string("command set_show_title: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_show_title"
  else {
    // not my command
    ErrMsg(MsgERR, msgHdr + string("unknown command: ") + nameStr);
    return false;
  }

  return true;
}
