//
// vsnGnuplotUnix
//
#ifndef _VSN_GNUPLOT_UNIX_H_
#define _VSN_GNUPLOT_UNIX_H_

#ifndef WINDOWS // non-Windows environments only

#include "vsnGnuplotIF.h"
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <set>


class vsnGnuplotUnix : public vsnGnuplotIF {
public:
  vsnGnuplotUnix();
  virtual ~vsnGnuplotUnix();

  virtual bool Open(const std::string& progname = std::string(""));
  virtual bool Close(const bool exitComm =true);
  virtual bool Command(const std::string& command);
  virtual int GetOrder() const;

private:
  FILE*  sfp;
  pid_t  pid;
  int    sfds[2];

  static std::set<vsnGnuplotUnix*> s_connLst;
  static void set_sig_handler();
  static void sig_handler(int sig, siginfo_t* sip, void* uc);
};

#endif // ! WINDOWS
#endif // _VSN_GNUPLOT_UNIX_H_
