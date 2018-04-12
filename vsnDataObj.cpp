//
// vsnDataObj
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
#include "wx/strconv.h"

#include "vsnDataObj.h"
#include "vsnDataReload.h"
#include "vsnApp.h"
#include "vsnError.h"
#include "utilPath.h"

// MHIR append begin
#include "vsnDataParallelFiles.h"
#include "vsnData_OctVol.h"
// MHIR append end
#include "vsnData_DfiSv.h"

#include <algorithm>
#include <cctype>
#include <sstream>

using namespace std;
using namespace CES;
using namespace VSN;


//----------------------------------------------------------------
// class vsnDataObj
//----------------------------------------------------------------

/* static members */

unsigned long vsnDataObj::s_dataSeq = 0;


/* constructors / destructor */

vsnDataObj::vsnDataObj(const std::string& name)
  : vfrGroup(name, FALSE), vsnIoObject(),
    m_pXmlNode(NULL), m_pReloader(NULL), m_show(true), m_ready(false)
{
  alcMaterial();
  setBboxWidth(1.f);
  setPickMode(PT_OBJECT|PT_BBOX);

  m_dataSeq = s_dataSeq++;
}

vsnDataObj::~vsnDataObj() {
  register int n = getNumChildren();
  while ( n > 0 ) {
    vfrNode* p = getChild(n - 1);
    if ( p ) delete p;
    n = getNumChildren();
  }

  setXmlNode(NULL);

  if ( m_pReloader ) {
    delete m_pReloader;
    m_pReloader = NULL;
  }
}


/* methods */

// XForm interface
void vsnDataObj::updateXForm() {
  setMatrix(getXFormMatrix());
  chkNotice();
}


// MethodObj operation interface

bool vsnDataObj::reload() {
  if ( ! m_ready ) return false;
  if ( ! m_pXmlNode ) return false;

  // reset minmax range
  vsnNumericalDataIF* pnd = dynamic_cast<vsnNumericalDataIF*>(this);
  if ( pnd ) pnd->m_hasMinMax = false;

  // reload by current context
  if ( ! init(getFilePath(), m_pXmlNode) ) return false;

  // update methods
  register size_t i, numMdt = getNumMethod();
  for ( i = 0; i < numMdt; i++ ) {
    vsnMethodObj* pMtd = getMethod(i);
    if ( pMtd ) pMtd->reloaded();
  } // end of for(i)

  // update TimeSeriesData refers
  vsnTimeSeriesDataIF* ptsd = dynamic_cast<vsnTimeSeriesDataIF*>(this);
  if ( ptsd ) ptsd->updateTSDataRefers();

  return true;
}

vsnMethodObj* vsnDataObj::addNewMethod(const std::string& mtype,
				       const std::string& mname) {
  return NULL;
}

bool vsnDataObj::addMethod(vsnMethodObj* pMtd) {
  if ( ! pMtd->isAcceptableData(this->getDataType()) )
    return false;
  if ( addChild(pMtd) ) {
    pMtd->setRefData(this);
    return true;
  }
  return false;
}

bool vsnDataObj::delMethod(vsnMethodObj* pMtd) {
  if ( remChild(pMtd) ) {
    pMtd->setRefData(NULL);
    return true;
  }
  return false;
}

size_t vsnDataObj::getNumMethod() const {
  return (size_t)getNumChildren();
}

vsnMethodObj* vsnDataObj::getMethod(const size_t n) {
  return dynamic_cast<vsnMethodObj*>(getChild((const int)n));
}

vsnMethodObj* vsnDataObj::getMethod(const std::string& name) {
  return dynamic_cast<vsnMethodObj*>(getNode(name));
}

vsnMethodObj* vsnDataObj::getNextMethod(vsnMethodObj* pm) {
  if ( ! pm ) return getMethod(0);

  register int i;
  for ( i = 0; i < getNumChildren(); i++ )
    if ( getMethod(i) == pm ) break;
  return getMethod(i+1);
}

bool vsnDataObj::adjustMethodOrder(vsnMethodObj* beforeMtd,
				   vsnMethodObj* afterMtd) {
  if ( ! beforeMtd || ! afterMtd ) return true;
  if ( beforeMtd == afterMtd ) return true;

  int ordBef = -1, ordAft = -1;
  for ( int i = 0; i < nChild; i++ ) {
    if ( beforeMtd == getMethod((size_t)i) )
      ordBef = i;
    if ( afterMtd == getMethod((size_t)i) )
      ordAft = i;
  } // end of for(i)
  if ( ordBef < 0 || ordAft < 0 ) return true;
  if ( ordBef < ordAft ) return true;

  // exchange order of the methods (swap order)
  vfrNode* xnode = _children[ordBef];
  _children[ordBef] = _children[ordAft];
  _children[ordAft] = xnode;
  return true;
}


// show mode interface

void vsnDataObj::setShow(const bool mode) {
  if ( m_show == mode ) return;
  m_show = mode;
  _material->setRenderMode(m_show ? VFR::RT_SMOOTH : VFR::RT_NONE);
}


// scene interface

vsnScene* vsnDataObj::getAncestorScene() const {
  register size_t k;
  vsnApp* papp = vsnApp::GetApp(); if ( ! papp ) return NULL;
  size_t numScn = papp->getNumScene();
  for ( k = 0; k < numScn; k++ ) {
    vsnScene* psc = papp->getScene(k); if ( ! psc ) continue;
    if ( psc->getNode(this->getID()) ) return psc;
  } // end of for(k)
  return NULL;
}


