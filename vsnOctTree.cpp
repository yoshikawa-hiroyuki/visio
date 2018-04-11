//
// vsnOctTree
//
// MHIR append begin
#include <map>
#include <iostream>
#include <stdexcept>
#include "vsnXVX.h"
// MHIR append end

#include "vsnOctTree.h"
#include "vfrDefs.h"
#include "utilEndian.h"

using namespace std;
using namespace CES;
using namespace VFR;


//----------------------------------------------------------------
// vsnOctTree::Node
//----------------------------------------------------------------
vsnOctTree::Node::Node(const size_t dlen)
  : m_pedigree(0), m_hasChildren(false), p_parent(NULL), m_pData(NULL)
{
  for ( register int i = 0; i < 8; i++ )
    p_children[i] = NULL;
  if ( dlen > 0 ) {
    m_pData = (float*)CeAllocate(dlen, sizeof(float));
    assert(m_pData);
  }
}

vsnOctTree::Node::~Node() {
  if ( m_pData ) DeAllocate((void*)m_pData);
}

bool vsnOctTree::Node::ImportFile(FILE* ifp, bool& flread, vsnOctTree& ot,
				  const bool needEc) {
  if ( ! ifp ) return false;
  int buff[4];
  if ( fread(buff, 4, 4, ifp) != 4 ) return false;
  if ( needEc ) BSWAPVEC(buff, 4);
  if ( ! dynamic_cast<vsnOctTree::RootNode*>(this) &&
       (buff[1] < 0 || buff[1] >= 8) ) return false;
  if ( buff[2] != 0 && buff[2] != 1 ) return false;
  ////m_pedigree = m_pedigree * 10 + (buff[1] + 1);
  bool hasChildren = buff[2];

  if ( ot.m_dataLen > 0 && m_pData ) {
    if ( fread(buff, 4, 1, ifp) != 1 ) return false;
    if ( needEc ) BSWAPVEC(buff, 1);
    if ( buff[0] != 4 * ot.m_dataLen ) return false;
    if ( fread(m_pData, 4, ot.m_dataLen, ifp) != ot.m_dataLen ) return false;
    if ( needEc ) BSWAPVEC(m_pData, ot.m_dataLen);
    if ( fread(buff, 4, 1, ifp) != 1 ) return false;
  }

  register size_t i;
  if ( hasChildren ) { //-------- this is a branch node --------
    register size_t j;
    if ( ! ot.AlcChildren(this) ) return false;
    memset(m_pData, 0, sizeof(float)*ot.m_dataLen);
    for ( i = 0; i < 8; i++ ) {
      if ( ! p_children[i] ) return false;
      if ( ! p_children[i]->ImportFile(ifp, flread, ot, needEc) ) return false;

      for ( j = 0; j < ot.m_dataLen; j++ ) {
	m_pData[j] += p_children[i]->m_pData[j];
      } // end of for(j)
    } // end of for(i)

    for ( j = 0; j < ot.m_dataLen; j++ )
      m_pData[j] /= 8.f;
  }
  else { //-------- this is a leaf node --------
    // check minmax
    if ( ot.m_dataLen > 0 && m_pData ) {
      if ( ! flread ) {
	memcpy(ot.m_pMin, m_pData, sizeof(float)*ot.m_dataLen);
	memcpy(ot.m_pMax, m_pData, sizeof(float)*ot.m_dataLen);
	flread = true;
      } else {
	for ( i = 0; i < ot.m_dataLen; i++ ) {
	  if ( ot.m_pMin[i] > m_pData[i] ) ot.m_pMin[i] = m_pData[i];
	  if ( ot.m_pMax[i] < m_pData[i] ) ot.m_pMax[i] = m_pData[i];
	} // end of for(i)
      }
    }

    // regist leaf
    ot.m_pLeafLst.insert(this);

    // max level check
    i = 0;
    Node* pn = this;
    while ( pn->p_parent ) {
      pn = pn->p_parent;
      i ++;
    } // end of while(pn->p_parent);
    RootNode* prn = dynamic_cast<RootNode*>(pn);
    if ( prn && i > prn->m_maxLevel )
      prn->m_maxLevel = i;
  }

   return true;
}

bool vsnOctTree::Node::ExportFile(FILE* ofp, vsnOctTree& ot) {
  if ( ! ofp ) return false;
  int buff[4];
  buff[0] = buff[3] = 4 * 2;
  buff[1] = (m_pedigree == 0) ? 0 : (m_pedigree % 10) - 1;
  buff[2] = m_hasChildren ? 1 : 0;
  if ( fwrite(buff, 4, 4, ofp) != 4 ) return false;

  if ( ot.m_dataLen > 0 ) {
    if ( ! m_pData ) return false;
    buff[0] = 4 * ot.m_dataLen;
    if ( fwrite(buff, 4, 1, ofp) != 1 ) return false;
    if ( fwrite(m_pData, 4, ot.m_dataLen, ofp) != ot.m_dataLen ) return false;
    if ( fwrite(buff, 4, 1, ofp) != 1 ) return false;
  }

  if ( m_hasChildren ) {
    for ( register int i = 0; i < 8 ; i++ ) {
      if ( ! p_children[i] ) return false;
      if ( ! p_children[i]->ExportFile(ofp, ot) ) return false;
    } // end of for(i)
  } // end of if(m_hasChildren)

  return true; 
}

