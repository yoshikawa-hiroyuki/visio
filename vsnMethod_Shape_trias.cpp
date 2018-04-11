//
// vsnMethod_Shape_trias
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

#include "vsnMethod_Shape_trias.h"
#include "vsnData_Shape.h"
#include "vsnError.h"

using namespace std;
using namespace CES;
using namespace VSN;


//----------------------------------------------------------------
// class vsnMPP_Shape_trias
//----------------------------------------------------------------

BEGIN_EVENT_TABLE(vsnMPP_Shape_trias, wxPanel)
  EVT_RADIOBOX(MPP_Shape_trias_RenType,
	       vsnMPP_Shape_trias::OnRenTypeRadio)
  EVT_TEXT_ENTER(MPP_Shape_trias_LineWidth,
		 vsnMPP_Shape_trias::OnLineWidthTxt)
  EVT_TEXT_ENTER(MPP_Shape_trias_PointSize,
		 vsnMPP_Shape_trias::OnPointSizeTxt)
  EVT_CHECKBOX(MPP_Shape_trias_AntiAlias,
               vsnMPP_Shape_trias::OnAntiAliasChk)
END_EVENT_TABLE()


/* constructors / destructor */

vsnMPP_Shape_trias::vsnMPP_Shape_trias(wxPanel* parent, vsnMethodObj* pm)
  : vsnMethodPP(parent, pm),
    m_pRenTypeRadio(NULL), m_pLineWidthTxt(NULL), m_pPointSizeTxt(NULL),
    m_pAntiAliasChk(NULL)
{
  assert(parent);
  assert(dynamic_cast<vsnMethod_Shape_trias*>(pm));

  wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer* sizerH;

  // render type
  wxString ritems[] = {wxString(wxT("Face")),
                       wxString(wxT("Wire")),
                       wxString(wxT("Point"))};
  m_pRenTypeRadio = new wxRadioBox(this, MPP_Shape_trias_RenType,
				   wxT("rendering type"),
				   wxDefaultPosition, wxDefaultSize,
				   3, ritems, 1, wxRA_SPECIFY_COLS);
  topsizer->Add(m_pRenTypeRadio, 0, wxEXPAND|wxALL, 3);

  // line width
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxALL, 0);
  sizerH->Add(new wxStaticText(this, -1, wxT("line width")),
              0, wxALIGN_LEFT|wxALL, 3);
  m_pLineWidthTxt = new wxTextCtrl(this, MPP_Shape_trias_LineWidth,
                                   wxT(""), wxDefaultPosition, wxDefaultSize,
                                   wxTE_PROCESS_ENTER);
  sizerH->Add(m_pLineWidthTxt, 0, wxALIGN_RIGHT|wxALL, 3);

  // point size
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxALL, 0);
  sizerH->Add(new wxStaticText(this, -1, wxT("point size")),
              0, wxALIGN_LEFT|wxALL, 3);
  m_pPointSizeTxt = new wxTextCtrl(this, MPP_Shape_trias_PointSize,
                                   wxT(""), wxDefaultPosition, wxDefaultSize,
                                   wxTE_PROCESS_ENTER);
  sizerH->Add(m_pPointSizeTxt, 0, wxALIGN_RIGHT|wxALL, 3);

  // anti-alias mode
  m_pAntiAliasChk = new wxCheckBox(this, MPP_Shape_trias_AntiAlias,
				   wxT("anti-alias line"));
  topsizer->Add(m_pAntiAliasChk, 0, wxALL, 3);

  SetSizer(topsizer);
  addTo(parent);
  topsizer->Layout();
  (void)update();
}

vsnMPP_Shape_trias::~vsnMPP_Shape_trias() {
}


/* interface */

