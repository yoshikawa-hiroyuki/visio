//
// vsnMethod_Sv_isosurf
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

#include "vsnMethod_Sv_isosurf.h"
#include "vsnIsosurf.h"
#include "vsnOctTree.h" // for decomp vecIdx
#include "vsnUiView.h"
#include "vsnColorBar.h"
#include "vsnError.h"

#include "vsnShapeExporter.h"
#include <sstream>

using namespace std;
using namespace CES;
using namespace VSN;


//----------------------------------------------------------------
// class vsnMPP_Sv_isosurf
//----------------------------------------------------------------

BEGIN_EVENT_TABLE(vsnMPP_Sv_isosurf, wxPanel)
  EVT_COMBOBOX(MPP_Sv_isosurf_SelDataLst,
               vsnMPP_Sv_isosurf::OnSelDataLst)
  EVT_CHECKLISTBOX(MPP_Sv_isosurf_VecDataChkLst,
                   vsnMPP_Sv_isosurf::OnVecDataChkLst)
  EVT_TEXT_ENTER(MPP_Sv_isosurf_ValueTxt,
		 vsnMPP_Sv_isosurf::OnValueTxt)
  EVT_CHECKBOX(MPP_Sv_isosurf_UseCMapChk,
	       vsnMPP_Sv_isosurf::OnUseCMapChk)
  EVT_CHECKBOX(MPP_Sv_isosurf_UseCMapAlphaChk,
	       vsnMPP_Sv_isosurf::OnUseCMapAlphaChk)
  EVT_CHECKBOX(MPP_Sv_isosurf_UpdMinMaxChk,
	       vsnMPP_Sv_isosurf::OnUpdMinMaxChk)
  EVT_RADIOBOX(MPP_Sv_isosurf_ShowFaceRadio,
	       vsnMPP_Sv_isosurf::OnShowFaceRadio)
  EVT_BUTTON(MPP_Sv_isosurf_ExportBtn,
	     vsnMPP_Sv_isosurf::OnExportBtn)
  EVT_CHECKBOX(MPP_Sv_isosurf_AutoExportChk,
	       vsnMPP_Sv_isosurf::OnAutoExportChk)
END_EVENT_TABLE()


/* constructors / destructor */

