//
// vsnRsmplOctVol
//
#include "nvrBspTree.h"
#include "vsnRsmplOctVol.h"
#include <algorithm>
#include <functional>

using namespace std;
using namespace CES;
using namespace VFR;
using namespace VSN;


/* constructors / destructor */

vsnRsmplOctVol::vsnRsmplOctVol()
  : p_refOct(NULL), m_dN(0)
{
}

vsnRsmplOctVol::~vsnRsmplOctVol() {
  Reset();
}


/* methods */

bool vsnRsmplOctVol::Setup(vsnOctTree* poct, const bool div,
			   const bool doSample, const bool doInterp,
			   const int levelLim) {
  Reset();
  if ( ! poct ) return false;

  Vec3<size_t> rootDims = poct->m_rootDims;
  size_t maxLevel = poct->m_maxLevel;
  m_bbox[0] = poct->m_bbox[0];
  m_bbox[1] = poct->m_bbox[1];

  m_realMaxDims[0] = (rootDims.m_v[0] << maxLevel);
  m_realMaxDims[1] = (rootDims.m_v[1] << maxLevel);
  m_realMaxDims[2] = (rootDims.m_v[2] << maxLevel);
  m_maxDims = m_realMaxDims;

  size_t nBlks = 1;
  m_nDivs = Vec3<size_t>(1,1,1);
  NVR::Dim3 origDims(m_realMaxDims[0], m_realMaxDims[1], m_realMaxDims[2]);
  NVR::Dim3 divTms(0, 0, 0);
  if ( div ) {
    divTms = NVR::CalcDivTimes(origDims);
    NVR::Dim3 wrapDims = NVR::GetWrapDims(origDims, divTms);
    m_maxDims = Vec3<size_t>(wrapDims.size);
  } else
    m_maxDims = m_realMaxDims;
  nBlks = 0x1 << (divTms[0] + divTms[1] + divTms[2]);
  m_nDivs[0] = 0x1 << divTms[0];
  m_nDivs[1] = 0x1 << divTms[1];
  m_nDivs[2] = 0x1 << divTms[2];

  Vec3<float> bbl = m_bbox[1] - m_bbox[0];
  m_bbox[1][0] = m_bbox[0][0]
    + bbl[0] * ((m_maxDims[0] - 1.f) / (m_realMaxDims[0] - 1.f));
  m_bbox[1][1] = m_bbox[0][1]
    + bbl[1] * ((m_maxDims[1] - 1.f) / (m_realMaxDims[1] - 1.f));
  m_bbox[1][2] = m_bbox[0][2]
    + bbl[2] * ((m_maxDims[2] - 1.f) / (m_realMaxDims[2] - 1.f));

  register size_t i, j, k, idx;
  size_t ri[2], rj[2], rk[2];
  m_blkLst.resize(nBlks);

  Vec3<float> bbSz = (m_bbox[1] - m_bbox[0]);
  bbSz[0] = bbSz[0] / (float)m_nDivs[0];
  bbSz[1] = bbSz[1] / (float)m_nDivs[1];
  bbSz[2] = bbSz[2] / (float)m_nDivs[2];
  Vec3<size_t> maxDimsBlk, subDims;
  maxDimsBlk[0] = NVR::WrapPow2(m_maxDims[0]) >> divTms[0];
  maxDimsBlk[1] = NVR::WrapPow2(m_maxDims[1]) >> divTms[1];
  maxDimsBlk[2] = NVR::WrapPow2(m_maxDims[2]) >> divTms[2];

  float minMax[2];
  if ( poct->m_dataLen > 0 ) {
    m_minLst.clear();
    for ( i = 0; i < poct->m_dataLen; i++ ) {
      poct->GetMinMax(i, minMax);
      m_minLst.push_back(minMax[0] - 1e-3f);
    } // end of for(i)
  }

  Vec3<float> wkBb[2];
  idx = 0;
  rk[0] = rk[1] = 0;
  subDims[2] = maxDimsBlk[2];
  for ( k = 0; k < m_nDivs[2]; k++ ) {
    wkBb[0][2] = m_bbox[0][2] + bbSz[2] * (float)k;
    rk[0] = rk[1];
    if ( k == m_nDivs[2] - 1 ) {
      wkBb[1][2] = m_bbox[1][2];
      rk[1] = rootDims[2] - 1;
      subDims[2] = m_maxDims[2] - maxDimsBlk[2] * k;
    } else {
      wkBb[1][2] = wkBb[0][2] + bbSz[2];
      rk[1] = (size_t)((wkBb[1][2] - m_bbox[0][2]) / poct->m_pitch[2]);
      if ( rk[1] >= rootDims[2] ) rk[1] = rootDims[2] - 1;
    }

    rj[0] = rj[1] = 0;
    subDims[1] = maxDimsBlk[1];
    for ( j = 0; j < m_nDivs[1]; j++ ) {
      wkBb[0][1] = m_bbox[0][1] + bbSz[1] * (float)j;
      rj[0] = rj[1];
      if ( j == m_nDivs[1] - 1 ) {
	wkBb[1][1] = m_bbox[1][1];
	rj[1] = rootDims[1] - 1;
	subDims[1] = m_maxDims[1] - maxDimsBlk[1] * j;
      } else {
	wkBb[1][1] = wkBb[0][1] + bbSz[1];
	rj[1] = (size_t)((wkBb[1][1] - m_bbox[0][1]) / poct->m_pitch[1]);
	if ( rj[1] >= rootDims[1] ) rj[1] = rootDims[1] - 1;
      }

      ri[0] = ri[1] = 0;
      subDims[0] = maxDimsBlk[0];
      for ( i = 0; i < m_nDivs[0]; i++ ) {
	RsmplBlk* b = new RsmplBlk();
	if ( ! b ) return false;

	wkBb[0][0] = m_bbox[0][0] + bbSz[0] * (float)i;
	ri[0] = ri[1];
	if ( i == m_nDivs[0] - 1 ) {
	  wkBb[1][0] = m_bbox[1][0];
	  ri[1] = rootDims[0] - 1;
	  subDims[0] = m_maxDims[0] - maxDimsBlk[0] * i;
	} else {
	  wkBb[1][0] = wkBb[0][0] + bbSz[0];
	  ri[1] = (size_t)((wkBb[1][0] - m_bbox[0][0]) / poct->m_pitch[0]);
	  if ( ri[1] >= rootDims[0] ) ri[1] = rootDims[0] - 1;
	}

	if ( wkBb[0][0] > poct->m_bbox[1][0] ||
	     wkBb[0][1] > poct->m_bbox[1][1] ||
	     wkBb[0][2] > poct->m_bbox[1][2] ) {
	  delete b;
	  m_blkLst[idx++] = NULL;
	  continue;
	}
	b->m_bbox[0] = wkBb[0];
	b->m_bbox[1] = wkBb[1];

	register int subMaxLevel = GetMaxLevelByArea(poct, ri, rj, rk);
	if ( subMaxLevel < 0 ) subMaxLevel = 0;
	if ( levelLim >= 0 && subMaxLevel > levelLim )
	  subMaxLevel = levelLim;
	b->m_level = b->m_subMaxLevel = (size_t)subMaxLevel;
	b->m_dims = subDims;
	b->m_dims.m_v[0] = (b->m_dims.m_v[0] >> (maxLevel - b->m_level));
	b->m_dims.m_v[1] = (b->m_dims.m_v[1] >> (maxLevel - b->m_level));
	b->m_dims.m_v[2] = (b->m_dims.m_v[2] >> (maxLevel - b->m_level));

	size_t dimSz = b->m_dims[0] * b->m_dims[1] * b->m_dims[2];
	if ( poct->m_dataLen > 0 ) {
	  b->m_pData
	    = (float*)VFR::Allocate(dimSz*poct->m_dataLen*sizeof(float));
	  if ( ! b->m_pData ) {
	    delete b;
	    return false;
	  }
	  memset(b->m_pData, 0, dimSz * poct->m_dataLen * sizeof(float));

	  if ( doSample ) {
	    Vec3<float> pos, dpos;
	    dpos[0]= (b->m_bbox[1][0]-b->m_bbox[0][0])/(float)(b->m_dims[0]-1);
	    dpos[1]= (b->m_bbox[1][1]-b->m_bbox[0][1])/(float)(b->m_dims[1]-1);
	    dpos[2]= (b->m_bbox[1][2]-b->m_bbox[0][2])/(float)(b->m_dims[2]-1);
	    register size_t ii, jj, kk, ll, idx2;
	    idx2 = 0;
	    for ( kk = 0; kk < b->m_dims[2]; kk++ ) {
	      if ( kk == b->m_dims[2] - 1 ) pos[2] = b->m_bbox[1][2];
	      else pos[2] = b->m_bbox[0][2] + (float)dpos[2] * kk;
	      for ( jj = 0; jj < b->m_dims[1]; jj++ ) {
		if ( jj == b->m_dims[1] - 1 ) pos[1] = b->m_bbox[1][1];
		else pos[1] = b->m_bbox[0][1] + (float)dpos[1] * jj;
		for ( ii = 0; ii < b->m_dims[0]; ii++ ) {
		  if ( ii == b->m_dims[0] - 1 ) pos[0] = b->m_bbox[1][0];
		  else pos[0] = b->m_bbox[0][0] + (float)dpos[0] * ii;
		  
		  vsnOctTree::Node* pnode = poct->FindByPos(pos);
		  if ( ! pnode ) {
		    for ( ll = 0; ll < poct->m_dataLen; ll++ ) {
		      b->m_pData[idx2] = m_minLst[ll];
		      idx2 ++;
		    } // end of for(ll)
		    continue;
		  }
		  if ( doInterp ) {
		    for ( ll = 0; ll < poct->m_dataLen; ll++ ) {
		      float dval;
		      if ( poct->InterpolateData(pos, pnode, ll, dval) )
			b->m_pData[idx2] = dval;
		      idx2 ++;
		    } // end of for(ll)
		  } else {
		    memcpy(&b->m_pData[idx2], pnode->m_pData,
			   sizeof(float)*poct->m_dataLen);
		    idx2 += poct->m_dataLen;
		  }
		} // end of for(ii)
	      } // end of for(jj)
	    } // end of for(kk)
	  } // end of if(doSample)
	} // end of if(dataLen)

	m_blkLst[idx++] = b;
      } // end of for(i)
    } // end of for(j)
  } // end of for(k)

  m_dN = divTms[0] + divTms[1] + divTms[2];
  p_refOct = poct;
  return true;
}

