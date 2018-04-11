//
// vsnMethod_Sv_crop
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

//#include "StdAfx.h"
#include "vsnMethod_Sv_crop.h"
#include "vsnOctTree.h" // for decomp probeIdx
#include "vsnApp.h"
#include "vsnError.h"

using namespace std;
using namespace CES;
using namespace VSN;


//----------------------------------------------------------------
// class vsnMPP_Sv_crop
//----------------------------------------------------------------

BEGIN_EVENT_TABLE(vsnMPP_Sv_crop, wxPanel)
  EVT_BUTTON(MPP_Sv_crop_ApplyBtn, vsnMPP_Sv_crop::OnApplyBtn)
  EVT_BUTTON(MPP_Sv_crop_NoneBtn, vsnMPP_Sv_crop::OnNoneBtn)
  EVT_COMMAND_SCROLL(MPP_Sv_crop_X1Sld, vsnMPP_Sv_crop::OnX1Sld)
  EVT_COMMAND_SCROLL(MPP_Sv_crop_X2Sld, vsnMPP_Sv_crop::OnX2Sld)
  EVT_COMMAND_SCROLL(MPP_Sv_crop_Y1Sld, vsnMPP_Sv_crop::OnY1Sld)
  EVT_COMMAND_SCROLL(MPP_Sv_crop_Y2Sld, vsnMPP_Sv_crop::OnY2Sld)
  EVT_COMMAND_SCROLL(MPP_Sv_crop_Z1Sld, vsnMPP_Sv_crop::OnZ1Sld)
  EVT_COMMAND_SCROLL(MPP_Sv_crop_Z2Sld, vsnMPP_Sv_crop::OnZ2Sld)
  EVT_TEXT_ENTER(MPP_Sv_crop_X1Txt, vsnMPP_Sv_crop::OnX1Txt)
  EVT_TEXT_ENTER(MPP_Sv_crop_X2Txt, vsnMPP_Sv_crop::OnX2Txt)
  EVT_TEXT_ENTER(MPP_Sv_crop_Y1Txt, vsnMPP_Sv_crop::OnY1Txt)
  EVT_TEXT_ENTER(MPP_Sv_crop_Y2Txt, vsnMPP_Sv_crop::OnY2Txt)
  EVT_TEXT_ENTER(MPP_Sv_crop_Z1Txt, vsnMPP_Sv_crop::OnZ1Txt)
  EVT_TEXT_ENTER(MPP_Sv_crop_Z2Txt, vsnMPP_Sv_crop::OnZ2Txt)
END_EVENT_TABLE()


