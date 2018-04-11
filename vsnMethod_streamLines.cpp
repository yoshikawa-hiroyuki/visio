//
// vsnMethod_streamLines
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

#include "vsnMethod_streamLines.h"
#include "vsnOctTree.h" // for decomp vecIdx
#include "vsnUiView.h"
#include "vsnError.h"

#include "vsnShapeExporter.h"
#include <sstream>

using namespace std;
using namespace CES;
using namespace VSN;


//----------------------------------------------------------------
// class vsnMPP_streamLines
//----------------------------------------------------------------

BEGIN_EVENT_TABLE(vsnMPP_streamLines, wxPanel)
  EVT_TEXT_ENTER(MPP_streamLines_SamplerTxt,
                 vsnMPP_streamLines::OnEnterSamplerTxt)
  EVT_BUTTON(MPP_streamLines_SetSamplerBtn,
             vsnMPP_streamLines::OnSetSamplerBtn)
  EVT_CHECKLISTBOX(MPP_streamLines_VecDataChkLst,
                   vsnMPP_streamLines::OnVecDataChkLst)
  EVT_TEXT_ENTER(MPP_streamLines_DivTimeTxt,
                 vsnMPP_streamLines::OnDivTimeTxt)
  EVT_TEXT_ENTER(MPP_streamLines_SkipOutTxt,
                 vsnMPP_streamLines::OnSkipOutTxt)
  EVT_TEXT_ENTER(MPP_streamLines_MaxPtsTxt,
                 vsnMPP_streamLines::OnMaxPtsTxt)
  EVT_TEXT_ENTER(MPP_streamLines_LineWidthTxt,
                 vsnMPP_streamLines::OnLineWidthTxt)
  EVT_CHECKBOX(MPP_streamLines_ColoredChk,
               vsnMPP_streamLines::OnColoredChk)
  EVT_CHECKBOX(MPP_streamLines_AntiAliasChk,
               vsnMPP_streamLines::OnAntiAliasChk)
  EVT_CHECKBOX(MPP_streamLines_AlwaysChk,
               vsnMPP_streamLines::OnAlwaysChk)
  EVT_BUTTON(MPP_streamLines_RunBtn,
             vsnMPP_streamLines::OnRunBtn)
  EVT_BUTTON(MPP_streamLines_ExportBtn,
             vsnMPP_streamLines::OnExportBtn)
  EVT_CHECKBOX(MPP_streamLines_AutoExportChk,
	       vsnMPP_streamLines::OnAutoExportChk)
END_EVENT_TABLE()


/* constructors / destructor */

