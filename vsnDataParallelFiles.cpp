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
#include "vsnDataParallelFiles.h"
#include "vsnApp.h"
#include "vsnError.h"

using namespace std;
using namespace VSN;

bool 
vsnDataParallelFiles::parseXML_ParallelFiles(xmlNodePtr xnp,
					     string& baseDir,
					     vector<ParallelFileInfo>*
					     dataFiles)
{
  static xmlChar* xs;

  vsnApp* pApp = vsnApp::GetApp();
  if ( ! xnp || ! pApp ) return false;

  // 'base_dir' prop
  baseDir = CES::DirName(pApp->getCurrentFilename(), vsnPath_getDelimChar());
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
    if ( !strcmp((const char*)cur->name, "parallel") ) {
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

      xs = xmlGetProp(cur, (const xmlChar*)"mode");
      if (!xs || strlen((const char*)xs) < 1) {
	goto _NEXT_XML_NODE;
      }
      string mode = (const char*)xs;
      bool flag = false;
      if (mode == string("data")) flag = true;
      else if (mode == string("skel")) flag = false;
      else goto _NEXT_XML_NODE;

      std::pair<std::string, bool> parallelFileInfo(seqpath, flag);
      dataFiles -> push_back(parallelFileInfo);
	//      seqFiles.push_back(seqpath);
    } // end of 'seq'

  _NEXT_XML_NODE:
    cur = cur->next;
  } // end of while(cur)

  //sort(seqFiles.begin(), seqFiles.end());
  return true;
}

