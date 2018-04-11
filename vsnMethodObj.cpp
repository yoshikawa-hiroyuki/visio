//
// vsnMethodObj
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

#include "vsnMethodObj.h"
#include "vsnColorBar.h"
#include "vsnUiView.h"
#include "vsnError.h"

using namespace std;
using namespace VFR;
using namespace VSN;


//----------------------------------------------------------------
// class vsnMethodPP
//----------------------------------------------------------------

/* constructors / destructor */

vsnMethodPP::vsnMethodPP(wxPanel* parent, class vsnMethodObj* pm)
  : wxPanel(parent), p_method(pm)
{
  if ( p_method )
    p_method->appendMPP(this);
}

vsnMethodPP::~vsnMethodPP() {
  if ( p_method )
    p_method->removeMPP(this);
}


/* methods */

void vsnMethodPP::addTo(wxPanel* parent) {
  if ( ! parent ) return;
  wxSizer* upSizer = parent->GetSizer();
  if ( ! upSizer ) return;
  upSizer->Add(this, 1, wxEXPAND|wxALL, 3);
}

vsnUiView* vsnMethodPP::getUiView() {
  vsnUiView* puiv = NULL;
  wxWindow* pp = GetParent();
  while ( pp ) {
    puiv = dynamic_cast<vsnUiView*>(pp);
    if ( puiv ) break;
    pp = pp->GetParent();
  } // end of while(pp)
  return puiv;
}

vsnMethodParamCanvas* vsnMethodPP::getMPC() {
  vsnMethodParamCanvas* pmpc = NULL;
  wxWindow* pp = GetParent();
  while ( pp ) {
    pmpc = dynamic_cast<vsnMethodParamCanvas*>(pp);
    if ( pmpc ) break;
    pp = pp->GetParent();
  } // end of while(pp)
  return pmpc;
}

void vsnMethodPP::methodDeleted(vsnMethodObj* pm) {
  if ( ! pm ) return;
  if ( p_method == pm ) {
    settlement();

    vsnUiView* puiv = getUiView();
    if ( puiv ) {
      puiv->selectObj(pm->getRefData());
    } // end of if(puiv)

    p_method = NULL;
  }
}


//----------------------------------------------------------------
// class vsnMethodLutRefer
//----------------------------------------------------------------

/* constructors / destructor */

vsnMethodLutRefer::vsnMethodLutRefer(class vsnMethodObj* pm)
  : p_method(pm)
{
  if ( p_method )
    p_method->appendLutRefer(this);
}

vsnMethodLutRefer::~vsnMethodLutRefer() {
  if ( p_method )
    p_method->removeLutRefer(this);  
}

/* methods */

void vsnMethodLutRefer::methodDeleted(vsnMethodObj* pm) {
  if ( ! pm ) return;
  if ( p_method == pm ) {
    p_method = NULL;
    (void)updateLut();
  }
}

bool vsnMethodLutRefer::setMethodObj(class vsnMethodObj* pm) {
  if ( p_method == pm ) return true;
  p_method = pm;
  return updateLut();
}


//----------------------------------------------------------------
// class vsnMethodObj
//----------------------------------------------------------------

/* static members */

unsigned long vsnMethodObj::s_mtdSeq;


/* constructors / destructor */

vsnMethodObj::vsnMethodObj(const std::string& name)
  : vfrGroup(name, FALSE), vsnIoObject(),
    m_useLut(false), m_pColorBar(NULL), m_antiAlias(false),
    m_show(true), m_showType(RT_SMOOTH), p_refData(NULL)
{
  alcMaterial();
  setPickMode(PT_OBJECT|PT_BBOX);

  m_colour[0] = m_colour[1] = m_colour[2] = m_colour[3] = 1.f;
  m_hilight = 0;

  m_mtdSeq = s_mtdSeq++;
}

