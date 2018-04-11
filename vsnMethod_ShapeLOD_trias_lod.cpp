//
// vsnMethod_ShapeLOD_trias_lod
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

#include "vsnMethod_ShapeLOD_trias_lod.h"
#include "vsnData_ShapeLOD.h"
#include "vsnError.h"

using namespace std;
using namespace CES;
using namespace VSN;


//----------------------------------------------------------------
// class vsnMPP_ShapeLOD_trias_lod
//----------------------------------------------------------------

BEGIN_EVENT_TABLE(vsnMPP_ShapeLOD_trias_lod, wxPanel)
  EVT_TEXT_ENTER(MPP_ShapeLOD_trias_lod_Areas,
		 vsnMPP_ShapeLOD_trias_lod::OnAreasTxt)
  EVT_RADIOBOX(MPP_ShapeLOD_trias_lod_RenType,
	       vsnMPP_ShapeLOD_trias_lod::OnRenTypeRadio)
  EVT_TEXT_ENTER(MPP_ShapeLOD_trias_lod_LineWidth,
		 vsnMPP_ShapeLOD_trias_lod::OnLineWidthTxt)
  EVT_TEXT_ENTER(MPP_ShapeLOD_trias_lod_PointSize,
		 vsnMPP_ShapeLOD_trias_lod::OnPointSizeTxt)
  EVT_CHECKBOX(MPP_ShapeLOD_trias_lod_AntiAlias,
               vsnMPP_ShapeLOD_trias_lod::OnAntiAliasChk)
END_EVENT_TABLE()


/* constructors / destructor */

vsnMPP_ShapeLOD_trias_lod::vsnMPP_ShapeLOD_trias_lod(wxPanel* parent,
						     vsnMethodObj* pm)
: vsnMethodPP(parent, pm),
  m_pAreasTxt(NULL),
  m_pRenTypeRadio(NULL), m_pLineWidthTxt(NULL), m_pPointSizeTxt(NULL),
  m_pAntiAliasChk(NULL)
{
  assert(parent);
  assert(dynamic_cast<vsnMethod_ShapeLOD_trias_lod*>(pm));

  wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer* sizerH;

  // areas list
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND|wxALL, 0);
  sizerH->Add(new wxStaticText(this, -1, wxT("area list(%)")),
              0, wxALIGN_LEFT|wxALL, 3);
  m_pAreasTxt = new wxTextCtrl(this, MPP_ShapeLOD_trias_lod_Areas,
			       wxT(""), wxDefaultPosition, wxDefaultSize,
			       wxTE_PROCESS_ENTER);
  sizerH->Add(m_pAreasTxt, 1, wxEXPAND|wxALIGN_RIGHT|wxALL, 3);

  // render type
  wxString ritems[] = {wxString(wxT("Face")),
                       wxString(wxT("Wire")),
                       wxString(wxT("Point"))};
  m_pRenTypeRadio = new wxRadioBox(this, MPP_ShapeLOD_trias_lod_RenType,
				   wxT("rendering type"),
				   wxDefaultPosition, wxDefaultSize,
				   3, ritems, 1, wxRA_SPECIFY_COLS);
  topsizer->Add(m_pRenTypeRadio, 0, wxEXPAND|wxALL, 3);

  // line width
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxALL, 0);
  sizerH->Add(new wxStaticText(this, -1, wxT("line width")),
              0, wxALIGN_LEFT|wxALL, 3);
  m_pLineWidthTxt = new wxTextCtrl(this, MPP_ShapeLOD_trias_lod_LineWidth,
                                   wxT(""), wxDefaultPosition, wxDefaultSize,
                                   wxTE_PROCESS_ENTER);
  sizerH->Add(m_pLineWidthTxt, 0, wxALIGN_RIGHT|wxALL, 3);

  // point size
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxALL, 0);
  sizerH->Add(new wxStaticText(this, -1, wxT("point size")),
              0, wxALIGN_LEFT|wxALL, 3);
  m_pPointSizeTxt = new wxTextCtrl(this, MPP_ShapeLOD_trias_lod_PointSize,
                                   wxT(""), wxDefaultPosition, wxDefaultSize,
                                   wxTE_PROCESS_ENTER);
  sizerH->Add(m_pPointSizeTxt, 0, wxALIGN_RIGHT|wxALL, 3);

  // anti-alias mode
  m_pAntiAliasChk = new wxCheckBox(this, MPP_ShapeLOD_trias_lod_AntiAlias,
				   wxT("anti-alias line"));
  topsizer->Add(m_pAntiAliasChk, 0, wxALL, 3);

  SetSizer(topsizer);
  addTo(parent);
  topsizer->Layout();
  (void)update();
}

