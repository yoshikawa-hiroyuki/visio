//
// vsnApp
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
#include "wx/image.h"
#include "wx/dir.h"
#include "wx/config.h"

#include <sstream>
#include <fstream>
#include <map>
#include <assert.h>
#include "vsnApp.h"
#include "vsnFrontImage.h"
#include "vsnError.h"
#include "utilPath.h"

using namespace std;
using namespace CES;
using namespace VSN;


/* static variables in namespace VSN */

std::ostream* VSN::g_pOutStream(&std::cerr);
#if defined(VSN_ERR_WXDLG)
bool VSN::g_useOutStream(false);
#else
bool VSN::g_useOutStream(true);
#endif


//----------------------------------------------------------------
// class vsnApp
//----------------------------------------------------------------
IMPLEMENT_APP(vsnApp)

BEGIN_EVENT_TABLE(vsnApp, wxApp)
  EVT_SOCKET(APP_SOCK_SERVER, vsnApp::OnServerEvent)
  EVT_SOCKET(APP_SOCK_SOCKET, vsnApp::OnSocketEvent)
END_EVENT_TABLE()


/* static members */
vsnApp* vsnApp::s_pApp = NULL;


/* constructors, destructor */

vsnApp::vsnApp()
  : m_locale(wxLANGUAGE_DEFAULT), m_needPathEncode(false) {}

vsnApp::~vsnApp() {
  // clean scene_list
  deque<vsnScene*>::iterator s_it;
  for ( s_it = m_sceneList.begin(); s_it != m_sceneList.end(); ) {
    if ( *s_it ) delete (*s_it);
    s_it = m_sceneList.erase(s_it);
  } // end of for(s_it)

  // clean view_frame_list
  deque<vsnViewFrame*>::iterator v_it;
  for ( v_it = m_viewList.begin(); v_it != m_viewList.end(); ) {
    if ( *v_it ) (*v_it)->Close();
    v_it = m_viewList.erase(v_it);
  } // end of for(v_it)

  // clean method param canvas list
  deque<vsnMethodParamCanvas*>::iterator mpc_it;
  for ( mpc_it = m_mpcList.begin(); mpc_it != m_mpcList.end(); ) {
    if ( *mpc_it ) (*mpc_it)->Destroy();
    mpc_it = m_mpcList.erase(mpc_it);
  } // end of for(mpc_it)

  // clean tmpl list
  deque<vsnMethodTmpl*>::iterator t_it;
  for ( t_it = m_tmplList.begin(); t_it != m_tmplList.end(); ) {
    if ( *t_it ) delete (*t_it);
    t_it = m_tmplList.erase(t_it);
  } // end of for(t_it)

  // m_pSockServ is wx object, don't delete.
}


/* OnInit: main() equivalent */
bool vsnApp::OnInit(void) {
  s_pApp = this;
  m_pSockServ = NULL;

  bool servMode = false;
  unsigned short servPort = APP_SOCK_PORT;
  int argIdx;

  // parse args
  for ( argIdx = 1; argIdx < argc; argIdx++ ) {
    if ( ! wxStrcmp(argv[argIdx], wxT("-h")) ) {
      SetUseOutStream(true);
      ErrMsg(MsgINFO, string("usage: ") + vsn_progname +
	     " [-h|-v][-s [-p port]][infile.xml [infile.xml ...]]");
      return false;
    }
    else if ( ! wxStrcmp(argv[argIdx], wxT("-v")) ) {
      SetUseOutStream(true);
      ErrMsg(MsgINFO, string(vsn_app_name) + " version " + vsn_version);
      return false;
    }
    else if ( ! wxStrcmp(argv[argIdx], wxT("-s")) ) {
      servMode = true;
      continue;
    }
    else if ( ! wxStrcmp(argv[argIdx], wxT("-p")) && argIdx+1 < argc ) {
      servPort
	= (unsigned short)atoi((const char*)wxString(argv[++argIdx]).mb_str());
      continue;
    }
    else
      break;
  } // end of for(argIdx)

  // initialize
  m_cwd = wxGetCwd().mb_str();
  m_cwd = vsnPath_normalize(m_cwd);
  m_openNew = false;
  m_chkProgress = false;

  // locale / encoding setup
  int lang = wxLocale::GetSystemLanguage();
  if ( lang == wxLANGUAGE_JAPANESE ) {
    m_needPathEncode = true;
    wxString encName = wxLocale::GetSystemEncodingName();
    if ( ! encName.CmpNoCase(wxT("UTF-8")) ) // Linux/ja
      m_xmlEncode = "UTF-8";
    else if ( ! encName.CmpNoCase(wxT("EUC-JP")) )
      m_xmlEncode = "EUC-JP";
    else if ( ! encName.CmpNoCase(wxT("WINDOWS-932")) ) // Windows/ja
      m_xmlEncode = "CP932";
    else
      m_xmlEncode = "UTF-8"; // MacOSX/ja: encName will be empty..
  }

  // install image handler
  wxImage::AddHandler(new wxPNGHandler);
  wxImage::AddHandler(new wxJPEGHandler);
  wxImage::AddHandler(new wxTIFFHandler);
  wxImage::AddHandler(new wxGIFHandler);
  wxImage::AddHandler(new wxPNMHandler);
  wxImage::AddHandler(new wxXPMHandler);

  // init tmpls
  (void)initTmpls();

  // create default ViewFrame
  vsnViewFrame* pvf = addViewFrame();
  if ( ! pvf ) return false;

  // create default Scene
  vsnScene* psc = addScene();
  if ( ! psc ) return false;
  pvf->setScene(psc);
  pvf->Show();

  // config
  SetVendorName(wxT("RIKEN"));
  SetAppName(wxT("Vtools"));
  wxConfigBase* pConfig = wxConfigBase::Get();
  pConfig->SetRecordDefaults();
  m_oprOrient.readConfig(); setOprOrientation(m_oprOrient);
  vsnScene::ReadConfig();
  vsnGfxView::ReadConfig();

  // parse args for infile[s]
  if ( argIdx < argc ) {
    // load the first infile
    bool firstLoad = parseXMLFile(ConvWxToSys(argv[argIdx]));

    // load the follow infile[s]
    for ( argIdx++; argIdx < argc; argIdx++ ) {
      if ( firstLoad )
	parseXMLFile(ConvWxToSys(argv[argIdx]), true); // merge
      else
	firstLoad = parseXMLFile(ConvWxToSys(argv[argIdx]));
    } // end of for(argIdx)
  } // end of if(argIdx<argc)

  // sock-server
  if ( servMode ) {
    wxIPV4address addr;
    addr.Service(servPort);
    m_pSockServ = new wxSocketServer(addr);
    if ( ! m_pSockServ ) {
      char txt[64]; sprintf(txt, "%d", servPort);
      ErrMsg(MsgWARN, string(vsn_app_name) + " can't setup socket server"
	     " at port " + txt);
    } else {
      m_pSockServ->SetEventHandler(*this, APP_SOCK_SERVER);
      m_pSockServ->SetNotify(wxSOCKET_CONNECTION_FLAG);
      m_pSockServ->Notify(TRUE);
    }
  } // end of if(servMode)

  // ok, let's roll
  m_chkProgress = true;
  return true;
}


// quit app
void vsnApp::quit(const bool confirm) {
  if ( confirm ) {
    string msg = string("Are you sure to quit ") +vsn_app_name +string(" ?");
    string ttl = string("Quit ") + vsn_app_name;
    wxMessageDialog dlg(NULL, ConvSysToWx(msg), ConvSysToWx(ttl),
			vsn_wxOK_CANCEL|wxICON_QUESTION);
    if ( dlg.ShowModal() != vsn_wxIDOK ) return;
  }

  reset(false);
  ExitMainLoop();

  // config
  m_oprOrient.writeConfig();
  vsnScene::WriteConfig();
  vsnGfxView::WriteConfig();
  delete wxConfigBase::Set((wxConfigBase*)NULL);

#if 1
  Yield();
  exit(0);
#endif
}