vsnMethodObj::~vsnMethodObj() {
  register int n = getNumChildren();
  while ( n > 0 ) {
    vfrNode* p = getChild(n - 1);
    if ( p ) delete p;
    n = getNumChildren();
  }

  if ( m_pColorBar ) delete m_pColorBar;

  set<vsnMethodPP*>::iterator it_pp;

  set<vsnMethodParamCanvas*> mpcLst;
  for ( it_pp = m_mppLst.begin(); it_pp != m_mppLst.end(); it_pp++ ) {
    if ( ! *it_pp ) continue;
    vsnMethodParamCanvas* pmpc = (*it_pp)->getMPC();
    if ( pmpc ) mpcLst.insert(pmpc);
  } // end of for(it_pp)

  set<vsnMethodParamCanvas*>::iterator it_mpc;
  for ( it_mpc = mpcLst.begin(); it_mpc != mpcLst.end(); it_mpc++ )
    if ( *it_mpc ) (*it_mpc)->delMethod(this);

  for ( it_pp = m_mppLst.begin(); it_pp != m_mppLst.end(); it_pp++ )
    if ( *it_pp ) (*it_pp)->methodDeleted(this);

  set<vsnMethodLutRefer*>::iterator it_lr;
  for ( it_lr = m_mlrLst.begin(); it_lr != m_mlrLst.end(); it_lr++ )
    if ( *it_lr ) (*it_lr)->methodDeleted(this);
}


/* methods */

void vsnMethodObj::appendMPP(vsnMethodPP* pmpp) {
  if ( ! pmpp ) return;
  m_mppLst.insert(pmpp);
}

void vsnMethodObj::removeMPP(vsnMethodPP* pmpp) {
  set<vsnMethodPP*>::iterator it = m_mppLst.find(pmpp);
  if ( it == m_mppLst.end() ) return;
  m_mppLst.erase(it);
}


void vsnMethodObj::updateUI() {
  set<vsnMethodPP*>::iterator it;
  for ( it = m_mppLst.begin(); it != m_mppLst.end(); it++ ) {
    if ( ! *it ) continue;
    vsnUiView* puiv = (*it)->getUiView();
    if ( puiv ) {
      if ( ! puiv->updateMethod(this, false) ) continue;
    }
    (*it)->update();
  } // end of for(it)
}


void vsnMethodObj::appendLutRefer(vsnMethodLutRefer* pmlr) {
  if ( ! pmlr ) return;
  m_mlrLst.insert(pmlr);
}

void vsnMethodObj::removeLutRefer(vsnMethodLutRefer* pmlr) {
  set<vsnMethodLutRefer*>::iterator it = m_mlrLst.find(pmlr);
  if ( it == m_mlrLst.end() ) return;
  m_mlrLst.erase(it);
}


void vsnMethodObj::setRefData(vsnDataObj* prd) {
  if ( p_refData == prd ) return;
  p_refData = prd;
  update();
}


void vsnMethodObj::setShow(const bool mode) {
  if ( m_show == mode ) return;
  m_show = mode;
  _material->setRenderMode(m_show ? m_showType : RT_NONE);
  if ( m_pColorBar ) {
    vfrMaterial* pcm = m_pColorBar->getPrivateMaterial();
    if ( pcm ) pcm->setRenderMode(m_show ? RT_NOLIGHT : RT_NONE);
  }
  update(false);
  updateUI();
}


bool vsnMethodObj::getLighting() const {
  return ((m_showType==RT_SMOOTH) ? true : false);
}

void vsnMethodObj::setLighting(const bool mode) {
  if ( mode && m_showType==RT_SMOOTH ) return;
  if ( ! mode && m_showType==RT_NOLIGHT ) return;
  if ( ! canLighting() && mode ) return;

  m_showType = mode ? RT_SMOOTH : RT_NOLIGHT;

  _material->setRenderMode(m_show ? m_showType : RT_NONE);
  for ( int i = 0; i < nChild; i++ ) {
    if ( ! _children[i] ) continue;
    vfrMaterial* pmate = _children[i]->getPrivateMaterial();
    if ( ! pmate ) continue;
    if ( pmate->getRenderMode() != RT_NONE )
      pmate->setRenderMode(m_showType);
  } // end of for(i)

  updateUI();
  chkNotice();
}


