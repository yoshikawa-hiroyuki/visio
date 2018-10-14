//
// vsnData_Scatter
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
#include "wx/progdlg.h"

#include "vfruGetLine.h"
#include "vsnData_Scatter.h"
#include "vsnDataSeqFiles.h"
#include "vsnError.h"
#include "utilString.h"

#include "vsnMethod_Scatter_info.h"
#include "vsnMethod_Scatter_plotArrows.h"
#include "vsnMethod_Scatter_plotAsLines.h"
#include "vsnMethod_Scatter_plotAsLineStrip.h"
#include "vsnMethod_Scatter_plotAsTubeStrip.h"
#include "vsnMethod_Scatter_plotPoints.h"
#include "vsnMethod_keyFrameAnim.h"
#include "vsnMethod_label.h"
#include "vsnMethod_timeStep.h"
#include "vsnMethod_timeStepSync.h"

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;
using namespace CES;
using namespace VSN;


/* constructors / destructor */

vsnData_Scatter::vsnData_Scatter(const string& name)
  : vsnDataObj(name), m_scatterType(ScatterNONE), m_pData(NULL),
    m_numStps(0), m_maxNumPts(0)
{
  m_currentStepIdx --;
}

vsnData_Scatter::~vsnData_Scatter() {
  if ( m_pData )
    DeAllocate(m_pData);
}


/* methods */

deque<string> vsnData_Scatter::getSupportMethodList() const {
  deque<string> retStrs;
  retStrs.push_back(string("info"));
  retStrs.push_back(string("keyFrameAnim"));
  retStrs.push_back(string("label"));
  retStrs.push_back(string("plotArrows"));
  retStrs.push_back(string("plotAsLines"));
  retStrs.push_back(string("plotAsLines2"));
  retStrs.push_back(string("plotAsLineStrip"));
  retStrs.push_back(string("plotAsTubeStrip"));
  retStrs.push_back(string("plotPoints"));
  retStrs.push_back(string("timeStep"));
  retStrs.push_back(string("timeStepSync"));
  return retStrs;
}

vsnMethodObj* vsnData_Scatter::addNewMethod(const std::string& mtype,
					    const std::string& mname) {
  string modType(mtype);
  vsnMethodObj* pMethod = NULL;
  string msgHdr
    = string("FdvStr[") + getName() + string("]: addNewMethod: ");

  // instance method
  if ( modType == string("info") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_Scatter_info());
  } // end of info
  else if ( modType == string("keyFrameAnim") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_keyFrameAnim());
  } // end of keyFrameAnim
  else if ( modType == string("label") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_label());
  } // end of label
  else if ( modType == string("plotArrows") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_Scatter_plotArrows());
  } // end of plotArrows
  else if ( modType == string("plotAsLines") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_Scatter_plotAsLines());
  } // end of plotAsLines
  else if ( modType == string("plotAsLines2") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_Scatter_plotAsLines2());
  } // end of plotAsLines2
  else if ( modType == string("plotAsLineStrip") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_Scatter_plotAsLineStrip());
  } // end of plotAsLineStrip
  else if ( modType == string("plotAsTubeStrip") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_Scatter_plotAsTubeStrip());
  } // end of plotAsTubeStrip
  else if ( modType == string("plotPoints") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_Scatter_plotPoints());
  } // end of plotPoints
  else if ( modType == string("timeStep") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_timeStep());
  } // end of timeStep
  else if ( modType == string("timeStepSync") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_timeStepSync());
  } // end of timeStepSync
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