vsnOctTree::Node* vsnOctTree::Node::FindByPos(const Vec3<float>& pos,
					      const Vec3<float>& orig,
					      const Vec3<float>& size,
					      const int level) {
  if ( pos.m_v[0] < orig.m_v[0] ||
       pos.m_v[1] < orig.m_v[1] ||
       pos.m_v[2] < orig.m_v[2] ) return NULL;

  Vec3<float> relPos = pos - orig;
  if ( size.m_v[0] < relPos.m_v[0] ||
       size.m_v[1] < relPos.m_v[1] ||
       size.m_v[2] < relPos.m_v[2] ) return NULL;

  if ( ! m_hasChildren ) return this;
  if ( level == 0 ) return this;

  Vec3<float> halfSize = size * 0.5f;
  Vec3<float> newOrig = orig;
  int tgtChild = 0;
  if ( relPos.m_v[0] > halfSize.m_v[0] ) {
    tgtChild |= 0x1; newOrig.m_v[0] += halfSize.m_v[0];
  }
  if ( relPos.m_v[1] > halfSize.m_v[1] ) {
    tgtChild |= (0x1<<1); newOrig.m_v[1] += halfSize.m_v[1];
  }
  if ( relPos.m_v[2] > halfSize.m_v[2] ) {
    tgtChild |= (0x1<<2); newOrig.m_v[2] += halfSize.m_v[2];
  }

  Node* pTgtChild = p_children[tgtChild];
  if ( ! pTgtChild ) return this;
  return pTgtChild->FindByPos(pos, newOrig, halfSize, level-1);
}

vsnOctTree::Node* vsnOctTree::Node::FindByPos(const Vec3<double>& pos,
					      const Vec3<double>& orig,
					      const Vec3<double>& size,
					      const int level) {
  if ( pos.m_v[0] < orig.m_v[0] ||
       pos.m_v[1] < orig.m_v[1] ||
       pos.m_v[2] < orig.m_v[2] ) return NULL;

  Vec3<double> relPos = pos - orig;
  if ( size.m_v[0] < relPos.m_v[0] ||
       size.m_v[1] < relPos.m_v[1] ||
       size.m_v[2] < relPos.m_v[2] ) return NULL;

  if ( ! m_hasChildren ) return this;
  if ( level == 0 ) return this;

  Vec3<double> halfSize = size * 0.5;
  Vec3<double> newOrig = orig;
  int tgtChild = 0;
  if ( relPos.m_v[0] > halfSize.m_v[0] ) {
    tgtChild |= 0x1; newOrig.m_v[0] += halfSize.m_v[0];
  }
  if ( relPos.m_v[1] > halfSize.m_v[1] ) {
    tgtChild |= (0x1<<1); newOrig.m_v[1] += halfSize.m_v[1];
  }
  if ( relPos.m_v[2] > halfSize.m_v[2] ) {
    tgtChild |= (0x1<<2); newOrig.m_v[2] += halfSize.m_v[2];
  }

  Node* pTgtChild = p_children[tgtChild];
  if ( ! pTgtChild ) return this;
  return pTgtChild->FindByPos(pos, newOrig, halfSize, level-1);
}

size_t vsnOctTree::Node::GetLevel() const {
  size_t level = 0;
  Node* pnode = p_parent;
  while ( pnode ) {
    level ++;
    pnode = pnode->p_parent;
  } // end of while(pnode)
  return level;
}


//----------------------------------------------------------------
// class vsnOctTree
//----------------------------------------------------------------

vsnOctTree::vsnOctTree()
  : m_dataLen(0), m_maxLevel(0), m_pMin(NULL), m_pMax(NULL) {
}

vsnOctTree::~vsnOctTree() {
  Clear();
  if ( m_pMin ) DeAllocate((void*)m_pMin);
  if ( m_pMax ) DeAllocate((void*)m_pMax);
}

void vsnOctTree::Clear() {
  set<Node*>::iterator it;
  for ( it = m_pWholeLst.begin(); it != m_pWholeLst.end(); it++ )
    if ( *it ) delete (*it);
  m_pWholeLst.clear();
  m_pRootLst.clear();
  m_pLeafLst.clear();

  m_rootDims[0] = m_rootDims[1] = m_rootDims[2] = 0;
  m_bbox[0] = Vec3<float>(0.f, 0.f, 0.f);
  m_bbox[1] = m_bbox[0];
  m_pitch[0] = m_pitch[1] = m_pitch[2] = 0.f;

  m_dataLen = 0;
  m_maxLevel = 0;
}

