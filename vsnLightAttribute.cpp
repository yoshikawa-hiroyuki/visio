//
// vsnLightAttribute
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
#include "wx/colordlg.h"

#include "vsnLightAttribute.h"
#include "vsnApp.h"
#include "vsnGfxView.h"

using namespace std;
using namespace CES;
using namespace VSN;


//----------------------------------------------------------------
// class vsnLightAttribute
//----------------------------------------------------------------
vsnLightAttribute::vsnLightAttribute(class vsnScene* psc)
  : vsnIoObject(), p_scene(psc), m_lightType(Light_Point)
{
  m_direction = CES::Vec3<float>(0.f,0.f,0.f);
  m_position = CES::Vec3<float>(0.f,0.f,15.f);
  m_color[0] = m_color[1] = m_color[2] = m_color[3] = 1.f;
}

vsnLightAttribute::~vsnLightAttribute() {
}


void vsnLightAttribute::setRefScene(class vsnScene* psc) {
  if ( p_scene == psc ) return;
  p_scene = psc;
  (void)applyLight();
}


/* transformation interface */

bool vsnLightAttribute::applyLight() const
{
  if ( ! p_scene ) return false;
  vsnApp* papp = vsnApp::GetApp();
  size_t i, nvf = papp->getNumViewFrame();
  for ( i = 0; i < nvf; i++ ) {
    vfrScene* psc = papp->getViewFrame(i)->getGfxView()->getRootScene();
    if ( ! psc ) continue;
    if ( ! psc->getNode(p_scene->getID()) ) continue;

    vfrLight* l0 = psc->getLight(0);
    vfrLight* l1 = psc->getLight(1);

    if( m_lightType == Light_BiDirection )
      l1->setOnOff(TRUE);
    else
      l1->setOnOff(FALSE);

    switch ( m_lightType ) {
    case Light_BiDirection:
    case Light_Direction:
    {
      vfrMatrix M;
      M.RotY(CES::Deg2Rad(m_direction[1]));
      M.RotX(CES::Deg2Rad(m_direction[0]));
      M.RotZ(CES::Deg2Rad(m_direction[2]));
      l0->setLightType(LT_DIRECTIONAL);
      l0->identity();
      l0->trans(0.f, 0.f, -15.f);
      l0->mult(M);
      if ( m_lightType == Light_BiDirection ) {
	l1->identity();
	l1->trans(0.f, 0.f, -15.f);
	M.Identity();
	M.RotY((float)M_PI);
	M.RotY(CES::Deg2Rad(m_direction[1]));
	M.RotX(-CES::Deg2Rad(m_direction[0]));
	M.RotZ(-CES::Deg2Rad(m_direction[2]));
	l1->mult(M);
      }
      break;
    }
    case Light_Point:
      l0->setLightType(LT_POINT);
      l0->identity();
      l0->trans(m_position.m_v);
      break;
    }

    l0->setSpecular(m_color);
    l1->setSpecular(m_color);
  } // end of for(i)

  p_scene->chkNotice();
  vsnViewFrame::UpdateAllLightAttrDlg();
  return true;
}

void vsnLightAttribute::resetLight()
{
  m_lightType = Light_BiDirection;
  m_direction = CES::Vec3<float>(0.f,0.f,0.f);
  m_position = CES::Vec3<float>(0.f,0.f,15.f);
  m_color[0] = m_color[1] = m_color[2] = m_color[3] = 1.f;
  applyLight();
}

bool vsnLightAttribute::isStandardLight() const {
  if ( m_lightType != Light_BiDirection ) return false;
  if ( m_direction[0] != 0.f || m_direction[1] != 0.f ||
       m_direction[2] != 0.f ) return false;
  if ( m_position[0] != 0.f || m_position[1] != 0.f ||
       m_position[2] != 15.f ) return false;
  if ( m_color[0] != 1.f || m_color[1] != 1.f || m_color[2] != 1.f )
    return false;
  return true;
}

void vsnLightAttribute::setLightType(const LightType lightType) {
  if ( m_lightType == lightType ) return;
  m_lightType = lightType;
  applyLight();
}

void vsnLightAttribute::setDirection(const CES::Vec3<float> xyz) {
  m_direction = xyz;
  applyLight();
}

void vsnLightAttribute::setPosition(const CES::Vec3<float> xyz) {
  m_position = xyz;
  applyLight();
}

void vsnLightAttribute::setColor(const VFR::vector4 color) {
  m_color[0] = color[0]; m_color[1] = color[1];
  m_color[2] = color[2]; m_color[3] = color[3];
  applyLight();
}


/* serialize : from vsnIoObject */