vsnMPP_Sv_isosurf::vsnMPP_Sv_isosurf(wxPanel* parent, vsnMethodObj* pm)
  : vsnMethodPP(parent, pm)
{
  assert(parent);
  assert(dynamic_cast<vsnMethod_Sv_isosurf*>(pm));

  wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer* sizerH;

  // iso value
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxALL|wxEXPAND, 0);
  sizerH->Add(new wxStaticText(this, -1, wxT("iso value")),
              0, wxALIGN_LEFT|wxALL, 3);
  m_pValueTxt = new wxTextCtrl(this, MPP_Sv_isosurf_ValueTxt, wxT(""),
                               wxDefaultPosition, wxDefaultSize,
                               wxTE_PROCESS_ENTER);
  sizerH->Add(m_pValueTxt, 1, wxEXPAND|wxALL, 3);

  // data selector
  topsizer->Add(new wxStaticText(this, -1, wxT("select scalar data")),
                0, wxALIGN_LEFT|wxALL, 3);
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxALL|wxEXPAND, 0);
  m_pSelDataLst = new wxComboBox(this, MPP_Sv_isosurf_SelDataLst,
                                 wxT(""), wxDefaultPosition, wxDefaultSize,
                                 0, NULL, wxCB_READONLY);
  sizerH->Add(m_pSelDataLst, 1, wxEXPAND|wxALL, 3);

  // vector data selector
  topsizer->Add(new wxStaticText(this, -1, wxT("vector components")),
                0, wxALIGN_LEFT|wxALL, 3);
  m_pVecDataChkLst = new wxCheckListBox(this, MPP_Sv_isosurf_VecDataChkLst);
  topsizer->Add(m_pVecDataChkLst, 0, wxEXPAND|wxALL, 3);

  // use CMap / CMap alpha
  m_pUseCMapChk = new wxCheckBox(this, MPP_Sv_isosurf_UseCMapChk,
				 wxT("use cmap"));
  m_pUseCMapChk->SetValue(TRUE);
  topsizer->Add(m_pUseCMapChk, 0, wxALL, 3);
  m_pUseCMapAlphaChk = new wxCheckBox(this, MPP_Sv_isosurf_UseCMapAlphaChk,
				      wxT("use cmap alpha"));
  topsizer->Add(m_pUseCMapAlphaChk, 0, wxALL, 3);

  // update minmax mode
  m_pUpdMinMaxChk = new wxCheckBox(this, MPP_Sv_isosurf_UpdMinMaxChk,
                                   wxT("update minmax"));
  m_pUpdMinMaxChk->SetValue(TRUE);
  topsizer->Add(m_pUpdMinMaxChk, 0, wxALL, 3);

  // show front / back
  wxString items[] = {wxString(wxT("both")),
		      wxString(wxT("front")), wxString(wxT("back"))};
  m_pShowFaceRadio = new wxRadioBox(this, MPP_Sv_isosurf_ShowFaceRadio,
				    wxT("show face"),
				    wxDefaultPosition, wxDefaultSize,
				    3, items, 0, wxRA_SPECIFY_COLS);
  topsizer->Add(m_pShowFaceRadio, 0, wxEXPAND|wxALL, 3);

  // export button
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND|wxALL, 0);
  m_pExportBtn = new wxButton(this, MPP_Sv_isosurf_ExportBtn, wxT("export"));
  sizerH->Add(m_pExportBtn, 0, wxALL, 3);
  m_pAutoExportChk = new wxCheckBox(this, MPP_Sv_isosurf_AutoExportChk,
				    wxT("auto export"));
  sizerH->Add(m_pAutoExportChk, 0, wxALL, 3);

  // post process
  SetSizer(topsizer);
  addTo(parent);
  topsizer->Layout();
  (void)update();
}

vsnMPP_Sv_isosurf::~vsnMPP_Sv_isosurf() {
}


/* interface */

bool vsnMPP_Sv_isosurf::update() {
  if ( ! m_pSelDataLst || ! m_pVecDataChkLst || ! m_pValueTxt ||
       ! m_pUseCMapChk || ! m_pUseCMapAlphaChk || ! m_pUpdMinMaxChk ||
       ! m_pShowFaceRadio )
    return false;

  vsnMethod_Sv_isosurf* pm
    = dynamic_cast<vsnMethod_Sv_isosurf*>(p_method);
  if ( ! pm ) return false;
  vsnData_Sv* pdo = dynamic_cast<vsnData_Sv*>(pm->getRefData());
  if ( ! pdo ) return false;

  register int i;
  char txt[64];

  // iso value
  float val = pm->getIsoValue();
  sprintf(txt, "%g", val); m_pValueTxt->SetValue(vsnApp::ConvSysToWx(txt));

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

  // use CMap / CMap alpha
  m_pUseCMapChk->SetValue(pm->getUseCMap());
  m_pUseCMapAlphaChk->SetValue(pm->getUseCMapAlpha());

  // update minmax mode
  m_pUpdMinMaxChk->SetValue(pm->getUpdateMinMaxMode());

  // show front / back
  PolyFaceType fm = pm->getShowFaceMode();
  switch ( fm ) {
  case PF_FRONT:
    m_pShowFaceRadio->SetSelection(1);
    break;
  case PF_BACK:
    m_pShowFaceRadio->SetSelection(2);
    break;
  case PF_BOTH: default:
    m_pShowFaceRadio->SetSelection(0);
  }

  // auto export
  m_pAutoExportChk->SetValue(pm->getAutoExport());

  return true;
}


/* event handler */

