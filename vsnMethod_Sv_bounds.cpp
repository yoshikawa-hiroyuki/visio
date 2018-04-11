//
// vsnMethod_Sv_bounds
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

#include "vsnMethod_Sv_bounds.h"
#include "vsnError.h"

using namespace std;
using namespace CES;
using namespace VSN;


//----------------------------------------------------------------
// class vsnMPP_Sv_bounds
//----------------------------------------------------------------

BEGIN_EVENT_TABLE(vsnMPP_Sv_bounds, wxPanel)
  EVT_TEXT_ENTER(MPP_Sv_bounds_LineWidthTxt, vsnMPP_Sv_bounds::OnLineWidthTxt)
  EVT_CHECKBOX(MPP_Sv_bounds_AntiAliasChk, vsnMPP_Sv_bounds::OnAntiAliasChk)
END_EVENT_TABLE()


/* constructors / destructor */

vsnMPP_Sv_bounds::vsnMPP_Sv_bounds(wxPanel* parent, vsnMethodObj* pm)
  : vsnMethodPP(parent, pm), m_pLineWidthTxt(NULL), m_pAntiAliasChk(NULL)
{
  assert(parent);
  assert(dynamic_cast<vsnMethod_Sv_bounds*>(pm));

  wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL);

  wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizer, 0, wxEXPAND);
  sizer->Add(new wxStaticText(this, -1, wxT("line width")),
	     0, wxALIGN_LEFT|wxALL, 3);
  m_pLineWidthTxt = new wxTextCtrl(this, MPP_Sv_bounds_LineWidthTxt, wxT(""),
				   wxDefaultPosition, wxDefaultSize,
				   wxTE_PROCESS_ENTER);
  sizer->Add(m_pLineWidthTxt, 0, wxALIGN_RIGHT|wxALL, 3);

  m_pAntiAliasChk = new wxCheckBox(this, MPP_Sv_bounds_AntiAliasChk,
				   wxT("anti-alias line"));
  topsizer->Add(m_pAntiAliasChk, 0, wxALL, 3);

  // post process
  SetSizer(topsizer);
  addTo(parent);
  topsizer->Layout();
  (void)update();
}

vsnMPP_Sv_bounds::~vsnMPP_Sv_bounds() {
}


/* interface */

bool vsnMPP_Sv_bounds::update() {
  if ( ! m_pLineWidthTxt || ! m_pAntiAliasChk ) return false;
  vsnMethod_Sv_bounds* pm = dynamic_cast<vsnMethod_Sv_bounds*>(p_method);
  if ( ! pm ) return false;

  char txt[64];
  sprintf(txt, "%g", pm->getLineWidth());
  m_pLineWidthTxt->SetValue(vsnApp::ConvSysToWx(txt));

  m_pAntiAliasChk->SetValue(pm->getAntiAliasMode());
  return true;
}


/* event handler */

void vsnMPP_Sv_bounds::OnLineWidthTxt(wxCommandEvent& event) {
  if ( ! m_pLineWidthTxt ) return;
  vsnMethod_Sv_bounds* pm = dynamic_cast<vsnMethod_Sv_bounds*>(p_method);
  if ( ! pm ) return;

  wxString valStr = m_pLineWidthTxt->GetValue();
  if ( valStr.IsEmpty() ) return;
  const char* xstr = vsnApp::ConvWxToSys(valStr).c_str();
  float value = (float)atof(xstr);
  if ( value <= 0.f ) return;
  float olw = pm->getLineWidth();
  if ( value == olw ) return;

  if ( pm->setLineWidth(value) )
    pm->chkNotice();
}

void vsnMPP_Sv_bounds::OnAntiAliasChk(wxCommandEvent& event) {
  if ( ! m_pAntiAliasChk ) return;
  vsnMethod_Sv_bounds* pm = dynamic_cast<vsnMethod_Sv_bounds*>(p_method);
  if ( ! pm ) return;

  bool val = m_pAntiAliasChk->GetValue();
  if ( pm->setAntiAliasMode(val) )
    pm->chkNotice();
}


