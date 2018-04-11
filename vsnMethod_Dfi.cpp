//
// vsnMethod_Dfi
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

#include "vsnMethod_Dfi.h"
#include "vsnData_DfiSv.h"
#include "vsnError.h"
#include <sstream>
#include <algorithm>

using namespace std;
using namespace CES;


//----------------------------------------------------------------
// class vsnMethod_Dfi_info
//----------------------------------------------------------------

/* constructors / destructor */

vsnMethod_Dfi_info::vsnMethod_Dfi_info(const string& name)
  : vsnMethod_info(name)
{
}

vsnMethod_Dfi_info::~vsnMethod_Dfi_info() {
}


/* vsnMethod_info methods */

bool vsnMethod_Dfi_info::update(const bool force) {
  vsnData_Dfi* pdDfi = dynamic_cast<vsnData_Dfi*>(p_refData);
  if ( ! pdDfi ) {
    m_repStr = "out of data";
    return false;
  }

  char dc = vsnPath_getDelimChar();
  char txt[256];
  string& msg = m_repStr;
  msg = "Data = ";
  msg += pdDfi->getDataType();
  msg += "[";
  msg += pdDfi->getName();
  msg += "]\n";
  msg += "Path = ";
  msg += pdDfi->getFilePath() + "\n";
  msg += "\n";

  size_t nstp = pdDfi->m_timeSliceList.size();
  int stp0 = -1, stp1 = -1;
  if ( nstp > 0 ) {
    stp0 = pdDfi->m_timeSliceList[0].step;
    stp1 = pdDfi->m_timeSliceList[nstp -1].step;
  }
  sprintf(txt, " Steps = %lu [%d : %d]\n", nstp, stp0, stp1);
  msg += txt;
  float tms0 = 0.f, tms1 = 0.f;
  if ( nstp > 0 ) {
    tms0 = pdDfi->m_timeSliceList[0].time;
    tms1 = pdDfi->m_timeSliceList[nstp -1].time;
  }
  sprintf(txt, " Time = [%g : %g]\n", tms0, tms1);
  msg += txt;

  Vec3<size_t> dims = pdDfi->m_globalDims;
  sprintf(txt, " GlbDims = %lu x %lu x %lu\n", dims[0], dims[1], dims[2]);
  msg += txt;

  Vec3<size_t> div = pdDfi->m_globalDiv;
  sprintf(txt, " GlbDiv = %lu x %lu x %lu\n", div[0], div[1], div[2]);
  msg += txt;

  Vec3<float> gOrg = pdDfi->m_globalOrig;
  sprintf(txt, " GlbOrig = (%g, %g, %g)\n", gOrg[0], gOrg[1], gOrg[2]);
  msg += txt;

  Vec3<float> gRgn = pdDfi->m_globalRegn;
  sprintf(txt, " GlbRegn = (%g, %g, %g)\n", gRgn[0], gRgn[1], gRgn[2]);
  msg += txt;

  sprintf(txt, " GuideCell = %lu\n", pdDfi->m_numGc);
  msg += txt;

  msg += " Bbox = \n";
  const Vec3<float>* pbb = pdDfi->getBbox();
  sprintf(txt, "   x = [%.3g : %.3g]\n", pbb[0][0], pbb[1][0]);
  msg += txt;
  sprintf(txt, "   y = [%.3g : %.3g]\n", pbb[0][1], pbb[1][1]);
  msg += txt;
  sprintf(txt, "   z = [%.3g : %.3g]\n", pbb[0][2], pbb[1][2]);
  msg += txt;

  size_t dataLen = pdDfi->getDataLen();
  sprintf(txt, " Data Length = %lu\n", dataLen);
  msg += txt;

  if ( dataLen > 0 ) {
    size_t d; float minmax[2];
    for ( d = 0; d < dataLen; d++ ) {
      pdDfi->getMinMax(d, minmax);
      sprintf(txt, " Data%lu\n", d); msg += txt;
      sprintf(txt, "  min = %g\n  max = %g\n", minmax[0], minmax[1]);
      msg += txt;
    } // end of for(d)

    if ( pdDfi->getVectorMaxLen(Vec3<int>(0,1,2), minmax[1]) ) {
      sprintf(txt, " Max vector length = %g\n", minmax[1]);
      msg += txt;
    }
  } // end of if(dataLen>0)

  return true;
}

