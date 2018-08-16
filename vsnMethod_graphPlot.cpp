//
// vsnMethod_graphPlot
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

#include "vsnMethod_graphPlot.h"
#include "vsnDataObj.h"
#include "vsnData_OctVol.h"
#include "vsnUiView.h"
#include "vsnError.h"
#include "vsnShapeExporter.h"
#include <fstream>
#include <sstream>

using namespace std;
using namespace CES;
using namespace VSN;


//----------------------------------------------------------------
// class vsnMPP_graphPlot
//----------------------------------------------------------------
BEGIN_EVENT_TABLE(vsnMPP_graphPlot, wxPanel)
  EVT_TEXT_ENTER(MPP_graphPlot_ProgramTxt,
		 vsnMPP_graphPlot::OnEnterProgramTxt)
  EVT_BUTTON(MPP_graphPlot_ProgBrwsBtn,
	     vsnMPP_graphPlot::OnProgBrwsBtn)
  EVT_TEXT_ENTER(MPP_graphPlot_PreCommandTxt,
		 vsnMPP_graphPlot::OnEnterPreCommandTxt)
  EVT_TEXT_ENTER(MPP_graphPlot_SamplerTxt,
		 vsnMPP_graphPlot::OnEnterSamplerTxt)
  EVT_BUTTON(MPP_graphPlot_SetSamplerBtn,
	     vsnMPP_graphPlot::OnSetSamplerBtn)
  EVT_COMBOBOX(MPP_graphPlot_SelDataLst,
	       vsnMPP_graphPlot::OnSelDataLst)
  EVT_CHECKLISTBOX(MPP_graphPlot_VecDataChkLst,
		   vsnMPP_graphPlot::OnVecDataChkLst)
  EVT_CHECKBOX(MPP_graphPlot_ShowTitleChk,
               vsnMPP_graphPlot::OnShowTitleChk)
  EVT_CHECKBOX(MPP_graphPlot_ShowLegendChk,
               vsnMPP_graphPlot::OnShowLegendChk)
  EVT_BUTTON(MPP_graphPlot_ExportBtn,
             vsnMPP_graphPlot::OnExportBtn)
  EVT_CHECKBOX(MPP_graphPlot_AutoExportChk,
               vsnMPP_graphPlot::OnAutoExportChk)
END_EVENT_TABLE()


/* constructors / destructor */

vsnMPP_graphPlot::vsnMPP_graphPlot(wxPanel* parent, vsnMethodObj* pm)
  : vsnMethodPP(parent, pm)
{
  assert(parent);
  assert(dynamic_cast<vsnMethod_graphPlot*>(pm));

  // create widgets
  m_pProgramTxt = new wxTextCtrl(this, MPP_graphPlot_ProgramTxt, wxT(""),
				 wxDefaultPosition, wxSize(60,-1),
                                 wxTE_PROCESS_ENTER);
  assert(m_pProgramTxt);
  m_pProgBrwsBtn = new wxButton(this, MPP_graphPlot_ProgBrwsBtn,
				wxT("..."), wxDefaultPosition,
				wxDefaultSize, wxBU_EXACTFIT);
  assert(m_pProgBrwsBtn);

  m_pPreCommandTxt = new wxTextCtrl(this, MPP_graphPlot_PreCommandTxt, wxT(""),
				    wxDefaultPosition, wxSize(60,-1),
				    wxTE_PROCESS_ENTER);
  assert(m_pPreCommandTxt);

  m_pSamplerLst = new wxListBox(this, MPP_graphPlot_SamplerLst);
  assert(m_pSamplerLst);
  m_pSamplerTxt = new wxTextCtrl(this, MPP_graphPlot_SamplerTxt, wxT(""),
				 wxDefaultPosition, wxDefaultSize,
                                 wxTE_READONLY|wxTE_PROCESS_ENTER);
  assert(m_pSamplerTxt);
  m_pSetSamplerBtn = new wxButton(this, MPP_graphPlot_SetSamplerBtn,
                                  wxT("set sampler"));
  assert(m_pSetSamplerBtn);
  m_pSelDataLst = new wxComboBox(this, MPP_graphPlot_SelDataLst, wxT(""),
				 wxDefaultPosition, wxDefaultSize,
                                 0, NULL, wxCB_READONLY);
  assert(m_pSelDataLst);
  m_pVecDataChkLst = new wxCheckListBox(this, MPP_graphPlot_VecDataChkLst);
  assert(m_pVecDataChkLst);

  m_pShowTitleChk = new wxCheckBox(this, MPP_graphPlot_ShowTitleChk,
                                   wxT("show title"));
  assert(m_pShowTitleChk);
  m_pShowLegendChk = new wxCheckBox(this, MPP_graphPlot_ShowLegendChk,
                                   wxT("show legend"));
  assert(m_pShowLegendChk);

  m_pExportBtn = new wxButton(this, MPP_graphPlot_ExportBtn, wxT("export"));
  assert(m_pExportBtn);
  m_pAutoExportChk = new wxCheckBox(this, MPP_graphPlot_AutoExportChk,
                                    wxT("auto export"));
  assert(m_pAutoExportChk);

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

  // sampler selector
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND|wxALL, 3);
  sizerH->Add(new wxStaticText(this, -1, wxT("selected")),
              0, wxALIGN_LEFT|wxALL, 3);
  sizerH->Add(m_pSamplerTxt, 0, wxEXPAND|wxALL, 3);
  topsizer->Add(m_pSamplerLst, 0, wxEXPAND|wxALL, 3);
  topsizer->Add(m_pSetSamplerBtn, 0, wxALIGN_LEFT|wxALL, 3);

  topsizer->Add(5, 5);
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

  // show title, legend
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND|wxALL, 3);
  sizerH->Add(m_pShowTitleChk, 0, wxALL, 3);
  sizerH->Add(5, 5);
  sizerH->Add(m_pShowLegendChk, 0, wxALL, 3);

  // export button
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND|wxALL, 0);
  sizerH->Add(m_pExportBtn, 0, wxALL, 3);
  sizerH->Add(m_pAutoExportChk, 0, wxALL, 3);
  
  // post process
  SetSizer(topsizer);
  addTo(parent);
  topsizer->Layout();
  (void)update();
}

