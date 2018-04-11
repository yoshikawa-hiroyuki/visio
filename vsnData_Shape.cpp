//
// vsnData_Shape
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

#include "vsnData_Shape.h"
#include "vsnDataSeqFiles.h"
#include "vsnError.h"
#include "vfruWfObj.h"
#include "vfruStl.h"

#include "vsnMethod_Shape_info.h"
#include "vsnMethod_Shape_trias.h"
#include "vsnMethod_Shape_trias_slbExt.h"
#include "vsnMethod_keyFrameAnim.h"
#include "vsnMethod_label.h"
#include "vsnMethod_timeStep.h"
#include "vsnMethod_timeStepSync.h"
// MHIR append begin
#include "vsnMethod_Shape_scalarMap.h"
// MHIR append end

#include <sstream>

using namespace std;
using namespace CES;
using namespace VSN;


/* constructors / destructor */

vsnData_Shape::vsnData_Shape(const string& name)
  : vsnDataObj(name), m_shapeType(ShapeNONE), m_pShape(NULL), m_numStps(0)
{
  m_currentStepIdx --;
}

vsnData_Shape::~vsnData_Shape() {
  if ( m_pShape )
    delete m_pShape;
}


/* methods */

deque<string> vsnData_Shape::getSupportMethodList() const {
  deque<string> retStrs;
  retStrs.push_back(string("info"));
  retStrs.push_back(string("keyFrameAnim"));
  retStrs.push_back(string("label"));
  // MHIR append begin
  // added by yoh@FNS
  if ( m_shapeType == ShapeSLA ||
       m_shapeType == ShapeSLB || m_shapeType == ShapeSLB_BE )
    retStrs.push_back(string("scalarMap"));
  // MHIR append end
  retStrs.push_back(string("timeStep"));
  retStrs.push_back(string("timeStepSync"));
  retStrs.push_back(string("trias"));
  retStrs.push_back(string("trias2"));
  retStrs.push_back(string("trias_slbExt"));
  return retStrs;
}

vsnMethodObj* vsnData_Shape::addNewMethod(const std::string& mtype,
					  const std::string& mname) {
  string modType(mtype);
  vsnMethodObj* pMethod = NULL;
  string msgHdr
    = string("FdvStr[") + getName() + string("]: addNewMethod: ");

  // instance method
  if ( modType == string("info") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_Shape_info());
  } // end of info
  else if ( modType == string("keyFrameAnim") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_keyFrameAnim());
  } // end of keyFrameAnim
  else if ( modType == string("label") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_label());
  } // end of label
  // MHIR append begin
  else if (modType == string("scalarMap")) {
    // added by yoh@FNS
    if ( m_shapeType == ShapeSLA ||
	 m_shapeType == ShapeSLB || m_shapeType == ShapeSLB_BE )
      pMethod = dynamic_cast<vsnMethodObj*>(new vsnMethod_Shape_scalarMap());
  }
  // MHIR append end
  else if ( modType == string("timeStep") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_timeStep());
  } // end of timeStep
  else if ( modType == string("timeStepSync") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_timeStepSync());
  } // end of timeStepSync
  else if ( modType == string("trias") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_Shape_trias());
  } // end of trias
  else if ( modType == string("trias2") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_Shape_trias2());
  } // end of trias2
  else if ( modType == string("trias_slbExt") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_Shape_trias_slbExt());
  } // end of triasdata
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


bool vsnData_Shape::init(const string& path, xmlNodePtr xnp) {
  static xmlChar* xs;
  deque<string> path_lst;
  string baseDir;

  m_ready = false;
  m_path = string("");

  if ( BaseName(path, string(""), vsnPath_getDelimChar())
       != string(VSN::vsn_seqfile) ) {
    path_lst.push_back(path);
    if ( ! init(path_lst) ) return false;
    (void)adjustStepList(path_lst, baseDir, xnp);
    m_path = path;
    setXmlNode(xnp);
    return true;
  }

  //---- sequential files ----
  vsnApp* pApp = vsnApp::GetApp();
  if ( ! xnp || ! pApp ) return false;

  // is 'data' node?
  if ( strcmp((const char*)xnp->name, "data") ) return false;

  // 'base_dir' prop and 'seq' child node
  vsnDataSeqFiles dsf;
  if ( ! dsf.parseXML_SeqFiles(xnp, baseDir, path_lst) ) return false;

  // local initialize
  if ( ! init(path_lst) ) return false;
  (void)adjustStepList(path_lst, baseDir, xnp);

  // ok
  m_path = path;
  setXmlNode(xnp);
  return true;
}