bool vsnMethodObj::setAntiAliasMode(const bool aam) {
  if ( m_antiAlias == aam ) return true;
  m_antiAlias = aam;

  register int i, j;
  for ( i = 0; i < getNumChildren(); i++ ) {
    vfrNode* pnode = getChild(i);
    if ( ! pnode ) continue;
    if ( pnode->getRenderMode() & (RT_SMOOTH|RT_FLAT|RT_NOLIGHT) ) {
      pnode->setTransparency(m_colour[3] < OPAC_THRESH);
      pnode->setAlpha(m_colour[3]);
    } else {
      pnode->setTransparency(m_antiAlias);
      pnode->setAlpha(1.f);
    }
    
    vfrGroup* pgrp = dynamic_cast<vfrGroup*>(pnode);
    if ( ! pgrp ) continue;
    for ( j = 0; j < pgrp->getNumChildren(); j++ ) {
      vfrNode* ppnode = pgrp->getChild(j);
      if ( ! ppnode ) continue;
      if ( ppnode->getRenderMode() & (RT_SMOOTH|RT_FLAT|RT_NOLIGHT) ) {
	ppnode->setTransparency(m_colour[3] < OPAC_THRESH);
	ppnode->setAlpha(m_colour[3]);
      } else {
	ppnode->setTransparency(m_antiAlias);
	ppnode->setAlpha(1.f);
      }
    } // end of for(j)
  } // end of for(i)

  updateUI();
  return true;
}


void vsnMethodObj::setBaseColor(const vector4 cv) {
  memcpy(m_colour, cv, sizeof(vector4));
  //m_colour[3] = (float)((int)(m_colour[3] * 10) / 10.f);
  updateUI();

  if ( m_pColorBar )
    m_pColorBar->setBaseColor(m_colour);

  if ( ! m_useLut ) {
    register int i, j;
    for ( i = 0; i < getNumChildren(); i++ ) {
      vfrNode* pnode = getChild(i);
      if ( ! pnode ) continue;
      pnode->setColor3(0, m_colour);
      if ( pnode->getRenderMode() & (RT_SMOOTH|RT_FLAT|RT_NOLIGHT) ) {
	pnode->setTransparency(m_colour[3] < OPAC_THRESH);
	pnode->setAlpha(m_colour[3]);
      }

      vfrGroup* pgrp = dynamic_cast<vfrGroup*>(pnode);
      if ( ! pgrp ) continue;
      for ( j = 0; j < pgrp->getNumChildren(); j++ ) {
	vfrNode* ppnode = pgrp->getChild(j);
	if ( ! ppnode ) continue;
	ppnode->setColor3(0, m_colour);
	if ( ppnode->getRenderMode() & (RT_SMOOTH|RT_FLAT|RT_NOLIGHT) ) {
	  ppnode->setTransparency(m_colour[3] < OPAC_THRESH);
	  ppnode->setAlpha(m_colour[3]);
	}
      } // end of for(j)
    } // end of for(i)
  } // end of if(!m_useLut)
  else {
    register int i, j;
    for ( i = 0; i < getNumChildren(); i++ ) {
      vfrNode* pnode = getChild(i);
      if ( ! pnode ) continue;
      if ( pnode->getRenderMode() & (RT_SMOOTH|RT_FLAT|RT_NOLIGHT) ) {
	pnode->setTransparency(m_colour[3] < OPAC_THRESH);
	pnode->setAlpha(m_colour[3]);
      }

      vfrGroup* pgrp = dynamic_cast<vfrGroup*>(pnode);
      if ( ! pgrp ) continue;
      for ( j = 0; j < pgrp->getNumChildren(); j++ ) {
	vfrNode* ppnode = pgrp->getChild(j);
	if ( ! ppnode ) continue;
	if ( ppnode->getRenderMode() & (RT_SMOOTH|RT_FLAT|RT_NOLIGHT) ) {
	  ppnode->setTransparency(m_colour[3] < OPAC_THRESH);
	  ppnode->setAlpha(m_colour[3]);
	}
      } // end of for(j)
    } // end of for(i)
  } // end of if(m_useLut)

  chkNotice();
}