bool vsnOctTree::ImportFile(const char* fname) {
  if ( ! fname ) return false;

  // MHIR append begin
  {
    // ファイル名が *.XVX であれば XVX の読み込みを行う
    //
    size_t fnlen = strlen(fname);
    if (fnlen > 4 &&
	(!strcmp(&fname[fnlen-3], "xvx") || !strcmp(&fname[fnlen-3], "XVX"))) {
      try {
	vsnXVX xvx;
	xvx.ImportFile(fname);
	xvx.Convert(this);
      } catch (std::runtime_error& e) {
	std::cout << e.what() << std::endl;
	return false;
      }
      return true;
    }
  }
  // MHIR append end

  FILE* ifp = fopen(fname, "rb");
  if ( ! ifp ) return false;
  int buff[9];

  // endian check
  bool needEc = false;
  EMatchType emt = MatchEndian(ifp, 12);
  switch ( emt ) {
  case UnMatch: needEc = true; break;
  case UnKnown: return false;
  default: break;
  } // end of switch(emt)

  // read dims of root cells
  if ( fread(buff, 4, 5, ifp) != 5 ) {
    fclose(ifp);
    return false;
  }
  if ( needEc ) BSWAPVEC(buff, 5);
  int dimSz = buff[1] * buff[2] * buff[3];
  if ( dimSz < 1 ) {
    fclose(ifp);
    return false;
  }

  Clear();
  m_rootDims[0] = buff[1];
  m_rootDims[1] = buff[2];
  m_rootDims[2] = buff[3];

  // read dataLen, aux
  if ( fread(buff, 4, 4, ifp) != 4 ) {
    fclose(ifp);
    return false;
  }
  if ( needEc ) BSWAPVEC(buff, 4);
  if ( buff[1] < 0 ) {
    fclose(ifp);
    return false;
  }
  m_dataLen = buff[1];
  if ( m_dataLen > 0 ) {
    m_pMin = (float*)ReAllocate(m_pMin, sizeof(float)*m_dataLen);
    m_pMax = (float*)ReAllocate(m_pMax, sizeof(float)*m_dataLen);
    if ( ! m_pMin || ! m_pMax ) {
      fclose(ifp);
      return false;
    }
  }

  // read geometry
  if ( fread(buff, 4, 9, ifp) != 9 ) {
    fclose(ifp);
    return false;
  }
  if ( needEc ) BSWAPVEC(buff, 9);
  float* pflt = (float*)buff;
  m_bbox[0][0] = pflt[2];
  m_bbox[0][1] = pflt[3];
  m_bbox[0][2] = pflt[4];

  switch ( buff[1] ) {
  case 0: // whole size
    m_bbox[1][0] = pflt[5] + m_bbox[0][0];
    m_bbox[1][1] = pflt[6] + m_bbox[0][1];
    m_bbox[1][2] = pflt[7] + m_bbox[0][2];
    break;
  case 1: // root-node size
    m_bbox[1][0] = pflt[5] * (float)m_rootDims[0] + m_bbox[0][0];
    m_bbox[1][1] = pflt[6] * (float)m_rootDims[1] + m_bbox[0][1];
    m_bbox[1][2] = pflt[7] * (float)m_rootDims[2] + m_bbox[0][2];
    break;
  case 2: // bbox
    m_bbox[1][0] = pflt[5];
    m_bbox[1][1] = pflt[6];
    m_bbox[1][2] = pflt[7];
    break;
  default:
    fclose(ifp);
    return false;
  } // end of switch(buff[1])

  // calc pitch
  Vec3<float> bbLen = m_bbox[1] - m_bbox[0];
  m_pitch[0] = bbLen[0] / m_rootDims[0];
  m_pitch[1] = bbLen[1] / m_rootDims[1];
  m_pitch[2] = bbLen[2] / m_rootDims[2];
  if ( m_pitch[0] < 1e-8 || m_pitch[1] < 1e-8 || m_pitch[2] < 1e-8 ) {
    fclose(ifp);
    return false;
  }

  // read nodes
  register size_t i, j, k;
  bool firstLeafRead = false;
  for ( k = 0; k < m_rootDims[2]; k++ )
    for ( j = 0; j < m_rootDims[1]; j++ )
      for ( i = 0; i < m_rootDims[0]; i++ ) {
        RootNode* pnode = new RootNode(Vec3<size_t>(i, j, k), m_dataLen);
        if ( ! pnode ) {
          fclose(ifp);
	  Clear();
          return false;
	}
	m_pWholeLst.insert(pnode);

        if ( ! pnode->ImportFile(ifp, firstLeafRead, *this, needEc) ) {
          fclose(ifp);
          Clear();
          return false;
        }
        m_pRootLst.push_back(pnode);

	if ( m_maxLevel < pnode->m_maxLevel )
	  m_maxLevel = pnode->m_maxLevel;
      } // end of for(i)

  fclose(ifp);
  return true;
}

bool vsnOctTree::ExportFile(const char* fname) {
  if ( ! fname ) return false;
  size_t rSz = m_rootDims[0]*m_rootDims[1]*m_rootDims[2];
  if ( rSz < 1 ) return false;
  if ( m_pRootLst.size() < rSz ) return false;

  FILE* ofp = fopen(fname, "wb");
  if ( ! ofp ) return false;
  int buff[5];
  float fbuff[6];

  // write dims
  buff[0] = buff[4] = 12; // 4 * 3
  buff[1] = m_rootDims[0];
  buff[2] = m_rootDims[1];
  buff[3] = m_rootDims[2];
  if ( fwrite(buff, 4, 5, ofp) != 5 ) {
    fclose(ofp);
    return false;
  }

  // write dataLen, misc
  buff[0] = buff[3] = 8; // 4 * 2
  buff[1] = m_dataLen;
  buff[2] = 0;
  if ( fwrite(buff, 4, 4, ofp) != 4 ) {
    fclose(ofp);
    return false;
  }

  // write geometry
  buff[0] = 28; // 4 * 7
  buff[1] = 0; // AreaType = whole
  if ( fwrite(buff, 4, 2, ofp) != 2 ) {
    fclose(ofp);
    return false;
  }
  fbuff[0] = m_bbox[0][0]; fbuff[1] = m_bbox[0][1]; fbuff[2] = m_bbox[0][2];
  fbuff[3] = m_bbox[1][0] - m_bbox[0][0];
  fbuff[4] = m_bbox[1][1] - m_bbox[0][1];
  fbuff[5] = m_bbox[1][2] - m_bbox[0][2];
  if ( fwrite(fbuff, 4, 6, ofp) != 6 ) {
    fclose(ofp);
    return false;
  }
  if ( fwrite(buff, 4, 1, ofp) != 1 ) {
    fclose(ofp);
    return false;
  }

  // write cells
  register size_t i, j, k;
  for ( k = 0; k < m_rootDims[2]; k++ )
    for ( j = 0; j < m_rootDims[1]; j++ )
      for ( i = 0; i < m_rootDims[0]; i++ ) {
        if ( ! m_pRootLst[RIDX(i,j,k)]->ExportFile(ofp, *this) ) {
          fclose(ofp);
          return false;
        }
      } // end of for(i)

  fclose(ofp);
  return true;
}

