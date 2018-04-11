//
// vsnMethod_Sv_histogram
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

#include "vsnMethod_Sv_histogram.h"
#include "vsnError.h"

using namespace std;
using namespace CES;
using namespace VSN;


//----------------------------------------------------------------
// class vsnMethod_Sv_histogram
//----------------------------------------------------------------

/* constructors / destructor */

vsnMethod_Sv_histogram::vsnMethod_Sv_histogram(const string& name)
  : vsnMethod_histogram(name)
{
  _values = NULL;
}

vsnMethod_Sv_histogram::~vsnMethod_Sv_histogram() {
  if ( _values )
    DeAllocate(_values);
}


/* vsnMethodObj methods */

bool vsnMethod_Sv_histogram::update(const bool force) {
  return updateStep(m_requestedStp, force);
}


/* vsnTimeSeriesMethodIF methods */

bool vsnMethod_Sv_histogram::updateStep(const int stp,
                                        const bool force, const bool cascade)
{
  vsnData_Sv* pData = dynamic_cast<vsnData_Sv*>(p_refData);
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

  // get Sv datas
  CES::Vec3<size_t> dims = pData->getDims();
  size_t dimSz = dims[0] * dims[1] * dims[2];
  size_t dlen = pData->getDataLen();
  if ( dimSz < 1 || dlen < 1 ) return true;
  const float* pd = pData->getData(m_requestedStp);
  const unsigned char* maskptr = pData->getMask();

  // check selected data
  float minmax[2] = {0.f, 1.f};
  if ( m_selectedData == DATA_Veclen ) {
    if ( ! isValidVecData() ) return true;
    pData->getVectorMaxLen(m_vecDataIdx, minmax[1]);
  }
  else if ( m_selectedData > 0 && m_selectedData <= dlen ) {
    pData->getMinMax(m_selectedData -1, minmax);
  }
  else return true;
  if ( minmax[1] == minmax[0] ) minmax[1] += 1.f;

  // class unit
  if ( m_numClasses < 1 ) return false;
  register float clsUnit = (minmax[1] - minmax[0]) / m_numClasses;

  // allocate
  _values = (vector2*)ReAllocate(_values, sizeof(vector2)*m_numClasses);
  if ( ! _values ) {
    ErrMsg(MsgERR, getMethodType() + string("[") + getName()
           + string("]: memory allocation failed"));
    return false;
  }

  // set class
  register int i, j;
  register float val, clsVal = minmax[0];
  for ( i = 0; i < m_numClasses; i++ ) {
    _values[i][0] = clsVal;
    _values[i][1] = 0.f;
    clsVal += clsUnit;
  } // end of for(i)

  // counting
  if ( m_selectedData == DATA_Veclen ) {
    Vec3<float> vv;
    for ( i = 0; i < dimSz; i++ ) {
      if ( maskptr && maskptr[i] ) continue;
      vv.m_v[0] = pd[i * dlen + m_vecDataIdx.m_v[0]];
      vv.m_v[1] = pd[i * dlen + m_vecDataIdx.m_v[1]];
      vv.m_v[2] = pd[i * dlen + m_vecDataIdx.m_v[2]];
      val = vv.Length();

      clsVal = minmax[0] + clsUnit;
      for ( j = 0; j < m_numClasses; j++ ) {
	if ( val < clsVal ) {
	  _values[j][1] += 1.f;
	  break;
	}
	clsVal += clsUnit;
      } // end of for(j)
    } // end of for(i)
  }
  else {
    for ( i = 0; i < dimSz; i++ ) {
      if ( maskptr && maskptr[i] ) continue;
      val = pd[i * dlen + m_selectedData -1];

      clsVal = minmax[0] + clsUnit;
      for ( j = 0; j < m_numClasses; j++ ) {
	if ( val < clsVal ) {
	  _values[j][1] += 1.f;
	  break;
	}
	clsVal += clsUnit;
      } // end of for(j)
    } // end of for(i)
  }

  // check range
  float range[2] = {0.f, 0.f};
  for ( j = 0; j < m_numClasses; j++ ) {
    if ( range[1] < _values[j][1] )
      range[1] = _values[j][1];
  } // end of for(j)

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
    titleStr += string("Data[") + pData->getName() + string("] Histogram");
  } // end of if(m_showTitle)

  if ( ! DoPlot(m_numClasses, _values, range, titleStr) )
    return false;

  // ok
  m_updatedStp = m_requestedStp;
  return true;
}