void vsnMethodObj::setHilight(const float hl) {
  if ( m_hilight == hl ) return;
  m_hilight = hl;
  if ( m_hilight < 0.f ) m_hilight = 0.f;
  else if ( m_hilight > 1.f ) m_hilight = 1.f;

  updateUI();
  if ( canLighting() ) {
    register int i, j;
    for ( i = 0; i < getNumChildren(); i++ ) {
      vfrNode* pnode = getChild(i);
      if ( ! pnode ) continue;
      vfrMaterial* pmate = pnode->getPrivateMaterial();
      if ( pmate ) pmate->setSpecular(m_hilight, m_hilight, m_hilight, 1.f);

      vfrGroup* pgrp = dynamic_cast<vfrGroup*>(pnode);
      if ( ! pgrp ) continue;
      for ( j = 0; j < pgrp->getNumChildren(); j++ ) {
	vfrNode* ppnode = pgrp->getChild(j);
	if ( ! ppnode ) continue;
	pmate = ppnode->getPrivateMaterial();
	if ( pmate ) pmate->setSpecular(m_hilight, m_hilight, m_hilight, 1.f);
      } // end of for(j)
    } // end of for(i)

    chkNotice();
  } // end of if(canLighting)
}

void vsnMethodObj::getBaseColor(vector4 cv) const {
  memcpy(cv, m_colour, sizeof(vector4));
}

void vsnMethodObj::setUseLut(const bool ulm) {
  if ( m_useLut == ulm ) return;
  m_useLut = ulm;

  if ( m_useLut )
    update();
  else
    setBaseColor(m_colour);

  chkNotice();
}

void vsnMethodObj::setLut(const vsnLut& ol) {
  m_lut = ol;
  updateUI();

  set<vsnMethodLutRefer*>::iterator it;
  for ( it = m_mlrLst.begin(); it != m_mlrLst.end(); it++ )
    if ( *it ) (*it)->updateLut();

  if ( m_useLut )
    update();

  if ( m_useLut || m_mlrLst.size() > 0 )
    chkNotice();
}


vsnColorBar* vsnMethodObj::getColorBar() {
  if ( ! m_pColorBar ) {
    m_pColorBar = new vsnColorBar(this);
    if ( ! m_pColorBar ) return NULL;
    m_pColorBar->setBaseColor(m_colour);
    appendLutRefer(m_pColorBar);
  } // end of if(!m_pColorBar)

  m_pColorBar->setLut(m_lut);
  return m_pColorBar;
}

bool vsnMethodObj::showColorBar(const bool scbm) {
  // get colorBar
  vsnColorBar* pcb = getColorBar();
  if ( ! pcb ) return false;

  // get the scene
  vsnApp* pApp = vsnApp::GetApp();
  if ( ! pApp ) return false;
  size_t nsc = pApp->getNumScene();
  vsnScene* psc = NULL;
  for ( size_t i = 0; i < nsc; i++ ) {
    vsnScene* xsc = pApp->getScene(i);
    if ( ! xsc ) continue;
    if ( xsc->getNode(this->getID()) ) {
      psc = xsc;
      break;
    }
  } // end of for(i)
  if ( ! psc ) return false;

  // show/hide
  psc->delFrontObj(pcb);
  if ( scbm ) {
    psc->addFrontObj(pcb);
    if ( ! m_show ) {
      vfrMaterial* pcm = m_pColorBar->getPrivateMaterial();
      if ( pcm ) pcm->setRenderMode(m_show ? RT_NOLIGHT : RT_NONE);
    }
  }

  chkNotice();
  return true;
}


/* serialize methods */