std::deque<std::string> vsnData_Shape::getSeqFilePathes() const {
  return m_seqPathes;
}

bool vsnData_Shape::init(const std::deque<std::string>& path_lst) {
  string msgHdr
    = getDataType() + string("[") + getName() + string("]: init: ");

  if ( path_lst.size() < 1 ) return false;
  m_ready = false;
  m_numStps = 0;
  m_seqPathes.clear();

  // setup seqPathes
  deque<string> pathLst = setupLists(path_lst);
  if ( pathLst.size() < 1 ) {
    ErrMsg(MsgERR, msgHdr + string("no valid file exists"));
    return false;
  }
  if ( pathLst.size() != path_lst.size() ) {
    string badLst;
    deque<string>::const_iterator it0 = path_lst.begin();
    deque<string>::const_iterator it1 = pathLst.begin();
    for ( ; it0 != path_lst.end(); it0++ ) {
      if ( it1 == pathLst.end() || (*it1) != (*it0) ) {
        badLst += string("  + ")
          + BaseName(*it0, string(""), vsnPath_getDelimChar()) + "\n";
        continue;
      }
      it1++;
    } // end of for(it0)
    ErrMsg(MsgWARN, msgHdr + string("file list contains bad file(s)\n")
           + badLst);
  }
  m_seqPathes = pathLst;
  m_numStps = m_seqPathes.size();

  // load the first file
  if ( ! setCurrentStepIdx(0) )
    return false;

  m_ready = true;
  return true;
}

std::deque<std::string>
vsnData_Shape::setupLists(const std::deque<std::string>& path_lst) {
  register size_t numPath = path_lst.size();
  if ( numPath < 1 ) {
    std::deque<std::string> pathLst;
    return pathLst;
  }

  m_stpList.resize(numPath);
  register size_t i;
  for ( i = 0; i < numPath; i++ ) {
    m_stpList[i].step = i;
    m_stpList[i].time = (float)i;
  } // end of for(i)

  return path_lst;
}

bool vsnData_Shape::adjustStepList(const std::deque<std::string>& path_lst,
				   const std::string& baseDir, xmlNodePtr xnp)
{
  static xmlChar* xs;
  StpUnit stpUnit;
  bool hasStep, hasTime, hasChanged = false;
  register size_t i, npl;

  vsnApp* pApp = vsnApp::GetApp();
  if ( ! xnp || ! pApp ) return false;
  npl = path_lst.size();
  if ( npl < 1 ) return false;
  if ( npl != m_stpList.size() ) return false;

  // 'seq' child node
  xmlNodePtr cur = xnp->xmlChildrenNode;
  while ( cur ) {
    if ( (cur = vsnIoObject::SkipCommentXML(cur)) == NULL ) break;
    if ( !strcmp((const char*)cur->name, "seq") ) {
      hasStep = hasTime = false;
      if ( xs ) xmlFree(xs);
      xs = xmlGetProp(cur, (const xmlChar*)"step");
      if ( xs && strlen((const char*)xs) > 0 ) {
        stpUnit.step = atoi((const char*)xs);
        hasStep = true;
      }
      if ( xs ) xmlFree(xs);
      xs = xmlGetProp(cur, (const xmlChar*)"time");
      if ( xs && strlen((const char*)xs) > 0 ) {
        stpUnit.time = (float)atof((const char*)xs);
        hasTime = true;
      }
      if ( ! hasStep && ! hasTime )
        goto _NEXT_XML_NODE;

      if ( xs ) xmlFree(xs);
      xs = xmlGetProp(cur, (const xmlChar*)"file");
      if ( ! xs || strlen((const char*)xs) < 1 )
        goto _NEXT_XML_NODE;

      string seqpath;
      if ( pApp->needPathEncode() ) {
	wxString cvtPath = wxString::FromUTF8((const char*)xs);
	seqpath = vsnApp::ConvWxToSys(cvtPath);
      } else
	seqpath = (const char*)xs;
      if ( ! vsnPath_isAbsolute(seqpath) )
        seqpath = vsnPath_concat(baseDir, seqpath);
      seqpath = vsnPath_normalize(seqpath);

      for ( i = 0; i < npl; i++ )
        if ( seqpath == path_lst[i] ) break;
      if ( i >= npl )
        goto _NEXT_XML_NODE;

      if ( hasStep ) m_stpList[i].step = stpUnit.step;
      if ( hasTime ) m_stpList[i].time = stpUnit.time;
      hasChanged = true;
    } // end of 'seq'

  _NEXT_XML_NODE:
    cur = cur->next;
  } // end of while(cur)

  if ( hasChanged ) {
    for ( i = 1; i < npl; i++ ) {
      if ( m_stpList[i].step <= m_stpList[i -1].step )
        m_stpList[i].step = m_stpList[i -1].step + 1;
    } // end of for(i)
  }
  return true;
}


