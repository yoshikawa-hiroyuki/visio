//
// vsnGnuplotMSW
//
#ifndef _VSN_GNUPLOT_MSW_H_
#define _VSN_GNUPLOT_MSW_H_

#ifdef WINDOWS // Windows environments only

#include "vsnGnuplotIF.h"
#include <stdlib.h>
#include <windows.h>
#include <io.h>
#include <conio.h>
#include <fcntl.h>
#include <string.h>


class vsnGnuplotMSW : public vsnGnuplotIF {
public:
  vsnGnuplotMSW();
  virtual ~vsnGnuplotMSW();

  virtual bool Open(const std::string& progname = std::string(""));
  virtual bool Close(const bool exitComm =true);
  virtual bool Command(const std::string& command);

private:
  HWND hwndParent;
  HWND hwndText;

  PROCESS_INFORMATION piProcInfo;
  STARTUPINFO         siStartInfo;

  static BOOL CALLBACK cbGetTextWindow(HWND hwnd, LPARAM lParam);
  static void PostString(HWND hwnd, char* pc);
};

#endif // WINDOWS
#endif // _VSN_GNUPLOT_MSW_H_