// reloader interface

vsnDataReload* vsnDataObj::getReloader(const bool alc) {
  if ( ! m_pReloader ) {
    if ( ! alc ) return NULL;
    m_pReloader = new vsnDataReload(this);
  }
  return m_pReloader;
}


// from vfrGroup

void vsnDataObj::generateBbox() {
  if ( nChild < 1 ) return;
  Vec3<float> cbox[2];
  register int j;

  for ( j = 0; j < nChild; j++ ) {
    vsnMethodObj* pmtd = dynamic_cast<vsnMethodObj*>(_children[j]);
    if ( ! pmtd ) continue;
    if ( ! pmtd->hasBbox() ) continue;
    pmtd->getMatrixBbox(cbox);
    _bbox[0] = cbox[0]; _bbox[1] = cbox[1];
    break;
  } // end of for(j)

  for ( ; j < nChild; j++ ) {
    vsnMethodObj* pmtd = dynamic_cast<vsnMethodObj*>(_children[j]);
    if ( ! pmtd ) continue;
    if ( ! pmtd->hasBbox() ) continue;
    pmtd->getMatrixBbox(cbox);

    if ( _bbox[0].m_v[0] > cbox[0].m_v[0] )
      _bbox[0].m_v[0] = cbox[0].m_v[0];
    if ( _bbox[1].m_v[0] < cbox[1].m_v[0] )
      _bbox[1].m_v[0] = cbox[1].m_v[0];

    if ( _bbox[0].m_v[1] > cbox[0].m_v[1] )
      _bbox[0].m_v[1] = cbox[0].m_v[1];
    if ( _bbox[1].m_v[1] < cbox[1].m_v[1] )
      _bbox[1].m_v[1] = cbox[1].m_v[1];

    if ( _bbox[0].m_v[2] > cbox[0].m_v[2] )
      _bbox[0].m_v[2] = cbox[0].m_v[2];
    if ( _bbox[1].m_v[2] < cbox[1].m_v[2] )
      _bbox[1].m_v[2] = cbox[1].m_v[2];
  } // end of for(j)

  checkBbox();
}


// from vsnIoObject

