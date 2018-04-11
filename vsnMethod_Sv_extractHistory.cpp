//
// vsnMethod_Sv_extractHistory
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
#include "wx/filename.h"

#include "vsnMethod_Sv_extractHistory.h"
#include "vsnError.h"
#include "vsnData_DfiSv.h"
#include <iomanip>
#include <fstream>

using namespace std;
using namespace CES;
using namespace VSN;


//----------------------------------------------------------------
// class vsnMPP_Sv_extractHistory
//----------------------------------------------------------------

BEGIN_EVENT_TABLE(vsnMPP_Sv_extractHistory, wxPanel)
  EVT_TEXT_ENTER(MPP_Sv_extractHistory_SamplerTxt,
                 vsnMPP_Sv_extractHistory::OnEnterSamplerTxt)
  EVT_BUTTON(MPP_Sv_extractHistory_SetSamplerBtn,
             vsnMPP_Sv_extractHistory::OnSetSamplerBtn)
  EVT_TEXT_ENTER(MPP_Sv_extractHistory_OutfileTxt,
                 vsnMPP_Sv_extractHistory::OnOutfileTxt)
  EVT_BUTTON(MPP_Sv_extractHistory_OutfileBrwsBtn,
             vsnMPP_Sv_extractHistory::OnOutfileBrwsBtn)
  EVT_CHECKBOX(MPP_Sv_extractHistory_AlwaysOutputChk,
               vsnMPP_Sv_extractHistory::OnAlwaysOutputChk)
  EVT_BUTTON(MPP_Sv_extractHistory_ExtractBtn,
             vsnMPP_Sv_extractHistory::OnExtractBtn)
  EVT_TEXT_ENTER(MPP_Sv_extractHistory_ExtractorTxt,
                 vsnMPP_Sv_extractHistory::OnExtractorTxt)
  EVT_BUTTON(MPP_Sv_extractHistory_ExtractorBrwsBtn,
             vsnMPP_Sv_extractHistory::OnExtractorBrwsBtn)
  EVT_TEXT_ENTER(MPP_Sv_extractHistory_DummyValTxt,
                 vsnMPP_Sv_extractHistory::OnDummyValTxt)
END_EVENT_TABLE()

/* constructors / destructor */