vsnMPP_Sv_crop::vsnMPP_Sv_crop(wxPanel* parent, vsnMethodObj* pm)
  : vsnMethodPP(parent, pm),
    m_pX1Sld(NULL), m_pX2Sld(NULL), m_pY1Sld(NULL), m_pY2Sld(NULL),
    m_pZ1Sld(NULL), m_pZ2Sld(NULL), m_pX1Txt(NULL), m_pX2Txt(NULL),
    m_pY1Txt(NULL), m_pY2Txt(NULL), m_pZ1Txt(NULL), m_pZ2Txt(NULL),
    m_pApplyBtn(NULL), m_pNoneBtn(NULL)
{
  assert(parent);
  assert(dynamic_cast<vsnMethod_Sv_crop*>(pm));

  // slider, text
  m_pX1Sld = new wxSlider(this, MPP_Sv_crop_X1Sld, 
			  20000 /* val */, 10000 /* min */, 30000 /* max */,
			  wxDefaultPosition, wxSize(120, -1),
			  wxSL_HORIZONTAL|wxSL_LABELS);
  assert(m_pX1Sld);

  m_pX1Txt = new wxTextCtrl(this, MPP_Sv_crop_X1Txt, wxT(""),
			    wxDefaultPosition, wxDefaultSize,
			    wxTE_PROCESS_ENTER);
  assert(m_pX1Txt);

  m_pX2Sld = new wxSlider(this, MPP_Sv_crop_X2Sld, 
			  20000 /* val */, 10000 /* min */, 30000 /* max */,
			  wxDefaultPosition, wxSize(120, -1),
			  wxSL_HORIZONTAL|wxSL_LABELS);
  assert(m_pX2Sld);

  m_pX2Txt = new wxTextCtrl(this, MPP_Sv_crop_X2Txt, wxT(""),
			    wxDefaultPosition, wxDefaultSize,
			    wxTE_PROCESS_ENTER);
  assert(m_pX2Txt);

  m_pY1Sld = new wxSlider(this, MPP_Sv_crop_Y1Sld,
			  20000 /* val */, 10000 /* min */, 30000 /* max */,
			  wxDefaultPosition, wxSize(120, -1),
			  wxSL_HORIZONTAL|wxSL_LABELS);
  assert(m_pY1Sld);

  m_pY1Txt = new wxTextCtrl(this, MPP_Sv_crop_Y1Txt, wxT(""),
			    wxDefaultPosition, wxDefaultSize,
			    wxTE_PROCESS_ENTER);
  assert(m_pY1Txt);

  m_pY2Sld = new wxSlider(this, MPP_Sv_crop_Y2Sld, 
			  20000 /* val */, 10000 /* min */, 30000 /* max */,
			  wxDefaultPosition, wxSize(120, -1),
			  wxSL_HORIZONTAL|wxSL_LABELS);
  assert(m_pY2Sld);

  m_pY2Txt = new wxTextCtrl(this, MPP_Sv_crop_Y2Txt, wxT(""),
			    wxDefaultPosition, wxDefaultSize,
			    wxTE_PROCESS_ENTER);
  assert(m_pY2Txt);

  m_pZ1Sld = new wxSlider(this, MPP_Sv_crop_Z1Sld, 
			  20000 /* val */, 10000 /* min */, 30000 /* max */,
			  wxDefaultPosition, wxSize(120, -1),
			  wxSL_HORIZONTAL|wxSL_LABELS);
  assert(m_pZ1Sld);

  m_pZ1Txt = new wxTextCtrl(this, MPP_Sv_crop_Z1Txt, wxT(""),
			    wxDefaultPosition, wxDefaultSize,
			    wxTE_PROCESS_ENTER);
  assert(m_pZ1Txt);

  m_pZ2Sld = new wxSlider(this, MPP_Sv_crop_Z2Sld, 
			  20000 /* val */, 10000 /* min */, 30000 /* max */,
			  wxDefaultPosition, wxSize(120, -1),
			  wxSL_HORIZONTAL|wxSL_LABELS);
  assert(m_pZ2Sld);

  m_pZ2Txt = new wxTextCtrl(this, MPP_Sv_crop_Z2Txt, wxT(""),
			    wxDefaultPosition, wxDefaultSize,
			    wxTE_PROCESS_ENTER);
  assert(m_pZ2Txt);

  // button
  m_pApplyBtn = new wxButton(this, MPP_Sv_crop_ApplyBtn, wxT("Apply"),
			     wxDefaultPosition, wxSize(65,-1), wxBU_EXACTFIT);
  assert(m_pApplyBtn);

  m_pNoneBtn = new wxButton(this, MPP_Sv_crop_NoneBtn, wxT("None"),
			    wxDefaultPosition, wxSize(65,-1), wxBU_EXACTFIT);
  assert(m_pNoneBtn);

  // top sizer
  wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer* sizerH;

  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND);
  sizerH->Add(new wxStaticText(this, -1, wxT("x1")),
              0, wxEXPAND|wxALL, 3);
  sizerH->Add(m_pX1Sld, 0, wxEXPAND|wxALL, 3);
  sizerH->Add(m_pX1Txt, 0, wxEXPAND|wxALL, 3);

  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND);
  sizerH->Add(new wxStaticText(this, -1, wxT("x2")),
              0, wxEXPAND|wxALL, 3);
  sizerH->Add(m_pX2Sld, 0, wxEXPAND|wxALL, 3);
  sizerH->Add(m_pX2Txt, 0, wxEXPAND|wxALL, 3);

  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND);
  sizerH->Add(new wxStaticText(this, -1, wxT("y1")),
              0, wxEXPAND|wxALL, 3);
  sizerH->Add(m_pY1Sld, 0, wxEXPAND|wxALL, 3);
  sizerH->Add(m_pY1Txt, 0, wxEXPAND|wxALL, 3);

  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND);
  sizerH->Add(new wxStaticText(this, -1, wxT("y2")),
              0, wxEXPAND|wxALL, 3);
  sizerH->Add(m_pY2Sld, 0, wxEXPAND|wxALL, 3);
  sizerH->Add(m_pY2Txt, 0, wxEXPAND|wxALL, 3);

  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND);
  sizerH->Add(new wxStaticText(this, -1, wxT("z1")),
              0, wxEXPAND|wxALL, 3);
  sizerH->Add(m_pZ1Sld, 0, wxEXPAND|wxALL, 3);
  sizerH->Add(m_pZ1Txt, 0, wxEXPAND|wxALL, 3);

  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND);
  sizerH->Add(new wxStaticText(this, -1, wxT("z2")),
              0, wxEXPAND|wxALL, 3);
  sizerH->Add(m_pZ2Sld, 0, wxEXPAND|wxALL, 3);
  sizerH->Add(m_pZ2Txt, 0, wxEXPAND|wxALL, 3);

  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND);
  sizerH->Add(m_pApplyBtn, 0, wxEXPAND|wxALL, 3);
  sizerH->Add(m_pNoneBtn, 0, wxEXPAND|wxALL, 3);

  // post process
  SetSizer(topsizer);
  addTo(parent);
  topsizer->Layout();
  (void)update();
}

