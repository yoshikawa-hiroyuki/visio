//
// vsnMethod_Shape_trias_slbExt
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

#include "vsnMethod_Shape_trias_slbExt.h"
#include "vsnData_Shape.h"
#include "vsnColorBar.h"
#include "vsnError.h"

using namespace std;
using namespace CES;
using namespace VSN;


//----------------------------------------------------------------
// class vsnMPP_Shape_trias_slbExt
//----------------------------------------------------------------

BEGIN_EVENT_TABLE(vsnMPP_Shape_trias_slbExt, wxPanel)
  EVT_CHECKBOX(MPP_Shape_trias_slbExt_ColorChk,
               vsnMPP_Shape_trias_slbExt::OnShowChk)
  EVT_CHECKBOX(MPP_Shape_trias_slbExt_ScalarChk,
               vsnMPP_Shape_trias_slbExt::OnShowChk)
  EVT_TEXT_ENTER(MPP_Shape_trias_slbExt_ShrinkFac,
                 vsnMPP_Shape_trias_slbExt::OnShrinkFacTxt)
  EVT_CHECKBOX(MPP_Shape_trias_slbExt_SmoothChk,
               vsnMPP_Shape_trias_slbExt::OnSmoothChk)
END_EVENT_TABLE()


/* constructors / destructor */

vsnMPP_Shape_trias_slbExt::
vsnMPP_Shape_trias_slbExt(wxPanel* parent, vsnMethodObj* pm)
  : vsnMethodPP(parent, pm)
{
  assert(parent);
  assert(dynamic_cast<vsnMethod_Shape_trias_slbExt*>(pm));

  wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer* sizerH;

  // show type
  m_pColorChk = new wxCheckBox(this, MPP_Shape_trias_slbExt_ColorChk,
			       wxT("show colored facets"));
  topsizer->Add(m_pColorChk, 0, wxEXPAND|wxALL, 3);

  m_pScalarChk = new wxCheckBox(this, MPP_Shape_trias_slbExt_ScalarChk,
				wxT("show facets with scalar"));
  topsizer->Add(m_pScalarChk, 0, wxEXPAND|wxALL, 3);

  // shrink factor
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxALL, 0);
  sizerH->Add(new wxStaticText(this, -1, wxT("shrink factor")),
              0, wxALIGN_LEFT|wxALL, 3);
  m_pShrinkFacTxt = new wxTextCtrl(this, MPP_Shape_trias_slbExt_ShrinkFac,
                                   wxT(""), wxDefaultPosition, wxDefaultSize,
                                   wxTE_PROCESS_ENTER);
  sizerH->Add(m_pShrinkFacTxt, 0, wxALIGN_RIGHT|wxALL, 3);

  // smooth check
  m_pSmoothChk = new wxCheckBox(this, MPP_Shape_trias_slbExt_SmoothChk,
				wxT("smooth normals"));
  topsizer->Add(m_pSmoothChk, 0, wxEXPAND|wxALL, 3);

  // post process
  SetSizer(topsizer);
  addTo(parent);
  topsizer->Layout();
  (void)update();
}

vsnMPP_Shape_trias_slbExt::~vsnMPP_Shape_trias_slbExt() {
}


/* interface */

bool vsnMPP_Shape_trias_slbExt::update() {
  vsnMethod_Shape_trias_slbExt* pm
    = dynamic_cast<vsnMethod_Shape_trias_slbExt*>(p_method);
  if ( ! pm ) return false;

  if ( ! m_pColorChk || ! m_pScalarChk || ! m_pShrinkFacTxt )
    return false;

  // extension type
  vsnMethod_Shape_trias_slbExt::StlExtType et = pm->getShowObjs();
  m_pColorChk->SetValue(et & vsnMethod_Shape_trias_slbExt::Colored);
  m_pScalarChk->SetValue(et & vsnMethod_Shape_trias_slbExt::ScalarData);

  // shrink factor
  char txt[64];
  sprintf(txt, "%g", pm->getShrinkFac());
  m_pShrinkFacTxt->SetValue(vsnApp::ConvSysToWx(txt));

  // smooth check
  m_pSmoothChk->SetValue(pm->getSmoothMode());

  return true;
}