bool vsnDataObj::parseXML(xmlNodePtr xnp) {
  static xmlChar* xs;
  xmlNodePtr cur;

  if ( ! xnp ) return false;
  vsnApp* pApp = vsnApp::GetApp();
  if ( ! pApp ) return false;

  // is 'data' node?
  if ( strcmp((const char*)xnp->name, "data") ) return false;

  // type check
  if ( xs ) xmlFree(xs);
  xs = xmlGetProp(xnp, (const xmlChar*)"type");
  if ( ! xs || strlen((const char*)xs) < 1 ) return false;
  if ( string((const char*)xs) != getDataType() ) return false;

  // get file path
  if ( xs ) xmlFree(xs);
  xs = xmlGetProp(xnp, (const xmlChar*)"file");
  string fpath;
  if ( xs && strlen((const char*)xs) > 0 ) {
    if ( pApp->needPathEncode() ) {
      wxString cvtPath = wxString::FromUTF8((const char*)xs);
      fpath = vsnApp::ConvWxToSys(cvtPath);
    } else
      fpath = (const char*)xs;
    if ( fpath != vsn_seqfile && ! vsnPath_isAbsolute(fpath) ) {
      string baseDir = DirName(pApp->getCurrentFilename(),
			       vsnPath_getDelimChar());
      fpath = vsnPath_concat(baseDir, fpath);
    }
  }
  fpath = vsnPath_normalize(fpath);

  // get name
  if ( xs ) xmlFree(xs);
  xs = xmlGetProp(xnp, (const xmlChar*)"name");
  if (  xs && strlen((const char*)xs) > 0 ) {
    setName((const char*)xs);
  } else {
    char txtbuf[128];
    string wkstr;
    if ( fpath.empty() ) {
      sprintf(txtbuf, "Data_%lu", getDataSeq());
    }
    else if ( fpath == vsn_seqfile ) {
      sprintf(txtbuf, "Data_%lu", getDataSeq());
    }
    // MHIR append begin
    else if ( fpath == vsn_parallelFiles ) {
      sprintf(txtbuf, "Data_%lu", getDataSeq());
    }
    // MHIR append end
    else {
      string xfpath, path_body, path_type; char c;
      istringstream iss(fpath);
      while ( iss.get(c) && (c != ';') ) path_body.push_back(c);
      while ( iss.get(c) ) path_type.push_back(c);
      if ( path_type == "OBJ" || path_type == "SLA" ||
	   path_type == "SLB" || path_type == "SLB_BE" ||
	   path_type == "PWN" || path_type == "SCT" )
	xfpath = path_body;
      else
	xfpath = fpath;
#if 0
      string xsfx = string(".") + getDataType();
      if ( ! xsfx.empty() ) { // tolower
	std::transform(xsfx.begin(), xsfx.end(), xsfx.begin(),
		       (int(*)(int))std::tolower);
      }
#endif
      wkstr = CES::BaseName(xfpath, string(""), vsnPath_getDelimChar());
      if ( ! wkstr.empty() )
	sprintf(txtbuf, "%s_%lu", wkstr.c_str(), getDataSeq());
      else
	sprintf(txtbuf, "Data_%lu", getDataSeq());
    }
    vsnScene* pscn = getAncestorScene();
    if ( pscn && pscn->getNode(txtbuf) ) {
      wkstr = txtbuf;
      register size_t xt;
      for ( xt = 1; ; xt++ ) {
	sprintf(txtbuf, "%s-%lu", wkstr.c_str(), xt);
	if ( ! pscn->getNode(txtbuf) ) break;
      } // end of for(xt)
    }
    setName(txtbuf);
  }
  string msgHdr
    = string("DataObj[") + getName() + string("]: parseXML: ");


  //========== FIRST PATH: traverse range ==========
  vsnNumericalDataIF* pnd = dynamic_cast<vsnNumericalDataIF*>(this);
  if ( pnd ) {
    // get children node
    cur = xnp->xmlChildrenNode;

    // traverse children
    while ( cur ) {
      if ( (cur = vsnIoObject::SkipCommentXML(cur)) == NULL ) break;

      //-------- "range" --------
      if ( !strcmp((const char*)cur->name, "range") ) {
	if ( ! pnd->parseMinMaxXml(cur) ) {
	  ErrMsg(MsgERR, msgHdr + string("range node parse failed, ignore"));
	}
      } // end of "range"

      cur = cur->next;
    } // end of while(cur) 
  } // end of pnd

  //========== LOAD: initialize with fpath ==========
  if ( ! init(fpath, xnp) ) {
    if ( fpath.empty() )
      ErrMsg(MsgERR, msgHdr + string("file not specified"));
    else
      ErrMsg(MsgERR, msgHdr + string("data initialize failed"));
    return false;
  }

  //========== SECOND PATH ==========
  // get children node
  cur = xnp->xmlChildrenNode;

  // traverse children
  while ( cur ) {
    if ( (cur = vsnIoObject::SkipCommentXML(cur)) == NULL ) break;

    //-------- "xform" --------
    if ( !strcmp((const char*)cur->name, "xform") ) {
      if ( ! importXMLNode(cur) ) {
	ErrMsg(MsgERR, msgHdr + string("xform node parse failed, ignore"));
	goto _NEXT_XML_NODE;
      }
    } // end of "xform"

    //-------- "param" --------
    else if ( !strcmp((const char*)cur->name, "param") ) {
      string xsN, xsV;
      if ( xs ) xmlFree(xs);
      xs = xmlGetProp(cur, (const xmlChar*)"name");
      if ( ! xs || strlen((const char*)xs) < 1 ) {
        ErrMsg(MsgERR, msgHdr + string("param node without name, ignore"));
        goto _NEXT_XML_NODE;
      }
      xsN = string((const char*)xs);
      if ( xs ) xmlFree(xs);
      xs = xmlGetProp(cur, (const xmlChar*)"value");
      if ( xs && strlen((const char*)xs) > 0 ) xsV = (const char*)xs;
      else xsV = "";

      if ( xsN == string("show") ) {
        bool showMode = (xsV == string("yes"));
        setShow(showMode);
      } // end of "show"
      else if ( xsN == string("bbox_show") ) {
	bool bbShowMode = (xsV == string("yes"));
	setBboxShowMode(bbShowMode);
      } // end of "bbox_show"
      else if ( xsN == string("reload_auto") ) {
	vsnDataReload* pReloader = getReloader();
	if ( ! pReloader ) {
	  ErrMsg(MsgERR, msgHdr
		 + string("param reload_auto: can't create reloader, ignore"));
	  goto _NEXT_XML_NODE;
	}
	bool autoMode = (xsV == string("yes"));
	pReloader->setAuto(autoMode);
	if ( autoMode ) pReloader->Start();
      } // end of "reload_auto"
      else if ( xsN == string("reload_interval") ) {
	vsnDataReload* pReloader = getReloader();
	if ( ! pReloader ) {
	  ErrMsg(MsgERR, msgHdr + string("param reload_interval: ")
		 + string("can't create reloader, ignore"));
	  goto _NEXT_XML_NODE;
	}
	float interval = (float)atof(xsV.c_str());
	if ( ! pReloader->setInterval(interval) ) {
	  ErrMsg(MsgERR, msgHdr + string("param reload_interval: ")
		 + string("can't set interval, ignore"));
	  goto _NEXT_XML_NODE;
	}
      } // end of "reload_interval"
      else if ( xsN == string("reload_pattern") ) {
	vsnDataReload* pReloader = getReloader();
	if ( ! pReloader ) {
	  ErrMsg(MsgERR, msgHdr + string("param reload_pattern: ")
		 + string("can't create reloader, ignore"));
	  goto _NEXT_XML_NODE;
	}
	if ( ! pReloader->setPattern(xsV) ) {
	  ErrMsg(MsgERR, msgHdr + string("param reload_pattern: ")
		 + string("can't set pattern, ignore"));
	  goto _NEXT_XML_NODE;
	}
      } // end of "reload_pattern"
    } // end of param

    //-------- "method" --------
    else if ( !strcmp((const char*)cur->name, "method") ) {
      if ( xs ) xmlFree(xs);
      xs = xmlGetProp(cur, (const xmlChar*)"type");
      if ( ! xs || strlen((const char*)xs) < 1 ) {
        ErrMsg(MsgERR, msgHdr + string("no-type method, ignore"));
        goto _NEXT_XML_NODE;
      }
      string modType((const char*)xs);
      vsnMethodObj* pMethod = addNewMethod(string((const char*)xs));
      if ( ! pMethod ) {
        goto _NEXT_XML_NODE;
      }
      if ( ! pMethod->parseXML(cur) ) {
        delete pMethod;
        goto _NEXT_XML_NODE;
      }
    } // end of method

  _NEXT_XML_NODE:
    cur = cur->next;
  } // end of while(cur)

  return true;
}

