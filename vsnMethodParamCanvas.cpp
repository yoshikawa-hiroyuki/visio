//
// vsnMethodParamCanvas
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
#include "wx/textctrl.h"
#include "wx/button.h"
#include "wx/statline.h"
#include "wx/choicdlg.h"
#include "wx/statbmp.h"

#include "vsnMethodParamCanvas.h"
#include "vsnApp.h"
#include "vsnError.h"
#include <map>
#include <sstream>

#include "icon/chk_on.xpm"
#include "icon/chk_off.xpm"

using namespace std;
using namespace VSN;


//----------------------------------------------------------------
// class vsnMethodParamCanvas
//----------------------------------------------------------------

BEGIN_EVENT_TABLE(vsnMethodParamCanvas, wxFrame)
  EVT_TEXT_ENTER(MethodParamCanvas_NameTxt,
		 vsnMethodParamCanvas::OnNameTxt)
  EVT_BUTTON(MethodParamCanvas_AddMethodBtn,
	     vsnMethodParamCanvas::OnAddMethodBtn)
  EVT_BUTTON(MethodParamCanvas_DelMethodBtn,
	     vsnMethodParamCanvas::OnDelMethodBtn)
  EVT_CLOSE(vsnMethodParamCanvas::OnClose)
END_EVENT_TABLE()


/* static members */
size_t vsnMethodParamCanvas::s_mpcSeq = 0;


/* constructors, destructor */

vsnMethodParamCanvas::
vsnMethodParamCanvas(wxWindow* parent, const wxString& title,
		     const wxPoint& pos, const wxSize& size, long style,
		     const std::string& name)
  : wxFrame(parent, -1, title, pos, size, style),
    m_pRootPanel(NULL), m_pNameTxt(NULL)
{
  // seqNo
  m_seq = ++s_mpcSeq;

  // top sizer
  wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL);
  assert(topsizer);

  // head area
  wxBoxSizer* sizerH = new wxBoxSizer(wxHORIZONTAL);
  topsizer->Add(sizerH, 0, wxEXPAND);

  sizerH->Add(new wxStaticText(this, -1, wxT("name")),
	      0, wxALIGN_LEFT|wxALL, 3);
  m_pNameTxt = new wxTextCtrl(this, MethodParamCanvas_NameTxt, wxT(""),
			      wxDefaultPosition, wxSize(100, -1),
			      wxTE_PROCESS_ENTER);
  sizerH->Add(m_pNameTxt, 0, wxALIGN_LEFT|wxALL, 3);
  if ( name == std::string(VFR_NONAME) ) {
    char nbuf[64]; sprintf(nbuf, "PCanvas%zd", m_seq);
    setName(nbuf);
  } else {
    setName(name);
  }

  wxButton* pAddBtn = new wxButton(this, MethodParamCanvas_AddMethodBtn,
				   wxT("add method"));
  sizerH->Add(pAddBtn, 0, wxALIGN_LEFT|wxALL, 3);
  wxButton* pDelBtn = new wxButton(this, MethodParamCanvas_DelMethodBtn,
				   wxT("del method"));
  sizerH->Add(pDelBtn, 0, wxALIGN_LEFT|wxALL, 3);

  // body area
  topsizer->Add(new wxStaticLine(this, -1, wxDefaultPosition, wxSize(3,3),
				 wxHORIZONTAL), 0, wxEXPAND|wxALL, 0);

#ifndef LINUX
  m_pRootPanel = new wxPanel(this, -1, wxDefaultPosition, wxDefaultSize);
#else
  m_pRootPanel = new wxScrolledWindow(this, -1, wxDefaultPosition,
				      wxDefaultSize, wxScrolledWindowStyle);
  assert(m_pRootPanel);
  m_pRootPanel->SetScrollRate(5, 5);
#endif
  topsizer->Add(m_pRootPanel, 1, wxEXPAND|wxALL, 3);
  wxBoxSizer* prpSizer = new wxBoxSizer(wxHORIZONTAL);
  m_pRootPanel->SetSizer(prpSizer);

  SetSizer(topsizer);
  topsizer->Layout();
}

vsnMethodParamCanvas::~vsnMethodParamCanvas() {
}


/* methods */