/* event handler */

void vsnMPP_Shape_trias_slbExt::OnShowChk(wxCommandEvent& event) {
  if ( ! m_pColorChk || ! m_pScalarChk ) return;
  vsnMethod_Shape_trias_slbExt* pm
    = dynamic_cast<vsnMethod_Shape_trias_slbExt*>(p_method);
  if ( ! pm ) return;

  vsnMethod_Shape_trias_slbExt::StlExtType et = 0;
  if ( m_pColorChk->GetValue() )
    et |= vsnMethod_Shape_trias_slbExt::Colored;
  if ( m_pScalarChk->GetValue() )
    et |= vsnMethod_Shape_trias_slbExt::ScalarData;
  if ( pm->getShowObjs() == et ) return;

  if ( pm->setShowObjs(et) )
    pm->chkNotice();
}

void vsnMPP_Shape_trias_slbExt::OnShrinkFacTxt(wxCommandEvent& event) {
  if ( ! m_pShrinkFacTxt ) return;
  vsnMethod_Shape_trias_slbExt* pm
    = dynamic_cast<vsnMethod_Shape_trias_slbExt*>(p_method);
  if ( ! pm ) return;

  wxString valStr = m_pShrinkFacTxt->GetValue();
  if ( valStr.IsEmpty() ) return;
  float value = (float)atof(vsnApp::ConvWxToSys(valStr).c_str());

  if ( pm->setShrinkFac(value) )
    pm->chkNotice();
  else
    update();
}

void vsnMPP_Shape_trias_slbExt::OnSmoothChk(wxCommandEvent& event) {
  if ( ! m_pSmoothChk ) return;
  vsnMethod_Shape_trias_slbExt* pm
    = dynamic_cast<vsnMethod_Shape_trias_slbExt*>(p_method);
  if ( ! pm ) return;

  if ( pm->setSmoothMode(m_pSmoothChk->GetValue()) )
    pm->chkNotice();
}


//----------------------------------------------------------------
// class vsnMethod_Shape_trias_slbExt
//----------------------------------------------------------------

/* constructors / destructor */

vsnMethod_Shape_trias_slbExt::vsnMethod_Shape_trias_slbExt(const string& name)
  : vsnMethodObj(name),
    m_showObjs(Colored|ScalarData), m_shrinkFac(0.f), m_smooth(false),
    m_trias(NULL)
{
}

vsnMethod_Shape_trias_slbExt::~vsnMethod_Shape_trias_slbExt() {
  if ( m_trias )
    delete m_trias;
}


/* methods */

bool vsnMethod_Shape_trias_slbExt::setShowObjs(const StlExtType et) {
  if ( et == m_showObjs ) return true;
  m_showObjs = et;

  if ( ! update() ) return false;
  updateUI();
  return true;
}

bool vsnMethod_Shape_trias_slbExt::setShrinkFac(const float sf) {
  float xsf = sf;
  if ( xsf < 0.f ) xsf = 0.f;
  else if ( xsf > 1.f ) xsf = 1.f;
  if ( xsf == m_shrinkFac ) return true;
  m_shrinkFac = xsf;

  if ( ! update() ) return false;
  updateUI();
  return true;
}

bool vsnMethod_Shape_trias_slbExt::setSmoothMode(const bool sm) {
  if ( m_smooth == sm ) return true;
  m_smooth = sm;

  if ( ! m_trias ) return true;

  if ( m_smooth ) {
    if ( m_trias->getNormalMode() == AT_PER_VERTEX ) return true;
    m_trias->setNormalMode(AT_PER_VERTEX);
    if ( ! m_trias->generateNormals() ) return false;
  }
  else {
    if ( m_trias->getNormalMode() == AT_PER_FACE ) return true;
    m_trias->setNormalMode(AT_PER_FACE);
    if ( ! m_trias->generateNormals() ) return false;
  }
  return true;
}


/* from vsnMethodObj */

bool vsnMethod_Shape_trias_slbExt::update(const bool force) {
  return updateStep(m_requestedStp, force);
}