//----------------------------------------------------------------
// class vsnMethod_Sv_bounds
//----------------------------------------------------------------

/* constructors / destructor */

vsnMethod_Sv_bounds::vsnMethod_Sv_bounds(const string& name)
  : vsnMethodObj(name),
    m_lineWidth(1.f), m_bounds(NULL)
{
  m_showType = RT_WIRE;
}

vsnMethod_Sv_bounds::~vsnMethod_Sv_bounds() {
  if ( m_bounds )
    delete m_bounds;
}


/* vsnMethodObj method */

bool vsnMethod_Sv_bounds::update(const bool force) {
  return updateStep(m_requestedStp, force);
}


vsnMethodPP* vsnMethod_Sv_bounds::getParamPanel(wxPanel* pp) {
  vsnMPP_Sv_bounds* pp_bounds = new vsnMPP_Sv_bounds(pp, this);
  if ( ! pp_bounds ) return NULL;
  return pp_bounds;
}


/* vsnTimeSeriesMethodIF methods */

#define IDX3(d,i,j,k) (3*((k)*(d)[0]*(d)[1] +(j)*(d)[0] +(i)))

bool vsnMethod_Sv_bounds::updateStep(const int stp,
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

  if ( ! m_bounds ) {
    m_bounds = new vfrIndexLines();
    if ( ! m_bounds ) {
      ErrMsg(MsgERR, getMethodType() + string("[") + getName()
	     + string("]: memory allocation failed"));
      return false;
    }
    m_bounds->alcMaterial();
    m_bounds->setLoopMode(FALSE);
    addChild(m_bounds);
  }

  m_bounds->getPrivateMaterial()->setRenderMode(RT_NONE);
  if ( ! m_show ) return true;

  // get dims
  Vec3<size_t> dims = pData->getDims();
  if ( dims[0] < 1 || dims[1] < 1 || dims[2] < 1 )
    return false;

  //---- uniform coordinates ----
  if ( pData->isUniformCoord() ) {
    m_bounds->alcVerts(8);
    m_bounds->alcIndices(24);
    vector3* vl = m_bounds->getVerts();
    int* il = m_bounds->getIndices();
    if ( ! vl || ! il ) {
      ErrMsg(MsgERR, getMethodType() + string("[") + getName()
	     + string("]: memory allocation failed"));
      return false;
    }

    const CES::Vec3<float>* dbb = pData->getBbox();
    vl[0][0] = dbb[0][0];  vl[0][1] = dbb[0][1];  vl[0][2] = dbb[0][2];
    vl[1][0] = dbb[1][0];  vl[1][1] = dbb[0][1];  vl[1][2] = dbb[0][2];
    vl[2][0] = dbb[1][0];  vl[2][1] = dbb[1][1];  vl[2][2] = dbb[0][2];
    vl[3][0] = dbb[0][0];  vl[3][1] = dbb[1][1];  vl[3][2] = dbb[0][2];
    vl[4][0] = dbb[0][0];  vl[4][1] = dbb[0][1];  vl[4][2] = dbb[1][2];
    vl[5][0] = dbb[1][0];  vl[5][1] = dbb[0][1];  vl[5][2] = dbb[1][2];
    vl[6][0] = dbb[1][0];  vl[6][1] = dbb[1][1];  vl[6][2] = dbb[1][2];
    vl[7][0] = dbb[0][0];  vl[7][1] = dbb[1][1];  vl[7][2] = dbb[1][2];

    il[0] = 0; il[1] = 1; il[2] = 2; il[3] = 3; il[4] = 0;
    il[5] = VFR_END_OF_ELEM;
    il[6] = 4; il[7] = 5; il[8] = 6; il[9] = 7; il[10] = 4;
    il[11] = VFR_END_OF_ELEM;
    il[12] = 0; il[13] = 4; il[14] = VFR_END_OF_ELEM;
    il[15] = 1; il[16] = 5; il[17] = VFR_END_OF_ELEM;
    il[18] = 2; il[19] = 6; il[20] = VFR_END_OF_ELEM;
    il[21] = 3; il[22] = 7; il[23] = VFR_END_OF_ELEM;

    m_bounds->getPrivateMaterial()->setRenderMode(m_showType);
    m_updatedStp = m_requestedStp;
    return true;
  } // end of if(isUniformCoord)


  //---- non-uniform coordinates ----

  m_bounds->alcVerts(4*(dims[0]+dims[1]+dims[2]-2) +2);
  m_bounds->alcIndices(4*(dims[0]+dims[1]+dims[2]-2) +8);
  vector3* vl = m_bounds->getVerts();
  int* il = m_bounds->getIndices();
  if ( ! vl || ! il ) {
    ErrMsg(MsgERR, getMethodType() + string("[") + getName()
	   + string("]: memory allocation failed"));
    return false;
  }

  register int i, j, k;
  register size_t i3, vindex, iindex;
  vindex = 0; iindex = 0;

  const float* gptr = pData->getCoord(m_requestedStp);
  if ( ! gptr ) return false;

  // ---- XY plane 1 ----
  // X
  j = 0; k = 0;
  for ( i = 0; i < dims[0]; i++ ) {
    i3 = IDX3(dims, i,j,k);
    vl[vindex][0] = gptr[i3++];
    vl[vindex][1] = gptr[i3++];
    vl[vindex][2] = gptr[i3];
    il[iindex++] = vindex++;
  }
  // Y
  i = dims[0] -1;
  for ( j = 1; j < dims[1]; j++ ) {
    i3 = IDX3(dims, i,j,k);
    vl[vindex][0] = gptr[i3++];
    vl[vindex][1] = gptr[i3++];
    vl[vindex][2] = gptr[i3];
    il[iindex++] = vindex++;
  }
  // -X
  j = dims[1] -1;
  for ( i = dims[0] -2; i >= 0; i-- ) {
    i3 = IDX3(dims, i,j,k);
    vl[vindex][0] = gptr[i3++];
    vl[vindex][1] = gptr[i3++];
    vl[vindex][2] = gptr[i3];
    il[iindex++] = vindex++;
  }
  // -Y
  i = 0;
  for ( j = dims[1] -2; j >= 0; j-- ) {
    i3 = IDX3(dims, i,j,k);
    vl[vindex][0] = gptr[i3++];
    vl[vindex][1] = gptr[i3++];
    vl[vindex][2] = gptr[i3];
    il[iindex++] = vindex++;
  }
  m_bounds->setIndice(iindex++, VFR_END_OF_ELEM);
  
  // ---- XY plane 2 ----
  // X
  j = 0; k = dims[2] -1;
  for ( i = 0; i < dims[0]; i++ ) {
    i3 = IDX3(dims, i,j,k);
    vl[vindex][0] = gptr[i3++];
    vl[vindex][1] = gptr[i3++];
    vl[vindex][2] = gptr[i3];
    il[iindex++] = vindex++;
  }
  // Y
  i = dims[0] -1;
  for ( j = 1; j < dims[1]; j++ ) {
    i3 = IDX3(dims, i,j,k);
    vl[vindex][0] = gptr[i3++];
    vl[vindex][1] = gptr[i3++];
    vl[vindex][2] = gptr[i3];
    il[iindex++] = vindex++;
  }
  
  // -X
  j = dims[1] -1;
  for ( i = dims[0] -2; i >= 0; i-- ) {
    i3 = IDX3(dims, i,j,k);
    vl[vindex][0] = gptr[i3++];
    vl[vindex][1] = gptr[i3++];
    vl[vindex][2] = gptr[i3];
    il[iindex++] = vindex++;
  }
  // -Y
  i = 0;
  for ( j = dims[1] -2; j >= 0; j-- ) {
    i3 = IDX3(dims, i,j,k);
    vl[vindex][0] = gptr[i3++];
    vl[vindex][1] = gptr[i3++];
    vl[vindex][2] = gptr[i3];
    il[iindex++] = vindex++;
  }
  m_bounds->setIndice(iindex++, VFR_END_OF_ELEM);
  
  // ---- Z direction 1 ----
  i = 0; j = 0;
  for ( k = 0; k < dims[2]; k++ ) {
    i3 = IDX3(dims, i,j,k);
    vl[vindex][0] = gptr[i3++];
    vl[vindex][1] = gptr[i3++];
    vl[vindex][2] = gptr[i3];
    il[iindex++] = vindex++;
  }
  m_bounds->setIndice(iindex++, VFR_END_OF_ELEM);
  
  // ---- Z direction 2 ----
  i = dims[0] -1; j = 0;
  for ( k = 0; k < dims[2]; k++ ) {
    i3 = IDX3(dims, i,j,k);
    vl[vindex][0] = gptr[i3++];
    vl[vindex][1] = gptr[i3++];
    vl[vindex][2] = gptr[i3];
    il[iindex++] = vindex++;
  }
  m_bounds->setIndice(iindex++, VFR_END_OF_ELEM);
  
  // ---- Z direction 3 ----
  i = 0; j = dims[1] -1;
  for ( k = 0; k < dims[2]; k++ ) {
    i3 = IDX3(dims, i,j,k);
    vl[vindex][0] = gptr[i3++];
    vl[vindex][1] = gptr[i3++];
    vl[vindex][2] = gptr[i3];
    il[iindex++] = vindex++;
  }
  m_bounds->setIndice(iindex++, VFR_END_OF_ELEM);
  
  // ---- Z direction 4 ----
  i = dims[0] -1; j = dims[1] -1;
  for ( k = 0; k < dims[2]; k++ ) {
    i3 = IDX3(dims, i,j,k);
    vl[vindex][0] = gptr[i3++];
    vl[vindex][1] = gptr[i3++];
    vl[vindex][2] = gptr[i3];
    il[iindex++] = vindex++;
  }
  m_bounds->setIndice(iindex++, VFR_END_OF_ELEM);
  m_bounds->getPrivateMaterial()->setLineWidth(m_lineWidth);
  m_bounds->generateBbox();
  m_bounds->setColor4(0, m_colour);
  m_bounds->setTransparency(m_antiAlias);
 
  // ok 
  m_bounds->getPrivateMaterial()->setRenderMode(m_showType);
  m_updatedStp = m_requestedStp;
  return true;
}