vsnMPP_Sv_extractHistory::vsnMPP_Sv_extractHistory(wxPanel* parent,
						   vsnMethodObj* pm)
  : vsnMethodPP(parent, pm)
{
  assert(parent);
  assert(dynamic_cast<vsnMethod_Sv_extractHistory*>(pm));

  wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer* sizerH;

  // sampler selector
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND|wxALL);
  sizerH->Add(new wxStaticText(this, -1, wxT("selected")),
                     0, wxALIGN_LEFT|wxALL, 3);
  m_pSamplerTxt = new wxTextCtrl(this, MPP_Sv_extractHistory_SamplerTxt,
                                 wxT(""), wxDefaultPosition, wxDefaultSize,
                                 wxTE_READONLY|wxTE_PROCESS_ENTER);
  sizerH->Add(m_pSamplerTxt, 0, wxEXPAND|wxALL, 3);

  m_pSamplerLst = new wxListBox(this, MPP_Sv_extractHistory_SamplerLst);
  topsizer->Add(m_pSamplerLst, 0, wxEXPAND|wxALL, 3);

  m_pSetSamplerBtn = new wxButton(this, MPP_Sv_extractHistory_SetSamplerBtn,
                                  wxT("set sampler"));
  topsizer->Add(m_pSetSamplerBtn, 0, wxALIGN_LEFT|wxALL, 3);

  // outfile
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND|wxALL, 3);
  sizerH->Add(new wxStaticText(this, -1, wxT("outfile")),
              0, wxALIGN_LEFT|wxALL, 3);
  m_pOutfileTxt = new wxTextCtrl(this, MPP_Sv_extractHistory_OutfileTxt,
				 wxT(""), wxDefaultPosition, wxSize(60,-1),
                                 wxTE_PROCESS_ENTER);
  sizerH->Add(m_pOutfileTxt, 1, wxEXPAND|wxALL, 3);
  m_pOutfileBrwsBtn = new wxButton(this, MPP_Sv_extractHistory_OutfileBrwsBtn,
				   wxT("..."), wxDefaultPosition,
				   wxDefaultSize, wxBU_EXACTFIT);
  sizerH->Add(m_pOutfileBrwsBtn, 0, wxALIGN_RIGHT|wxALL, 3);

  // extract
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND|wxALL, 3);
  m_pExtractBtn = new wxButton(this, MPP_Sv_extractHistory_ExtractBtn,
			       wxT("extract"), wxDefaultPosition,
			       wxDefaultSize);
  sizerH->Add(m_pExtractBtn, 0, wxEXPAND|wxALL, 3);
  m_pAlwaysOutputChk = new wxCheckBox(this,
				      MPP_Sv_extractHistory_AlwaysOutputChk,
				      wxT("always output"));
  sizerH->Add(m_pAlwaysOutputChk, 0, wxEXPAND|wxALL, 3);

  // extractor
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND|wxALL, 3);
  sizerH->Add(new wxStaticText(this, -1, wxT("extractor path")),
              0, wxALIGN_LEFT|wxALL, 3);
  m_pExtractorTxt = new wxTextCtrl(this, MPP_Sv_extractHistory_ExtractorTxt,
				   wxT(""), wxDefaultPosition, wxSize(60,-1),
				   wxTE_PROCESS_ENTER);
  sizerH->Add(m_pExtractorTxt, 1, wxEXPAND|wxALL, 3);
  m_pExtractorBrwsBtn = new wxButton(this,
				     MPP_Sv_extractHistory_ExtractorBrwsBtn,
				     wxT("..."), wxDefaultPosition,
				     wxDefaultSize, wxBU_EXACTFIT);
  sizerH->Add(m_pExtractorBrwsBtn, 0, wxALIGN_RIGHT|wxALL, 3);  

  // dummy value
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND|wxALL, 3);
  sizerH->Add(new wxStaticText(this, -1, wxT("dummy value")),
              0, wxALIGN_LEFT|wxALL, 3);
  m_pDummyValTxt = new wxTextCtrl(this, MPP_Sv_extractHistory_DummyValTxt,
				 wxT(""), wxDefaultPosition, wxDefaultSize,
                                 wxTE_PROCESS_ENTER);
  sizerH->Add(m_pDummyValTxt, 1, wxEXPAND|wxALL, 3);

  // post process
  SetSizer(topsizer);
  addTo(parent);
  topsizer->Layout();
  (void)update();
}

vsnMPP_Sv_extractHistory::~vsnMPP_Sv_extractHistory() {
}


/* interface */

bool vsnMPP_Sv_extractHistory::update() {
  if ( ! m_pSamplerTxt || ! m_pSamplerLst ||
       ! m_pOutfileTxt || ! m_pExtractorTxt || ! m_pAlwaysOutputChk )
    return false;

  vsnMethod_Sv_extractHistory* pm
    = dynamic_cast<vsnMethod_Sv_extractHistory*>(p_method);
  if ( ! pm ) return false;
  vsnDataObj* pdo = pm->getRefData();
  if ( ! pdo ) return false;

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

  // outfile
  m_pOutfileTxt->SetValue(vsnApp::ConvSysToWx(pm->getOutfile()));

  // extractor
  m_pExtractorTxt->SetValue(vsnApp::ConvSysToWx(pm->getExtractorPath()));

  // dummy value
  char txt[64];
  sprintf(txt, "%g", pm->getDummyVal());
  m_pDummyValTxt->SetValue(vsnApp::ConvSysToWx(string(txt)));

  // always output check
  m_pAlwaysOutputChk->SetValue(pm->getAlways());

  return true;
}


/* event handler */

void vsnMPP_Sv_extractHistory::OnEnterSamplerTxt(wxCommandEvent& event) {
  if ( ! m_pSamplerTxt ) return;

  vsnMethod_Sv_extractHistory* pm
    = dynamic_cast<vsnMethod_Sv_extractHistory*>(p_method);
  if ( ! pm ) return;
  vsnMethod_sampler* psplr = pm->getSampler();
  if ( ! psplr ) return;
  vsnUiView* puiv = getUiView();
  if ( ! puiv ) return;

  puiv->selectObj((vfrNode*)psplr);
}