vsnMethodPP* vsnMethod_Shape_trias_slbExt::getParamPanel(wxPanel* pp) {
  vsnMPP_Shape_trias_slbExt* pp_trias_slbExt
    = new vsnMPP_Shape_trias_slbExt(pp, this);
  if ( ! pp_trias_slbExt ) return NULL;
  return pp_trias_slbExt;
}

void vsnMethod_Shape_trias_slbExt::setBaseColor(const vector4 cv) {
  memcpy(m_colour, cv, sizeof(vector4));
  updateUI();

  if ( m_pColorBar )
    m_pColorBar->setBaseColor(m_colour);

  if ( ! m_useLut ) {
    if ( m_trias ) {
      m_trias->setTransparency(m_colour[3] < OPAC_THRESH);
      m_trias->setAlpha(m_colour[3]);
    }
  } // end of if(!m_useLut)
  else {
    if ( m_trias ) {
      m_trias->setTransparency(m_colour[3] < OPAC_THRESH);
      m_trias->setAlpha(m_colour[3]);
    }
  } // end of if(m_useLut)

  chkNotice();
}


/* from vsnTimeSeriesMethodIF */

bool vsnMethod_Shape_trias_slbExt::updateStep(const int stp,
					      const bool force,
					      const bool cascade)
{
  vsnData_Shape* pData = dynamic_cast<vsnData_Shape*>(p_refData);
  if ( ! pData ) return false;
  m_requestedStp = stp;
  if ( m_requestedStp < 0 )
    m_requestedStp = pData->getCurrentStepIdx();

  if ( force ) m_updatedStp = -1;
  if ( m_updatedStp == m_requestedStp ) return true;
  m_updatedStp = -1;

  // allocate trias
  if ( ! m_trias ) {
    m_trias = new vfrTriangles();
    if ( ! m_trias || ! m_trias->alcMaterial() ) {
      ErrMsg(MsgERR, getMethodType() + string("[") + getName()
             + string("]: memory allocation failed"));
      return false;
    }
    m_trias->setNormalMode(AT_PER_FACE);
    m_trias->setColorMode(AT_PER_FACE);
    addChild(m_trias);
  }
  m_trias->getPrivateMaterial()->setRenderMode(RT_NONE);

  // check shape type
  vsnData_Shape::ShapeType shapeType = pData->getShapeType();
  if ( shapeType != vsnData_Shape::ShapeSLB &&
       shapeType != vsnData_Shape::ShapeSLB_BE ) {
    //ErrMsg(MsgERR, getMethodType() + string("[") + getName()
    //   + string("]: data is not SLB"));
    //return false;
    return true; // accept non-slb file, but shows nothing...
  }

  // get shape primitive
  vfrNode* pobj = pData->getShape();
  if ( ! pobj ) return false;
  vector3* pv = pobj->getVerts();
  vector3* pn = pobj->getNormals();
  int* pi = pobj->getIndices();
  int nF = pobj->getNumNormals();
  int nV = pobj->getNumVerts();
  int nI = pobj->getNumIndices();
  if ( nF < 1 || nV < 3 ) return true;
  if ( nF != nI || nF * 3 != nV ) {
    ErrMsg(MsgERR, getMethodType() + string("[") + getName()
	   + string("]: incomplete data"));
    return false;
  }

  // set up trias
  if ( m_shrinkFac == 1.f ) return true;

  if ( ! m_trias->alcVerts(nV) ) {
    ErrMsg(MsgERR, getMethodType() + string("[") + getName()
	   + string("]: memory allocation failed for vetices"));
    return false;
  }
  if ( ! m_trias->alcNormals(nF) ) {
    ErrMsg(MsgERR, getMethodType() + string("[") + getName()
	   + string("]: memory allocation failed for normals"));
    return false;
  }
  if ( ! m_trias->alcColors(nF) ) {
    ErrMsg(MsgERR, getMethodType() + string("[") + getName()
	   + string("]: memory allocation failed for colors"));
    return false;
  }

  register int idx, i, c;
  register float offsetFac = 1.f - m_shrinkFac;
  register float offsetFacN = m_shrinkFac * 0.01f;
  Vec3<float> g, wkp, wkn, vtx[3];
  vector4* p_col = m_trias->getColors();
  for ( idx = 0, i = 0; i < nF; i++ ) {
    // color
    if ( (pi[i] & (0x1<<15)) && (m_showObjs & Colored) ) {
      p_col[idx][0] = ((pi[i] & (31<<10)) >> 10) / 31.0f;
      p_col[idx][1] = ((pi[i] & (31<<5)) >> 5) / 31.0f;
      p_col[idx][2] = (pi[i] & 31) / 31.0f;
      p_col[idx][3] = 1.f;
    } // end of Colored
    else if ( !(pi[i] & (0x1<<15)) && (m_showObjs & ScalarData) ) {
      float value = (pi[i] & 32767) / 32767.f;
      c = m_lut.getValIdx(value);
      p_col[idx][0] = m_lut.lutEntry[c*4  ];
      p_col[idx][1] = m_lut.lutEntry[c*4+1];
      p_col[idx][2] = m_lut.lutEntry[c*4+2];
      p_col[idx][3] = 1.f;
    } // end of ScalarData
    else
      continue;

    // vertex
    vtx[0] = Vec3<float>(pv[i*3]);
    vtx[1] = Vec3<float>(pv[i*3 +1]);
    vtx[2] = Vec3<float>(pv[i*3 +2]);
    g = vtx[0] + vtx[1]; g = g + vtx[2];
    g.m_v[0] /= 3.f; g.m_v[1] /= 3.f; g.m_v[2] /= 3.f;
    wkn = Vec3<float>(pn[i]) * offsetFacN;
    wkp = ((vtx[0] - g) * offsetFac) + g;
    wkp = wkp + wkn;
    m_trias->setVert(idx*3, wkp.m_v, FALSE);
    wkp = ((vtx[1] - g) * offsetFac) + g;
    wkp = wkp + wkn;
    m_trias->setVert(idx*3 +1, wkp.m_v, FALSE);
    wkp = ((vtx[2] - g) * offsetFac) + g;
    wkp = wkp + wkn;
    m_trias->setVert(idx*3 +2, wkp.m_v, FALSE);

    // normal
    m_trias->setNormal(idx, pn[i]);

    idx ++;
  } // end of for(i)
  m_trias->alcVerts(idx * 3);
  m_trias->alcNormals(idx);
  m_trias->alcColors(idx);
  m_trias->generateBbox();

  // LUT mode
  setUseLut(m_showObjs & ScalarData);

  // Smooth normals
  if ( m_smooth ) {
    m_trias->setNormalMode(AT_PER_VERTEX);
    m_trias->generateNormals();
  }

  // ok
  m_trias->getPrivateMaterial()->setRenderMode(m_showType);
  setBaseColor(m_colour);
  setHilight(m_hilight);
  m_updatedStp = m_requestedStp;
  return true;
}


