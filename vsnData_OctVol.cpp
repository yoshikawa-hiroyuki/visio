//
// vsnData_OctVol
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

// need to include before vsnData (by reason of glew)
#include "vsnMethod_OctVol_shaderVolren.h"

#include "vsnData_OctVol.h"
#include "vsnDataSeqFiles.h"
#include "vsnError.h"

#include "vsnMethod_OctVol_graphPlot.h"
#include "vsnMethod_OctVol_histogram.h"
#include "vsnMethod_OctVol_info.h"
#include "vsnMethod_OctVol_isoCells.h"
#include "vsnMethod_OctVol_isosurf.h"
#include "vsnMethod_OctVol_leafSlice.h"
#include "vsnMethod_OctVol_minmaxGlyph.h"
#include "vsnMethod_OctVol_plotContour.h"
#include "vsnMethod_OctVol_plotScalar.h"
#include "vsnMethod_OctVol_plotVector.h"
#include "vsnMethod_OctVol_probe.h"
#include "vsnMethod_OctVol_streamLines.h"
#include "vsnMethod_OctVol_volren.h"
#include "vsnMethod_keyFrameAnim.h"
#include "vsnMethod_label.h"
#include "vsnMethod_sampler.h"
#include "vsnMethod_timeStep.h"
#include "vsnMethod_timeStepSync.h"

using namespace std;
using namespace CES;
using namespace VSN;


/* static members */

deque<string> vsnData_OctVol::s_methodList;


/* constructors / destructor */

vsnData_OctVol::vsnData_OctVol(const std::string& name)
  : vsnDataObj(name), m_otv_stamp(0), m_numStps(0)
{
  m_currentStepIdx --;
}

vsnData_OctVol::~vsnData_OctVol() {
}


/* methods */

bool vsnData_OctVol::getVectorMaxLen012(float& vml) const {
  if ( m_otv.m_dataLen < 3 ) return false;
  vml = m_maxVecLen012;
  return true;
}

bool vsnData_OctVol::checkMinMax(const bool wholeStp, const bool progress) {
  string errMsg = string("DataOctVol[") + getName() + string("]: ");
  if ( m_numStps < 1 ) return false;

  register int i, k;
  float minmax[2] = {0.f, 0.f};

  // progressive dialog
  vsnViewFrame* pw = vsnApp::GetApp()->getViewFrame(0);
  wxProgressDialog* progDlg = NULL;
  char msgBuff[512];
  wxString prgMsg;
  if ( progress ) {
    sprintf(msgBuff, "Data_Otv: checking min/max of file #1 of %lu", m_numStps);
    prgMsg = vsnApp::ConvSysToWx(msgBuff);
    progDlg = new wxProgressDialog(wxT("check min/max value"),
				   prgMsg, m_numStps, pw,
				   wxPD_CAN_ABORT | wxPD_APP_MODAL |
				   wxPD_AUTO_HIDE | wxPD_SMOOTH);
    progDlg->Update(0, prgMsg);
  }

  // data of the first step
  if ( wholeStp ) i = m_numStps -1;
  else i = 0;

  if ( ! setCurrentStepIdx(i) ) {
    if ( progDlg ) progDlg->Destroy();
    m_otv.Clear();
    errMsg += string("can't read the first file:\n  ");
    errMsg += BaseName(m_seqPathes[i], string(""), vsnPath_getDelimChar());
    ErrMsg(MsgERR, errMsg);
    return false;
  }

  m_minVals.resize(m_otv.m_dataLen);
  m_maxVals.resize(m_otv.m_dataLen);
  for ( k = 0; k < m_otv.m_dataLen; k++ ) {
    if ( m_otv.GetMinMax(k, minmax) ) {
      m_minVals[k] = minmax[0];
      m_maxVals[k] = minmax[1];
    }
  }
  if ( m_otv.m_dataLen >= 3 )
    getVectorMaxLen(CES::Vec3<int>(0, 1, 2), m_maxVecLen012);

  if ( ! wholeStp ) {
    if ( progDlg ) progDlg->Destroy();
    return true;
  }

  // follow steps
  bool badFileExists = false;
  errMsg += string("can't read the file(s) below:\n");
  for ( i--; i >= 0; i-- ) {
    sprintf(msgBuff, "Data_Otv: checking min/max of file #%lu of %lu",
	    m_numStps-i, m_numStps);
    prgMsg = vsnApp::ConvSysToWx(msgBuff);
    if ( progDlg && ! progDlg->Update(m_numStps-i-1, prgMsg) ) break;

    if ( ! setCurrentStepIdx(i) ) {
      m_otv.Clear();
      char txt[64]; sprintf(txt, "  step#%d ", i);
      errMsg += txt;
      errMsg += BaseName(m_seqPathes[i], string(""), vsnPath_getDelimChar());
      badFileExists = true;
      continue;
    }

    for ( k = 0; k < m_otv.m_dataLen; k++ ) {
      if ( m_otv.GetMinMax(k, minmax) ) {
	if ( m_minVals[k] > minmax[0] ) m_minVals[k] = minmax[0];
	if ( m_maxVals[k] < minmax[1] ) m_maxVals[k] = minmax[1];
      }
    } // end of for(k)
    if ( m_otv.m_dataLen >= 3 ) {
      float vl = 0.f; getVectorMaxLen(CES::Vec3<int>(0, 1, 2), vl);
      if ( m_maxVecLen012 < vl ) m_maxVecLen012 = vl;
    }
  } // end of for(i)

  if ( badFileExists ) {
    ErrMsg(MsgWARN, errMsg);
  }

  if ( progDlg ) progDlg->Destroy();
  return true;
}

