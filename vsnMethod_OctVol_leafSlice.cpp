//
// vsnMethod_OctVol_leafSlice
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

#include "vsnMethod_OctVol_leafSlice.h"
#include "vsnData_OctVol.h"
#include "vsnError.h"

using namespace std;
using namespace CES;
using namespace VSN;


//----------------------------------------------------------------
// class vsnMPP_OctVol_leafSlice
//----------------------------------------------------------------
BEGIN_EVENT_TABLE(vsnMPP_OctVol_leafSlice, wxPanel)
  EVT_RADIOBOX(MPP_OctVol_leafSlice_AxisRdo,
               vsnMPP_OctVol_leafSlice::OnAxisRdo)
  EVT_COMMAND_SCROLL(MPP_OctVol_leafSlice_PlaneSld,
                     vsnMPP_OctVol_leafSlice::OnPlaneSld)
  EVT_TEXT_ENTER(MPP_OctVol_leafSlice_PlaneTxt,
                 vsnMPP_OctVol_leafSlice::OnPlaneTxt)
  EVT_TEXT_ENTER(MPP_OctVol_leafSlice_LineWidthTxt,
                 vsnMPP_OctVol_leafSlice::OnLineWidthTxt)
  EVT_CHECKBOX(MPP_OctVol_leafSlice_AntiAliasChk,
               vsnMPP_OctVol_leafSlice::OnAntiAliasChk)
END_EVENT_TABLE()


/* constructors / destructor */

vsnMPP_OctVol_leafSlice::vsnMPP_OctVol_leafSlice(wxPanel* parent,
						 vsnMethodObj* pm)
  : vsnMethodPP(parent, pm)
{
  assert(parent);
  assert(dynamic_cast<vsnMethod_OctVol_leafSlice*>(pm));

  // create widgets
  wxString ritems[] = {wxString(wxT("X")),
                       wxString(wxT("Y")), wxString(wxT("Z"))};
  m_pAxisRdo = new wxRadioBox(this, MPP_OctVol_leafSlice_AxisRdo,
			      wxT("slice axis"),
			      wxDefaultPosition,wxDefaultSize,
			      3, ritems, 1, wxRA_SPECIFY_ROWS);
  assert(m_pAxisRdo);

  m_pPlaneSld = new wxSlider(this, MPP_OctVol_leafSlice_PlaneSld,
			     20000 /* val */, 10000 /* min */, 30000 /* max */,
			     wxDefaultPosition, wxSize(120, -1),
			     wxSL_HORIZONTAL|wxSL_LABELS);
  assert(m_pPlaneSld);

  m_pPlaneTxt = new wxTextCtrl(this, MPP_OctVol_leafSlice_PlaneTxt, wxT(""),
                               wxDefaultPosition, wxDefaultSize,
                               wxTE_PROCESS_ENTER);
  assert(m_pPlaneTxt);

  m_pLineWidthTxt = new wxTextCtrl(this, MPP_OctVol_leafSlice_LineWidthTxt,
                                   wxT(""), wxDefaultPosition, wxDefaultSize,
                                   wxTE_PROCESS_ENTER);
  assert(m_pLineWidthTxt);

  m_pAntiAliasChk = new wxCheckBox(this, MPP_OctVol_leafSlice_AntiAliasChk,
				   wxT("anti-alias line"));
  assert(m_pAntiAliasChk);

  // prepare sizers
  wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL); assert(topsizer);
  wxBoxSizer* sizerH;

  // axis
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxALL, 0);
  sizerH->Add(m_pAxisRdo, 1, wxEXPAND|wxALL, 3);

  // plane
  topsizer->Add(new wxStaticText(this, -1, wxT("slice plane")),
                0, wxALIGN_LEFT|wxALL, 3);
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxALL, 0);
  sizerH->Add(m_pPlaneSld, 0, wxALIGN_RIGHT|wxALL, 3);
  sizerH->Add(m_pPlaneTxt, 0, wxEXPAND|wxALL, 3);

  // line width
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxALL, 0);
  sizerH->Add(new wxStaticText(this, -1, wxT("line width")),
              0, wxALIGN_LEFT|wxALL, 3);
  sizerH->Add(m_pLineWidthTxt, 0, wxALIGN_RIGHT|wxALL, 3);

  // anti-alias
  topsizer->Add(m_pAntiAliasChk, 0, wxALL, 3);

  // post process
  SetSizer(topsizer);
  addTo(parent);
  topsizer->Layout();
  (void)update();
}

vsnMPP_OctVol_leafSlice::~vsnMPP_OctVol_leafSlice() {
}


/* interface */