bool vsnDataObj::outputXML(std::ostream& os, const size_t ts) {
  string idts;
  register int i;
  for ( i = 0; i < ts; i++ ) idts.push_back(' ');
  string idts_2 = idts; idts_2.push_back(' '); idts_2.push_back(' ');
  string idts_4 = idts_2; idts_4.push_back(' '); idts_4.push_back(' ');

  bool ret = true;
  string nameStr = getName();
  string filePath = vsnPath_normalize(getFilePath());

  // file sequence check
  string seqBaseDir;
  bool hasMF = hasMultiFiles();
  if ( hasMF ) {
    deque<string> mfl = getFileList();
    size_t nmf = mfl.size();
    if ( nmf < 1 )
      hasMF = false;
    else if ( nmf == 1 ) {
      filePath = vsnPath_normalize(mfl[0]);
      hasMF = false;
    } else {
      filePath = vsn_seqfile;
      seqBaseDir = DirName(vsnPath_normalize(mfl[0]), vsnPath_getDelimChar());
      for ( i = 1; i < nmf; i++ ) {
	if ( seqBaseDir !=
	     DirName(vsnPath_normalize(mfl[i]), vsnPath_getDelimChar()) ) {
	  seqBaseDir = "";
	  break;
	}
      } // end of for(i)
    }
  } // end of if(hasMF)

  //-------- output --------
  // MHIR append begin
  bool done = false;
  if ( vsnData_OctVol* oct = dynamic_cast<vsnData_OctVol*>(this) ) {
    const std::vector<VSN::ParallelFileInfo>& paraPathLst
      = oct -> getParaPathLst();
    if (! paraPathLst.empty()) {
      string baseDir = DirName(vsnPath_normalize(paraPathLst[0].first),
			       vsnPath_getDelimChar());
      os << idts << "<data type=\"" << getDataType() << "\"";
      if ( ! nameStr.empty() && nameStr != string(VFR_NONAME) )
        os << " name=\"" << nameStr << "\"";
      os << " file=\"" << VSN::vsn_parallelFiles << "\"";
      os << " base_dir=\"" << baseDir << "\"";
      os << ">" << std::endl;

      for (size_t i = 0; i < paraPathLst.size(); ++i) {
        const VSN::ParallelFileInfo& info = paraPathLst[i];
        std::string fileName = BaseName(info.first, "", vsnPath_getDelimChar());
        string mode = (info.second) ? "data" : "skel";
        os << idts_2 << "<parallel file=\"" << fileName <<"\"";
        os << " mode=\"" << mode << "\"" << " />" << std::endl;
      }

      done = true;
    }
  }
  // MHIR append end

  vsnData_DfiSv* pdfisv = dynamic_cast<vsnData_DfiSv*>(this);
  if ( ! done && pdfisv ) {
    os << idts << "<data type=\"DfiSv\"";
    if ( ! nameStr.empty() && nameStr != string(VFR_NONAME) )
      os << " name=\"" << nameStr << "\"";
    os << " file=\"_DfiSv_dummy_\">" << endl;

    vsnData_Dfi* pdo = pdfisv->getRefDataDfi();
    if ( pdo ) {
      const Vec3<size_t>* regIdx = pdfisv->getRegionIdx();
      os << idts_2 << "<brick dfi=\"" << pdo->getName() << "\">" << endl;
      os << idts_4 << "<head x=\"" << regIdx[0][0] << "\" y=\""
	 << regIdx[0][1] << "\" z=\"" << regIdx[0][2] << "\"/>" << endl;
      os << idts_4 << "<tail x=\"" << regIdx[1][0] << "\" y=\""
	 << regIdx[1][1] << "\" z=\"" << regIdx[1][2] << "\"/>" << endl;
      os << idts_2 << "</brick>" << endl;

      done = true;
    }
  }

  if ( ! done ) {
    os << idts << "<data type=\"" << getDataType() << "\"";
    if ( ! nameStr.empty() && nameStr != string(VFR_NONAME) )
      os << " name=\"" << nameStr << "\"";
    if ( ! filePath.empty() ) {
      string baseDir = DirName(vsnApp::GetApp()->getCurrentFilename(),
			       vsnPath_getDelimChar());
      if ( ! hasMF ) {
	// single file path
	string sub_path = filePath.substr(0, baseDir.size());
	if ( sub_path == baseDir )
	  filePath = filePath.substr(baseDir.size()+1);
	os << " file=\"" << filePath << "\"";
      }
      else {
	// multi(sequential) file pathes
	if ( ! seqBaseDir.empty() ) {
	  string sub_path = seqBaseDir.substr(0, baseDir.size());
	  if ( sub_path == baseDir ) {
	    if ( seqBaseDir != baseDir )
	      seqBaseDir = seqBaseDir.substr(baseDir.size()+1);
	    else
	      seqBaseDir = string(".");
	  }
	}
	os << " file=\"" << filePath << "\""; // filePath is vsn_seqfile
	if ( ! seqBaseDir.empty() && seqBaseDir != string(".") )
	  os << " base_dir=\"" << seqBaseDir << "\"";
      }
    }
    os << ">" << endl;

    // output seq_files
    if ( hasMF ) {
      deque<string> mfl = getFileList();
      size_t nmf = mfl.size();
      for ( i = 0; i < nmf; i++ ) {
	string seqFile = vsnPath_normalize(mfl[i]);
	if ( ! seqBaseDir.empty() )
	  seqFile = BaseName(seqFile, string(""), vsnPath_getDelimChar());
	if ( seqFile.empty() ) continue;
	os << idts_2 << "<seq file=\"" << seqFile << "\" />" << endl;
      } // end of for(i)
    }
  } // end of if(!done)

  // output xform
  if ( m_T.Length() > 1e-8 || m_C.Length() > 1e-8 || m_HPR.Length() > 1e-8 ||
       (m_S - Vec3<float>(1.f,1.f,1.f)).Length() > 1e-8 ) {
    if ( ! exportXMLNode(os, ts+2) )
      ret = false;
  }

  // output show param
  if ( ! getShow() ) {
    os << idts_2 << "<param name=\"show\" value=\"no\" />" << endl;
  }

  // output bbox_show param
  size_t numMethods = getNumMethod();
  if ( getBboxShowMode() && numMethods > 0 ) {
    os << idts_2 << "<param name=\"bbox_show\" value=\"yes\" />" << endl;
  }
  else if ( ! getBboxShowMode() && numMethods < 1 ) {
    os << idts_2 << "<param name=\"bbox_show\" value=\"no\" />" << endl;
  }

  // output reload params
  if ( m_pReloader ) {
    float rel_interval = m_pReloader->getInterval();
    if ( rel_interval != 600.f )
      os << idts_2 << "<param name=\"reload_interval\" value=\""
	 << rel_interval << "\" />" << endl;
    string rel_pat = m_pReloader->getPattern();
    if ( ! rel_pat.empty() )
      os << idts_2 << "<param name=\"reload_pattern\" value=\""
	 << rel_pat << "\" />" << endl;
    if ( m_pReloader->isAuto() )
      os << idts_2 << "<param name=\"reload_auto\" value=\"yes\" />" << endl;
  }

  // output range
  vsnNumericalDataIF* pnd = dynamic_cast<vsnNumericalDataIF*>(this);
  if ( pnd ) {
    pnd->outputMinMaxXml(os, ts+2);
  }

  // output methods
  for ( i = 0; i < numMethods; i++ ) {
    vsnMethodObj* pmtd = getMethod(i);
    if ( ! pmtd ) continue;
    if ( ! pmtd->outputXML(os, ts+2) ) {
      ret = false;
      break;
    }
  } // end of for(i)

  os << idts << "</data>" << endl;
  return ret;
}