void vsnMethodParamCanvas::setName(const std::string& name) {
  if ( name == m_name ) return;
  if ( name.empty() )
    m_name = VFR_NONAME;
  else
    m_name = name;

  if ( m_pNameTxt ) {
    m_pNameTxt->SetValue(vsnApp::ConvSysToWx(m_name));
  }
}

void vsnMethodParamCanvas::updateLayout() {
  if ( ! m_pRootPanel ) return;
  m_pRootPanel->DestroyChildren();
  if ( m_mtdLst.size() < 1 ) return;

  wxSizer* rootSizer = m_pRootPanel->GetSizer();
  if ( ! rootSizer ) return;

  deque<vsnMethodObj*>::iterator it;
  for ( it = m_mtdLst.begin(); it != m_mtdLst.end(); it++ ) {
    wxPanel* pPnl = new wxPanel(m_pRootPanel);
    wxBoxSizer* ppaSizer = new wxBoxSizer(wxVERTICAL);
    pPnl->SetSizer(ppaSizer);
    rootSizer->Add(pPnl, 0, wxEXPAND|wxALL, 3);

    wxString mtdName = wxT("");
    vsnDataObj* pdt = (*it)->getRefData();
    if ( pdt ) {
      mtdName = vsnApp::ConvSysToWx(pdt->getAncestorScene()->getName());
      mtdName += wxT("/");
      mtdName += vsnApp::ConvSysToWx(pdt->getName());
      mtdName += wxT("/");
    }
    mtdName += vsnApp::ConvSysToWx((*it)->getName());
    mtdName += wxT("[");
    mtdName += vsnApp::ConvSysToWx((*it)->getMethodType());
    mtdName += wxT("]");
    ppaSizer->Add(new wxStaticText(pPnl, -1, mtdName),
		  0, wxALIGN_LEFT|wxALL, 3);
    ppaSizer->Add(new wxStaticLine(pPnl, -1, wxDefaultPosition, wxSize(280,3),
				   wxHORIZONTAL), 0, wxEXPAND|wxALL, 0);

    vsnMethodPP* pmpp = (*it)->getParamPanel(pPnl);

    rootSizer->Add(new wxStaticLine(m_pRootPanel, -1, wxDefaultPosition,
				    wxSize(3,600), wxVERTICAL),
		   0, wxEXPAND|wxALL, 0);
    ppaSizer->Layout();
    pPnl->FitInside();
  } // end of for(it)

  rootSizer->Layout();
  GetSizer()->Layout();
}

bool vsnMethodParamCanvas::addMethod(vsnMethodObj* pMtd) {
  if ( ! m_pRootPanel ) return false;
  if ( ! pMtd ) return false;

  deque<vsnMethodObj*>::iterator it;
  for ( it = m_mtdLst.begin(); it != m_mtdLst.end(); it++ )
    if ( *it == pMtd ) return false;

  m_mtdLst.push_back(pMtd);
  updateLayout();
  return true;
}

bool vsnMethodParamCanvas::delMethod(vsnMethodObj* pMtd) {
  if ( ! m_pRootPanel ) return false;
  if ( ! pMtd ) return false;

  deque<vsnMethodObj*>::iterator it;
  for ( it = m_mtdLst.begin(); it != m_mtdLst.end(); it++ )
    if ( *it == pMtd ) break;
  if ( it == m_mtdLst.end() ) return false;

  m_mtdLst.erase(it);
  updateLayout();
  return true;
}

bool vsnMethodParamCanvas::setGeometry(const int x, const int y,
				       const int w, const int h) {
  if ( x == -1 && y == -1 && w == -1 && h == -1 ) return false;
  SetSize(x, y, w, h);
  return true;
}

bool vsnMethodParamCanvas::setShowMode(const bool sm) {
  Show(sm);
  return true;
}


/* event handlers */

void vsnMethodParamCanvas::OnNameTxt(wxCommandEvent& event) {
  if ( ! m_pNameTxt ) return;
  wxString valStr = m_pNameTxt->GetValue();
  if ( valStr.IsEmpty() ) {
    m_pNameTxt->SetValue(vsnApp::ConvSysToWx(m_name));
    return;
  }
  string nname = vsnApp::ConvWxToSys(valStr);
  if ( nname == m_name ) return;
  setName(nname);
}

