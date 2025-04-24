//
// vsnApp
//
#ifndef _VSN_APP_H_
#define _VSN_APP_H_

#ifdef WINDOWS
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#else
#include <sys/time.h>
#endif

#include "vsnViewFrame.h"
#include "vsnScene.h"
#include "vsnMethodParamCanvas.h"
#include "vsnMethodTmpl.h"
#include "vsnPathUtil.h"
#include "wx/socket.h"
#include "wx/intl.h"
#include <deque>

namespace VSN {
  // app name
  static const char vsn_app_name[] = "V-Isio";

  // program name
  static const char vsn_progname[] = "Visio";

  // version code of the program
  static const char vsn_version[] = "2.4.10b"
#ifdef PGSQL
    " (pgsql implements)"
#endif // PGSQL
    ;

  // copyright of the program
  static const char vsn_copyright[] = "(c) 2005-2015 RIKEN, 2018-2025 Fujitsu Ltd";


  // symbols for wxMessageDialog
  static const long vsn_wxOK_CANCEL = (wxOK|wxCANCEL);
  static const long vsn_wxIDOK      = wxID_OK;
  static const long vsn_wxIDCANCEL  = wxID_CANCEL;


  // ids for sock-service
  enum {APP_SOCK_PORT   = 9697,
	APP_SOCK_SERVER = 1000,
	APP_SOCK_SOCKET
  };
};


//----------------------------------------------------------------
// class vsnApp
//----------------------------------------------------------------
class vsnApp : public wxApp, public vsnIoObject {
public:
  vsnApp();
  virtual ~vsnApp();

  // basic commands
  void quit(const bool confirm =false);
  void reset(const bool confirm =false);
  void update(const bool refreshMode =false, vsnViewFrame* exclView =NULL);

  // view commands
  vsnViewFrame* addViewFrame(vsnViewFrame* pvf =NULL);
  bool delViewFrame(vsnViewFrame* pvf);
  bool delViewFrame(const std::string& name);
  size_t getNumViewFrame() const {return m_viewList.size();}
  vsnViewFrame* getViewFrame(const size_t n);
  vsnViewFrame* getViewFrame(const std::string& name);
  std::deque<vsnViewFrame*> getViewFrameList() const {return m_viewList;}
  void updateViewTitle();

  // scene commands
  vsnScene* addScene(vsnScene* psc =NULL);
  bool delScene(vsnScene* psc);
  bool delScene(const std::string& name);
  size_t getNumScene() const {return m_sceneList.size();}
  vsnScene* getScene(const size_t n);
  vsnScene* getScene(const std::string& name);
  std::deque<vsnScene*> getSceneList() const {return m_sceneList;}

  // param canvas commands
  vsnMethodParamCanvas* addMethodParamCanvas(vsnMethodParamCanvas* pmpc =NULL);
  bool delMethodParamCanvas(vsnMethodParamCanvas* pmpc);
  bool delMethodParamCanvas(const std::string& name);
  size_t getNumMethodParamCanvas() const {return m_mpcList.size();}
  vsnMethodParamCanvas* getMethodParamCanvas(const size_t n);
  vsnMethodParamCanvas* getMethodParamCanvas(const std::string& name);
  std::deque<vsnMethodParamCanvas*>* getMethodParamCanvasList() {
    return &m_mpcList;
  }

  // file context commands
  std::string getCwd() const {return m_cwd;}
  std::string getCurrentFilename() const {return m_currentFile;}
  std::string getImportDir() const {return m_importDir;}
  void setImportDir(const std::string& dir) {m_importDir = dir;}
  bool isChkProgress() const {return m_chkProgress;}
  std::string getScreenshotDir() const {return m_scshotDir;}
  void setScreenshotDir(const std::string& dir) {m_scshotDir = dir;}

  // method-template interface
  bool initTmpls();
  bool importTmpl(const std::string& path);
  std::deque<vsnMethodTmpl*>
    getTmplList(const std::string& dtype =std::string(""));
  vsnMethodTmpl* getTmpl(const std::string& tname,
			 const std::string& dtype =std::string(""));

  // locale / encoding interface
  bool needPathEncode() const {return m_needPathEncode;}
  std::string getXmlEncoding() const {return m_xmlEncode;}

  // gfx operation orientation interface
  vsnGfxOprOrientation getOprOrientation() const {return m_oprOrient;}
  bool setOprOrientation(const vsnGfxOprOrientation& o);

  // static method
  static vsnApp* GetApp();
  static std::string GetAppDir();
  static double GetTime();

  // from wxApp
  bool OnInit(void);

  // from IoObject
  virtual bool parseXML(xmlNodePtr xnp);
  virtual bool outputXML(std::ostream& os, const size_t ts =0);
  virtual bool commandXML(xmlNodePtr xnp);

  // XML
  bool parseXMLCommand(const std::string& comstr);
  bool parseXMLCommand(xmlNodePtr xnp);
  bool parseXMLFile(const std::string& path, const bool marge =false);
  bool parseXMLFile2(const std::string& path, const bool viewKeep,
		     const float fadeSec =0.f,
		     const std::string& altImgPath =std::string(""));
  bool outputXMLFile(const std::string& path);
  void registExtOutStr(const std::string& str) const;

  // encoding converter
  static wxString ConvSysToWx(const std::string& str);
  static std::string ConvWxToSys(const wxString& str);

  // event handler
  void OnServerEvent(wxSocketEvent& event);
  void OnSocketEvent(wxSocketEvent& event);

private:
  // scene list
  std::deque<vsnScene*> m_sceneList;
  // view list
  std::deque<vsnViewFrame*> m_viewList;
  // param canvas list
  std::deque<vsnMethodParamCanvas*> m_mpcList;
  // method tmpl list
  std::deque<vsnMethodTmpl*> m_tmplList;

  // file context
  std::string m_cwd;
  std::string m_currentFile;
  std::string m_importDir;
  std::string m_scshotDir;
  mutable bool m_openNew;
  mutable bool m_chkProgress;
  mutable std::string m_extOutStr;

  // sock server
  wxSocketServer* m_pSockServ;

  // locale
  wxLocale m_locale;
  bool m_needPathEncode;
  std::string m_xmlEncode;

  // gfx operation orientation
  vsnGfxOprOrientation m_oprOrient;

  // the app
  static vsnApp* s_pApp;
  static std::string s_appDir;

  // XML utils
  void adjustSceneNameXML(xmlNodePtr xnp);
  void adjustViewNameXML(xmlNodePtr xnp);

  // wxWidgets event table
  DECLARE_EVENT_TABLE()
};


// static inline method

inline vsnApp* vsnApp::GetApp() {
  return s_pApp;
}

inline std::string vsnApp::GetAppDir() {
  return s_appDir;
}

inline double vsnApp::GetTime() {
  double cur_time;
#ifdef WINDOWS
  DWORD t = timeGetTime(); // need to link to 'winmm.lib'
  cur_time = (double)t * 1e-3;
#else
  timeval tv;
  gettimeofday(&tv, NULL);
  cur_time = (double)tv.tv_sec + (double)tv.tv_usec * 1.0e-6;
#endif
  return cur_time;
}

#endif // _VSN_APP_H_