bool vsnLightAttribute::parseXML(xmlNodePtr xnp)
{
  static xmlChar* xs;
  if ( ! xnp ) return false;

  vsnLightAttribute::LightType lightType = Light_BiDirection;
  CES::Vec3<float> xyz(0.f, 0.f, -1.f);
  VFR::vector4 color = {1.f, 1.f, 1.f, 1.f};

  // is 'light' node?
  if ( strcmp((const char*)xnp->name, "light") ) return false;

  // type
  if ( xs ) xmlFree(xs);
  xs = xmlGetProp(xnp, (const xmlChar*)"type");
  if ( ! xs || strlen((const char*)xs) < 1 ) return false;
  if ( (const char*)xs == string("bi-directional") )
    lightType = vsnLightAttribute::Light_BiDirection;
  else if ( (const char*)xs == string("directional") )
    lightType = vsnLightAttribute::Light_Direction;
  else if ( (const char*)xs == string("point") )
    lightType = vsnLightAttribute::Light_Point;

  // get children node
  xmlNodePtr cur = xnp->xmlChildrenNode;

  // traverse
  while ( cur ) {
    if ( (cur = vsnIoObject::SkipCommentXML(cur)) == NULL )
      break;

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

      if ( xsV.empty() ) goto _NEXT_XML_NODE;

      if ( xsN == string("xyz") ) {
        int nscan = sscanf(xsV.c_str(), "%f %f %f",
			   &xyz[0], &xyz[1], &xyz[2]);
        if ( nscan < 3 ) goto _NEXT_XML_NODE;
      } // end of "xyz"
      else if ( xsN == string("color") ) {
        int nscan = sscanf(xsV.c_str(), "%f %f %f %f",
			   &color[0], &color[1], &color[2], &color[3]);
        if ( nscan < 3 ) goto _NEXT_XML_NODE;
        if ( nscan < 4 ) color[3] = 1.f;
      } // end of "color"
    }

  _NEXT_XML_NODE:
    cur = cur->next;
  } // end of while(cur)

  setLightType(lightType);
  switch ( lightType ) {
  case Light_BiDirection:
  case Light_Direction:
    setDirection(xyz);
    break;
  case Light_Point:
    setPosition(xyz);
    break;
  } // end of switch(lightType)
  setColor(color);

  applyLight();
  return true;
}

bool vsnLightAttribute::outputXML(std::ostream& os, const size_t ts)
{
  std::string idts;
  register size_t i;
  for ( i = 0; i < ts; i++ ) idts.push_back(' ');
  string idts_2 = idts; idts_2.push_back(' '); idts_2.push_back(' ');

  std::string str;
  CES::Vec3<float> xyz(0.f, 0.f, -1.f);
  switch ( getLightType() ) {
  case Light_BiDirection:
    str = "bi-directional";
    xyz = getDirection();
    break;
  case Light_Direction:
    str = "directional";
    xyz = getDirection();
    break;
  case Light_Point:
    str = "point";
    xyz = getPosition();
    break;
  }

  // output
  os << idts << "<light type=\"" << str << "\"";
  os << " >" << endl;

  // output params
  // xyz
  os << idts_2 << "<param name=\"xyz\" value=\"" 
     << xyz[0] << " " << xyz[1] << " " << xyz[2] 
     << "\" />" << endl;

  // color
  float* cv = m_color;
  os << idts_2 << "<param name=\"color\" value=\""
     << cv[0] << " " << cv[1] << " " << cv[2] << " " << cv[3]
     << "\" />" << endl;

  os << idts << "</light>" << std::endl;
  return true;
}


//----------------------------------------------------------------
// class vsnLightAttributeDlg
//----------------------------------------------------------------

BEGIN_EVENT_TABLE(vsnLightAttributeDlg, wxDialog)
  EVT_RADIOBOX(LightAttDlg_Type, vsnLightAttributeDlg::OnChangeRadio)
  EVT_TEXT_ENTER(LightAttDlg_X, vsnLightAttributeDlg::OnChangeValues)
  EVT_TEXT_ENTER(LightAttDlg_Y, vsnLightAttributeDlg::OnChangeValues)
  EVT_TEXT_ENTER(LightAttDlg_Z, vsnLightAttributeDlg::OnChangeValues)
  EVT_BUTTON(LightAttDlg_ColorBtn, vsnLightAttributeDlg::OnColorBtn)
  EVT_BUTTON(LightAttDlg_ResetBtn, vsnLightAttributeDlg::OnResetBtn)
  EVT_BUTTON(LightAttDlg_CloseBtn, vsnLightAttributeDlg::OnCloseBtn)
  EVT_CLOSE(vsnLightAttributeDlg::OnClose)
END_EVENT_TABLE()

