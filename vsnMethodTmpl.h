//
// vsnMethodTmpl
//
#ifndef _VSN_METHOD_TMPL_H_
#define _VSN_METHOD_TMPL_H_

#include <string>
#include <vector>
#include <libxml/tree.h>


//----------------------------------------------------------------
// class vsnMethodTmpl
//----------------------------------------------------------------
class vsnMethodTmpl {
public:
  vsnMethodTmpl(const std::string& path =std::string(""));
  ~vsnMethodTmpl();

  const std::string& getName() const {return m_name;}
  const std::string& getPath() const {return m_path;}
  const std::string& getDataType() const {return m_dataType;}
  bool isSupportedDataType(const std::string& dt) const;

  xmlNodePtr getXmlNode() {return m_pXmlNode;}
  const xmlNodePtr getXmlNode() const {return m_pXmlNode;}

  bool loadXmlFile(const std::string& path);
  bool apply(class vsnDataObj* pdata) const;

private:
  std::string m_name;
  std::string m_path;
  std::string m_dataType;
  xmlNodePtr  m_pXmlNode;

  // sequence number
  unsigned long        m_tmplSeq;
  static unsigned long s_tmplSeq;
};

#endif // _VSN_METHOD_TMPL_H_