bool vsnMPP_OctVol_leafSlice::update() {
  if ( ! m_pAxisRdo || ! m_pPlaneSld || ! m_pPlaneTxt ||
       ! m_pLineWidthTxt || ! m_pAntiAliasChk )
    return false;
  int val; char txt[64];

  vsnMethod_OctVol_leafSlice* pm
    = dynamic_cast<vsnMethod_OctVol_leafSlice*>(p_method);
  if ( ! pm ) return false;
  vsnData_OctVol* pdata = dynamic_cast<vsnData_OctVol*>(pm->getRefData());
  if ( ! pdata ) return false;

  val = pm->getSliceAxis();
  m_pAxisRdo->SetSelection(val);

  Vec3<size_t> dims = pdata->getRootDims();
  int rangeMax = dims[(size_t)val] - 1;
  if ( rangeMax < 1 ) rangeMax = 1;
  m_pPlaneSld->SetRange(0, rangeMax);

  val = pm->getSlicePlane();
  m_pPlaneSld->SetValue(val);
  sprintf(txt, "%d", val);
  m_pPlaneTxt->SetValue(vsnApp::ConvSysToWx(txt));

  sprintf(txt, "%g", pm->getLineWidth());
  m_pLineWidthTxt->SetValue(vsnApp::ConvSysToWx(txt));

  m_pAntiAliasChk->SetValue(pm->getAntiAliasMode());

  return true;
}


/* event handler */

void vsnMPP_OctVol_leafSlice::OnAxisRdo(wxCommandEvent& event) {
  if ( ! m_pAxisRdo || ! m_pPlaneSld || ! m_pPlaneTxt ) return;
  vsnMethod_OctVol_leafSlice* pm
    = dynamic_cast<vsnMethod_OctVol_leafSlice*>(p_method);
  if ( ! pm ) return;
  vsnData_OctVol* pdata = dynamic_cast<vsnData_OctVol*>(pm->getRefData());
  if ( ! pdata ) return;

  int val = m_pAxisRdo->GetSelection();
  int oaxis = pm->getSliceAxis();
  if ( val == oaxis ) return;

  Vec3<size_t> dims = pdata->getRootDims();
  m_pPlaneSld->SetRange(0, dims[(size_t)val]-1);
  m_pPlaneSld->SetValue(dims[(size_t)val]/2);

  char txt[64]; sprintf(txt, "%d", m_pPlaneSld->GetValue());
  m_pPlaneTxt->SetValue(vsnApp::ConvSysToWx(txt));

  if ( pm->setSliceParam(val, m_pPlaneSld->GetValue()) )
    pm->chkNotice();
}

void vsnMPP_OctVol_leafSlice::OnPlaneSld(wxScrollEvent& event) {
  if ( ! m_pPlaneSld ) return;
  vsnMethod_OctVol_leafSlice* pm
    = dynamic_cast<vsnMethod_OctVol_leafSlice*>(p_method);
  if ( ! pm ) return;

  int val = m_pPlaneSld->GetValue();
  int oplane = pm->getSlicePlane();
  if ( val == oplane ) return;

  if ( pm->setSliceParam(pm->getSliceAxis(), val) )
    pm->chkNotice();
}

void vsnMPP_OctVol_leafSlice::OnPlaneTxt(wxCommandEvent& event) {
  if ( ! m_pPlaneTxt ) return;
  vsnMethod_OctVol_leafSlice* pm
    = dynamic_cast<vsnMethod_OctVol_leafSlice*>(p_method);
  if ( ! pm ) return;

  wxString valStr = m_pPlaneTxt->GetValue();
  if ( valStr.IsEmpty() ) return;
  int val = atoi(vsnApp::ConvWxToSys(valStr).c_str());
  if ( val < 0 || val > m_pPlaneSld->GetMax() )
    return;

  if ( pm->setSliceParam(pm->getSliceAxis(), val) )
    pm->chkNotice();
}

void vsnMPP_OctVol_leafSlice::OnLineWidthTxt(wxCommandEvent& event) {
  if ( ! m_pLineWidthTxt ) return;
  vsnMethod_OctVol_leafSlice* pm
    = dynamic_cast<vsnMethod_OctVol_leafSlice*>(p_method);
  if ( ! pm ) return;

  wxString valStr = m_pLineWidthTxt->GetValue();
  if ( valStr.IsEmpty() ) return;
  float value = (float)atof(vsnApp::ConvWxToSys(valStr).c_str());

  if ( pm->setLineWidth(value) )
    pm->chkNotice();
}