// reset all data
void vsnApp::reset(const bool confirm) {
  if ( confirm ) {
    wxString msg(wxT("Are you sure to clear all data ?"));
    wxMessageDialog dlg(NULL, msg, wxT("Clear all data"),
			vsn_wxOK_CANCEL|wxICON_QUESTION);
    if ( dlg.ShowModal() != vsn_wxIDOK ) return;
  }

  // clear file path
  m_currentFile = "";
  m_importDir = "";

  // close view_frame_list w/o the 1st view
  deque<vsnViewFrame*>::iterator v_it;
  if ( m_viewList.size() > 0 ) {
    for ( v_it = m_viewList.begin() + 1; v_it != m_viewList.end(); ) {
      //if ( * v_it ) (*v_it)->Close(); // don't use view::Close(), we ignore
      if ( * v_it ) (*v_it)->Destroy();
      v_it = m_viewList.erase(v_it);
    } // end of for(v_it)
  }
  else {
    addViewFrame();
  }

  // delete scene_list w/o the 1st scene
  deque<vsnScene*>::iterator s_it;
  if ( m_sceneList.size() > 0 ) {
    for ( s_it = m_sceneList.begin() + 1; s_it != m_sceneList.end(); ) {
      if ( * s_it ) delete (*s_it);
      s_it = m_sceneList.erase(s_it);
    } // end of for(s_it)
  }
  else {
    addScene();
  }

  // delete method param canvas list
  deque<vsnMethodParamCanvas*>::iterator mpc_it;
  for ( mpc_it = m_mpcList.begin(); mpc_it != m_mpcList.end(); ) {
    if ( *mpc_it ) (*mpc_it)->Destroy();
    mpc_it = m_mpcList.erase(mpc_it);
  } // end of for(mpc_it)

  // the 1st scene
  s_it = m_sceneList.begin();
  assert(*s_it);
  (*s_it)->setName("Scene_0");
  (*s_it)->reset();

  // the 1st view
  v_it = m_viewList.begin();
  assert(*v_it);
  (*v_it)->setName("View_0");
  (*v_it)->reset();
  (*v_it)->updateSceneList(m_sceneList);
  (*v_it)->setScene((*s_it));

  updateViewTitle();
}

void vsnApp::update(const bool refreshMode, vsnViewFrame* exclView) {
  deque<vsnViewFrame*>::iterator it;
  for ( it = m_viewList.begin(); it != m_viewList.end(); it++ ) {
    if ( *it == exclView ) continue;
    (*it)->updateSceneList(m_sceneList);
    (*it)->refresh(refreshMode);
  } // end of for(it)
}


// ViewFrame-list control

vsnViewFrame* vsnApp::addViewFrame(vsnViewFrame* pvf) {
  if ( ! pvf ) {
    stringstream ss;
    ss << "View_" << m_viewList.size();
    while ( getViewFrame(ss.str()) )
      ss << "_" << m_viewList.size();
    pvf = new vsnViewFrame(NULL, ConvSysToWx(vsn_app_name), ss.str(),
			   wxDefaultPosition, wxSize(800, 600));
  }
  if ( ! pvf ) return NULL;

  deque<vsnViewFrame*>::iterator it;
  for ( it = m_viewList.begin(); it != m_viewList.end(); it++ )
    if ( *it == pvf ) return NULL;

  m_viewList.push_back(pvf);
  pvf->setApp(this);
  pvf->updateSceneList(m_sceneList);

  updateViewTitle();
  return pvf;
}

bool vsnApp::delViewFrame(vsnViewFrame* pvf) {
  if ( ! pvf ) return false;
  deque<vsnViewFrame*>::iterator it;
  for ( it = m_viewList.begin(); it != m_viewList.end(); it++ )
    if ( *it == pvf ) {
      m_viewList.erase(it);
      if ( m_viewList.empty() ) {
	vsnMethodParamCanvas* pmpc = getMethodParamCanvas(0);
	while ( pmpc ) {
	  delMethodParamCanvas(pmpc);
	  pmpc = getMethodParamCanvas(0);
	} // end of while(mpcc)
      }
      return true;
    }
  return false;
}

bool vsnApp::delViewFrame(const std::string& name) {
  vsnViewFrame* pvf = getViewFrame(name);
  return delViewFrame(pvf);
}

vsnViewFrame* vsnApp::getViewFrame(const size_t n) {
  if ( n >= m_viewList.size() ) return NULL;
  return m_viewList[n];
}

vsnViewFrame* vsnApp::getViewFrame(const std::string& name) {
  deque<vsnViewFrame*>::iterator it;
  for ( it = m_viewList.begin(); it != m_viewList.end(); it++ ) {
    if ( *it && (*it)->getName() == name )
      return (*it);
  } // end of for(it)
  return NULL;
}

void vsnApp::updateViewTitle() {
  wxString title0, title;

  if ( m_currentFile.empty() )
    title = wxT(" [no file]");
  else {
    string fileBase = BaseName(m_currentFile,
			       string(""), vsnPath_getDelimChar());
    if ( fileBase.empty() ) fileBase = "no file";
    title = wxT(" [");
    title += ConvSysToWx(fileBase);
    title += wxT("]");
  }

  deque<vsnViewFrame*>::iterator it;
  for ( it = m_viewList.begin(); it != m_viewList.end(); it++ ) {
    if ( ! (*it) ) continue;
    title0 = ConvSysToWx(vsn_app_name);
    title0 += ConvSysToWx(string(":") + (*it)->getName());
    (*it)->SetTitle(title0 + title);
  } // end of for(it)
}


// Scene-list control

vsnScene* vsnApp::addScene(vsnScene* psc) {
  if ( ! psc ) {
    stringstream ss;
    ss << "Scene_" << m_sceneList.size();
    while ( getScene(ss.str()) )
      ss << "_" << m_sceneList.size();
    psc = new vsnScene(ss.str());
  }
  if ( ! psc ) return NULL;

  // add to scene list
  deque<vsnScene*>::iterator sit;
  for ( sit = m_sceneList.begin(); sit != m_sceneList.end(); sit++ )
    if ( *sit == psc ) return NULL;
  m_sceneList.push_back(psc);

  // update scene list of view
  deque<vsnViewFrame*>::iterator vit;
  for ( vit = m_viewList.begin(); vit != m_viewList.end(); vit++ )
    (*vit)->updateSceneList(m_sceneList);

  return psc;
}

bool vsnApp::delScene(vsnScene* psc) {
  if ( ! psc ) return false;
  if ( m_sceneList.size() == 1 ) {
    ErrMsg(MsgWARN, string("App: delScene: can't delete the last scene"));
    return false;
  }

  deque<vsnScene*>::iterator it;
  for ( it = m_sceneList.begin(); it != m_sceneList.end(); it++ ) {
    if ( *it == psc ) {
      deque<vsnViewFrame*>::iterator vit;

      // clear the reference of views to the scene
      for ( vit = m_viewList.begin(); vit != m_viewList.end(); vit++ ) {
	if ( (*vit)->getScene() == psc )
	  (*vit)->setScene(NULL);
      } // end of for(vit)

      // delete the scene, and remove from sceneList
      delete (*it);
      m_sceneList.erase(it);

      // update scene list of views
      for ( vit = m_viewList.begin(); vit != m_viewList.end(); vit++ ) {
	(*vit)->updateSceneList(m_sceneList);
      } // end of for(vit)

      return true;
    }
  } // end of for(it)

  ErrMsg(MsgERR, string("App: delScene: can't delete the scene"));
  return false;
}