bool vsnMPP_Shape_trias::update() {
  vsnMethod_Shape_trias* pm
    = dynamic_cast<vsnMethod_Shape_trias*>(p_method);
  if ( ! pm ) return false;

  if ( ! m_pRenTypeRadio || ! m_pLineWidthTxt || ! m_pPointSizeTxt ||
       ! m_pAntiAliasChk )
    return false;

  // render type
  RenderType rt = pm->getRenderType();
  switch ( rt ) {
  case RT_WIRE:
    m_pRenTypeRadio->SetSelection(1); break;
  case RT_POINT:
    m_pRenTypeRadio->SetSelection(2); break;
  case RT_SMOOTH: default:
    m_pRenTypeRadio->SetSelection(0);
  }

  // line width / point size
  char txt[64];
  sprintf(txt, "%g", pm->getLineWidth());
  m_pLineWidthTxt->SetValue(vsnApp::ConvSysToWx(txt));
  sprintf(txt, "%g", pm->getPointSize());
  m_pPointSizeTxt->SetValue(vsnApp::ConvSysToWx(txt));

  // anti-alias mode
  m_pAntiAliasChk->SetValue(pm->getAntiAliasMode());

  return true;
}


/* event handler */

void vsnMPP_Shape_trias::OnRenTypeRadio(wxCommandEvent& event) {
  if ( ! m_pRenTypeRadio ) return;
  vsnMethod_Shape_trias* pm
    = dynamic_cast<vsnMethod_Shape_trias*>(p_method);
  if ( ! pm ) return;

  int val = m_pRenTypeRadio->GetSelection();
  RenderType rt;
  switch ( val ) {
  case 1: rt = RT_WIRE; break;
  case 2: rt = RT_POINT; break;
  case 0: default: rt = RT_SMOOTH;
  }
  if ( pm->getRenderType() == rt ) return;

  if ( pm->setRenderType(rt) )
    pm->chkNotice();
}

void vsnMPP_Shape_trias::OnLineWidthTxt(wxCommandEvent& event) {
  if ( ! m_pLineWidthTxt ) return;
  vsnMethod_Shape_trias* pm
    = dynamic_cast<vsnMethod_Shape_trias*>(p_method);
  if ( ! pm ) return;

  wxString valStr = m_pLineWidthTxt->GetValue();
  if ( valStr.IsEmpty() ) return;
  float value = (float)atof(vsnApp::ConvWxToSys(valStr).c_str());

  if ( pm->setLineWidth(value) )
    pm->chkNotice();
}

void vsnMPP_Shape_trias::OnPointSizeTxt(wxCommandEvent& event) {
  if ( ! m_pPointSizeTxt ) return;
  vsnMethod_Shape_trias* pm
    = dynamic_cast<vsnMethod_Shape_trias*>(p_method);
  if ( ! pm ) return;

  wxString valStr = m_pPointSizeTxt->GetValue();
  if ( valStr.IsEmpty() ) return;
  float value = (float)atof(vsnApp::ConvWxToSys(valStr).c_str());

  if ( pm->setPointSize(value) )
    pm->chkNotice();
}

void vsnMPP_Shape_trias::OnAntiAliasChk(wxCommandEvent& event) {
  if ( ! m_pAntiAliasChk ) return;
  vsnMethod_Shape_trias* pm
    = dynamic_cast<vsnMethod_Shape_trias*>(p_method);
  if ( ! pm ) return;

  bool val = m_pAntiAliasChk->GetValue();
  if ( pm->setAntiAliasMode(val) )
    pm->chkNotice();
}


//----------------------------------------------------------------
// class vsnMethod_Shape_trias
//----------------------------------------------------------------

/* constructors / destructor */

vsnMethod_Shape_trias::vsnMethod_Shape_trias(const string& name)
  : vsnMethodObj(name),
    m_renType(RT_SMOOTH), m_lineWidth(1.f), m_pointSize(2.f),
    m_lightMode(true), m_shape(NULL)
{
}

vsnMethod_Shape_trias::~vsnMethod_Shape_trias() {
  if ( m_shape )
    delete m_shape;
}


/* methods */