bool vsnMethod_Dfi_info::updateRepStr() {
  return update();
}


//----------------------------------------------------------------
// class vsnMPP_Dfi_brickLoader
//   method parameter-panel for vsnMethod_Dfi_brickLoader
//----------------------------------------------------------------
BEGIN_EVENT_TABLE(vsnMPP_Dfi_brickLoader, wxPanel)
  EVT_BUTTON(MPP_Dfi_brickLoader_LoadBtn,
	     vsnMPP_Dfi_brickLoader::OnLoadBtn)
  EVT_BUTTON(MPP_Dfi_brickLoader_RegionResetBtn,
	     vsnMPP_Dfi_brickLoader::OnRegionResetBtn)
  EVT_TEXT_ENTER(MPP_Dfi_brickLoader_RegionX1Txt,
                 vsnMPP_Dfi_brickLoader::OnRegionTxt)
  EVT_TEXT_ENTER(MPP_Dfi_brickLoader_RegionX2Txt,
                 vsnMPP_Dfi_brickLoader::OnRegionTxt)
  EVT_TEXT_ENTER(MPP_Dfi_brickLoader_RegionY1Txt,
                 vsnMPP_Dfi_brickLoader::OnRegionTxt)
  EVT_TEXT_ENTER(MPP_Dfi_brickLoader_RegionY2Txt,
                 vsnMPP_Dfi_brickLoader::OnRegionTxt)
  EVT_TEXT_ENTER(MPP_Dfi_brickLoader_RegionZ1Txt,
                 vsnMPP_Dfi_brickLoader::OnRegionTxt)
  EVT_TEXT_ENTER(MPP_Dfi_brickLoader_RegionZ2Txt,
                 vsnMPP_Dfi_brickLoader::OnRegionTxt)
END_EVENT_TABLE()


/* constructors / destructor */

vsnMPP_Dfi_brickLoader::vsnMPP_Dfi_brickLoader(wxPanel* parent,
                                               vsnMethodObj* pm)