void vsnMPP_Sv_extractHistory::OnSetSamplerBtn(wxCommandEvent& event) {
  if ( ! m_pSamplerLst || ! m_pSamplerTxt || ! m_pSetSamplerBtn )
    return;

  int sel = m_pSamplerLst->GetSelection();
  if ( sel < 0 ) return;
  m_pSamplerTxt->SetValue(m_pSamplerLst->GetString(sel));

  vsnMethod_Sv_extractHistory* pm
    = dynamic_cast<vsnMethod_Sv_extractHistory*>(p_method);
  if ( ! pm ) return;
  vsnDataObj* pdo = pm->getRefData();
  if ( ! pdo ) return;

  pm->vsnRef_sampler::setSampler(pdo, sel);
}

void vsnMPP_Sv_extractHistory::OnOutfileTxt(wxCommandEvent& event) {
  if ( ! m_pOutfileTxt ) return;

  vsnMethod_Sv_extractHistory* pm
    = dynamic_cast<vsnMethod_Sv_extractHistory*>(p_method);
  if ( ! pm ) return;

  wxString valStr = m_pOutfileTxt->GetValue();
  string ofn = vsnApp::ConvWxToSys(valStr);
  if ( ! pm->setOutfile(ofn) ) {
    update();
    return;
  }
}

void vsnMPP_Sv_extractHistory::OnOutfileBrwsBtn(wxCommandEvent& event) {
  if ( ! m_pOutfileBrwsBtn || ! m_pOutfileTxt ) return;

  vsnMethod_Sv_extractHistory* pm
    = dynamic_cast<vsnMethod_Sv_extractHistory*>(p_method);
  if ( ! pm ) return;

  wxFileDialog fileDlg(this, wxT("select output file"),
                       wxT(""), wxT(""), // default Dir / File
                       wxT("text file (*.txt)|*.txt|(*)|*"), wxFD_SAVE);

  wxString valStr = m_pOutfileTxt->GetValue();
  if ( ! valStr.IsEmpty() )
    fileDlg.SetPath(valStr);
  else
    fileDlg.SetDirectory(vsnApp::ConvSysToWx(vsnApp::GetApp()->getCwd()));

  if ( fileDlg.ShowModal() != wxID_OK ) return;
  string ofn = vsnApp::ConvWxToSys(fileDlg.GetPath());
  if ( ! pm->setOutfile(ofn) ) return;

  update(); // update Outfile
}

void vsnMPP_Sv_extractHistory::OnExtractBtn(wxCommandEvent& event) {
  vsnMethod_Sv_extractHistory* pm
    = dynamic_cast<vsnMethod_Sv_extractHistory*>(p_method);
  if ( ! pm ) return;

  // get sampler
  if ( pm->getSampler() == NULL ) {
    ErrMsg(MsgERR, string("extractHistory: extract: sampler not set."));
    return;
  }

  // get outfile
  string outf = pm->getOutfile();
  if ( outf.empty() ) {
    ErrMsg(MsgERR, string("extractHistory: extract: no outfile specified."));
    return;
  }

  // override check (if not always)
  if ( ! pm->getAlways() ) {
    FILE* ofp = fopen(outf.c_str(), "r");
    if ( ofp ) {
      fclose(ofp);
      string msg = "The specified outfile has already existed\n  ";
      msg += outf;
      msg += "\n\nAre you sure to override ?\n";
      wxMessageDialog dlg(NULL, vsnApp::ConvSysToWx(msg), wxT("extractHistory"),
			  vsn_wxOK_CANCEL|wxICON_QUESTION);
      if ( dlg.ShowModal() != vsn_wxIDOK ) return;
    }
  }

  // sampling execute
  if ( ! pm->extract(outf) ) {
    ErrMsg(MsgERR, string("extractHistory: extract: extraction failed."));
    return;
  }
}

void vsnMPP_Sv_extractHistory::OnAlwaysOutputChk(wxCommandEvent& event) {
  if ( ! m_pAlwaysOutputChk ) return;

  vsnMethod_Sv_extractHistory* pm
    = dynamic_cast<vsnMethod_Sv_extractHistory*>(p_method);
  if ( ! pm ) return;

  bool val = m_pAlwaysOutputChk->GetValue();
  if ( ! pm->setAlways(val) )
    update();
}