bool vsnApp::delScene(const std::string& name) {
  vsnScene* psc = getScene(name);
  return delScene(psc);
}

vsnScene* vsnApp::getScene(const size_t n) {
  if ( n >= m_sceneList.size() ) return NULL;
  return m_sceneList[n];
}

vsnScene* vsnApp::getScene(const std::string& name) {
  deque<vsnScene*>::iterator it;
  for ( it = m_sceneList.begin(); it != m_sceneList.end(); it++ ) {
    if ( *it && (*it)->getName() == name )
      return (*it);
  } // end of for(it)
  return NULL;
}


// Method param canvas control

vsnMethodParamCanvas*
vsnApp::addMethodParamCanvas(vsnMethodParamCanvas* pmpc) {
  if ( ! pmpc )
    pmpc = new vsnMethodParamCanvas(NULL, wxT("method param canvas"));
  if ( ! pmpc ) return NULL;

  deque<vsnMethodParamCanvas*>::iterator it;
  for ( it = m_mpcList.begin(); it != m_mpcList.end(); it++ )
    if ( *it == pmpc ) return NULL;

  m_mpcList.push_back(pmpc);
  return pmpc;
}

bool vsnApp::delMethodParamCanvas(vsnMethodParamCanvas* pmpc) {
  if ( ! pmpc ) return false;
  deque<vsnMethodParamCanvas*>::iterator it;
  for ( it = m_mpcList.begin(); it != m_mpcList.end(); it++ )
    if ( *it == pmpc ) {
      (*it)->Destroy();
      m_mpcList.erase(it);
      return true;
    }
  return false;
}

bool vsnApp::delMethodParamCanvas(const std::string& name) {
  vsnMethodParamCanvas* pmpc = getMethodParamCanvas(name);
  return delMethodParamCanvas(pmpc);
}

vsnMethodParamCanvas* vsnApp::getMethodParamCanvas(const size_t n) {
  if ( n >= m_mpcList.size() ) return NULL;
  return m_mpcList[n];
}

vsnMethodParamCanvas* vsnApp::getMethodParamCanvas(const std::string& name) {
  deque<vsnMethodParamCanvas*>::iterator it;
  for ( it = m_mpcList.begin(); it != m_mpcList.end(); it++ ) {
    if ( *it && (*it)->getName() == name )
      return (*it);
  } // end of for(it)
  return NULL;
}


bool vsnApp::initTmpls() {
  // clean tmpl list at first
  deque<vsnMethodTmpl*>::iterator t_it;
  for ( t_it = m_tmplList.begin(); t_it != m_tmplList.end(); ) {
    if ( *t_it ) delete (*t_it);
    t_it = m_tmplList.erase(t_it);
  } // end of for(t_it)

  std::string default_tmpl_dir
#if defined(LINUX)
    = "/usr/local/Vtools/etc/tmpl";
#elif defined(MacOSX)
  = "/Applications/Vtools/etc/tmpl";
#elif defined(WINDOWS)
  = "/Program Files/Vtools/etc/tmpl";
#else
  = "";
#endif

  std::string pTmplDir = default_tmpl_dir;
#if defined(WINDOWS)
  char* sysDrv = getenv("SYSTEMDRIVE");
  if ( sysDrv )
    pTmplDir = std::string(sysDrv) + default_tmpl_dir;
#endif

  char* pEnvTmplDir = getenv("VSN_TMPL_DIR");
  if ( pEnvTmplDir ) pTmplDir = pEnvTmplDir;
  if ( pTmplDir.empty() ) return false;
  if ( ! wxDir::Exists(ConvSysToWx(pTmplDir)) ) return false;

  // scan dir
  wxDir xDir(ConvSysToWx(pTmplDir));
  if ( ! xDir.IsOpened() ) return false;
  set<string> fileLst;
  wxString resStr;
  if ( xDir.GetFirst(&resStr, wxT("*.vsn"), wxDIR_FILES) &&
       ! resStr.IsEmpty() ) {
    fileLst.insert(vsnApp::ConvWxToSys(resStr));
    while ( xDir.GetNext(&resStr) )
      fileLst.insert(vsnApp::ConvWxToSys(resStr));
  }
  if ( xDir.GetFirst(&resStr, wxT("*.xml"), wxDIR_FILES) &&
       ! resStr.IsEmpty() ) {
    fileLst.insert(vsnApp::ConvWxToSys(resStr));
    while ( xDir.GetNext(&resStr) )
      fileLst.insert(vsnApp::ConvWxToSys(resStr));
  }

  // import
  set<string>::iterator fit;
  for ( fit = fileLst.begin(); fit != fileLst.end(); fit++ ) {
    if ( ! importTmpl(pTmplDir + "/" + (*fit)) ) continue;
  } // end of for(fit)

  return (! m_tmplList.empty());
}

bool vsnApp::importTmpl(const std::string& path) {
  if ( path.empty() ) return false;

  vsnMethodTmpl* pTmpl = new vsnMethodTmpl();
  if ( ! pTmpl ) return false;
  if ( ! pTmpl->loadXmlFile(path) ) {
    delete pTmpl;
    return false;
  }

  deque<vsnMethodTmpl*>::iterator t_it;
  for ( t_it = m_tmplList.begin(); t_it != m_tmplList.end(); t_it++ ) {
    if ( ! *t_it ) continue;
    if ( (*t_it)->getPath() == path ) {
      delete (*t_it);
      m_tmplList.erase(t_it);
      break;
    }
  } // end of for(t_it)
  for ( t_it = m_tmplList.begin(); t_it != m_tmplList.end(); t_it++ ) {
    if ( ! *t_it ) continue;
    if ( (*t_it)->getName() == pTmpl->getName() ) {
      delete (*t_it);
      m_tmplList.erase(t_it);
      break;
    }
  } // end of for(t_it)

  m_tmplList.push_back(pTmpl);
  return true;
}

std::deque<vsnMethodTmpl*> vsnApp::getTmplList(const std::string& dtype) {
  if ( dtype.empty() ) return m_tmplList;
  deque<vsnMethodTmpl*> tmplLst;

  deque<vsnMethodTmpl*>::iterator t_it;
  for ( t_it = m_tmplList.begin(); t_it != m_tmplList.end(); t_it++ ) {
    if ( ! *t_it ) continue;
    if ( (*t_it)->isSupportedDataType(dtype) )
      tmplLst.push_back(*t_it);
  } // end of for(t_it)

  return tmplLst;
}

vsnMethodTmpl* vsnApp::getTmpl(const std::string& tname,
			       const std::string& dtype) {
  if ( tname.empty() ) return NULL;
  deque<vsnMethodTmpl*>::iterator t_it;
  for ( t_it = m_tmplList.begin(); t_it != m_tmplList.end(); t_it++ ) {
    if ( ! *t_it ) continue;
    if ( (*t_it)->getName() == tname &&
	 (*t_it)->getDataType().find(dtype) != string::npos )
      return (*t_it);
  } // end of for(t_it)
  return NULL;
}


bool vsnApp::setOprOrientation(const vsnGfxOprOrientation& o) {
  if ( ! o.isValid() ) return false;
  m_oprOrient = o;

  deque<vsnViewFrame*>::iterator v_it;
  for ( v_it = m_viewList.begin(); v_it != m_viewList.end(); v_it++ ) {
    if ( ! *v_it ) continue;

    // escape from SetCenter mode if need
    (*v_it)->escapeSetCenterMode();

    // select Root item on UiView
    vsnUiView* puiv = (*v_it)->getUiView();
    if ( puiv ) {
      vsnTreeItem ti(TI_Scene);
      (void)puiv->selectObj(&ti);
    }

    // re-setup standard actions
    vsnGfxView* pgv = (*v_it)->getGfxView();
    if ( pgv ) pgv->setStandardActions();
  } // end of for(v_it)

  return true;
}