/* methods */

bool vsnMethod_Sv_bounds::setLineWidth(const float lw) {
  if ( m_lineWidth == lw ) return true;
  if ( lw <= 0.f ) return false;
  m_lineWidth = lw;

  if ( m_bounds )
    m_bounds->getPrivateMaterial()->setLineWidth(m_lineWidth);

  updateUI();
  return true;
}


/* serialize : from vsnIoObject */

bool vsnMethod_Sv_bounds::parseXML(xmlNodePtr xnp) {
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

      if ( xsN == string("line_width") ) {
	if ( xsV.empty() ) {
	  ErrMsg(MsgERR, msgHdr + string("no value in param line_width"));
	  goto _NEXT_XML_NODE;
	}
	float lw = (float)atof(xsV.c_str());
	if ( ! setLineWidth(lw) ) {
	  ErrMsg(MsgERR, msgHdr + string("invalid value in param line_width"));
	  goto _NEXT_XML_NODE;
	}
      } // end of "line_width"
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

bool
vsnMethod_Sv_bounds::outputXML(std::ostream& os, const size_t ts) {
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
  // line_width
  if ( m_lineWidth != 1.f ) {
    os << idts_2 << "<param name=\"line_width\" value=\""
       << m_lineWidth << "\" />" << endl;
  }

  // antialias
  if ( m_antiAlias ) {
    os << idts_2 << "<param name=\"antialias\" value=\"yes\" />" << endl;
  }

  os << idts << "</method>" << endl;

  return ret;
}

bool vsnMethod_Sv_bounds::commandXML(xmlNodePtr xnp) {
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

  if ( nameStr == "set_line_width" ) {
    float lw = (float)atof(valueStr.c_str());
    if ( ! setLineWidth(lw) ) {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_line_width: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_line_width"
  else if ( nameStr == "set_antialias" ) {
    bool am;
    if ( valueStr == string("yes") ) am = true;
    else if ( valueStr == string("no") ) am = false;
    else {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_antialias: invalid value") + valueStr);
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
