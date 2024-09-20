//
// vsnOctTree
//
#ifndef _VSN_OCT_TREE_H_
#define _VSN_OCT_TREE_H_

#include "utilMath.h"
#include <assert.h>
#include <deque>
#include <set>
#include <sstream>

namespace VSN {
  typedef unsigned long long  PedigType;

  inline std::istream&
    operator >>(std::istream& is, CES::Vec3<int>& v) {
    is >> v.m_v[0];
    if ( is.get() == '/' ) {
      if ( is.peek() != '/' ) {
	is >> v.m_v[1];
      }
      if ( is.get() == '/' ) {
	is >> v.m_v[2];
      }
    }
    return is;
  }

};


//----------------------------------------------------------------
// class vsnOctTree
//----------------------------------------------------------------
class vsnOctTree {
public:

  // struct Node
  //----------------------------------------------------------------
  struct Node {
    Node(const size_t dlen =0);
    virtual ~Node();

    bool ImportFile(FILE* ifp, bool& flread, class vsnOctTree& ot,
		    const bool needEc = false);
    bool ExportFile(FILE* ofp, class vsnOctTree& ot);
    Node* FindByPos(const CES::Vec3<float>& pos, const CES::Vec3<float>& orig,
		    const CES::Vec3<float>& size, const int level =-1);
    Node* FindByPos(const CES::Vec3<double>& pos,const CES::Vec3<double>& orig,
		    const CES::Vec3<double>& size, const int level =-1);
    size_t GetLevel() const;

    // MHIR append begin
    /**
     * データ領域を確保する
     * @param dataLen 領域の長さ（データの個数）
     * @author MHIR
     */
    void AllocDataArea(size_t dataLen);

    /**
     * データを持っているか
     * @retval true データを持っている（m_pData が NULL でない）
     * @retval false データを持っていない（m_pData が NULL である）
     * @author MHIR
     */
    bool HasData(void) const;
    // MHIR append end

    VSN::PedigType m_pedigree; // 0 as root, [1..8]+[1..8]*10+...
    bool    m_hasChildren;
    Node*   p_parent;
    Node*   p_children[8];
    float*  m_pData;
  };

  // struct RootNode
  //----------------------------------------------------------------
  struct RootNode : public Node {
    RootNode(const CES::Vec3<size_t>& idx, const size_t dlen =0)
      : Node(dlen), m_idx(idx), m_maxLevel(0) {}

    CES::Vec3<size_t> m_idx;
    size_t            m_maxLevel;
  };

  // methods
  //----------------------------------------------------------------
  vsnOctTree();
  virtual ~vsnOctTree();


  void Clear();
  bool ImportFile(const char* fname);
  bool ExportFile(const char* fname);

  RootNode* GetRootNode(const size_t I, const size_t J, const size_t K);
  Node* FindByPos(const CES::Vec3<float>& pos, const int level =-1);
  Node* FindByPos(const CES::Vec3<double>& pos, const int level =-1);
  bool  GetMinMax(const size_t didx, float minmax[2]) const;

  bool AlcChildren(Node* pn);
  bool DelChildren(Node* pn);

  Node* GetNeighbor(Node* pn, const CES::Vec3<int>& dir);
  bool InterpolateData(const CES::Vec3<float>& pos, Node* pn,
		       const size_t didx, float& dval);
  bool InterpolateData(const CES::Vec3<float>& pos, Node* pn,
		       const CES::Vec3<int>& didx, CES::Vec3<float>& dval);

  bool TriLinear_Interp(const CES::Vec3<float>& x0,
			const CES::Vec3<float>& sz,
			const CES::Vec3<float>& pos,
			const CES::Vec3<int>& didx,
			Node* pnlst[8],
			CES::Vec3<float>& dval);
  bool CoLat_Interp(const CES::Vec3<float>& x0, const CES::Vec3<float>& sz,
		    const CES::Vec3<float>& pos, const CES::Vec3<int>& didx,
		    Node* pnlst[8], CES::Vec3<float>& dval);

  // members
  //----------------------------------------------------------------
  CES::Vec3<size_t>  m_rootDims;
  CES::Vec3<float>   m_bbox[2];
  CES::Vec3<float>   m_pitch;
  size_t             m_dataLen;
  size_t             m_maxLevel;
  float*             m_pMin; // [m_dataLen]
  float*             m_pMax; // [m_dataLen]

  std::set<Node*>    m_pWholeLst;
  std::deque<RootNode*>
                     m_pRootLst;
  std::set<Node*>    m_pLeafLst;

  /**
   * ペディグリーからノードを取得する
   * @param pedig ペディグリー
   * @return ノード（無ければNULL）
   * @author MHIR
   */
  const Node* GetNode(VSN::PedigType  pedig) const;

private:
  inline size_t RIDX(const size_t i, const size_t j, const size_t k) const {
    return (m_rootDims[0]*m_rootDims[1]*k + m_rootDims[0]*j + i);
  }
};

#endif // _VSN_OCT_TREE_H_
