//
// vsnGnuplotMSW
//

#include "stdafx.h"
#ifdef WINDOWS // Windows environments only
#include <locale.h>
#include <string>
#include "vsnGnuplotMSW.h"


//----------------------------------------------------------------
// class vsnGnuplotUnix
//----------------------------------------------------------------

#define PROGNAME "wgnuplot.exe"
#define TEXTCLASS "wgnuplot_text"
#define BUFFER_SIZE 512


/* constructors / destructor */

vsnGnuplotMSW::vsnGnuplotMSW()
  : vsnGnuplotIF(), hwndParent(NULL), hwndText(NULL)
{
}

vsnGnuplotMSW::~vsnGnuplotMSW() {
  vsnGnuplotMSW::Close();
}


/* implements */

PROCESS_INFORMATION x_piProcInfo;
STARTUPINFO         x_siStartInfo;

bool vsnGnuplotMSW::Open(const std::string& progname) {
  if ( IsConnected() ) return false;

  wchar_t psGnuplotCommandLine[MAX_PATH];
  BOOL    bSuccess;

  if ( isatty(fileno(stdin)) ) {
    return false;
  }

  size_t retVal;
  if ( ! progname.empty() ) {
    mbstowcs_s(&retVal, psGnuplotCommandLine, MAX_PATH,
	       progname.c_str(), progname.size());
  } else {
    mbstowcs_s(&retVal, psGnuplotCommandLine, MAX_PATH,
	       PROGNAME, std::string(PROGNAME).size());
  }
  piProcInfo = x_piProcInfo;
  siStartInfo = x_siStartInfo;

  siStartInfo.cb = sizeof(STARTUPINFO); 
  siStartInfo.lpReserved = NULL; 
  siStartInfo.lpReserved2 = NULL; 
  siStartInfo.cbReserved2 = 0; 
  siStartInfo.lpDesktop = NULL; 
  siStartInfo.dwFlags = STARTF_USESHOWWINDOW;
  siStartInfo.wShowWindow = SW_SHOWMINIMIZED;

  bSuccess = CreateProcess( 
	NULL,                   /* pointer to name of executable module   */
	psGnuplotCommandLine,   /* pointer to command line string         */
	NULL,                   /* pointer to process security attributes */
	NULL,                   /* pointer to thread security attributes  */
	FALSE,                  /* handle inheritance flag                */
	0,                      /* creation flags                         */
	NULL,                   /* pointer to new environment block       */
	NULL,                   /* pointer to current directory name      */
	&siStartInfo,           /* pointer to STARTUPINFO                 */
	&piProcInfo             /* pointer to PROCESS_INFORMATION         */
  );

  /* if CreateProcess() failed, print a warning and exit. */
  if ( ! bSuccess ) {
    return false;
  }

  if ( WaitForInputIdle(piProcInfo.hProcess, 1000) ) {
    return false;
  }

  /* get the HWND of the parent window and text windows */
  EnumThreadWindows(piProcInfo.dwThreadId, cbGetTextWindow, (LPARAM)this);

  /* free the process and thread handles */
  CloseHandle(piProcInfo.hProcess);
  CloseHandle(piProcInfo.hThread);

  if ( ! hwndParent || ! hwndText ) {
    /* Still no gnuplot window? something wrong... */
    return false;
  }

  m_connected = true;
  return true;
}

bool vsnGnuplotMSW::Close(const bool exitComm) {
  if ( ! IsConnected() ) return false;

  if ( exitComm )
    PostString(hwndText, "\nexit\n");
  m_connected = false;
  return true;
}

bool vsnGnuplotMSW::Command(const std::string& command) {
  if ( ! IsConnected() ) return false;
  if ( command.empty() ) return false;

  char psBuffer[BUFFER_SIZE];
  if ( command[command.size()-1] == '\\' ) {
    sprintf(psBuffer, "%s", command.c_str());
    psBuffer[command.size()-1] = '\0';
  } else
    sprintf(psBuffer, "%s\r\n", command.c_str());
  PostString(hwndText, psBuffer);

  return true;
}


/* static utils */

BOOL CALLBACK vsnGnuplotMSW::cbGetTextWindow(HWND hwnd, LPARAM lParam)
{
  vsnGnuplotMSW* pif = (vsnGnuplotMSW*)lParam;
  if ( ! pif ) return FALSE;

  /* save the value of the parent window */
  pif->hwndParent = hwnd;
  /* check to see if it has a child text window */
  size_t retVal;
  wchar_t textBuff[MAX_PATH];
  mbstowcs_s(&retVal, textBuff, MAX_PATH, TEXTCLASS,
	     std::string(TEXTCLASS).size());
  pif->hwndText = FindWindowEx(hwnd, NULL, textBuff, NULL);

  /* if the text window was found, stop looking */
  return ( pif->hwndText == NULL );
}

void vsnGnuplotMSW::PostString(HWND hwnd, char *pc)
{
  while ( *pc ){
    PostMessage(hwnd, WM_CHAR, *pc, 1L);
    /* should add a check of return code on PostMessage. 
       if 0, the message que was full and the message wasn't posted. */
    pc++;
  }
}


//----------------------------------------------------------------
// class vsnGnuplotIF
//----------------------------------------------------------------

vsnGnuplotIF* VSN::GetGnuplotIF() {
  vsnGnuplotMSW* pgpif = new vsnGnuplotMSW();
  return (vsnGnuplotIF*)pgpif;
}

#endif // WINDOWS