vsnMPP_streamLines::vsnMPP_streamLines(wxPanel* parent, vsnMethodObj* pm)
  : vsnMethodPP(parent, pm)
{
  assert(parent);
  assert(dynamic_cast<vsnMethod_streamLines*>(pm));

  wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer* sizerH;

  // sampler selector
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND|wxALL);
  sizerH->Add(new wxStaticText(this, -1, wxT("selected")),
                     0, wxALIGN_LEFT|wxALL, 3);
  m_pSamplerTxt = new wxTextCtrl(this, MPP_streamLines_SamplerTxt,
                                 wxT(""), wxDefaultPosition, wxDefaultSize,
                                 wxTE_READONLY|wxTE_PROCESS_ENTER);
  sizerH->Add(m_pSamplerTxt, 0, wxEXPAND|wxALL, 3);

  m_pSamplerLst = new wxListBox(this, MPP_streamLines_SamplerLst);
  topsizer->Add(m_pSamplerLst, 0, wxEXPAND|wxALL, 3);

  m_pSetSamplerBtn = new wxButton(this, MPP_streamLines_SetSamplerBtn,
				  wxT("set sampler"));
  topsizer->Add(m_pSetSamplerBtn, 0, wxALIGN_LEFT|wxALL, 3);

  // run button
  m_pRunBtn = new wxButton(this, MPP_streamLines_RunBtn,
                           wxT("calculate stream lines"));
  topsizer->Add(m_pRunBtn, 0, wxEXPAND|wxALL, 5);

  // always
  m_pAlwaysChk = new wxCheckBox(this, MPP_streamLines_AlwaysChk,
                                wxT("always update"));
  topsizer->Add(m_pAlwaysChk, 0, wxALL, 3);

  // vector data selector
  topsizer->Add(new wxStaticText(this, -1, wxT("vector components")),
                0, wxALIGN_LEFT|wxALL, 3);
  m_pVecDataChkLst = new wxCheckListBox(this, MPP_streamLines_VecDataChkLst);
  topsizer->Add(m_pVecDataChkLst, 0, wxEXPAND|wxALL, 3);

  // colored
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND);
  m_pColoredChk = new wxCheckBox(this, MPP_streamLines_ColoredChk,
                                wxT("colored line"));
  sizerH->Add(m_pColoredChk, 0, wxALL, 3);

  // anti-alias
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND);
  m_pAntiAliasChk = new wxCheckBox(this, MPP_streamLines_AntiAliasChk,
                                   wxT("anti-alias line"));
  sizerH->Add(m_pAntiAliasChk, 0, wxALL, 3);

  // lineWidth
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND);
  sizerH->Add(new wxStaticText(this, -1, wxT("line width")),
              1, wxALIGN_LEFT|wxEXPAND|wxALL, 3);
  m_pLineWidthTxt = new wxTextCtrl(this, MPP_streamLines_LineWidthTxt,
                                   wxT(""), wxDefaultPosition, wxDefaultSize,
                                   wxTE_PROCESS_ENTER);
  sizerH->Add(m_pLineWidthTxt, 0, wxALIGN_RIGHT|wxALL, 3);

  // divTime
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND);
  sizerH->Add(new wxStaticText(this, -1, wxT("div time")),
              1, wxALIGN_LEFT|wxEXPAND|wxALL, 3);
  m_pDivTimeTxt = new wxTextCtrl(this, MPP_streamLines_DivTimeTxt,
                                 wxT(""), wxDefaultPosition, wxDefaultSize,
                                 wxTE_PROCESS_ENTER);
  sizerH->Add(m_pDivTimeTxt, 0, wxALIGN_RIGHT|wxALL, 3);

  // numSkip
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND);
  sizerH->Add(new wxStaticText(this, -1, wxT("skip step")),
              1, wxALIGN_LEFT|wxEXPAND|wxALL, 3);
  m_pSkipOutTxt = new wxTextCtrl(this, MPP_streamLines_SkipOutTxt,
                                 wxT(""), wxDefaultPosition, wxDefaultSize,
                                 wxTE_PROCESS_ENTER);
  sizerH->Add(m_pSkipOutTxt, 0, wxALIGN_RIGHT|wxALL, 3);

  // maxPts
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND);
  sizerH->Add(new wxStaticText(this, -1, wxT("max points")),
              1, wxALIGN_LEFT|wxEXPAND|wxALL, 3);
  m_pMaxPtsTxt = new wxTextCtrl(this, MPP_streamLines_MaxPtsTxt,
                                wxT(""), wxDefaultPosition, wxDefaultSize,
                                wxTE_PROCESS_ENTER);
  sizerH->Add(m_pMaxPtsTxt, 0, wxALIGN_RIGHT|wxALL, 3);

  // export button
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND|wxALL, 0);
  m_pExportBtn = new wxButton(this, MPP_streamLines_ExportBtn, wxT("export"));
  sizerH->Add(m_pExportBtn, 0, wxALL, 3);
  m_pAutoExportChk = new wxCheckBox(this, MPP_streamLines_AutoExportChk,
				    wxT("auto export"));
  sizerH->Add(m_pAutoExportChk, 0, wxALL, 3);

  // post process
  SetSizer(topsizer);
  addTo(parent);
  topsizer->Layout();
  (void)update();
}

vsnMPP_streamLines::~vsnMPP_streamLines() {
}


/* interface */

