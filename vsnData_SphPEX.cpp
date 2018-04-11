//
// vsnData_SphPEX
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

#include "vsnData_SphPEX.h"
#include "vsnDataReload.h"
#include "vsnError.h"
#include "vsnMethod_SphPEX_paramSpace.h"
#include "vsnMethod_timeStep.h"

using namespace std;
using namespace CES;


/* constructors / destructor */

vsnData_SphPEX::vsnData_SphPEX(const std::string& name)
  : vsnData_Sv(name), m_currentParamIdx(0),
    m_paramName(VFR_NONAME), m_param0(0.f), m_paramSkip(0.f)
{
}

vsnData_SphPEX::~vsnData_SphPEX() {
}


/* methods */

bool vsnData_SphPEX::setCurrentParamIdx(const size_t idx) {
  //if ( idx == m_currentParamIdx ) return true;
  if ( idx >= m_pDataLst.size() || ! m_pDataLst[idx] ) return false;
  m_currentParamIdx = idx;

  register size_t i;
  m_dims = m_pDataLst[m_currentParamIdx]->getDims();
  m_dataLen = m_pDataLst[m_currentParamIdx]->getDataLen();

  m_minVals.clear(); m_maxVals.clear();
  float minmax[2];
  for ( i = 0; i < m_dataLen; i++ ) {
    m_pDataLst[m_currentParamIdx]->getMinMax(i, minmax);
    m_minVals.push_back(minmax[0]);
    m_maxVals.push_back(minmax[1]);
  } // end of for(i)
  m_pDataLst[m_currentParamIdx]->getVectorMaxLen(CES::Vec3<int>(0,1,2),
						 m_maxVecLen012);

  m_numStps = m_pDataLst[m_currentParamIdx]->getNumSteps();
  m_stpList.clear();
  for ( i = 0; i < m_numStps; i++ ) {
    StpUnit stpUnit;
    m_pDataLst[m_currentParamIdx]->getTimeStepNo(i, stpUnit.step);
    stpUnit.time = m_pDataLst[m_currentParamIdx]->getTime(i);
    m_stpList.push_back(stpUnit);
  } // end of for(i)
  if ( ! m_pDataLst[m_currentParamIdx]->setCurrentStepIdx(m_currentStepIdx) ) {
    m_pDataLst[m_currentParamIdx]->setCurrentStepIdx(0);
    m_currentStepIdx = 0;
  }

  const CES::Vec3<float>* xbb = m_pDataLst[m_currentParamIdx]->getBbox();
  _bbox[0] = xbb[0]; _bbox[1] = xbb[1];

  if ( m_pReloader ) m_pReloader->update();

  // update methods
  register size_t numMtd = getNumMethod();
  for ( i = 0; i < numMtd; i++ ) {
    vsnMethodObj* pMtd = getMethod(i);
    vsnMethod_timeStep* ptsMtd = dynamic_cast<vsnMethod_timeStep*>(pMtd);
    if ( ptsMtd ) continue;
    if ( pMtd && pMtd->update() )
      pMtd->updateUI();
  } // end of for(i)

  // update TimeSeriesData refers
  vsnTimeSeriesDataIF* ptsd = dynamic_cast<vsnTimeSeriesDataIF*>(this);
  if ( ptsd ) ptsd->updateTSDataRefers();

  return true;
}

bool vsnData_SphPEX::getParams(float params[2]) const {
  if ( m_pDataLst.size() < 1 ) return false;
  params[0] = m_param0; params[1] = m_paramSkip;
  return true;
}

bool vsnData_SphPEX::getCurrentParamValue(float& pval) const {
  if ( m_currentParamIdx >= m_pDataLst.size() ||
       ! m_pDataLst[m_currentParamIdx] ) return false;
  pval = m_param0 + m_paramSkip * m_currentParamIdx;
  return true;
}


bool vsnData_SphPEX::getOrig(CES::Vec3<float>& orig, const size_t stp) {
  if ( m_currentParamIdx >= m_pDataLst.size() ||
       ! m_pDataLst[m_currentParamIdx] ) return false;
  return m_pDataLst[m_currentParamIdx]->getOrig(orig, stp);
}

bool vsnData_SphPEX::getPitch(CES::Vec3<float>& pitch, const size_t stp) {
  if ( m_currentParamIdx >= m_pDataLst.size() ||
       ! m_pDataLst[m_currentParamIdx] ) return false;
  return m_pDataLst[m_currentParamIdx]->getPitch(pitch, stp);
}

bool vsnData_SphPEX::hasRectCoord() const {
  if ( m_currentParamIdx >= m_pDataLst.size() ||
       ! m_pDataLst[m_currentParamIdx] ) return false;
  return m_pDataLst[m_currentParamIdx]->hasRectCoord();
}