vsnOctTree::RootNode*
vsnOctTree::GetRootNode(const size_t I, const size_t J, const size_t K) {
  if ( m_pRootLst.empty() ) return NULL;
  if ( I >= m_rootDims[0] || J >= m_rootDims[1] || K >= m_rootDims[2] )
    return NULL;
  return m_pRootLst[RIDX(I, J, K)];
}

vsnOctTree::Node*
vsnOctTree::FindByPos(const Vec3<float>& pos, const int level) {
  if ( m_pRootLst.size() < 1 ) return NULL;

  // bbox check
  if ( pos.m_v[0] < m_bbox[0].m_v[0] || pos.m_v[0] > m_bbox[1].m_v[0] ||
       pos.m_v[1] < m_bbox[0].m_v[1] || pos.m_v[1] > m_bbox[1].m_v[1] ||
       pos.m_v[2] < m_bbox[0].m_v[2] || pos.m_v[2] > m_bbox[1].m_v[2] ) {
    return NULL;
  }

  // get target root cell
  Vec3<float> relPos = pos - m_bbox[0];
  size_t I = (size_t)(relPos.m_v[0] / m_pitch[0]); if (I == m_rootDims[0]) I--;
  size_t J = (size_t)(relPos.m_v[1] / m_pitch[1]); if (J == m_rootDims[1]) J--;
  size_t K = (size_t)(relPos.m_v[2] / m_pitch[2]); if (K == m_rootDims[2]) K--;
  size_t rootIdx = RIDX(I, J, K);
  if ( rootIdx >= m_pRootLst.size() ) return NULL;
  Node* pTgtRoot = m_pRootLst[rootIdx];
  if ( ! pTgtRoot || level == 0 ) return pTgtRoot;

  // find in the target root cell
  Vec3<float> tgtRootOrg = m_bbox[0];
  tgtRootOrg.m_v[0] += m_pitch[0] * I;
  tgtRootOrg.m_v[1] += m_pitch[1] * J;
  tgtRootOrg.m_v[2] += m_pitch[2] * K;
  return pTgtRoot->FindByPos(pos, tgtRootOrg, m_pitch, level);
}

vsnOctTree::Node*
vsnOctTree::FindByPos(const Vec3<double>& pos, const int level) {
  if ( m_pRootLst.size() < 1 ) return NULL;

  Vec3<double> dbb[2], dpitch;
  dbb[0].m_v[0] = m_bbox[0].m_v[0];
  dbb[0].m_v[1] = m_bbox[0].m_v[1];
  dbb[0].m_v[2] = m_bbox[0].m_v[2];
  dbb[1].m_v[0] = m_bbox[1].m_v[0];
  dbb[1].m_v[1] = m_bbox[1].m_v[1];
  dbb[1].m_v[2] = m_bbox[1].m_v[2];
  dpitch.m_v[0] = m_pitch.m_v[0];
  dpitch.m_v[1] = m_pitch.m_v[1];
  dpitch.m_v[2] = m_pitch.m_v[2];

  // bbox check
  if ( pos.m_v[0] < dbb[0].m_v[0] || pos.m_v[0] > dbb[1].m_v[0] ||
       pos.m_v[1] < dbb[0].m_v[1] || pos.m_v[1] > dbb[1].m_v[1] ||
       pos.m_v[2] < dbb[0].m_v[2] || pos.m_v[2] > dbb[1].m_v[2] ) {
    return NULL;
  }

  // get target root cell
  Vec3<double> relPos = pos - dbb[0];
  size_t I = (size_t)(relPos.m_v[0] / dpitch[0]); if (I == m_rootDims[0]) I--;
  size_t J = (size_t)(relPos.m_v[1] / dpitch[1]); if (J == m_rootDims[1]) J--;
  size_t K = (size_t)(relPos.m_v[2] / dpitch[2]); if (K == m_rootDims[2]) K--;
  
  size_t rootIdx = RIDX(I, J, K);
  if ( rootIdx >= m_pRootLst.size() ) return NULL;
  Node* pTgtRoot = m_pRootLst[rootIdx];
  if ( ! pTgtRoot || level == 0 ) return pTgtRoot;

  // find in the target root cell
  Vec3<double> tgtRootOrg = dbb[0];
  tgtRootOrg.m_v[0] += dpitch[0] * I;
  tgtRootOrg.m_v[1] += dpitch[1] * J;
  tgtRootOrg.m_v[2] += dpitch[2] * K;
  return pTgtRoot->FindByPos(pos, tgtRootOrg, dpitch, level);
}

bool vsnOctTree::GetMinMax(const size_t didx, float minmax[2]) const {
  if ( didx >= m_dataLen ) return false;
  if ( ! m_pMin || ! m_pMax ) return false;
  minmax[0] = m_pMin[didx];
  minmax[1] = m_pMax[didx];
  return true;
}