vsnMPP_Sv_crop::~vsnMPP_Sv_crop()
{
}


/* interface */

bool vsnMPP_Sv_crop::update() {
  if ( ! m_pX1Sld || ! m_pX1Txt || ! m_pX2Sld || ! m_pX2Txt ||
       ! m_pY1Sld || ! m_pY1Txt || ! m_pY2Sld || ! m_pY2Txt ||
       ! m_pZ1Sld || ! m_pZ1Txt || ! m_pZ2Sld || ! m_pZ2Txt ||
       ! m_pApplyBtn || ! m_pNoneBtn )
    return false;
  int val[2]; char txt[64];

  vsnMethod_Sv_crop* pm
    = dynamic_cast<vsnMethod_Sv_crop*>(p_method);
  if ( ! pm ) return false;
  vsnData_Sv* pdata = dynamic_cast<vsnData_Sv*>(pm->getRefData());
  if ( ! pdata ) return false;

  // range
  Vec3<size_t> dims = pdata->getDims();
  m_pX1Sld->SetRange(0, (int)dims[0]-1);
  m_pX2Sld->SetRange(0, (int)dims[0]-1);

  m_pY1Sld->SetRange(0, (int)dims[1]-1);
  m_pY2Sld->SetRange(0, (int)dims[1]-1);

  m_pZ1Sld->SetRange(0, (int)dims[2]-1);
  m_pZ2Sld->SetRange(0, (int)dims[2]-1);

  // paramater
  // x
  pm->getXValue(val);
  m_pX1Sld->SetValue(val[0]);
  m_pX2Sld->SetValue(val[1]);

  sprintf(txt, "%d", val[0]); m_pX1Txt->SetValue(vsnApp::ConvSysToWx(txt));
  sprintf(txt, "%d", val[1]); m_pX2Txt->SetValue(vsnApp::ConvSysToWx(txt));

  // y
  pm->getYValue(val);
  m_pY1Sld->SetValue(val[0]);
  m_pY2Sld->SetValue(val[1]);

  sprintf(txt, "%d", val[0]); m_pY1Txt->SetValue(vsnApp::ConvSysToWx(txt));
  sprintf(txt, "%d", val[1]); m_pY2Txt->SetValue(vsnApp::ConvSysToWx(txt));

  // z
  pm->getZValue(val);
  m_pZ1Sld->SetValue(val[0]);
  m_pZ2Sld->SetValue(val[1]);

  sprintf(txt, "%d", val[0]); m_pZ1Txt->SetValue(vsnApp::ConvSysToWx(txt));
  sprintf(txt, "%d", val[1]); m_pZ2Txt->SetValue(vsnApp::ConvSysToWx(txt));

  return true;
}