void vsnRsmplOctVol::Reset() {
  p_refOct = NULL;
  m_dN = 0;
  m_maxDims = Vec3<size_t>(0, 0, 0);
  m_realMaxDims = Vec3<size_t>(0, 0, 0);
  m_nDivs = Vec3<size_t>(0, 0, 0);

  deque<RsmplBlk*>::iterator it;
  for ( it = m_blkLst.begin(); it != m_blkLst.end(); it++ )
    if ( *it ) delete (*it);
  m_blkLst.clear();
}

vsnRsmplOctVol::RsmplBlk*
vsnRsmplOctVol::FindByPos(const CES::Vec3<float>& pos) {
  if ( pos[0] < m_bbox[0][0] || pos[0] > m_bbox[1][0] ||
       pos[1] < m_bbox[0][1] || pos[1] > m_bbox[1][1] ||
       pos[2] < m_bbox[0][2] || pos[2] > m_bbox[1][2] )
    return NULL;

  Vec3<float> bbSz = (m_bbox[1] - m_bbox[0]);
  bbSz[0] = bbSz[0] / (float)m_nDivs[0];
  bbSz[1] = bbSz[1] / (float)m_nDivs[1];
  bbSz[2] = bbSz[2] / (float)m_nDivs[2];  
  Vec3<float> relpos = pos - m_bbox[0];
  Vec3<int> idxs;
  idxs[0] = (int)(relpos[0] / bbSz[0]);
  idxs[1] = (int)(relpos[1] / bbSz[1]);
  idxs[2] = (int)(relpos[2] / bbSz[2]);
  if ( idxs[0] < 0 || idxs[0] >= m_nDivs[0] ||
       idxs[1] < 0 || idxs[1] >= m_nDivs[1] ||
       idxs[2] < 0 || idxs[2] >= m_nDivs[2] )
    return NULL;
  size_t idx = m_nDivs[0]*m_nDivs[1]*idxs[2] + m_nDivs[0]*idxs[1] + idxs[0];
  if ( m_blkLst.size() <= idx ) return NULL;
  return m_blkLst[idx];
}

size_t vsnRsmplOctVol::GetTotalProxySize() const {
  register size_t total = 0;
  deque<RsmplBlk*>::const_iterator it;
  for ( it = m_blkLst.begin(); it != m_blkLst.end(); it++ ) {
    if ( ! *it ) continue;
    total += (*it)->GetProxySize();
  } // end of for(it)
  return total;
}

void vsnRsmplOctVol::SortBlkByLevel() {
  if ( m_blkLst.size() < 2 ) return;
  std::sort(m_blkLst.begin(), m_blkLst.end(), RsmplBlk::LevelComp);
}
