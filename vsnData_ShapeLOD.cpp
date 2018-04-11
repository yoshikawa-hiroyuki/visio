//
// vsnData_ShapeLOD
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

#include "vsnData_ShapeLOD.h"
#include "vsnError.h"
#include "vfruWfObj.h"
#include "vfruStl.h"

#include "vsnMethod_ShapeLOD_info.h"
#include "vsnMethod_ShapeLOD_trias_lod.h"
#include "vsnMethod_keyFrameAnim.h"
#include "vsnMethod_label.h"

#include <sstream>
#include <algorithm>

using namespace std;
using namespace CES;
using namespace VSN;


/* constructors / destructor */

vsnData_ShapeLOD::vsnData_ShapeLOD(const string& name)
  : vsnDataObj(name), m_pShapes(NULL)
{
}

vsnData_ShapeLOD::~vsnData_ShapeLOD() {
  if ( m_pShapes )
    delete m_pShapes;
}


/* methods */

deque<string> vsnData_ShapeLOD::getSupportMethodList() const {
  deque<string> retStrs;
  retStrs.push_back(string("info"));
  retStrs.push_back(string("keyFrameAnim"));
  retStrs.push_back(string("label"));
  retStrs.push_back(string("trias_lod"));
  return retStrs;
}

vsnMethodObj* vsnData_ShapeLOD::addNewMethod(const std::string& mtype,
					     const std::string& mname) {
  string modType(mtype);
  vsnMethodObj* pMethod = NULL;
  string msgHdr
    = string("FdvStr[") + getName() + string("]: addNewMethod: ");

  // instance method
  if ( modType == string("info") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_ShapeLOD_info());
  } // end of info
  else if ( modType == string("keyFrameAnim") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_keyFrameAnim());
  } // end of keyFrameAnim
  else if ( modType == string("label") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_label());
  } // end of label
  else if ( modType == string("trias_lod") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_ShapeLOD_trias_lod());
  } // end of trias_lod
  else {
    ErrMsg(MsgERR, msgHdr + string("method ")
	   + modType + string(" not supported"));
    return NULL;
  }

  if ( ! pMethod ) {
    ErrMsg(MsgERR, msgHdr + string("can't create ")
	   + modType + string(" method"));
    return NULL;
  }

  if ( ! mname.empty() )
    pMethod->setName(mname);

  if ( ! addMethod(pMethod) ) {
    ErrMsg(MsgERR, msgHdr + string("can't regist ")
	   + modType + string(" method"));
    delete pMethod;
    return NULL;
  }

  return pMethod;
}

size_t vsnData_ShapeLOD::getNumLevels() const {
  if ( ! m_pShapes ) return 0;
  return m_pShapes->getNumChildren();
}

vsnData_ShapeLOD::ShapeType
vsnData_ShapeLOD::getShapeType(const int level) const {
  size_t n = m_lodShapeTypes.size();
  if ( level < 0 || level >= n ) return ShapeNONE;
  return m_lodShapeTypes[level];
}

vfrNode* vsnData_ShapeLOD::getShape(const int level) {
  if ( ! m_pShapes ) return NULL;
  return m_pShapes->getChild(level);
}

bool vsnData_ShapeLOD::parseXML_LodFiles(xmlNodePtr xnp, std::string& baseDir,
					 std::deque<std::string>& lodFiles)
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
      bdir = (const char*)cvtPath.c_str();
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
      string lodpath;
      if ( pApp->needPathEncode() ) {
        wxString cvtPath = wxString::FromUTF8((const char*)xs);
        lodpath = (const char*)cvtPath.c_str();
      } else
        lodpath = (const char*)xs; 
      if ( ! vsnPath_isAbsolute(lodpath) )
        lodpath = vsnPath_concat(baseDir, lodpath);
      lodpath = vsnPath_normalize(lodpath);
      lodFiles.push_back(lodpath);
    } // end of 'seq'

  _NEXT_XML_NODE:
    cur = cur->next;
  } // end of while(cur)

  return true;
}


bool vsnData_ShapeLOD::init(const string& path, xmlNodePtr xnp) {
  static xmlChar* xs;
  deque<string> path_lst;
  string baseDir;

  m_ready = false;
  m_path = string("");

  if ( BaseName(path, string(""), vsnPath_getDelimChar())
       != string(VSN::vsn_seqfile) ) {
    path_lst.push_back(path);
    if ( ! init(path_lst) ) return false;
    m_path = path;
    setXmlNode(xnp);
    return true;
  }

  //---- lod files ----
  vsnApp* pApp = vsnApp::GetApp();
  if ( ! xnp || ! pApp ) return false;

  // is 'data' node?
  if ( strcmp((const char*)xnp->name, "data") ) return false;

  // 'base_dir' prop and 'seq' child node
  if ( ! parseXML_LodFiles(xnp, baseDir, path_lst) ) return false;

  // load shape files
  if ( ! init(path_lst) ) return false;

  // ok
  m_path = path;
  setXmlNode(xnp);
  return true;
}