bool vsnMethodObj::parseXML(xmlNodePtr xnp) {
  static xmlChar* xs;
  xmlNodePtr cur;

  if ( ! xnp ) return false;

  // is 'method' node?
  if ( strcmp((const char*)xnp->name, "method") ) return false;

  // type check
  if ( xs ) xmlFree(xs);
  xs = xmlGetProp(xnp, (const xmlChar*)"type");
  if ( ! xs || strlen((const char*)xs) < 1 ) return false;
  if ( string((const char*)xs) != getMethodType() ) return false;

  // get name
  if ( xs ) xmlFree(xs);
  xs = xmlGetProp(xnp, (const xmlChar*)"name");
  if ( xs && strlen((const char*)xs) > 0 ) {
    setName((const char*)xs);
  } else {
    //setName(VFR_NONAME);
    char txtbuf[64];
    sprintf(txtbuf, "%s_%lu", getMethodType().c_str(), getMethodSeq());
    vsnScene* pscn = NULL;
    if ( p_refData ) pscn = p_refData->getAncestorScene();
    if ( pscn && pscn->getNode(txtbuf) ) {
      string wkstr(txtbuf);
      register size_t xt;
      for ( xt = 1; ; xt++ ) {
	sprintf(txtbuf, "%s-%lu", wkstr.c_str(), xt);
	if ( ! pscn->getNode(txtbuf) ) break;
      } // end of for(xt)
    }
    setName(txtbuf);
  }
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
      if ( ! xs || strlen((const char*)xs) < 1 ) {
	ErrMsg(MsgERR, msgHdr + string("param node without name, ignore"));
	goto _NEXT_XML_NODE;
      }
      xsN = (const char*)xs;
      if ( xs ) xmlFree(xs);
      xs = xmlGetProp(cur, (const xmlChar*)"value");
      if ( xs && strlen((const char*)xs) > 0 ) xsV = (const char*)xs;
      else xsV = "";

      if ( xsN == string("show") ) {
	bool showMode = (xsV == string("yes"));
	setShow(showMode);
      } // end of "show"
      else if ( xsN == string("lighting") ) {
	bool lightMode = (xsV == string("yes"));
	if ( canLighting() ) {
	  setLighting(lightMode);
	} else {
	  if ( lightMode ) {
	    ErrMsg(MsgWARN, msgHdr + string("can't enable lighting, ignore"));
	    goto _NEXT_XML_NODE;
	  }
	}
      } // end of "lighting"
      else if ( xsN == string("base_color") ) {
	if ( xsV.empty() ) {
	  ErrMsg(MsgERR, msgHdr + string("invalid base_color: no value"));
	  goto _NEXT_XML_NODE;
	}
	vector4 cv = {1.f, 1.f, 1.f, 1.f};
	int nscan = sscanf(xsV.c_str(), "%f %f %f %f",
			   &cv[0], &cv[1], &cv[2], &cv[3]);
	if ( nscan < 3 ) {
	  ErrMsg(MsgERR, msgHdr + string("invalid base_color"));
	  goto _NEXT_XML_NODE;
	}
	if ( nscan < 4 ) cv[3] = 1.f;
	setBaseColor(cv);
      } // end of "base_color"
      else if ( xsN == string("hilight") ) {
	if ( xsV.empty() ) {
	  ErrMsg(MsgERR, msgHdr + string("invalid hilight: no value"));
	  goto _NEXT_XML_NODE;
	}
	float hilight = atof(xsV.c_str());
	setHilight(hilight);
      } // end of "hilight"
    } // end of param
    else if ( !strcmp((const char*)cur->name, "cmap") ||
	      !strcmp((const char*)cur->name, "lut") ) {
      xmlNodePtr lutNode = cur->xmlChildrenNode;
      for ( ; lutNode; lutNode = lutNode->next ) {
	if ( ! lutNode || lutNode->type != XML_TEXT_NODE )
	  continue;
	if ( ! lutNode->content || strlen((const char*)lutNode->content) < 1 )
	  continue;
	istringstream iss((const char*)lutNode->content);
	if ( ! m_lut.ImportStream(iss) ) {
	  ErrMsg(MsgERR, msgHdr + string("invalid lut node, ignore"));
	  m_lut = vsnLut();
	  continue;
	}
	m_lut.normalize();
      } // end of for(lutNode)
    } // end of lut
    else if ( !strcmp((const char*)cur->name, "color_bar") ) {
      bool showVal = false;
      string showStr;
      if ( xs ) xmlFree(xs);
      xs = xmlGetProp(cur, (const xmlChar*)"show");
      if ( xs && strlen((const char*)xs) > 0 ) showStr = (const char*)xs;
      if ( showStr == "yes" ) showVal = true;

      vsnColorBar* pcb = getColorBar();
      if ( ! pcb ) {
	ErrMsg(MsgERR, msgHdr + string("can't create color_bar"));
	goto _NEXT_XML_NODE;
      }
      if ( ! pcb->importXMLNode(cur) ) {
	ErrMsg(MsgERR, msgHdr + string("import color_bar XML node failed"));
	goto _NEXT_XML_NODE;
      }
      if ( ! showColorBar(showVal) ) {
	ErrMsg(MsgERR, msgHdr + string("set show color_bar failed"));
	goto _NEXT_XML_NODE;
      }
    } // end of color_bar
    else if ( !strcmp((const char*)cur->name, "anchor") ) {
      if ( ! parseAnchorXML(cur) ) {
        ErrMsg(MsgERR, msgHdr + string("anchor node parse failed, ignore"));
        goto _NEXT_XML_NODE;
      }
    } // end of anchor


  _NEXT_XML_NODE:
    cur = cur->next;
  } // end of while(cur)

  return true;
}

