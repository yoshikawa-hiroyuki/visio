//
// vsnMethodTmpl
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

#include "vsnMethodTmpl.h"
#include "vsnApp.h"
#include "vsnDataObj.h"
#include "vsnError.h"
#include "vfruGetLine.h"

using namespace std;


//----------------------------------------------------------------
// class vsnMethodTmpl
//----------------------------------------------------------------

/* static members */

unsigned long vsnMethodTmpl::s_tmplSeq;


/* constructors / destructor */

vsnMethodTmpl::vsnMethodTmpl(const string& path)
  : m_pXmlNode(NULL)
{
  m_tmplSeq = s_tmplSeq++;

  (void)loadXmlFile(path);
}

vsnMethodTmpl::~vsnMethodTmpl() {
  if ( m_pXmlNode ) {
    xmlUnlinkNode(m_pXmlNode);
    xmlFreeNode(m_pXmlNode);
    m_pXmlNode = NULL;
  }
}


/* methods */

bool vsnMethodTmpl::isSupportedDataType(const string& dt) const {
  if ( dt.empty() ) return false;
  vector<string> dtl = VFR::SplitString(m_dataType, string(":"));
  vector<string>::iterator it = dtl.begin();
  for ( ; it != dtl.end(); it++ ) {
    if ( *it == dt ) return true;
  } // end of for(it)
  return false;
}

bool vsnMethodTmpl::loadXmlFile(const string& path) {
  static xmlChar* xs;
  string msgHdr = string("MethodTmpl: loadXmlFile: ");

  // clean at first
  if ( m_pXmlNode ) {
    xmlUnlinkNode(m_pXmlNode);
    xmlFreeNode(m_pXmlNode);
    m_pXmlNode = NULL;
  }
  m_name = "";
  m_path = "";
  m_dataType = "";

  if ( path.empty() ) return true;

  // load file and construct XML/DOM tree
  xmlDocPtr doc = xmlParseFile(path.c_str());
  if ( ! doc ) return false;
  xmlNodePtr xnp = xmlDocGetRootElement(doc);

  // search 'vsn_tmpl' node
  while ( xnp ) {
    if ( ! strncmp((const char*)xnp->name, "vsn_tmpl", 8) ) break;
    xnp = xnp->next;
  }
  if ( ! xnp ) {
    ErrMsg(MsgERR, msgHdr + "no 'vsn_tmpl' node exists");
    xmlFreeDoc(doc); return false;
  }

  // get name props
  if ( xs ) xmlFree(xs);
  string xname;
  xs = xmlGetProp(xnp, (const xmlChar*)"name");
  if ( xs && strlen((const char*)xs) > 0 &&
       strcmp(VFR_NONAME, (const char*)xs) )
    xname = (const char*)xs;
  if ( xname.empty() ) {
    char txtbuf[64];
    sprintf(txtbuf, "tmpl_%lu", m_tmplSeq);
    xname = txtbuf;
  }

  // get type prop
  if ( xs ) xmlFree(xs);
  string xtype;
  xs = xmlGetProp(xnp, (const xmlChar*)"type");
  if ( xs && strlen((const char*)xs) > 0 )
    xtype = (const char*)xs;
  size_t xpos;
  if ( (xpos = xtype.find("Sv")) != string::npos )
    xtype.replace(xpos, 2, string("Sph:P3dF:DfiSv:FdvStr:SphPEX"));
  if ( (xpos = xtype.find("Otv")) != string::npos )
    xtype.replace(xpos, 3, string("OctVol"));
  if ( xtype.empty() ) {
    ErrMsg(MsgERR, msgHdr + "no type specified");
    xmlFreeDoc(doc); return false;
  }

  // count method node
  size_t numMtd = 0;
  xmlNodePtr cur = xnp->xmlChildrenNode;
  while ( cur ) {
    if ( cur->type == XML_COMMENT_NODE ) {cur = cur->next; continue;}
    if ( !strcmp((const char*)cur->name, "method") )
      numMtd++;
    cur = cur->next;
  } // end of while(cur)

  // check $n parameter in command node
  cur = xnp->xmlChildrenNode;
  while ( cur ) {
    if ( cur->type == XML_COMMENT_NODE ) {cur = cur->next; continue;}
    if ( !strcmp((const char*)cur->name, "command") ) {
      // check target prop
      if ( xs ) xmlFree(xs);
      xs = xmlGetProp(cur, (const xmlChar*)"target");
      if ( xs && strlen((const char*)xs) > 0 ) {
	string wks((const char*)xs);
	size_t np = 0;
	while ( (np = wks.find('$', np)) != string::npos ) {
	  string nbuf(""); np++;
	  while ( isdigit(wks[np]) ) nbuf.push_back(wks[np++]);
	  if ( nbuf.empty() ) continue;
	  int order = atoi(nbuf.c_str());
	  if ( order < 0 || order > numMtd ) {
	    ErrMsg(MsgERR, msgHdr + "invalid $param specified: " + wks);
	    xmlFreeDoc(doc); return false;
	  }
	} // end of while(np)
      }

      // check value prop
      if ( xs ) xmlFree(xs);
      xs = xmlGetProp(cur, (const xmlChar*)"value");
      if ( xs && strlen((const char*)xs) > 0 ) {
	string wks((const char*)xs);
	size_t np = 0;
	while ( (np = wks.find('$', np)) != string::npos ) {
	  string nbuf(""); np++;
	  while ( isdigit(wks[np]) ) nbuf.push_back(wks[np++]);
	  if ( nbuf.empty() ) continue;
	  int order = atoi(nbuf.c_str());
	  if ( order < 0 || order > numMtd ) {
	    ErrMsg(MsgERR, msgHdr + "invalid $param specified: " + wks);
	    xmlFreeDoc(doc); return false;
	  }
	} // end of while(np)
      }
    } // end of "command"

    cur = cur->next;
  } // end of while(cur)

  // ok
  m_name = xname;
  m_path = path;
  m_dataType = xtype;
  m_pXmlNode = xmlCopyNode(xnp, 1);
  xmlFreeDoc(doc);
  return true;
}