/* event handler */

void vsnMPP_Sv_crop::OnApplyBtn(wxCommandEvent& event) {
  if ( ! m_pApplyBtn ) return;
  vsnMethod_Sv_crop* pm
    = dynamic_cast<vsnMethod_Sv_crop*>(p_method);
  if ( ! pm ) return;

  if( pm->setMask(true) )
    pm->chkNotice();
}

void vsnMPP_Sv_crop::OnNoneBtn(wxCommandEvent& event) {
  if ( ! m_pNoneBtn ) return;
  vsnMethod_Sv_crop* pm
    = dynamic_cast<vsnMethod_Sv_crop*>(p_method);
  if ( ! pm ) return;

  if( pm->setMask(false) )
    pm->chkNotice();
}

void vsnMPP_Sv_crop::OnX1Sld(wxScrollEvent& event) {
  if ( ! m_pX1Sld ) return;
  vsnMethod_Sv_crop* pm
    = dynamic_cast<vsnMethod_Sv_crop*>(p_method);
  if ( ! pm ) return;

  int val = m_pX1Sld->GetValue();
  int x[2]; pm->getXValue(x);
  if ( val == x[0] ) return;

  pm->setParam(pm->X1, val);
  (void)update();
}

void vsnMPP_Sv_crop::OnX2Sld(wxScrollEvent& event) {
  if ( ! m_pX2Sld ) return;
  vsnMethod_Sv_crop* pm
    = dynamic_cast<vsnMethod_Sv_crop*>(p_method);
  if ( ! pm ) return;

  int val = m_pX2Sld->GetValue();
  int x[2]; pm->getXValue(x);
  if ( val == x[1] ) return;

  pm->setParam(pm->X2, val);
  (void)update();
}

void vsnMPP_Sv_crop::OnY1Sld(wxScrollEvent& event) {
  if ( ! m_pY1Sld ) return;
  vsnMethod_Sv_crop* pm
    = dynamic_cast<vsnMethod_Sv_crop*>(p_method);
  if ( ! pm ) return;

  int val = m_pY1Sld->GetValue();
  int y[2]; pm->getYValue(y);
  if ( val == y[0] ) return;

  pm->setParam(pm->Y1, val);
  (void)update();
}

void vsnMPP_Sv_crop::OnY2Sld(wxScrollEvent& event) {
  if ( ! m_pY2Sld ) return;
  vsnMethod_Sv_crop* pm
    = dynamic_cast<vsnMethod_Sv_crop*>(p_method);
  if ( ! pm ) return;

  int val = m_pY2Sld->GetValue();
  int y[2]; pm->getYValue(y);
  if ( val == y[1] ) return;

  pm->setParam(pm->Y2, val);
  (void)update();
}

void vsnMPP_Sv_crop::OnZ1Sld(wxScrollEvent& event) {
  if ( ! m_pZ1Sld ) return;
  vsnMethod_Sv_crop* pm
    = dynamic_cast<vsnMethod_Sv_crop*>(p_method);
  if ( ! pm ) return;

  int val = m_pZ1Sld->GetValue();
  int z[2]; pm->getZValue(z);
  if ( val == z[0] ) return;

  pm->setParam(pm->Z1, val);
  (void)update();
}

void vsnMPP_Sv_crop::OnZ2Sld(wxScrollEvent& event) {
  if ( ! m_pZ2Sld ) return;
  vsnMethod_Sv_crop* pm
    = dynamic_cast<vsnMethod_Sv_crop*>(p_method);
  if ( ! pm ) return;

  int val = m_pZ2Sld->GetValue();
  int z[2]; pm->getZValue(z);
  if ( val == z[1] ) return;

  pm->setParam(pm->Z2, val);
  (void)update();
}