: vsnMethodPP(parent, pm)
{
  assert(parent);
  assert(dynamic_cast<vsnMethod_Dfi_brickLoader*>(pm));

  wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer* sizerH;

  // regions
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND|wxALL, 3);
  sizerH->Add(new wxStaticText(this, -1, wxT("load region")),
              0, wxALIGN_LEFT|wxALL, 3);
  m_pRegionResetBtn = new wxButton(this, MPP_Dfi_brickLoader_RegionResetBtn,
                               wxT("reset region"),
                               wxDefaultPosition, wxDefaultSize);
  sizerH->Add(10, 5);
  sizerH->Add(m_pRegionResetBtn, 0, wxALIGN_LEFT|wxALL, 3);

  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND|(wxALL & ~wxTOP), 5);
  sizerH->Add(new wxStaticText(this, -1, wxT("x1")), 0, wxALIGN_LEFT|wxALL, 3);
  m_pRegionX1Txt = new wxTextCtrl(this, MPP_Dfi_brickLoader_RegionX1Txt,
                                  wxT("0"), wxDefaultPosition, wxSize(60,-1),
                                  wxTE_PROCESS_ENTER);
  sizerH->Add(m_pRegionX1Txt, 1, wxEXPAND|wxALL, 3);
  sizerH->Add(new wxStaticText(this, -1, wxT("x2")), 0, wxALIGN_LEFT|wxALL, 3);
  m_pRegionX2Txt = new wxTextCtrl(this, MPP_Dfi_brickLoader_RegionX2Txt,
                                  wxT("1"), wxDefaultPosition, wxSize(60,-1),
                                  wxTE_PROCESS_ENTER);
  sizerH->Add(m_pRegionX2Txt, 1, wxEXPAND|wxALL, 3);

  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND|(wxALL & ~wxTOP), 5);
  sizerH->Add(new wxStaticText(this, -1, wxT("y1")), 0, wxALIGN_LEFT|wxALL, 3);
  m_pRegionY1Txt = new wxTextCtrl(this, MPP_Dfi_brickLoader_RegionY1Txt,
                                  wxT("0"), wxDefaultPosition, wxSize(60,-1),
                                  wxTE_PROCESS_ENTER);
  sizerH->Add(m_pRegionY1Txt, 1, wxEXPAND|wxALL, 3);
  sizerH->Add(new wxStaticText(this, -1, wxT("y2")), 0, wxALIGN_LEFT|wxALL, 3);
  m_pRegionY2Txt = new wxTextCtrl(this, MPP_Dfi_brickLoader_RegionY2Txt,
                                  wxT("1"), wxDefaultPosition, wxSize(60,-1),
                                  wxTE_PROCESS_ENTER);
  sizerH->Add(m_pRegionY2Txt, 1, wxEXPAND|wxALL, 3);

  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND|(wxALL & ~wxTOP), 5);
  sizerH->Add(new wxStaticText(this, -1, wxT("z1")), 0, wxALIGN_LEFT|wxALL, 3);
  m_pRegionZ1Txt = new wxTextCtrl(this, MPP_Dfi_brickLoader_RegionZ1Txt,
                                  wxT("0"), wxDefaultPosition, wxSize(60,-1),
                                  wxTE_PROCESS_ENTER);
  sizerH->Add(m_pRegionZ1Txt, 1, wxEXPAND|wxALL, 3);
  sizerH->Add(new wxStaticText(this, -1, wxT("z2")), 0, wxALIGN_LEFT|wxALL, 3);
  m_pRegionZ2Txt = new wxTextCtrl(this, MPP_Dfi_brickLoader_RegionZ2Txt,
                                  wxT("1"), wxDefaultPosition, wxSize(60,-1),
                                  wxTE_PROCESS_ENTER);
  sizerH->Add(m_pRegionZ2Txt, 1, wxEXPAND|wxALL, 3);

  // load
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND|wxALL, 3);
  m_pLoadBtn = new wxButton(this, MPP_Dfi_brickLoader_LoadBtn,
			    wxT("load bricks"),
			    wxDefaultPosition, wxDefaultSize);
  sizerH->Add(10, 5);
  sizerH->Add(m_pLoadBtn, 1, wxEXPAND|wxALL, 3);

  // post process
  SetSizer(topsizer);
  addTo(parent);
  topsizer->Layout();
  (void)update();
}

vsnMPP_Dfi_brickLoader::~vsnMPP_Dfi_brickLoader() {
}

/* interface */

bool vsnMPP_Dfi_brickLoader::update() {
  if ( ! m_pRegionX1Txt || ! m_pRegionY1Txt || ! m_pRegionZ1Txt ||
       ! m_pRegionX2Txt || ! m_pRegionY2Txt || ! m_pRegionZ2Txt )
    return false;

  vsnMethod_Dfi_brickLoader* pm
    = dynamic_cast<vsnMethod_Dfi_brickLoader*>(p_method);
  if ( ! pm ) return false;
  vsnData_Dfi* pdo = dynamic_cast<vsnData_Dfi*>(pm->getRefData());
  if ( ! pdo ) return false;

  char txt[64];
  Vec3<size_t> region[2];
  if ( pm->getRegion(region) ) {
    sprintf(txt, "%lu", region[0][0]);
    m_pRegionX1Txt->SetValue(vsnApp::ConvSysToWx(txt));
    sprintf(txt, "%lu", region[1][0]);
    m_pRegionX2Txt->SetValue(vsnApp::ConvSysToWx(txt));
    sprintf(txt, "%lu", region[0][1]);
    m_pRegionY1Txt->SetValue(vsnApp::ConvSysToWx(txt));
    sprintf(txt, "%lu", region[1][1]);
    m_pRegionY2Txt->SetValue(vsnApp::ConvSysToWx(txt));
    sprintf(txt, "%lu", region[0][2]);
    m_pRegionZ1Txt->SetValue(vsnApp::ConvSysToWx(txt));
    sprintf(txt, "%lu", region[1][2]);
    m_pRegionZ2Txt->SetValue(vsnApp::ConvSysToWx(txt));
  }

  return true;
}