vsnLightAttributeDlg::vsnLightAttributeDlg(wxWindow *parent,
					   vsnLightAttribute* refLightAtt)
  : wxDialog(parent, -1, wxString(wxT("light attribute"))), 
    p_lightAtt(refLightAtt)
{
  assert(parent);

  wxBoxSizer *sizerTop = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer* sizerH;

  // prepare widgets
  wxString items[] = {wxString(wxT("bi-directional")),
		      wxString(wxT("directional")), wxString(wxT("point"))};

  m_pTypeRadio = new wxRadioBox(this, LightAttDlg_Type, wxT("type"),
				wxDefaultPosition, wxDefaultSize,
				3, items, 1, wxRA_SPECIFY_ROWS);
  assert(m_pTypeRadio);

  m_pXLbl = new wxStaticText(this, -1, wxT("rx"));
  assert(m_pXLbl);
  m_pYLbl = new wxStaticText(this, -1, wxT("ry"));
  assert(m_pYLbl);
  m_pZLbl = new wxStaticText(this, -1, wxT("rz"));
  assert(m_pZLbl);

  m_pXTxt = new wxTextCtrl(this, LightAttDlg_X, wxT(""),
			   wxDefaultPosition, wxDefaultSize,
			   wxTE_PROCESS_ENTER);
  assert(m_pXTxt);
  m_pYTxt = new wxTextCtrl(this, LightAttDlg_Y, wxT(""),
			   wxDefaultPosition, wxDefaultSize,
			   wxTE_PROCESS_ENTER);
  assert(m_pYTxt);
  m_pZTxt = new wxTextCtrl(this, LightAttDlg_Z, wxT(""),
			   wxDefaultPosition, wxDefaultSize,
			   wxTE_PROCESS_ENTER);
  assert(m_pZTxt);

  m_pColorBtn = new wxButton(this, LightAttDlg_ColorBtn, wxT("color.."));
  assert(m_pColorBtn);
  m_pResetBtn = new wxButton(this, LightAttDlg_ResetBtn, wxT("reset"));
  assert(m_pResetBtn);
  m_pCloseBtn = new wxButton(this, LightAttDlg_CloseBtn, wxT("close"));
  assert(m_pCloseBtn);

  // layout
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  sizerTop->Add(sizerH, 0, wxALL, 5);
  sizerH->Add(m_pTypeRadio, 0, wxALL, 3);
  sizerH->Add(5, 5);
  sizerH->Add(m_pColorBtn, 0, wxALIGN_CENTER|wxALL, 3);

  sizerH = new wxBoxSizer(wxHORIZONTAL);
  sizerTop->Add(sizerH, 0, wxEXPAND|wxALL, 5);
  sizerH->Add(m_pXLbl, 0, wxALL, 3);
  sizerH->Add(m_pXTxt, 1, wxEXPAND|wxALL, 3);
  sizerH->Add(5, 5);
  sizerH->Add(m_pYLbl, 0, wxALL, 3);
  sizerH->Add(m_pYTxt, 1, wxEXPAND|wxALL, 3);
  sizerH->Add(5, 5);
  sizerH->Add(m_pZLbl, 0, wxALL, 3);
  sizerH->Add(m_pZTxt, 1, wxEXPAND|wxALL, 3);

  sizerTop->Add(0, 5, 0, wxGROW);
  sizerTop->Add(
     new wxStaticLine(this, -1, wxDefaultPosition, wxSize(3,3), wxHORIZONTAL),
     0, wxEXPAND|wxALL, 0);

  sizerH = new wxBoxSizer(wxHORIZONTAL);
  sizerTop->Add(sizerH, 0, wxALIGN_RIGHT|wxALL, 5);
  sizerH->Add(m_pResetBtn, 0, wxALL, 3);
  sizerH->Add(10, 10);
  sizerH->Add(m_pCloseBtn, 0, wxALL, 3);

  SetAutoLayout(TRUE);
  SetSizer(sizerTop);
  sizerTop->SetSizeHints(this);
  sizerTop->Layout();
  sizerTop->Fit(this);

  (void)update();
}

vsnLightAttributeDlg::~vsnLightAttributeDlg() {
}

/* interface */