/* serialize : from vsnIoObject */

bool vsnMethod_Shape_trias_slbExt::parseXML(xmlNodePtr xnp) {
  static xmlChar* xs;
  xmlNodePtr cur;
  bool showColored = true, showScalar = true;

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

      if ( xsN == string("show_colored") ) {
        if ( xsV == "yes" )
	  showColored = true;
        else if ( xsV == "no" )
	  showColored = false;
	else {
          ErrMsg(MsgERR, msgHdr +
                 string("invalid value in param show_colored"));
          goto _NEXT_XML_NODE;
        }
      } // end of "show_colored"
      else if ( xsN == string("show_scalar") ) {
        if ( xsV == "yes" )
	  showScalar = true;
        else if ( xsV == "no" )
	  showScalar = false;
	else {
          ErrMsg(MsgERR, msgHdr +
                 string("invalid value in param show_scalar"));
          goto _NEXT_XML_NODE;
        }
      } // end of "show_scalar"
      else if ( xsN == string("shrink_fac") ) {
        float sf = (float)atof(xsV.c_str());
        if ( ! setShrinkFac(sf) ) {
          ErrMsg(MsgERR, msgHdr + string("invalid value in param shrink_fac"));
          goto _NEXT_XML_NODE;
        }
      } // end of "shrink_fac"
      else if ( xsN == string("smooth_normal") ) {
	bool sm = false;
        if ( xsV == "yes" )
	  sm = true;
        else if ( xsV == "no" )
	  sm = false;
	else {
          ErrMsg(MsgERR, msgHdr +
                 string("invalid value in param smooth_normal"));
          goto _NEXT_XML_NODE;
        }
	if ( ! setSmoothMode(sm) ) {
	  ErrMsg(MsgERR, msgHdr + string("set smooth_normal mode failed"));
	  goto _NEXT_XML_NODE;
	}
      } // end of "smooth_normal"
    } // end of param

  _NEXT_XML_NODE:
    cur = cur->next;
  } // end of while(cur)

  // update param
  StlExtType showMode = 0;
  if ( showColored ) showMode |= Colored;
  if ( showScalar  ) showMode |= ScalarData;
  if ( showMode != (Colored | ScalarData) )
    if ( ! setShowObjs(showMode) ) {
      ErrMsg(MsgERR, msgHdr + string("set show mode failed"));
      return false;
    }

  return true;
}