bool vsnDataObj::commandXML(xmlNodePtr xnp) {
  static xmlChar* xs;
  string msgHdr
    = string("DataObj[") + getName() + string("]: commandXML: ");

  // is 'command' node?
  if ( ! xnp ) return false;
  if ( strcmp((const char*)xnp->name, "command") ) return false;

  // is my command?
  if ( xs ) xmlFree(xs);
  xs = xmlGetProp(xnp, (const xmlChar*)"target");
  if ( ! xs || strlen((const char*)xs) < 1 ) return false;
  if ( string((const char*)xs) != getName() ) return false;

  // get command name
  string nameStr, valueStr;
  if ( xs ) xmlFree(xs);
  xs = xmlGetProp(xnp, (const xmlChar*)"name");
  if ( ! xs || strlen((const char*)xs) < 1 ) {
    ErrMsg(MsgERR, msgHdr + "invalid command: no 'name' property");
    return false;
  }
  nameStr = string((const char*)xs);

  // get command value (if there)
  if ( xs ) xmlFree(xs);
  xs = xmlGetProp(xnp, (const xmlChar*)"value");
  if ( xs && strlen((const char*)xs) > 0 )
    valueStr = string((const char*)xs);

  // do the command
  if ( nameStr == "set_name" ) {
    if ( valueStr.empty() )
      setName(VFR_NONAME);
    else
      setName(valueStr);
  }
  else if ( nameStr == "set_show" ) {
    if ( valueStr == string("yes") )
      setShow(true);
    else if ( valueStr == string("no") )
      setShow(false);
    else {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_show: invalid value: ") + valueStr);
      return false;
    }
  }
  else if ( nameStr == "set_xform" ) {
    xmlNodePtr cur = xnp->xmlChildrenNode;
    while ( cur ) {
      if ( (cur = vsnIoObject::SkipCommentXML(cur)) == NULL ) break;
      if ( !strcmp((const char*)cur->name, "xform") ) {
	if ( ! importXMLNode(cur) )
	  ErrMsg(MsgERR, msgHdr +
		 string("command set_xform: xform node parse failed, ignore"));
	break;
      }
      cur = cur->next;
    } // end of while(cur)
  }
  else if ( nameStr == "add_method" ) {
    if ( valueStr.empty() ) {
      ErrMsg(MsgERR, msgHdr + "command add_methd: no value");
      return false;
    }
    string methodType, methodName; char c;
    istringstream iss(valueStr);
    while ( iss.get(c) && (c != ';') ) methodType.push_back(c);
    while ( iss.get(c) ) methodName.push_back(c);

    vsnMethodObj* pMtd = addNewMethod(methodType, methodName);
    if ( ! pMtd || ! pMtd->update() ) {
      ErrMsg(MsgERR, msgHdr + "command add_method: can't add method: " +
	     methodType + "[" + methodName + "]");
      if ( pMtd ) delete pMtd;
      return false;
    }
  }
  else if ( nameStr == "del_method" ) {
    if ( valueStr.empty() || valueStr == VFR_NONAME ) {
      ErrMsg(MsgERR, msgHdr + "command del_method: no value");
      return false;
    }
    vsnMethodObj* pMtd = getMethod(valueStr);
    if ( ! pMtd ) {
      ErrMsg(MsgERR, msgHdr +
	     string("command del_methd: can't find method: ") + valueStr);
      return false;
    }
    if ( ! delMethod(pMtd) ) {
      ErrMsg(MsgERR, msgHdr +
	     "command del_methd: can't delete method: " + valueStr);
      return false;
    }
    delete pMtd;
  }
  else if ( nameStr == "set_reload_auto" ) {
    vsnDataReload* pReloader = getReloader();
    if ( ! pReloader ) {
      ErrMsg(MsgERR, msgHdr + string("command set_reload_auto: ")
		 + string("can't create reloader"));
      return false;
    }
    if ( valueStr == string("yes") ) {
      pReloader->setAuto(true);
      pReloader->Start();
    }
    else if ( valueStr == string("no") )
      pReloader->setAuto(false);
    else {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_reload_auto: invalid value: ") + valueStr);
      return false;
    }
  }
  else if ( nameStr == "set_reload_interval" ) {
    vsnDataReload* pReloader = getReloader();
    if ( ! pReloader ) {
      ErrMsg(MsgERR, msgHdr + string("command set_reload_interval: ")
		 + string("can't create reloader"));
      return false;
    }
    float interval = (float)atof(valueStr.c_str());
    if ( ! pReloader->setInterval(interval) ) {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_reload_interval: can't set value: ")
	     + valueStr);
      return false;
    }
  }
  else if ( nameStr == "set_reload_pattern" ) {
    vsnDataReload* pReloader = getReloader();
    if ( ! pReloader ) {
      ErrMsg(MsgERR, msgHdr + string("command set_reload_pattern: ")
		 + string("can't create reloader"));
      return false;
    }
    if ( ! pReloader->setPattern(valueStr) ) {
      ErrMsg(MsgERR, msgHdr +
	     string("command set_reload_pattern: can't set value: ")
	     + valueStr);
      return false;
    }
  }
  else if ( nameStr == "reload" ) {
    if ( m_pReloader ) {
      if ( ! m_pReloader->checkFiles() ) {
 	ErrMsg(MsgERR, msgHdr + "command reload(using reloader) failed.");
	return false;
      }
    }
    else {
      if ( ! reload() ) {
	ErrMsg(MsgERR, msgHdr + "command reload failed.");
	return false;
      }
    }
  }
  else {
    // not 'base DataObj' command
    return false;
  }

  return true; 
}