void vsnMPP_OctVol_leafSlice::OnAntiAliasChk(wxCommandEvent& event) {
  if ( ! m_pAntiAliasChk ) return;
  vsnMethod_OctVol_leafSlice* pm
    = dynamic_cast<vsnMethod_OctVol_leafSlice*>(p_method);
  if ( ! pm ) return;

  bool value = m_pAntiAliasChk->GetValue();
  if ( pm->setAntiAliasMode(value) )
    pm->chkNotice();
}


//----------------------------------------------------------------
// class vsnMethod_OctVol_leafSlice
//----------------------------------------------------------------

/* constructors / destructor */

vsnMethod_OctVol_leafSlice::vsnMethod_OctVol_leafSlice(const string& name)
  : vsnMethodObj(name), m_sliceAxis(2), m_slicePlane(0), m_lineWidth(1.f),
    m_leafs(NULL), m_pcube(NULL)
{
  m_showType = RT_WIRE;
}

vsnMethod_OctVol_leafSlice::~vsnMethod_OctVol_leafSlice() {
  if ( m_leafs )
    delete m_leafs;
  if ( m_pcube )
    delete m_pcube;
}


/* methods */

bool vsnMethod_OctVol_leafSlice::setSliceParam(const int sa, const int sp) {
  if ( sa == m_sliceAxis && sp == m_slicePlane )
    return true;

  vsnData_OctVol* pData = dynamic_cast<vsnData_OctVol*>(p_refData);
  if ( ! pData ) return false;
  Vec3<size_t> dims = pData->getRootDims();
  
  if ( sa < 0 || sa > 2 ) return false;
  if ( sp < 0 ) return false;

  m_sliceAxis = sa;
  m_slicePlane = sp;
  if ( m_slicePlane >= dims[sa] ) m_slicePlane = dims[sa] -1;

  switch ( m_sliceAxis ) {
  case 0: // YZ
    m_sliceSize.x = dims[1]; m_sliceSize.y = dims[2]; break;
  case 1: // ZX
    m_sliceSize.x = dims[2]; m_sliceSize.y = dims[0]; break;
  case 2: // XY
    m_sliceSize.x = dims[0]; m_sliceSize.y = dims[1]; break;
  } // end of switch(m_sliceAxis)

  if ( ! update() ) return false;
  updateUI();
  return true;
}

bool vsnMethod_OctVol_leafSlice::setLineWidth(const float lw) {
  if ( m_lineWidth == lw ) return true;
  if ( lw <= 0.f ) return false;

  m_lineWidth = lw;

  if ( m_leafs )
    m_leafs->getPrivateMaterial()->setLineWidth(m_lineWidth);

  updateUI();
  return true;
}


/* from vsnMethodObj */

bool vsnMethod_OctVol_leafSlice::update(const bool force) {
  return updateStep(m_requestedStp, force);
}

vsnMethodPP* vsnMethod_OctVol_leafSlice::getParamPanel(wxPanel* pp) {
  vsnMPP_OctVol_leafSlice* pp_leafSlice
    = new vsnMPP_OctVol_leafSlice(pp, this);
  if ( ! pp_leafSlice ) return NULL;
  return pp_leafSlice;
}


/* from vsnTimeSeriesMethodIF */