/* event handler */

void vsnMPP_Dfi_brickLoader::OnRegionResetBtn(wxCommandEvent& event) {
  vsnMethod_Dfi_brickLoader* pm
    = dynamic_cast<vsnMethod_Dfi_brickLoader*>(p_method);
  if ( ! pm ) return;
  vsnData_Dfi* pdo = dynamic_cast<vsnData_Dfi*>(pm->getRefData());
  if ( ! pdo ) return;

  Vec3<size_t> region[2];
  region[1] = pdo->m_globalDiv - Vec3<size_t>(1, 1, 1);
  if ( pm->setRegion(region) ) {
    update();
  }
}

void vsnMPP_Dfi_brickLoader::OnRegionTxt(wxCommandEvent& event) {
  if ( ! m_pRegionX1Txt || ! m_pRegionY1Txt || ! m_pRegionZ1Txt ||
       ! m_pRegionX2Txt || ! m_pRegionY2Txt || ! m_pRegionZ2Txt ) return;
  vsnMethod_Dfi_brickLoader* pm
    = dynamic_cast<vsnMethod_Dfi_brickLoader*>(p_method);
  if ( ! pm ) return;
  vsnData_Dfi* pdo = dynamic_cast<vsnData_Dfi*>(pm->getRefData());
  if ( ! pdo ) return;

  Vec3<size_t> region[2];
  if ( ! pm->getRegion(region) ) return;
  Vec3<size_t> gdiv = pdo->m_globalDiv;

  wxString valStr; long val;
  valStr = m_pRegionX1Txt->GetValue();
  if ( ! valStr.IsEmpty() ) {
    val = atol(vsnApp::ConvWxToSys(valStr).c_str());
    if ( val < 0 ) val = 0;
    else if ( val >= gdiv[0] ) val = gdiv[0] - 1;
    region[0][0] = val;
  }
  valStr = m_pRegionX2Txt->GetValue();
  if ( ! valStr.IsEmpty() ) {
    val = atol(vsnApp::ConvWxToSys(valStr).c_str());
    if ( val < 0 ) val = 0;
    else if ( val >= gdiv[0] ) val = gdiv[0] - 1;
    region[1][0] = val;
  }
  valStr = m_pRegionY1Txt->GetValue();
  if ( ! valStr.IsEmpty() ) {
    val = atol(vsnApp::ConvWxToSys(valStr).c_str());
    if ( val < 0 ) val = 0;
    else if ( val >= gdiv[1] ) val = gdiv[1] - 1;
    region[0][1] = val;
  }
  valStr = m_pRegionY2Txt->GetValue();
  if ( ! valStr.IsEmpty() ) {
    val = atol(vsnApp::ConvWxToSys(valStr).c_str());
    if ( val < 0 ) val = 0;
    else if ( val >= gdiv[1] ) val = gdiv[1] - 1;
    region[1][1] = val;
  }
  valStr = m_pRegionZ1Txt->GetValue();
  if ( ! valStr.IsEmpty() ) {
    val = atol(vsnApp::ConvWxToSys(valStr).c_str());
    if ( val < 0 ) val = 0;
    else if ( val >= gdiv[2] ) val = gdiv[2] - 1;
    region[0][2] = val;
  }
  valStr = m_pRegionZ2Txt->GetValue();
  if ( ! valStr.IsEmpty() ) {
    val = atol(vsnApp::ConvWxToSys(valStr).c_str());
    if ( val < 0 ) val = 0;
    else if ( val >= gdiv[2] ) val = gdiv[2] - 1;
    region[1][2] = val;
  }

  (void)pm->setRegion(region);
  (void)update();
}

void vsnMPP_Dfi_brickLoader::OnLoadBtn(wxCommandEvent& event) {
  vsnMethod_Dfi_brickLoader* pm
    = dynamic_cast<vsnMethod_Dfi_brickLoader*>(p_method);
  if ( ! pm ) return;

  if ( pm->loadRegion() ) {
    pm->chkNotice();

    // select on UiView
    vsnUiView* puiv = getUiView();
    vsnDataObj* pdo = pm->getRefData();
    if ( pdo ) {
      vsnScene* psc = pdo->getAncestorScene();
      if ( psc ) {
	vsnDataObj* pld = psc->getLastLoadedData();
	if ( pld && puiv )
	  puiv->selectObj(pld);
      }
    }
  }
}


