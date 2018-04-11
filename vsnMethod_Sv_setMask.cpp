//
// vsnMethod_Sv_setMask
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

#include "vsnMethod_Sv_setMask.h"
#include "vsnOctTree.h" // for decomp probeIdx
#include "vsnApp.h"
#include "vsnError.h"

using namespace std;
using namespace CES;
using namespace VSN;


//----------------------------------------------------------------
// class vsnMPP_Sv_setMask
//----------------------------------------------------------------

BEGIN_EVENT_TABLE(vsnMPP_Sv_setMask, wxPanel)
  EVT_BUTTON(MPP_Sv_setMask_BrowsBtn, vsnMPP_Sv_setMask::OnBrowsBtn)
  EVT_BUTTON(MPP_Sv_setMask_SetMaskBtn, vsnMPP_Sv_setMask::OnSetMaskBtn)
  EVT_TEXT_ENTER(MPP_Sv_setMask_OffsetXTxt, vsnMPP_Sv_setMask::OnOffsetTxts)
  EVT_TEXT_ENTER(MPP_Sv_setMask_OffsetYTxt, vsnMPP_Sv_setMask::OnOffsetTxts)
  EVT_TEXT_ENTER(MPP_Sv_setMask_OffsetZTxt, vsnMPP_Sv_setMask::OnOffsetTxts)
  EVT_CHECKBOX(MPP_Sv_setMask_ShowMaskChk, vsnMPP_Sv_setMask::OnShowMaskChk)
END_EVENT_TABLE()


/* constructors / destructor */

vsnMPP_Sv_setMask::vsnMPP_Sv_setMask(wxPanel* parent, vsnMethodObj* pm)
  : vsnMethodPP(parent, pm),
    m_pFileTxt(NULL), m_pBrowsBtn(NULL), m_pTypeRadio(NULL),
    m_pVolRateMinTxt(NULL), m_pVolRateMaxTxt(NULL),
    m_pMediumTxt(NULL), m_pSetMaskBtn(NULL), m_pShowMaskChk(NULL),
    //m_pUpdateMinMaxChk(NULL),
    m_pOffsetXTxt(NULL), m_pOffsetYTxt(NULL), m_pOffsetZTxt(NULL)
{
  assert(parent);
  assert(dynamic_cast<vsnMethod_Sv_setMask*>(pm));

  wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer* sizerH;

  // file
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND);
  sizerH->Add(new wxStaticText(this, -1, wxT("mask file")),
              0, wxEXPAND|wxALL, 3);
  m_pFileTxt = new wxTextCtrl(this, MPP_Sv_setMask_FileTxt, wxT(""),
			      wxDefaultPosition, wxSize(60,-1));
  sizerH->Add(m_pFileTxt, 1, wxEXPAND|wxALL, 3);
  m_pBrowsBtn = new wxButton(this, MPP_Sv_setMask_BrowsBtn, wxT("..."),
			     wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
  sizerH->Add(m_pBrowsBtn, 0, wxEXPAND|wxALL, 3);

  // mask type
  wxString ritems[] = {wxString(wxT("None ")),
                       wxString(wxT("VolRate ")),
                       wxString(wxT("Medium "))};
  m_pTypeRadio = new wxRadioBox(this, MPP_Sv_setMask_TypeRadio,
				wxT("mask type"),
				wxDefaultPosition, wxDefaultSize,
				3, ritems, 1, wxRA_SPECIFY_COLS);
  topsizer->Add(m_pTypeRadio, 0, wxEXPAND|wxALL, 3);

  // VolRate
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxALL, 0);
  sizerH->Add(new wxStaticText(this, -1, wxT("VolRate")),
              0, wxALIGN_LEFT|wxALL, 3);
  m_pVolRateMinTxt = new wxTextCtrl(this, MPP_Sv_setMask_VolRateMinTxt,
				    wxT("0.0"),
				    wxDefaultPosition, wxSize(60,-1));
  sizerH->Add(m_pVolRateMinTxt, 1, wxEXPAND|wxALL, 3);
  sizerH->Add(new wxStaticText(this, -1, wxT(" : ")),
	      0, wxALIGN_LEFT|wxALL, 3);
  m_pVolRateMaxTxt = new wxTextCtrl(this, MPP_Sv_setMask_VolRateMaxTxt,
				    wxT("1.0"),
				    wxDefaultPosition, wxSize(60,-1));
  sizerH->Add(m_pVolRateMaxTxt, 1, wxEXPAND|wxALL, 3);

  // Medium
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxALL, 0);
  sizerH->Add(new wxStaticText(this, -1, wxT("Medium ID")),
              0, wxALIGN_LEFT|wxALL, 3);
  m_pMediumTxt = new wxTextCtrl(this, MPP_Sv_setMask_MediumTxt, wxT("0"),
				wxDefaultPosition, wxDefaultSize);
  sizerH->Add(m_pMediumTxt, 0, wxEXPAND|wxALL, 3);

  // set mask button
  m_pSetMaskBtn = new wxButton(this, MPP_Sv_setMask_SetMaskBtn,
			       wxT("set mask"));
  topsizer->Add(m_pSetMaskBtn, 0, wxEXPAND|wxALL, 5);

  // Offset
  topsizer->Add(new wxStaticText(this, -1, wxT("shift index")),
                0, wxALIGN_LEFT|wxALL, 3);
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND);
  sizerH->Add(new wxStaticText(this, -1, wxT(" I ")), 0, wxALIGN_LEFT|wxALL, 3);
  m_pOffsetXTxt = new wxTextCtrl(this, MPP_Sv_setMask_OffsetXTxt, wxT(""),
				 wxDefaultPosition, wxDefaultSize,
				 wxTE_PROCESS_ENTER);
  sizerH->Add(m_pOffsetXTxt, 0, wxALL, 3);

  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND);
  sizerH->Add(new wxStaticText(this, -1, wxT(" J")), 0, wxALIGN_LEFT|wxALL, 3);
  m_pOffsetYTxt = new wxTextCtrl(this, MPP_Sv_setMask_OffsetYTxt, wxT(""),
				 wxDefaultPosition, wxDefaultSize,
				 wxTE_PROCESS_ENTER);
  sizerH->Add(m_pOffsetYTxt, 0, wxALL, 3);

  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND);
  sizerH->Add(new wxStaticText(this, -1, wxT(" K")), 0, wxALIGN_LEFT|wxALL, 3);
  m_pOffsetZTxt = new wxTextCtrl(this, MPP_Sv_setMask_OffsetZTxt, wxT(""),
				 wxDefaultPosition, wxDefaultSize,
				 wxTE_PROCESS_ENTER);
  sizerH->Add(m_pOffsetZTxt, 0, wxALL, 3);