bool vsnMethod_Shape_trias::setRenderType(const VFR::RenderType rt) {
  if ( m_renType == rt ) return true;
  if ( rt != RT_SMOOTH && rt != RT_WIRE && rt != RT_POINT )
    return false;
  m_renType = rt;

  if ( m_shape ) {
    if ( m_renType == RT_SMOOTH ) {
      m_shape->setTransparency(m_colour[3]<0.991f);
      m_shape->setAlpha(m_colour[3]);
    } else {
      m_shape->setTransparency(m_antiAlias);
      m_shape->setAlpha(1.f);
    }
    vfrMaterial* pmate = m_shape->getPrivateMaterial();
    if ( pmate->getRenderMode() != RT_NONE ) {
      if ( ! m_lightMode && m_renType == RT_SMOOTH )
	pmate->setRenderMode(RT_NOLIGHT);
      else
	pmate->setRenderMode(m_renType);
    }
  } // end of if(m_shape)

  updateUI();
  return true;  
}

bool vsnMethod_Shape_trias::setLineWidth(const float lw) {
  if ( m_lineWidth == lw ) return true;
  if ( lw <= 0.f ) return false;
  m_lineWidth = lw;

  if ( m_shape ) {
    m_shape->getPrivateMaterial()->setLineWidth(m_lineWidth);
  }

  updateUI();
  return true;
}

bool vsnMethod_Shape_trias::setPointSize(const float psz) {
  if ( m_pointSize == psz ) return true;
  if ( psz <= 0.f ) return false;
  m_pointSize = psz;

  if ( m_shape ) {
    m_shape->getPrivateMaterial()->setPointSize(m_pointSize);
  }

  updateUI();
  return true;
}


/* from vsnMethodObj */

bool vsnMethod_Shape_trias::update(const bool force) {
  return updateStep(m_requestedStp, force);
}

vsnMethodPP* vsnMethod_Shape_trias::getParamPanel(wxPanel* pp) {
  vsnMPP_Shape_trias* pp_trias = new vsnMPP_Shape_trias(pp, this);
  if ( ! pp_trias ) return NULL;
  return pp_trias;
}

void vsnMethod_Shape_trias::setLighting(const bool mode) {
  if ( m_lightMode == mode ) return;
  m_lightMode = mode;

  if ( m_renType != RT_SMOOTH ) return;
  RenderType rtype = mode ? RT_SMOOTH : RT_NOLIGHT;

  for ( int i = 0; i < nChild; i++ ) {
    if ( ! _children[i] ) continue;
    vfrMaterial* pmate = _children[i]->getPrivateMaterial();
    if ( pmate && pmate->getRenderMode() != RT_NONE )
      pmate->setRenderMode(rtype);
  } // end of for(i)

  updateUI();
  chkNotice();
}


/* from vsnTimeSeriesMethodIF */

bool vsnMethod_Shape_trias::updateStep(const int stp,
				       const bool force, const bool cascade)
{
  vsnData_Shape* pData = dynamic_cast<vsnData_Shape*>(p_refData);
  if ( ! pData ) return false;
  m_requestedStp = stp;
  if ( m_requestedStp < 0 )
    m_requestedStp = pData->getCurrentStepIdx();

  if ( force ) m_updatedStp = -1;
  if ( m_updatedStp == m_requestedStp ) return true;
  m_updatedStp = -1;

  // allocate shape
  if ( ! m_shape ) {
    m_shape = new vfrPrimSet();
    if ( ! m_shape || ! m_shape->alcMaterial() || ! m_shape->alcVerts(1) ) {
      ErrMsg(MsgERR, getMethodType() + string("[") + getName()
             + string("]: memory allocation failed"));
      return false;
    }
    m_shape->getPrivateMaterial()->setLineWidth(m_lineWidth);
    m_shape->getPrivateMaterial()->setPointSize(m_pointSize);
    addChild(m_shape);
  }
  m_shape->getPrivateMaterial()->setRenderMode(RT_NONE);

  // set shape primitive
  vfrNode* pobj = pData->getShape();
  if ( ! pobj ) return false;
  m_shape->setPrimitive(pobj);

  // update parameters
  if ( m_antiAlias ) { // force set anti-alias mode
    m_antiAlias = false;
    setAntiAliasMode(true);
  }
  setBaseColor(m_colour); // force set base-color

  // set rendering mode
  if ( ! m_lightMode && m_renType == RT_SMOOTH )
    m_shape->getPrivateMaterial()->setRenderMode(RT_NOLIGHT);
  else
    m_shape->getPrivateMaterial()->setRenderMode(m_renType);

  // ok 
  m_updatedStp = m_requestedStp;
  return true;
}