void vsnDataObj::setXmlNode(xmlNodePtr xnp) {
  if ( m_pXmlNode == xnp ) return;
  if ( m_pXmlNode ) {
    xmlUnlinkNode(m_pXmlNode);
    xmlFreeNode(m_pXmlNode);
    m_pXmlNode = NULL;
  }
  if ( ! xnp ) return;
  m_pXmlNode = xmlCopyNode(xnp, 1);
}


//----------------------------------------------------------------
// class vsnTSDataRefer
//----------------------------------------------------------------

/* destructor */

vsnTSDataRefer::~vsnTSDataRefer() {
  if ( p_tsd ) p_tsd->removeTSDataRefer(this);
}


/* methods */

std::deque< std::pair<vsnTimeSeriesDataIF*, vsnScene*> >
vsnTSDataRefer::getTimeSeriesDataList(vsnDataObj* pdata) {
  deque< pair<vsnTimeSeriesDataIF*, vsnScene*> > retLst;
  if ( ! pdata ) return retLst;

  // create retLst
  register size_t j, k;
  vsnApp* papp = vsnApp::GetApp(); if ( ! papp ) return retLst;
  size_t numScn = papp->getNumScene();
  for ( k = 0; k < numScn; k++ ) {
    vsnScene* pscn = papp->getScene(k); if ( ! pscn ) continue;
    size_t numDO = pscn->getNumDataObj();
    for ( j = 0; j < numDO; j++ ) {
      vsnDataObj* pdo = pscn->getDataObj(j); if ( ! pdo ) continue;
      if ( pdo == pdata ) continue;
      vsnTimeSeriesDataIF* ptsd = dynamic_cast<vsnTimeSeriesDataIF*>(pdo);
      if ( ptsd ) retLst.push_back(make_pair(ptsd, pscn));
    } // end of for(j)
  } // end of for(k)
  
  return retLst;
}

void vsnTSDataRefer::setTimeSeriesData(vsnTimeSeriesDataIF* tsd) {
  if ( tsd == p_tsd ) return;
  if ( p_tsd ) p_tsd->removeTSDataRefer(this);
  p_tsd = tsd;
  if ( p_tsd ) p_tsd->appendTSDataRefer(this);

  noticeUpdate();
}