//----------------------------------------------------------------
// class vsnMethod_Dfi_brickLoader
//----------------------------------------------------------------

/* constructors / destructor */

vsnMethod_Dfi_brickLoader::vsnMethod_Dfi_brickLoader(const string& name)
  : vsnMethodObj(name), m_loadedDataGrp(NULL), m_loadCnt(0)
{
  alcVerts(2);
  setBboxWidth(3.5f);
  vector4 cv = {0.f, 0.5f, 0.0f, 1.f};
  setBaseColor(cv);

  // loaded group
  m_loadedDataGrp = new vfrGroup("LOADED_DFISV_GRP");
  assert(m_loadedDataGrp);
}

vsnMethod_Dfi_brickLoader::~vsnMethod_Dfi_brickLoader() {
  if ( ! m_loadedDataGrp ) return;
  int n = m_loadedDataGrp->getNumChildren();
  while ( n > 0 ) {
    vfrNode* p = m_loadedDataGrp->getChild(n - 1);
    if ( p ) delete p;
    n = m_loadedDataGrp->getNumChildren();
  } // end of while(n)
  delete m_loadedDataGrp;
}


/* methods */

bool vsnMethod_Dfi_brickLoader::setRegion(const Vec3<size_t>* preg) {
  if ( ! preg ) return false;
  vsnData_Dfi* pdo = dynamic_cast<vsnData_Dfi*>(getRefData());
  if ( ! pdo ) return false;
  Vec3<size_t> gdiv = pdo->m_globalDiv;
 
  if ( m_regionIdx[0][0] == preg[0][0] && m_regionIdx[1][0] == preg[1][0] &&
       m_regionIdx[0][1] == preg[0][1] && m_regionIdx[1][1] == preg[1][1] &&
       m_regionIdx[0][2] == preg[0][2] && m_regionIdx[1][2] == preg[1][2] )
    return true;
  m_regionIdx[0] = preg[0];
  m_regionIdx[1] = preg[1];

  if ( m_regionIdx[0][0] >= gdiv[0] ) m_regionIdx[0][0] = gdiv[0] - 1;
  if ( m_regionIdx[1][0] >= gdiv[0] ) m_regionIdx[1][0] = gdiv[0] - 1;
  if ( m_regionIdx[0][1] >= gdiv[1] ) m_regionIdx[0][1] = gdiv[1] - 1;
  if ( m_regionIdx[1][1] >= gdiv[1] ) m_regionIdx[1][1] = gdiv[1] - 1;
  if ( m_regionIdx[0][2] >= gdiv[2] ) m_regionIdx[0][2] = gdiv[2] - 1;
  if ( m_regionIdx[1][2] >= gdiv[2] ) m_regionIdx[1][2] = gdiv[2] - 1;

  if ( m_regionIdx[0][0] > m_regionIdx[1][0] ) {
    size_t x = m_regionIdx[0][0]; m_regionIdx[0][0] = m_regionIdx[1][0];
    m_regionIdx[1][0] = x;
  }
  if ( m_regionIdx[0][1] > m_regionIdx[1][1] ) {
    size_t x = m_regionIdx[0][1]; m_regionIdx[0][1] = m_regionIdx[1][1];
    m_regionIdx[1][1] = x;
  }
  if ( m_regionIdx[0][2] > m_regionIdx[1][2] ) {
    size_t x = m_regionIdx[0][2]; m_regionIdx[0][2] = m_regionIdx[1][2];
    m_regionIdx[1][2] = x;
  }

  if ( ! update() )
    return false;
  chkNotice();
  return true;
}

bool vsnMethod_Dfi_brickLoader::getRegion(Vec3<size_t>* preg) const {
  if ( ! preg ) return false;
  preg[0] = m_regionIdx[0];
  preg[1] = m_regionIdx[1];
  return true;
}