vsnMPP_ShapeLOD_trias_lod::~vsnMPP_ShapeLOD_trias_lod() {
}


/* interface */

bool vsnMPP_ShapeLOD_trias_lod::update() {
  vsnMethod_ShapeLOD_trias_lod* pm
    = dynamic_cast<vsnMethod_ShapeLOD_trias_lod*>(p_method);
  if ( ! pm ) return false;

  if ( ! m_pAreasTxt || ! m_pRenTypeRadio || ! m_pLineWidthTxt ||
       ! m_pPointSizeTxt || ! m_pAntiAliasChk )
    return false;
  char txt[256];

  // areas list
  vsnLOD::AreaListType al = pm->getAreaList();
  txt[0] = '\0';
  vsnLOD::AreaListType::iterator it;
  for ( it = al.begin(); it != al.end(); it++ ) {
    char buf[32];
    if ( it == al.begin() )
      sprintf(buf, "%.1f", *it * 100.0);
    else
      sprintf(buf, ", %.1f", *it * 100.0);
    strcat(txt, buf);
  } // end of for(it)
  m_pAreasTxt->SetValue(vsnApp::ConvSysToWx(txt));

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
  sprintf(txt, "%g", pm->getLineWidth());
  m_pLineWidthTxt->SetValue(vsnApp::ConvSysToWx(txt));
  sprintf(txt, "%g", pm->getPointSize());
  m_pPointSizeTxt->SetValue(vsnApp::ConvSysToWx(txt));

  // anti-alias mode
  m_pAntiAliasChk->SetValue(pm->getAntiAliasMode());

  return true;
}


/* event handler */

void vsnMPP_ShapeLOD_trias_lod::OnAreasTxt(wxCommandEvent& event) {
  static const char* delim = " ,\t\r\n";
  if ( ! m_pAreasTxt ) return;
  vsnMethod_ShapeLOD_trias_lod* pm
    = dynamic_cast<vsnMethod_ShapeLOD_trias_lod*>(p_method);
  if ( ! pm ) return;

  vsnLOD::AreaListType al;
  wxString valStr = m_pAreasTxt->GetValue();
  if ( ! valStr.IsEmpty() ) {
    string xstr = vsnApp::ConvWxToSys(valStr);
    char buff[256]; sprintf(buff, "%s", xstr.c_str());
    char* tok = strtok(buff, delim);
    while ( tok ){
      float val = atof(tok) * 0.01f;
      if ( val > 0.0f )
	al.insert(val);
      tok = strtok(NULL, delim);
    } // end of while(tok)
  }

  if ( pm->setAreaList(al) )
    pm->chkNotice();
  update();
}

