//
// vsnIoObject
//
#ifndef _VSN_IO_OBJECT_H_
#define _VSN_IO_OBJECT_H_

#include <string>
#include <iostream>
#include <sstream>
#include <libxml/tree.h>


class vsnIoObject {
public:
  virtual ~vsnIoObject() {}

  virtual bool parseXML(xmlNodePtr xnp) {return true;}
  virtual bool outputXML(std::ostream& os, const size_t ts =0) {
    return true;
  }
  virtual bool commandXML(xmlNodePtr xnp) {return true;}

  // static utilities
  std::string ConvXmlEntChars(const std::string& str);

protected:
  vsnIoObject() {}

  static xmlNodePtr SkipCommentXML(xmlNodePtr cur);
};


/* inline static methods */

inline std::string vsnIoObject::ConvXmlEntChars(const std::string& str) {
  std::string xstr;
  std::istringstream bstr(str); char c;
  while ( bstr.get(c) ) {
    switch ( c ) {
    case '<':  xstr += "&lt;";   break;
    case '>':  xstr += "&gt;";   break;
    case '\'': xstr += "&apos;"; break;
    case '"':  xstr += "&quot;"; break;
    default: xstr.push_back(c);
    } // end of switch(c)
  } // end of while(bstr.get)
  return xstr;  
}

inline xmlNodePtr vsnIoObject::SkipCommentXML(xmlNodePtr cur) {
  while ( cur ) {
    if ( cur->type == XML_COMMENT_NODE ) {
      cur = cur->next;
      continue;
    }
    break;
  }
  return cur;
}

#endif // _VSN_IO_OBJECT_H_