bool vsnMethod_Dfi_brickLoader::loadRegion(const string& dname) {
  vsnData_Dfi* pdo = dynamic_cast<vsnData_Dfi*>(getRefData());
  if ( ! pdo ) return false;
  vsnScene* psc = pdo->getAncestorScene();
  if ( ! psc ) return false;
  string targScn = psc->getName();
  if ( targScn.empty() || targScn == VFR_NONAME )
    return false;
  string msgHdr = "Dfi_brickLoader: loadRegion: ";

  // check the same region has loaded
  register int i, n = m_loadedDataGrp->getNumChildren();
  for ( i = 0; i < n; i++ ) {
    vsnData_DfiSv* d
      = dynamic_cast<vsnData_DfiSv*>(m_loadedDataGrp->getChild(i));
    if ( ! d ) continue;
    const Vec3<size_t>* dregIdx = d->getRegionIdx();
    if ( m_regionIdx[0][0] == dregIdx[0][0] &&
	 m_regionIdx[0][1] == dregIdx[0][1] &&
	 m_regionIdx[0][2] == dregIdx[0][2] &&
	 m_regionIdx[1][0] == dregIdx[1][0] &&
	 m_regionIdx[1][1] == dregIdx[1][1] &&
	 m_regionIdx[1][2] == dregIdx[1][2] ) {
      ErrMsg(MsgERR, msgHdr+string("the same region data has already loaded."));
      return false;
    }
  } // end of for(i)

  stringstream ss;
  string xdname = dname;
  if ( xdname.empty() ) {
    xdname = pdo->getName();
    ss << m_loadCnt++;
    xdname += string(".DfiSv_") + ss.str();
    ss.str("");
  }

  string cmd = string("<command target=\"") + targScn
    + string("\" name=\"import_data\">\n");
  cmd += string("<data type=\"DfiSv\" name=\"") + xdname + string("\">\n");
  cmd += string("<brick dfi=\"") + pdo->getName() + string("\">\n");
  ss << "x=\"" << m_regionIdx[0][0] << "\" y=\"" << m_regionIdx[0][1]
     << "\" z=\"" << m_regionIdx[0][2] << "\"";
  cmd += string("<head ") + ss.str() + string(" />\n");
  ss.str("");
  ss << "x=\"" << m_regionIdx[1][0] << "\" y=\"" << m_regionIdx[1][1]
     << "\" z=\"" << m_regionIdx[1][2] << "\"";
  cmd += string("<tail ") + ss.str() + string(" />\n");
  ss.str("");
  cmd += string("</brick>\n</data>\n");
  cmd += string("</command>\n");
  if ( ! vsnApp::GetApp()->parseXMLCommand(cmd) ) {
    return false;
  }

  addloadedDataRef(dynamic_cast<vsnData_DfiSv*>(psc->getLastLoadedData()));
  return true;
}

bool vsnMethod_Dfi_brickLoader::addloadedDataRef(vsnData_DfiSv* pd) {
  if ( ! pd || ! m_loadedDataGrp ) return false;
  if ( ! m_loadedDataGrp->addChild(pd) ) return false;
  return true;
}


/* from vsnMethodObj */

void vsnMethod_Dfi_brickLoader::setRefData(vsnDataObj* prd) {
  if ( p_refData == prd ) return;

  vsnData_Dfi* pdo = dynamic_cast<vsnData_Dfi*>(prd);
  if ( ! pdo ) return;
  p_refData = prd;

  m_regionIdx[0] = Vec3<size_t>(0, 0, 0);
  m_regionIdx[1] = pdo->m_globalDiv - Vec3<size_t>(1, 1, 1);
  update();
}

void vsnMethod_Dfi_brickLoader::setBaseColor(const vector4 cv) {
  setBboxColor(cv);
  vsnMethodObj::setBaseColor(cv);
}