bool vsnMethodTmpl::apply(vsnDataObj* pdata) const {
  static xmlChar* xs;
  static unsigned long sTmplOrder = 0;

  if ( ! pdata ) return false;
  if ( ! m_pXmlNode ) return false;

  string msgHdr = string("MethodTmpl: ") + m_name
    + string(" apply to Data[") + pdata->getName() + string("]: ");
  char txt[128];
  sprintf(txt, "%s_%lu", m_name.c_str(), sTmplOrder);
  string myName = txt;

  // add method(s) at first
  deque<vsnMethodObj*> mtdLst;
  xmlNodePtr cur = m_pXmlNode->xmlChildrenNode;
  while ( cur ) {
    if ( cur->type == XML_COMMENT_NODE ) {cur = cur->next; continue;}
    if ( !strcmp((const char*)cur->name, "method") ) {
      if ( xs ) xmlFree(xs);
      xs = xmlGetProp(cur, (const xmlChar*)"type");
      if ( ! xs || strlen((const char*)xs) < 1 ) {
        ErrMsg(MsgERR, msgHdr + string("method with no type"));
        return false;
      }
      string modType((const char*)xs);
      vsnMethodObj* pMethod = pdata->addNewMethod(string((const char*)xs));
      if ( ! pMethod ) {
        ErrMsg(MsgERR, msgHdr + string("can't create method: ")
	       + string((const char*)xs));
        return false;
      }
      if ( ! pMethod->parseXML(cur) ) {
	ErrMsg(MsgERR, msgHdr + string("method parse failed: ")
	       + string((const char*)xs));
        delete pMethod;
        return false;
      }
      mtdLst.push_back(pMethod);
    }
    cur = cur->next;
  } // end of while(cur)

  // parse commands
  vsnScene* psc = pdata->getAncestorScene();
  string scName; if ( psc ) scName = psc->getName();
  string pdName = pdata->getName();
  cur = m_pXmlNode->xmlChildrenNode;
  while ( cur ) {
    if ( cur->type == XML_COMMENT_NODE ) {cur = cur->next; continue;}
    if ( !strcmp((const char*)cur->name, "command") ) {
      xmlNodePtr commNode = xmlCopyNode(cur, 1);
      if ( ! commNode ) {cur = cur->next; continue;}

      // check target prop
      if ( xs ) xmlFree(xs);
      xs = xmlGetProp(cur, (const xmlChar*)"target");
      if ( xs && strlen((const char*)xs) > 0 ) {
	string wks((const char*)xs);
	size_t np = 0;
	while ( (np = wks.find('$', np)) != string::npos ) {
	  size_t start_p = np++;
	  if ( wks[np] == 'D' ) {
	    wks.replace(start_p, np+1-start_p, pdName);
	  } else if ( wks[np] == 'S' ) {
	    wks.replace(start_p, np+1-start_p, scName);
	  } else {
	    string nbuf("");
	    while ( isdigit(wks[np]) ) nbuf.push_back(wks[np++]);
	    if ( nbuf.empty() ) continue;
	    int order = atoi(nbuf.c_str());
	    if ( order == 0 )
	      wks.replace(start_p, np-start_p, myName);
	    else
	      wks.replace(start_p, np-start_p, mtdLst[order-1]->getName());
	  }
	  np = 0;
	} // end of while(np)
	if ( wks != string((const char*)xs) ) {
	  xmlUnsetProp(commNode, (const xmlChar*)"target");
	  xmlNewProp(commNode,
		     (const xmlChar*)"target", (const xmlChar*)wks.c_str());
	}
      }

      // check value prop
      if ( xs ) xmlFree(xs);
      xs = xmlGetProp(cur, (const xmlChar*)"value");
      if ( xs && strlen((const char*)xs) > 0 ) {
	string wks((const char*)xs);
	size_t np = 0;
	while ( (np = wks.find('$', np)) != string::npos ) {
	  size_t start_p = np++;
	  if ( wks[np] == 'D' ) {
	    wks.replace(start_p, np+1-start_p, pdName);
	  } else if ( wks[np] == 'S' ) {
	    wks.replace(start_p, np+1-start_p, scName);
	  } else {
	    string nbuf("");
	    while ( isdigit(wks[np]) ) nbuf.push_back(wks[np++]);
	    if ( nbuf.empty() ) continue;
	    int order = atoi(nbuf.c_str());
	    if ( order == 0 )
	      wks.replace(start_p, np-start_p, myName);
	    else
	      wks.replace(start_p, np-start_p, mtdLst[order-1]->getName());
	  }
	  np = 0;
	} // end of while(np)
	if ( wks != string((const char*)xs) ) {
	  xmlUnsetProp(commNode, (const xmlChar*)"value");
	  xmlNewProp(commNode,
		     (const xmlChar*)"value", (const xmlChar*)wks.c_str());
	}
      }

      // check scene prop
      if ( xs ) xmlFree(xs);
      xs = xmlGetProp(cur, (const xmlChar*)"scene");
      if ( ! xs || strlen((const char*)xs) < 1 ) {
	if ( ! scName.empty() && scName != VFR_NONAME )
	  xmlNewProp(commNode,
		     (const xmlChar*)"scene", (const xmlChar*)scName.c_str());
      }

      // execute command
      if ( ! vsnApp::GetApp()->parseXMLCommand(commNode) ) {
	ErrMsg(MsgERR, msgHdr + string("command parse failed"));
	//xmlFreeNode(commNode); return false;
      }

      xmlFreeNode(commNode);
    } // end of "command"

    cur = cur->next;
  } // end of while(cur)

  sTmplOrder ++;
  return true;  
}