bool vsnMethodObj::commandXML(xmlNodePtr xnp) {
  static xmlChar* xs;
  string msgHdr
    = string("MethodObj[") + getName() + string("]: commandXML: ");

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

  // get command value (if there)
  if ( xs ) xmlFree(xs);
  xs = xmlGetProp(xnp, (const xmlChar*)"value");
  if ( xs && strlen((const char*)xs) > 0 )
    valueStr = string((const char*)xs);

  // do the command
  if ( nameStr == "set_name" ) {
    if ( valueStr.empty() )
      setName(VFR_NONAME);
    else
      setName(valueStr);
  }
  else if ( nameStr == "set_show" ) {
    if ( valueStr == string("yes") )
      setShow(true);
    else if ( valueStr == string("no") )
      setShow(false);
    else {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_show: invalid value: ") + valueStr);
      return false;
    }
  }
  else if ( nameStr == "set_lighting" ) {
    if ( valueStr == string("yes") )
      setLighting(true);
    else if ( valueStr == string("no") )
      setLighting(false);
    else {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_lighting: invalid value: ") + valueStr);
      return false;
    }
  }
  else if ( nameStr == string("set_base_color") ) {
    if ( valueStr.empty() ) {
      ErrMsg(MsgERR, msgHdr + string("command base_color: no value"));
      return false;
    }
    vector4 cv = {1.f, 1.f, 1.f, 1.f};
    int nscan = sscanf(valueStr.c_str(), "%f %f %f %f",
		       &cv[0], &cv[1], &cv[2], &cv[3]);
    if ( nscan < 3 ) {
      ErrMsg(MsgERR, msgHdr +string("command set_base_color: lack of values"));
      return false;
    }
    if ( nscan < 4 ) cv[3] = 1.f;
    setBaseColor(cv);    
  }
  else if ( nameStr == string("set_hilight") ) {
    if ( valueStr.empty() ) {
      ErrMsg(MsgERR, msgHdr + string("command set_hilight: no value"));
      return false;
    }
    float hilight = atof(valueStr.c_str());
    setHilight(hilight);
  }
  else if ( nameStr == string("set_cmap") || nameStr == string("set_lut") ) {
    xmlNodePtr lutNode = xnp->xmlChildrenNode;
    for ( ; lutNode; lutNode = lutNode->next ) {
      if ( ! lutNode || lutNode->type != XML_TEXT_NODE )
	continue;
      if ( ! lutNode->content || strlen((const char*)lutNode->content) < 1 )
	continue;
      istringstream iss((const char*)lutNode->content);
      if ( ! m_lut.ImportStream(iss) ) {
	ErrMsg(MsgERR, msgHdr + string("command ") + nameStr +
	       string(": invalid lut node, ignore"));
	m_lut = vsnLut();
	continue;
      }
      m_lut.normalize();
      update();
      break;
    } // end of for(lutNode)
  }
  else if ( nameStr == string("show_color_bar") ) {
    bool scm;
    if ( valueStr == string("yes") )
      scm = true;
    else if ( valueStr == string("no") )
      scm = false;
    else {
      ErrMsg(MsgERR, msgHdr +
	     string("command show_color_bar: invalid value: ") + valueStr);
      return false;
    }    
    if ( ! showColorBar(scm) ) {
      ErrMsg(MsgERR, msgHdr +
	     string("command show_color_bar: set failed") + valueStr);
      return false;
    }
  }
  else if ( nameStr == string("set_anchor") ) {
    xmlNodePtr cur = xnp->xmlChildrenNode;
    while ( cur ) {
      if ( (cur = vsnIoObject::SkipCommentXML(cur)) == NULL ) break;
      if ( !strcmp((const char*)cur->name, "anchor") ) {
        if ( ! parseAnchorXML(cur) )
          ErrMsg(MsgERR, msgHdr +
                 "command set_anchor: anchor node parse failed, ignore");
        break;
      }
      cur = cur->next;
    } // end of while(cur)
  }
  else {
    // not 'base MethodObj' command
    return false;
  }

  return true; 
}