void vsnMethodParamCanvas::OnAddMethodBtn(wxCommandEvent& event) {
  vsnApp* pApp = vsnApp::GetApp();
  if ( ! pApp ) return;

  register size_t i, j, k, l, nmtd;
  map<string, vsnMethodObj*> wholeMtdLst;
  map<string, vsnMethodObj*>::iterator mit;

  size_t nScene = pApp->getNumScene();
  for ( i = 0; i < nScene; i++ ) {
    vsnScene* pScene = pApp->getScene(i);
    if ( ! pScene ) continue;
    size_t nObjGrp = pScene->getNumObjGroup();
    for ( j = 0; j < nObjGrp; j++ ) {
      vsnObjGroup* pObjGrp = pScene->getObjGroup(j);
      if ( ! pObjGrp ) continue;
      size_t nData = pObjGrp->getNumData();
      for ( k = 0; k < nData; k++ ) {
	vsnDataObj* pData = pObjGrp->getData(k);
	if ( ! pData ) continue;
	size_t nMethod = pData->getNumMethod();
	for ( l = 0; l < nMethod; l++ ) {
	  vsnMethodObj* pMethod = pData->getMethod(l);
	  if ( ! pMethod ) continue;
	  deque<vsnMethodObj*>::iterator it;
	  for ( it = m_mtdLst.begin(); it != m_mtdLst.end(); it++ )
	    if ( *it == pMethod ) break;
	  if ( it != m_mtdLst.end() ) continue; // already registered

	  string entryStr = pScene->getName() + string("/");
	  entryStr += pData->getName() + string("/");
	  entryStr += pMethod->getName() +"[" + pMethod->getMethodType() +"]";
	  wholeMtdLst.insert(make_pair(entryStr, pMethod));
	} // end of for(l)
      } // end of for(k)
    } // end of for(j)
  } // end of for(i)

  nmtd = wholeMtdLst.size();
  if ( nmtd < 1 ) {
    ErrMsg(MsgWARN, string("Param canvas: Add: no method to add."));
    return;
  }
  wxString* choices = new wxString[nmtd];
  for ( mit = wholeMtdLst.begin(), i = 0;
	mit != wholeMtdLst.end(); mit++, i++ )
    choices[i] = vsnApp::ConvSysToWx(mit->first);

  wxSingleChoiceDialog dlg(this, wxT("select a method to add"),
			   wxT("add method"), nmtd, choices);
  delete [] choices;
  if ( dlg.ShowModal() != wxID_OK ) return;

  int val = dlg.GetSelection();
  if ( val < 0 || val >= nmtd ) {
    ErrMsg(MsgERR, string("Param canvas: Add: invalid selection"));
    return;
  }
  wxString valStr = dlg.GetStringSelection();
  string keyStr = vsnApp::ConvWxToSys(valStr);
  mit = wholeMtdLst.find(keyStr);
  if ( mit == wholeMtdLst.end() ) {
    ErrMsg(MsgERR,
	   string("Param canvas: Add: can't find selected method"));
    return;
  }

  if ( ! addMethod(mit->second) ) {
    ErrMsg(MsgERR,
	   string("Param canvas: Add: can't add selected method"));
    return;
  }
}

void vsnMethodParamCanvas::OnDelMethodBtn(wxCommandEvent& event) {
  size_t nmtd = m_mtdLst.size();
  if ( nmtd < 1 ) {
    ErrMsg(MsgWARN, string("Param canvas: Del: no method to delete."));
    return;
  }

  wxString* choices = new wxString[nmtd];
  size_t i;
  string entryStr;
  deque<vsnMethodObj*>::iterator it;
  for ( it = m_mtdLst.begin(), i = 0; it != m_mtdLst.end(); it++, i++ ) {
    entryStr = (*it)->getName() +"[" + (*it)->getMethodType() +"]";
    choices[i] = vsnApp::ConvSysToWx(entryStr); // fail safe
    vsnDataObj* pData = (*it)->getRefData();
    if ( ! pData ) continue;
    vsnScene* pScene = pData->getAncestorScene();
    if ( ! pScene ) continue;
    entryStr = pScene->getName() + string("/");
    entryStr += pData->getName() + string("/");
    entryStr += (*it)->getName() +"[" + (*it)->getMethodType() +"]";
    choices[i] = vsnApp::ConvSysToWx(entryStr);
  } // end of for(it)

  wxSingleChoiceDialog dlg(this, wxT("select a method to delete"),
			   wxT("del method"), nmtd, choices);
  delete [] choices;
  if ( dlg.ShowModal() != wxID_OK ) return;

  int val = dlg.GetSelection();
  if ( val < 0 || val >= nmtd ) {
    ErrMsg(MsgERR, string("Param canvas: Del: invalid selection"));
    return;
  }
  
  if ( ! delMethod(m_mtdLst[val]) ) {
    ErrMsg(MsgERR,
	   string("Param canvas: Del: can't delete selected method"));
    return;
  }
}

