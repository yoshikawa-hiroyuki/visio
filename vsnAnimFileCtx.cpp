//
// vsnAnimFileCtx
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
#include "wx/utils.h"
#include "wx/filename.h"

#include "vsnAnimFileCtx.h"


//----------------------------------------------------------------
// class vsnAnimFileCtx
//----------------------------------------------------------------

void vsnAnimFileCtx::reset() {
  m_mkMov = false;
  m_sfPrefix = m_sfPostfix = m_sfMov = m_sufx = "";
  m_sfIdxPrec = 0;
  m_movFrames.clear();
}

bool vsnAnimFileCtx::setup(const std::string& ftmpl) {
  if ( ftmpl.empty() ) return false;
  string fileTmpl = vsnPath_normalize(ftmpl);
  string::size_type dot = fileTmpl.rfind('.');
  if ( dot == string::npos ) return false;
  m_sufx = fileTmpl.substr(dot + 1);
  if ( m_sufx.empty() ) return false;
  if ( m_sufx == "mpg" ||  // MPEG-1
       m_sufx == "avi" ||  // AVI with MS-MPEG4v2(DivX4/5) codec
       m_sufx == "mov" ||  // QuickTime with MPEG-4 codec
       m_sufx == "mp4" )   // ISMA MPEG-4
    m_mkMov = true;
  else if ( m_sufx == "png" || m_sufx == "jpg" ||
            m_sufx == "tif" || m_sufx == "bmp" ) // not support GIF
    m_mkMov = false;
  else
    return false;

  m_sfPrefix = m_sfPostfix = m_sfMov = ""; m_sfIdxPrec = 0;
  if ( ! m_mkMov ) {
    istringstream strBuff(fileTmpl); char c;
    while ( strBuff.get(c) && c != '#' )
      m_sfPrefix.push_back(c);
    if ( ! strBuff.eof() ) m_sfIdxPrec++;
    while ( strBuff.get(c) && c == '#' )
      m_sfIdxPrec++;
    if ( ! strBuff.eof() ) m_sfPostfix.push_back(c);
    while ( strBuff.get(c) )
      m_sfPostfix.push_back(c);
  } else {
    m_sfMov = fileTmpl;
    m_sfPrefix
      = vsnApp::ConvWxToSys(wxFileName::CreateTempFileName(wxT("vsn")));
    m_sfPostfix = ".png";
    m_sfIdxPrec = 8;
    wxRemoveFile(vsnApp::ConvSysToWx(m_sfPrefix));
  }
  return true;
}

std::string vsnAnimFileCtx::getPath(const int idx) const {
  string repStr;
  if ( m_sfPrefix.empty() && m_sfPostfix.empty() ) return repStr;
  repStr = m_sfPrefix;
  if ( m_sfIdxPrec > 0 ) {
    char fmtBuff[16], idxBuff[64];
    sprintf(fmtBuff, "%%0%dd", m_sfIdxPrec);
    sprintf(idxBuff, fmtBuff, idx);
    repStr += idxBuff;
  }
  repStr += m_sfPostfix;
  return repStr;
}

bool vsnAnimFileCtx::makeMovieFile() {
  if ( ! m_mkMov ) return false;
  size_t numFrms = m_movFrames.size();

  // duplicate the last frame
  string sspath0 = getPath(numFrms - 1);
  string sspath1 = getPath(numFrms);
  if ( wxCopyFile(vsnApp::ConvSysToWx(sspath0), vsnApp::ConvSysToWx(sspath1)) )
    m_movFrames.push_back(sspath1);

  // create the movie by using ffmpeg
  char fmtBuf[16]; sprintf(fmtBuf, "%%0%dd", m_sfIdxPrec);
#if defined(MacOSX)
  string encCom = "/Applications/Vtools/bin/ffmpeg -i \"";
#elif defined(Linux)
  string encCom = "/usr/local/Vtools/bin/ffmpeg -i \"";
#else
  string encCom = "ffmpeg -i \"";
#endif
  encCom += m_sfPrefix;
  encCom += fmtBuf;
  encCom += m_sfPostfix;
  encCom += '\"';
  if ( m_sufx == "mpg" )
    encCom += " -f mpeg -vcodec mpeg1video";
  else if ( m_sufx == "avi" )
    encCom += " -f avi -vcodec mjpeg";
  else if ( m_sufx == "mov" )
    encCom += " -f mov -vcodec mpeg4";
  else if ( m_sufx == "mp4" )
    encCom += " -f mp4 -vcodec mpeg4";
  sprintf(fmtBuf, " -qscale 0 ");
  encCom += fmtBuf;
  encCom += '\"';
  encCom += m_sfMov;
  encCom += '\"';

  bool retVal = true;
  int ret = system(encCom.c_str());
#if defined(WINDOWS)
  if ( ret != 0  ) {
#else
  int xret = WEXITSTATUS(ret);
  if ( xret != 0 ) {
#endif
    retVal = false;
  }

  // remove all frames of movie
  deque<string>::iterator it;
  for ( it = m_movFrames.begin(); it != m_movFrames.end(); it++ ) {
    wxRemoveFile(vsnApp::ConvSysToWx(*it));
  }

  return retVal;
}