bool vsnMethod_Shape_trias_slbExt::outputXML(std::ostream& os, const size_t ts)
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
  // show_colored
  if ( !(m_showObjs & Colored) ) {
    os << idts_2 << "<param name=\"show_colored\" value=\"no\" />" << endl;
  }

  // show_scalar
  if ( !(m_showObjs & ScalarData) ) {
    os << idts_2 << "<param name=\"show_scalar\" value=\"no\" />" << endl;
  }

  // shrink_fac
  if ( m_shrinkFac != 0.f ) {
    os << idts_2 << "<param name=\"shrink_fac\" value=\""
       << m_shrinkFac << "\" />" << endl;
  }

  // smooth_normal
  if ( m_smooth ) {
    os << idts_2 << "<param name=\"smooth_normal\" value=\"yes\" />" << endl;
  }

  os << idts << "</method>" << endl;
  return true;
}

bool vsnMethod_Shape_trias_slbExt::commandXML(xmlNodePtr xnp) {
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

  StlExtType showMode = m_showObjs;
  if ( nameStr == "set_show_colored" ) {
    if ( valueStr == "yes" )
      showMode |= Colored;
    else if ( valueStr == "no" )
      showMode &= (~Colored);
    else {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_show_colored: invalid value: ") + valueStr);
      return false;
    }
    if ( showMode != m_showObjs )
      if ( ! setShowObjs(showMode) ) {
	ErrMsg(MsgERR, msgHdr + string("command set_show_colored: set failed"));
	return false;
      }
  } // end of "set_show_colored"
  else if ( nameStr == "set_show_scalar" ) {
    if ( valueStr == "yes" )
      showMode |= ScalarData;
    else if ( valueStr == "no" )
      showMode &= (~ScalarData);
    else {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_show_scalar: invalid value: ") + valueStr);
      return false;
    }
    if ( showMode != m_showObjs )
      if ( ! setShowObjs(showMode) ) {
	ErrMsg(MsgERR, msgHdr + string("command set_show_scalar: set failed"));
	return false;
      }
  } // end of "set_show_scalar"
  else if ( nameStr == "set_shrink_fac" ) {
    float sf = (float)atof(valueStr.c_str());
    if ( ! setShrinkFac(sf) ) {
      ErrMsg(MsgERR, msgHdr +
             string("command set_shrink_fac: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_shrink_fac"
  else if ( nameStr == "set_smooth_normal" ) {
    bool sm = false;
    if ( valueStr == "yes" )
      sm = true;
    else if ( valueStr == "no" )
      sm = false;
    else {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_smooth_normal: invalid value: ") + valueStr);
      return false;
    }
    if ( sm != m_smooth )
      if ( ! setSmoothMode(sm) ) {
	ErrMsg(MsgERR, msgHdr +string("command set_smooth_normal: set failed"));
	return false;
      }
  } // end of "set_smooth_normal"
  else {
    // not my command
    ErrMsg(MsgERR, msgHdr + string("unknown command: ") + nameStr);
    return false;
  }

  return true;
}
