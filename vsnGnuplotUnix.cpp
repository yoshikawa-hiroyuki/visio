//
// vsnGnuplotUnix
//

#ifndef WINDOWS // non-Windows environments only

#include "vsnGnuplotUnix.h"
#include "vsnPathUtil.h"
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <sys/wait.h>

using namespace std;


//----------------------------------------------------------------
// class vsnGnuplotUnix
//----------------------------------------------------------------

/* static members */

std::set<vsnGnuplotUnix*> vsnGnuplotUnix::s_connLst;


/* constructors / destructor */

vsnGnuplotUnix::vsnGnuplotUnix()
  : vsnGnuplotIF(), sfp(NULL), pid(0)
{
  sfds[0] = sfds[1] = 0;

  set_sig_handler();
}

vsnGnuplotUnix::~vsnGnuplotUnix() {
  vsnGnuplotUnix::Close();
}


/* implements */

bool vsnGnuplotUnix::Open(const std::string& progname) {
  if ( IsConnected() ) return false;

  // create args for exec
  char bf0[128];
  char *args[2] = {bf0, NULL};
  if ( ! progname.empty() )
    strncpy(bf0, progname.c_str(), 128);
  else
    strcpy(bf0, "gnuplot");

  if ( VSN::vsnPath_isAbsolute(bf0) ) {
    FILE* xfp = fopen(bf0, "r");
    if ( ! xfp ) return false;
    fclose(xfp);
  }

  // create pipe
  if ( pipe(sfds) < 0 )
    return false;

  // fork child process
  pid = fork();
  if ( pid < 0 ) return false;

  // child process
  if ( pid == 0 ) {
    dup2(sfds[0], 0);
    close(sfds[1]);
    close(sfds[0]);
    execvp(*args, args);
    kill(getppid(), SIGPIPE);
    fprintf(stderr, "Caution: exec gnuplot failed.\n"); fflush(stderr);
    _exit(-1);
  }

  // parent process
  close(sfds[0]);
  sfp = fdopen(sfds[1], "w");
  if ( ! sfp ) return false;

  // add to connection list
  s_connLst.insert(this);
  m_connected = true;

  // testing connection
  fprintf(sfp, ";"); fflush(sfp);
  if ( ! m_connected ) return false;

  return true;
}

bool vsnGnuplotUnix::Close(const bool exitComm) {
  if ( ! IsConnected() ) return false;

  if ( exitComm )
    (void)Command(std::string("exit"));

  fclose(sfp); sfp = NULL;
  close(sfds[1]); sfds[1] = 0;
  m_connected = false;

  int status;
  waitpid(pid, &status, 0);

  std::set<vsnGnuplotUnix*>::iterator it = s_connLst.find(this);
  if ( it != s_connLst.end() ) s_connLst.erase(it);

  return true;
}

bool vsnGnuplotUnix::Command(const std::string& command) {
  if ( ! IsConnected() ) return false;

  char buff[512]; memset(buff, 0, 512);
  if ( command.empty() ) return false;
  sprintf(buff, "%s\n", command.c_str()); // need '\n'

  fprintf(sfp, buff);
  fflush(sfp);

  if ( ferror(sfp) ) {
    clearerr(sfp);
    if ( ferror(sfp) )
      return false;
  }

  return true;
}

int vsnGnuplotUnix::GetOrder() const {
  set<vsnGnuplotUnix*>::iterator it;
  register int i = 0;
  for ( it = s_connLst.begin(); it != s_connLst.end(); it++, i++ )
    if ( (*it) == this ) break;
  if ( it == s_connLst.end() ) return -1;
  return i;
}


/* static methods */

void vsnGnuplotUnix::set_sig_handler() {
  static bool hasSet(false);

  if ( hasSet ) return;
  struct sigaction sa_sigint;
  memset(&sa_sigint, 0, sizeof(sa_sigint));
  sa_sigint.sa_sigaction = vsnGnuplotUnix::sig_handler;
  sa_sigint.sa_flags = SA_SIGINFO;

  if ( sigaction(SIGPIPE, &sa_sigint, NULL) < 0 ) return;
  hasSet = true;
  return;
}

void vsnGnuplotUnix::sig_handler(int sig, siginfo_t* sip, void* uc) {
  if ( sig != SIGPIPE ) return;

  std::set<vsnGnuplotUnix*>::iterator it;
  for ( it = s_connLst.begin(); it != s_connLst.end(); it++ ) {
    if ( ! *it ) continue;
    (*it)->Close(false);
  } // end of for(it)
  s_connLst.clear();

  return;
}


//----------------------------------------------------------------
// class vsnGnuplotIF
//----------------------------------------------------------------

vsnGnuplotIF* VSN::GetGnuplotIF() {
  vsnGnuplotUnix* pgpif = new vsnGnuplotUnix();
  return (vsnGnuplotIF*)pgpif;
}

#endif // ! WINDOWS
