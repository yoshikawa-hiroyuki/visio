//
// vsnDataSeqFiles
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

#include <algorithm>
#include "vsnDataSeqFiles.h"
#include "vsnApp.h"
#include "vsnError.h"

using namespace std;
using namespace CES;
using namespace VSN;


//----------------------------------------------------------------
// class vsnDataSeqFiles
//----------------------------------------------------------------

bool vsnDataSeqFiles::parseXML_SeqFiles(xmlNodePtr xnp,
					std::string& baseDir,
					std::deque<std::string>& seqFiles)
{
  static xmlChar* xs;

  vsnApp* pApp = vsnApp::GetApp();
  if ( ! xnp || ! pApp ) return false;

  // 'base_dir' prop
  baseDir = DirName(pApp->getCurrentFilename(), vsnPath_getDelimChar());
  if ( xs ) xmlFree(xs);
  xs = xmlGetProp(xnp, (const xmlChar*)"base_dir");
  if ( xs && strlen((const char*)xs) > 0 ) {
    string bdir = (const char*)xs;
    if ( pApp->needPathEncode() ) {
      wxString cvtPath = wxString::FromUTF8((const char*)xs);
      bdir = vsnApp::ConvWxToSys(cvtPath);
    } else
      bdir = (const char*)xs;
    if ( ! vsnPath_isAbsolute(bdir) )
      baseDir = vsnPath_concat(baseDir, bdir);
    else
      baseDir = bdir;
  }
  baseDir = vsnPath_normalize(baseDir);

  // 'seq' child node
  xmlNodePtr cur = xnp->xmlChildrenNode;
  while ( cur ) {
    if ( (cur = vsnIoObject::SkipCommentXML(cur)) == NULL ) break;
    if ( !strcmp((const char*)cur->name, "seq") ) {
      if ( xs ) xmlFree(xs);
      xs = xmlGetProp(cur, (const xmlChar*)"file");
      if ( ! xs || strlen((const char*)xs) < 1 ) {
        goto _NEXT_XML_NODE;
      }
      string seqpath;
      if ( pApp->needPathEncode() ) {
	wxString cvtPath = wxString::FromUTF8((const char*)xs);
	seqpath = vsnApp::ConvWxToSys(cvtPath);
      } else
	seqpath = (const char*)xs; 
      if ( ! vsnPath_isAbsolute(seqpath) )
	seqpath = vsnPath_concat(baseDir, seqpath);
      seqpath = vsnPath_normalize(seqpath);
      seqFiles.push_back(seqpath);
    } // end of 'seq'

  _NEXT_XML_NODE:
    cur = cur->next;
  } // end of while(cur)

  return true;
}