bool vsnOctTree::AlcChildren(vsnOctTree::Node* pn) {
  if ( ! pn ) return false;
  if ( pn->m_pedigree > 1000000000000000ll ) return false; // max level (16)
  if ( ! DelChildren(pn) ) return false;

  register int i;
  for ( i = 0; i < 8; i++ ) {
    pn->p_children[i] = new Node(m_dataLen);
    if ( ! pn->p_children[i] ) return false;
    pn->p_children[i]->m_pedigree = pn->m_pedigree * 10 + i + 1;
    pn->p_children[i]->p_parent = pn;
    m_pWholeLst.insert(pn->p_children[i]);
  }

  pn->m_hasChildren = true;
  return true;
}

bool vsnOctTree::DelChildren(vsnOctTree::Node* pn) {
  if ( ! pn ) return false;
  if ( ! pn->m_hasChildren ) return true;

  register int i;
  for ( i = 0; i < 8; i++ ) {
    if ( ! pn->p_children[i] ) continue;
    if ( ! DelChildren(pn->p_children[i]) )
      return false;

    set<Node*>::iterator it_s = m_pLeafLst.find(pn->p_children[i]);
    if ( it_s != m_pLeafLst.end() ) m_pLeafLst.erase(it_s);
    it_s = m_pWholeLst.find(pn->p_children[i]);
    if ( it_s != m_pWholeLst.end() ) m_pWholeLst.erase(it_s);

    delete pn->p_children[i];
    pn->p_children[i] = NULL;
  } // end of for(i)

  pn->m_hasChildren = false;
  return true;
}

// GetNeighbor : returns a neighbor node
vsnOctTree::Node*
vsnOctTree::GetNeighbor(vsnOctTree::Node* pn, const Vec3<int>& dir) {
  if ( ! pn || m_pRootLst.size() < 1 ) return NULL;
  if ( dir.m_v[0] < -1 || dir.m_v[0] > 1 ||
       dir.m_v[1] < -1 || dir.m_v[1] > 1 ||
       dir.m_v[2] < -1 || dir.m_v[2] > 1 ) return NULL;
  if ( dir.m_v[0] == 0 && dir.m_v[1] == 0 && dir.m_v[2] == 0 ) return pn;

  register size_t i, j, l;
  register size_t lvl = 0; // = pn->GetLevel();
  Node* wpn = pn;
  while ( wpn->p_parent ) {
    lvl ++;
    wpn = wpn->p_parent;
  } // end of while(wpn)
  RootNode* rpn = dynamic_cast<RootNode*>(wpn);
  if ( ! rpn ) return NULL;
  Vec3<size_t> rootIdx = rpn->m_idx;

  // decomp the pedigree
  Vec3<size_t> wkpedig;
  VSN::PedigType pdg = pn->m_pedigree;
  for ( i = 0; i < lvl ; i++ ) {
    l = (pdg % 10) - 1;
    if ( l & 0x1      ) wkpedig[0] += (0x1 << i);
    if ( l & (0x1<<1) ) wkpedig[1] += (0x1 << i);
    if ( l & (0x1<<2) ) wkpedig[2] += (0x1 << i);
      
    pdg /= 10;
  } // end of for(i)

  // find target root
  wkpedig.m_v[0] = wkpedig.m_v[0] + dir.m_v[0];
  wkpedig.m_v[1] = wkpedig.m_v[1] + dir.m_v[1];
  wkpedig.m_v[2] = wkpedig.m_v[2] + dir.m_v[2];

  Vec3<int> tgtRootIdx((int)rootIdx[0], (int)rootIdx[1], (int)rootIdx[2]);
  l = (0x1 << lvl);
  if ( wkpedig.m_v[0] & l ) tgtRootIdx.m_v[0] += dir.m_v[0];
  if ( wkpedig.m_v[1] & l ) tgtRootIdx.m_v[1] += dir.m_v[1];
  if ( wkpedig.m_v[2] & l ) tgtRootIdx.m_v[2] += dir.m_v[2];

  if ( tgtRootIdx[0] < 0 || tgtRootIdx[0] >= m_rootDims[0] ||
       tgtRootIdx[1] < 0 || tgtRootIdx[1] >= m_rootDims[1] ||
       tgtRootIdx[2] < 0 || tgtRootIdx[2] >= m_rootDims[2] )
    return NULL; // out of region

  // get the target node
  rpn = GetRootNode((size_t)tgtRootIdx[0],
		    (size_t)tgtRootIdx[1], (size_t)tgtRootIdx[2]);
  if ( ! rpn ) return NULL;
  wpn = rpn;
  for ( i = lvl; i > 0; i-- ) {
    if ( ! wpn->m_hasChildren ) break;

    l = (0x1 << (i-1));
    j = 0;
    if ( wkpedig[0] & l ) j += 1;
    if ( wkpedig[1] & l ) j += 2;
    if ( wkpedig[2] & l ) j += 4;

    wpn = wpn->p_children[j];
    // 2009-05-27 MHIR begin
    // 並列計算結果の部分領域では全ての子供がいるとは限らない
    //
    if (wpn == NULL) break;
    // 2009-05-27 MHIR end
  } // end of for(i)

  return wpn;
}

// InterpolateData : scalar data interplation
bool vsnOctTree::InterpolateData(const Vec3<float>& pos, Node* p_node,
				 const size_t didx, float& dval) {
  const Vec3<int> di3((int)didx, -1, -1);
  Vec3<float> dv3;
  if ( ! InterpolateData(pos, p_node, di3, dv3) ) return false;
  dval = dv3.m_v[0];
  return true;
}