std::deque<std::string> vsnData_OctVol::getSeqFilePathes() const {
  return m_seqPathes;
}

// MHIR append begin
#include <wx/regex.h>
#include <stdexcept>

bool 
vsnData_OctVol::init(const std::vector<VSN::ParallelFileInfo>& para_path_lst)
{
  bool result = true;

  string msgHdr = getDataType() + string("[") + getName() + string("]: init: ");

  if ( para_path_lst.size() < 1 ) return false;
  m_ready = false;
  m_numStps = 0;
  m_seqPathes.clear();

  deque<string> path_lst;
  for (size_t i = 0; i < para_path_lst.size(); ++i) {
    path_lst.push_back(para_path_lst[i].first);
  }

  // setup seqPathes
  deque<string> pathLst = setupLists(path_lst);
  if ( pathLst.size() < 1 ) {
    ErrMsg(MsgERR, msgHdr + string("no valid file exists"));
    return false;
  }
  if (pathLst.size() != path_lst.size()) {
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

  try {
    m_para_path_lst = para_path_lst;

    m_seqPathes.push_back(pathLst[0]);
    m_numStps = m_seqPathes.size();

    if (! vsnTimeSeriesDataIF::setCurrentStepIdx(0))
      throw std::runtime_error("vsnTimeSeriesDataIF::setCurrentStepIdx failed");

    m_otv_stamp = wxGetLocalTime();
    m_otv.Clear();

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

    float minmax[2] = {0.f, 0.f};
    m_minVals.resize(m_otv.m_dataLen);
    m_maxVals.resize(m_otv.m_dataLen);
    for (int k = 0; k < m_otv.m_dataLen; k++ ) {
      if ( m_otv.GetMinMax(k, minmax) ) {
	m_minVals[k] = minmax[0];
	m_maxVals[k] = minmax[1];
      }
    }
    if ( m_otv.m_dataLen >= 3 )
      getVectorMaxLen(CES::Vec3<int>(0, 1, 2), m_maxVecLen012);
  } catch (std::runtime_error& e) {
    m_otv.Clear();
    std::cout << e.what() << std::endl;
    return false;
  }

  // set bbox
  if ( ! updateBbox() )
    return false;

  m_ready = true;

  return result;
}
// MHIR append end


bool vsnData_OctVol::init(const std::deque<std::string>& path_lst) {
  string msgHdr
    = getDataType() + string("[") + getName() + string("]: init: ");
  // MHIR append begin
  m_para_path_lst.clear();
  // MHIR append end

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

  // set min/max
  vsnApp* pApp = vsnApp::GetApp();
  if ( ! checkMinMax(true, pApp->isChkProgress()) )
    return false;

  // set bbox
  if ( ! updateBbox() )
    return false;

  m_ready = true;
  return true;
}

std::deque<std::string>
vsnData_OctVol::setupLists(const std::deque<std::string>& path_lst) {
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

bool vsnData_OctVol::adjustStepList(const std::deque<std::string>& path_lst,
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

bool vsnData_OctVol::getMinMax(const size_t didx, float minmax[2]) const {
  return m_otv.GetMinMax(didx, minmax);
}

bool vsnData_OctVol::getVectorMaxLen(const CES::Vec3<int>& vidx, float& vml) {
  if ( vidx[0] < 0 || vidx[0] >= m_otv.m_dataLen ) return false;
  if ( vidx[1] < 0 || vidx[1] >= m_otv.m_dataLen ) return false;
  if ( vidx[2] < 0 || vidx[2] >= m_otv.m_dataLen ) return false;
  if ( m_otv.m_pLeafLst.empty() ) return false;

  register float vl;
  set<vsnOctTree::Node*>::const_iterator it = m_otv.m_pLeafLst.begin();
  if ( ! *it || ! (*it)->m_pData ) return false;
  CES::Vec3<float> vv((*it)->m_pData[vidx[0]],
		      (*it)->m_pData[vidx[1]], (*it)->m_pData[vidx[2]]);
  vml = vv.Length();
  for ( ; it != m_otv.m_pLeafLst.end(); it++ ) {
    if ( ! *it || ! (*it)->m_pData ) continue;
    vv = CES::Vec3<float>((*it)->m_pData[vidx[0]],
			  (*it)->m_pData[vidx[1]], (*it)->m_pData[vidx[2]]);
    vl = vv.Length();
    if ( vl > vml ) vml = vl;
  } // end of for(it)

  return true;
}

vsnOctTree::Node*
vsnData_OctVol::getNode(const CES::Vec3<float>& pos, const int level) {
  CES::Vec3<double> dpos(pos.m_v[0], pos.m_v[1], pos.m_v[2]);
  return m_otv.FindByPos(dpos, level);
}


/* methods from vsnDataObj */

string vsnData_OctVol::getFilePath() const {
  return m_path;
}

deque<string> vsnData_OctVol::getSupportMethodList() const {
  if ( s_methodList.empty() ) {
    s_methodList.push_back(string("graphPlot"));
    s_methodList.push_back(string("histogram"));
    s_methodList.push_back(string("info"));
    s_methodList.push_back(string("isoCells"));
    s_methodList.push_back(string("isosurf"));
    s_methodList.push_back(string("keyFrameAnim"));
    s_methodList.push_back(string("label"));
    s_methodList.push_back(string("leafSlice"));
    s_methodList.push_back(string("minmaxGlyph"));
    s_methodList.push_back(string("plotContour"));
    s_methodList.push_back(string("plotScalar"));
    s_methodList.push_back(string("plotVector"));
    s_methodList.push_back(string("probe"));
    s_methodList.push_back(string("sampler"));
    s_methodList.push_back(string("shaderVolren"));
    s_methodList.push_back(string("streamLines"));
    s_methodList.push_back(string("timeStep"));
    s_methodList.push_back(string("timeStepSync"));
    s_methodList.push_back(string("volren"));
  }
  return s_methodList;
}

bool vsnData_OctVol::init(const string& path, xmlNodePtr xnp) {
  static xmlChar* xs;
  deque<string> path_lst;
  string baseDir;

  m_ready = false;
  m_path = string("");
  m_otv_stamp = wxGetLocalTime();

  // MHIR modify begin
  //if ( BaseName(path, string(""), vsnPath_getDelimChar())
  //     != string(VSN::vsn_seqfile) ) {
  int flag = 0;
  std::string str = BaseName(path, string(""), vsnPath_getDelimChar());
  if (str == string(VSN::vsn_seqfile)) flag = 1;
  else if (str == string(VSN::vsn_parallelFiles)) flag = 2;
  if (flag == 0) {
    // MHIR modify end
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
  // MHIR modify begin
  if (flag == 1) {
    vsnDataSeqFiles dsf;
    if ( ! dsf.parseXML_SeqFiles(xnp, baseDir, path_lst) ) return false;

    // local initialize
    if ( ! init(path_lst) ) return false;
    (void)adjustStepList(path_lst, baseDir, xnp);
  } else if (flag == 2) {
    std::vector<ParallelFileInfo> parallel_path_lst;
    vsnDataParallelFiles dpf;
    if (! dpf.parseXML_ParallelFiles(xnp, baseDir, &parallel_path_lst))
      return false;

    if (! init(parallel_path_lst)) return false;
    (void)adjustStepList(path_lst, baseDir, xnp);
  }
  // MHIR modify end

  // ok
  m_path = path;
  setXmlNode(xnp);
  return true;
}

vsnMethodObj* vsnData_OctVol::addNewMethod(const string& mtype,
					   const string& mname) {
  string modType(mtype);
  vsnMethodObj* pMethod = NULL;
  string msgHdr
    = string("OctVol[") + getName() + string("]: addNewMethod: ");

  // instance method
  if ( modType == string("info") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_OctVol_info());
  } // end of info
  else if ( modType == string("isoCells") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_OctVol_isoCells());
  } // end of isoCells
  else if ( modType == string("isosurf") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_OctVol_isosurf());
  } // end of isosurf
  else if ( modType == string("graphPlot") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_OctVol_graphPlot());
  } // end of graphPlot
  else if ( modType == string("histogram") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_OctVol_histogram());
  } // end of histogram
  else if ( modType == string("keyFrameAnim") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_keyFrameAnim());
  } // end of keyFrameAnim
  else if ( modType == string("label") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_label());
  } // end of label
  else if ( modType == string("leafSlice") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_OctVol_leafSlice());
  } // end of leafSlice
  else if ( modType == string("plotScalar") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_OctVol_plotScalar());
  } // end of plotScalar
  else if ( modType == string("plotVector") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_OctVol_plotVector());
  } // end of plotVector
  else if ( modType == string("plotContour") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_OctVol_plotContour());
  } // end of plotContour
  else if ( modType == string("minmaxGlyph") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_OctVol_minmaxGlyph());
  } // end of minmaxGlyph
  else if ( modType == string("probe") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_OctVol_probe());
  } // end of probe
  else if ( modType == string("sampler") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_sliceSampler());
  } // end of sampler
  else if ( modType == string("shaderVolren") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_OctVol_shaderVolren());
  } // end of shaderVolren
  else if ( modType == string("streamLines") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_OctVol_streamLines());
  } // end of streamLines
  else if ( modType == string("timeStep") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_timeStep());
  } // end of timeStep
  else if ( modType == string("timeStepSync") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_timeStepSync());
  } // end of timeStepSync
  else if ( modType == string("volren") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_OctVol_volren());
  } // end of volren
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