#if 0
  // update minmax
  m_pUpdateMinMaxChk = new wxCheckBox(this, MPP_Sv_setMask_UpdateMinMaxChk,
				      wxT("update minmax"));
  topsizer->Add(m_pUpdateMinMaxChk, 0, wxEXPAND|wxALL, 3);
#endif

  // show mask
  m_pShowMaskChk = new wxCheckBox(this, MPP_Sv_setMask_ShowMaskChk,
				  wxT("show mask"));
  topsizer->Add(m_pShowMaskChk, 0, wxEXPAND|wxALL, 3);

  // post process
  SetSizer(topsizer);
  addTo(parent);
  topsizer->Layout();
  (void)update();
}

vsnMPP_Sv_setMask::~vsnMPP_Sv_setMask() {
}


/* interface */

bool vsnMPP_Sv_setMask::update() {
  if ( ! m_pFileTxt || ! m_pTypeRadio || ! m_pMediumTxt ||
       ! m_pVolRateMinTxt || ! m_pVolRateMaxTxt || ! m_pShowMaskChk ||
       //! m_pUpdateMinMaxChk || 
       ! m_pOffsetXTxt || ! m_pOffsetYTxt || ! m_pOffsetZTxt )
    return false;

  vsnMethod_Sv_setMask* pm = dynamic_cast<vsnMethod_Sv_setMask*>(p_method);
  if ( ! pm ) return false;
  char txt[64];

  // path
  string path = pm->getMaskPath();
  if ( path.empty() ) m_pFileTxt->SetValue(wxT(""));
  else m_pFileTxt->SetValue(vsnApp::ConvSysToWx(path));

  // type
  vsnData_Sv::MaskType type = pm->getMaskType();
  m_pTypeRadio->SetSelection(type);

  // VolRate
  float vrr[2]; pm->getMaskVolRateRange(vrr);
  sprintf(txt, "%g", vrr[0]);
  m_pVolRateMinTxt->SetValue(vsnApp::ConvSysToWx(txt));
  sprintf(txt, "%g", vrr[1]);
  m_pVolRateMaxTxt->SetValue(vsnApp::ConvSysToWx(txt));

  // Medium
  sprintf(txt, "%d", pm->getMaskMediumID());
  m_pMediumTxt->SetValue(vsnApp::ConvSysToWx(txt));

  // Offset
  int ofst[3]; pm->getOffset(ofst);
  sprintf(txt, "%d", ofst[0]);
  m_pOffsetXTxt->SetValue(vsnApp::ConvSysToWx(txt));
  sprintf(txt, "%d", ofst[1]);
  m_pOffsetYTxt->SetValue(vsnApp::ConvSysToWx(txt));
  sprintf(txt, "%d", ofst[2]);
  m_pOffsetZTxt->SetValue(vsnApp::ConvSysToWx(txt));

#if 0
  // update minmax
  m_pUpdateMinMaxChk->SetValue(pm->getUpdateMinMaxMode());
#endif

  // show mask
  m_pShowMaskChk->SetValue(pm->getMaskShowMode());

  return true;
}