bool vsnMethod_OctVol_leafSlice::updateStep(const int stp,
					   const bool force,
					   const bool cascade)
{
  vsnData_OctVol* pData = dynamic_cast<vsnData_OctVol*>(p_refData);
  if ( ! pData ) return false;
  m_requestedStp = stp;
  if ( m_requestedStp < 0 )
    m_requestedStp = pData->getCurrentStepIdx();

  // slice size
  Vec3<size_t> dims = pData->getRootDims();
  size_t slSz = m_sliceSize.x * m_sliceSize.y;
  if ( slSz < 1 ) {
    m_slicePlane = dims[m_sliceAxis] / 2;
    m_sliceSize.x = dims[(m_sliceAxis+1)%3];
    m_sliceSize.y = dims[(m_sliceAxis+2)%3];
    slSz = m_sliceSize.x * m_sliceSize.y;
    m_updatedStp = -1; // force
  }

  if ( force ) m_updatedStp = -1;
  if ( m_updatedStp == m_requestedStp ) return true;
  m_updatedStp = -1;

  // leafs data
  if ( ! m_leafs ) {
    m_leafs = new vfrPrimSet();
    if ( ! m_leafs ) {
      ErrMsg(MsgERR, getMethodType() + string("[") + getName()
             + string("]: memory allocation failed"));
      return false;
    }
    m_leafs->alcMaterial();

    if ( ! m_pcube ) {
      m_pcube = new vfrCube();
      if ( ! m_pcube ) {
	ErrMsg(MsgERR, getMethodType() + string("[") + getName()
	       + string("]: memory allocation failed"));
	delete m_leafs; m_leafs = NULL;
	return false;
      }
    } // end of if(!m_pcube)
    m_leafs->setPrimitive(m_pcube);
    m_leafs->setRotScaleMode(VFR_PRIMSET_SCALE);

    addChild(m_leafs);
  }
  m_leafs->getPrivateMaterial()->setRenderMode(RT_NONE);
  if ( ! m_show ) {
    return true;
  }

  // get Node list, params
  const set<vsnOctTree::Node*>& leafLst = pData->getLeafNodeList();
  Vec3<float> rootPitch = pData->getPitch();
  const CES::Vec3<float>* dbb = pData->getBbox();

  // resize primitive cube
  m_pcube->setWidth(rootPitch.m_v[0]);
  m_pcube->setHeight(rootPitch.m_v[1]);
  m_pcube->setDepth(rootPitch.m_v[2]);

  // alloc verts / normals
  deque<vsnOctTree::Node*> sliceLeafLst;
  set<vsnOctTree::Node*>::const_iterator its;
  for ( its = leafLst.begin(); its != leafLst.end(); its++ ) {
    if ( ! (*its) ) continue;
    vsnOctTree::Node* pn = (*its);
    // MHIR append begin
    if (! pn -> HasData()) continue;
    // MHIR append end
    while ( pn->p_parent ) pn = pn->p_parent;
    vsnOctTree::RootNode* prn = dynamic_cast<vsnOctTree::RootNode*>(pn);
    if ( ! prn ) continue;
    if ( prn->m_idx[m_sliceAxis] != m_slicePlane ) continue;
    sliceLeafLst.push_back(*its);
  }
  size_t lnSz = sliceLeafLst.size();
  if ( lnSz < 1 ) return true;
  if ( ! m_leafs->alcVerts(lnSz) ||
       ! m_leafs->alcNormals(lnSz) ) {
    ErrMsg(MsgERR, getMethodType() + string("[") + getName()
	   + string("]: memory allocation failed"));
    return false;
  }
  vector3* vl = m_leafs->getVerts();
  vector3* nl = m_leafs->getNormals();

  // set verts / normals
  register size_t l, i = 0;
  vector3 wkv = {0.f, 0.f, 0.f};
  deque<vsnOctTree::Node*>::iterator it;
  for ( it = sliceLeafLst.begin(); it != sliceLeafLst.end(); it++ ) {
    if ( ! (*it) ) continue;
    register size_t lvl = (*it)->GetLevel();
    register float scaleFac = 1.f;
    for ( l = 0; l < lvl; l++ ) scaleFac *= 0.5f;
    vsnOctTree::Node* pnode = *it;

    Vec3<float> orig, size = rootPitch;
    PedigType pedig = pnode->m_pedigree;
    while ( pedig && pnode ) {
      register int relp = pedig % 10 -1;
      if ( relp & (0x1)    ) orig.m_v[0] += rootPitch.m_v[0] * scaleFac;
      if ( relp & (0x1<<1) ) orig.m_v[1] += rootPitch.m_v[1] * scaleFac;
      if ( relp & (0x1<<2) ) orig.m_v[2] += rootPitch.m_v[2] * scaleFac;
      size = size * 0.5f;
      scaleFac *= 2.f;

      pedig = pedig / 10;
      pnode = pnode->p_parent;
    } // end of while(pedig && pnode)

    vsnOctTree::RootNode* proot = dynamic_cast<vsnOctTree::RootNode*>(pnode);
    if ( ! proot ) continue;

    orig.m_v[0] += rootPitch.m_v[0] * (float)proot->m_idx[0];
    orig.m_v[1] += rootPitch.m_v[1] * (float)proot->m_idx[1];
    orig.m_v[2] += rootPitch.m_v[2] * (float)proot->m_idx[2];
    orig = orig + dbb[0];
    orig = orig + (size * 0.5f); // move to center

    m_leafs->setVert(i, orig.m_v, FALSE);
    wkv[0] = size.m_v[0]/rootPitch.m_v[0]; m_leafs->setNormal(i, wkv);

    i ++;
  } // end of for(it)

  m_leafs->generateBbox();

  // ok
  m_leafs->getPrivateMaterial()->setRenderMode(m_showType);
  setBaseColor(m_colour);
  setHilight(m_hilight);
  m_updatedStp = m_requestedStp;
  return true;
}


/* from vsnIoObject */