// from vsnIoObject

bool vsnApp::parseXML(xmlNodePtr xnp) {
  if ( ! xnp ) return false;

  vsnScene* pCurScene(NULL);
  vsnViewFrame* pCurView(NULL);
  string msgHdr = string("App: parseXML: ");

  bool firstScene(true), firstView(true);
  if ( ! m_openNew ) {
    firstScene = false;
    firstView = false;
  }

  // is 'vsn' or 'vfv' node?
  if ( strcmp((const char*)xnp->name, "vsn") &&
       strcmp((const char*)xnp->name, "vfv"))
    return false;

  // backup view-scene pair
  bool exScene = false, exView = false;
  map<vsnViewFrame*, vsnScene*> vspLst;
  deque<vsnViewFrame*>::iterator vit;
  for ( vit = m_viewList.begin(); vit != m_viewList.end(); vit++ ) {
    if ( ! *vit ) continue;
    vspLst.insert(make_pair(*vit, (*vit)->getScene()));
  } // end of for(vit)

  // traverse #1 : scene-graph
  xmlNodePtr cur = xnp->xmlChildrenNode;
  while ( cur ) {
    if ( (cur = vsnIoObject::SkipCommentXML(cur)) == NULL )
      break;

    if ( !strcmp((const char*)cur->name, "scene") ) {
      if ( firstScene )
	pCurScene = m_sceneList[0];
      else
	pCurScene = addScene();
      if ( ! pCurScene ) {
	ErrMsg(MsgFATAL, msgHdr + string("can't create scene, giving up"));
	return false;
      }

      // check dup-name
      if ( ! m_openNew )
	adjustSceneNameXML(cur);

      // parse Scene
      if ( ! pCurScene->parseXML(cur) ) {
	ErrMsg(MsgERR, msgHdr +
	       string("scene ")+pCurScene->getName()+string(" parse failed"));
	if ( firstScene )
	  pCurScene->reset();
	else
	  delScene(pCurScene);
	pCurScene = NULL;
      }
      if ( pCurScene && ! m_openNew ) exScene = true;
      if ( firstScene )
	firstScene = false;
    } // end of "scene"
    else if ( !strcmp((const char*)cur->name, "view") ) {
      if ( firstView )
	pCurView = m_viewList[0];
      else
	pCurView = addViewFrame();
      if ( ! pCurView ) {
	ErrMsg(MsgFATAL, msgHdr + string("can't create ViewFrame, giving up"));
	return false;
      }

      // check dup-name
      if ( ! m_openNew )
	adjustViewNameXML(cur);

      // parse ViewFrame
      if ( ! pCurView->parseXML(cur) ) {
	if ( firstView )
	  pCurView->reset();
	else
	  delViewFrame(pCurView);
	pCurView = NULL;
      }
      if ( pCurView ) {
	if ( m_openNew ) {
	  map<string, vsnViewPoint>& vplst = pCurView->getViewPointList();
	  if ( ! vplst.empty() )
	    pCurView->updateViewPoint(vplst.begin()->first, false);
	} else
	  exView = true;
	if ( firstView )
	  firstView = false;
	else
	  pCurView->Show();
      } // end of if(pCurView)
    } // end of "view"
    else if ( !strcmp((const char*)cur->name, "param_canvas") ) {
      vsnMethodParamCanvas* pmpc = addMethodParamCanvas();
      if ( ! pmpc ) {
	ErrMsg(MsgFATAL, msgHdr
	       + string("can't create ParamCanvas, giving up"));
	return false;
      }
      
      // parse ParamCanvas
      if ( ! pmpc->parseXML(cur) ) {
	delMethodParamCanvas(pmpc);
      }
      pmpc->Show();
    } // end of "param_canvas"

    cur = cur->next;
  } // end of while(cur)

  // post process for merge: new scene has created
  if ( exScene ) {
    // restore view-scene pair if new view has created
    if ( exView ) {
      map<vsnViewFrame*, vsnScene*>::iterator vsp_it;
      for ( vsp_it = vspLst.begin(); vsp_it != vspLst.end(); vsp_it++ ) {
	vsp_it->first->setScene(vsp_it->second);
      } // end of for(vit)
    }

    m_viewList[m_viewList.size() -1]
      ->setScene(m_sceneList[m_sceneList.size() -1]);

    map<string, vsnViewPoint>& vplst
      = m_viewList[m_viewList.size() -1]->getViewPointList();
    if ( ! vplst.empty() )
      m_viewList[m_viewList.size() -1]->updateViewPoint(vplst.begin()->first);
  }

  // update all ViewFrame
  update();

  // traverse #2 : commands
  cur = xnp->xmlChildrenNode;
  while ( cur ) {
    if ( (cur = vsnIoObject::SkipCommentXML(cur)) == NULL )
      break;

    if ( !strcmp((const char*)cur->name, "command") ) {
      if ( ! parseXMLCommand(cur) ) {
	; //ErrMsg(MsgERR, msgHdr + string("command parse failed"));
      }
    }

    cur = cur->next;
  } // end of while(cur)

  return true;
}

bool vsnApp::outputXML(std::ostream& os, const size_t ts) {
  string idts;
  for ( int i = 0; i < ts; i++ ) idts.push_back(' ');

  deque<vsnScene*>::const_iterator s_it;
  deque<vsnViewFrame*>::const_iterator v_it;
  deque<vsnMethodParamCanvas*>::const_iterator mpc_it;

  // ----- output -----
  os << idts << "<vsn>" << endl;
  m_extOutStr = "";

  // output scene(s)
  for ( s_it = m_sceneList.begin(); s_it != m_sceneList.end(); s_it ++ ) {
    (*s_it)->outputXML(os, (ts+2));
  }

  // output view(s)
  for ( v_it = m_viewList.begin(); v_it != m_viewList.end(); v_it ++ ) {
    (*v_it)->outputXML(os, (ts+2));
  }

  // output param_canvas(s)
  for ( mpc_it = m_mpcList.begin(); mpc_it != m_mpcList.end(); mpc_it ++ ) {
    (*mpc_it)->outputXML(os, (ts+2));
  }

  // output external string(s)
  if ( ! m_extOutStr.empty() ) {
    os << idts << "<!-- ext output strings: begin -->" << endl;
    os << m_extOutStr;
    os << idts << "<!-- ext output strings: end -->" << endl;
    m_extOutStr = "";
  }

  os << idts << "</vsn>" << endl;

  return true;
}