// InterpolateData : vector data interplation
bool vsnOctTree::InterpolateData(const Vec3<float>& pos, Node* p_node,
				 const Vec3<int>& didx, Vec3<float>& dval) {
  if ( ! p_node ) {
    p_node = FindByPos(pos);
    if ( ! p_node ) return false; // out of range ?
    // MHIR append begin
    // 可視化対象のセルではない
    if (! p_node -> HasData()) return false;
    // MHIR append end
  }
  register size_t i, j, k, l;

  // get node geometry
  register size_t lvl = p_node->GetLevel();
  Node* pnode(p_node);
  register float scaleFac = 1.f;
  for ( l = 0; l < lvl; l++ ) scaleFac *= 0.5f;
  Vec3<float> orig, size = m_pitch;
  register VSN::PedigType pedig = pnode->m_pedigree;
  while ( pedig && pnode ) {
    register int relp = pedig % 10 -1;
    if ( relp & (0x1)    ) orig.m_v[0] += m_pitch.m_v[0] * scaleFac;
    if ( relp & (0x1<<1) ) orig.m_v[1] += m_pitch.m_v[1] * scaleFac;
    if ( relp & (0x1<<2) ) orig.m_v[2] += m_pitch.m_v[2] * scaleFac;
    size = size * 0.5f;
    scaleFac *= 2.f;

    pedig = pedig / 10;
    pnode = pnode->p_parent;
  } // end of while(pedig && pnode)

  RootNode* proot = dynamic_cast<RootNode*>(pnode);
  if ( ! proot ) return false;

  orig.m_v[0] += m_pitch.m_v[0] * (float)proot->m_idx[0];
  orig.m_v[1] += m_pitch.m_v[1] * (float)proot->m_idx[1];
  orig.m_v[2] += m_pitch.m_v[2] * (float)proot->m_idx[2];
  orig = orig + m_bbox[0];
  orig = orig + (size * 0.5f); // move to center

  if ( pos.m_v[0] == orig.m_v[0] &&
       pos.m_v[1] == orig.m_v[1] && pos.m_v[2] == orig.m_v[2] ) {
    for ( i = 0; i < 3; i++ ) {
      if ( didx[i] < 0 || didx[i] >= m_dataLen ) {
	dval.m_v[i] = 0.f; continue;
      }
      dval.m_v[i] = p_node->m_pData[didx[i]];
    } // end of for(i)
    return true;
  }

  // get relative area of pos in p_node
  unsigned int relPos = 0; // 0 .. 7
  if ( pos.m_v[0] > orig.m_v[0] ) relPos |= (0x1);
  if ( pos.m_v[1] > orig.m_v[1] ) relPos |= (0x1<<1);
  if ( pos.m_v[2] > orig.m_v[2] ) relPos |= (0x1<<2);

  // get neighbors
  Node* pnlst[8] = {NULL, };
  Vec3<int> ndir, wnd[2];
  ndir.m_v[0] = (relPos & 0x1) ? 1 : -1;
  ndir.m_v[1] = (relPos & (0x1<<1)) ? 1 : -1;
  ndir.m_v[2] = (relPos & (0x1<<2)) ? 1 : -1;
  wnd[0] = ndir;
  if ( wnd[0].m_v[0] > wnd[1].m_v[0] ) // swap
    wnd[0].m_v[0] ^= wnd[1].m_v[0] ^= wnd[0].m_v[0] ^= wnd[1].m_v[0];
  if ( wnd[0].m_v[1] > wnd[1].m_v[1] )
    wnd[0].m_v[1] ^= wnd[1].m_v[1] ^= wnd[0].m_v[1] ^= wnd[1].m_v[1];
  if ( wnd[0].m_v[2] > wnd[1].m_v[2] )
    wnd[0].m_v[2] ^= wnd[1].m_v[2] ^= wnd[0].m_v[2] ^= wnd[1].m_v[2];
  l = 0;
  for ( k = 0; k < 2; k++ )
    for ( j = 0; j < 2; j++ )
      for ( i = 0; i < 2; i++ ) {
	Vec3<int> nnidx(wnd[i].m_v[0], wnd[j].m_v[1], wnd[k].m_v[2]);
	pnlst[l] = GetNeighbor(p_node, nnidx);
	l++;
      } // end of for(i)

  // check level of the neighbors
  j = k = 0; // flags
  for ( i = 0; i < 8; i++ ) {
    if ( ! pnlst[i] ) {
      j = 1; // out of region, p_node is boundary
    }
    else if ( pnlst[i]->GetLevel() != lvl ) {
      k = 1;
      break;
    }
  } // end of for(i)

  if ( k == 0 ) {  // all neighbors level is the same as p_node
    Vec3<float> x0;
    x0.m_v[0] = orig.m_v[0] + size.m_v[0] * wnd[0].m_v[0];
    x0.m_v[1] = orig.m_v[1] + size.m_v[1] * wnd[0].m_v[1];
    x0.m_v[2] = orig.m_v[2] + size.m_v[2] * wnd[0].m_v[2];
    if ( j == 0 )
      return TriLinear_Interp(x0, size, pos, didx, pnlst, dval);
    else
      return CoLat_Interp(x0, size, pos, didx, pnlst, dval);
  }

  // level is not the same, 
  if ( ! p_node->p_parent )
    return false; // something wrong ...?
  return InterpolateData(pos, p_node->p_parent, didx, dval);
}

