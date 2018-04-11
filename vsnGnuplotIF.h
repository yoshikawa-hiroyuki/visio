//
// vsnGnuplotIF
//
#ifndef _VSN_GNUPLOT_IF_H_
#define _VSN_GNUPLOT_IF_H_

#include <stdio.h>
#include <string>


class vsnGnuplotIF {
public:
  vsnGnuplotIF() : m_connected(false) {}
  virtual ~vsnGnuplotIF() {}

  virtual bool Open(const std::string& progname = std::string("")) =0;
  virtual bool Close(const bool extiComm =true) =0;
  virtual bool Command(const std::string& command) =0;
  virtual int GetOrder() const {return -1;}
  bool IsConnected() const {return m_connected;}

protected:
  bool m_connected;
};


namespace VSN {
  vsnGnuplotIF* GetGnuplotIF();
};

#endif // _VSN_GNUPLOT_IF_H_