/* from vsnTimeSeriesDataIF */

bool vsnData_Shape::getStepIdx(const int NSTEP, size_t& stp) const {
  if ( m_numStps < 1 || m_stpList.empty() )
    return false;
  if ( m_numStps < 2 || NSTEP <= m_stpList[0].step ) {
    stp = 0;
    return true;
  }
  if ( NSTEP >= m_stpList[m_numStps -1].step ) {
    stp = m_numStps -1;
    return true;
  }

  // find step
  register int i;
  for ( i = 0; i < m_numStps -1; i++ ) {
    if ( m_stpList[i].step <= NSTEP && NSTEP < m_stpList[i+1].step )
      break;
  } // end of for(i)
  stp = (size_t)i;

  return true;
}

bool vsnData_Shape::getTimeStepNo(const size_t stp, int& tsNo) const {
  if ( m_numStps < 1 || m_stpList.empty() ) return false;
  if ( stp >= m_numStps ) {
    tsNo = m_stpList[m_numStps -1].step;
    return false;
  }
  tsNo = m_stpList[stp].step;
  return true;
}

float vsnData_Shape::getTime(const size_t stp) const {
  if ( m_numStps < 1 || m_stpList.empty() ) return 0.f;
  if ( stp >= m_numStps ) return m_stpList[m_numStps -1].time;
  return m_stpList[stp].time;
}