vsnMPP_graphPlot::~vsnMPP_graphPlot() {
}


/* interface */

bool vsnMPP_graphPlot::update() {
  if ( ! m_pProgramTxt || ! m_pProgBrwsBtn || ! m_pPreCommandTxt ||
       ! m_pSamplerLst || ! m_pSamplerTxt || ! m_pSetSamplerBtn ||
       ! m_pSelDataLst || ! m_pVecDataChkLst ||
       ! m_pShowTitleChk || ! m_pShowLegendChk )
    return false;

  register size_t i;
  char txt[16];

  vsnMethod_graphPlot* pm = dynamic_cast<vsnMethod_graphPlot*>(p_method);
  if ( ! pm ) return false;
  vsnDataObj* pdo = pm->getRefData();
  if ( ! pdo ) return false;
  vsnNumericalDataIF* pndo = dynamic_cast<vsnNumericalDataIF*>(pdo);
  if ( ! pndo ) return false;

  // program name
  m_pProgramTxt->SetValue(vsnApp::ConvSysToWx(pm->getProgramName()));

  // pre-command
  m_pPreCommandTxt->SetValue(vsnApp::ConvSysToWx(pm->getPreCommand()));

  // selected sampler
  vsnMethod_sampler* psplr = pm->getSampler();
  if ( psplr )
    m_pSamplerTxt->SetValue(vsnApp::ConvSysToWx(psplr->getName()));
  else
    m_pSamplerTxt->SetValue(wxT("none"));

  // sampler list
  m_pSamplerLst->Clear();
  deque<class vsnMethod_sampler*> samplerMtdLst = pm->getSamplerList(pdo);
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

  // show title, legend
  m_pShowTitleChk->SetValue(pm->getShowTitle());
  m_pShowLegendChk->SetValue(pm->getShowLegend());

  // auto export
  m_pAutoExportChk->SetValue(pm->getAutoExport());
  
  return true;
}


/* event handler */

void vsnMPP_graphPlot::OnEnterProgramTxt(wxCommandEvent& event) {
  if ( ! m_pProgramTxt ) return;
  vsnMethod_graphPlot* pm = dynamic_cast<vsnMethod_graphPlot*>(p_method);
  if ( ! pm ) return;

  wxString valStr = m_pProgramTxt->GetValue();
  string pgName = vsnApp::ConvWxToSys(valStr);
  if ( ! pm->setProgramName(pgName) ) {
    update();
    return;
  }
}