bool vsnTSDataRefer::setTimeSeriesData(vsnDataObj* pdata,
				       const std::string& tgtName) {
  if ( ! pdata ) return false;
  if ( tgtName.empty() ) return false;
  string dataName, scnName;
  istringstream iss(tgtName); char c;
  while ( iss.get(c) && (c != '@') ) dataName.push_back(c);
  while ( iss.get(c) ) scnName.push_back(c);
  if ( dataName.empty() || dataName == string(VFR_NONAME) )
    return false;

  deque< pair<vsnTimeSeriesDataIF*, vsnScene*> > tsdLst
    = getTimeSeriesDataList(pdata);
  register int i;
  for ( i = 0; i < tsdLst.size(); i++ ) {
    vsnDataObj* pdo = dynamic_cast<vsnDataObj*>(tsdLst[i].first);
    vsnScene* psc =dynamic_cast<vsnScene*>(tsdLst[i].second);
    if ( ! pdo || ! psc ) continue;
    if ( dataName == pdo->getName() && 
	 (scnName.empty() || scnName == psc->getName()) ) {
      this->vsnTSDataRefer::setTimeSeriesData(tsdLst[i].first);
      return (p_tsd != NULL);
    }
  } // end of for(i)
  return false;
}

void vsnTSDataRefer::noticeDie() {
  if ( p_tsd ) p_tsd = NULL;
  noticeUpdate();
}

/* XML util method */

bool
vsnTSDataRefer::exportXMLCommand(const string& myName,
				 std::ostream& os, const size_t ts) const
{
  if ( ! os.good() ) return false;
  string coms;
  for ( int i = 0; i < ts; i++ ) coms.push_back(' ');

  string msgHdr = string("[")
    + (myName.empty() ? string(VFR_NONAME) : myName)
    + string("]: exportXMLCommand:\n");
  if ( myName.empty() || myName == string(VFR_NONAME) ) {
    ErrMsg(MsgERR, msgHdr + string("name has not set,\n")
	   + string("so can't export 'set_ref_data' command."));
    return false;
  }

  if ( p_tsd ) {
    vsnDataObj* pdo = dynamic_cast<vsnDataObj*>(p_tsd);
    if ( ! pdo ) return false;
    string tsdNm = pdo->getName();
    if ( tsdNm.empty() || tsdNm == string(VFR_NONAME) ) {
      ErrMsg(MsgWARN, msgHdr
             + string("ref_data has set, but the data has no name,\n")
             + string("so don't output 'set_ref_data' command node"));
      return false;
    }

    register size_t k;
    vsnApp* papp = vsnApp::GetApp();
    size_t numScn = papp->getNumScene();
    for ( k = 0; k < numScn; k++ ) {
      vsnScene* pscn = papp->getScene(k); if ( ! pscn ) continue;
      if ( pscn->getNode(pdo->getID()) ) {
	tsdNm += string("@") + pscn->getName();
	break;
      }
    } // end of for(k)

    coms += string("<command target=\"") + myName + "\" ";
    coms += string("name=\"set_ref_data\" value=\"") + tsdNm + "\" />\n";
    vsnApp::GetApp()->registExtOutStr(coms);
  }
  return true;
}


//----------------------------------------------------------------
// class vsnTimeSeriesDataIF
//----------------------------------------------------------------

vsnTimeSeriesDataIF::~vsnTimeSeriesDataIF() {
  set<vsnTSDataRefer*>::iterator it;
  for ( it = m_tsdrLst.begin(); it != m_tsdrLst.end(); it++ ) {
    if ( *it ) (*it)->noticeDie();
  } // end of for(it)
}


bool vsnTimeSeriesDataIF::setCurrentStepIdx(const size_t stpIdx) {
  if ( stpIdx == m_currentStepIdx ) return true;
  if ( stpIdx >= getNumSteps() ) return false;

  m_currentStepIdx = stpIdx;
  updateTSDataRefers();
  return true;
}

/* TSDataRefer control methods */

void vsnTimeSeriesDataIF::appendTSDataRefer(vsnTSDataRefer* tsdr) {
  if ( tsdr ) m_tsdrLst.insert(tsdr);
}

void vsnTimeSeriesDataIF::removeTSDataRefer(vsnTSDataRefer* tsdr) {
  set<vsnTSDataRefer*>::iterator it = m_tsdrLst.find(tsdr);
  if ( it == m_tsdrLst.end() ) return;
  m_tsdrLst.erase(it);
}

void vsnTimeSeriesDataIF::updateTSDataRefers() {
  set<vsnTSDataRefer*>::iterator it;
  for ( it = m_tsdrLst.begin(); it != m_tsdrLst.end(); it++ ) {
    if ( *it ) (*it)->noticeUpdate();
  } // end of for(it)
}


//----------------------------------------------------------------
// class vsnNumericalDataIF
//----------------------------------------------------------------

vsnNumericalDataIF::vsnNumericalDataIF()
  : m_dataLen(0), m_minVecLen012(0.f), m_maxVecLen012(0.f),
    m_hasMinMax(false)
{
}

bool vsnNumericalDataIF::getMinMax(const size_t n, float minmax[2]) const {
  if ( n >= m_dataLen ) return false;
  minmax[0] = m_minVals[n];
  minmax[1] = m_maxVals[n];
  return true;
}

bool
vsnNumericalDataIF::getVectorMinLen(const CES::Vec3<int>& vidx, float& vml)
{
  if ( ! m_hasMinMax ) return false;
  if ( (vidx.m_v[0] == 0 && vidx.m_v[1] == 1 && vidx.m_v[2] == 2) ||
       (vidx.m_v[0] == 0 && vidx.m_v[1] == 2 && vidx.m_v[2] == 1) ||
       (vidx.m_v[0] == 1 && vidx.m_v[1] == 0 && vidx.m_v[2] == 2) ||
       (vidx.m_v[0] == 1 && vidx.m_v[1] == 2 && vidx.m_v[2] == 0) ||
       (vidx.m_v[0] == 2 && vidx.m_v[1] == 0 && vidx.m_v[2] == 1) ||
       (vidx.m_v[0] == 2 && vidx.m_v[1] == 1 && vidx.m_v[2] == 0) ) {
    if ( m_dataLen >= 3 ) {
      vml = m_minVecLen012;
      return true;
    }
  }
  return false;
}