bool vsnMethod_Dfi_brickLoader::update(const bool force) {
  vsnData_Dfi* pdo = dynamic_cast<vsnData_Dfi*>(getRefData());
  if ( ! pdo ) return false;

  setBboxShowMode(FALSE);
  if ( ! m_show ) {
    return true;
  }

  Vec3<float> pit = pdo->getPitch();
  Vec3<float> half_pit = pit * 0.5f;
  Vec3<float> gOrig = pdo->m_globalOrig + half_pit;
  Vec3<float> gRegn = pdo->m_globalRegn - pit;
  const Vec3<size_t>& gDiv = pdo->m_globalDiv;
  const Vec3<size_t>& gDims = pdo->m_globalDims;
  size_t headIdx = gDiv[0]*gDiv[1]*m_regionIdx[0][2]
    + gDiv[0]*m_regionIdx[0][1] + m_regionIdx[0][0];
  size_t tailIdx = gDiv[0]*gDiv[1]*m_regionIdx[1][2]
    + gDiv[0]*m_regionIdx[1][1] + m_regionIdx[1][0];

  // sort brickList
  deque<vsnData_Dfi::BrickInfo> brickList = pdo->m_brickList;
  std::sort(brickList.begin(), brickList.end());
  
  vsnData_Dfi::BrickInfo& headBi = brickList[headIdx];
  vsnData_Dfi::BrickInfo& tailBi = brickList[tailIdx];
  _verts[0][0] = gOrig[0] + pit[0] * headBi.headIdx[0];
  _verts[0][1] = gOrig[1] + pit[1] * headBi.headIdx[1];
  _verts[0][2] = gOrig[2] + pit[2] * headBi.headIdx[2];
  _verts[1][0] = gOrig[0]
    + pit[0] * (tailBi.headIdx[0] +tailBi.voxelSize[0] -1);
  _verts[1][1] = gOrig[1]
    + pit[1] * (tailBi.headIdx[1] +tailBi.voxelSize[1] -1);
  _verts[1][2] = gOrig[2]
    + pit[2] * (tailBi.headIdx[2] +tailBi.voxelSize[2] -1);
  vfrNode::generateBbox();

  setBboxShowMode(TRUE);
  setBboxColor(m_colour);
  return true;
}

vsnMethodPP* vsnMethod_Dfi_brickLoader::getParamPanel(wxPanel* pp) {
  vsnMPP_Dfi_brickLoader* pp_brickLoader
    = new vsnMPP_Dfi_brickLoader(pp, this);
  if ( ! pp_brickLoader ) return NULL;
  return pp_brickLoader;
}


/* vsnIoObject */

struct LoadedBrick {
  string name;
  Vec3<size_t> regIdx[2];
};

bool vsnMethod_Dfi_brickLoader::parseXML(xmlNodePtr xnp) {
  static xmlChar* xs;
  xmlNodePtr cur;

  if ( ! vsnMethodObj::parseXML(xnp) ) return false;

  Vec3<size_t> regIdx[2];
  deque<LoadedBrick> loadedBrickList;

  // get children node
  cur = xnp->xmlChildrenNode;

  // traverse
  while ( cur ) {
    if ( (cur = vsnIoObject::SkipCommentXML(cur)) == NULL ) break;

    if ( !strcmp((const char*)cur->name, "param") ) {
      string xsN;
      if ( xs ) xmlFree(xs);
      xs = xmlGetProp(cur, (const xmlChar*)"name");
      if ( ! xs || strlen((const char*)xs) < 1 ) goto _NEXT_XML_NODE;
      xsN = (const char*)xs;

      if ( xsN == string("region") ) {
	xmlNodePtr cur2 = cur->xmlChildrenNode;
	while ( cur2 ) {
	  if ( (cur2 = vsnIoObject::SkipCommentXML(cur2)) == NULL )
	    break;
	  if ( !strcmp((const char*)cur2->name, "head") ) {
	    regIdx[0] = vsnData_DfiSv::ParseAttrXYZ(cur2);
	  }
	  else if ( !strcmp((const char*)cur2->name, "tail") ) {
	    regIdx[1] = vsnData_DfiSv::ParseAttrXYZ(cur2);
	  }
	  cur2 = cur2->next;
	} // end of while(cur2)
      } // end of region
    } // end of param

  _NEXT_XML_NODE:
    cur = cur->next;
  } // end of while(cur)

  // load bricks
  deque<LoadedBrick>::iterator bit = loadedBrickList.begin();
  for ( ; bit != loadedBrickList.end(); bit++ ) {
    m_regionIdx[0] = bit->regIdx[0];
    m_regionIdx[1] = bit->regIdx[1];
    (void)loadRegion(bit->name);
  } // end of for(bit)

  // at the last, set region param
  (void)setRegion(regIdx);

  return true;
}

