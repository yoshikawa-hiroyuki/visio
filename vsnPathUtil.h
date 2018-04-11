//
// vsnPathUtil
//
#ifndef _VSN_PATH_UTIL_H_
#define _VSN_PATH_UTIL_H_

#include "utilPath.h"


namespace VSN {
  inline char vsnPath_getDelimChar() {
#ifdef WINDOWS
    return '\\';
#else
    return '/';
#endif
  }

  inline void vsnPath_adjustDelim(std::string& path) {
#ifdef WINDOWS
    const char newDelim = '\\';
    const char oldDelim = '/';
#else
    const char oldDelim = '\\';
    const char newDelim = '/';
#endif
    size_t pathLen = path.size();
    for ( register size_t i = 0; i < pathLen; i++ ) {
      if ( path[i] == oldDelim )
	path[i] = newDelim;
    } // end of for(i)
  }

  inline bool vsnPath_hasDrive(const std::string& path) {
    if ( path.size() < 2 ) return false;
    char x = path[0];
    if ( ((x >= 'A' && x <= 'Z' ) || (x >= 'a' && x <= 'z')) &&
	 path[1] == ':' )
      return true;
    return false;
  }

  inline std::string vsnPath_emitDrive(std::string& path) {
    // returns drive (ex. 'C:')
    if ( ! vsnPath_hasDrive(path) ) return std::string();
    std::string driveStr = path.substr(0, 2);
    path = path.substr(2);
    return driveStr;
  }

  inline bool vsnPath_isAbsolute(const std::string& path) {
    std::string xpath(path);
    vsnPath_emitDrive(xpath);
    return (xpath[0] == vsnPath_getDelimChar());
  }

  inline std::string vsnPath_concat(const std::string& path1,
				    const std::string& path2) {
    if ( vsnPath_isAbsolute(path2) ) return path2; 
    std::string xpath;
    if ( ! path1.empty() ) {
      xpath = path1;
      xpath += vsnPath_getDelimChar();
    }
    xpath += path2;
    return xpath;
  }

  inline std::string vsnPath_normalize(const std::string& path) {
    std::string xpath(path);
    vsnPath_adjustDelim(xpath);
    std::string driveStr = vsnPath_emitDrive(xpath);
    xpath = CES::OmitDots(xpath, vsnPath_getDelimChar());
#if defined(WIN32) || defined(WIN64)
	if ( xpath[0] == '\\' )
      xpath = string("\\") + xpath;
#endif
    if ( ! driveStr.empty() )
      xpath = driveStr + xpath;
    return xpath;
  }
};

#endif // _VSN_PATH_UTIL_H_