std::string vsnData_SphPEX::getRectCoordPath() const {
  if ( m_currentParamIdx >= m_pDataLst.size() ||
       ! m_pDataLst[m_currentParamIdx] ) return std::string("");
  return m_pDataLst[m_currentParamIdx]->getRectCoordPath();
}

float** vsnData_SphPEX::getRectCoord() {
  if ( m_currentParamIdx >= m_pDataLst.size() ||
       ! m_pDataLst[m_currentParamIdx] ) return NULL;
  return m_pDataLst[m_currentParamIdx]->getRectCoord();
}

size_t* vsnData_SphPEX::getRectCoordOffset() {
  if ( m_currentParamIdx >= m_pDataLst.size() ||
       ! m_pDataLst[m_currentParamIdx] ) return NULL;
  return m_pDataLst[m_currentParamIdx]->getRectCoordOffset();
}

bool vsnData_SphPEX::loadRectCoord(const std::string& path,
				   const size_t* ofst) {
  if ( m_currentParamIdx >= m_pDataLst.size() ||
       ! m_pDataLst[m_currentParamIdx] ) return false;
  return m_pDataLst[m_currentParamIdx]->loadRectCoord(path, ofst);
}


/* from vsnData_Sv */

bool vsnData_SphPEX::isUniformCoord() const {
  return !hasRectCoord();
}

const float* const vsnData_SphPEX::getCoord(const size_t stp) {
  if ( m_currentParamIdx >= m_pDataLst.size() ||
       ! m_pDataLst[m_currentParamIdx] ) return NULL;
  return m_pDataLst[m_currentParamIdx]->getCoord(stp);
}

const float* const vsnData_SphPEX::getData(const size_t stp) {
  if ( m_currentParamIdx >= m_pDataLst.size() ||
       ! m_pDataLst[m_currentParamIdx] ) return NULL;
  return m_pDataLst[m_currentParamIdx]->getData(stp);
}

std::deque<std::string> vsnData_SphPEX::getSeqFilePathes() const {
  if ( m_currentParamIdx >= m_pDataLst.size() ||
       ! m_pDataLst[m_currentParamIdx] ) {
    std::deque<std::string> ret; return ret;
  }
  return m_pDataLst[m_currentParamIdx]->getSeqFilePathes();
}

bool vsnData_SphPEX::updateBbox(const int stp) {
  if ( m_currentParamIdx >= m_pDataLst.size() ||
       ! m_pDataLst[m_currentParamIdx] ) return false;
  return m_pDataLst[m_currentParamIdx]->updateBbox(stp);
}

bool vsnData_SphPEX::getMinMax(const size_t n, float minmax[2]) const {
  if ( m_currentParamIdx >= m_pDataLst.size() ||
       ! m_pDataLst[m_currentParamIdx] ) return false;
  return m_pDataLst[m_currentParamIdx]->getMinMax(n, minmax);
}

bool vsnData_SphPEX::getVectorMaxLen(const CES::Vec3<int>& vidx, float& vml) {
  if ( m_currentParamIdx >= m_pDataLst.size() ||
       ! m_pDataLst[m_currentParamIdx] ) return false;
  return m_pDataLst[m_currentParamIdx]->getVectorMaxLen(vidx, vml);
}


/* from vsnDataObj */

std::string vsnData_SphPEX::getFilePath() const {
  if ( m_currentParamIdx >= m_pDataLst.size() ||
       ! m_pDataLst[m_currentParamIdx] ) return std::string("");
  return m_pDataLst[m_currentParamIdx]->getFilePath();
}

