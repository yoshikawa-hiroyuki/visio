//
// vsnMethod_Sv_minmaxGlyph
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

#include "vsnMethod_Sv_minmaxGlyph.h"
#include "vsnError.h"

#define LIMIT_GLYPH_NUM 1000

using namespace std;
using namespace CES;
using namespace VSN;


//----------------------------------------------------------------
// class vsnMethod_Sv_minmaxGlyph
//----------------------------------------------------------------

/* methods */

bool vsnMethod_Sv_minmaxGlyph::getMinMaxCurStp(float minmax[2]) {
  vsnData_Sv* pData = dynamic_cast<vsnData_Sv*>(p_refData);
  if ( ! pData ) return false;

  const Vec3<size_t>& dims = pData->getDims();
  register size_t dimSz = dims[0]*dims[1]*dims[2];
  if ( dimSz < 1 ) return false;

  const float *pd = pData->getData(pData->getCurrentStepIdx());
  if ( ! pd ) return false;

  register size_t i, idx, dlen = pData->getDataLen();
  register float val;

  if ( m_selectedData == DATA_None ) return false;
  else if ( m_selectedData == DATA_Veclen ) {
    Vec3<int> vidx = m_vecDataIdx;
    if ( vidx.m_v[0] < 0 || vidx.m_v[0] >= dlen ||
	 vidx.m_v[1] < 0 || vidx.m_v[1] >= dlen ||
	 vidx.m_v[2] < 0 || vidx.m_v[2] >= dlen ) return false;

    minmax[0] = Vec3<float>(pd[vidx.m_v[0]],
			    pd[vidx.m_v[1]], pd[vidx.m_v[2]]).Length();
    minmax[1] = minmax[0];
    for ( i = 1; i < dimSz; i++ ) {
      idx = dlen * i;
      val = Vec3<float>(pd[idx+vidx.m_v[0]],
			pd[idx+vidx.m_v[1]], pd[idx+vidx.m_v[2]]).Length();
      if ( minmax[0] > val ) minmax[0] = val;
      if ( minmax[1] < val ) minmax[1] = val;
    } // end of for(i)
  }
  else if ( m_selectedData > 0 && m_selectedData <= dlen ) {
    register int n = m_selectedData -1;
    if ( pData->getNumSteps() == 1 ) return pData->getMinMax(n, minmax);

    minmax[0] = minmax[1] = pd[n];
    for ( i = 1; i < dimSz; i++ ) {
      val = pd[dlen * i + n];
      if ( minmax[0] > val ) minmax[0] = val;
      if ( minmax[1] < val ) minmax[1] = val;
    } // end of for(i)
  }
  else return false;

  return true;
}


/* vsnTimeSeriesMethodIF method */

bool vsnMethod_Sv_minmaxGlyph::updateStep(const int stp,
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

  // check glyphs
  if ( ! m_minGlyph || ! m_maxGlyph ) return false;
  m_minGlyph->getPrivateMaterial()->setRenderMode(RT_NONE);
  m_maxGlyph->getPrivateMaterial()->setRenderMode(RT_NONE);
  if ( ! m_show ) return true;

  // check selected data
  size_t dlen = pData->getDataLen();
  float dr[2] = {0.f, 1.f};
  if ( ! getMinMaxCurStp(dr) ) return true;
  float delta = (float)(fabs(dr[1] - dr[0]) * m_tolerance / 100.0);
  dr[0] += delta;
  dr[1] -= delta;

  // get Sv data
  const float* gptr = pData->getCoord(m_requestedStp);
  if ( ! gptr ) {
    return false;
  }
  const float* dptr = pData->getData(m_requestedStp);
  if ( ! dptr ) {
    return false;
  }
  const unsigned char* maskptr = pData->getMask();

  // prepare data
  Vec3<size_t> dims = pData->getDims();
  size_t dimSz = dims[0] * dims[1] * dims[2];
  if ( dimSz < 1 ) return false;
  if ( ! m_minGlyph->alcVerts(dimSz) || ! m_maxGlyph->alcVerts(dimSz) ) {
    ErrMsg(MsgERR, getMethodType() + string("[") + getName()
	   + string("]: out of memory for vertices"));
    return false;
  }
  vector3* pvMin = m_minGlyph->getVerts();
  vector3* pvMax = m_maxGlyph->getVerts();

  register size_t i, j, idx;
  register float val;
  register int idxMin =0, idxMax =0;
  if ( m_selectedData == DATA_Veclen ) {
    Vec3<float> vv;
    for ( i = 0; i < dimSz; i++ ) {
      if ( maskptr && maskptr[i] ) continue;
      idx = i * dlen;
      j = i * 3;
      vv.m_v[0] = dptr[idx + m_vecDataIdx.m_v[0]];
      vv.m_v[1] = dptr[idx + m_vecDataIdx.m_v[1]];
      vv.m_v[2] = dptr[idx + m_vecDataIdx.m_v[2]];
      val = vv.Length();

      if ( val < dr[0] ) {
	pvMin[idxMin][0]= gptr[j];
	pvMin[idxMin][1]= gptr[j + 1];
	pvMin[idxMin][2]= gptr[j + 2];
	idxMin ++;
      }
      if ( val > dr[1] ) {
	pvMax[idxMax][0]= gptr[j];
	pvMax[idxMax][1]= gptr[j + 1];
	pvMax[idxMax][2]= gptr[j + 2];
	idxMax ++;
      }
    } // end of for(i)
  }
  else {
    for ( i = 0; i < dimSz; i++ ) {
      if ( maskptr && maskptr[i] ) continue;
      idx = i * dlen + m_selectedData -1;
      j = i * 3;
      val = dptr[idx];

      if ( val < dr[0] ) {
	pvMin[idxMin][0]= gptr[j];
	pvMin[idxMin][1]= gptr[j + 1];
	pvMin[idxMin][2]= gptr[j + 2];
	idxMin ++;
      }
      if ( val > dr[1] ) {
	pvMax[idxMax][0]= gptr[j];
	pvMax[idxMax][1]= gptr[j + 1];
	pvMax[idxMax][2]= gptr[j + 2];
	idxMax ++;
      }
    } // end of for(i)
  }
  (void)m_minGlyph->alcVerts(idxMin);
  (void)m_maxGlyph->alcVerts(idxMax);

  // set range
  updateLutRange();

  // check force show
  if ( m_showMin && idxMin > LIMIT_GLYPH_NUM ) {
    char msg[128];
    sprintf(msg, "too many min-glyphs (%d+) may be shown, "
	    "set to off 'show min glyph.", LIMIT_GLYPH_NUM);
    ErrMsg(MsgINFO, msg);
    m_showMin = false;
    updateUI();
  }
  if ( m_showMax && idxMax > LIMIT_GLYPH_NUM ) {
    char msg[128];
    sprintf(msg, "too many max-glyphs (%d+) may be shown, "
	    "set to off 'show max glyph.", LIMIT_GLYPH_NUM);
    ErrMsg(MsgINFO, msg);
    m_showMax = false;
    updateUI();
  }
  updateShowGlyph();

  // ok
  m_updatedStp = m_requestedStp;
  return true;
}