void vsnMPP_Sv_extractHistory::OnExtractorTxt(wxCommandEvent& event) {
  if ( m_pExtractorTxt ) return;

  vsnMethod_Sv_extractHistory* pm
    = dynamic_cast<vsnMethod_Sv_extractHistory*>(p_method);
  if ( ! pm ) return;

  wxString valStr = m_pExtractorTxt->GetValue();
  if ( valStr.IsEmpty() ) {
    update();
    return;
  }

  if ( ! pm->setExtractorPath(vsnApp::ConvWxToSys(valStr)) ) {
    update();
    return;
  }
}

void vsnMPP_Sv_extractHistory::OnExtractorBrwsBtn(wxCommandEvent& event) {
  if ( ! m_pExtractorBrwsBtn || ! m_pExtractorTxt ) return;

  vsnMethod_Sv_extractHistory* pm
    = dynamic_cast<vsnMethod_Sv_extractHistory*>(p_method);
  if ( ! pm ) return;

  wxFileDialog fileDlg(this, wxT("select extractor program"),
                       wxT(""), wxT(""), // default Dir / File
                       wxT("(*)|*"), wxFD_OPEN);

  wxString valStr = m_pExtractorTxt->GetValue();
  if ( ! valStr.IsEmpty() )
    fileDlg.SetPath(valStr);
  else
    fileDlg.SetDirectory(vsnApp::ConvSysToWx(vsnApp::GetApp()->getCwd()));

  if ( fileDlg.ShowModal() != wxID_OK ) return;
  if ( ! pm->setExtractorPath(vsnApp::ConvWxToSys(fileDlg.GetPath())) )
    return;

  update(); // update Outfile
}

void vsnMPP_Sv_extractHistory::OnDummyValTxt(wxCommandEvent& event) {
  if ( ! m_pDummyValTxt ) return;

  vsnMethod_Sv_extractHistory* pm
    = dynamic_cast<vsnMethod_Sv_extractHistory*>(p_method);
  if ( ! pm ) return;

  wxString valStr = m_pDummyValTxt->GetValue();
  if ( valStr.IsEmpty() ) {
    update();
    return;
  }
  float ob = atof(vsnApp::ConvWxToSys(valStr).c_str());
  if ( ! pm->setDummyVal(ob) ) {
    update();
    return;
  }
}


//----------------------------------------------------------------
// class vsnMethod_Sv_extractHistory
//----------------------------------------------------------------
#define OB_VAL \
  -999.0

// static
string vsnMethod_Sv_extractHistory::s_extractorPath;

/* constructors / destructor */

vsnMethod_Sv_extractHistory::vsnMethod_Sv_extractHistory(const string& name)
  : vsnMethodObj(name), m_dummyVal(OB_VAL), m_always(false)
{
  (void)setupExtractor();
}

vsnMethod_Sv_extractHistory::~vsnMethod_Sv_extractHistory() {
}


/* methods */

// static
bool vsnMethod_Sv_extractHistory::setupExtractor() {
  s_extractorPath
#if defined(LINUX)
    = "/usr/local/Vtools/bin/histExtract";
#elif defined(MacOSX)
    = "/Applications/Vtools/bin/histExtract";
#elif defined(WINDOWS)
    = "/Program Files/Vtools/bin/histExtract.exe";
#else
    = "histExtract";
#endif

#if defined(WINDOWS)
  char* sysDrv = getenv("SYSTEMDRIVE");
  if ( sysDrv && s_extractorPath[0] == '/' )
    s_extractorPath = string(sysDrv) + s_extractorPath;
#endif

  char* pEnvExtr = getenv("VSN_HIST_EXTRACTOR");
  if ( pEnvExtr && strlen(pEnvExtr) > 0 )
    s_extractorPath = string(pEnvExtr);

  return true;
}


bool vsnMethod_Sv_extractHistory::setOutfile(const string& outfile) {
  if ( m_outfile == outfile ) return true;
  m_outfile = outfile;

  update();
  updateUI();
  return true;
}

bool vsnMethod_Sv_extractHistory::setDummyVal(const float ob) {
  if ( fabs(m_dummyVal - ob) < 1e-8 ) return true;
  m_dummyVal = ob;

  update();
  updateUI();
  return true;
}

bool vsnMethod_Sv_extractHistory::setAlways(const bool am) {
  if ( m_always == am ) return true;
  m_always = am;

  update();
  updateUI();
  return true;
}