bool vsnMethodObj::exportXMLNode(std::ostream& os, const size_t ts) const {
  std::string idts;
  for ( int i = 0; i < ts; i++ ) idts.push_back(' ');
  bool ret = true;

  // output show param
  if ( ! getShow() ) {
    os << idts << "<param name=\"show\" value=\"no\" />" << endl;
  }
  
  // output lighting param
  if ( canLighting() ) {
    os << idts << "<param name=\"lighting\" value=\""
       << (getLighting() ? "yes" : "no")
       << "\" />" << endl;
  }

  // output base_color param
  vector4 cv; getBaseColor(cv);
  if ( cv[0] != 1.f || cv[1] != 1.f || cv[2] != 1.f || cv[3] != 1.f ) {
    os << idts << "<param name=\"base_color\" value=\""
       << cv[0] << " " << cv[1] << " " << cv[2] << " " << cv[3]
       << "\" />" << endl;
  }

  // output hilight param
  float hl = getHilight();
  if ( hl != 0.f ) {
    os << idts << "<param name=\"hilight\" value=\"" << hl << "\" />" << endl;
  }

  // output cmap node
  if ( ! m_lut.isStdLut ) {
    os << idts << "<cmap>" << endl;
    if ( ! m_lut.ExportStream(os, ts+2) )
      ret = false;
    os << idts << "</cmap>" << endl;
  }

  // output color_bar node
  if ( m_pColorBar ) {
    // get show/hide
    bool show_cbmode = false;
    size_t nsc = 0;
    vsnScene* psc = NULL;
    vfrGroup* pfg = NULL;
    vsnApp* pApp = vsnApp::GetApp();
    if ( ! pApp ) goto END_CBNODE;
    nsc = pApp->getNumScene();
    for ( size_t i = 0; i < nsc; i++ ) {
      vsnScene* xsc = pApp->getScene(i);
      if ( ! xsc ) continue;
      if ( xsc->getNode(this->getID()) ) {
	psc = xsc;
	break;
      }
    } // end of for(i)
    if ( ! psc ) goto END_CBNODE;
    pfg = psc->getFrontObjGrp();
    if ( ! pfg ) goto END_CBNODE;
    show_cbmode = (pfg->getNode(m_pColorBar->getID()) ? true : false);
    if ( ! m_pColorBar->exportXMLNode(os, show_cbmode, ts) )
      ret = false;
  END_CBNODE:
    ;
  }

  // output anchor node
  outputAnchorXML(os, ts);

  return ret;
}

bool vsnMethodObj::isAcceptableData(const std::string& dtype) const {
  deque<string> dtLst = getDataTypes();
  deque<string>::iterator it;
  for ( it = dtLst.begin(); it != dtLst.end(); it++ ) {
    if ( (*it) == string("*") ) return true;
    if ( dtype == (*it) ) return true;
  } // end of for(it)
  return false;
}