bool
vsnNumericalDataIF::getVectorMaxLen(const CES::Vec3<int>& vidx, float& vml)
{
  if ( ! m_hasMinMax ) return false;
  if ( (vidx.m_v[0] == 0 && vidx.m_v[1] == 1 && vidx.m_v[2] == 2) ||
       (vidx.m_v[0] == 0 && vidx.m_v[1] == 2 && vidx.m_v[2] == 1) ||
       (vidx.m_v[0] == 1 && vidx.m_v[1] == 0 && vidx.m_v[2] == 2) ||
       (vidx.m_v[0] == 1 && vidx.m_v[1] == 2 && vidx.m_v[2] == 0) ||
       (vidx.m_v[0] == 2 && vidx.m_v[1] == 0 && vidx.m_v[2] == 1) ||
       (vidx.m_v[0] == 2 && vidx.m_v[1] == 1 && vidx.m_v[2] == 0) ) {
    if ( m_dataLen >= 3 ) {
      vml = m_maxVecLen012;
      return true;
    }
  }
  return false;
}

bool vsnNumericalDataIF::parseMinMaxXml(xmlNodePtr xnp) {
  static xmlChar* xs;
  xmlNodePtr cur;

  if ( ! xnp ) return false;
  vsnApp* pApp = vsnApp::GetApp();
  if ( ! pApp ) return false;

  // is 'range' node?
  if ( strcmp((const char*)xnp->name, "range") ) return false;

  m_hasMinMax = false;
  bool vec_range_read = false;

  // get children node
  cur = xnp->xmlChildrenNode;

  // traverse
  while ( cur ) {
    if ( cur->type == XML_COMMENT_NODE ) {
      cur = cur->next;
      continue;
    }
    
    //-------- "data_range" --------
    if ( !strcmp((const char*)cur->name, "data_range") ) {
      m_minVals.clear();
      m_maxVals.clear();

      float minVal, maxVal;
      xmlNodePtr dNode = cur->xmlChildrenNode;
      for ( ; dNode; dNode = dNode->next ) {
        if ( ! dNode || dNode->type != XML_TEXT_NODE )
          continue;
	istringstream iss((const char*)dNode->content);
	char buff[256];
	iss.getline(buff, 256);
	while ( ! iss.fail() ) {
	  if ( sscanf(buff, "%f %f", &minVal, &maxVal) == 2 ) {
	    m_minVals.push_back(minVal);
	    m_maxVals.push_back(maxVal);
	  }
	  iss.getline(buff, 256);
	} // end of while(iss)
      } // end of for(dNode)
    } // end of "data_range"

    //-------- "vec_range" --------
    else if ( !strcmp((const char*)cur->name, "vec_range") ) {
      m_minVecLen012 = m_maxVecLen012 = 0.0f;

      float minVal, maxVal;
      xmlNodePtr dNode = cur->xmlChildrenNode;
      for ( ; dNode; dNode = dNode->next ) {
        if ( ! dNode || dNode->type != XML_TEXT_NODE )
          continue;
	istringstream iss((const char*)dNode->content);
	char buff[256];
	iss.getline(buff, 256);
	while ( ! iss.fail() ) {
	  if ( sscanf(buff, "%f %f", &minVal, &maxVal) == 2 ) {
	    m_minVecLen012 = minVal;
	    m_maxVecLen012 = maxVal;
	    vec_range_read = true;
	  }
	  iss.getline(buff, 256);
	} // end of while(iss)
      } // end of for(dNode)
    } // end of "vec_range"

  _NEXT_XML_NODE:
    cur = cur->next;
  } // end of while(cur)

  size_t dlen = m_minVals.size();
  if ( dlen > 0 && m_maxVals.size() == dlen ) {
    if ( dlen >= 3 ) {
      if ( vec_range_read )
	m_hasMinMax = true;
    } else
      m_hasMinMax = true;
  }
  return true;
}

bool
vsnNumericalDataIF::outputMinMaxXml(std::ostream& os, const size_t ts) {
  if ( ! m_hasMinMax ) return false;

  std::string idts;
  for ( int i = 0; i < ts; i++ ) idts.push_back(' ');
  string idts_2 = idts; idts_2.push_back(' '); idts_2.push_back(' ');
  string idts_4 = idts_2; idts_4.push_back(' '); idts_4.push_back(' ');
  int i;

  os << idts << "<range>" << endl;

  os << idts_2 << "<data_range>" << endl;
  for ( i = 0; i < m_dataLen; i++ ) {
    os << idts_4 << m_minVals[i] << " " << m_maxVals[i] << endl;
  } // end of for(i)
  os << idts_2 << "</data_range>" << endl;

  if ( m_dataLen >= 3 ) {
    os << idts_2 << "<vec_range>" << endl;
    os << idts_4 << m_minVecLen012 << " " << m_maxVecLen012 << endl;
    os << idts_2 << "</vec_range>" << endl;
  }

  os << idts << "</range>" << endl;
  return true;
}