bool vsnApp::commandXML(xmlNodePtr xnp) {
  static xmlChar* xs = NULL;
  string xStr;
  string msgHdr = string("App: commandXML: ");

  // is 'command' node?
  if ( ! xnp ) return false;
  if ( strcmp((const char*)xnp->name, "command") ) return false;

  // is 'app' command?
  if ( xs ) xmlFree(xs);
  xs = xmlGetProp(xnp, (const xmlChar*)"target");
  if ( ! xs || strlen((const char*)xs) < 1 ) return false;
  xStr = (const char*)xs;
  if ( xStr != "app" && xStr != "App" && xStr != "APP" )
    return false;

  // get command name
  string nameStr, valueStr;
  if ( xs ) xmlFree(xs);
  xs = xmlGetProp(xnp, (const xmlChar*)"name");
  if ( ! xs || strlen((const char*)xs) < 1 ) {
    ErrMsg(MsgERR, msgHdr + "invalid command: no 'name' property");
    return false;
  }
  nameStr = (const char*)xs;

  // get command value (if there)
  if ( xs ) xmlFree(xs);
  xs = xmlGetProp(xnp, (const xmlChar*)"value");
  if ( xs && strlen((const char*)xs) > 1 )
    valueStr = (const char*)xs;

  // do the command
  if ( nameStr == "quit" ) {
    quit();
  } // end of 'quit'
  else if ( nameStr == "reset" || nameStr == "new" ) {
    reset();
  } // end of 'reset'|'new'
  else if ( nameStr == "open" ) {
    if ( valueStr.empty() ) {
      ErrMsg(MsgERR, msgHdr + string("command open: no value"));
      return false;
    }
    if ( ! parseXMLFile(valueStr.c_str()) )
      return false;
  } // end of 'open'
  else if ( nameStr == "open2" ) {
    if ( valueStr.empty() ) {
      ErrMsg(MsgERR, msgHdr + string("command open2: no value"));
      return false;
    }

    float fadeSec = 0.f;
    if ( xs ) xmlFree(xs);
    xs = xmlGetProp(xnp, (const xmlChar*)"fade");
    if ( xs && strlen((const char*)xs) > 0 )
      fadeSec = (float)atof((const char*)xs);

    bool viewKeep = true;
    if ( xs ) xmlFree(xs);
    xs = xmlGetProp(xnp, (const xmlChar*)"viewkeep");
    if ( xs && strlen((const char*)xs) > 0 && !strcmp((const char*)xs, "no") )
      viewKeep = false;

    string altImg;
    if ( xs ) xmlFree(xs);
    xs = xmlGetProp(xnp, (const xmlChar*)"altimg");
    if ( xs && strlen((const char*)xs) > 0 )
      altImg = string((const char*)xs);

    if ( ! parseXMLFile2(valueStr, viewKeep, fadeSec, altImg) )
      return false;
  } // end of 'open2'
  else if ( nameStr == "import" ) {
    if ( valueStr.empty() ) {
      ErrMsg(MsgERR, msgHdr + string("command import: no value"));
      return false;
    }
    bool bkup_openNew = m_openNew;
    m_openNew = false;
    if ( ! parseXMLFile(valueStr.c_str(), true) ) {
      m_openNew = bkup_openNew;
      return false;
    }
    m_openNew = bkup_openNew;
  } // end of 'import'
  else if ( nameStr == "save" ) {
    if ( m_currentFile.empty() ) {
      ErrMsg(MsgERR, msgHdr + string("command save: no current file"));
      return false;
    }
    if ( ! outputXMLFile(m_currentFile.c_str()) )
      return false;
  } // end of 'save'
  else if ( nameStr == "save_as" ) {
    if ( valueStr.empty() ) {
      ErrMsg(MsgERR, msgHdr + string("command save_as: no value"));
      return false;
    }
    if ( ! outputXMLFile(valueStr.c_str()) )
      return false;
  } // end of 'save_as'
  else if ( nameStr == "new_view" ) {
    vsnViewFrame* pnvf = addViewFrame();
    if ( ! pnvf ) {
      ErrMsg(MsgERR, msgHdr +
	     string("command new_view: can't create new ViewFrame"));
      return false;
    }
    if ( ! valueStr.empty() && valueStr != VFR_NONAME ) {
      vsnViewFrame* pov = getViewFrame(valueStr);
      if ( pov && pov != pnvf ) {
	ErrMsg(MsgWARN, msgHdr + string("command new_view: the view named ") +
	       valueStr + " has already exists, don't set name");
      } else
	pnvf->setName(valueStr);
    }
    pnvf->Show();
  } // end of 'new_view'
  else if ( nameStr == "close_view" ) {
    if ( valueStr.empty() || valueStr == VFR_NONAME ) {
      ErrMsg(MsgERR, msgHdr + string("command close_view: no value"));
      return false;
    }
    vsnViewFrame* pvf = getViewFrame(valueStr);
    if ( ! pvf ) {
      ErrMsg(MsgERR, msgHdr +
	     string("command close_view: can't find the view: ") + valueStr);
      return false;
    }
    pvf->closeView();
  } // end of 'close_view'
  else if ( nameStr == "add_scene" ) {
    vsnScene* pnsc = addScene();
    if ( ! pnsc ) {
      ErrMsg(MsgERR, msgHdr +
	     string("command add_scene: can't create new Scene"));
      return false;
    }
    if ( ! valueStr.empty() && valueStr != VFR_NONAME ) {
      vsnScene* posc = getScene(valueStr);
      if ( posc && posc != pnsc ) {
	ErrMsg(MsgWARN, msgHdr +
	       string("command add_scene: the scene named ") +
	       valueStr + " has already exists, don't set name");
      } else
	pnsc->setName(valueStr);
    }
  } // end of 'add_scene'
  else if ( nameStr == "del_scene" ) {
    if ( valueStr.empty() || valueStr == VFR_NONAME ) {
      ErrMsg(MsgERR, msgHdr + string("command del_scene: no value"));
      return false;
    }
    vsnScene* psc = getScene(valueStr);
    if ( ! psc ) {
      ErrMsg(MsgERR, msgHdr +
	     string("command del_scene: can't find the scene: ") + valueStr);
      return false;
    }
    if ( ! delScene(psc) ) {
      return false;
    }
  } // end of 'del_scene'
  else if ( nameStr == "add_param_canvas" ) {
    vsnMethodParamCanvas* pmpc = addMethodParamCanvas();
    if ( ! pmpc ) {
      ErrMsg(MsgERR, msgHdr + string("command add_param_canvas: ")
	     + string("can't create new param_canvas"));
      return false;
    }
    pmpc->Show();
    if ( ! valueStr.empty() && valueStr != VFR_NONAME ) {
      vsnMethodParamCanvas* pompc = getMethodParamCanvas(valueStr);
      if ( pompc && pompc != pmpc ) {
	ErrMsg(MsgWARN, msgHdr +
	       string("command add_param_canvas: the param_canvas named ") +
	       valueStr + " has already exists, don't set name");
      } else
	pmpc->setName(valueStr);
    }
  } // end of 'add_param_canvas'
  else if ( nameStr == "del_param_canvas" ) {
    if ( valueStr.empty() || valueStr == VFR_NONAME ) {
      ErrMsg(MsgERR, msgHdr + string("command del_param_canvas: no value"));
      return false;
    }
    vsnMethodParamCanvas* pmpc = getMethodParamCanvas(valueStr);
    if ( ! pmpc ) {
      ErrMsg(MsgERR, msgHdr + string("command del_param_canvas: ")
	     + string("can't find the param_canvas: ") + valueStr);
      return false;
    }
    if ( ! delMethodParamCanvas(pmpc) ) {
      return false;
    }
  } // end of 'del_param_canvas'
  else {
    ErrMsg(MsgERR, msgHdr + string("unknown command: ") + nameStr);
    return false;
  }

  update();
  return true;
}

// parse command

bool vsnApp::parseXMLCommand(const std::string& xcomstr) {
  if ( xcomstr.empty() ) return false;

  // get document
  string comstr;
  if ( m_needPathEncode && ! m_xmlEncode.empty() )
    comstr = string("<?xml version=\"1.0\" encoding=\"")
      + m_xmlEncode + string("\"?>");
  //comstr += wxFNCONV(xcomstr.c_str());
  comstr += xcomstr.c_str();
  xmlDocPtr doc = xmlParseMemory(comstr.c_str(), comstr.size());
  if ( ! doc ) return false;

  // traverse command nodes
  xmlNodePtr cur = xmlDocGetRootElement(doc);
  while ( cur ) {
    if ( (cur = vsnIoObject::SkipCommentXML(cur)) == NULL )
      break;
    if ( !strcmp((const char*)cur->name, "command") ) {
      if ( ! parseXMLCommand(cur) ) {
	xmlFreeDoc(doc);
	return false;
      }
    }
    cur = cur->next;
  } // end of while(cur)

  xmlFreeDoc(doc);
  return true;
}