bool vsnMPP_streamLines::update() {
  if ( ! m_pSamplerTxt || ! m_pSamplerLst || ! m_pSetSamplerBtn ||
       ! m_pVecDataChkLst || ! m_pDivTimeTxt || ! m_pSkipOutTxt ||
       ! m_pMaxPtsTxt || ! m_pAlwaysChk || ! m_pRunBtn ||
       ! m_pLineWidthTxt || ! m_pColoredChk || ! m_pAntiAliasChk )
    return false;

  vsnMethod_streamLines* pm
    = dynamic_cast<vsnMethod_streamLines*>(p_method);
  if ( ! pm ) return false;
  vsnNumericalDataIF* pdo
    = dynamic_cast<vsnNumericalDataIF*>(pm->getRefData());
  if ( ! pdo ) return false;
  char txt[64];

  // selected sampler
  vsnMethod_sampler* splr = pm->getSampler();
  if ( splr )
    m_pSamplerTxt->SetValue(vsnApp::ConvSysToWx(splr->getName()));
  else
    m_pSamplerTxt->SetValue(wxT("none"));

  // sampler list
  m_pSamplerLst->Clear();
  deque<vsnMethod_sampler*> samplerMtdLst
    = pm->getSamplerList(pm->getRefData());
  deque<vsnMethod_sampler*>::iterator it;
  for ( it = samplerMtdLst.begin(); it != samplerMtdLst.end(); it++ ) {
    if ( ! *it ) continue;
    string samplerName = (*it)->getName();
    if ( samplerName.empty() )
      m_pSamplerLst->Append(wxT(VFR_NONAME));
    else
      m_pSamplerLst->Append(vsnApp::ConvSysToWx(samplerName));
  } // end of for(it)

  // vector data indices
  int dlen = (int)pdo->getDataLen();
  m_pVecDataChkLst->Clear();
  if ( dlen >= 3 ) {
    register int i;
    for ( i = 0; i < dlen; i++ ) {
      sprintf(txt, "data%d", i);
      m_pVecDataChkLst->Append(vsnApp::ConvSysToWx(txt));
    } // end of for(i)

    Vec3<int> vidx = pm->getVecDataIdx();
    if ( vidx[0] >= 0 && vidx[0] < dlen ) m_pVecDataChkLst->Check(vidx[0]);
    if ( vidx[1] >= 0 && vidx[1] < dlen ) m_pVecDataChkLst->Check(vidx[1]);
    if ( vidx[2] >= 0 && vidx[2] < dlen ) m_pVecDataChkLst->Check(vidx[2]);
  }

  // parameters
  sprintf(txt, "%d", pm->getDivTime());
  m_pDivTimeTxt->SetValue(vsnApp::ConvSysToWx(txt));
  sprintf(txt, "%d", pm->getNumSkip());
  m_pSkipOutTxt->SetValue(vsnApp::ConvSysToWx(txt));
  sprintf(txt, "%d", pm->getMaxPts());
  m_pMaxPtsTxt->SetValue(vsnApp::ConvSysToWx(txt));
  sprintf(txt, "%g", pm->getLineWidth());
  m_pLineWidthTxt->SetValue(vsnApp::ConvSysToWx(txt));
  m_pColoredChk->SetValue(pm->getColored());
  m_pAntiAliasChk->SetValue(pm->getAntiAliasMode());
  m_pAlwaysChk->SetValue(pm->getAlways());

  // auto export
  m_pAutoExportChk->SetValue(pm->getAutoExport());

  return true;
}


/* event handler */

void vsnMPP_streamLines::OnEnterSamplerTxt(wxCommandEvent& event) {
  if ( ! m_pSamplerTxt ) return;

  vsnMethod_streamLines* pm = dynamic_cast<vsnMethod_streamLines*>(p_method);
  if ( ! pm ) return;
  vsnMethod_sampler* psplr = pm->getSampler();
  if ( ! psplr ) return;
  vsnUiView* puiv = getUiView();
  if ( ! puiv ) return;

  puiv->selectObj((vfrNode*)psplr);
}

void vsnMPP_streamLines::OnSetSamplerBtn(wxCommandEvent& event) {
  if ( ! m_pSamplerLst || ! m_pSetSamplerBtn ) return;

  int sel = m_pSamplerLst->GetSelection();
  if ( sel < 0 ) return;

  if ( m_pSamplerTxt )
    m_pSamplerTxt->SetValue(m_pSamplerLst->GetString(sel));

  vsnMethod_streamLines* pm = dynamic_cast<vsnMethod_streamLines*>(p_method);
  if ( ! pm ) return;
  vsnDataObj* pdo = dynamic_cast<vsnDataObj*>(pm->getRefData());
  if ( ! pdo ) return;

  pm->vsnRef_sampler::setSampler(pdo, sel);
  pm->chkNotice();
}