bool vsnData_Scatter::init(const string& path, xmlNodePtr xnp) {
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

std::deque<std::string> vsnData_Scatter::getSeqFilePathes() const {
  return m_seqPathes;
}

bool vsnData_Scatter::init(const std::deque<std::string>& path_lst) {
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

  // check minmax
  checkMinMax(true, vsnApp::GetApp()->isChkProgress());

  // load the first file
  if ( ! setCurrentStepIdx(0) )
    return false;

  m_ready = true;
  return true;
}

std::deque<int> vsnData_Scatter::getNvList(const size_t stpIdx) const {
  if ( m_numStps < 1 || m_stpList.empty() ) {
    return deque<int>();
  }
  if ( stpIdx >= m_numStps ) {
    return deque<int>(); //m_nvList[m_numStps -1];
  }
  return m_nvList[stpIdx];
}

std::deque<std::string>
vsnData_Scatter::setupLists(const std::deque<std::string>& path_lst) {
  deque<std::string> pathLst;

  register size_t numPath = path_lst.size();
  if ( numPath < 1 ) return pathLst;

  m_stpList.resize(numPath);
  m_nvList.resize(numPath);
  register size_t i, idx = 0;
  for ( i = 0; i < numPath; i++ ) {
    m_stpList[idx].step = idx;
    m_stpList[idx].time = (float)idx;
    m_nvList[idx].clear();

    string buff, path_body;
    ScatterType sType = checkType(path_lst[i], path_body);
    if ( sType != ScatterPWN && sType != ScatterSCAT ) {
      pathLst.push_back(path_lst[i]);
      idx++;
      continue;
    }

    // PWN|SCAT: check '#TS' and '#NV'
    ifstream sf(path_body.c_str());
    if ( ! sf ) continue;
    while ( ! sf.eof() ) {
      VFR::GetLine(sf, buff);
      if ( buff.size() < 1 ) continue;
      if ( buff[0] != '#' ) break;
      if ( buff.size() >= 4 && buff.substr(0, 3) == "#TS" ) {
	int st; float tm;
	if ( sscanf(buff.substr(3).c_str(), "%d %f", &st, &tm) != 2 ) continue;
	m_stpList[idx].step = st;
	m_stpList[idx].time = tm;
	continue;
      } else if (buff.size() >= 4 && buff.substr(0, 3) == "#NV" ) {
	vector<string> toks = SplitString(TrimString(buff.substr(3)));
	vector<string>::iterator tit;
	for ( tit = toks.begin(); tit != toks.end(); tit++ ) {
	  int nv = atoi(tit->c_str());
	  if ( nv > 0 )
	    m_nvList[idx].push_back(nv);
	} // end of for(tit)
      }
    } // end of while
    sf.close();

    if ( idx != 0 ) {
      if ( m_stpList[idx].step <= m_stpList[idx -1].step )
        m_stpList[idx].step = m_stpList[idx -1].step + 1;
    }

    pathLst.push_back(path_lst[i]);
    idx++;
  } // end of for(i)

  m_stpList.resize(idx);
  return pathLst;
}

bool vsnData_Scatter::adjustStepList(const std::deque<std::string>& path_lst,
				     const std::string& baseDir,
				     xmlNodePtr xnp)
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

bool vsnData_Scatter::getStepIdx(const int NSTEP, size_t& stp) const {
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

bool vsnData_Scatter::getTimeStepNo(const size_t stp, int& tsNo) const {
  if ( m_numStps < 1 || m_stpList.empty() ) return false;
  if ( stp >= m_numStps ) {
    tsNo = m_stpList[m_numStps -1].step;
    return false;
  }
  tsNo = m_stpList[stp].step;
  return true;
}

float vsnData_Scatter::getTime(const size_t stp) const {
  if ( m_numStps < 1 || m_stpList.empty() ) return 0.f;
  if ( stp >= m_numStps ) return m_stpList[m_numStps -1].time;
  return m_stpList[stp].time;
}

bool vsnData_Scatter::setCurrentStepIdx(const size_t stpIdx) {
  if ( stpIdx == m_currentStepIdx && m_ready ) return true;
  if ( ! vsnTimeSeriesDataIF::setCurrentStepIdx(stpIdx) )
    return false;

  string path = m_seqPathes[m_currentStepIdx];
  if ( path.empty() ) return false;

  string path_body;
  m_scatterType = checkType(path, path_body);
  if ( m_scatterType == ScatterNONE ) return false;

  // load data
  bool ret = false;;
  switch ( m_scatterType ) {
  case ScatterPWN:
    ret = readPWN(path_body);
    break;
  case ScatterSCAT:
    ret = readSCAT(path_body);
    break;
  } // end of switch(m_scatterType)
  if ( ! ret ) {
    m_scatterType = ScatterNONE;
    return false;
  }

  // update all TimeSeriesMethods
  ret = true;
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

vsnData_Scatter::ScatterType
vsnData_Scatter::checkType(const std::string& path,
			   std::string& path_body) const {
  if ( path.empty() ) return ScatterNONE;

  string path_type; char c;
  istringstream iss(path);
  path_body = "";
  while ( iss.get(c) && (c != ';') ) path_body.push_back(c);
  while ( iss.get(c) ) path_type.push_back(c);
  if ( path_body.empty() ) return ScatterNONE;

  if ( path_type == "PWN" )
    return ScatterPWN;
  else if ( path_type == "SCAT" )
    return ScatterSCAT;

  // guess scatterType from suffix
  size_t pathlen = path_body.size();
  if ( pathlen < 3 ) return ScatterNONE;

  if ( (path_body[pathlen-3]=='p' || path_body[pathlen-3]=='P') &&
       (path_body[pathlen-2]=='w' || path_body[pathlen-2]=='W') &&
       (path_body[pathlen-1]=='n' || path_body[pathlen-1]=='N') ) {
    // PWN : Points with Normal file
    return ScatterPWN;
  } // end of 'pwn'

  if ( (path_body[pathlen-3]=='s' || path_body[pathlen-3]=='S') &&
       (path_body[pathlen-2]=='c' || path_body[pathlen-2]=='C') &&
       (path_body[pathlen-1]=='t' || path_body[pathlen-1]=='T') ) {
    // SCT : Scatter file
    return ScatterSCAT;
  } // end of 'sct'

  if ( pathlen > 3 &&
       (path_body[pathlen-4]=='s' || path_body[pathlen-4]=='S') &&
       (path_body[pathlen-3]=='c' || path_body[pathlen-3]=='C') &&
       (path_body[pathlen-2]=='a' || path_body[pathlen-2]=='A') &&
       (path_body[pathlen-1]=='t' || path_body[pathlen-1]=='T') ) {
    // SCAT : Scatter file
    return ScatterSCAT;
  } // end of 'scat'

  return ScatterNONE;
}

bool vsnData_Scatter::readPWN(const std::string& path) {
  if ( path.empty() ) return false;
  register size_t i, idx;
  string buff;

  ifstream sf(path.c_str());
  if ( ! sf ) return false;

  // read header (np)
  size_t np = 0, nd = 3;
  while ( ! sf.eof() ) {
    VFR::GetLine(sf, buff);
    if ( buff.size() < 1 ) continue;
    if ( buff[0] == '#' ) continue;
    istringstream iss(buff);
    iss >> np;
    break;
  } // end of while(!sf.eof)
  if ( np < 1 )
    return false;

  // allocate
  if ( ! alcVerts(np) ) return false;   
  m_pData = (float*)ReAllocate(m_pData, sizeof(float)*np*nd);
  if ( ! m_pData ) return false;

  // read vertex
  for ( i = 0; i < np; ) {
    VFR::GetLine(sf, buff);
    if ( buff.size() < 1 ) continue;
    if ( buff[0] == '#' ) continue;

    // x, y, z
    istringstream iss(buff);
    iss >> _verts[i][0] >> _verts[i][1] >> _verts[i][2];
    i++;
  } // end of for(i)

  // read normal
  for ( idx = 0, i = 0; i < np; ) {
    VFR::GetLine(sf, buff);
    if ( buff.size() < 1 ) continue;
    if ( buff[0] == '#' ) continue;

    // normal vector
    istringstream iss(buff);
    iss >> m_pData[idx++];
    iss >> m_pData[idx++];
    iss >> m_pData[idx++];
    i++;
  } // end of for(i)
  m_dataLen = nd;

  generateBbox();
  notice();
  return true;
}

bool vsnData_Scatter::readSCAT(const std::string& path) {
  if ( path.empty() ) return false;
  register size_t i, j, idx;
  string buff;

  ifstream sf(path.c_str());
  if ( ! sf ) return false;

  // read header (np, nd)
  size_t np = 0, nd = 0;
  while ( ! sf.eof() ) {
    VFR::GetLine(sf, buff);
    if ( buff.size() < 1 ) continue;
    if ( buff[0] == '#' ) continue;
    istringstream iss(buff);
    iss >> np >> nd;
    break;
  } // end of while(!sf.eof)
  if ( np < 1 || nd < 1 )
    return false;

  // allocate
  if ( ! alcVerts(np) ) return false;   
  m_pData = (float*)ReAllocate(m_pData, sizeof(float)*np*nd);
  if ( ! m_pData ) return false;

  // read data
  for ( idx = 0, i = 0; i < np; ) {
    if ( sf.eof() ) break;
    VFR::GetLine(sf, buff);
    if ( buff.size() < 1 ) continue;
    if ( buff[0] == '#' ) continue;

    istringstream iss(buff);

    // x, y, z
    iss >> _verts[i][0] >> _verts[i][1] >> _verts[i][2];

    // data
    for ( j = 0; j < nd; j++ )
      iss >> m_pData[idx++];

    i++;
  } // end of for(i)
  if ( np != i ) alcVerts(i);
  m_dataLen = nd;

  generateBbox();
  notice();
  return true;
}

bool vsnData_Scatter::checkMinMax(const bool wholeStp, const bool progress) {
  register size_t i, j, k, idx;
  string errMsg = string("DataScatter[") + getName() + string("]: ");

  // load data of the first step
  if ( ! setCurrentStepIdx(0) ) {
    errMsg += string("can't read the first file:\n  ");
    errMsg += m_seqPathes[0];
    ErrMsg(MsgERR, errMsg);
    return false;
  }
  if ( m_hasMinMax && m_minVals.size() == m_dataLen )
    return true;

  // progressive dialog
  vsnViewFrame* pw = vsnApp::GetApp()->getViewFrame(0);
  wxProgressDialog* progDlg = NULL;
  char msgBuff[512];
  wxString prgMsg;
  if ( progress ) {
    sprintf(msgBuff, "Data_Scatter: checking min/max of file #1 of %lu",
	    m_numStps);
    prgMsg = vsnApp::ConvSysToWx(msgBuff);
    progDlg = new wxProgressDialog(wxT("check min/max value"), prgMsg,
				   m_numStps, pw,
				   wxPD_CAN_ABORT | wxPD_APP_MODAL |
				   wxPD_AUTO_HIDE | wxPD_SMOOTH);
    progDlg->Update(0, prgMsg);
  }

  m_minVals.resize(m_dataLen);
  m_maxVals.resize(m_dataLen);
  const float *pd = m_pData;
  for ( k = 0; k < m_dataLen; k++ ) {
    m_minVals[k] = m_maxVals[k] = pd[k];
  } // end of for(k)
  if ( m_dataLen >= 3 )
    m_maxVecLen012 = CES::Vec3<float>(pd).Length();
  m_maxNumPts = nVerts;
  for ( j = 0; j < nVerts; j++ ) {
    for ( k = 0; k < m_dataLen; k++ ) {
      idx = m_dataLen * j + k;
      if ( m_minVals[k] > pd[idx] ) m_minVals[k] = pd[idx];
      if ( m_maxVals[k] < pd[idx] ) m_maxVals[k] = pd[idx];
    } // end of for(k)
    if ( m_dataLen >= 3 ) {
      float vl = CES::Vec3<float>(&pd[m_dataLen * j]).Length();
      if ( m_maxVecLen012 < vl ) m_maxVecLen012 = vl;
    }
  } // end of for(j)
  if ( ! wholeStp ) {
    if ( progDlg ) progDlg->Destroy();
    return true;
  }

  // follow steps
  string path, path_body;
  bool badFileExists = false;
  errMsg += string("can't read the file(s) below:\n");
  for ( i = 1; i < m_numStps; i++ ) {
    sprintf(msgBuff, "Data_Scatter: checking min/max of file #%lu of %lu",
	    i+1, m_numStps);
    prgMsg = vsnApp::ConvSysToWx(msgBuff);
    if ( progDlg && ! progDlg->Update(i, prgMsg) ) break;

    path = m_seqPathes[i];
    ScatterType type = checkType(path, path_body);
    if ( type == ScatterNONE ) {
      char txt[64]; sprintf(txt, "  step#%zd ", i); errMsg += txt;
      errMsg += BaseName(path_body, string(""), vsnPath_getDelimChar());
      errMsg += string("\n");
      badFileExists = true;
      continue;
    }

    // load data
    bool ret = false;;
    switch ( type ) {
    case ScatterPWN:
      ret = readPWN(path_body);
      break;
    case ScatterSCAT:
      ret = readSCAT(path_body);
      break;
    } // end of switch(m_scatterType)
    if ( ! ret ) {
      char txt[64]; sprintf(txt, "  step#%zd ", i); errMsg += txt;
      errMsg += BaseName(path_body, string(""), vsnPath_getDelimChar());
      errMsg += string("\n");
      badFileExists = true;
      continue;
    }

    // check min/max
    pd = m_pData;
    if ( m_maxNumPts < nVerts ) m_maxNumPts = nVerts;
    for ( j = 0; j < nVerts; j++ ) {
      for ( k = 0; k < m_dataLen; k++ ) {
	idx = m_dataLen * j + k;
	if ( m_minVals[k] > pd[idx] ) m_minVals[k] = pd[idx];
	if ( m_maxVals[k] < pd[idx] ) m_maxVals[k] = pd[idx];
      } // end of for(k)
      if ( m_dataLen >= 3 ) {
	float vl = CES::Vec3<float>(&pd[m_dataLen * j]).Length();
	if ( m_maxVecLen012 < vl ) m_maxVecLen012 = vl;
      }
    } // end of for(j)    
  } // end of for(i)

  if ( badFileExists ) {
    ErrMsg(MsgWARN, errMsg);
  }

  if ( progDlg ) progDlg->Destroy();
  m_hasMinMax = true;
  return true;
}

bool vsnData_Scatter::getVectorMaxLen(const CES::Vec3<int>& vidx, float& vml) {
  if ( vsnNumericalDataIF::getVectorMaxLen(vidx, vml) )
    return true;

  register size_t i, j, idx;
  string path, path_body;

  if ( vidx.m_v[0] < 0 || vidx.m_v[0] >= m_dataLen ||
       vidx.m_v[1] < 0 || vidx.m_v[1] >= m_dataLen ||
       vidx.m_v[2] < 0 || vidx.m_v[2] >= m_dataLen )
    return false;

  // data of the first step
  size_t stpIdx = m_currentStepIdx;
  if ( ! setCurrentStepIdx(0) ) return false;
  const float *pd = m_pData;
  vml = CES::Vec3<float>(pd[vidx.m_v[0]],
                         pd[vidx.m_v[1]], pd[vidx.m_v[2]]).Length();
  for ( j = 0; j < nVerts; j++ ) {
    idx = m_dataLen * j;
    float vl = CES::Vec3<float>(pd[idx+vidx.m_v[0]], pd[idx+vidx.m_v[1]],
				pd[idx+vidx.m_v[2]]).Length();
    if ( vml < vl ) vml = vl;
  } // end of for(j)

  // follow steps
  for ( i = 1; i < m_numStps; i++ ) {
    path = m_seqPathes[i];
    ScatterType type = checkType(path, path_body);
    if ( type == ScatterNONE ) continue;

    // load data
    bool ret = false;;
    switch ( type ) {
    case ScatterPWN:
      ret = readPWN(path_body);
      break;
    case ScatterSCAT:
      ret = readSCAT(path_body);
      break;
    } // end of switch(m_scatterType)
    if ( ! ret ) continue;

    // check min/max
    pd = m_pData;
    for ( j = 0; j < nVerts; j++ ) {
      idx = m_dataLen * j;
      float vl = CES::Vec3<float>(pd[idx+vidx.m_v[0]], pd[idx+vidx.m_v[1]],
				  pd[idx+vidx.m_v[2]]).Length();
      if ( vml < vl ) vml = vl;
    } // end of for(j)    
  } // end of for(i)
  m_hasMinMax = true;

  return setCurrentStepIdx(stpIdx);  
}


// from vsnIoObject

bool vsnData_Scatter::outputXML(std::ostream& os, const size_t ts) {
  const char *scatterTypeStr[] = {"", ";PWN", ";SCT"};

  string bkupPath = m_path;
  m_path = m_path + scatterTypeStr[m_scatterType];
  bool ret = vsnDataObj::outputXML(os, ts);
  m_path = bkupPath;
  return ret;
}