void vsnMethodParamCanvas::OnClose(wxCloseEvent& event) {
  setShowMode(false);
}


/* from vsnIoObject */

bool vsnMethodParamCanvas::parseXML(xmlNodePtr xnp) {
  static xmlChar* xs;
  if ( ! xnp ) return false;
  vsnApp* pApp = vsnApp::GetApp();

  // is 'param_canvas' node?
  if ( strcmp((const char*)xnp->name, "param_canvas") ) return false;

  // get name
  if ( xs ) xmlFree(xs);
  xs = xmlGetProp(xnp, (const xmlChar*)"name");
  if (  xs && strlen((const char*)xs) > 0 &&
        strcmp(VFR_NONAME, (const char*)xs) ) {
    vsnMethodParamCanvas* pmpc = NULL;
    if ( pApp ) pmpc = pApp->getMethodParamCanvas((const char*)xs);
    if ( pmpc && pmpc != this ) {
      ErrMsg(MsgWARN, string("Param canvas: parseXML: the param_canvas named ")
	     + (const char*)xs + " has already exists, don't set name");
    } else
      setName((const char*)xs);
  }
  string msgHdr
    = string("Param canvas[") + getName() + string("]: parseXML: ");

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
      if ( ! xs || strlen((const char*)xs) < 1 ) {
        ErrMsg(MsgERR, msgHdr + string("param node without name, ignore"));
        goto _NEXT_XML_NODE;
      }
      xsN = (const char*)xs;
      if ( xs ) xmlFree(xs);
      xs = xmlGetProp(cur, (const xmlChar*)"value");
      if ( xs && strlen((const char*)xs) > 0 ) xsV = (const char*)xs;
      else xsV = "";

      if ( xsN == string("geometry") ) {
        if ( xsV.empty() ) {
          ErrMsg(MsgERR, msgHdr +
                 string("invalid value in param geometry: no value"));
          goto _NEXT_XML_NODE;
        }
        char xc; int c, x, y, w, h; x = y = w = h = -1;
        istringstream iss(xsV); string delim(" ,:/");
        iss >> x;
        while ( (c=iss.peek())!=EOF && delim.find((char)c)!=string::npos )
          iss.get(xc);
        if ( iss.good() ) {
          iss >> y;
          while ( (c=iss.peek())!=EOF && delim.find((char)c)!=string::npos )
            iss.get(xc);
          if ( iss.good() ) {
            iss >> w;
            while ( (c=iss.peek())!=EOF && delim.find((char)c)!=string::npos )
              iss.get(xc);
            if ( iss.good() ) iss >> h;
          }
        }
        if ( x == -1 && y == -1 && w == -1 && h == -1 ) {
          ErrMsg(MsgERR, msgHdr +
                 string("invalid value in param geometry: ") + xsV);
          goto _NEXT_XML_NODE;
        }
        if ( ! setGeometry(x, y, w, h) ) {
          ErrMsg(MsgERR, msgHdr + string("set geometry failed: ") + xsV);
          goto _NEXT_XML_NODE;
        }
      } // end of "geometry"
    } // end of "param"

  _NEXT_XML_NODE:
    cur = cur->next;
  } // end of while(cur)

  return true;
}