/* event handler */

void vsnMPP_Sv_setMask::OnBrowsBtn(wxCommandEvent& event) {
  if ( ! m_pFileTxt ) return;
  vsnMethod_Sv_setMask* pm = dynamic_cast<vsnMethod_Sv_setMask*>(p_method);
  if ( ! pm ) return;

  wxFileDialog fileDlg(this, wxT("specify mask file"),
                       wxT(""), wxT(""), // default Dir / File
                       wxT("SVX files (*.svx)|*.svx|")
                       wxT("SBX files (*.sbx)|*.sbx|")
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

void vsnMPP_Sv_setMask::OnSetMaskBtn(wxCommandEvent& event) {
  if ( ! m_pFileTxt || ! m_pTypeRadio ||
       ! m_pVolRateMinTxt || ! m_pVolRateMaxTxt ||
       ! m_pMediumTxt /* || ! m_pUpdateMinMaxChk */ ) return;
  vsnMethod_Sv_setMask* pm = dynamic_cast<vsnMethod_Sv_setMask*>(p_method);
  if ( ! pm ) return;

  wxString valStr;
  valStr = m_pFileTxt->GetValue();
  string mskPath = vsnPath_normalize(vsnApp::ConvWxToSys(valStr));
  vsnData_Sv::MaskType mskType
    = (vsnData_Sv::MaskType)m_pTypeRadio->GetSelection();
  float vrRange[2];
  valStr = m_pVolRateMinTxt->GetValue();
  vrRange[0] = (float)atof(vsnApp::ConvWxToSys(valStr).c_str());
  valStr = m_pVolRateMaxTxt->GetValue();
  vrRange[1] = (float)atof(vsnApp::ConvWxToSys(valStr).c_str());
  valStr = m_pMediumTxt->GetValue();
  int mskMid = atoi(vsnApp::ConvWxToSys(valStr).c_str());
#if 0
  bool updMinMax = m_pUpdateMinMaxChk->GetValue();
#else
  bool updMinMax = false;
#endif
  int ofst[3];
  valStr = m_pOffsetXTxt->GetValue();
  ofst[0] = atoi(vsnApp::ConvWxToSys(valStr).c_str());
  valStr = m_pOffsetYTxt->GetValue();
  ofst[1] = atoi(vsnApp::ConvWxToSys(valStr).c_str());
  valStr = m_pOffsetZTxt->GetValue();
  ofst[2] = atoi(vsnApp::ConvWxToSys(valStr).c_str());

  if ( pm->setMask(mskPath, mskType, vrRange, mskMid, updMinMax, ofst) )
    pm->chkNotice();
  else
    update();
}

void vsnMPP_Sv_setMask::OnOffsetTxts(wxCommandEvent& event) {
  if ( ! m_pFileTxt || ! m_pTypeRadio ||
       ! m_pVolRateMinTxt || ! m_pVolRateMaxTxt ||
       ! m_pMediumTxt /* || ! m_pUpdateMinMaxChk */ ) return;
  vsnMethod_Sv_setMask* pm = dynamic_cast<vsnMethod_Sv_setMask*>(p_method);
  if ( ! pm ) return;

  wxString valStr;
    valStr = m_pFileTxt->GetValue();
  string mskPath = vsnPath_normalize(vsnApp::ConvWxToSys(valStr));
  vsnData_Sv::MaskType mskType
    = (vsnData_Sv::MaskType)m_pTypeRadio->GetSelection();
  float vrRange[2];
  valStr = m_pVolRateMinTxt->GetValue();
  vrRange[0] = (float)atof(vsnApp::ConvWxToSys(valStr).c_str());
  valStr = m_pVolRateMaxTxt->GetValue();
  vrRange[1] = (float)atof(vsnApp::ConvWxToSys(valStr).c_str());
  valStr = m_pMediumTxt->GetValue();
  int mskMid = atoi(vsnApp::ConvWxToSys(valStr).c_str());
#if 0
  bool updMinMax = m_pUpdateMinMaxChk->GetValue();
#else
  bool updMinMax = false;
#endif
  int ofst[3];
  valStr = m_pOffsetXTxt->GetValue();
  ofst[0] = atoi(vsnApp::ConvWxToSys(valStr).c_str());
  valStr = m_pOffsetYTxt->GetValue();
  ofst[1] = atoi(vsnApp::ConvWxToSys(valStr).c_str());
  valStr = m_pOffsetZTxt->GetValue();
  ofst[2] = atoi(vsnApp::ConvWxToSys(valStr).c_str());

  if ( pm->setMask(mskPath, mskType, vrRange, mskMid, updMinMax, ofst) )
    pm->chkNotice();
  else
    update();
}

void vsnMPP_Sv_setMask::OnShowMaskChk(wxCommandEvent& event) {
  if ( ! m_pShowMaskChk ) return;
  bool val = m_pShowMaskChk->GetValue();
  
  vsnMethod_Sv_setMask* pm = dynamic_cast<vsnMethod_Sv_setMask*>(p_method);
  if ( ! pm ) return;

  if ( pm->setMaskShowMode(val) )
    pm->chkNotice();
}


//----------------------------------------------------------------
// class vsnSvMaskArea
//----------------------------------------------------------------

/* constructors / destructor */

vsnSvMaskArea::vsnSvMaskArea(vsnData_Sv* pdata,
			     const std::string& name, const Bool ssm)
  : vfrNode(name, ssm), p_data(NULL)
{
  alcMaterial();
  setNormalMode(AT_PER_FACE);
  setData(pdata);
}

vsnSvMaskArea::~vsnSvMaskArea() {
}


/* interface */

bool vsnSvMaskArea::setData(vsnData_Sv* pdata) {
  p_data = pdata;
  return update();
}

bool vsnSvMaskArea::update() {
  if ( ! p_data ) return false;

  generateBbox();
  notice();
  return true;
}

void vsnSvMaskArea::setShowMode(const bool sm) {
  _material->setRenderMode(sm ? RT_SMOOTH : RT_NONE);
}

bool vsnSvMaskArea::getShowMode() const {
  return ((_material->getRenderMode() == RT_NONE) ? false : true);
}


/* override vfrNode method */

void vsnSvMaskArea::generateBbox() {
  if ( ! p_data ) return;
  const CES::Vec3<float>* dbb = p_data->getBbox();
  if ( dbb ) {
    _bbox[0] = dbb[0];
    _bbox[1] = dbb[1];
  }
  checkBbox();
}

void vsnSvMaskArea::renderSolid() {
  if ( ! p_data ) return;
  CES::Vec3<size_t> dims = p_data->getDims();
  size_t dimSz = dims[0] * dims[1] * dims[2];
  if ( dimSz < 8 ) return;

  unsigned char* pmsk = (unsigned char*)p_data->getMask();
  if ( ! pmsk ) return;
  vector3* pCoord = (vector3*)p_data->getCoord(p_data->getCurrentStepIdx());
  if ( ! pCoord ) return;

  glColor4fv(_colors[0]);

  // display-list check
  if ( beginDispList(DLF_SOLID) ) return;

  register size_t i, j, k, idx;
  Vec3<float> nv, pv[8];
  DVec3 dv0, dv1, dv;
  _gus.setup(dims, 0, (float*)pCoord, (float*)NULL);

  glBegin(GL_QUADS);

  for ( k = 0; k < dims[2]; k++ ) {
    dv0[2] = ((k==0) ? 0.f : k - 0.5);
    dv1[2] = ((k==dims[2]-1) ? k : k + 0.5);

    for ( j = 0; j < dims[1]; j++ ) {
      dv0[1] = ((j==0) ? 0.f : j - 0.5);
      dv1[1] = ((j==dims[1]-1) ? j : j + 0.5);

      for ( i = 0; i < dims[0]; i++ ) {
        idx = dims[0]*dims[1]*k + dims[0]*j + i;
	if ( ! pmsk[idx] ) continue;

	dv0[0] = ((i==0) ? 0.f : i - 0.5);
	dv1[0] = ((i==dims[0]-1) ? i : i + 0.5);

	dv[0] = dv0[0];
	dv[1] = dv0[1];
	dv[2] = dv0[2];
	if ( ! _gus.InterpolateCoord(dv, pv[0].m_v) ) continue;
	dv[0] = dv1[0];
	if ( ! _gus.InterpolateCoord(dv, pv[1].m_v) ) continue;
	dv[0] = dv0[0];
	dv[1] = dv1[1];
	if ( ! _gus.InterpolateCoord(dv, pv[2].m_v) ) continue;
	dv[0] = dv1[0];
	if ( ! _gus.InterpolateCoord(dv, pv[3].m_v) ) continue;

	dv[0] = dv0[0];
	dv[1] = dv0[1];
	dv[2] = dv1[2];
	if ( ! _gus.InterpolateCoord(dv, pv[4].m_v) ) continue;
	dv[0] = dv1[0];
	if ( ! _gus.InterpolateCoord(dv, pv[5].m_v) ) continue;
	dv[0] = dv0[0];
	dv[1] = dv1[1];
	if ( ! _gus.InterpolateCoord(dv, pv[6].m_v) ) continue;
	dv[0] = dv1[0];
	if ( ! _gus.InterpolateCoord(dv, pv[7].m_v) ) continue;

	// -X face
        if ( i == 0 || ! pmsk[idx -1] ) {
	  nv = (pv[4] - pv[0]) ^ (pv[2] - pv[0]); nv.UnitVec();
          glNormal3fv(nv.m_v);
          glVertex3fv(pv[0].m_v);
          glVertex3fv(pv[4].m_v);
          glVertex3fv(pv[6].m_v);
          glVertex3fv(pv[2].m_v);
        }
        // +X face
        if ( i == dims[0]-1 || ! pmsk[idx +1] ) {
	  nv = (pv[3] - pv[1]) ^ (pv[5] - pv[1]); nv.UnitVec();
	  glNormal3fv(nv.m_v);
	  glVertex3fv(pv[1].m_v);
          glVertex3fv(pv[3].m_v);
          glVertex3fv(pv[7].m_v);
          glVertex3fv(pv[5].m_v);
        }
        // -Y face
        if ( j == 0 || ! pmsk[dims[0]*dims[1]*k + dims[0]*(j-1) + i] ) {
	  nv = (pv[1] - pv[0]) ^ (pv[4] - pv[0]); nv.UnitVec();
	  glNormal3fv(nv.m_v);
	  glVertex3fv(pv[0].m_v);
          glVertex3fv(pv[1].m_v);
          glVertex3fv(pv[5].m_v);
          glVertex3fv(pv[4].m_v);
        }
        // +Y face
        if ( j == dims[1]-1 ||
	     ! pmsk[dims[0]*dims[1]*k + dims[0]*(j+1) + i] ) {
	  nv = (pv[6] - pv[2]) ^ (pv[3] - pv[2]); nv.UnitVec();
	  glNormal3fv(nv.m_v);
	  glVertex3fv(pv[2].m_v);
          glVertex3fv(pv[6].m_v);
          glVertex3fv(pv[7].m_v);
          glVertex3fv(pv[3].m_v);
        }
        // -Z face
        if ( k == 0 || ! pmsk[dims[0]*dims[1]*(k-1) + dims[0]*j + i] ) {
	  nv = (pv[2] - pv[0]) ^ (pv[1] - pv[0]); nv.UnitVec();
	  glNormal3fv(nv.m_v);
	  glVertex3fv(pv[0].m_v);
          glVertex3fv(pv[2].m_v);
          glVertex3fv(pv[3].m_v);
          glVertex3fv(pv[1].m_v);
        }
        // Z face
        if ( k == dims[2]-1 ||
	     ! pmsk[dims[0]*dims[1]*(k+1) + dims[0]*j + i] ) {
	  nv = (pv[5] - pv[4]) ^ (pv[6] - pv[4]); nv.UnitVec();
	  glNormal3fv(nv.m_v);
	  glVertex3fv(pv[4].m_v);
          glVertex3fv(pv[5].m_v);
          glVertex3fv(pv[7].m_v);
          glVertex3fv(pv[6].m_v);
        }
      } // end of for(i)
    } // end of for(j)
  } // end of for(k)

  glEnd(); // GL_QUADS

  // end display-list definition
  endDispList(DLF_SOLID);
}

void vsnSvMaskArea::renderWire() {
  glEnable(GL_BLEND);
  glDepthMask(GL_FALSE);

  renderSolid();

  glDisable(GL_BLEND);
  glDepthMask(GL_TRUE);
}


//----------------------------------------------------------------
// class vsnMethod_Sv_setMask
//----------------------------------------------------------------

/* constructors / destructor */

vsnMethod_Sv_setMask::vsnMethod_Sv_setMask(const std::string& name)
  : vsnMethodObj(name), m_mskArea(NULL),
    m_maskType(vsnData_Sv::MASK_None), m_mediumId(0),
    m_updMinMax(false), m_showMask(false), m_fileType(FILE_None)
{
  m_volRateRange[0] = 0.f; m_volRateRange[1] = 1.f;
  m_offset[0] = m_offset[1] = m_offset[2] = 0;
}

vsnMethod_Sv_setMask::~vsnMethod_Sv_setMask() {
  if ( m_mskArea )
    delete m_mskArea;

  if ( ! m_maskFile.empty() && m_maskType != vsnData_Sv::MASK_None ) {
    vsnData_Sv* pdSv = dynamic_cast<vsnData_Sv*>(p_refData);
    if ( pdSv && pdSv->getMask() )
      pdSv->setMaskSVX(string());
  }
}


/* methods */

bool vsnMethod_Sv_setMask::setMask(const std::string& mskPath,
				   const vsnData_Sv::MaskType mskType,
				   const float* vrRange, const int medId,
				   const bool updMinMax, const int* ofst) {
  vsnData_Sv* pData = dynamic_cast<vsnData_Sv*>(p_refData);
  if ( ! pData ) return false;

  m_fileType = getSuffixType(mskPath);
  bool ret = true;
  if ( m_fileType == FILE_SVX )
    ret = pData->setMaskSVX(mskPath, mskType, vrRange, medId, updMinMax, ofst);
  else if ( m_fileType == FILE_SBX )
    ret = pData->setMaskSBX(mskPath, mskType, vrRange, medId, updMinMax, ofst);

  m_maskFile = mskPath;
  m_maskType = mskType;
  m_volRateRange[0] = vrRange[0]; m_volRateRange[1] = vrRange[1];
  m_mediumId = medId;
  m_updMinMax = updMinMax;
  if ( ofst ) {
    m_offset[0] = ofst[0]; m_offset[1] = ofst[1]; m_offset[2] = ofst[2];
  } else {
    m_offset[0] = m_offset[1] = m_offset[2] = 0;
  }

  return ret;
}

void vsnMethod_Sv_setMask::getOffset(int* ofst) const {
  if ( ! ofst ) return;
  ofst[0] = m_offset[0]; ofst[1] = m_offset[1]; ofst[2] = m_offset[2];
}

bool vsnMethod_Sv_setMask::setMaskShowMode(const bool smm) {
  if ( m_showMask == smm ) return true;
  m_showMask = smm;

  if ( m_mskArea ) {
    m_mskArea->setShowMode(true);
  }

  if ( ! updateStep(m_requestedStp, true, false) ) {
    return false;
  }

  updateUI();
  return true;
}


/* vsnTimeSeriesMethodIF methods */

bool vsnMethod_Sv_setMask::updateStep(const int stp,
				      const bool force, const bool cascade)
{
  vsnData_Sv* pData = dynamic_cast<vsnData_Sv*>(p_refData);
  if ( ! pData ) return false;
  m_requestedStp = stp;
  if ( m_requestedStp < 0 )
    m_requestedStp = pData->getCurrentStepIdx();

  if ( ! force && m_updatedStp >= 0 && pData->isUniformCoord() ) {
    m_updatedStp = m_requestedStp;
    return true;
  }
  if ( force ) m_updatedStp = -1;
  if ( m_updatedStp == m_requestedStp ) return true;
  m_updatedStp = -1;

  // mask area data
  if ( ! m_mskArea ) {
    m_mskArea = new vsnSvMaskArea(NULL);
    if ( ! m_mskArea ) {
      ErrMsg(MsgERR, getMethodType() + string("[") + getName()
             + string("]: memory allocation failed"));
      return false;
    }
    addChild(m_mskArea);
  }
  m_mskArea->setShowMode(false);
  if ( ! m_show ) {
    return true;
  }

  // check 'showMask'
  if ( ! m_showMask ) {
    return true;
  }

  // set data
  if ( ! m_mskArea->setData(pData) ) {
    ErrMsg(MsgERR, getMethodType() + string("[") + getName()
           + string("]: setup data failed"));
    return false;
  }

  // set color
  m_mskArea->setColor4(0, m_colour);
  m_mskArea->setTransparency(m_colour[3] < OPAC_THRESH);

  // ok
  m_mskArea->setShowMode(true);
  setHilight(m_hilight);
  m_updatedStp = m_requestedStp;
  return true;
}


/* vsnMethodObj methods */

bool vsnMethod_Sv_setMask::update(const bool force) {
  return updateStep(m_requestedStp, force);
}

vsnMethodPP* vsnMethod_Sv_setMask::getParamPanel(wxPanel* pp) {
  vsnMPP_Sv_setMask* pp_setMask = new vsnMPP_Sv_setMask(pp, this);
  if ( ! pp_setMask ) return NULL;
  return pp_setMask;
}

void vsnMethod_Sv_setMask::setFileType(const int ftype) {
  if ( ftype == 0 ) m_fileType = FILE_SVX;
  else if ( ftype == 1 ) m_fileType = FILE_SBX;
  else m_fileType = FILE_None;
}

/* serialize : from vsnIoObject */

bool vsnMethod_Sv_setMask::parseXML(xmlNodePtr xnp) {
  static xmlChar* xs;
  xmlNodePtr cur;
  string maskFile;
  vsnData_Sv::MaskType maskType = vsnData_Sv::MASK_None;
  float volRange[2] = {0.f, 1.f};
  int ofst[3] = {0, 0, 0};
  int mediumId = 0;
  bool updMinMax = false;

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

      if ( xsN == string("mask_file") ) {
        maskFile = xsV;
      } // end of "mask_file"
      else if ( xsN == string("mask_type") ) {
	if ( xsV == string("none") )
	  maskType = vsnData_Sv::MASK_None;
	else if ( xsV == string("vol_rate") )
	  maskType = vsnData_Sv::MASK_VolRate;
	else if ( xsV == string("medium") )
	  maskType = vsnData_Sv::MASK_Medium;
	else {
	  ErrMsg(MsgERR, msgHdr + string("invalid value in param mask_type"));
	  goto _NEXT_XML_NODE;
        }
      } // end of "mask_type"
      else if ( xsN == string("vol_rate") ) {
	string minStr, maxStr; char c;
        istringstream iss(xsV);
        while( iss.get(c) && c != ':' ) minStr.push_back(c);
        while( iss.get(c) ) maxStr.push_back(c);
        if ( minStr.empty() || maxStr.empty() ) {
          ErrMsg(MsgERR, msgHdr
                 + string("invalid format in param vol_rate"));
          goto _NEXT_XML_NODE;
        }
	volRange[0] = (float)atof(minStr.c_str());
	volRange[1] = (float)atof(maxStr.c_str());
      } // end of "vol_rate"
      else if ( xsN == string("medium_id") ) {
	mediumId = atoi(xsV.c_str());
      } // end of "medium_id"
      else if ( xsN == string("upd_minmax") ) {
	if ( xsV == string("yes") ) updMinMax = true;
	else if ( xsV == string("no") ) updMinMax = false;
	else {
	  ErrMsg(MsgERR, msgHdr +
		 string("invalid value in param upd_minmax"));
	  goto _NEXT_XML_NODE;
	}
      } // end of "upd_minmax"
      else if ( xsN == string("offset") ) {
	Vec3<int> idcs;
        istringstream iss(xsV);
        iss >> idcs;
	ofst[0] = idcs[0]; ofst[1] = idcs[1]; ofst[2] = idcs[2];
      } // end of "offset"
      else if ( xsN == string("show_mask") ) {
	bool sm = false;
	if ( xsV == string("yes") ) sm = true;
	else if ( xsV == string("no") ) sm = false;
	else {
	  ErrMsg(MsgERR, msgHdr + string("invalid value in param show_mask"));
	  goto _NEXT_XML_NODE;
	}
	if ( ! setMaskShowMode(sm) ) {
	  ErrMsg(MsgERR, msgHdr + string("can't update show_mask mode"));
          goto _NEXT_XML_NODE;
	}
      } // end of "show_mask"
    } // end of param

  _NEXT_XML_NODE:
    cur = cur->next;
  } // end of while(cur)

  // set mask params
  if ( ! maskFile.empty() || maskType != vsnData_Sv::MASK_None ||
       volRange[0] != 0.f || volRange[1] != 1.f ||
       mediumId != 0 || updMinMax != false ) {
    if ( ! setMask(maskFile, maskType, volRange, mediumId, updMinMax, ofst) ) {
      ErrMsg(MsgERR, msgHdr + string("setMask failed"));
      return false;
    }
  }

  return true;
}