bool vsnApp::parseXMLCommand(xmlNodePtr xnp) {
  static xmlChar* xs;
  if ( ! xnp ) return false;
  if ( strcmp((const char*)xnp->name, "command") ) return false;
  string msgHdr = string("App: parseXMLCommand: ");

  // get target name
  if ( xs ) xmlFree(xs);
  xs = xmlGetProp(xnp, (const xmlChar*)"target");
  if ( ! xs || strlen((const char*)xs) < 1 ) {
    ErrMsg(MsgERR, msgHdr + "invalid command: no 'target' property");
    return false;
  }
  string targStr((const char*)xs);

  // find target
  vsnIoObject* pTarget = NULL;
  if ( targStr == "app" || targStr == "App" || targStr == "APP" ) {
    pTarget = dynamic_cast<vsnIoObject*>(this);
  }
  if ( ! pTarget ) {
    // serach view list
    string targSub = targStr.substr(0, 5);
    if ( targSub == "view;" || targSub == "View;" || targSub == "VIEW;" ) {
      targSub = targStr.substr(5);
      if ( ! targSub.empty() && targSub != VFR_NONAME ) {
	deque<vsnViewFrame*>::iterator vit;
	for ( vit = m_viewList.begin(); vit != m_viewList.end(); vit++ ) {
	  if ( ! *vit ) continue;
	  if ( (*vit)->getName() == targSub ) {
	    pTarget = dynamic_cast<vsnIoObject*>(*vit);
	    if ( pTarget ) break;
	  }
	} // end of for(vit)
      } // end of if(!targSub.empty)
      else if ( m_viewList.size() > 0 ) {
	pTarget = m_viewList[0];
	targStr = string("view;") + m_viewList[0]->getName();
	xmlSetProp(xnp, (const xmlChar*)"target",
		   (const xmlChar*)(targStr.c_str()));
      } // end of "View;"
    } // end of "View;*"
  }
  if ( ! pTarget ) {
    if ( xs ) xmlFree(xs);
    xs = xmlGetProp(xnp, (const xmlChar*)"scene");
    char* searchName = (char*)xs;
    if ( ! searchName || strlen(searchName) < 1 ||
	 ! strcmp(searchName, VFR_NONAME) ) {
      // serach all scene
      deque<vsnScene*>::iterator sit;
      for ( sit = m_sceneList.begin(); sit != m_sceneList.end(); sit++ ) {
	if ( ! *sit ) continue;
	vfrNode* pnode = (*sit)->getNode(targStr);
	if ( ! pnode ) continue;
	pTarget = dynamic_cast<vsnIoObject*>(pnode);
	if ( pTarget ) break;
      } // end of for(sit)
    } // end of if(!searchName)
    else {
      // serach in the specified scene
      string search = searchName;
      deque<vsnScene*>::iterator sit;
      for ( sit = m_sceneList.begin(); sit != m_sceneList.end(); sit++ ) {
	if ( ! *sit ) continue;
	if ( (*sit)->getName() != search ) continue;
	vfrNode* pnode = (*sit)->getNode(targStr);
	if ( ! pnode ) continue;
	pTarget = dynamic_cast<vsnIoObject*>(pnode);
	if ( pTarget ) break;
      } // end of for(sit)
    } // end of if(searchName)
  }
  if ( ! pTarget ) {
    // serach ParamCanvas list
    string targSub = targStr.substr(0, 13);
    if ( targSub == "param_canvas;" ||
	 targSub == "Param_Canvas;" || targSub == "PARAM_CANVAS;" ) {
      targSub = targStr.substr(13);
      if ( ! targSub.empty() && targSub != VFR_NONAME ) {
	deque<vsnMethodParamCanvas*>::iterator pcit;
	for ( pcit = m_mpcList.begin(); pcit != m_mpcList.end(); pcit++ ) {
	  if ( ! *pcit ) continue;
	  if ( (*pcit)->getName() == targSub ) {
	    pTarget = dynamic_cast<vsnIoObject*>(*pcit);
	    if ( pTarget ) break;
	  }
	} // end of for(pcit)
      } // end of if(!targSub.empty)
    } // end of "Param_Canvas;*"
  }

  if ( ! pTarget ) {
    ErrMsg(MsgERR, msgHdr + "can't find target: " + targStr);
    return false;
  }

  // traverse command
  if ( ! pTarget->commandXML(xnp) )
    return false;

  update(true);
  return true;
}

bool vsnApp::parseXMLFile(const std::string& path, const bool merge) {
  string msgHdr = string("App: parseXMLFile: ");
  if ( path.empty() ) return false;
  if ( path == m_currentFile && merge ) {
    ErrMsg(MsgERR, msgHdr + string("can't import the same file"));
    return false;
  }

  //---- read and construct XML/DOM tree
  xmlDocPtr doc = xmlParseFile(path.c_str());
  if ( ! doc ) {
    ErrMsg(MsgERR, msgHdr + "invalid XML file: " + path);
    return false;
  }
  xmlNodePtr cur = xmlDocGetRootElement(doc);

  //---- search 'vsn' or 'vfv' node
  while ( cur ) {
    if ( ! strncmp((const char*)cur->name, "vsn", 3) ||
	 ! strncmp((const char*)cur->name, "vfv", 3) ) break;
    cur = cur->next;
  }
  if ( ! cur ) {
    ErrMsg(MsgERR, msgHdr + "no 'vsn' nor 'vfv' node exists");
    xmlFreeDoc(doc); return false;
  }

  //---- reset app (if need)
  if ( ! merge )
    reset();

  //---- set file path
  string bkupPath;
  if ( merge )
    bkupPath = m_currentFile;
  m_currentFile = path;
  if ( ! vsnPath_isAbsolute(m_currentFile) )
    m_currentFile = vsnPath_concat(m_cwd, m_currentFile);
  m_currentFile = vsnPath_normalize(m_currentFile);

  //---- parse the node
  if ( ! merge )
    m_openNew = true;
  if ( ! parseXML(cur) ) {
    ErrMsg(MsgERR, msgHdr + "parse failed");
    xmlFreeDoc(doc);
    m_openNew = false;
    m_currentFile = bkupPath;
    return false;
  }

  //---- update view title
  if ( merge )
    m_currentFile = bkupPath;
  updateViewTitle();

  m_openNew = false;
  xmlFreeDoc(doc);
  return true;
}