void vsnMPP_Sv_crop::OnX1Txt(wxCommandEvent& event) {
  if ( ! m_pX1Txt ) return;
  vsnMethod_Sv_crop* pm
    = dynamic_cast<vsnMethod_Sv_crop*>(p_method);
  if ( ! pm ) return;

  wxString valStr = m_pX1Txt->GetValue();
  if ( valStr.IsEmpty() ) return;
  int val = atoi(vsnApp::ConvWxToSys(valStr).c_str());
  if ( val >= 0 && val <= m_pX1Sld->GetMax() )
    pm->setParam(pm->X1, val);

  (void)update();
}

void vsnMPP_Sv_crop::OnX2Txt(wxCommandEvent& event) {
  if ( ! m_pX2Txt ) return;
  vsnMethod_Sv_crop* pm
    = dynamic_cast<vsnMethod_Sv_crop*>(p_method);
  if ( ! pm ) return;

  wxString valStr = m_pX2Txt->GetValue();
  if ( valStr.IsEmpty() ) return;
  int val = atoi(vsnApp::ConvWxToSys(valStr).c_str());
  if ( val >= 0 && val <= m_pX2Sld->GetMax() )
    pm->setParam(pm->X2, val);
  
  (void)update();
}

void vsnMPP_Sv_crop::OnY1Txt(wxCommandEvent& event) {
  if ( ! m_pY1Txt ) return;
  vsnMethod_Sv_crop* pm
    = dynamic_cast<vsnMethod_Sv_crop*>(p_method);
  if ( ! pm ) return;

  wxString valStr = m_pY1Txt->GetValue();
  if ( valStr.IsEmpty() ) return;
  int val = atoi(vsnApp::ConvWxToSys(valStr).c_str());
  if ( val >= 0 || val <= m_pY1Sld->GetMax() )
    pm->setParam(pm->Y1, val);
  
  (void)update();
}

void vsnMPP_Sv_crop::OnY2Txt(wxCommandEvent& event) {
  if ( ! m_pY2Txt ) return;
  vsnMethod_Sv_crop* pm
    = dynamic_cast<vsnMethod_Sv_crop*>(p_method);
  if ( ! pm ) return;

  wxString valStr = m_pY2Txt->GetValue();
  if ( valStr.IsEmpty() ) return;
  int val = atoi(vsnApp::ConvWxToSys(valStr).c_str());
  if ( val >= 0 || val <= m_pY2Sld->GetMax() )
    pm->setParam(pm->Y2, val);

  (void)update();
}

void vsnMPP_Sv_crop::OnZ1Txt(wxCommandEvent& event) {
  if ( ! m_pZ1Txt ) return;
  vsnMethod_Sv_crop* pm
    = dynamic_cast<vsnMethod_Sv_crop*>(p_method);
  if ( ! pm ) return;

  wxString valStr = m_pZ1Txt->GetValue();
  if ( valStr.IsEmpty() ) return;
  int val = atoi(vsnApp::ConvWxToSys(valStr).c_str());
  if ( val >= 0 || val <= m_pZ1Sld->GetMax() )
    pm->setParam(pm->Z1, val);
    
  (void)update();
}

void vsnMPP_Sv_crop::OnZ2Txt(wxCommandEvent& event) {
  if ( ! m_pZ2Txt ) return;
  vsnMethod_Sv_crop* pm
    = dynamic_cast<vsnMethod_Sv_crop*>(p_method);
  if ( ! pm ) return;

  wxString valStr = m_pZ2Txt->GetValue();
  if ( valStr.IsEmpty() ) return;
  int val = atoi(vsnApp::ConvWxToSys(valStr).c_str());
  if ( val >= 0 || val <= m_pZ2Sld->GetMax() )
    pm->setParam(pm->Z2, val);

  (void)update();
}


//----------------------------------------------------------------
// class vsnMethod_Sv_crop
//----------------------------------------------------------------

/* constructors / destructor */

vsnMethod_Sv_crop::vsnMethod_Sv_crop(const std::string& name)
  : vsnMethodObj(name)
{
  m_x1 = m_x2 = 0;
  m_y1 = m_y2 = 0;
  m_z1 = m_z2 = 0;
}

vsnMethod_Sv_crop::~vsnMethod_Sv_crop()
{
  vsnData_Sv* pdata = dynamic_cast<vsnData_Sv*>(getRefData());
  if ( pdata && pdata->getMask() ) {
    pdata->setMask(false, 0, 0, 0, 0, 0, 0);
  }
}