bool vsnData_ShapeLOD::init(const std::deque<std::string>& path_lst) {
  string msgHdr
    = getDataType() + string("[") + getName() + string("]: init: ");

  if ( path_lst.size() < 1 ) return false;
  m_ready = false;
  m_lodPathes.clear();
  m_lodShapeTypes.clear();
  if ( ! m_pShapes ) {
    m_pShapes = new vfrGroup("LOD_Shapes", false);
    if ( ! m_pShapes ) {
      ErrMsg(MsgWARN, msgHdr + string("can't create LOD Group"));
      return false;
    }
  } else {
    m_pShapes->remAllChildren();
  }

  deque<string> pathList = path_lst;
  sort(pathList.begin(), pathList.end());

  //////// LOAD ////////
  deque<string>::reverse_iterator it;
  for ( it = pathList.rbegin(); it != pathList.rend(); it++ ) {
    string path = *it;
    if ( path.empty() ) continue;

    string path_body, path_type;
    char c;
    istringstream iss(path);
    while ( iss.get(c) && (c != ';') ) path_body.push_back(c);
    while ( iss.get(c) ) path_type.push_back(c);
    if ( path_body.empty() ) continue;

    ShapeType shapeType = ShapeNONE;
    if ( path_type == "OBJ" )
      shapeType = ShapeOBJ;
    else if ( path_type == "SLA" )
      shapeType = ShapeSLA;
    else if ( path_type == "SLB" )
      shapeType = ShapeSLB;
    else if ( path_type == "SLB_BE")
      shapeType = ShapeSLB_BE;

    if ( shapeType == ShapeNONE ) {
      // guess shapeType from suffix
      size_t pathlen = path_body.size();
      if ( pathlen < 3 ) continue;
      
      if ( ! path_body.compare(pathlen-3, 3, "stl") ||
	   ! path_body.compare(pathlen-3, 3, "STL") ) {
	// STL : STL Ascii or Binary
	if ( vfruStl::IsAscii(path_body.c_str()) )
	  shapeType = ShapeSLA;
	else
	  shapeType = ShapeSLB;
      } // end of 'stl'
      else if ( ! path_body.compare(pathlen-3, 3, "sla") ||
		! path_body.compare(pathlen-3, 3, "SLA") ) {
	// SLA : STL Ascii
	shapeType = ShapeSLA;
      } // end of 'sla'
      else if ( ! path_body.compare(pathlen-3, 3, "slb") ||
		! path_body.compare(pathlen-3, 3, "SLB") ) {
	// SLB : STL Binary
	shapeType = ShapeSLB;
      } // end of 'slb'
      else if ( ! path_body.compare(pathlen-3, 3, "obj") ||
		! path_body.compare(pathlen-3, 3, "OBJ") ) {
	// OBJ : Wavefront obj
	shapeType = ShapeOBJ;
      } // end of 'obj'
      else if ( ! path_body.compare(pathlen-3, 3, "wfo") ||
		! path_body.compare(pathlen-3, 3, "WFO") ) {
	// OBJ : Wavefront obj
	shapeType = ShapeOBJ;
      } // end of 'wfo'
      else {
	continue;
      }
    } // end of if(ShapeNONE)

    vfrNode* pNode = NULL;
    switch ( shapeType ) {
    case ShapeOBJ:
      pNode = vfruWfObj::ReadObj(path_body.c_str()); break;
    case ShapeSLA:
      pNode = vfruStl::ReadSla(path_body.c_str()); break;
    case ShapeSLB:
      pNode = vfruStl::ReadSlb(path_body.c_str()); break;
    case ShapeSLB_BE:
      pNode = vfruStl::ReadSlb(path_body.c_str(), true); break;
    } // end of switch(shapeType)
    if ( ! pNode ) {
      shapeType = ShapeNONE;
      continue;
    }

    m_lodPathes.push_back(path_body);
    m_lodShapeTypes.push_back(shapeType);
    m_pShapes->addChild(pNode);
  } // end of for(it)

  if ( m_lodPathes.size() < 1 ) {
    ErrMsg(MsgERR, msgHdr + string("no valid file exists"));
    return false;
  }
  if ( m_lodPathes.size() != pathList.size() ) {
    string badLst;
    deque<string>::const_iterator it0 = pathList.begin();
    deque<string>::const_iterator it1 = m_lodPathes.begin();
    for ( ; it0 != pathList.end(); it0++ ) {
      if ( it1 == m_lodPathes.end() || (*it1) != (*it0) ) {
        badLst += string("  + ")
          + BaseName(*it0, string(""), vsnPath_getDelimChar()) + "\n";
        continue;
      }
      it1++;
    } // end of for(it0)
    ErrMsg(MsgWARN, msgHdr + string("file list contains bad file(s)\n")
           + badLst);
  }

  (void)updateBbox();
  m_ready = true;
  return true;
}


// override vsnDataObj method

bool vsnData_ShapeLOD::updateBbox() {
  if ( ! m_pShapes ) return false;
  const Vec3<float>* sbb = m_pShapes->getBbox();
  if ( ! sbb ) return false;
  _bbox[0] = sbb[0];
  _bbox[1] = sbb[1];
  notice();
  return true;
}