bool vsnMethodParamCanvas::outputXML(std::ostream& os, const size_t ts) {
  std::string idts;
  for ( int i = 0; i < ts; i++ ) idts.push_back(' ');
  string idts_2 = idts; idts_2.push_back(' '); idts_2.push_back(' ');

  bool ret = true;
  string nameStr = getName();

  // output
  os << idts << "<param_canvas";
  if ( ! nameStr.empty() && nameStr != string(VFR_NONAME) )
    os << " name=\"" << nameStr << "\"";
  os << ">" << endl;

  // output params
  // geometry
  int winX, winY, winW, winH;
  GetPosition(&winX, &winY); GetSize(&winW, &winH);
  os << idts_2 << "<param name=\"geometry\" value=\""
     << winX << " " << winY << " " << winW << " " << winH << "\" />" << endl;

  os << idts << "</param_canvas>" << endl;

  // output commands
  if ( ! nameStr.empty() && nameStr != string(VFR_NONAME) ) {
    if ( ! IsShown() ) {
      os << idts << "<command target=\"param_canvas;" << nameStr
	 << "\" name=\"set_show\" value=\"no\" />" << endl;
    }

    deque<vsnMethodObj*>::iterator it;
    for ( it = m_mtdLst.begin(); it != m_mtdLst.end(); it++ ) {
      vsnDataObj* pData = (*it)->getRefData();
      if ( ! pData ) continue;
      vsnScene* pScene = pData->getAncestorScene();
      if ( ! pScene ) continue;

      os << idts << "<command target=\"param_canvas;" << nameStr
	 << "\" name=\"add_method\" value=\"";
      os << pScene->getName() << "/" << pData->getName() << "/"
	 << (*it)->getName() << "\" />" << endl;
    } // end of for(it)
  }

  return true;
}

