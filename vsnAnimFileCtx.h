//
// vsnAnimFileCtx
//
#ifndef _VSN_ANIM_FILE_CTX_H_
#define _VSN_ANIM_FILE_CTX_H_

#include "vsnError.h"


//----------------------------------------------------------------
// class vsnAnimFileCtx
//----------------------------------------------------------------

struct vsnAnimFileCtx {
  bool m_mkMov;
  std::string m_sfPrefix, m_sfPostfix, m_sfMov, m_sufx;
  int m_sfIdxPrec;
  std::deque<std::string> m_movFrames;

  vsnAnimFileCtx() : m_mkMov(false), m_sfIdxPrec(0) {}
  void reset();
  bool setup(const std::string& ftmpl);
  std::string getPath(const int idx) const;

  bool makeMovieFile();
};

#endif // _VSN_ANIM_FILE_CTX_H_