/* from vsnTimeSeriesDataIF */

bool vsnData_OctVol::getStepIdx(const int NSTEP, size_t& stp) const {
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

bool vsnData_OctVol::getTimeStepNo(const size_t stp, int& tsNo) const {
  if ( m_numStps < 1 || m_stpList.empty() ) return false;
  if ( stp >= m_numStps ) {
    tsNo = m_stpList[m_numStps -1].step;
    return false;
  }
  tsNo = m_stpList[stp].step;
  return true;
}

float vsnData_OctVol::getTime(const size_t stp) const {
  if ( m_numStps < 1 || m_stpList.empty() ) return 0.f;
  if ( stp >= m_numStps ) return m_stpList[m_numStps -1].time;
  return m_stpList[stp].time;
}

bool vsnData_OctVol::setCurrentStepIdx(const size_t stpIdx) {
  if ( stpIdx == m_currentStepIdx ) return true;
  if ( ! vsnTimeSeriesDataIF::setCurrentStepIdx(stpIdx) )
    return false;

  // load oct-tree data
  m_otv_stamp = wxGetLocalTime();
  if ( ! m_otv.ImportFile(m_seqPathes[m_currentStepIdx].c_str()) ) {
    m_otv.Clear();
    return false;
  }
  m_dataLen = m_otv.m_dataLen;

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

  return ret;
}

bool vsnData_OctVol::updateBbox() {
  _bbox[0] = m_otv.m_bbox[0];
  _bbox[1] = m_otv.m_bbox[1];
  notice();
  return true;
}