bool vsnMethod_OctVol_leafSlice::parseXML(xmlNodePtr xnp) {
  static xmlChar* xs;
  xmlNodePtr cur;
  int axis(-1), plane(-1);

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

      if ( xsN == string("axis") ) {
        if ( xsV == "0" || xsV == "x" || xsV == "X" )
          axis = 0;
        else if ( xsV == "1" || xsV == "y" || xsV == "Y" )
          axis = 1;
        else if ( xsV == "2" || xsV == "z" || xsV == "Z" )
          axis = 2;
        else {
          ErrMsg(MsgERR, msgHdr + string("invalid value in param axis"));
          axis = -1;
          goto _NEXT_XML_NODE;
        }
      } // end of "axis"
      else if ( xsN == string("plane") ) {
        plane = atoi(xsV.c_str());
        if ( plane < 0 ) {
          ErrMsg(MsgERR, msgHdr + string("invalid value in param plane"));
          plane = -1;
          goto _NEXT_XML_NODE;
        }
      } // end of "plane"
      else if ( xsN == string("line_width") ) {
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

  // update slice parameters
  vsnData_OctVol* pData = dynamic_cast<vsnData_OctVol*>(p_refData);
  if ( pData ) {
    Vec3<size_t> dims = pData->getRootDims();
    if ( axis < 0 || plane < 0 ) {
      if ( axis < 0 ) axis = 2; // Z
      if ( plane < 0 ) plane = dims[axis] / 2;
    }
    if ( plane >= dims[axis] ) {
      ErrMsg(MsgERR, msgHdr + string("param plane is out of range"));
      plane = dims[axis] / 2;
    }

    if ( ! setSliceParam(axis, plane) ) {
      ErrMsg(MsgERR, msgHdr + string("set slice params failed"));
      return false;
    }
  }

  return true;
}

bool vsnMethod_OctVol_leafSlice::outputXML(std::ostream& os, const size_t ts)
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
  // axis
  if ( m_sliceAxis != 2 ) {
    os << idts_2 << "<param name=\"axis\" value=\"";
    switch ( m_sliceAxis ) {
    case 0: os << "X"; break;
    case 1: os << "Y"; break;
    case 2: os << "Z"; break;
    default: ret = false;
    } // end of switch(m_sliceAxis)
    os << "\" />" << endl;
  }

  // plane
  vsnData_OctVol* pData = dynamic_cast<vsnData_OctVol*>(p_refData);
  if ( pData ) {
    Vec3<size_t> dims = pData->getRootDims();
    if ( m_slicePlane != dims[m_sliceAxis] / 2 ) {
      os << idts_2 << "<param name=\"plane\" value=\""
         << m_slicePlane << "\" />" << endl;
    }
  }

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

bool vsnMethod_OctVol_leafSlice::commandXML(xmlNodePtr xnp) {
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

  if ( nameStr == "set_axis" ) {
    int axis = -1, plane = -1;
    if ( valueStr == "0" || valueStr == "x" || valueStr == "X" ) axis = 0;
    else if ( valueStr == "1" || valueStr == "y" || valueStr == "Y" ) axis = 1;
    else if ( valueStr == "2" || valueStr == "z" || valueStr == "Z" ) axis = 2;
    else {
      ErrMsg(MsgERR, msgHdr +
             string("command set_axis: invalid value") + valueStr);
      return false;
    }
    vsnData_OctVol* pData = dynamic_cast<vsnData_OctVol*>(p_refData);
    if ( ! pData ) {
      ErrMsg(MsgERR, msgHdr +
             string("command set_axis: can't set slice params"));
      return false;
    }
    Vec3<size_t> dims = pData->getRootDims();
    plane = dims[axis] / 2;
    if ( ! setSliceParam(axis, plane) ) {
      ErrMsg(MsgERR, msgHdr +
             string("command set_axis: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_axis"
  else if ( nameStr == "set_plane" ) {
    int plane = atoi(valueStr.c_str());
    if ( ! setSliceParam(m_sliceAxis, plane) ) {
      ErrMsg(MsgERR, msgHdr +
             string("command set_plane: set failed: ") + valueStr);
      return false;
    }
  } // end of "set_plane"
  else if ( nameStr == "set_line_width" ) {
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


/* override vfrGroup method */

void vsnMethod_OctVol_leafSlice::generateBbox() {
  vsnData_OctVol* pData = dynamic_cast<vsnData_OctVol*>(p_refData);
  if ( ! pData ) return;

  const CES::Vec3<float>* dbb = pData->getBbox();
  if ( ! dbb ) return;

  _bbox[0] = dbb[0];
  _bbox[1] = dbb[1];
}

