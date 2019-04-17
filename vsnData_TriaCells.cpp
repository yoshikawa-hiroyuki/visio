//
// vsnData_TriaCells
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
#include "vsnData_TriaCells.h"
#include "vsnDataSeqFiles.h"
#include "vsnError.h"

#include "vsnMethod_TriaCells_info.h"
#include "vsnMethod_TriaCells_plotContour.h"
#include "vsnMethod_TriaCells_plotScalar.h"
#include "vsnMethod_TriaCells_plotVector.h"
#include "vsnMethod_TriaCells_trias.h"
#include "vsnMethod_keyFrameAnim.h"
#include "vsnMethod_label.h"
#include "vsnMethod_timeStep.h"
#include "vsnMethod_timeStepSync.h"

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;
using namespace CES;
using namespace VSN;


/* constructors / destructor */

vsnData_TriaCells::vsnData_TriaCells(const string& name)
  : vsnDataObj(name), m_triaCellsType(TriaCellsNONE), m_pData(NULL),
    m_numStps(0), m_maxNumPts(0), m_maxNumCells(0)
{
  m_currentStepIdx --;
}

vsnData_TriaCells::~vsnData_TriaCells() {
  if ( m_pData )
    DeAllocate(m_pData);
}


/* methods */

deque<string> vsnData_TriaCells::getSupportMethodList() const {
  deque<string> retStrs;
  retStrs.push_back(string("info"));
  retStrs.push_back(string("keyFrameAnim"));
  retStrs.push_back(string("label"));
  retStrs.push_back(string("plotContour"));
  retStrs.push_back(string("plotScalar"));
  retStrs.push_back(string("plotVector"));
  retStrs.push_back(string("trias"));
  retStrs.push_back(string("timeStep"));
  retStrs.push_back(string("timeStepSync"));
  return retStrs;
}

vsnMethodObj* vsnData_TriaCells::addNewMethod(const std::string& mtype,
					      const std::string& mname) {
  string modType(mtype);
  vsnMethodObj* pMethod = NULL;
  string msgHdr
    = string("FdvStr[") + getName() + string("]: addNewMethod: ");
  
  // instance method
  if ( modType == string("info") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_TriaCells_info());
  } // end of info
  else if ( modType == string("keyFrameAnim") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_keyFrameAnim());
  } // end of keyFrameAnim
  else if ( modType == string("label") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_label());
  } // end of label
  else if ( modType == string("plotContour") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_TriaCells_plotContour());
  } // end of plotContour
  else if ( modType == string("plotScalar") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_TriaCells_plotScalar());
  } // end of plotScalar
  else if ( modType == string("plotVector") ) {
    pMethod
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_TriaCells_plotVector());
  } // end of plotVector
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
      = dynamic_cast<vsnMethodObj*>(new vsnMethod_TriaCells_trias());
  } // end of trias
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