bool vsnMethod_Dfi_brickLoader::outputXML(ostream& os, const size_t ts) {
  std::string idts;
  for ( int i = 0; i < ts; i++ ) idts.push_back(' ');
  string idts_2 = idts; idts_2.push_back(' '); idts_2.push_back(' ');
  string idts_4 = idts_2; idts_4.push_back(' '); idts_4.push_back(' ');
  bool ret= true;
  string msgHdr
    = getMethodType() +string("[") +getName() +string("]: outputXML: ");

  // output
  os << idts << "<method type=\"" << getMethodType() << "\"";
  if ( !_name.empty() && _name != string(VFR_NONAME) )
    os << " name=\"" << _name << "\"";
  os << " >" << endl;

  // output original params
  // region
  os << idts_2 << "<param name=\"region\">" << endl;
  os << idts_4 << "<head x=\"" << m_regionIdx[0][0] << "\" y=\""
     << m_regionIdx[0][1] << "\" z=\"" << m_regionIdx[0][2] << "\" />\n";
  os << idts_4 << "<tail x=\"" << m_regionIdx[1][0] << "\" y=\""
     << m_regionIdx[1][1] << "\" z=\"" << m_regionIdx[1][2] << "\" />\n";
  os << idts_2 << "</param>" << endl;

  // output base method params
  if ( ! vsnMethodObj::exportXMLNode(os, ts +2) ) {
    ErrMsg(MsgERR, msgHdr + string("base MethodObj exportXMLNode failed"));
    ret = false;
  }

  os << idts << "</method>" << endl;
  return true;
}

bool vsnMethod_Dfi_brickLoader::commandXML(xmlNodePtr xnp) {
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
  if ( valueStr.empty() ) {
    ErrMsg(MsgERR, msgHdr + "invalid command: " + nameStr + ": no value");
    return false;
  }

  // do the command
  Vec3<size_t> regIdx[2]; int xyz[3];
  regIdx[0] = m_regionIdx[0]; regIdx[1] = m_regionIdx[1];

  if ( nameStr == "set_region_head" ) {
    int nscan = sscanf(valueStr.c_str(), "%d %d %d", &xyz[0], &xyz[1], &xyz[2]);
    if ( nscan < 3 || xyz[0] < 0 || xyz[1] < 0 || xyz[2] < 0 ) {
      ErrMsg(MsgERR, msgHdr +
             string("command set_region_head: invalid value: ") + valueStr);
      return false;
    }
    regIdx[0][0] = xyz[0]; regIdx[0][1] = xyz[1]; regIdx[0][2] = xyz[2];
    if ( ! setRegion(regIdx) ) {
      ErrMsg(MsgERR, msgHdr +
             string("command set_region_head: setRegion failed: ") + valueStr);
      return false;
    }
  }
  else if ( nameStr == "set_region_tail" ) {
    int nscan = sscanf(valueStr.c_str(), "%d %d %d", &xyz[0], &xyz[1], &xyz[2]);
    if ( nscan < 3 || xyz[0] < 0 || xyz[1] < 0 || xyz[2] < 0 ) {
      ErrMsg(MsgERR, msgHdr +
             string("command set_region_tail: invalid value: ") + valueStr);
      return false;
    }
    regIdx[1][0] = xyz[0]; regIdx[1][1] = xyz[1]; regIdx[1][2] = xyz[2];
    if ( ! setRegion(regIdx) ) {
      ErrMsg(MsgERR, msgHdr +
             string("command set_region_tail: setRegion failed: ") + valueStr);
      return false;
    }
  }
  else if ( nameStr == "load_region" ) {
    if ( ! loadRegion(valueStr) ) {
      return false;
    }
  }
  else {
    // not my command
    ErrMsg(MsgERR, msgHdr + string("unknown command: ") + nameStr);
    return false;
  }

  return true;
}

