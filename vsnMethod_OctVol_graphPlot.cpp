//
// vsnMethod_OctVol_graphPlot
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

#include "vsnMethod_OctVol_graphPlot.h"
#include "vsnError.h"

using namespace std;
using namespace CES;
using namespace VSN;


//----------------------------------------------------------------
// class vsnMethod_OctVol_graphPlot
//----------------------------------------------------------------

/* constructors / destructor */

vsnMethod_OctVol_graphPlot::vsnMethod_OctVol_graphPlot(const string& name)
  : vsnMethod_graphPlot(name)
{
  m_pnodeLst = NULL;
  m_sampleData = NULL;
}

vsnMethod_OctVol_graphPlot::~vsnMethod_OctVol_graphPlot() {
  if ( m_pnodeLst )
    DeAllocate(m_pnodeLst);

  if ( m_sampleData )
    DeAllocate(m_sampleData);
}


/* vsnMethod_graphPlot methods */

bool vsnMethod_OctVol_graphPlot::exportCsv(const std::string& path) {
  if ( path.empty() ) return false;
  if ( m_updatedStp < 0 ) return false;
  if ( ! m_sampleData ) return false;

  // check sampler
  if ( ! p_splr ) return true;
  Point2 sampleSize = p_splr->getSampleNumber();
  size_t sampleSz = sampleSize.x * sampleSize.y;
  if ( sampleSz < 1 ) return true;
  const vector3* const samplePts = p_splr->getSamplePoints();
  if ( ! samplePts ) {
    ErrMsg(MsgERR, getMethodType() + string("[") + getName()
           + string("]: can't get sampling points data"));
    return false;
  }

  if ( ! ExportCsv(path, sampleSize, samplePts, m_sampleData) )
    return false;

  // ok
  return true;
}

/* vsnMethodObj methods */

bool vsnMethod_OctVol_graphPlot::update(const bool force) {
  return updateStep(m_requestedStp, force);
}


/* vsnTimeSeriesMethodIF methods */

bool vsnMethod_OctVol_graphPlot::updateStep(const int stp,
					    const bool force,
					    const bool cascade)
{
  vsnData_OctVol* pData = dynamic_cast<vsnData_OctVol*>(p_refData);
  if ( ! pData ) return false;

  m_requestedStp = stp;
  if ( m_requestedStp < 0 )
    m_requestedStp = pData->getCurrentStepIdx();

  if ( force ) m_updatedStp = -1;
  if ( m_updatedStp == m_requestedStp ) return true;
  m_updatedStp = -1;

  // gnuplot interface
  if ( ! m_pGnuPlotIF ) return false;
  if ( ! m_pGnuPlotIF->IsConnected() ) {
    ErrMsg(MsgWARN, getMethodType() + string("[") + getName()
           + string("]: can't connect to Gnuplot, check program path"));
    return true;
  }

  // check selected data
  size_t dlen = pData->getDataLen();
  float minmax[2] = {0.f, 1.f};
  if ( m_selectedData == DATA_None ) return true;
  else if ( m_selectedData == DATA_Veclen ) {
    if ( ! isValidVecData() ) return true;
    pData->getVectorMaxLen(m_vecDataIdx, minmax[1]);
  }
  else if ( m_selectedData > 0 && m_selectedData <= dlen ) {
    pData->getMinMax(m_selectedData -1, minmax);
  }
  else return true;

  // check sampler
  if ( ! p_splr ) return true;
  Point2 sampleSize = p_splr->getSampleNumber();
  size_t sampleSz = sampleSize.x * sampleSize.y;
  if ( sampleSz < 1 ) return true;
  const vector3* const samplePts = p_splr->getSamplePoints();
  if ( ! samplePts ) {
    ErrMsg(MsgERR, getMethodType() + string("[") + getName()
           + string("]: can't get sampling points data"));
    return false;
  }

  // get nodelist of samplePts
  register int i;
  m_pnodeLst
    = (vsnOctTree::Node**)ReAllocate(m_pnodeLst,
				     sizeof(vsnOctTree::Node*)*sampleSz);
  if ( ! m_pnodeLst ) {
    ErrMsg(MsgERR, getMethodType() + string("[") + getName()
           + string("]: memory allocation failed"));
    return false;
  }
  for ( i = 0; i < sampleSz; i++ ) {
    m_pnodeLst[i] = pData->getNode(samplePts[i]);
  } // end of for(i)

  // get datalist on samplePts
  m_sampleData = (float*)ReAllocate(m_sampleData, sizeof(float)*sampleSz);
  if ( ! m_sampleData ) {
    ErrMsg(MsgERR, getMethodType() + string("[") + getName()
           + string("]: memory allocation failed"));
    return false;
  }
  for ( i = 0; i < sampleSz; i++ ) m_sampleData[i] = minmax[0];
  if ( m_selectedData == DATA_Veclen ) {
    Vec3<float> dval;
    for ( i = 0; i < sampleSz; i++ ) {
      if ( ! m_pnodeLst[i] ) continue;
      if ( pData->interpolateData(Vec3<float>(samplePts[i]), m_pnodeLst[i],
                                  m_vecDataIdx, dval) )
        m_sampleData[i] = dval.Length();
    } // end of for(i)
  } else {
    Vec3<float> dval; Vec3<int> didx(m_selectedData-1, -1, -1);
    for ( i = 0; i < sampleSz; i++ ) {
      if ( ! m_pnodeLst[i] ) continue;
      if ( pData->interpolateData(Vec3<float>(samplePts[i]), m_pnodeLst[i],
                                  didx, dval) )
        m_sampleData[i] = dval.m_v[0];
    } // end of for(i)
  }

  // send commands to gnuplot
  char txt[64];
  string titleStr;
  if ( m_showTitle ) {
    if ( m_selectedData == DATA_Veclen )
      titleStr += string("vector length : ");
    else {
      sprintf(txt, "data%d : ", m_selectedData -1);
      titleStr += txt;
    }
    titleStr += string("sampler[") + p_splr->getName() + string("], ");
    titleStr += string("Data[") + pData->getName() + string("]");
  } // end of if(m_showTitle)

  if ( ! DoPlot(sampleSize, samplePts, m_sampleData, minmax, titleStr) )
    return false;

  // ok
  m_updatedStp = m_requestedStp;
  return true;
}