bool vsnData_SphPEX::init(const std::string& path, xmlNodePtr xnp) {
  // ignore path, check data list
  static xmlChar* xs;
  string msgHdr = "Data_SphPEX: init: ";

  vsnApp* pApp = vsnApp::GetApp();
  if ( ! xnp || ! pApp ) return false;

  // is 'data' node?
  if ( strcmp((const char*)xnp->name, "data") ) return false;
  m_pDataLst.clear();
  m_paramName = VFR_NONAME; m_param0 = m_paramSkip = 0.f;
  m_ready = false;

  // child node
  xmlNodePtr cur = xnp->xmlChildrenNode;
  while ( cur ) {
    if ( (cur = vsnIoObject::SkipCommentXML(cur)) == NULL ) break;
    if ( !strcmp((const char*)cur->name, "param_space") ) {
      // check param-name
      if ( xs ) xmlFree(xs);
      xs = xmlGetProp(cur, (const xmlChar*)"name");
      if ( xs && strlen((const char*)xs) > 0 )
	m_paramName = (const char*)xs;

      // check value
      if ( xs ) xmlFree(xs);
      xs = xmlGetProp(cur, (const xmlChar*)"value");
      string p0Str, p1Str; char c;
      istringstream iss((const char*)xs);
      while( iss.get(c) && c != ':' ) p0Str.push_back(c);
      while( iss.get(c) ) p1Str.push_back(c);
      if ( p0Str.empty() || p1Str.empty() ) {
	ErrMsg(MsgERR, msgHdr
	       + string("invalid format in param value"));
	goto _NEXT_XML_NODE;
      }
      m_param0 = (float)atof(p0Str.c_str());
      m_paramSkip = (float)atof(p1Str.c_str());
    } // end of 'param'
    else if ( !strcmp((const char*)cur->name, "data") ) {
      // check data-type
      if ( xs ) xmlFree(xs);
      xs = xmlGetProp(cur, (const xmlChar*)"type");
      if ( ! xs || strlen((const char*)xs) < 1 ) {
	ErrMsg(MsgERR, msgHdr
	       + string("invalid data: no type specified, ignore"));
	goto _NEXT_XML_NODE;
      }
      if ( strcmp((const char*)xs, "Sph") ) {
	ErrMsg(MsgERR, msgHdr
	       + string("invalid data: non Sph type specified, ignore"));
	goto _NEXT_XML_NODE;
      }
      vsnData_Sph* pdata = new vsnData_Sph();
      if ( ! pdata ) {
	ErrMsg(MsgERR, msgHdr + string("create Sph data failed"));
	break;
      }
      if ( ! pdata->parseXML(cur) ) {
	delete pdata;
	goto _NEXT_XML_NODE;
      }

      m_pDataLst.push_back(pdata);
    } // end of 'data'

  _NEXT_XML_NODE:
    cur = cur->next;
  } // end of while(cur)

  if ( m_pDataLst.size() < 1 ) {
    ErrMsg(MsgERR, msgHdr + string("no valid data of children"));
    return false;
  }

  if ( ! setCurrentParamIdx(0) ) return false;
  m_ready = true;

  // don't set XmlNode: to avoid reload
  //setXmlNode(xnp);
  return true;
}

std::deque<std::string> vsnData_SphPEX::getFileList() const {
  if ( m_currentParamIdx >= m_pDataLst.size() ||
       ! m_pDataLst[m_currentParamIdx] ) {
    std::deque<std::string> ret; return ret;
  }
  return m_pDataLst[m_currentParamIdx]->getFileList();
}

std::deque<std::string> vsnData_SphPEX::getSupportMethodList() const {
  std::deque<std::string> ret = vsnData_Sv::getSupportMethodList();
  ret.push_back(string("paramSpace"));
  return ret;
}

vsnMethodObj* vsnData_SphPEX::addNewMethod(const std::string& mtype,
					   const std::string& mname) {
  string modType(mtype);
  vsnMethodObj* pMethod = NULL;
  string msgHdr
    = string("DataSphPEX[") + getName() + string("]: addNewMethod: ");

  // instance method
  if ( modType == string("paramSpace") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_SphPEX_paramSpace());
  } // end of paramSpace

  if ( pMethod ) {
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

  return vsnData_Sv::addNewMethod(mtype, mname);
}


/* from vsnIoObject */

bool vsnData_SphPEX::outputXML(std::ostream& os, const size_t ts) {
  string idts;
  register int i;
  for ( i = 0; i < ts; i++ ) idts.push_back(' ');
  string idts_2 = idts; idts_2.push_back(' '); idts_2.push_back(' ');

  bool ret = true;
  string nameStr = getName();
  string filePath = vsnPath_normalize(getFilePath());

  // output
  os << idts << "<data type=\"" << getDataType() << "\"";
  if ( ! nameStr.empty() && nameStr != string(VFR_NONAME) )
    os << " name=\"" << nameStr << "\">" << endl;

  // output param_space
  if ( (!m_paramName.empty() && m_paramName != VFR_NONAME) ||
       m_param0 != 0.f || m_paramSkip != 0.f ) {
    os << idts_2 << "<param_space ";
    if ( !m_paramName.empty() && m_paramName != VFR_NONAME )
      os << "name=\"" << m_paramName << "\" ";
    if ( m_param0 != 0.f || m_paramSkip != 0.f )
      os << "value=\"" << m_param0 << ":" << m_paramSkip << "\" ";
    os << "/>" << endl;
  }

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

  // output datas
  for ( i = 0; i < m_pDataLst.size(); i++ ) {
    if ( ! m_pDataLst[i] ) continue;
    if ( ! m_pDataLst[i]->outputXML(os, ts+2) ) ret = false;
  } // end of for(i)

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
