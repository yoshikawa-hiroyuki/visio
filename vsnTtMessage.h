//
// vsnTtMessage
//
#ifndef _VSN_TT_MESSAGE_H_
#define _VSN_TT_MESSAGE_H_

#include <cstdio>
#include <cstring>
#include <tt/util/message.h>
#include <tt/util/string.h>


//----------------------------------------------------------------
// class vsnTtMessage
//----------------------------------------------------------------
class vsnTtMessage : public TtMessage {
public:
  vsnTtMessage(const char* pofn) : TtMessage(), m_pof(NULL) {
    if ( pofn && strlen(pofn) > 0 )
      m_pof = fopen(pofn, "w");
    if ( ! m_pof )
      m_pof = stderr;
  }
  virtual ~vsnTtMessage() {}

  // from TtMessage
  virtual void printInfo(const char* str) {
    if ( ! str ) return;
    if ( m_pof ) {fprintf(m_pof, "%s", str); fflush(m_pof);}
    m_history = str;
    if ( ! m_history.empty() ) tt_remove_newline(m_history);
  }
  virtual void printVerbose(const char* str) {
    printInfo(str);
  }
  virtual void printData(const char* str) {
    if ( ! m_pof || ! str ) return;
    fprintf(m_pof, "%s", str);
    fflush(m_pof);
  }
  virtual void startProgress(int percent) {
    if ( ! m_pof ) return;
    fprintf(m_pof, "[%3d %%]", percent);
    fflush(m_pof);
  }
  virtual void printProgress(int percent) {
    if ( ! m_pof ) return;
    fprintf(m_pof, "\b\b\b\b\b\b\b[%3d %%]", percent);
    fflush(m_pof);
  }
  virtual void endProgress(int percent) {
    if ( ! m_pof ) return;
    fprintf(m_pof, "\b\b\b\b\b\b\b");
    fflush(m_pof);
  }

  // static utils
  static void SetTtMessageClass(TtMessage* o, const bool delOld =true) {
    if ( tt::g_msg && delOld ) delete tt::g_msg;
    tt::g_msg = o;
  }

protected:
  FILE* m_pof;
};

#endif // _VSN_TT_MESSAGE_H_