/* serialize : from vsnIoObject */

bool vsnMethod_Shape_trias::parseXML(xmlNodePtr xnp) {
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

      if ( xsN == string("render_type") ) {
	RenderType rt = RT_SMOOTH;
        if ( xsV == "face" || xsV == "Face" || xsV == "FACE" )
	  rt = RT_SMOOTH;
	else if ( xsV == "wire" || xsV == "Wire" || xsV == "WIRE" )
	  rt = RT_WIRE;
	else if ( xsV == "point" || xsV == "Point" || xsV == "POINT" )
	  rt = RT_POINT;
	else {
          ErrMsg(MsgERR, msgHdr +
		 string("invalid value in param render_type"));
          goto _NEXT_XML_NODE;
        }
	if ( ! setRenderType(rt) ) {
	  ErrMsg(MsgERR, msgHdr + string("set render_type param failed"));
	  goto _NEXT_XML_NODE;
	}
      } // end of "render_type"
      else if ( xsN == string("line_width") ) {
        float line_width = (float)atof(xsV.c_str());
        if ( ! setLineWidth(line_width) ) {
          ErrMsg(MsgERR, msgHdr + string("invalid value in param line_width"));
          goto _NEXT_XML_NODE;
        }
      } // end of "line_width"
      else if ( xsN == string("point_size") ) {
        float point_size = (float)atof(xsV.c_str());
        if ( ! setPointSize(point_size) ) {
          ErrMsg(MsgERR, msgHdr + string("invalid value in param point_size"));
          goto _NEXT_XML_NODE;
        }
      } // end of "point_size"
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
    } // end of param

  _NEXT_XML_NODE:
    cur = cur->next;
  } // end of while(cur)

  return true;
}

bool vsnMethod_Shape_trias::outputXML(std::ostream& os, const size_t ts) {
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
  // render_type
  if ( m_renType != RT_SMOOTH ) {
    os << idts_2 << "<param name=\"render_type\" value=\"";
    switch ( m_renType ) {
    case RT_SMOOTH: os << "face"; break;
    case RT_WIRE: os << "wire"; break;
    case RT_POINT: os << "point"; break;
    default: ret = false;
    } // end of switch(m_renType)
    os << "\" />" << endl;
  }

  // line_width
  if ( m_lineWidth != 1.f ) {
    os << idts_2 << "<param name=\"line_width\" value=\""
       << m_lineWidth << "\" />" << endl;
  }

  // point_size
  if ( m_pointSize != 2.f ) {
    os << idts_2 << "<param name=\"point_size\" value=\""
       << m_pointSize << "\" />" << endl;
  }

  // antialias
  if ( m_antiAlias ) {
    os << idts_2 << "<param name=\"antialias\" value=\"yes\" />" << endl;
  }

  os << idts << "</method>" << endl;

  return true;
}