bool vsnApp::parseXMLFile2(const std::string& path, const bool viewKeep,
			   const float fadeSec, const std::string& altImgPath) {
  static xmlChar* xs;
  string msgHdr = string("App: parseXMLFile2: ");
  if ( path.empty() ) return false;
  string xpath = path;
  if ( ! vsnPath_isAbsolute(path) ) {
    if ( ! m_currentFile.empty() ) {
      string baseDir = DirName(m_currentFile, vsnPath_getDelimChar());
      xpath = vsnPath_concat(baseDir, path);
    } else {
      xpath = vsnPath_concat(m_cwd, path);
    }
  }

  //---- read and construct XML/DOM tree
  xmlDocPtr doc = xmlParseFile(xpath.c_str());
  if ( ! doc ) {
    ErrMsg(MsgERR, msgHdr + "invalid XML file: " + xpath);
    return false;
  }
  xmlNodePtr cur = xmlDocGetRootElement(doc);

  //---- search 'vsn' or 'vfv' node
  while ( cur ) {
    if ( ! strncmp((const char*)cur->name, "vsn", 3) ||
	 ! strncmp((const char*)cur->name, "vfv", 3) ) break;
    cur = cur->next;
  }
  if ( ! cur ) {
    ErrMsg(MsgERR, msgHdr + "no 'vsn' nor 'vfv' node exists");
    xmlFreeDoc(doc); return false;
  }

  //---- get current screen shot (and alt image)
  vsnViewFrame* pvf = getViewFrame(0);
  vsnGfxView* pgv = pvf->getGfxView();
  vfrDrawAreaWx* pda = pgv->getDrawArea();
  pda->getCanvas()->SetCurrent();
  vsnFrontImage* pfi = NULL;
  vsnFrontImage* pfi2 = NULL;
  if ( fadeSec >= 0.001f ) {
    Point2 ssz = pgv->getScreenShotSize();
    unsigned char* pss = pgv->screenShot();
    if ( pss ) {
      pfi = new vsnFrontImage();
      if ( pfi ) pfi->setImageData(pss, ssz.x, ssz.y, 3);
      DeAllocate(pss);
    }

    if ( ! altImgPath.empty() ) {
      wxImage altImg;
      if ( altImg.LoadFile(ConvSysToWx(altImgPath)) ) {
	pfi2 = new vsnFrontImage();
	if ( pfi2 ) {
	  wxImage mrrImg = altImg.Mirror(false);
	  pfi2->setImageData(mrrImg.GetData(),
			     mrrImg.GetWidth(), mrrImg.GetHeight(), 3);
	  pfi2->setAlpha(FALSE, 1.0f);
	}
      }
    }
  }
  vsnViewPoint vp = pgv->getXForm();

  //---- reset view
  deque<vsnViewFrame*>::iterator v_it;
  if ( m_viewList.size() > 0 ) {
    for ( v_it = m_viewList.begin() + 1; v_it != m_viewList.end(); ) {
      if ( * v_it ) (*v_it)->Destroy();
      v_it = m_viewList.erase(v_it);
    } // end of for(v_it)
  }
  pvf->clearViewpointList();

  //---- reset scene
  deque<vsnScene*>::iterator s_it;
  if ( m_sceneList.size() > 0 ) {
    for ( s_it = m_sceneList.begin() + 1; s_it != m_sceneList.end(); ) {
      if ( * s_it ) delete (*s_it);
      s_it = m_sceneList.erase(s_it);
    } // end of for(s_it)
  }
  s_it = m_sceneList.begin();
  (*s_it)->reset();

  //---- reset method param canvas list
  deque<vsnMethodParamCanvas*>::iterator mpc_it;
  for ( mpc_it = m_mpcList.begin(); mpc_it != m_mpcList.end(); ) {
    if ( *mpc_it ) (*mpc_it)->Destroy();
    mpc_it = m_mpcList.erase(mpc_it);
  } // end of for(mpc_it)

  //---- set file path
  m_currentFile = vsnPath_normalize(xpath);

  //---- arrange 'view' node (unlink 'view/param' node)
  xmlNodePtr chld = cur->xmlChildrenNode;
  while ( chld ) {
    if ( ! strncmp((const char*)chld->name, "view", 4) ) {
      xmlNodePtr chld2 = chld->xmlChildrenNode;
      while ( chld2 ) {
	if ( ! strncmp((const char*)chld2->name, "param", 5) ) {
	  string xsN = "";
	  if ( xs ) xmlFree(xs);
	  xs = xmlGetProp(chld2, (const xmlChar*)"name");
	  if ( xs && strlen((const char*)xs) > 0 ) xsN = (const char*)xs;
	  if ( xsN == string("show_ui") ||
	       xsN == string("show_aux_ui") ||
	       xsN == string("show_toolbar") ||
	       xsN == string("geometry") ) {
	    xmlNodePtr chldBk = chld2;
	    chld2 = chld2->next;
	    xmlUnlinkNode(chldBk);
	    continue;
	  }
	}
	chld2 = chld2->next;
      } // end of while(chld2)
    }
    chld = chld->next;
  } // end of while(chld)

  //---- parse the node
  m_openNew = true;
  m_chkProgress = false;
  pvf->Freeze();
  if ( ! parseXML(cur) ) {
    ErrMsg(MsgERR, msgHdr + "parse failed");
    xmlFreeDoc(doc);
    m_openNew = false;
    m_currentFile = "";
    m_chkProgress = true;
    pvf->Thaw();
    return false;
  }
  m_openNew = false;
  m_chkProgress = true;

  //---- update viewpoint and view title
  if ( viewKeep ) {
    pgv->setXForm(vp);
  } else {
    map<string, vsnViewPoint>& rvpl = pvf->getViewPointList();
    if ( rvpl.size() > 0 ) {
      pgv->setXForm(rvpl.begin()->second);
    }
  }
  updateViewTitle();
  pvf->Thaw();

  //---- fade in/out
  if ( pfi ) {
    vsnScene* psc = pgv->getScene();
    if ( pfi2 ) psc->addFrontObj(pfi2);
    psc->addFrontObj(pfi);
    pvf->setShowFrAxis(false);

    register double totalTime = (double)fadeSec;
    register double startTime = GetTime();
    register double elapsTime = 0.0;
    while ( elapsTime < totalTime ) {
      register float dt = (float)(elapsTime / totalTime);
      register float alp = 1.0 - dt;
      pfi->setAlpha(alp);
      pda->chkNotice();
      Yield();
      elapsTime = GetTime() - startTime;
    } // end of for(elapsTime)

    pvf->setShowFrAxis(true);
    delete pfi;
    if ( pfi2 ) delete pfi2;
  }

  xmlFreeDoc(doc);
  return true;
}

bool vsnApp::outputXMLFile(const std::string& path) {
  if ( path.empty() ) return false;

  string msgHdr = string("App: outputXMLFile: ");
  string bkupPath = m_currentFile;
  static const char* xml_doctype_str =
    "<!DOCTYPE vsn SYSTEM \"vsn.dtd\" [\n"
    "  <!ENTITY lt   \"&#38;#60;\">\n"
    "  <!ENTITY gt   \"&#62;\">\n"
    "  <!ENTITY amp  \"&#38;#38;\">\n"
    "  <!ENTITY apos \"&#39;\">\n"
    "  <!ENTITY quot \"&#34;\">\n"
    "]>\n";

  string xml_hdr_str;
  if ( m_needPathEncode && ! m_xmlEncode.empty() )
    xml_hdr_str = string("<?xml version=\"1.0\" encoding=\"")
      + m_xmlEncode + string("\"?>\n");
  else
    xml_hdr_str = "<?xml version=\"1.0\"?>\n";
  xml_hdr_str += xml_doctype_str;

  bool ret;
  if ( path == "-" ) {
    // write to std::cout
    m_currentFile = "";
    cout << xml_hdr_str;
    ret = outputXML(cout);
  }
  else {
    ofstream fout(path.c_str());
    if ( ! fout ) {
      ErrMsg(MsgERR, msgHdr + string("open failed: ") + path);
      return false;
    }
    m_currentFile = path;
    fout << xml_hdr_str;
    ret = outputXML(fout);
  }

  if ( ! ret ) {
    ErrMsg(MsgERR, msgHdr + "output failed");
    m_currentFile = bkupPath;
    return false;
  }

  if ( path == "-" )
    m_currentFile = bkupPath;
  updateViewTitle();
  return true;
}

void vsnApp::registExtOutStr(const std::string& str) const {
  m_extOutStr += str;
}


/* XML utils (private) */