bool
vsnMethod_Sv_extractHistory::setExtractorPath(const std::string& extractorPath)
{
  if ( s_extractorPath == extractorPath ) return true;
  s_extractorPath = extractorPath;

  updateUI();
  return true;
}

string vsnMethod_Sv_extractHistory::generateInputFile() const {
  if ( ! p_refData ) return string();
  string dt = p_refData->getDataType();
  if ( dt == string("DfiSv") ) {
    // use original DFI file
    vsnData_DfiSv* pdSv = dynamic_cast<vsnData_DfiSv*>(p_refData);
    if ( ! pdSv ) return string();
    vsnData_Dfi* pdfi = pdSv->getRefDataDfi();
    if ( ! pdfi ) return string();
    return pdfi->getFilePath();
  }

  //-------- Sph or P3dF --------
  vsnData_Sv* pdSv = dynamic_cast<vsnData_Sv*>(p_refData);
  if ( ! pdSv ) return string();
  Vec3<size_t> dims = pdSv->getDims();
  const Vec3<float>* dbb = pdSv->getBbox();

  // create temporary filename
  wxString tmpPath = wxFileName::CreateTempFileName(wxT("vsn_extractHist"));
  string outPath = vsnApp::ConvWxToSys(tmpPath + wxT(".txt"));

  // output txt file
  ofstream ofp(outPath.c_str());
  if ( ! ofp ) return string();
  ofp << "# V-Isio extractHistory exported data" << endl;
  ofp << "dims " << dims[0] << " " << dims[1] << " " << dims[2] << endl;
  ofp << "veclen " << pdSv->getDataLen() << endl;
  ofp << "orig " << dbb[0][0] << " " << dbb[0][1] << " " << dbb[0][2] << endl;
  Vec3<float> regn = dbb[1] - dbb[0];
  ofp << "region " << regn[0] << " " << regn[1] << " " << regn[2] << endl;
  ofp << endl;

  // time step and filename
  if ( pdSv->hasMultiFiles() ) {
    deque<string> mfl = pdSv->getFileList();
    size_t idx;
    for ( idx = 0; idx < pdSv->getNumSteps(); idx++ ) {
      int stp;
      if ( ! pdSv->getTimeStepNo(idx, stp) ) break;
      float tm = pdSv->getTime(idx);
      ofp << stp << " " << tm << " " << mfl[idx] << endl;
    } // end of for(idx)
  } else {
    ofp << 0 << " " << 0.0f << " " << pdSv->getFilePath() << endl;
  }

  return outPath;
}

bool vsnMethod_Sv_extractHistory::extract(const string& path) {
  if ( ! p_refData ) return false;
  if ( ! p_splr ) return false;
  Point2 smplSize = p_splr->getSampleNumber();
  size_t smplSz = smplSize.x * smplSize.y;
  if ( smplSz < 1 ) return false;
  const vector3* const smplPts = p_splr->getSamplePoints();
  if ( ! smplPts ) return false;

  string infn = generateInputFile();
  if ( infn.empty() ) return false;

  char buff[128];
  size_t i;

  string comm = s_extractorPath;
  comm += string(" -in ") + infn + string(" -out ") + path;
  if ( m_dummyVal != OB_VAL ) {
    sprintf(buff, "%g", m_dummyVal);
    comm += string(" -ob ") + string(buff);
  }

  for ( i = 0; i < smplSz; i++ ) {
    sprintf(buff, " %f %f %f", smplPts[i][0], smplPts[i][1], smplPts[i][2]);
    comm += string(buff);
  } // end of for(i)

  // execute
  bool retVal = true;
  int ret = system(comm.c_str());
#if defined(WINDOWS)
  if ( ret != 0  )
    retVal = false;
#else
  int xret = WEXITSTATUS(ret);
  if ( ret == -1 || xret == 127 ) {
    retVal = false;
  }
#endif

  if ( p_refData->getDataType() != string("DfiSv") ) {
    wxString infnWx = vsnApp::ConvSysToWx(infn);
    wxRemoveFile(infnWx);
  }

  return retVal;
}


// from vsnRef_sampler