bool vsnMethod_Shape_trias::commandXML(xmlNodePtr xnp) {
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

  if ( nameStr == "set_render_type" ) {
    RenderType rt = RT_SMOOTH;
    if ( valueStr == "face" || valueStr == "Face" || valueStr == "FACE" )
      rt = RT_SMOOTH;
    else if ( valueStr == "wire" || valueStr == "Wire" || valueStr == "WIRE" )
      rt = RT_WIRE;
    else if ( valueStr == "point"|| valueStr == "Point"|| valueStr == "POINT" )
      rt = RT_POINT;
    else {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_render_type: invalid value: ") + valueStr);
      return false;
    }
    if ( ! setRenderType(rt) ) {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_render_type: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_render_type"
  else if ( nameStr == "set_line_width" ) {
    float lw = (float)atof(valueStr.c_str());
    if ( ! setLineWidth(lw) ) {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_line_width: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_line_width"
  else if ( nameStr == "set_point_size" ) {
    float ps = (float)atof(valueStr.c_str());
    if ( ! setPointSize(ps) ) {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_point_size: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_point_size"
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
  else {
    // not my command
    ErrMsg(MsgERR, msgHdr + string("unknown command: ") + nameStr);
    return false;
  }

  return true;
}


//----------------------------------------------------------------
// class vsnMethod_Shape_trias2
//----------------------------------------------------------------

/* constructors / destructor */

vsnMethod_Shape_trias2::vsnMethod_Shape_trias2(const string& name)
  : vsnMethod_Shape_trias(name), m_refShape(NULL)
{
}

vsnMethod_Shape_trias2::~vsnMethod_Shape_trias2() {
  if ( m_refShape )
    delete m_refShape;
}


/* methods */

bool vsnMethod_Shape_trias2::setRenderType(const VFR::RenderType rt) {
  if ( m_renType == rt ) return true;
  if ( rt != RT_SMOOTH && rt != RT_WIRE && rt != RT_POINT )
    return false;
  m_renType = rt;

  if ( m_refShape ) {
    vfrMaterial* pmate = m_refShape->getPrivateMaterial();
    if ( pmate && pmate->getRenderMode() != RT_NONE ) {
      if ( ! m_lightMode && m_renType == RT_SMOOTH )
	pmate->setRenderMode(RT_NOLIGHT);
      else
	pmate->setRenderMode(m_renType);
    }
  } // end of if (m_refShape)

  updateUI();
  return true;  
}

bool vsnMethod_Shape_trias2::setLineWidth(const float lw) {
  if ( m_lineWidth == lw ) return true;
  if ( lw <= 0.f ) return false;
  m_lineWidth = lw;

  if ( m_refShape ) {
    m_refShape->getPrivateMaterial()->setLineWidth(m_lineWidth);
  }

  updateUI();
  return true;
}

bool vsnMethod_Shape_trias2::setPointSize(const float psz) {
  if ( m_pointSize == psz ) return true;
  if ( psz <= 0.f ) return false;
  m_pointSize = psz;

  if ( m_refShape ) {
    m_refShape->getPrivateMaterial()->setPointSize(m_pointSize);
  }

  updateUI();
  return true;
}

bool vsnMethod_Shape_trias2::setAntiAliasMode(const bool aam) {
  if ( m_antiAlias == aam ) return true;
  m_antiAlias = aam;

  if ( m_refShape ) {
    vfrNode* pobj = m_refShape->getChild(0);
    if ( pobj )
      pobj->setTransparency(m_antiAlias && m_renType != RT_SMOOTH);
  }

  updateUI();
  return true;
}


/* from vsnMethodObj */

bool vsnMethod_Shape_trias2::updateStep(const int stp,
					const bool force, const bool cascade) {
  vsnData_Shape* pData = dynamic_cast<vsnData_Shape*>(p_refData);
  if ( ! pData ) return false;
  m_requestedStp = stp;
  if ( m_requestedStp < 0 )
    m_requestedStp = pData->getCurrentStepIdx();

  if ( force ) m_updatedStp = -1;
  if ( m_updatedStp == m_requestedStp ) return true;
  m_updatedStp = -1;

  // allocate refShape
  if ( ! m_refShape ) {
    m_refShape = new vfrGroup();
    if ( ! m_refShape || ! m_refShape->alcMaterial() ) {
      ErrMsg(MsgERR, getMethodType() + string("[") + getName()
             + string("]: memory allocation failed"));
      return false;
    }
    addChild(m_refShape);
  }
  m_refShape->getPrivateMaterial()->setRenderMode(RT_NONE);
  m_refShape->remAllChildren();

  // set shape primitive
  vfrNode* pobj = pData->getShape();
  if ( ! pobj ) return false;
  m_refShape->addChild(pobj);

  // update parameters
  if ( m_antiAlias ) { // force set anti-alias mode
    m_antiAlias = false;
    setAntiAliasMode(true);
  }
  setBaseColor(m_colour); // force set base-color

  // set rendering mode
  if ( ! m_lightMode && m_renType == RT_SMOOTH )
    m_refShape->getPrivateMaterial()->setRenderMode(RT_NOLIGHT);
  else
    m_refShape->getPrivateMaterial()->setRenderMode(m_renType);

  // ok 
  m_updatedStp = m_requestedStp;
  return true;
}