// TriLinear_Interp : tri-linear interpolation
bool vsnOctTree::TriLinear_Interp(const Vec3<float>& x0,
				  const Vec3<float>& sz,
				  const Vec3<float>& pos,
				  const Vec3<int>& didx,
				  Node* pnlst[8],
				  CES::Vec3<float>& dval)
{
  if ( sz.m_v[0] == 0.f || sz.m_v[1] == 0.f || sz.m_v[2] == 0.f )
    return false;
  register size_t l;
  for ( l = 0; l < 8; l++ )
    if ( ! pnlst[l] ) return false;

  // MHIR append begin
  for (l = 0; l < 8; ++l) {
    if (!pnlst[l] -> HasData()) return false;
  }
  // MHIR append end

  Vec3<float> u[2];
  u[1].m_v[0] = (pos.m_v[0] - x0.m_v[0]) / sz.m_v[0];
  u[1].m_v[1] = (pos.m_v[1] - x0.m_v[1]) / sz.m_v[1];
  u[1].m_v[2] = (pos.m_v[2] - x0.m_v[2]) / sz.m_v[2];
  u[0] = Vec3<float>(1.f, 1.f, 1.f) - u[1];

  for ( l = 0; l < 3; l++ ) {
    dval[l] = 0.f;
    if ( didx[l] < 0 || didx[l] >= m_dataLen ) continue;

    dval[l] += // (0,0,0)
      u[0].m_v[0] * u[0].m_v[1] * u[0].m_v[2] * pnlst[0]->m_pData[didx[l]];
    dval[l] += // (1,0,0)
      u[1].m_v[0] * u[0].m_v[1] * u[0].m_v[2] * pnlst[1]->m_pData[didx[l]];
    dval[l] += // (0,1,0)
      u[0].m_v[0] * u[1].m_v[1] * u[0].m_v[2] * pnlst[2]->m_pData[didx[l]];
    dval[l] += // (1,1,0)
      u[1].m_v[0] * u[1].m_v[1] * u[0].m_v[2] * pnlst[3]->m_pData[didx[l]];
    dval[l] += // (0,0,1)
      u[0].m_v[0] * u[0].m_v[1] * u[1].m_v[2] * pnlst[4]->m_pData[didx[l]];
    dval[l] += // (1,0,1)
      u[1].m_v[0] * u[0].m_v[1] * u[1].m_v[2] * pnlst[5]->m_pData[didx[l]];
    dval[l] += // (0,1,1)
      u[0].m_v[0] * u[1].m_v[1] * u[1].m_v[2] * pnlst[6]->m_pData[didx[l]];
    dval[l] += // (1,1,1)
      u[1].m_v[0] * u[1].m_v[1] * u[1].m_v[2] * pnlst[7]->m_pData[didx[l]];
  } // end of for(l)
  return true;
}