bool vsnMethodParamCanvas::commandXML(xmlNodePtr xnp) {
  static xmlChar* xs;
  string msgHdr
    = string("Param canvas[") + getName() + string("]: commandXML: ");
  vsnApp* pApp = vsnApp::GetApp();

  // is 'command' node?
  if ( ! xnp ) return false;
  if ( strcmp((const char*)xnp->name, "command") ) return false;

  // is my command?
  if ( xs ) xmlFree(xs);
  xs = xmlGetProp(xnp, (const xmlChar*)"target");
  if ( ! xs || strlen((const char*)xs) < 1 ) return false;
  string targStr = (const char*)xs;
  string targSub = targStr.substr(0, 13);
  if ( targSub != "param_canvas;" &&
       targSub != "Param_Canvas;" && targSub != "PARAM_CANVAS;" )
    return false;
  targSub = targStr.substr(13);
  if ( targSub.empty() || targSub == VFR_NONAME ) return false;
  if ( targSub != getName() ) return false;

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
    if ( valueStr == m_name ) return true;
    if ( valueStr.empty() || valueStr == VFR_NONAME ) {
      ErrMsg(MsgERR, msgHdr + string("command set_name: no value"));
      return false;
    }
    vsnMethodParamCanvas* pmpc = NULL;
    if ( pApp ) pmpc = pApp->getMethodParamCanvas(valueStr);
    if ( pmpc && pmpc != this ) {
      ErrMsg(MsgERR, msgHdr + "command set_name: the param_canvas named " +
             valueStr + " has already exists");
      return false;
    }
    setName(valueStr);
  } // end of "set_name"
  else if ( nameStr == "set_show" ) {
    bool showMode;
    if ( valueStr == string("yes") ) showMode = true;
    else if ( valueStr == string("no") ) showMode = false;
    else {
      ErrMsg(MsgERR, msgHdr +
             string("command set_show: invalid value: ") + valueStr);
      return false;
    }
    setShowMode(showMode);
  } // end of "set_show"
  else if ( nameStr == "set_geometry" ) {
    if ( valueStr.empty() ) {
      ErrMsg(MsgERR, msgHdr + string("command set_geometry: no value"));
      return false;
    }
    char xc; int c, x, y, w, h; x = y = w = h = -1;
    istringstream iss(valueStr); string delim(" ,:/");
    iss >> x;
    while ( (c=iss.peek()) != EOF && delim.find((char)c) != string::npos )
      iss.get(xc);
    if ( iss.good() ) {
      iss >> y;
      while ( (c=iss.peek()) != EOF && delim.find((char)c) != string::npos )
        iss.get(xc);
      if ( iss.good() ) {
        iss >> w;
        while ( (c=iss.peek()) != EOF && delim.find((char)c) != string::npos )
          iss.get(xc);
        if ( iss.good() ) iss >> h;
      }
    }
    if ( x == -1 && y == -1 && w == -1 && h == -1 ) {
      ErrMsg(MsgERR, msgHdr + string("command set_geometry: invalid value: ")
             + valueStr);
      return false;
    }
    if ( ! setGeometry(x, y, w, h) ) {
      ErrMsg(MsgERR, msgHdr + string("command set_geometry: set failed: ")
             + valueStr);
      return false;
    }
  } // end of "set_geometry"
  else if ( nameStr == "add_method" ) {
    string entry[3];
    istringstream strBuff(valueStr); char c;
    while ( strBuff.get(c) && c != '/' ) entry[0].push_back(c);
    if ( strBuff.good() ) {
      while ( strBuff.get(c) && c != '/' ) entry[1].push_back(c);
      if ( strBuff.good() ) {
	while ( strBuff.get(c) && c != '/' ) entry[2].push_back(c);
      }
    }
    string mtdName, dtName, scName;
    if ( ! entry[2].empty() ) {
      mtdName = entry[2]; dtName = entry[1]; scName = entry[0];
    } else if ( ! entry[1].empty() ) {
      mtdName = entry[1]; dtName = entry[0];
    } else if ( ! entry[0].empty() ) {
      mtdName = entry[0];
    }
    if ( mtdName.empty() ) {
      ErrMsg(MsgERR, msgHdr + string("command add_method: no value"));
      return false;
    }
    vsnScene* psc = NULL; vsnDataObj* pdt = NULL;
    vsnMethodObj* pmtd = NULL;
    if ( ! scName.empty() )
      psc = vsnApp::GetApp()->getScene(scName);
    else
      psc = vsnApp::GetApp()->getScene(0);
    if ( ! psc ) {
      ErrMsg(MsgERR, msgHdr
	     + string("command add_method: can't find the scene"));
      return false;
    }
    if ( ! dtName.empty() ) {
      pdt = dynamic_cast<vsnDataObj*>(psc->getNode(dtName));
      if ( ! pdt ) {
	ErrMsg(MsgERR, msgHdr
	       + string("command add_method: can't find the data: ") + dtName);
	return false;
      }
      pmtd = dynamic_cast<vsnMethodObj*>(pdt->getNode(mtdName));
    } else
      pmtd = dynamic_cast<vsnMethodObj*>(psc->getNode(mtdName));
    if ( ! pmtd ) {
      ErrMsg(MsgERR, msgHdr + string("command add_method: ")
	     + string("can't find the method: ") + mtdName);
      return false;
    }
    if ( ! addMethod(pmtd) ) {
      ErrMsg(MsgERR, msgHdr + string("command add_method: ")
	     + string("can't add the method: ") + mtdName);
      return false;
    }
  } // end of "add_method"
  else if ( nameStr == "del_method" ) {
    string entry[3];
    istringstream strBuff(valueStr); char c;
    while ( strBuff.get(c) && c != '/' ) entry[0].push_back(c);
    if ( strBuff.good() ) {
      while ( strBuff.get(c) && c != '/' ) entry[1].push_back(c);
      if ( strBuff.good() ) {
	while ( strBuff.get(c) && c != '/' ) entry[2].push_back(c);
      }
    }
    string mtdName, dtName, scName;
    if ( ! entry[2].empty() ) {
      mtdName = entry[2]; dtName = entry[1]; scName = entry[0];
    } else if ( ! entry[1].empty() ) {
      mtdName = entry[1]; dtName = entry[0];
    } else if ( ! entry[0].empty() ) {
      mtdName = entry[0];
    }
    if ( mtdName.empty() ) {
      ErrMsg(MsgERR, msgHdr + string("command del_method: no value"));
      return false;
    }
    vsnScene* psc = NULL; vsnDataObj* pdt = NULL;
    vsnMethodObj* pmtd = NULL;
    if ( ! scName.empty() )
      psc = vsnApp::GetApp()->getScene(scName);
    else
      psc = vsnApp::GetApp()->getScene(0);
    if ( ! psc ) {
      ErrMsg(MsgERR, msgHdr
	     + string("command del_method: can't find the scene"));
      return false;
    }
    if ( ! dtName.empty() ) {
      pdt = dynamic_cast<vsnDataObj*>(psc->getNode(dtName));
      if ( ! pdt ) {
	ErrMsg(MsgERR, msgHdr
	       + string("command del_method: can't find the data: ") + dtName);
	return false;
      }
      pmtd = dynamic_cast<vsnMethodObj*>(pdt->getNode(mtdName));
    } else
      pmtd = dynamic_cast<vsnMethodObj*>(psc->getNode(mtdName));
    if ( ! pmtd ) {
      ErrMsg(MsgERR, msgHdr + string("command del_method: ")
	     + string("can't find the method: ") + mtdName);
      return false;
    }
    if ( ! delMethod(pmtd) ) {
      ErrMsg(MsgERR, msgHdr + string("command del_method: ")
	     + string("can't delete the method: ") + mtdName);
      return false;
    }
  } // end of "del_method"
  else {
    // not 'param_canvas' command
    ErrMsg(MsgERR, msgHdr + string("unknown command: ") + nameStr);
    return false;
  }

  return true;
}


