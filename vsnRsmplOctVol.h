//
// vsnRsmplOctVol
//
#ifndef _VSN_RSMPL_OCT_VOL_H_
#define _VSN_RSMPL_OCT_VOL_H_

#include "vsnOctTree.h"
#include "vfrDefs.h"

namespace VSN {
  static inline bool IsPow2(unsigned int x) {
    return ((x > 0) && !(x & (x - 1)));
  }
  static inline size_t WrapPow2(const size_t x) {
    register int s;
    for ( s = 1; s < x; s *= 2 );
    return s;
  }
};


//----------------------------------------------------------------
// class vsnRsmplOctVol
//----------------------------------------------------------------
class vsnRsmplOctVol {
public:

  //--------------------------------------------------------------
  // class vsnRsmplOctVol::RsmplBlk
  //--------------------------------------------------------------
  class RsmplBlk {
  public:
    size_t            m_level;
    size_t            m_subMaxLevel;
    CES::Vec3<size_t> m_dims;
    CES::Vec3<float>  m_bbox[2];
    float*            m_pData;

    RsmplBlk() : m_level(0), m_subMaxLevel(0), m_pData(NULL) {}
    virtual ~RsmplBlk() {if ( m_pData ) VFR::DeAllocate(m_pData);}
    size_t GetProxySize() const {
      return m_dims.m_v[0]*m_dims.m_v[1]*m_dims.m_v[2];
    }

    static bool LevelComp(const RsmplBlk* b1, const RsmplBlk* b2);
  };

  vsnRsmplOctVol();
  ~vsnRsmplOctVol();

  void Reset();
  bool Setup(vsnOctTree* poct, const bool div,
	     const bool doSample =true, const bool doInterp = false,
	     const int levelLim =-1);

  RsmplBlk* FindByPos(const CES::Vec3<float>& pos);

  size_t getDivN() const {return m_dN;}
  CES::Vec3<size_t> getDivs() const {return m_nDivs;}
  CES::Vec3<size_t> getMaxDims() const {return m_maxDims;}
  CES::Vec3<size_t> getRealMaxDims() const {return m_realMaxDims;}
  const CES::Vec3<float>* getBbox() const {return m_bbox;}
  const std::deque<RsmplBlk*>& getRsmplBlkList() const {return m_blkLst;}
  std::deque<RsmplBlk*>& getRsmplBlkList() {return m_blkLst;}

  size_t GetTotalProxySize() const;
  void SortBlkByLevel();

private:
  vsnOctTree*           p_refOct;
  size_t                m_dN;
  CES::Vec3<size_t>     m_nDivs;
  CES::Vec3<size_t>     m_maxDims;
  CES::Vec3<size_t>     m_realMaxDims;
  CES::Vec3<float>      m_bbox[2];
  std::deque<RsmplBlk*> m_blkLst;
  std::deque<float>     m_minLst;

  static int GetMaxLevelByArea(const vsnOctTree* poct,
			       const size_t rI[2],
			       const size_t rJ[2],
			       const size_t rK[2]);
};


/* inline methods */

inline bool
vsnRsmplOctVol::RsmplBlk::LevelComp(const vsnRsmplOctVol::RsmplBlk* b1,
				    const vsnRsmplOctVol::RsmplBlk* b2) {
  if ( ! b1 && ! b2 ) return true;
  if ( ! b1 ) return false;
  if ( ! b2 ) return true;
  if ( b1->m_level == b2->m_level )
    return (b1->m_subMaxLevel > b2->m_subMaxLevel);
  return (b1->m_level > b2->m_level);
}


inline int vsnRsmplOctVol::GetMaxLevelByArea(const vsnOctTree* poct,
					     const size_t rI[2],
					     const size_t rJ[2],
					     const size_t rK[2]) {
  if ( ! poct ) return -1;
  if ( poct->m_rootDims[0] <= rI[1] ||
       poct->m_rootDims[1] <= rJ[1] ||
       poct->m_rootDims[2] <= rK[1] ) return -1;
  register size_t i, j, k, idx;
  register size_t ml = 0;
  idx = poct->m_rootDims[0]*poct->m_rootDims[1]*rK[0]
    + poct->m_rootDims[0]*rJ[0] + rI[0];
  if ( poct->m_pRootLst[idx] ) ml = poct->m_pRootLst[idx]->m_maxLevel;
  for ( k = rK[0]; k < rK[1]; k++ )
    for ( j = rJ[0]; j < rJ[1]; j++ )
      for ( i = rI[0]; i < rI[1]; i++ ) {
	idx = poct->m_rootDims[0]*poct->m_rootDims[1]*k
	  + poct->m_rootDims[0]*j + i;
	vsnOctTree::RootNode* prn = poct->m_pRootLst[idx];
	if ( ! prn ) continue;
	if ( ml < prn->m_maxLevel )
	  ml = prn->m_maxLevel;
      } // end of for(i)
  return ml;
}

#endif // _VSN_RSMPL_OCT_VOL_H_