/* vsnMethodObj methods */

bool vsnMethod_Sv_crop::update(const bool force) {
  // paramater
  return setParam();
}

vsnMethodPP* vsnMethod_Sv_crop::getParamPanel(wxPanel* pp) {
  vsnMPP_Sv_crop* pp_crop = new vsnMPP_Sv_crop(pp, this);
  if ( ! pp_crop ) return NULL;
  return pp_crop;
}

bool vsnMethod_Sv_crop::setParam(const ParamType type, const int val)
{
  vsnData_Sv* pdata = dynamic_cast<vsnData_Sv*>(getRefData());
  if ( ! pdata ) return false;

  Vec3<size_t> dims = pdata->getDims();
  switch ( type ) {
  case NoneNone:
    if ( m_x2 == 0 ) m_x2 = (int)dims[0] - 1;
    if ( m_y2 == 0 ) m_y2 = (int)dims[1] - 1;
    if ( m_z2 == 0 ) m_z2 = (int)dims[2] - 1;
    break;
  case X1:
    if ( val < 0 || val >= m_x2 ) return false;
    m_x1 = val;
    break;
  case X2:
    if ( val <= m_x1 || val >= (int)dims[0] ) return false;
    m_x2 = val;
    break;
  case Y1:
    if ( val < 0 || val >= m_y2 ) return false;
    m_y1 = val;
    break;
  case Y2:
    if ( val <= m_y1 || val >= (int)dims[1] ) return false;
    m_y2 = val;
    break;
  case Z1:
    if ( val < 0 || val >= m_z2 ) return false;
    m_z1 = val;
    break;
  case Z2:
    if ( val <= m_z1 || val >= (int)dims[2] ) return false;
    m_z2 = val;
    break;
  }

  return true;
}

bool vsnMethod_Sv_crop::setMask(const bool apply)
{
  bool res = true;
  vsnData_Sv* pdata = dynamic_cast<vsnData_Sv*>(getRefData());
  if ( ! pdata ) return false;

  res = pdata->setMask(apply, m_x1, m_x2, m_y1, m_y2, m_z1, m_z2);

  return res;
}

bool vsnMethod_Sv_crop::setMask(const bool apply, 
                                const int x1, const int x2,
                                const int y1, const int y2,
                                const int z1, const int z2)
{
  bool res = true;
  vsnData_Sv* pdata = dynamic_cast<vsnData_Sv*>(getRefData());
  if ( ! pdata ) return false;

  res = pdata->setMask(apply, x1, x2, y1, y2, z1, z2);

  if ( res ) {
    m_x1 = x1; m_x2 = x2;
    m_y1 = y1; m_y2 = y2;
    m_z1 = z1; m_z2 = z2;
  }

  return res;
}


/* serialize methods */

bool vsnMethod_Sv_crop::parseXML(xmlNodePtr xnp) {
  static xmlChar* xs;
  xmlNodePtr cur;

  vsnData_Sv* pdata = dynamic_cast<vsnData_Sv*>(getRefData());
  if ( ! pdata ) return false;
  Vec3<size_t> dims = pdata->getDims();
  int x1(0), x2(dims[0]-1);
  int y1(0), y2(dims[1]-1);
  int z1(0), z2(dims[2]-1);
  bool mode = false;

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

      if ( xsN == string("x1") ) {
	x1 = atoi(xsV.c_str()); mode = true;
      } else if ( xsN == string("x2") ) {
	x2 = atoi(xsV.c_str()); mode = true;
      } else if ( xsN == string("y1") ) {
	y1 = atoi(xsV.c_str()); mode = true;
      } else if ( xsN == string("y2") ) {
	y2 = atoi(xsV.c_str()); mode = true;
      } else if ( xsN == string("z1") ) {
	z1 = atoi(xsV.c_str()); mode = true;
      } else if ( xsN == string("z2") ) {
	z2 = atoi(xsV.c_str()); mode = true;
      }
    } // end of "param"

  _NEXT_XML_NODE:
    cur = cur->next;
  }

  // set mask params
  if ( ! setMask(mode, x1, x2, y1, y2, z1, z2) ) {
    ErrMsg(MsgERR, msgHdr + string("set params failed"));
    return false;
  }

  return true;
}