bool vsnData_Shape::setCurrentStepIdx(const size_t stpIdx) {
  if ( stpIdx == m_currentStepIdx && m_ready ) return true;
  if ( ! vsnTimeSeriesDataIF::setCurrentStepIdx(stpIdx) )
    return false;

  string path = m_seqPathes[m_currentStepIdx];
  if ( path.empty() ) return false;

  string path_body, path_type;
  char c;
  istringstream iss(path);
  while ( iss.get(c) && (c != ';') ) path_body.push_back(c);
  while ( iss.get(c) ) path_type.push_back(c);
  if ( path_body.empty() ) return false;

  m_shapeType = ShapeNONE;
  if ( path_type == "OBJ" )
    m_shapeType = ShapeOBJ;
  else if ( path_type == "SLA" )
    m_shapeType = ShapeSLA;
  else if ( path_type == "SLB" )
    m_shapeType = ShapeSLB;
  else if ( path_type == "SLB_BE")
    m_shapeType = ShapeSLB_BE;

  if ( m_shapeType == ShapeNONE ) {
    // guess shapeType from suffix
    size_t pathlen = path_body.size();
    if ( pathlen < 3 ) return false;

    if ( (path_body[pathlen-3]=='s' || path_body[pathlen-3]=='S') &&
	 (path_body[pathlen-2]=='t' || path_body[pathlen-2]=='T') &&
	 (path_body[pathlen-1]=='l' || path_body[pathlen-1]=='L') ) {
      // STL : STL Ascii or Binary
      if ( vfruStl::IsAscii(path_body.c_str()) )
	m_shapeType = ShapeSLA;
      else
	m_shapeType = ShapeSLB;
    } // end of 'stl'
    else if ( (path_body[pathlen-3]=='s' || path_body[pathlen-3]=='S') &&
	      (path_body[pathlen-2]=='l' || path_body[pathlen-2]=='L') &&
	      (path_body[pathlen-1]=='a' || path_body[pathlen-1]=='A') ) {
      // SLA : STL Ascii
      m_shapeType = ShapeSLA;
    } // end of 'sla'
    else if ( (path_body[pathlen-3]=='s' || path_body[pathlen-3]=='S') &&
	      (path_body[pathlen-2]=='l' || path_body[pathlen-2]=='L') &&
	      (path_body[pathlen-1]=='b' || path_body[pathlen-1]=='B') ) {
      // SLB : STL Binary
      m_shapeType = ShapeSLB;
    } // end of 'slb'
    else if ( (path_body[pathlen-3]=='o' || path_body[pathlen-3]=='O') &&
	      (path_body[pathlen-2]=='b' || path_body[pathlen-2]=='B') &&
	      (path_body[pathlen-1]=='j' || path_body[pathlen-1]=='J') ) {
      // OBJ : Wavefront obj
      m_shapeType = ShapeOBJ;
    } // end of 'obj'
    else {
      return false;
    }
  } // end of if(ShapeNONE)

  // load data
  if ( m_pShape ) {delete m_pShape; m_pShape = NULL;}
  switch ( m_shapeType ) {
  case vsnData_Shape::ShapeOBJ:
    m_pShape = vfruWfObj::ReadObj(path_body.c_str()); break;
  case vsnData_Shape::ShapeSLA:
    m_pShape = vfruStl::ReadSla(path_body.c_str()); break;
  case vsnData_Shape::ShapeSLB:
    m_pShape = vfruStl::ReadSlb(path_body.c_str()); break;
  case vsnData_Shape::ShapeSLB_BE:
    m_pShape = vfruStl::ReadSlb(path_body.c_str(), true); break;
  } // end of switch(m_shapeType)
  if ( ! m_pShape ) {
    m_shapeType = ShapeNONE;
    return false;
  }

  // remove private material of shape (if there)
  if ( m_pShape->getPrivateMaterial() )
    m_pShape->setPrivateMaterial(NULL);

  // update bbox
  (void)updateBbox();

  // update all TimeSeriesMethods
  bool ret = true;
  size_t numMtds = getNumMethod();
  register size_t i;
  for ( i = 0; i < numMtds; i++ ) {
    vsnTimeSeriesMethodIF* pMtdTS
      = dynamic_cast<vsnTimeSeriesMethodIF*>(getMethod(i));
    if ( ! pMtdTS ) continue;
    if ( ! pMtdTS->updateStep(m_currentStepIdx, /* force */ false) )
      ret = false;
  } // end of for(i)

  return true;
}


// from vsnIoObject

bool vsnData_Shape::outputXML(std::ostream& os, const size_t ts) {
  const char *shapeTypeStr[] = {"", ";OBJ", ";SLA", ";SLB", "SLB_BE"};

  string bkupPath = m_path;
  m_path = m_path + shapeTypeStr[m_shapeType];
  bool ret = vsnDataObj::outputXML(os, ts);
  m_path = bkupPath;
  return ret;
}


// override vsnDataObj method

bool vsnData_Shape::updateBbox() {
  if ( ! m_pShape ) return false;
  const Vec3<float>* sbb = m_pShape->getBbox();
  if ( ! sbb ) return false;
  _bbox[0] = sbb[0];
  _bbox[1] = sbb[1];
  notice();
  return true;
}