void vsnMPP_graphPlot::OnProgBrwsBtn(wxCommandEvent& event) {
  if ( ! m_pProgBrwsBtn || ! m_pProgramTxt ) return;
  vsnMethod_graphPlot* pm = dynamic_cast<vsnMethod_graphPlot*>(p_method);
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

void vsnMPP_graphPlot::OnEnterPreCommandTxt(wxCommandEvent& event) {
  if ( ! m_pPreCommandTxt ) return;
  vsnMethod_graphPlot* pm = dynamic_cast<vsnMethod_graphPlot*>(p_method);
  if ( ! pm ) return;

  wxString valStr = m_pPreCommandTxt->GetValue();
  string pcName = vsnApp::ConvWxToSys(valStr);
  if ( ! pm->setPreCommand(pcName) ) {
    update();
    return;
  }  
}

void vsnMPP_graphPlot::OnEnterSamplerTxt(wxCommandEvent& event) {
  if ( ! m_pSamplerTxt ) return;

  vsnMethod_graphPlot* pm = dynamic_cast<vsnMethod_graphPlot*>(p_method);
  if ( ! pm ) return;
  vsnMethod_sampler* psplr = pm->getSampler();
  if ( ! psplr ) return;
  vsnUiView* puiv = getUiView();
  if ( ! puiv ) return;

  puiv->selectObj((vfrNode*)psplr);
}

void vsnMPP_graphPlot::OnSetSamplerBtn(wxCommandEvent& event) {
  if ( ! m_pSamplerLst || ! m_pSamplerTxt || ! m_pSetSamplerBtn )
    return;

  int sel = m_pSamplerLst->GetSelection();
  if ( sel < 0 ) return;
  m_pSamplerTxt->SetValue(m_pSamplerLst->GetString(sel));

  vsnMethod_graphPlot* pm = dynamic_cast<vsnMethod_graphPlot*>(p_method);
  if ( ! pm ) return;
  vsnDataObj* pdo = pm->getRefData();
  if ( ! pdo ) return;

  pm->setSampler(pdo, sel);
}

void vsnMPP_graphPlot::OnSelDataLst(wxCommandEvent& event) {
  if ( ! m_pSelDataLst ) return;
  WhichDataType sel = event.GetInt();
  if ( sel < 0 ) return;

  wxString valStr = m_pSelDataLst->GetString(sel);
  if ( valStr == wxT("vector length") )
    sel = DATA_Veclen;

  vsnMethod_graphPlot* pm = dynamic_cast<vsnMethod_graphPlot*>(p_method);
  if ( ! pm ) return;
  if ( sel == pm->getSelectedData() ) return;

  if ( ! pm->setSelectedData(sel) )
    update();
}

void vsnMPP_graphPlot::OnVecDataChkLst(wxCommandEvent& event) {
  if ( ! m_pVecDataChkLst ) return;
  int dlen = m_pVecDataChkLst->GetCount();
  if ( dlen < 3 ) return;

  vsnMethod_graphPlot* pm = dynamic_cast<vsnMethod_graphPlot*>(p_method);
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

void vsnMPP_graphPlot::OnShowTitleChk(wxCommandEvent& event) {
  if ( ! m_pShowTitleChk ) return;
  vsnMethod_graphPlot* pm = dynamic_cast<vsnMethod_graphPlot*>(p_method);
  if ( ! pm ) return;

  bool val = m_pShowTitleChk->GetValue();
  if ( ! pm->setShowTitle(val) )
    update();
}

void vsnMPP_graphPlot::OnShowLegendChk(wxCommandEvent& event) {
  if ( ! m_pShowLegendChk ) return;
  vsnMethod_graphPlot* pm = dynamic_cast<vsnMethod_graphPlot*>(p_method);
  if ( ! pm ) return;

  bool val = m_pShowLegendChk->GetValue();
  if ( ! pm->setShowLegend(val) )
    update();
}

void vsnMPP_graphPlot::OnExportBtn(wxCommandEvent& event) {
  vsnMethod_graphPlot* pm = dynamic_cast<vsnMethod_graphPlot*>(p_method);
  if ( ! pm ) return;
  vsnApp* pApp = vsnApp::GetApp();

  wxFileDialog fileDlg(this, wxT("streamLines: specify file to export"),
                       wxT(""), wxT(""), // default Dir / File
                       wxT("CSV (*.csv)|*.csv")
                       wxT("|(*)|*"),
                       wxFD_SAVE);
  // set default params
  string targDir = pApp->getImportDir();
  if ( targDir.empty() ) {
    string appCurFile = pApp->getCurrentFilename();
    if ( ! appCurFile.empty() )
      targDir = DirName(appCurFile, vsnPath_getDelimChar());
  }
  if ( targDir.empty() )
    targDir = pApp->getCwd();
  if ( ! targDir.empty() )
    fileDlg.SetDirectory(vsnApp::ConvSysToWx(targDir));

  // get output path
  if ( fileDlg.ShowModal() != wxID_OK ) return;
  string outPath
    = vsnPath_normalize(vsnApp::ConvWxToSys(fileDlg.GetPath()));
  if ( outPath.empty() ) return;

  // override check
  FILE* ofp = fopen(outPath.c_str(), "r");
  if ( ofp ) {
    fclose(ofp);
    wxString msg = wxT("The specified file has already existed\n  ");
    msg += wxString::FromUTF8(outPath.c_str());
    msg += wxT("\n\nAre you sure to override ?\n");
    wxMessageDialog dlg(NULL, msg, wxT("graphPlot: export"),
                        vsn_wxOK_CANCEL|wxICON_QUESTION);
    if ( dlg.ShowModal() != vsn_wxIDOK ) return;
    }

  // export
  if ( ! pm->exportCsv(outPath) ) {
    ErrMsg(MsgERR, string("graphPlot: export failed.\n  File: ") + outPath);
    return;
  }

  return;
}

void vsnMPP_graphPlot::OnAutoExportChk(wxCommandEvent& event) {
  if ( ! m_pAutoExportChk ) return;
  bool val = m_pAutoExportChk->GetValue();

  vsnMethod_graphPlot* pm = dynamic_cast<vsnMethod_graphPlot*>(p_method);
  if ( ! pm ) return;

  if ( pm->setAutoExport(val) )
    pm->chkNotice();
}


//----------------------------------------------------------------
// class vsnMethod_graphPlot
//----------------------------------------------------------------

/* constructors / destructor */

vsnMethod_graphPlot::vsnMethod_graphPlot(const string& name)
  : vsnMethodObj(name), m_pGnuPlotIF(NULL),
    m_selectedData(DATA_None), m_vecDataIdx(0,1,2),
    m_showTitle(true), m_showLegend(true), m_autoExport(false)
{
  m_pGnuPlotIF = VSN::GetGnuplotIF();
  assert(m_pGnuPlotIF);
  if ( ! m_pGnuPlotIF->Open(m_progName) )
    m_progName = string(VFR_NONAME);
  
  wxString tmpPfx = wxT("vsn_graphPlot_");
  wxString tmpPath = wxFileName::CreateTempFileName(tmpPfx);
  if ( tmpPath.IsEmpty() ) {
    tmpPfx = vsnApp::ConvSysToWx(vsnApp::GetApp()->getCwd());
    tmpPath = wxFileName::CreateTempFileName(tmpPfx);
  }
  if ( ! tmpPath.IsEmpty() ) 
    m_tmpPath = vsnApp::ConvWxToSys(tmpPath);
}

vsnMethod_graphPlot::~vsnMethod_graphPlot() {
  if ( m_pGnuPlotIF )
    delete m_pGnuPlotIF;

  if ( ! m_tmpPath.empty() ) {
    wxRemoveFile(vsnApp::ConvSysToWx(m_tmpPath));
  }
}


/* methods */

bool vsnMethod_graphPlot::setProgramName(const std::string& progName) {
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

bool vsnMethod_graphPlot::setPreCommand(const std::string& preCmd) {
  if ( m_preCommand == preCmd ) return true;
  m_preCommand = preCmd;

  if ( ! update() ) return false;
  return true;
}

bool vsnMethod_graphPlot::setSelectedData(const WhichDataType sd) {
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

bool vsnMethod_graphPlot::setVecDataIdx(const CES::Vec3<int>& vdidx) {
  if ( vdidx[0] == m_vecDataIdx[0] &&
       vdidx[1] == m_vecDataIdx[1] &&
       vdidx[2] == m_vecDataIdx[2] ) return true;
  m_vecDataIdx = vdidx;

  if ( ! update() ) return false;
  updateUI();
  return true; 
}

bool vsnMethod_graphPlot::isValidVecData() const {
  vsnNumericalDataIF* pData = dynamic_cast<vsnNumericalDataIF*>(p_refData);
  if ( ! pData ) return false;

  size_t dlen = pData->getDataLen();
  if ( m_vecDataIdx[0] < 0 || m_vecDataIdx[0] >= dlen ) return false;
  if ( m_vecDataIdx[1] < 0 || m_vecDataIdx[1] >= dlen ) return false;
  if ( m_vecDataIdx[2] < 0 || m_vecDataIdx[2] >= dlen ) return false;
  return true;
}

bool vsnMethod_graphPlot::setShowTitle(const bool stm) {
  if ( m_showTitle == stm ) return true;
  m_showTitle = stm;

  if ( ! update() ) return false;
  updateUI();
  return true;
}

bool vsnMethod_graphPlot::setShowLegend(const bool slm) {
  if ( m_showLegend == slm ) return true;
  m_showLegend = slm;

  if ( ! update() ) return false;
  updateUI();
  return true;
}

bool vsnMethod_graphPlot::DoPlot(const Point2& sampleSize,
				 const vector3* samplePts,
				 const float* values,
				 const float range[2],
				 const std::string& titleStr) const {
  // check gnuplot interface
  if ( ! m_pGnuPlotIF || ! m_pGnuPlotIF->IsConnected() )
    return false;
 
  // check args
  if ( sampleSize.x < 1 || sampleSize.y < 1 || ! samplePts || ! values )
    return false;

  // open output file
  ofstream fout(m_tmpPath.c_str());
  if ( ! fout ) {
    ErrMsg(MsgERR, getMethodType() + string("[") + getName()
	   + string("]: can't open temporary file: ") + m_tmpPath);
    return false;
  }

  // write to file (in NxM order)
  register int i, j, idx;
  Vec3<float> pos, val;
  pos = samplePts[0];
  for ( i = 0; i < sampleSize.x; i++ ) {
    // write coord
    float distance = (Vec3<float>(samplePts[i]) - pos).Length();
    fout << distance;

    // write values
    for ( j = 0; j < sampleSize.y; j++ ) {
      idx = sampleSize.x * j + i;
      fout << '\t' << values[idx];
    } // end of for(j)

    fout << endl;
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

  for ( i = 0; i < sampleSize.y; i++ ) {
    if ( i == 0 ) {
      command = "plot ";
      if ( sampleSize.x <= 1 ) command += "[-1:1]";
      else command += "[]";
      if ( range[0] == range[1] )
	sprintf(txt, "[%g:%g] ", floor(range[0]-1.), ceil(range[1]+1.));
      else
	sprintf(txt, "[] ");
      command += txt;
      command += string("\"");
    } else
      command = string("\"");
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
    sprintf(txt, "using 1:%d ", i + 2); command += txt;
    if ( m_showLegend )
      sprintf(txt, "title \"%d\" ", i);
    else
      sprintf(txt, "title \"\" ");
    command += txt;
    command += string("with linespoints");
    if ( i != sampleSize.y -1 ) command += string(", \\");

    if ( ! m_pGnuPlotIF->Command(command) ) {
      ErrMsg(MsgERR, getMethodType() + string("[") + getName()
             + string("]: can't send command to GNUPLOT"));
      return false;
    }
  } // end of for(i)

  return true;
}

bool vsnMethod_graphPlot::ExportCsv(const std::string& path,
				    const Point2& sampleSize,
				    const vector3* samplePos,
				    const float* values) {
  // check args
  if ( path.empty() ) return false;
  if ( sampleSize.x < 1 || sampleSize.y < 1 || ! samplePos || ! values )
    return false;

  // open output file
  ofstream fout(path.c_str());
  if ( ! fout ) {
    ErrMsg(MsgERR, getMethodType() + string("[") + getName()
	   + string("]: can't open file: ") + path);
    return false;
  }

  // write to file (in NxM order)
  register int i, j, idx;
  Vec3<float> pos, val;
  pos = samplePos[0];
  for ( i = 0; i < sampleSize.x; i++ ) {
    // write coord
    float distance = (Vec3<float>(samplePos[i]) - pos).Length();
    fout << distance;

    // write values
    for ( j = 0; j < sampleSize.y; j++ ) {
      idx = sampleSize.x * j + i;
      fout << ", " << values[idx];
    } // end of for(j)

    fout << endl;
  } // end of for(i)
  fout.close();

  m_exportPath = path;
  return true;
}

bool vsnMethod_graphPlot::setAutoExport(const bool aem) {
  if ( m_autoExport == aem ) return true;
  m_autoExport = aem;
  if ( m_autoExport ) {
    if ( ! update(true) )
      return false;
  }
  updateUI();
  return true;
}


/* from vsnMethodObj */

vsnMethodPP* vsnMethod_graphPlot::getParamPanel(wxPanel* pp) {
  vsnMPP_graphPlot* pp_graphPlot = new vsnMPP_graphPlot(pp, this);
  if ( ! pp_graphPlot ) return NULL;
  return pp_graphPlot;
}


/* from vsnRef_sampler */

void vsnMethod_graphPlot::noticeUpdate() {
  update();
}


/* from vsnIoObject */

bool vsnMethod_graphPlot::parseXML(xmlNodePtr xnp) {
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
      else if ( xsN == string("program") ) {
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
      else if ( xsN == string("show_legend") ) {
	bool slm;
	if ( xsV == string("yes") ) slm = true;
	else if ( xsV == string("no") ) slm = false;
	else {
          ErrMsg(MsgERR, msgHdr +string("invalid value in param show_legend"));
          goto _NEXT_XML_NODE;
        }
        if ( ! setShowLegend(slm) ) {
	  ErrMsg(MsgERR, msgHdr + string("can't set show_legend"));
          goto _NEXT_XML_NODE;
        }
      } // end of "show_legend"
      else if ( xsN == string("auto_export") ) {
        bool aem;
        if ( xsV == string("yes") ) aem = true;
        else if ( xsV == string("no") ) aem = false;
        else {
          ErrMsg(MsgERR, msgHdr + string("invalid value in param auto_export"));
          goto _NEXT_XML_NODE;
        }
        if ( ! setAutoExport(aem) ) {
          ErrMsg(MsgERR, msgHdr + string("can't set auto_export"));
          goto _NEXT_XML_NODE;
        }
      } // end of "auto_export
    } // end of param

  _NEXT_XML_NODE:
    cur = cur->next;
  } // end of while(cur)

  return true;
}

bool vsnMethod_graphPlot::outputXML(std::ostream& os, const size_t ts) {
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
  // sampler
  if ( p_splr ) {
    if ( ! vsnRef_sampler::exportXMLNode(os, ts+2) ) {
      ErrMsg(MsgWARN, msgHdr
             + string("sampler has set, but the sampler has no name,\n")
             + string("so don't output sampler param node"));
    }
  }

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

  // show_title
  if ( ! m_showTitle ) {
    os << idts_2 << "<param name=\"show_title\" value=\"no\" />" << endl;
  }

  // show_legend
  if ( ! m_showLegend ) {
    os << idts_2 << "<param name=\"show_legend\" value=\"no\" />" << endl;
  }

  // auto export
  if ( m_autoExport ) {
    os << idts_2 << "<param name=\"auto_export\" value=\"yes\" />" << endl;
  }
  
  os << idts << "</method>" << endl;
  return true;
}

bool vsnMethod_graphPlot::commandXML(xmlNodePtr xnp) {
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
  else if ( nameStr == "set_program" ) {
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
  else if ( nameStr == "set_show_legend" ) {
    bool slm;
    if ( valueStr == string("yes") ) slm = true;
    else if ( valueStr == string("no") ) slm = false;
    else {
      ErrMsg(MsgERR, msgHdr +string("command set_show_legend: invalid value"));
      return false;
    }
    if ( ! setShowLegend(slm) ) {
      ErrMsg(MsgERR, msgHdr +
             string("command set_show_legend: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_show_legend"
  else if ( nameStr == "export" ) {
    if ( ! exportCsv(valueStr) ) {
      ErrMsg(MsgERR, msgHdr +
             string("command export: failed to export to file: ") + valueStr);
      return false;
    }
  } // end of "export"
  else if ( nameStr == "set_auto_export" ) {
    bool aem;
    if ( valueStr == string("yes") ) aem = true;
    else if ( valueStr == string("no") ) aem = false;
    else {
      ErrMsg(MsgERR, msgHdr + string("command set_auto_export: invalid value"));
      return false;
    }
    if ( ! setAutoExport(aem) ) {
      ErrMsg(MsgERR, msgHdr +
             string("command set_auto_export: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_auto_export"
  else {
    // not my command
    ErrMsg(MsgERR, msgHdr + string("unknown command: ") + nameStr);
    return false;
  }

  return true;
}