bool vsnData_TriaCells::init(const string& path, xmlNodePtr xnp) {
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

std::deque<std::string> vsnData_TriaCells::getSeqFilePathes() const {
  return m_seqPathes;
}

bool vsnData_TriaCells::init(const std::deque<std::string>& path_lst) {
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

std::deque<std::string>
vsnData_TriaCells::setupLists(const std::deque<std::string>& path_lst) {
  deque<std::string> pathLst;

  register size_t numPath = path_lst.size();
  if ( numPath < 1 ) return pathLst;

  m_stpList.resize(numPath);
  register size_t i, idx = 0;
  for ( i = 0; i < numPath; i++ ) {
    m_stpList[idx].step = idx;
    m_stpList[idx].time = (float)idx;

    string buff, path_body;
    TriaCellsType tcType = checkType(path_lst[i], path_body);
    if ( tcType != TriaCellsVTK ) {
      pathLst.push_back(path_lst[i]);
      idx++;
      continue;
    }

    pathLst.push_back(path_lst[i]);
    idx++;
  } // end of for(i)

  m_stpList.resize(idx);
  return pathLst;
}

bool vsnData_TriaCells::adjustStepList(const std::deque<std::string>& path_lst,
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

bool vsnData_TriaCells::getStepIdx(const int NSTEP, size_t& stp) const {
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

bool vsnData_TriaCells::getTimeStepNo(const size_t stp, int& tsNo) const {
  if ( m_numStps < 1 || m_stpList.empty() ) return false;
  if ( stp >= m_numStps ) {
    tsNo = m_stpList[m_numStps -1].step;
    return false;
  }
  tsNo = m_stpList[stp].step;
  return true;
}

float vsnData_TriaCells::getTime(const size_t stp) const {
  if ( m_numStps < 1 || m_stpList.empty() ) return 0.f;
  if ( stp >= m_numStps ) return m_stpList[m_numStps -1].time;
  return m_stpList[stp].time;
}

bool vsnData_TriaCells::setCurrentStepIdx(const size_t stpIdx) {
  if ( stpIdx == m_currentStepIdx && m_ready ) return true;
  if ( ! vsnTimeSeriesDataIF::setCurrentStepIdx(stpIdx) )
    return false;

  string path = m_seqPathes[m_currentStepIdx];
  if ( path.empty() ) return false;

  string path_body;
  m_triaCellsType = checkType(path, path_body);
  if ( m_triaCellsType == TriaCellsNONE ) return false;

  // load data
  bool ret = false;;
  switch ( m_triaCellsType ) {
  case TriaCellsVTK:
    ret = readVTK(path_body);
    break;
  } // end of switch(m_triaCellsType)
  if ( ! ret ) {
    m_triaCellsType = TriaCellsNONE;
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

vsnData_TriaCells::TriaCellsType
vsnData_TriaCells::checkType(const std::string& path,
			     std::string& path_body) const {
  if ( path.empty() ) return TriaCellsNONE;

  string path_type; char c;
  istringstream iss(path);
  path_body = "";
  while ( iss.get(c) && (c != ';') ) path_body.push_back(c);
  while ( iss.get(c) ) path_type.push_back(c);
  if ( path_body.empty() ) return TriaCellsNONE;

  if ( path_type == "VTK" )
    return TriaCellsVTK;

  // guess Type from suffix
  size_t pathlen = path_body.size();
  if ( pathlen < 3 ) return TriaCellsNONE;
  if ( (path_body[pathlen-3]=='v' || path_body[pathlen-3]=='V') &&
       (path_body[pathlen-2]=='t' || path_body[pathlen-2]=='T') &&
       (path_body[pathlen-1]=='k' || path_body[pathlen-1]=='K') ) {
    return TriaCellsVTK;
  } // end of 'vtk'

  return TriaCellsNONE;
}

bool vsnData_TriaCells::readVTK(const std::string& path) {
  if ( path.empty() ) return false;
  register size_t i, j, idx;
  string buff;

  ifstream sf(path.c_str());
  if ( ! sf ) return false;

  // read header (4 lines)
  // 1: # vtk ...
  VFR::GetLine(sf, buff);
  if ( buff.size() < 14 || buff.substr(0, 14) != string("# vtk DataFile") )
    return false;
  // 2: title string(skip)
  if ( sf.eof() ) return false;
  VFR::GetLine(sf, buff);
  // 3: ASCII|BINARY
  if ( sf.eof() ) return false;
  VFR::GetLine(sf, buff);
  if ( buff != string("ASCII") )
    return false; // BINARY not supported
  // 4: DATASET
  if ( sf.eof() ) return false;
  VFR::GetLine(sf, buff);
  if ( buff != string("DATASET UNSTRUCTURED_GRID") )
    return false;

  size_t np = 0, nc = 0;
  
  // read POINTS block
  if ( sf.eof() ) return false;
  VFR::GetLine(sf, buff);
  vector<string> pl = VFR::SplitString(buff, string(" \t\n\r"));
  if ( pl.size() < 3 || pl[0] != string("POINTS") ) return false;
  np = atoi(pl[1].c_str());
  //if ( np < 3 ) return false;
  if ( np < 3 ) {
    alcVerts(0);
    alcIndices(0);
    if ( m_pData ) {
      DeAllocate(m_pData);
      m_pData = NULL;
    }
    m_dataLen = 0;
    return true;
  }

  // allocate coordinates
  if ( ! alcVerts(np) ) return false;   

  // read vertex
  for ( i = 0; i < np; ) {
    if ( sf.eof() ) return false;
    VFR::GetLine(sf, buff);
    if ( buff.size() < 1 ) continue;

    // x, y, z
    istringstream iss(buff);
    iss >> _verts[i][0] >> _verts[i][1] >> _verts[i][2];
    i++;
  } // end of for(i)

  // read CELLS block
  if ( sf.eof() ) return false;
  VFR::GetLine(sf, buff);
  vector<string> cl = VFR::SplitString(buff, string(" \t\n\r"));
  if ( cl.size() < 3 ) return false;
  nc = atoi(cl[1].c_str());
  if ( nc < 1 ) return false;

  // allocate indices (nc * 3)
  if ( ! alcIndices(nc * 3) ) return false;

  // read cells
  for ( idx = 0, i = 0; i < nc; ) {
    if ( sf.eof() ) return false;
    VFR::GetLine(sf, buff);
    if ( buff.size() < 1 ) continue;
    i++;

    cl = VFR::SplitString(buff, string(" \t\n\r"));
    if ( cl.size() < 4 || atoi(cl[0].c_str()) != 3 ) continue;
    _indices[idx * 3    ] = atoi(cl[1].c_str());
    _indices[idx * 3 + 1] = atoi(cl[2].c_str());
    _indices[idx * 3 + 2] = atoi(cl[3].c_str());
    idx++;
  } // end of for(i)
  if ( idx != nc )
    alcIndices(idx * 3);

  // read CELL_TYPES block
  if ( sf.eof() ) return false;
  VFR::GetLine(sf, buff);
  cl = VFR::SplitString(buff, string(" \t\n\r"));
  if ( cl.size() < 2 ) return false;
  if ( nc != atoi(cl[1].c_str()) ) return false;

  // read skip nc tokens
  idx = 0;
  while ( ! sf.eof() ) {
    VFR::GetLine(sf, buff);
    if ( buff.size() < 1 ) continue;

    cl = VFR::SplitString(buff, string(" \t\n\r"));
    idx += cl.size();
    if ( idx >= nc ) break;
  } // end of while

  // read POINT_DATA block
  if ( sf.eof() ) return false;
  VFR::GetLine(sf, buff);
  pl = VFR::SplitString(buff, string(" \t\n\r"));
  if ( pl.size() < 2 || pl[0] != string("POINT_DATA") ) return false;
  if ( atoi(pl[1].c_str()) != np ) return false;

  // read SCALARS/VECTORS
  size_t nds, ndv;
  nds = ndv = 0;
  float *p_ds = NULL;
  float *p_dv = NULL;
  while ( ! sf.eof() ) {
    VFR::GetLine(sf, buff);
    if ( buff.size() < 1 ) continue;

    pl = VFR::SplitString(buff, string(" \t\n\r"));
    if ( pl.size() < 2 ) return false;

    // SCALARS
    if ( pl[0] == string("SCALARS") ) {
      if ( pl.size() > 3 )
	nds = atoi(pl[3].c_str());
      else
	nds = 1;
      if ( nds < 1 || nds > 4 ) return false;

      // allocate
      p_ds = (float*)Allocate(sizeof(float)*np*nds);
      if ( ! p_ds ) return false;

      // read LOOKUP_TABLE line
      string lut_name;
      while ( ! sf.eof() ) {
	VFR::GetLine(sf, buff);
	if ( buff.size() < 1 ) continue;
	pl = VFR::SplitString(buff, string(" \t\n\r"));
	if ( pl.size() < 2 ) return false;
	if ( pl[0] != string("LOOKUP_TABLE") ) return false;
	lut_name = pl[1];
	break;
      } // end of while      

      // read scalar data
      idx = 0;
      while ( ! sf.eof() ) {
	VFR::GetLine(sf, buff);
	if ( buff.size() < 1 ) continue;

	pl = VFR::SplitString(buff, string(" \t\n\r"));
	for ( i = 0; i < pl.size(); i++ ) {
	  p_ds[idx++] = (float)atof(pl[i].c_str());
	  if ( idx >= np*nds ) break;
	} // end of for(i)

	if ( idx >= np*nds ) break;
      } // end of while of scalar data

      //break;
    } // end of SCALARS

    // VECTORS
    else if ( pl[0] == string("VECTORS") ) {
      ndv = 3;

      // allocate
      p_dv = (float*)Allocate(sizeof(float)*np*ndv);
      if ( ! p_dv ) return false;

      // read vector data
      idx = 0;
      while ( ! sf.eof() ) {
	VFR::GetLine(sf, buff);
	if ( buff.size() < 1 ) continue;

	pl = VFR::SplitString(buff, string(" \t\n\r"));
	for ( i = 0; i < pl.size(); i++ ) {
	  p_dv[idx++] = (float)atof(pl[i].c_str());
	  if ( idx >= np*ndv ) break;
	} // end of for(i)

	if ( idx >= np*ndv ) break;
      } // end of while of vector data

      //break;
    } // end of VECTORS

    else { // not supported
      //return false;
      continue;
    }

  } // end of while

  // combine VECTOR/SCALAR data
  m_dataLen = nds + ndv;
  if ( m_dataLen > 0 ) {
    m_pData = (float*)ReAllocate(m_pData, sizeof(float)*np*m_dataLen);
    if ( ! m_pData ) return false;
    idx = 0;
    if ( nds > 0 && ndv > 0 ) {
      // store data order = VECTOR, SCALAR
      for ( i = 0; i < np; i++ ) {
	for ( j = 0; j < ndv; j++ )
	  m_pData[idx++] = p_dv[i*ndv + j];
	for ( j = 0; j < nds; j++ )
	  m_pData[idx++] = p_ds[i*nds + j];
      } // end of for(i)
    }
    else if ( ndv > 0 ) {
      for ( i = 0; i < np; i++ ) {
	for ( j = 0; j < ndv; j++ )
	  m_pData[idx++] = p_dv[i*ndv + j];
      } // end of for(i)
    }
    else { // nds > 0
      for ( i = 0; i < np; i++ ) {
	for ( j = 0; j < nds; j++ )
	  m_pData[idx++] = p_ds[i*nds + j];
      } // end of for(i)
    }
  }
  if ( p_ds ) DeAllocate(p_ds);
  if ( p_dv ) DeAllocate(p_dv);

  generateBbox();
  notice();
  return true;
}

bool vsnData_TriaCells::checkMinMax(const bool wholeStp, const bool progress) {
  register size_t i, j, k, idx;
  string errMsg = string("DataTriaCells[") + getName() + string("]: ");

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
    sprintf(msgBuff, "Data_TriaCells: checking min/max of file #1 of %lu",
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
  m_maxNumCells = nIndices / 3;
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
    sprintf(msgBuff, "Data_TriaCells: checking min/max of file #%lu of %lu",
	    i+1, m_numStps);
    prgMsg = vsnApp::ConvSysToWx(msgBuff);
    if ( progDlg && ! progDlg->Update(i, prgMsg) ) break;

    path = m_seqPathes[i];
    TriaCellsType type = checkType(path, path_body);
    if ( type == TriaCellsNONE ) {
      char txt[64]; sprintf(txt, "  step#%zd ", i); errMsg += txt;
      errMsg += BaseName(path_body, string(""), vsnPath_getDelimChar());
      errMsg += string("\n");
      badFileExists = true;
      continue;
    }

    // load data
    bool ret = false;;
    switch ( type ) {
    case TriaCellsVTK:
      ret = readVTK(path_body);
      break;
    } // end of switch(type)
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
    if ( m_maxNumCells < nIndices / 3 ) m_maxNumCells = nIndices / 3;
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

bool vsnData_TriaCells::getVectorMaxLen(const CES::Vec3<int>& vidx, float& vml)
{
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
    TriaCellsType type = checkType(path, path_body);
    if ( type == TriaCellsNONE ) continue;

    // load data
    bool ret = false;;
    switch ( type ) {
    case TriaCellsVTK:
      ret = readVTK(path_body);
      break;
    } // end of switch(type)
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