void vsnMPP_ShapeLOD_trias_lod::OnRenTypeRadio(wxCommandEvent& event) {
  if ( ! m_pRenTypeRadio ) return;
  vsnMethod_ShapeLOD_trias_lod* pm
    = dynamic_cast<vsnMethod_ShapeLOD_trias_lod*>(p_method);
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

void vsnMPP_ShapeLOD_trias_lod::OnLineWidthTxt(wxCommandEvent& event) {
  if ( ! m_pLineWidthTxt ) return;
  vsnMethod_ShapeLOD_trias_lod* pm
    = dynamic_cast<vsnMethod_ShapeLOD_trias_lod*>(p_method);
  if ( ! pm ) return;

  wxString valStr = m_pLineWidthTxt->GetValue();
  if ( valStr.IsEmpty() ) return;
  float value = (float)atof(vsnApp::ConvWxToSys(valStr).c_str());

  if ( pm->setLineWidth(value) )
    pm->chkNotice();
}

void vsnMPP_ShapeLOD_trias_lod::OnPointSizeTxt(wxCommandEvent& event) {
  if ( ! m_pPointSizeTxt ) return;
  vsnMethod_ShapeLOD_trias_lod* pm
    = dynamic_cast<vsnMethod_ShapeLOD_trias_lod*>(p_method);
  if ( ! pm ) return;

  wxString valStr = m_pPointSizeTxt->GetValue();
  if ( valStr.IsEmpty() ) return;
  float value = (float)atof(vsnApp::ConvWxToSys(valStr).c_str());

  if ( pm->setPointSize(value) )
    pm->chkNotice();
}

void vsnMPP_ShapeLOD_trias_lod::OnAntiAliasChk(wxCommandEvent& event) {
  if ( ! m_pAntiAliasChk ) return;
  vsnMethod_ShapeLOD_trias_lod* pm
    = dynamic_cast<vsnMethod_ShapeLOD_trias_lod*>(p_method);
  if ( ! pm ) return;

  bool val = m_pAntiAliasChk->GetValue();
  if ( pm->setAntiAliasMode(val) )
    pm->chkNotice();
}


//----------------------------------------------------------------
// class vsnMethod_ShapeLOD_trias_lod
//----------------------------------------------------------------

/* constructors / destructor */

vsnMethod_ShapeLOD_trias_lod::vsnMethod_ShapeLOD_trias_lod(const string& name)
  : vsnMethodObj(name),
    m_renType(RT_SMOOTH), m_lineWidth(1.f), m_pointSize(2.f),
    m_lightMode(true), m_pLOD(NULL)
{
  m_pLOD = new vsnLOD("LOD_Grp", false);
  if ( ! m_pLOD ) return;
  if ( ! m_pLOD->alcMaterial() ) return;
  addChild(m_pLOD);
}

vsnMethod_ShapeLOD_trias_lod::~vsnMethod_ShapeLOD_trias_lod() {
  if ( m_pLOD )
    delete m_pLOD;
}


/* methods */

vsnLOD::AreaListType
vsnMethod_ShapeLOD_trias_lod::getAreaList() const {
  vsnLOD::AreaListType al;
  if ( ! m_pLOD ) return al;
  al = m_pLOD->getAreaList();
  return al;
}

bool
vsnMethod_ShapeLOD_trias_lod::setAreaList(const vsnLOD::AreaListType& al) {
  if ( ! m_pLOD ) return false;
  return m_pLOD->setAreaList(al);
}

bool vsnMethod_ShapeLOD_trias_lod::setRenderType(const VFR::RenderType rt) {
  if ( ! m_pLOD ) return false;
  if ( m_renType == rt ) return true;
  if ( rt != RT_SMOOTH && rt != RT_WIRE && rt != RT_POINT )
    return false;
  m_renType = rt;

  if ( m_renType == RT_SMOOTH ) {
    m_pLOD->setTransparency(m_colour[3]<0.991f);
    m_pLOD->setAlpha(m_colour[3]);
  } else {
    m_pLOD->setTransparency(m_antiAlias);
    m_pLOD->setAlpha(1.f);
  }
  vfrMaterial* pmate = m_pLOD->getPrivateMaterial();
  if ( pmate->getRenderMode() != RT_NONE ) {
    if ( ! m_lightMode && m_renType == RT_SMOOTH )
      pmate->setRenderMode(RT_NOLIGHT);
    else
      pmate->setRenderMode(m_renType);
  }

  notice();
  updateUI();
  return true;  
}

bool vsnMethod_ShapeLOD_trias_lod::setLineWidth(const float lw) {
  if ( ! m_pLOD ) return false;
  if ( m_lineWidth == lw ) return true;
  if ( lw <= 0.f ) return false;
  m_lineWidth = lw;

  m_pLOD->getPrivateMaterial()->setLineWidth(m_lineWidth);

  updateUI();
  return true;
}

bool vsnMethod_ShapeLOD_trias_lod::setPointSize(const float psz) {
  if ( ! m_pLOD ) return false;
  if ( m_pointSize == psz ) return true;
  if ( psz <= 0.f ) return false;
  m_pointSize = psz;

  m_pLOD->getPrivateMaterial()->setPointSize(m_pointSize);

  updateUI();
  return true;
}


/* from vsnMethodObj */

bool vsnMethod_ShapeLOD_trias_lod::update(const bool force) {
  vsnData_ShapeLOD* pData = dynamic_cast<vsnData_ShapeLOD*>(p_refData);
  if ( ! pData ) return false;
  if ( ! m_pLOD ) return false;

  vfrMaterial* pmate = m_pLOD->getPrivateMaterial();
  if ( ! pmate ) return false;
  pmate->setLineWidth(m_lineWidth);
  pmate->setPointSize(m_pointSize);
  pmate->setRenderMode(RT_NONE);

  // set LOD models
  m_pLOD->remAllChildren();
  size_t i, nl = pData->getNumLevels();
  for ( i = 0; i < nl; i++ ) {
    m_pLOD->addChild(pData->getShape(i));
  } // end of for(i)

  // update parameters
  if ( m_antiAlias ) { // force set anti-alias mode
    m_antiAlias = false;
    setAntiAliasMode(true);
  }
  setBaseColor(m_colour); // force set base-color

  // set rendering mode
  if ( ! m_lightMode && m_renType == RT_SMOOTH )
    pmate->setRenderMode(RT_NOLIGHT);
  else
    pmate->setRenderMode(m_renType);

  // ok 
  return true;
}

vsnMethodPP* vsnMethod_ShapeLOD_trias_lod::getParamPanel(wxPanel* pp) {
  vsnMPP_ShapeLOD_trias_lod* pp_trias = new vsnMPP_ShapeLOD_trias_lod(pp, this);
  if ( ! pp_trias ) return NULL;
  return pp_trias;
}

void vsnMethod_ShapeLOD_trias_lod::setLighting(const bool mode) {
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


/* serialize : from vsnIoObject */

bool vsnMethod_ShapeLOD_trias_lod::parseXML(xmlNodePtr xnp) {
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

      if ( xsN == string("areas") ) {
	vsnLOD::AreaListType al;
	const char* delim = " ,\t\r\n";
	char buff[256]; sprintf(buff, "%s", xsV.c_str());
	char* tok = strtok(buff, delim);
	while ( tok ){
	  float val = atof(tok) * 0.01f;
	  if ( val > 0.0f )
	    al.insert(val);
	  tok = strtok(NULL, delim);
	} // end of while(tok)
	if ( ! setAreaList(al) ) {
	  ErrMsg(MsgERR, msgHdr + string("set areas param failed"));
	  goto _NEXT_XML_NODE;
	}
      }
      else if ( xsN == string("render_type") ) {
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

bool vsnMethod_ShapeLOD_trias_lod::outputXML(std::ostream& os, const size_t ts)
{
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
  // areas
  vsnLOD::AreaListType al = getAreaList();
  os << idts_2 << "<param name=\"areas\" value=\"";
  vsnLOD::AreaListType::iterator ait;
  for ( ait = al.begin(); ait != al.end(); ait++ ) {
    if ( ait == al.begin() )
      os << *ait * 100.0;
    else
      os << ", " << *ait * 100.0;
  } // end of for(ait)
  os << "\" />" << endl;

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

bool vsnMethod_ShapeLOD_trias_lod::commandXML(xmlNodePtr xnp) {
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

  if ( nameStr == "set_areas" ) {
    vsnLOD::AreaListType al;
    const char* delim = " ,\t\r\n";
    char buff[256]; sprintf(buff, "%s", valueStr.c_str());
    char* tok = strtok(buff, delim);
    while ( tok ){
      float val = atof(tok) * 0.01f;
      if ( val > 0.0f )
	al.insert(val);
	  tok = strtok(NULL, delim);
    } // end of while(tok)
    if ( ! setAreaList(al) ) {
      ErrMsg(MsgERR, msgHdr + string("set areas param failed"));
      return false;
    }
  }
  else if ( nameStr == "set_render_type" ) {
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