bool vsnMethod_Sv_extractHistory::setSampler(const string& samplerName) {
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
 
void vsnMethod_Sv_extractHistory::noticeUpdate() {
  update();
}


// from vsnMethodObj

bool vsnMethod_Sv_extractHistory::update(const bool force) {
  if ( ! m_always ) return true;
  if ( m_outfile.empty() ) return true;
  if ( ! p_splr ) return true;

  if ( ! extract(m_outfile) )
    return false;

  return true;
}

vsnMethodPP* vsnMethod_Sv_extractHistory::getParamPanel(wxPanel* pp) {
  vsnMPP_Sv_extractHistory* pp_extractHistory
    = new vsnMPP_Sv_extractHistory(pp, this);
  if ( ! pp_extractHistory ) return NULL;
  return pp_extractHistory;
}


// from vsnIoObject

bool vsnMethod_Sv_extractHistory::parseXML(xmlNodePtr xnp) {
  static xmlChar* xs;
  xmlNodePtr cur;
  bool always = false;

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
        if ( ! setSampler(xsV) ) {
          ErrMsg(MsgERR, msgHdr + string("failed to set sampler: ") + xsV);
          goto _NEXT_XML_NODE;
        }
      } // end of "sampler"
      else if ( xsN == string("outfile") ) {
        if ( ! setOutfile(xsN) ) {
          ErrMsg(MsgERR, msgHdr + string("can't set outfile"));
          goto _NEXT_XML_NODE;
        }
      } // end of "outfile"
      else if ( xsN == string("always_output") ) {
        bool aom;
        if ( xsV == string("yes") ) aom = true;
        else if ( xsV == string("no") ) aom = false;
        else {
          ErrMsg(MsgERR,
		 msgHdr + string("invalid value in param always_output"));
          goto _NEXT_XML_NODE;
        }
	always = aom;
      } // end of "always_output"
      else if ( xsN == string("dummy_value") ) {
        float ob = (float)atof(xsV.c_str());
        if ( ob != m_dummyVal && ! setDummyVal(ob) ) {
          ErrMsg(MsgERR, msgHdr + string("invalid value in param dummy_value"));
          goto _NEXT_XML_NODE;
        }
      } // end of "dummy_value"
    } // end of param

  _NEXT_XML_NODE:
    cur = cur->next;
  } // end of while(cur)

  // set always output mode
  if ( m_always != always ) {
    if ( ! setAlways(always) ) { // may be called extract()
      ErrMsg(MsgERR, msgHdr + string("can't set always_output"));
      goto _NEXT_XML_NODE;
    }
  }

  return true;
}

bool vsnMethod_Sv_extractHistory::outputXML(ostream& os, const size_t ts) {
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

  // outfile
  if ( ! m_outfile.empty() ) {
    os << idts_2 << "<param name=\"outfile\" value=\""
       << m_outfile << "\" />" << endl;
  }

  // always_output
  if ( m_always ) {
    os << idts_2 << "<param name=\"always_output\" value=\"yes\" />" << endl;
  }

  // dummy_value
  if ( m_dummyVal != OB_VAL ) {
    os << idts_2 << "<param name=\"dummy_value\" value=\""
       << m_dummyVal << "\" />" << endl;
  }

  os << idts << "</method>" << endl;

  // output command if need

  return true;
}

bool vsnMethod_Sv_extractHistory::commandXML(xmlNodePtr xnp) {
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
  if ( nameStr == "extract" ) {
    if ( ! extract(m_outfile) ) {
      ErrMsg(MsgERR, msgHdr + string("command extract: exec failed"));
      return false;
    }
    return true;
  } // end of "extract"

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
  else if ( nameStr == "set_outfile" ) {
    if ( ! setOutfile(valueStr) ) {
      ErrMsg(MsgERR, msgHdr +
             string("command set_outfile: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_outfile"
  else if ( nameStr == "set_always_output" ) {
    bool am;
    if ( valueStr == string("yes") ) am = true;
    else if ( valueStr == string("no") ) am = false;
    else {
      ErrMsg(MsgERR, msgHdr +
             string("invalid command: set_always_output: invalid value"));
      return false;
    }
    if ( ! setAlways(am) ) {
      ErrMsg(MsgERR, msgHdr +
             string("command set_always_output: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_always_output"
  else if ( nameStr == "set_dummy_value" ) {
    float ob = (float)atof(valueStr.c_str());
    if ( ! setDummyVal(ob) ) {
      ErrMsg(MsgERR, msgHdr +
             string("command set_dummy_value: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_dummy_value"

  return true;
}