//----------------------------------------------------------------
// class vsnMPCListDlg
//----------------------------------------------------------------

BEGIN_EVENT_TABLE(vsnMPCListDlg, wxDialog)
  EVT_LIST_ITEM_ACTIVATED(MPCListDlg_MPCListCtl, vsnMPCListDlg::OnItemSelected)
  EVT_BUTTON(MPCListDlg_NewBtn, vsnMPCListDlg::OnNewBtn)
  EVT_BUTTON(MPCListDlg_DeleteBtn, vsnMPCListDlg::OnDeleteBtn)
  EVT_BUTTON(MPCListDlg_CloseBtn, vsnMPCListDlg::OnCloseBtn)
END_EVENT_TABLE()


/* constructors / destructor */

vsnMPCListDlg::vsnMPCListDlg(wxWindow* parent,
			     std::deque<vsnMethodParamCanvas*>* plst)
  : wxDialog(parent, -1, wxString(wxT("Param canvases"))),
    p_refLst(plst), m_pImgLst(NULL),
    m_pMPCListCtl(NULL), m_pNewBtn(NULL), m_pDeleteBtn(NULL), m_pCloseBtn(NULL)
{
  assert(parent);

  // create image list
  m_pImgLst = new wxImageList(16, 16, TRUE);
  m_pImgLst->Add(wxIcon(chk_off));
  m_pImgLst->Add(wxIcon(chk_on));

  // create widgets
  wxBoxSizer* sizerTop = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer* sizerH;

  m_pMPCListCtl = new vsnMPCListCtl(this, MPCListDlg_MPCListCtl,
				    wxDefaultPosition, wxSize(350, 300),
				    wxLC_REPORT | wxSUNKEN_BORDER);
  m_pMPCListCtl->SetImageList(m_pImgLst, wxIMAGE_LIST_SMALL);

  wxListItem itemCol;
  itemCol.m_image = -1;
  itemCol.m_mask = wxLIST_MASK_TEXT;
  itemCol.m_text = wxT("Show");
  m_pMPCListCtl->InsertColumn(0, itemCol);
  itemCol.m_text = wxT("Param canvas");
  m_pMPCListCtl->InsertColumn(1, itemCol);
  m_pMPCListCtl->SetColumnWidth(0, 50);
  m_pMPCListCtl->SetColumnWidth(1, 300);

  sizerTop->Add(m_pMPCListCtl, 0, wxEXPAND|wxALL, 3);

  // buttons
  sizerH = new wxBoxSizer(wxHORIZONTAL);
  sizerTop->Add(sizerH, 0, wxALIGN_RIGHT|wxALL, 5);
  m_pNewBtn = new wxButton(this, MPCListDlg_NewBtn, wxT("new"));
  sizerH->Add(m_pNewBtn);
  sizerH->Add(5, 0);
  m_pDeleteBtn = new wxButton(this, MPCListDlg_DeleteBtn, wxT("delete"));
  sizerH->Add(m_pDeleteBtn);
  sizerH->Add(10, 0);
  m_pCloseBtn = new wxButton(this, MPCListDlg_CloseBtn, wxT("close"));
  sizerH->Add(m_pCloseBtn);

  // update the list
  (void)updateList();

  // post process
  SetAutoLayout(TRUE);
  SetSizer(sizerTop);
  sizerTop->SetSizeHints(this);
  sizerTop->Fit(this);
}