bool vsnMethod_Sv_setMask::outputXML(std::ostream& os, const size_t ts) {
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
  // mask_file
  if ( ! m_maskFile.empty() ) {
    os << idts_2 << "<param name=\"mask_file\" value=\"" << m_maskFile
       << "\" />" << endl;
  }

  // mask_type
  if ( m_maskType == vsnData_Sv::MASK_VolRate ) {
    os << idts_2 << "<param name=\"mask_type\" value=\"vol_rate\" />" << endl;
  } else if ( m_maskType == vsnData_Sv::MASK_Medium ) {
    os << idts_2 << "<param name=\"mask_type\" value=\"medium\" />" << endl;
  }

  // vol_rate
  if ( m_volRateRange[0] != 0.f || m_volRateRange[1] != 1.f ) {
    os << idts_2 << "<param name=\"vol_rate\" value=\""
       << m_volRateRange[0] << ":" << m_volRateRange[1] << "\" />" << endl;
  }

  // medium_id
  if ( m_mediumId != 0 ) {
    os << idts_2 << "<param name=\"medium_id\" value=\""
       << m_mediumId << "\" />" << endl;
  }

  // update minmax
  if ( m_updMinMax ) {
    os << idts_2 << "<param name=\"upd_minmax\" value=\"yes\" />" << endl;
  }

  // offset
  if ( m_offset[0] != 0 || m_offset[1] != 0 || m_offset[2] != 0 ) {
    os << idts_2 << "<param name=\"offset\" value=\""
       << m_offset[0] << "/" << m_offset[1] << "/" << m_offset[2]
       << "\" />" << endl;
  }

  // show_mask
  if ( m_showMask ) {
    os << idts_2 << "<param name=\"show_mask\" value=\"yes\" />" << endl;
  }

  os << idts << "</method>" << endl;

  return true;
}