void vsnMPP_Sv_isosurf::OnSelDataLst(wxCommandEvent& event) {
  if ( ! m_pSelDataLst ) return;
  WhichDataType sel = event.GetInt();
  if ( sel < 0 ) return;

  wxString valStr = m_pSelDataLst->GetString(sel);
  if ( valStr == wxT("vector length") )
    sel = DATA_Veclen;

  vsnMethod_Sv_isosurf* pm
    = dynamic_cast<vsnMethod_Sv_isosurf*>(p_method);
  if ( ! pm ) return;
  if ( sel == pm->getSelectedData() ) return;

  if ( ! pm->setSelectedData(sel) ) {
    update();
    return;
  }
  pm->chkNotice();
}

void vsnMPP_Sv_isosurf::OnVecDataChkLst(wxCommandEvent& event) {
  if ( ! m_pVecDataChkLst ) return;
  int dlen = m_pVecDataChkLst->GetCount();
  if ( dlen < 3 ) return;

  vsnMethod_Sv_isosurf* pm
    = dynamic_cast<vsnMethod_Sv_isosurf*>(p_method);
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

void vsnMPP_Sv_isosurf::OnValueTxt(wxCommandEvent& event) {
  if ( ! m_pValueTxt ) return;

  vsnMethod_Sv_isosurf* pm
    = dynamic_cast<vsnMethod_Sv_isosurf*>(p_method);
  if ( ! pm ) return;

  wxString valStr = m_pValueTxt->GetValue();
  if ( valStr.IsEmpty() ) {
    char txt[64]; sprintf(txt, "%g", pm->getIsoValue());
    m_pValueTxt->SetValue(vsnApp::ConvSysToWx(txt));
    return;
  }
  float val = (float)atof(vsnApp::ConvWxToSys(valStr).c_str());

  if ( pm->setIsoValue(val) )
    pm->chkNotice();
}

void vsnMPP_Sv_isosurf::OnUseCMapChk(wxCommandEvent& event) {
  if ( ! m_pUseCMapChk ) return;
  bool val = m_pUseCMapChk->GetValue();

  vsnMethod_Sv_isosurf* pm
    = dynamic_cast<vsnMethod_Sv_isosurf*>(p_method);
  if ( ! pm ) return;

  if ( pm->setUseCMap(val) )
    pm->chkNotice();
}

void vsnMPP_Sv_isosurf::OnUseCMapAlphaChk(wxCommandEvent& event) {
  if ( ! m_pUseCMapAlphaChk ) return;
  bool val = m_pUseCMapAlphaChk->GetValue();

  vsnMethod_Sv_isosurf* pm
    = dynamic_cast<vsnMethod_Sv_isosurf*>(p_method);
  if ( ! pm ) return;

  if ( pm->setUseCMapAlpha(val) )
    pm->chkNotice();
}

void vsnMPP_Sv_isosurf::OnUpdMinMaxChk(wxCommandEvent& event) {
  if ( ! m_pUpdMinMaxChk ) return;
  bool val = m_pUpdMinMaxChk->GetValue();

  vsnMethod_Sv_isosurf* pm
    = dynamic_cast<vsnMethod_Sv_isosurf*>(p_method);
  if ( ! pm ) return;

  if ( pm->setUpdateMinMaxMode(val) )
    pm->chkNotice();
}

void vsnMPP_Sv_isosurf::OnShowFaceRadio(wxCommandEvent& event) {
  vsnMethod_Sv_isosurf* pm
    = dynamic_cast<vsnMethod_Sv_isosurf*>(p_method);
  if ( ! pm ) return;
  if ( ! m_pShowFaceRadio ) return;
  int val = m_pShowFaceRadio->GetSelection();
  PolyFaceType fm;
  switch ( val ) {
  case 1: fm = PF_FRONT; break;
  case 2: fm = PF_BACK; break;
  case 0: default: fm = PF_BOTH;
  }
  if ( pm->setShowFaceMode(fm) )
    pm->chkNotice();
}

void vsnMPP_Sv_isosurf::OnExportBtn(wxCommandEvent& event) {
  static int fltIdx = 0;

  vsnMethod_Sv_isosurf* pm
    = dynamic_cast<vsnMethod_Sv_isosurf*>(p_method);
  if ( ! pm ) return;
  vsnApp* pApp = vsnApp::GetApp();

  wxFileDialog fileDlg(this, wxT("isosurf: specify file to export"),
		       wxT(""), wxT(""), // default Dir / File
		       wxT("Wavefront OBJ (*.obj;*.wfo)|*.obj;*.wfo")
		       wxT("|STL (*.stl;*.sla;*.stla;*.slb;*.stlb)")
		       wxT("|*.stl;*.sla;*.stla;*.slb;*.stlb")
		       wxT("|(*)|*"),
                       wxFD_SAVE);
  string exportedPath = pm->getExportedPath();
  if ( ! exportedPath.empty() ) {
    fileDlg.SetFilename(vsnApp::ConvSysToWx(exportedPath));
  } else {
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
  }
  fileDlg.SetFilterIndex(fltIdx);

  // get output path
  if ( fileDlg.ShowModal() != wxID_OK ) return;
  string outPath = vsnPath_normalize(vsnApp::ConvWxToSys(fileDlg.GetPath()));
  if ( outPath.empty() ) return;
  fltIdx = fileDlg.GetFilterIndex();

  // override check
  FILE* ofp = fopen(outPath.c_str(), "r");
  if ( ofp ) {
    fclose(ofp);
    wxString msg = wxT("The specified file has already existed\n  ");
    msg += vsnApp::ConvSysToWx(outPath);
    msg += wxT("\n\nAre you sure to override ?\n");
    wxMessageDialog dlg(NULL, msg, wxT("isosurf: export"),
			vsn_wxOK_CANCEL|wxICON_QUESTION);
    if ( dlg.ShowModal() != vsn_wxIDOK ) return;
  }

  // export
  if ( ! pm->exportFaces(outPath) ) {
    ErrMsg(MsgERR, string("isosurf: export failed.\n  File: ") + outPath);
    return;
  }

  return;
}

void vsnMPP_Sv_isosurf::OnAutoExportChk(wxCommandEvent& event) {
   if ( ! m_pAutoExportChk ) return;
  bool val = m_pAutoExportChk->GetValue();

  vsnMethod_Sv_isosurf* pm
    = dynamic_cast<vsnMethod_Sv_isosurf*>(p_method);
  if ( ! pm ) return;

  if ( pm->setAutoExport(val) )
    pm->chkNotice(); 
}


//----------------------------------------------------------------
// class vsnMethod_Sv_isosurf
//----------------------------------------------------------------

/* constructors / destructor */

vsnMethod_Sv_isosurf::
vsnMethod_Sv_isosurf(const std::string& name)
  : vsnMethodObj(name), m_isoTrias(NULL), m_pd(NULL),
    m_selectedData(DATA_None), m_vecDataIdx(0,1,2), m_isoValue(0.f),
    m_useCMap(true), m_useCMapAlpha(false), m_updateMinMax(true),
    m_faceMode(PF_BOTH), m_autoExport(false)
{
}

vsnMethod_Sv_isosurf::~vsnMethod_Sv_isosurf() {
  if ( m_isoTrias )
    delete m_isoTrias;
  if ( m_pd )
    DeAllocate(m_pd);
}


/* methods */

void vsnMethod_Sv_isosurf::adjustRange(const bool updval) {
  bool needRangeUpd = false;

  vsnData_Sv* pData = dynamic_cast<vsnData_Sv*>(p_refData);
  if ( ! pData ) return;
  int dlen = (int)pData->getDataLen();

  float dr[2];
  if ( m_selectedData == DATA_Veclen ) {
    if ( pData->getVectorMaxLen(m_vecDataIdx, dr[1]) ) {
      if ( updval )
	m_isoValue = dr[1] * 0.5f;
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
      if ( updval )
	m_isoValue = (dr[0] + dr[1])*0.5f;
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

void vsnMethod_Sv_isosurf::updateColor() {
  if ( ! m_isoTrias ) return;

  vector4 cv;
  int c = m_lut.getValIdx(m_isoValue);
  memcpy(cv, &m_lut.lutEntry[c*4], sizeof(float)*4);

  if ( m_useCMap )
    m_isoTrias->setColor3(0, cv);
  else
    m_isoTrias->setColor3(0, m_colour);

  if ( m_useCMapAlpha )
    m_isoTrias->setAlpha(TRUE, cv[3]);
  else
    m_isoTrias->setAlpha((m_colour[3]<OPAC_THRESH), m_colour[3]);

  chkNotice();
}

bool vsnMethod_Sv_isosurf::setSelectedData(const WhichDataType sd) {
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

bool vsnMethod_Sv_isosurf::setVecDataIdx(const CES::Vec3<int>& vdidx) {
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

bool vsnMethod_Sv_isosurf::isValidVecData() const {
  vsnData_Sv* pData = dynamic_cast<vsnData_Sv*>(p_refData);
  if ( ! pData ) return false;

  size_t dlen = pData->getDataLen();
  if ( m_vecDataIdx[0] < 0 || m_vecDataIdx[0] >= dlen ) return false;
  if ( m_vecDataIdx[1] < 0 || m_vecDataIdx[1] >= dlen ) return false;
  if ( m_vecDataIdx[2] < 0 || m_vecDataIdx[2] >= dlen ) return false;
  return true;
}

bool vsnMethod_Sv_isosurf::setIsoValue(const float val) {
  if ( m_isoValue == val ) return true;
  m_isoValue = val;

  if ( ! update() ) return false;
  updateUI();
  return true;
}

bool vsnMethod_Sv_isosurf::setUseCMap(const bool ucm) {
  if ( m_useCMap == ucm ) return true;
  m_useCMap = ucm;

  updateColor();
  updateUI();
  return true;
}

bool vsnMethod_Sv_isosurf::setUseCMapAlpha(const bool ucma) {
  if ( m_useCMapAlpha == ucma ) return true;
  m_useCMapAlpha = ucma;

  updateColor();
  updateUI();
  return true;
}

bool vsnMethod_Sv_isosurf::setUpdateMinMaxMode(const bool mode) {
  if ( m_updateMinMax == mode ) return true;
  m_updateMinMax = mode;

  // data range
  adjustRange(false);

  if ( ! update() ) return false;
  updateUI();
  return true;
}

bool vsnMethod_Sv_isosurf::setShowFaceMode(const PolyFaceType fm) {
  if ( m_faceMode == fm ) return true;
  m_faceMode = fm;

  if ( m_isoTrias ) {
    m_isoTrias->getPrivateMaterial()->setFaceMode(m_faceMode);
    chkNotice();
  }
  updateUI();
  return true;
}

bool vsnMethod_Sv_isosurf::exportFaces(const std::string& path) {
  if ( path.empty() ) return false;
  if ( ! m_isoTrias ) return false;

  string path_body, fmt;
  string path_type;
  char c;
  istringstream iss(path);
  while ( iss.get(c) && (c != ';') ) path_body.push_back(c);
  while ( iss.get(c) ) path_type.push_back(c);
  if ( path_body.empty() ) return false;

  if ( path_type == "OBJ" )
    fmt = "obj_a";
  else if ( path_type == "SLA" )
    fmt = "stl_a";
  else if ( path_type == "SLB" )
    fmt = "stl_b";
  else if ( path.size() > 4 ) {
    if ( path.substr(path.size()-4) == ".obj" )
      fmt = "obj_a";
    else if ( path.substr(path.size()-4) == ".wfo" )
      fmt = "obj_a";
    else if ( path.substr(path.size()-4) == ".sla" )
      fmt = "stl_a";
    else if ( path.substr(path.size()-4) == ".slb" )
      fmt = "stl_b";
    else if ( path.substr(path.size()-4) == ".stl" )
      fmt = "stl_a";
    else if ( path.size() > 5 ) {
      if ( path.substr(path.size()-5) == ".stla" )
	fmt = "stl_a";
      else if ( path.substr(path.size()-5) == ".stlb" )
	fmt = "stl_b";
    }
  }
  if ( path_body.empty() || fmt.empty() ) return false;

  string exPath = VSN::ConvTimeStepPath(path_body, p_refData);
  bool ret = false;
  if ( fmt == "obj_a" )
    ret = VSN::SaveWfo(m_isoTrias, exPath);
  else if ( fmt == "stl_a" )
    ret = VSN::SaveSla(m_isoTrias, exPath);
  else if ( fmt == "stl_b" )
    ret = VSN::SaveSlb(m_isoTrias, exPath);

  m_exportPath = path_body;
  return ret;
}

bool vsnMethod_Sv_isosurf::setAutoExport(const bool aem) {
  if ( m_autoExport == aem ) return true;
  m_autoExport = aem;
  if ( m_autoExport ) {
    if ( m_isoTrias ) {
      if ( ! updateStep(m_updatedStp, true) )
	return false;
    }
  }
  updateUI();
  return true;
}


/* vsnTimeSeriesMethodIF methods */

bool vsnMethod_Sv_isosurf::updateStep(const int stp,
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

  // tria data
  if ( ! m_isoTrias ) {
    m_isoTrias = new vfrTriangles();
    if ( ! m_isoTrias ) {
      ErrMsg(MsgERR, getMethodType() + string("[") + getName()
             + string("]: memory allocation failed"));
      return false;
    }
    m_isoTrias->alcMaterial();
    m_isoTrias->setNormalMode(AT_PER_VERTEX);
    addChild(m_isoTrias);
  }
  m_isoTrias->getPrivateMaterial()->setFaceMode(m_faceMode);
  m_isoTrias->getPrivateMaterial()->setRenderMode(RT_NONE);
  if ( ! m_show ) {
    return true;
  }

  // check selected data
  size_t dlen = pData->getDataLen();
  float dr[2] = {0.f, 1.f};
  if ( m_selectedData == DATA_None ) return true;
  else if ( m_selectedData == DATA_Veclen ) {
    if ( ! isValidVecData() ) return true;
    if ( ! pData->getVectorMaxLen(m_vecDataIdx, dr[1]) ) return false;
  }
  else if ( m_selectedData > 0 && m_selectedData <= dlen ) {
    if ( ! pData->getMinMax(m_selectedData -1, dr) ) return false;
  }
  else return true;
  if ( m_isoValue <= dr[0] || m_isoValue >= dr[1] ) return true;

  // get Sv data
  const float* gptr = pData->getCoord(m_requestedStp);
  if ( ! gptr ) {
    return false;
  }
  const float* dptr = pData->getData(m_requestedStp);
  if ( ! dptr ) {
    return false;
  }
  const unsigned char* maskptr = pData->getMask();

  // prepare data
  Vec3<size_t> dims = pData->getDims();
  size_t dimSz = dims[0] * dims[1] * dims[2];
  if ( dimSz < 1 ) return false;
  m_pd = (float*)ReAllocate(m_pd, sizeof(float)*dimSz);
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
      if ( maskptr && maskptr[i] ) {sptr[i] = 0.f; continue;}
      idx = i * dlen;
      vv.m_v[0] = dptr[idx + m_vecDataIdx.m_v[0]];
      vv.m_v[1] = dptr[idx + m_vecDataIdx.m_v[1]];
      vv.m_v[2] = dptr[idx + m_vecDataIdx.m_v[2]];
      sptr[i] = vv.Length();
    } // end of for(i)
  }
  else {
    for ( i = 0; i < dimSz; i++ ) {
      if ( maskptr && maskptr[i] ) {sptr[i] = dr[0]; continue;}
      idx = i * dlen + m_selectedData -1;
      sptr[i] = dptr[idx];
    } // end of for(i)
  }

  // generate isosurf
  if ( ! vsnIsosurf::IsoSurf(dims.m_v, (const vector3*)gptr,
			     sptr, m_isoValue, *m_isoTrias) ) {
    ErrMsg(MsgERR, getMethodType() + string("[") + getName()
	   + string("]: generate isosurface failed"));
    return false;
  }
  m_isoTrias->generateNormals();
  //m_isoTrias->generateBbox();

  // set color
  updateColor();

  // ok
  m_isoTrias->getPrivateMaterial()->setRenderMode(m_showType);
  setHilight(m_hilight);
  m_updatedStp = m_requestedStp;

  if ( m_autoExport && ! m_exportPath.empty() ) {
    if ( ! exportFaces(m_exportPath) )
      setAutoExport(false);
  }

  return true;
}


/* vsnMethodObj methods */

bool vsnMethod_Sv_isosurf::update(const bool force) {
  return updateStep(m_requestedStp, force);
}

void vsnMethod_Sv_isosurf::reloaded() {
  adjustRange(false);
  vsnMethodObj::reloaded();
}

vsnMethodPP* vsnMethod_Sv_isosurf::getParamPanel(wxPanel* pp) {
  vsnMPP_Sv_isosurf* pp_isosurf
    = new vsnMPP_Sv_isosurf(pp, this);
  if ( ! pp_isosurf ) return NULL;
  return pp_isosurf;
}

void vsnMethod_Sv_isosurf::setBaseColor(const vector4 cv) {
  memcpy(m_colour, cv, sizeof(vector4));
  //m_colour[3] = (float)((int)(m_colour[3] * 10) / 10.f);
  updateUI();
  
  if ( m_pColorBar )
    m_pColorBar->setBaseColor(m_colour);

  updateColor();
}


/* serialize : from vsnIoObject */

bool vsnMethod_Sv_isosurf::parseXML(xmlNodePtr xnp) {
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
      else if ( xsN == string("iso_value") ) {
        float val = (float)atof(xsV.c_str());
        if ( ! setIsoValue(val) ) {
          ErrMsg(MsgERR, msgHdr + string("invalid value in param iso_value"));
          goto _NEXT_XML_NODE;
        }
      } // end of "iso_value"
      else if ( xsN == string("use_cmap") ) {
        bool ucm;
        if ( xsV == string("yes") ) ucm = true;
        else if ( xsV == string("no") ) ucm = false;
        else {
          ErrMsg(MsgERR, msgHdr + string("invalid value in param use_cmap"));
          goto _NEXT_XML_NODE;
        }
        if ( ! setUseCMap(ucm) ) {
          ErrMsg(MsgERR, msgHdr + string("can't update use_cmap mode"));
          goto _NEXT_XML_NODE;
        }
      } // end of "use_cmap"
      else if ( xsN == string("use_cmap_alpha") ) {
        bool ucma;
        if ( xsV == string("yes") ) ucma = true;
        else if ( xsV == string("no") ) ucma = false;
        else {
          ErrMsg(MsgERR, msgHdr
		 + string("invalid value in param use_cmap_alpha"));
          goto _NEXT_XML_NODE;
        }
        if ( ! setUseCMapAlpha(ucma) ) {
          ErrMsg(MsgERR, msgHdr + string("can't update use_cmap_alpha mode"));
          goto _NEXT_XML_NODE;
        }
      } // end of "use_cmap"
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
      else if ( xsN == string("show_face") ) {
	PolyFaceType fm;
	if ( xsV == string("front") ) fm = PF_FRONT;
	else if ( xsV == string("back") ) fm = PF_BACK;
	else if ( xsV == string("both") ) fm = PF_BOTH;
	else {
	  ErrMsg(MsgERR, msgHdr + string("invalid value in param show_face"));
          goto _NEXT_XML_NODE;
	}
	if ( ! setShowFaceMode(fm) ) {
	  ErrMsg(MsgERR, msgHdr + string("can't set show face mode"));
          goto _NEXT_XML_NODE;
	}
      } // end of "show_face"
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
      } // end of "auto_export"
    } // end of param

  _NEXT_XML_NODE:
    cur = cur->next;
  } // end of while(cur)

  return true;
}

bool vsnMethod_Sv_isosurf::outputXML(std::ostream& os, const size_t ts) {
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

  // iso_value
  os << idts_2 << "<param name=\"iso_value\" value=\""
     << m_isoValue << "\" />" << endl;

  // use_cmap / use_cmap_alpha
  if ( ! m_useCMap ) {
    os << idts_2 << "<param name=\"use_cmap\" value=\"no\" />" << endl;
  }
  if ( m_useCMapAlpha ) {
    os << idts_2 << "<param name=\"use_cmap_alpha\" value=\"yes\" />" << endl;
  }

  // show front / back
  if ( m_faceMode != PF_BOTH ) {
    os << idts_2 << "<param name=\"show_face\" value=\"";
    if ( m_faceMode == PF_FRONT )
      os << "front";
    else if ( m_faceMode == PF_BACK )
      os << "back";
    else
      os << "both";
    os << "\" />" << endl;
  }

  // auto export
  if ( m_autoExport ) {
    os << idts_2 << "<param name=\"auto_export\" value=\"yes\" />" << endl;
  }

  // output base method params
  if ( ! vsnMethodObj::exportXMLNode(os, ts+2) ) {
    ErrMsg(MsgERR, msgHdr + string("base MethodObj exportXMLNode failed"));
    ret = false;
  }

  os << idts << "</method>" << endl;
  return ret;
}

bool vsnMethod_Sv_isosurf::commandXML(xmlNodePtr xnp) {
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
  else if ( nameStr == "set_iso_value" ) {
    float val = (float)atof(valueStr.c_str());
    if ( ! setIsoValue(val) ) {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_iso_value: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_iso_value"
  else if ( nameStr == "set_use_cmap" ) {
    bool ucm;
    if ( valueStr == string("yes") ) ucm = true;
    else if ( valueStr == string("no") ) ucm = false;
    else {
      ErrMsg(MsgERR, msgHdr + string("command set_use_cmap: invalid value"));
      return false;
    }
    if ( ! setUseCMap(ucm) ) {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_use_cmap: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_use_cmap"
  else if ( nameStr == "set_use_cmap_alpha" ) {
    bool ucma;
    if ( valueStr == string("yes") ) ucma = true;
    else if ( valueStr == string("no") ) ucma = false;
    else {
      ErrMsg(MsgERR, msgHdr
	     + string("command set_use_cmap_alpha: invalid value"));
      return false;
    }
    if ( ! setUseCMapAlpha(ucma) ) {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_use_cmap_alpha: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_use_cmap_alpha"
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
  else if ( nameStr == "set_show_face" ) {
    PolyFaceType fm;
    if ( valueStr == string("front") ) fm = PF_FRONT;
    else if ( valueStr == string("back") ) fm = PF_BACK;
    else if ( valueStr == string("both") ) fm = PF_BOTH;
    else {
      ErrMsg(MsgERR, msgHdr + string("command set_show_face: invalid value"));
      return false;
    }
    if ( ! setShowFaceMode(fm) ) {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_show_face: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_show_face"
  else if ( nameStr == "export" ) {
    if ( ! exportFaces(valueStr) ) {
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


/* override vfrGroup method */

void vsnMethod_Sv_isosurf::generateBbox() {
  vsnData_Sv* pData = dynamic_cast<vsnData_Sv*>(p_refData);
  if ( ! pData ) return;
  const CES::Vec3<float>* pbb = pData->getBbox();
  if ( ! pbb ) return;
  _bbox[0] = pbb[0];
  _bbox[1] = pbb[1];
}