void vsnMPP_streamLines::OnVecDataChkLst(wxCommandEvent& event) {
  if ( ! m_pVecDataChkLst ) return;
  int dlen = m_pVecDataChkLst->GetCount();
  if ( dlen < 3 ) return;

  vsnMethod_streamLines* pm = dynamic_cast<vsnMethod_streamLines*>(p_method);
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

void vsnMPP_streamLines::OnDivTimeTxt(wxCommandEvent& event) {
  if ( ! m_pDivTimeTxt ) return;

  vsnMethod_streamLines* pm = dynamic_cast<vsnMethod_streamLines*>(p_method);
  if ( ! pm ) return;

  wxString valStr = m_pDivTimeTxt->GetValue();
  if ( valStr.IsEmpty() ) return;
  int val = atoi(vsnApp::ConvWxToSys(valStr).c_str());
  if ( val == pm->getDivTime() ) return;

  if ( ! pm->setDivTime(val) ) {
    char txt[64]; sprintf(txt, "%d", pm->getDivTime());
    m_pDivTimeTxt->SetValue(vsnApp::ConvSysToWx(txt));
  } else
    pm->chkNotice();
}

void vsnMPP_streamLines::OnSkipOutTxt(wxCommandEvent& event) {
  if ( ! m_pSkipOutTxt ) return;

  vsnMethod_streamLines* pm = dynamic_cast<vsnMethod_streamLines*>(p_method);
  if ( ! pm ) return;

  wxString valStr = m_pSkipOutTxt->GetValue();
  if ( valStr.IsEmpty() ) return;
  int val = atoi(vsnApp::ConvWxToSys(valStr).c_str());
  if ( val == pm->getNumSkip() ) return;

  if ( ! pm->setNumSkip(val) ) {
    char txt[64]; sprintf(txt, "%d", pm->getNumSkip());
    m_pSkipOutTxt->SetValue(vsnApp::ConvSysToWx(txt));
  } else
    pm->chkNotice();
}

void vsnMPP_streamLines::OnMaxPtsTxt(wxCommandEvent& event) {
  if ( ! m_pMaxPtsTxt ) return;

  vsnMethod_streamLines* pm = dynamic_cast<vsnMethod_streamLines*>(p_method);
  if ( ! pm ) return;

  wxString valStr = m_pMaxPtsTxt->GetValue();
  if ( valStr.IsEmpty() ) return;
  int val = atoi(vsnApp::ConvWxToSys(valStr).c_str());
  if ( val == pm->getMaxPts() ) return;

  if ( ! pm->setMaxPts(val) ) {
    char txt[64]; sprintf(txt, "%d", pm->getMaxPts());
    m_pMaxPtsTxt->SetValue(vsnApp::ConvSysToWx(txt));
  } else
    pm->chkNotice();
}

void vsnMPP_streamLines::OnLineWidthTxt(wxCommandEvent& event) {
  if ( ! m_pLineWidthTxt ) return;

  vsnMethod_streamLines* pm = dynamic_cast<vsnMethod_streamLines*>(p_method);
  if ( ! pm ) return;

  wxString valStr = m_pLineWidthTxt->GetValue();
  if ( valStr.IsEmpty() ) return;
  float fval = (float)atof(vsnApp::ConvWxToSys(valStr).c_str());
  if ( fval == pm->getLineWidth() ) return;

  if ( pm->setLineWidth(fval) )
    pm->chkNotice();
}

void vsnMPP_streamLines::OnColoredChk(wxCommandEvent& event) {
  if ( ! m_pColoredChk ) return;
  bool val = m_pColoredChk->GetValue();

  vsnMethod_streamLines* pm = dynamic_cast<vsnMethod_streamLines*>(p_method);
  if ( ! pm ) return;
  if ( val == pm->getColored() ) return;

  if ( pm->setColored(val) )
    pm->chkNotice();
}

void vsnMPP_streamLines::OnAntiAliasChk(wxCommandEvent& event) {
  if ( ! m_pAntiAliasChk ) return;
  bool val = m_pAntiAliasChk->GetValue();

  vsnMethod_streamLines* pm = dynamic_cast<vsnMethod_streamLines*>(p_method);
  if ( ! pm ) return;
  if ( val == pm->getAntiAliasMode() ) return;

  if ( pm->setAntiAliasMode(val) )
    pm->chkNotice();
}

void vsnMPP_streamLines::OnAlwaysChk(wxCommandEvent& event) {
  if ( ! m_pAlwaysChk ) return;
  bool val = m_pAlwaysChk->GetValue();

  vsnMethod_streamLines* pm = dynamic_cast<vsnMethod_streamLines*>(p_method);
  if ( ! pm ) return;
  if ( val == pm->getAlways() ) return;

  if ( pm->setAlways(val) )
    pm->chkNotice();
}

void vsnMPP_streamLines::OnRunBtn(wxCommandEvent& event) {
  vsnMethod_streamLines* pm = dynamic_cast<vsnMethod_streamLines*>(p_method);
  if ( ! pm ) return;

  if ( pm->requestCalc() )
    pm->chkNotice();
}

void vsnMPP_streamLines::OnExportBtn(wxCommandEvent& event) {
  vsnMethod_streamLines* pm = dynamic_cast<vsnMethod_streamLines*>(p_method);
  if ( ! pm ) return;
  vsnApp* pApp = vsnApp::GetApp();

  wxFileDialog fileDlg(this, wxT("streamLines: specify file to export"),
		       wxT(""), wxT(""), // default Dir / File
		       wxT("Scatter (*.scat;*.sct)|*.scat;*.sct")
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
  string outPath = vsnPath_normalize(vsnApp::ConvWxToSys(fileDlg.GetPath()));
  if ( outPath.empty() ) return;

  // override check
  FILE* ofp = fopen(outPath.c_str(), "r");
  if ( ofp ) {
    fclose(ofp);
    wxString msg = wxT("The specified file has already existed\n  ");
    msg += wxString::FromUTF8(outPath.c_str());
    msg += wxT("\n\nAre you sure to override ?\n");
    wxMessageDialog dlg(NULL, msg, wxT("streamLines: export"),
			vsn_wxOK_CANCEL|wxICON_QUESTION);
    if ( dlg.ShowModal() != vsn_wxIDOK ) return;
  }

  // export
  if ( ! pm->exportLines(outPath) ) {
    ErrMsg(MsgERR, string("streamLines: export failed.\n  File: ") + outPath);
    return;
  }

  return;
}

void vsnMPP_streamLines::OnAutoExportChk(wxCommandEvent& event) {
  if ( ! m_pAutoExportChk ) return;
  bool val = m_pAutoExportChk->GetValue();

  vsnMethod_streamLines* pm = dynamic_cast<vsnMethod_streamLines*>(p_method);
  if ( ! pm ) return;

  if ( pm->setAutoExport(val) )
    pm->chkNotice(); 
}


//----------------------------------------------------------------
// class vsnMethod_streamLines
//----------------------------------------------------------------

/* constructors / destructor */

vsnMethod_streamLines::vsnMethod_streamLines(const string& name)
  : vsnMethodObj(name), m_lines(NULL), m_always(false), m_needUpd(false),
    m_vecDataIdx(0,1,2), m_divTime(4), m_numSkip(1), m_maxPts(1000),
    m_lineWidth(1.f), m_colored(false), m_autoExport(false)
{
}

vsnMethod_streamLines::~vsnMethod_streamLines() {
}


/* methods */

bool vsnMethod_streamLines::setVecDataIdx(const Vec3<int>& vdidx) {
  if ( vdidx[0] == m_vecDataIdx[0] &&
       vdidx[1] == m_vecDataIdx[1] &&
       vdidx[2] == m_vecDataIdx[2] ) return true;
  m_vecDataIdx = vdidx;

  if ( ! update(false) ) return false;
  updateUI();
  return true;
}

bool vsnMethod_streamLines::isValidVecData() const {
  vsnNumericalDataIF* pData = dynamic_cast<vsnNumericalDataIF*>(p_refData);
  if ( ! pData ) return false;

  int dlen = (int)pData->getDataLen();
  if ( m_vecDataIdx[0] >= dlen || m_vecDataIdx[1] >= dlen ||
       m_vecDataIdx[2] >= dlen ) return false;
  if ( m_vecDataIdx[0] < 0 && m_vecDataIdx[1] < 0 && m_vecDataIdx[2] < 0 )
    return false;
  return true;
}

bool vsnMethod_streamLines::setDivTime(const int divt) {
  if ( divt == m_divTime ) return true;
  if ( divt < 1 ) return false;
  m_divTime = divt;
  if ( m_divTime < m_numSkip )
    return setNumSkip(m_divTime);

  if ( ! update(false) ) return false;
  updateUI();
  return true;
}

bool vsnMethod_streamLines::setNumSkip(const int ns) {
  if ( ns == m_numSkip ) return true;
  if ( ns < 1 || ns > m_divTime ) return false;
  m_numSkip = ns;

  if ( ! update(false) ) return false;
  updateUI();
  return true;
}

bool vsnMethod_streamLines::setMaxPts(const int mpt) {
  if ( mpt == m_maxPts ) return true;
  if ( mpt < 1 ) return false;
  m_maxPts = mpt;

  if ( ! update(false) ) return false;
  updateUI();
  return true;
}

bool vsnMethod_streamLines::setLineWidth(const float lw) {
  if ( m_lineWidth == lw ) return true;
  if ( lw <= 0.f ) return false;
  m_lineWidth = lw;

  if ( m_lines )
    m_lines->getPrivateMaterial()->setLineWidth(m_lineWidth);

  updateUI();
  return true;
}

bool vsnMethod_streamLines::setColored(const bool cm) {
  if ( cm == m_colored ) return true;
  m_colored = cm;

  if ( ! update(false) ) return false;
  updateUI();
  return true;
}

bool vsnMethod_streamLines::setAlways(const bool am) {
  if ( am == m_always ) return true;
  m_always = am;

  if ( ! update(false) ) return false;
  updateUI();
  return true;
}

bool vsnMethod_streamLines::exportLines(const std::string& path) {
  if ( path.empty() ) return false;
  if ( ! m_lines ) return false;

  string exPath = VSN::ConvTimeStepPath(path, p_refData);
  if ( ! VSN::SaveScatAsLines(m_lines, exPath) )
    return false;

  m_exportPath = path;
  return true;
}

bool vsnMethod_streamLines::setAutoExport(const bool aem) {
  if ( m_autoExport == aem ) return true;
  m_autoExport = aem;
  if ( m_autoExport ) {
    if ( m_lines ) {
      if ( ! updateStep(m_updatedStp, true) )
	return false;
    }
  }
  updateUI();
  return true;
}


/* from vsnRef_sampler */

bool vsnMethod_streamLines::setSampler(const string& samplerName) {
  if ( samplerName.empty() || samplerName == string(VFR_NONAME) )
    return false;
  if ( ! p_refData ) return false;

  deque<vsnMethod_sampler*> splrLst = getSamplerList(p_refData);
  for ( int i = 0; i < splrLst.size(); i++ ) {
    if ( ! splrLst[i] ) continue;
    if ( samplerName == splrLst[i]->getName() ) {
      this->vsnRef_sampler::setSampler(splrLst[i]);
      return (p_splr != NULL);
    }
  } // end of for(i)
  return false;
}


/* from vsnMethodObj */

bool vsnMethod_streamLines::update(const bool force) {
  if ( m_always ) m_needUpd = true;
  if ( ! m_needUpd ) return true;

  return updateStep(m_requestedStp, force);
}

vsnMethodPP* vsnMethod_streamLines::getParamPanel(wxPanel* pp) {
  vsnMPP_streamLines* pp_streamLines = new vsnMPP_streamLines(pp, this);
  if ( ! pp_streamLines ) return NULL;
  return pp_streamLines;
}


/* from vsnIoObject */

bool vsnMethod_streamLines::parseXML(xmlNodePtr xnp) {
  static xmlChar* xs;
  xmlNodePtr cur;
  bool always = false;
  int dtn = -1;
  int ns = -1;

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
        if ( ! setSampler(string(xsV)) ) {
          ErrMsg(MsgERR, msgHdr + string("failed to set sampler: ") + xsV);
          goto _NEXT_XML_NODE;
        }
      } // end of "sampler"
      else if ( xsN == string("vec_idx") ) {
        Vec3<int> idcs(-1, -1, -1);
        istringstream iss(xsV);
        iss >> idcs;
        if ( idcs[0] < 0 && idcs[1] < 0 &&  idcs[2] < 0 ) {
          ErrMsg(MsgERR, msgHdr + string("bad value format ") +xsV);
          goto _NEXT_XML_NODE;
        }
        if ( ! setVecDataIdx(idcs) ) {
          ErrMsg(MsgERR, msgHdr + string("failed to set ") +xsV);
          goto _NEXT_XML_NODE;
        }
      } // end of "vec_idx"
      else if ( xsN == string("div_time") ) {
        dtn = atoi(xsV.c_str());
      } // end of "div_time"
      else if ( xsN == string("num_skip") ) {
        ns = atoi(xsV.c_str());
      } // end of "num_skip"
      else if ( xsN == string("max_pts") ) {
        int mpt = atoi(xsV.c_str());
        if ( ! setMaxPts(mpt) ) {
          ErrMsg(MsgERR, msgHdr + string("invalid value in param max_pts"));
          goto _NEXT_XML_NODE;
        }
      } // end of "max_pts"
      else if ( xsN == string("line_width") ) {
        float lw = (float)atof(xsV.c_str());
        if ( lw != m_lineWidth && ! setLineWidth(lw) ) {
          ErrMsg(MsgERR, msgHdr + string("invalid value in param line_width"));
          goto _NEXT_XML_NODE;
        }
      } // end of "line_width"
      else if ( xsN == string("colored") || xsN == string("colord") ) {
        bool cm;
        if ( xsV == string("yes") ) cm = true;
        else if ( xsV == string("no") ) cm = false;
        else {
          ErrMsg(MsgERR, msgHdr + string("invalid value in param colored"));
          goto _NEXT_XML_NODE;
        }
        if ( ! setColored(cm) ) {
          ErrMsg(MsgERR, msgHdr + string("can't set param colored"));
          goto _NEXT_XML_NODE;
        }
      } // end of "colored"
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
      else if ( xsN == string("always_update") ) {
        if ( xsV == string("yes") ) always = true;
        else if ( xsV == string("no") ) always = false;
        else {
          ErrMsg(MsgERR, msgHdr
                 + string("invalid value in param always_update"));
          goto _NEXT_XML_NODE;
        }
      } // end of "always_update"
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

  // set params : need to set divTime before numSkip ....
  if ( dtn != -1 ) {
    if ( ! setDivTime(dtn) ) {
      ErrMsg(MsgERR, msgHdr + string("invalid value in param div_time"));
    }
  }
  if ( ns != -1 ) {
    if ( ! setNumSkip(ns) ) {
      ErrMsg(MsgERR, msgHdr + string("invalid value in param num_skip"));
    }
  }

  // do calculate if always
  if ( always && ! setAlways(always) ) {
    ErrMsg(MsgERR, msgHdr + string("can't set param always_update"));
    return false;
  }

  return true;
}

bool vsnMethod_streamLines::outputXML(std::ostream& os, const size_t ts) {
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

  // vec_idx
  if ( m_vecDataIdx[0] != 0 || m_vecDataIdx[1] != 1 || m_vecDataIdx[2] != 2 ) {
    os << idts_2 << "<param name=\"vec_idx\" value=\"";
    os << m_vecDataIdx[0] << "/" << m_vecDataIdx[1] << "/" << m_vecDataIdx[2];
    os << "\" />" << endl;
  }

  // div_time
  if ( m_divTime != 4 ) {
    os << idts_2 << "<param name=\"div_time\" value=\""
       << m_divTime << "\" />" << endl;
  }

  // num_skip
  if ( m_numSkip != 1 ) {
    os << idts_2 << "<param name=\"num_skip\" value=\""
       << m_numSkip << "\" />" << endl;
  }

  // max_pts
  if ( m_maxPts != 1000 ) {
    os << idts_2 << "<param name=\"max_pts\" value=\""
       << m_maxPts << "\" />" << endl;
  }

  // line_width
  if ( m_lineWidth != 1.f ) {
    os << idts_2 << "<param name=\"line_width\" value=\""
       << m_lineWidth << "\" />" << endl;
  }

  // colored
  if ( m_colored ) {
    os << idts_2 << "<param name=\"colored\" value=\"yes\" />" << endl;
  }

  // antialias
  if ( m_antiAlias ) {
    os << idts_2 << "<param name=\"antialias\" value=\"yes\" />" << endl;
  }

  // always_update
  if ( m_always ) {
    os << idts_2 << "<param name=\"always_update\" value=\"yes\" />" << endl;
  }

  // auto export
  if ( m_autoExport ) {
    os << idts_2 << "<param name=\"auto_export\" value=\"yes\" />" << endl;
  }

  os << idts << "</method>" << endl;

  // calculate command if need
  if ( !_name.empty() && _name != string(VFR_NONAME) &&
       m_lines && m_lines->getPrivateMaterial()->getRenderMode() != RT_NONE ) {
    string extOutCmd = string("<command target=\"") + _name
      + string("\" name=\"calculate\" />\n");
    vsnApp::GetApp()->registExtOutStr(extOutCmd);
  }

  return ret;
}

bool vsnMethod_streamLines::commandXML(xmlNodePtr xnp) {
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
  if ( nameStr == "calculate" ) {
    if ( ! requestCalc() ) {
      ErrMsg(MsgERR, msgHdr + string("command calculate: update failed"));
      return false;
    }
    return true;
  } // end of "calculate"

  if ( valueStr.empty() ) {
    ErrMsg(MsgERR, msgHdr + "invalid command: " + nameStr + ": no value");
    return false;
  }

  if ( nameStr == "set_sampler" ) {
    if ( valueStr == VFR_NONAME ) {
      ErrMsg(MsgERR, msgHdr + "command " + nameStr +
             ": can't set sampler with no name");
      return false;
    }
    if ( ! setSampler(valueStr) ) {
      ErrMsg(MsgERR, msgHdr +
             string("command set_sampler: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_sampler"
  else if ( nameStr == string("set_vec_idx") ) {
    Vec3<int> idcs(-1, -1, -1);
    istringstream iss(valueStr);
    iss >> idcs;
    if ( idcs[0] < 0 && idcs[1] < 0 &&  idcs[2] < 0 ) {
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
  else if ( nameStr == "set_div_time" ) {
    int dtn = atoi(valueStr.c_str());
    if ( ! setDivTime(dtn) ) {
      ErrMsg(MsgERR, msgHdr +
             string("command set_div_time: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_div_time"
  else if ( nameStr == "set_num_skip" ) {
    int ns = atoi(valueStr.c_str());
    if ( ! setNumSkip(ns) ) {
      ErrMsg(MsgERR, msgHdr +
             string("command set_num_skip: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_num_skip"
  else if ( nameStr == "set_max_pts" ) {
    int mpt = atoi(valueStr.c_str());
    if ( ! setMaxPts(mpt) ) {
      ErrMsg(MsgERR, msgHdr +
             string("command set_max_pts: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_max_pts"
  else if ( nameStr == "set_line_width" ) {
    float lw = (float)atof(valueStr.c_str());
    if ( ! setLineWidth(lw) ) {
      ErrMsg(MsgERR, msgHdr +
             string("command set_line_width: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_line_width"
  else if ( nameStr == "set_colored" || nameStr == "set_colord" ) {
    bool cm;
    if ( valueStr == string("yes") ) cm = true;
    else if ( valueStr == string("no") ) cm = false;
    else {
      ErrMsg(MsgERR, msgHdr +
             string("invalid command: set_colored: invalid value"));
      return false;
    }
    if ( ! setColored(cm) ) {
      ErrMsg(MsgERR, msgHdr +
             string("command set_colored: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_colored"
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
  else if ( nameStr == "set_always_update" ) {
    bool am;
    if ( valueStr == string("yes") ) am = true;
    else if ( valueStr == string("no") ) am = false;
    else {
      ErrMsg(MsgERR, msgHdr +
             string("invalid command: set_always_update: invalid value"));
      return false;
    }
    if ( ! setAlways(am) ) {
      ErrMsg(MsgERR, msgHdr +
             string("command set_always_update: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_always_update"
  else if ( nameStr == "export" ) {
    if ( ! exportLines(valueStr) ) {
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


/* from vsnRef_sampler */

void vsnMethod_streamLines::noticeUpdate() {
  update();
}