bool vsnLightAttributeDlg::update()
{
  if ( ! p_lightAtt ) return false;

  CES::Vec3<float> xyz;
  // light type
  switch ( p_lightAtt->getLightType() ) {
  case vsnLightAttribute::Light_BiDirection:
    m_pTypeRadio->SetSelection(0);
    m_pXLbl->SetLabel(wxT("rx"));
    m_pYLbl->SetLabel(wxT("ry"));
    m_pZLbl->SetLabel(wxT("rz"));
    xyz = p_lightAtt->getDirection();
    break;
  case vsnLightAttribute::Light_Direction:
    m_pTypeRadio->SetSelection(1);
    m_pXLbl->SetLabel(wxT("rx"));
    m_pYLbl->SetLabel(wxT("ry"));
    m_pZLbl->SetLabel(wxT("rz"));
    xyz = p_lightAtt->getDirection();
    break;
  case vsnLightAttribute::Light_Point:
    m_pTypeRadio->SetSelection(2);
    m_pXLbl->SetLabel(wxT("px"));
    m_pYLbl->SetLabel(wxT("py"));
    m_pZLbl->SetLabel(wxT("pz"));
    xyz = p_lightAtt->getPosition();
    break;
  }

  // xyz
  char txt[64];
  sprintf(txt, "%g", xyz[0]); m_pXTxt->SetValue(vsnApp::ConvSysToWx(txt));
  sprintf(txt, "%g", xyz[1]); m_pYTxt->SetValue(vsnApp::ConvSysToWx(txt));
  sprintf(txt, "%g", xyz[2]); m_pZTxt->SetValue(vsnApp::ConvSysToWx(txt));

  // color
  VFR::vector4 color;
  p_lightAtt->getColor(color);
  m_colour[0] = color[0];
  m_colour[1] = color[1];
  m_colour[2] = color[2];
  m_colour[3] = color[3];

  return true;
}

void vsnLightAttributeDlg::setRefLightAtt(vsnLightAttribute* refLightAtt)
{
  if ( p_lightAtt == refLightAtt ) return;
  p_lightAtt = refLightAtt;

  (void)update();
}


/* event handler */

void vsnLightAttributeDlg::OnChangeRadio(wxCommandEvent& event)
{
  if ( ! p_lightAtt ) return;

  // type
  int sel = m_pTypeRadio->GetSelection();
  if ( sel == 0 )
    p_lightAtt->setLightType(vsnLightAttribute::Light_BiDirection);
  else if ( sel == 1 )
    p_lightAtt->setLightType(vsnLightAttribute::Light_Direction);
  else if ( sel == 2 )
    p_lightAtt->setLightType(vsnLightAttribute::Light_Point);

  (void)update();
}

void vsnLightAttributeDlg::OnChangeValues(wxCommandEvent& event)
{
  if ( ! p_lightAtt ) return;

  // xyz
  wxString wkStr;
  float rx, ry, rz;
  rx = ry = rz = -1.f;
  wkStr = m_pXTxt->GetValue();
  if ( ! wkStr.IsEmpty() ) rx = (float)atof(vsnApp::ConvWxToSys(wkStr).c_str());
  wkStr = m_pYTxt->GetValue();
  if ( ! wkStr.IsEmpty() ) ry = (float)atof(vsnApp::ConvWxToSys(wkStr).c_str());
  wkStr = m_pZTxt->GetValue();
  if ( ! wkStr.IsEmpty() ) rz = (float)atof(vsnApp::ConvWxToSys(wkStr).c_str());
  CES::Vec3<float> xyz(rx, ry, rz);
  switch ( p_lightAtt->getLightType() ) {
  case vsnLightAttribute::Light_BiDirection:
  case vsnLightAttribute::Light_Direction:
    p_lightAtt->setDirection(xyz);
    break;
  case vsnLightAttribute::Light_Point:
    p_lightAtt->setPosition(xyz);
    break;
  }

  (void)update();
}

void vsnLightAttributeDlg::OnColorBtn(wxCommandEvent& event)
{
  if ( ! p_lightAtt ) return;

  // init color
  wxColourData colourData;
  wxColour colour((unsigned char)(m_colour[0]*255.f),
		  (unsigned char)(m_colour[1]*255.f),
		  (unsigned char)(m_colour[2]*255.f));
  colourData.SetColour(colour);

  wxColourDialog colourDlg(this, &colourData);

  if ( colourDlg.ShowModal() == wxID_OK ) {
    wxColour newColour = colourDlg.GetColourData().GetColour();
    m_colour[0] = newColour.Red() / 255.f;
    m_colour[1] = newColour.Green() / 255.f;
    m_colour[2] = newColour.Blue() / 255.f;
    m_colour[3] = 1.f;
  }

  p_lightAtt->setColor(m_colour);

  (void)update();
}

void vsnLightAttributeDlg::OnResetBtn(wxCommandEvent& event)
{
  if ( ! p_lightAtt ) return;
  p_lightAtt->resetLight();
  (void)update();
}

void vsnLightAttributeDlg::OnCloseBtn(wxCommandEvent& event)
{
  if ( IsModal() )
    EndModal(vsn_wxIDOK);
  else
    this->Hide();
}

void vsnLightAttributeDlg::OnClose(wxCloseEvent& event) {
}