/* methods */

bool vsnMPCListDlg::updateList() {
  if ( ! p_refLst ) return false;
  if ( ! m_pMPCListCtl || ! m_pImgLst ) return false;

  // at first, remove all items
  m_pMPCListCtl->DeleteAllItems();

  // create list items
  deque<vsnMethodParamCanvas*>::iterator it;
  int i = 0;
  for ( it = p_refLst->begin(); it != p_refLst->end(); it++ ) {
    if ( ! *it ) continue;
    m_pMPCListCtl->InsertItem(i, (*it)->IsShown() ? 1 : 0);

    char msgBuff[512];
    sprintf(msgBuff, "%s", (*it)->getName().c_str());
    m_pMPCListCtl->SetItem(i, 1, vsnApp::ConvSysToWx(msgBuff));

    i++;
  } // end of for(it)

  return true;
}


/* event handler */

void vsnMPCListDlg::OnItemSelected(wxListEvent& event) {
  if ( ! p_refLst ) return;
  if ( ! m_pMPCListCtl || ! m_pImgLst ) return;

  // selected list item
  wxListItem info;
  info.m_itemId = event.m_itemIndex;
  info.m_col = 1;
  info.m_mask = wxLIST_MASK_TEXT;
  if ( ! m_pMPCListCtl->GetItem(info) ) return;

  // find item from ref list
  deque<vsnMethodParamCanvas*>::iterator it;
  for ( it = p_refLst->begin(); it != p_refLst->end(); it++ ) {
    if ( ! *it ) continue;
    wxString bpLoc = wxString::Format(wxT("%s"), (*it)->getName().c_str());
    if ( bpLoc == info.m_text ) break;
  } // end of for(it)
  if ( it == p_refLst->end() ) return;

  // flip Show flag
  vsnMethodParamCanvas* pmpc = *it;
  pmpc->Show(! pmpc->IsShown());

  updateList();
}

void vsnMPCListDlg::OnNewBtn(wxCommandEvent& event) {
  if ( ! p_refLst ) return;
  vsnApp* pApp = vsnApp::GetApp();
  if ( ! pApp ) return;

  vsnMethodParamCanvas* pmpc =  pApp->addMethodParamCanvas();
  if ( ! pmpc ) return;
  pmpc->Show();
  updateList();
}

void vsnMPCListDlg::OnDeleteBtn(wxCommandEvent& event) {
  if ( ! p_refLst ) return;
  if ( ! m_pMPCListCtl ) return;
  if ( m_pMPCListCtl->GetSelectedItemCount() < 1 ) {
    ErrMsg(MsgINFO, string("Delete Param canvas: no param_canvas selected."));
    return;
  }

  long item = -1;
  while ( 1 ) {
    item = m_pMPCListCtl->GetNextItem(item,
				      wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if ( item == -1 ) break;

    // selected list item
    wxListItem info;
    info.m_itemId = item;
    info.m_col = 1;
    info.m_mask = wxLIST_MASK_TEXT;
    if ( ! m_pMPCListCtl->GetItem(info) ) continue;

    // find item from ref list
    deque<vsnMethodParamCanvas*>::iterator it;
    for ( it = p_refLst->begin(); it != p_refLst->end(); it++ ) {
      if ( ! *it ) continue;
      wxString bpLoc = wxString::Format(wxT("%s"), (*it)->getName().c_str());
      if ( bpLoc == info.m_text ) break;
    } // end of for(it)
    if ( it == p_refLst->end() ) continue;

    wxString msg = wxT("Are you sure to delete Param canvas: ") + info.m_text;
    wxMessageDialog dlg(this, msg, wxT("delete param canvas"),
                        vsn_wxOK_CANCEL|wxICON_QUESTION);
    if ( dlg.ShowModal() != vsn_wxIDOK ) continue;

    vsnApp::GetApp()->delMethodParamCanvas(*it);
  } // end of while(1)

  updateList();
}

void vsnMPCListDlg::OnCloseBtn(wxCommandEvent& event) {
  if ( IsModal() )
    EndModal(vsn_wxIDOK);
  else
    this->Hide();
}