// CoLat_Interp : linear interpolation in co-lattice space
bool vsnOctTree::CoLat_Interp(const Vec3<float>& x0, const Vec3<float>& sz,
			      const Vec3<float>& pos, const Vec3<int>& didx,
			      Node* pnlst[8], CES::Vec3<float>& dval)
{
  if ( sz.m_v[0] == 0.f || sz.m_v[1] == 0.f || sz.m_v[2] == 0.f )
    return false;
  map<size_t, Node*> pnmap;
  map<size_t, Node*>::iterator it;
  register size_t i, l, nvc = 0;

  // count valid nodes
  for ( l = 0; l < 8; l++ ) {
    if ( pnlst[l] )
      pnmap.insert(make_pair(l, pnlst[l]));
  } // end of for(l)
  nvc = pnmap.size();
  if ( nvc == 0 ) return false;

  // MHIR append begin
  for (l = 0; l < 8; ++l) {
    if (pnlst[l] && !pnlst[l] -> HasData()) return false;
  }
  // MHIR append end

  Vec3<float> u[2];
  u[1].m_v[0] = (pos.m_v[0] - x0.m_v[0]) / sz.m_v[0];
  u[1].m_v[1] = (pos.m_v[1] - x0.m_v[1]) / sz.m_v[1];
  u[1].m_v[2] = (pos.m_v[2] - x0.m_v[2]) / sz.m_v[2];
  u[0] = Vec3<float>(1.f, 1.f, 1.f) - u[1];

  if ( nvc == 2 ) { // linear interpolation
    int dir = -1;
    size_t bptn;
    it = pnmap.begin(); bptn = it->first;
    it++; bptn ^= it->first;
    if ( bptn & 0x1 ) dir = 0; 
    if ( bptn & (0x1<<1) ) dir = 1; 
    if ( bptn & (0x1<<2) ) dir = 2;
    if ( dir == -1 ) goto _CALC_MEAN;

    for ( l = 0; l < 3; l++ ) {
      dval[l] = 0.f; i = 0;
      if ( didx[l] < 0 || didx[l] >= m_dataLen ) continue;
      for ( it = pnmap.begin(); it != pnmap.end(); it++ )
	dval[l] += u[i++].m_v[dir] * it->second->m_pData[didx[l]];
    } // end of for(l)

    return true;
  } // end of (nvc==2)

  if ( nvc == 4 ) { // bi-linear interpolation
    int idir = -1;
    it = pnmap.begin();
    size_t bfst = it->first;
    size_t bptn = 7;
    for ( it++; it != pnmap.end(); it++ ) {
      if ( (bfst & 0x1) != (it->first & 0x1) ) bptn &= (~0x1);
      if ( (bfst & (0x1<<1)) != (it->first & (0x1<<1)) ) bptn &= (~(0x1<<1));
      if ( (bfst & (0x1<<2)) != (it->first & (0x1<<2)) ) bptn &= (~(0x1<<2));
    }
    if ( bptn & 0x1 ) idir = 0; 
    if ( bptn & (0x1<<1) ) idir = 1; 
    if ( bptn & (0x1<<2) ) idir = 2;
    if ( idir == -1 ) goto _CALC_MEAN;

    u[0].m_v[idir] = u[1].m_v[idir] = 1.f;

    for ( l = 0; l < 3; l++ ) {
      dval[l] = 0.f;
      if ( didx[l] < 0 || didx[l] >= m_dataLen ) continue;
      if ( pnlst[0] )
	dval[l] += // (0,0,0)
	  u[0].m_v[0] * u[0].m_v[1] * u[0].m_v[2] * pnlst[0]->m_pData[didx[l]];
      if ( pnlst[1] )
	dval[l] += // (1,0,0)
	  u[1].m_v[0] * u[0].m_v[1] * u[0].m_v[2] * pnlst[1]->m_pData[didx[l]];
      if ( pnlst[2] )
	dval[l] += // (0,1,0)
	  u[0].m_v[0] * u[1].m_v[1] * u[0].m_v[2] * pnlst[2]->m_pData[didx[l]];
      if ( pnlst[3] )
	dval[l] += // (1,1,0)
	  u[1].m_v[0] * u[1].m_v[1] * u[0].m_v[2] * pnlst[3]->m_pData[didx[l]];
      if ( pnlst[4] )
	dval[l] += // (0,0,1)
	  u[0].m_v[0] * u[0].m_v[1] * u[1].m_v[2] * pnlst[4]->m_pData[didx[l]];
      if ( pnlst[5] )
	dval[l] += // (1,0,1)
	  u[1].m_v[0] * u[0].m_v[1] * u[1].m_v[2] * pnlst[5]->m_pData[didx[l]];
      if ( pnlst[6] )
	dval[l] += // (0,1,1)
	  u[0].m_v[0] * u[1].m_v[1] * u[1].m_v[2] * pnlst[6]->m_pData[didx[l]];
      if ( pnlst[7] )
	dval[l] += // (1,1,1)
	  u[1].m_v[0] * u[1].m_v[1] * u[1].m_v[2] * pnlst[7]->m_pData[didx[l]];
    } // end of for(l)

    return true;
  } // end of (nvc==4)

  if ( nvc == 8 ) { // tri-linear interpolation
    for ( l = 0; l < 3; l++ ) {
      dval[l] = 0.f;
      if ( didx[l] < 0 || didx[l] >= m_dataLen ) continue;
      dval[l] += // (0,0,0)
	u[0].m_v[0] * u[0].m_v[1] * u[0].m_v[2] * pnlst[0]->m_pData[didx[l]];
      dval[l] += // (1,0,0)
	u[1].m_v[0] * u[0].m_v[1] * u[0].m_v[2] * pnlst[1]->m_pData[didx[l]];
      dval[l] += // (0,1,0)
	u[0].m_v[0] * u[1].m_v[1] * u[0].m_v[2] * pnlst[2]->m_pData[didx[l]];
      dval[l] += // (1,1,0)
	u[1].m_v[0] * u[1].m_v[1] * u[0].m_v[2] * pnlst[3]->m_pData[didx[l]];
      dval[l] += // (0,0,1)
	u[0].m_v[0] * u[0].m_v[1] * u[1].m_v[2] * pnlst[4]->m_pData[didx[l]];
      dval[l] += // (1,0,1)
	u[1].m_v[0] * u[0].m_v[1] * u[1].m_v[2] * pnlst[5]->m_pData[didx[l]];
      dval[l] += // (0,1,1)
	u[0].m_v[0] * u[1].m_v[1] * u[1].m_v[2] * pnlst[6]->m_pData[didx[l]];
      dval[l] += // (1,1,1)
	u[1].m_v[0] * u[1].m_v[1] * u[1].m_v[2] * pnlst[7]->m_pData[didx[l]];
    } // end of for(l)

    return true;
  } // end of (nvc==8)

 _CALC_MEAN:
  if ( nvc < 1 ) return false;
  for ( l = 0; l < 3; l++ ) {
    dval[l] = 0.f;
    if ( didx[l] < 0 || didx[l] >= m_dataLen ) continue;
    for ( it = pnmap.begin(); it != pnmap.end(); it++ )
      dval[l] += it->second->m_pData[didx[l]];
    dval[l] /= (float)nvc;
  } // end of for(l)
  return true;
}


// MHIR append begin
const vsnOctTree::Node*
vsnOctTree::GetNode(VSN::PedigType  pedig) const
{
  const Node* result = NULL;

  for (std::set<Node*>::const_iterator it = m_pWholeLst.begin();
       it != m_pWholeLst.end(); ++it) {
    const Node* node = (*it);
    if (node -> m_pedigree == pedig) {
      result = node; break;
    }
  }

  return result;
}

void 
vsnOctTree::Node::AllocDataArea(size_t dataLen)
{
  if (dataLen > 0) {
    if (m_pData) DeAllocate((void*)m_pData);
    m_pData = (float*)CeAllocate(dataLen, sizeof(float));
    assert(m_pData);
  }
}

bool
vsnOctTree::Node::HasData(void) const
{
  return (m_pData != NULL) ? true : false;
}

// MHIR append end