void vsnApp::adjustSceneNameXML(xmlNodePtr cur) {
  static xmlChar* xs;
  if ( ! cur ) return;

  if ( xs ) xmlFree(xs);
  xs = xmlGetProp(cur, (const xmlChar*)"name");
  char* scName = (char*)xs;
  if ( ! scName || strlen(scName) < 1 ) return;

  deque<vsnScene*>::iterator sit, cur_sit;
  cur_sit = m_sceneList.end() - 1;
  if ( ! (*cur_sit) ) return;
  string curName = (*cur_sit)->getName();
  if ( curName.empty() || curName == VFR_NONAME ) return;

  string msgHdr = string("App: parseXML: ");
  bool changed = false;
  string xExistName;
  for ( sit = m_sceneList.begin(); sit != cur_sit; sit++ ) {
    xExistName = (*sit)->getName();
    if ( xExistName.empty() || xExistName == VFR_NONAME ) continue;
    if ( xExistName == string(scName) ) {
      ErrMsg(MsgWARN, msgHdr + string("the scene named ") + xExistName +
	     string(" already exists,\n so change the name of the scene to ") +
	     curName);
      xmlSetProp(cur, (const xmlChar*)"name",
		 (const xmlChar*)(curName.c_str()));
      changed = true;
    }
  } // end of for(sit)
  if ( ! changed ) return;

  // change scene-attrib of command(s)
  while ( (cur = cur->next) ) {
    if ( strcmp((const char*)cur->name, "command") ) continue;
    if ( xs ) xmlFree(xs);
    xs = xmlGetProp(cur, (const xmlChar*)"target");
    if ( ! xs || strlen((const char*)xs) < 1 ) continue;
    string targStr = (const char*)xs;
    if ( targStr == "app" || targStr == "App" || targStr == "APP" )
      continue;
    string targSub = targStr.substr(0, 5);
    if ( targSub == "view;" || targSub == "View;" || targSub == "VIEW;" )
      continue;

    if ( targStr == xExistName ) {
      xmlSetProp(cur, (const xmlChar*)"target",
		 (const xmlChar*)(curName.c_str()));
    }

    if ( xs ) xmlFree(xs);
    xs = xmlGetProp(cur, (const xmlChar*)"scene");
    char* searchName = (char*)xs;
    if ( ! searchName || strlen(searchName) < 1 ||
	 ! strcmp(searchName, VFR_NONAME) ) {
      continue;
    } // end of if(!searchName)

    if ( string(searchName) == xExistName ) {
      xmlSetProp(cur, (const xmlChar*)"scene",
		 (const xmlChar*)(curName.c_str()));
    }
  } // end of while(cur)
}

void vsnApp::adjustViewNameXML(xmlNodePtr cur) {
  static xmlChar* xs;
  if ( ! cur ) return;

  if ( xs ) xmlFree(xs);
  xs = xmlGetProp(cur, (const xmlChar*)"name");
  char* vName = (char*)xs;
  if ( ! vName || strlen(vName) < 1 ) return;

  deque<vsnViewFrame*>::iterator vit, cur_vit;
  cur_vit = m_viewList.end() - 1;
  if ( ! (*cur_vit) ) return;
  string curName = (*cur_vit)->getName();
  if ( curName.empty() || curName == VFR_NONAME ) return;

  string msgHdr = string("App: parseXML: ");
  bool changed = false;
  string xExistName;
  for ( vit = m_viewList.begin(); vit != cur_vit; vit++ ) {
    xExistName = (*vit)->getName();
    if ( xExistName.empty() || xExistName == VFR_NONAME ) continue;
    if ( xExistName == string(vName) ) {
      ErrMsg(MsgWARN, msgHdr + string("the view named ") + xExistName +
	     string(" already exists,\n so change the name of the view to ") +
	     curName);
      xmlSetProp(cur, (const xmlChar*)"name",
		 (const xmlChar*)(curName.c_str()));
      changed = true;
    }
  } // end of for(vit)
  if ( ! changed ) return;

  // change search-attrib of commands
  while ( (cur = cur->next) ) {
    if ( strcmp((const char*)cur->name, "command") ) continue;
    if ( xs ) xmlFree(xs);
    xs = xmlGetProp(cur, (const xmlChar*)"target");
    if ( ! xs || strlen((const char*)xs) < 1 ) continue;
    string targStr = (const char*)xs;
    string targSub = targStr.substr(0, 5);
    if ( targSub == "view;" || targSub == "View;" || targSub == "VIEW;" ) {
      targSub = targStr.substr(5);
      if ( targSub.empty() ) {
	targSub = m_viewList[0]->getName();
      }
      if ( targSub == xExistName ) {
	string ovrStr = "view;" + curName;
	xmlSetProp(cur, (const xmlChar*)"target",
		   (const xmlChar*)(ovrStr.c_str()));
      }
    } // end of "View;"
  } // end of while(cur)
}

// STATIC
wxString vsnApp::ConvSysToWx(const std::string& str) {
  wxString retStr;
  if ( str.empty() ) return retStr;
#ifdef WINDOWS
  retStr = wxString(str.c_str());
#else
  retStr = wxString::FromUTF8(str.c_str());
#endif
  return retStr;
}

// STATIC
std::string vsnApp::ConvWxToSys(const wxString& str) {
  string retStr;
  if ( str.IsEmpty() ) return retStr;
  retStr = str.mb_str();
  return retStr;
}


// event handler
void vsnApp::OnServerEvent(wxSocketEvent& event) {
  string msgHdr = string("App: SockServerEvent: ");

  wxSocketBase* sock = m_pSockServ->Accept(FALSE);
  if ( ! sock ) {
    ErrMsg(MsgWARN, msgHdr + string("can't accept a new connection"));
    return;
  }

  sock->SetEventHandler(*this, APP_SOCK_SOCKET);
  sock->SetNotify(wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG);
  sock->Notify(TRUE);
}

void vsnApp::OnSocketEvent(wxSocketEvent& event) {
  static string commandStr;
  string msgHdr = string("App: SockServerEvent: ");
#ifdef WINDOWS
  const char* el = "\r\n";
#else
  const char* el = " \n";
#endif

  wxSocketBase* sock = event.GetSocket();
  if ( ! sock ) {
    ErrMsg(MsgWARN, msgHdr + string("can't get socket connection"));
    return;
  }

  // process the event
  char buff[512];
  switch ( event.GetSocketEvent() ) {
  case wxSOCKET_INPUT:
    // disable input events, so that the test doesn't trigger
    // wxSocketEvent again
    sock->SetNotify(wxSOCKET_LOST_FLAG);

    // read data
    memset(buff, 0, 512);
    sock->Read(buff, 512);
    if ( sock->LastCount() > 0 ) {
      istringstream buffStr(buff); char c;
      while ( 1 ) {
	c = '\0';
        while ( buffStr.get(c) && (c != '\r' && c != '\n') )
          commandStr.push_back(c);
	if ( c == '\r' || c == '\n' ) {
	  // invoke XML command
	  char wbuff[8];
          if ( parseXMLCommand(commandStr) )
            sprintf(wbuff, "ok%s", el);
          else
            sprintf(wbuff, "ng%s", el);
          sock->Write(wbuff, /* strlen(buff) */ 4);

	  // skip return code(s)
          while ( buffStr.get(c) && (c == '\r' || c == '\n') );
	  commandStr = "";
        }
	else break;
      } // end of while(1)
    }

    // enable input events again.
    sock->SetNotify(wxSOCKET_LOST_FLAG | wxSOCKET_INPUT_FLAG);
    break;

  case wxSOCKET_LOST:
    sock->Destroy();
    break;

  default:
    break;
  } // end of switch(event.GetSocketEvent)

  return;
}


/////////////////////////
#if defined(WINDOWS)
void wxAssert(int cond,
              const wxChar *szFile,
              int nLine,
              const wxChar *szCond,
              const wxChar *szMsg)
{
}
#endif