bool vsnMethod_Sv_setMask::commandXML(xmlNodePtr xnp) {
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

  if ( nameStr == "set_mask_file" ) {
    if ( ! setMask(valueStr, m_maskType,
		   m_volRateRange, m_mediumId, m_updMinMax, m_offset) ) {
      ErrMsg(MsgERR, msgHdr +
             string("command set_mask_file: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_mask_file"
  else if ( nameStr == "set_mask_type" ) {
    vsnData_Sv::MaskType maskType = vsnData_Sv::MASK_None;
    if ( valueStr == string("none") )
      maskType = vsnData_Sv::MASK_None;
    else if ( valueStr == string("vol_rate") )
      maskType = vsnData_Sv::MASK_VolRate;
    else if ( valueStr == string("medium") )
      maskType = vsnData_Sv::MASK_Medium;
    else {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_mask_type: invalid value: ") + valueStr);
      return false;
    }
    if ( ! setMask(m_maskFile, maskType,
		   m_volRateRange, m_mediumId, m_updMinMax, m_offset) ) {
      ErrMsg(MsgERR, msgHdr +
             string("command set_mask_type: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_mask_type"
  else if ( nameStr == "set_vol_rate" ) {
    float volRange[2] = {0.f, 1.f};
    string minStr, maxStr; char c;
    istringstream iss(valueStr);
    while( iss.get(c) && c != ':' ) minStr.push_back(c);
    while( iss.get(c) ) maxStr.push_back(c);
    if ( minStr.empty() || maxStr.empty() ) {
      ErrMsg(MsgERR, msgHdr + string("command set_vol_rate: ") +
	     string("invalid format: ") + valueStr);
      return false;
    }
    volRange[0] = (float)atof(minStr.c_str());
    volRange[1] = (float)atof(maxStr.c_str());
    if ( ! setMask(m_maskFile, m_maskType,
		   volRange, m_mediumId, m_updMinMax, m_offset) ) {
      ErrMsg(MsgERR, msgHdr +
             string("command set_vol_rate: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_vol_rate"
  else if ( nameStr == "set_medium_id" ) {
    int mediumId = atoi(valueStr.c_str());
    if ( ! setMask(m_maskFile, m_maskType,
		   m_volRateRange, mediumId, m_updMinMax, m_offset) ) {
      ErrMsg(MsgERR, msgHdr +
             string("command set_medium_id: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_medium_id"
  else if ( nameStr == "set_upd_minmax" ) {
    bool updMinMax = false;
    if ( valueStr == string("yes") ) updMinMax = true;
    else if ( valueStr == string("no") ) updMinMax = false;
    else {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_upd_minmax: invalid value: ") + valueStr);
      return false;
    }
    if ( ! setMask(m_maskFile, m_maskType,
		   m_volRateRange, m_mediumId, updMinMax, m_offset) ) {
      ErrMsg(MsgERR, msgHdr +
             string("command set_upd_minmax: set failed: ") + valueStr);
      return false;
    }
  } // of "set_upd_minmax"
  else if ( nameStr == "set_offset" ) {
    Vec3<int> idcs;
    istringstream iss(valueStr);
    iss >> idcs;
    if ( ! setMask(m_maskFile, m_maskType,
		   m_volRateRange, m_mediumId, m_updMinMax, idcs.m_v) ) {
      ErrMsg(MsgERR, msgHdr +
             string("command set_offset: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_offset"
  else if ( nameStr == "set_show_mask" ) {
    bool sm = false;
    if ( valueStr == string("yes") ) sm = true;
    else if ( valueStr == string("no") ) sm = false;
    else {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_show_mask: invalid value: ") + valueStr);
      return false;
    }
    if ( ! setMaskShowMode(sm) ) {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_show_mask: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_show_mask"

  return true;
}

// static
vsnMethod_Sv_setMask::FileType vsnMethod_Sv_setMask::getSuffixType(const std::string& path) 
{
  if ( path.empty() ) return FILE_None;
  size_t dp = path.rfind('.');
  if ( dp == string::npos ) return FILE_None;

  string sfx = path.substr(dp);
  if ( sfx == ".svx" || sfx == ".SVX" )
    return FILE_SVX;
  else if ( sfx == ".sbx" || sfx == ".SBX" )
    return FILE_SBX;
  else return FILE_None;
}