bool vsnMethod_Sv_crop::outputXML(std::ostream& os, const size_t ts)
{
  std::string idts;
  for ( int i = 0; i < (int)ts; i++ ) idts.push_back(' ');
  string idts_2 = idts; idts_2.push_back(' '); idts_2.push_back(' ');
  bool ret= true;
  string msgHdr
    = getMethodType() +string("[") +getName() +string("]: outputXML: ");

  vsnData_Sv* pdata = dynamic_cast<vsnData_Sv*>(getRefData());
  if ( ! pdata ) return false;
  Vec3<size_t> dims = pdata->getDims();

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
  // x1
  if ( m_x1 != 0 ) {
    os << idts_2 << "<param name=\"x1\" value=\"" << m_x1 << "\" />" << endl;
  }

  // x2
  if ( m_x2 != dims[0]-1 ) {
    os << idts_2 << "<param name=\"x2\" value=\"" << m_x2 << "\" />" << endl;
  }

  // y1
  if ( m_y1 != 0 ) {
    os << idts_2 << "<param name=\"y1\" value=\"" << m_y1 << "\" />" << endl;
  }

  // y2
  if ( m_y2 != dims[1]-1 ) {
    os << idts_2 << "<param name=\"y2\" value=\"" << m_y2 << "\" />" << endl;
  }

  // z1
  if ( m_z1 != 0 ) {
    os << idts_2 << "<param name=\"z1\" value=\"" << m_z1 << "\" />" << endl;
  }

  // z2
  if ( m_z2 != dims[2]-1 ) {
    os << idts_2 << "<param name=\"z2\" value=\"" << m_z2 << "\" />" << endl;
  }

  os << idts << "</method>" << endl;

  return ret;
}

bool vsnMethod_Sv_crop::commandXML(xmlNodePtr xnp) {
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

  vsnData_Sv* pdata = dynamic_cast<vsnData_Sv*>(getRefData());
  if ( ! pdata ) return false;
  Vec3<size_t> dims = pdata->getDims();

  int val = atoi(valueStr.c_str());
  if ( nameStr == "set_x1" ) {
    if ( val < 0 || val >= m_x2 ) {
      ErrMsg(MsgERR, msgHdr +
             string("command set_x1: set failed: ") + valueStr);
      return false;
    }
	m_x1 = val;
  } // end of "set_x1"
  else if ( nameStr == "set_x2" ) {
    if ( val <= m_x1 || val >= (int)dims[0] ) {
      ErrMsg(MsgERR, msgHdr +
             string("command set_x2: set failed: ") + valueStr);
      return false;
    }
    m_x2 = val;
  } // end of "set_x2"
  else if ( nameStr == "set_y1" ) {
    if ( val < 0 || val >= m_y2 ) {
      ErrMsg(MsgERR, msgHdr +
             string("command set_y1: set failed: ") + valueStr);
      return false;
    }
    m_y1 = val;
  } // end of "set_y1"
  else if ( nameStr == "set_y2" ) {
    if ( val <= m_y1 || val >= (int)dims[1] ) {
      ErrMsg(MsgERR, msgHdr +
             string("command set_y2: set failed: ") + valueStr);
      return false;
    }
	m_y2 = val;
  } // end of "set_y2"
  else if ( nameStr == "set_z1" ) {
    if ( val < 0 || val >= m_z2 ) {
      ErrMsg(MsgERR, msgHdr +
             string("command set_z1: set failed: ") + valueStr);
      return false;
    }
	m_z1 = val;
  } // end of "set_z1"
  else if ( nameStr == "set_z2" ) {
    if ( val <= m_z1 || val >= (int)dims[2] ) {
      ErrMsg(MsgERR, msgHdr +
             string("command set_z2: set failed: ") + valueStr);
      return false;
    }
	m_z2 = val;
  } // end of "set_z2"

  return true;
}
