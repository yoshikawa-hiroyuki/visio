#include <stdexcept>
#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include <iostream>
#include <map>
#include <math.h>

#include "utilMemory.h"

#include "vsnXVX.h"
#include "vsnOctTree.h"

//
// ヘッダー
//

vsnXVX::Header::Header(void)
{
}

void
vsnXVX::Header::ImportFile(FILE* fp)
{
  if (!fp) throw std::runtime_error("XVX::Header:fp is NULL");

  if (fread(&m_rLen, sizeof(int), 1, fp) != 1)
    throw std::runtime_error("XVX:Header:RLen Read Error");
  if (!(m_rLen == 4 || m_rLen == 8))
    throw std::runtime_error("XVX:Header:RLen Invalid Value");

  if (fread(&m_step, sizeof(int), 1, fp) != 1)
    throw std::runtime_error("XVX:Header:Step Read Error");

    if (m_rLen == 4) {
      if (fread(&m_time, sizeof(float), 1, fp) != 1)
	throw std::runtime_error("XVX:Header:Time Read Error");
    } else {
      double tmp;
      if (fread(&tmp, sizeof(double), 1, fp) != 1)
	throw std::runtime_error("XVX:Header:Time Read Error");
      m_time = tmp;
    }

    int dims[3];
    if (fread(dims, sizeof(int), 3, fp) != 3)
      throw std::runtime_error("XVX:Header:Dim Read Error");
    for (int i = 0; i < 3; ++i) {
      if (dims[i] <= 0)
	throw std::runtime_error("XVX:Header:Dim Invalid Value");
      m_dims[i] = dims[i];
      //      m_rootDims[i] = dim[i];
    }

    float org[3];
    if (m_rLen == 4) {
      if (fread(org, sizeof(float), 3, fp) != 3)
	throw std::runtime_error("XVX:Header:Org Read Error");
    } else {
      double tmp[3];
      if (fread(tmp, sizeof(double), 3, fp) != 3)
	throw std::runtime_error("XVX:Header:Org Read Error");
      for (int i = 0; i < 3; ++i) org[i] = tmp[i];
    }
    for (int i = 0; i < 3; ++i) m_org[i] = org[i];
    //    for (int i = 0; i < 3; ++i) m_bbox[0][i] = org[i];

    float size[3];
    if (m_rLen == 4) {
      if (fread(size, sizeof(float), 3, fp) != 3)
	throw std::runtime_error("XVX:Header:Size Read Error");
    } else {
      double tmp[3];
      if (fread(tmp, sizeof(double), 3, fp) != 3)
	throw std::runtime_error("XVX:Header:Size Read Error");
      for (int i = 0; i < 3; ++i) size[i] = tmp[i];
    }
    for (int i = 0; i < 3; ++i) m_size[i] = size[i];
    //    for (int i = 0; i < 3; ++i) m_bbox[1][i] = org[i] + size[i];

#ifdef OLD
    // calc pitch
    Vec3<float> bbLen = m_bbox[1] - m_bbox[0];
    for (int i = 0; i < 3; ++i) {
      m_pitch[i] = bbLen[i] / dim[i];
      if (m_pitch[i] < 1.0E-08)
	throw std::runtime_error("XVX:Header:Pitch Invalid Value");
      std::cout << boost::format("m_pitch[%1%] = %2%") % i % m_pitch[i]
		<< std::endl;
    }
#endif // OLD

    if (fread(&m_dLen, sizeof(int), 1, fp) != 1)
      throw std::runtime_error("XVX:Header:DLen Read Error");
    if (!(m_dLen == 1 || m_dLen == 2 || m_dLen == 4 || m_dLen == 8))
      throw std::runtime_error("XVX:Header:DLen Invalid Value");

    if (fread(&m_vecLen, sizeof(int), 1, fp) != 1)
      throw std::runtime_error("XVX:Header:VecLen Read Error");

#ifdef OLD
    m_dataLen = vecLen;
    m_pMin = (float*)CES::ReAllocate(m_pMin, sizeof(float) * m_dataLen);
    if (!m_pMin)
      throw std::runtime_error("XVX:Header:pMin Can't Allocate Memory");
    m_pMax = (float*)CES::ReAllocate(m_pMax, sizeof(float) * m_dataLen);
    if (!m_pMax)
      throw std::runtime_error("XVX:Header:pMin Can't Allocate Memory");
    for (int i = 0; i < m_dataLen; ++i) {
      m_pMin[i] = MAXFLOAT;
      m_pMax[i] = - MAXFLOAT;
    }

    // Root の作成
    //
    for (int k = 0; k < dim[2]; ++k) {
      for (int j = 0; j < dim[1]; ++j) {
	for (int i = 0; i < dim[0]; ++i) {
	  RootNode* pnode = new RootNode(Vec3<size_t>(i, j, k), m_dataLen);
	  if (! pnode) {
	    Clear();
	    throw
	      std::runtime_error("XVX:Header:RootNode Can't Allocate Memory");
	  }
	  m_pWholeLst.insert(pnode);
	  m_pRootLst.push_back(pnode);
	}
      }
    }
#endif // OLD

    int physNum;
    if (fread(&physNum, sizeof(int), 1, fp) != 1)
      throw std::runtime_error("XVX:Header:PhysNum Read Error");
    const int PHYS_NAME_LEN(16);
    const int PHYS_UNIT_LEN(16);
    for (int i = 0; i < physNum; ++i) {
      int physKind;
      if (fread(&physKind, sizeof(int), 1, fp) != 1)
	throw std::runtime_error("XVX:Header:PhysKind Read Error");
      if (!(physKind == 1 || physKind == 3 || physKind == 9))
	throw std::runtime_error("XVX:Header:PhysKind Invalid Value");

      char physName[PHYS_NAME_LEN + 1];
      if (fread(physName, sizeof(char), PHYS_NAME_LEN, fp) != PHYS_NAME_LEN)
	throw std::runtime_error("XVX:Header:PhysName Read Error");
      physName[PHYS_NAME_LEN] = '\0';

      char physUnit[PHYS_UNIT_LEN + 1];
      if (fread(physUnit, sizeof(char), PHYS_UNIT_LEN, fp) != PHYS_UNIT_LEN)
	throw std::runtime_error("XVX:Header:PhysUnit Read Error");
      physUnit[PHYS_UNIT_LEN] = '\0';

      m_physV.push_back(Phys(physKind, physName, physUnit));
    }
}

vsnXVX::Struct::Struct(void)
{
}

//#define CHECK_PEDIGREE


int 
vsnXVX::Struct::ImportFile(FILE* fp, const Header& h, bool visible)
{
  // リーフ個数
  //
  int result = 0;

  if (!fp) throw std::runtime_error("XVX::Header:fp is NULL");

  const CES::Vec3<size_t>& dims = h.GetDims();

  int rootLen;
  if (fread(&rootLen, sizeof(int), 1, fp) != 1)
    throw std::runtime_error("XVX:Struct:RootLen Read Error");

  for (int i = 0; i < rootLen; ++i) {
    int root;
    if (fread(&root, sizeof(int), 1, fp) != 1)
      throw std::runtime_error("XVX:Struct:Root Read Error");
    if (!(0 <= root && root < (dims[0] * dims[1] * dims[2])))
      throw std::runtime_error("XVX:Struct:Root Invalid Value");

#ifdef CHECK_PEDIGREE
    std::set<VSN::PedigType> pedigCheck;
#endif // CHECK_PEDIGREE

    RootInfo rootInfo(root);

    int leafLen;
    if (fread(&leafLen, sizeof(int), 1, fp) != 1)
      throw std::runtime_error("XVX:Struct:LeafLen Read Error");
    result += leafLen;
    
    for (int j = 0; j < leafLen; ++j) {
      unsigned short pedigree[3];
      if (fread(pedigree, sizeof(short), 3, fp) != 3)
	throw std::runtime_error("XVX:Struct:LeafLen Read Error");
      unsigned char info;
      if (fread(&info, sizeof(char), 1, fp) != 1)
	throw std::runtime_error("XVX:Struct:Info Read Error");

      unsigned int level = (info >> 3) & 0x1F;
      if (!(level <= 16))
	throw std::runtime_error("XVX:struct:Level Invalid Value");

      unsigned int pos = info & 0x7;
      if (!(pos <= 7))
	throw std::runtime_error("XVX:Struct:Position Invalid Value");

      LeafInfo leafInfo(pedigree, info, visible);

#ifdef CHECK_PEDIGREE
      std::pair<std::set<VSN::PedigType>::iterator, bool> result
	= pedigCheck.insert(leafInfo.GetPedig());
      if (! result.second) {
	std::string
	  msg(boost::io::str(boost::format("pedig[0] = %1% pedig[1] = %2% "
					   "pedig[2] = %3% level = %4% "
					   "pos = %5% root = %6%") % \
			     pedigree[0] % pedigree[1] % pedigree[2] % \
			     level % pos % root));
	throw std::runtime_error(msg); //"XVX:Struct:Pedigree duplicate");
      }
#endif // CHECK_PEDIGREE

      rootInfo.Add(leafInfo);

    }

    m_rootInfoV.push_back(rootInfo);
  }

  return result;
}

size_t
vsnXVX::Struct::GetLeafLen(void) const
{
  size_t result = 0;
  
  BOOST_FOREACH(const RootInfo& rootInfo, m_rootInfoV) {
    result += rootInfo.GetLeafLen();
  }

  return result;
}

vsnXVX::Data::Data(void)
{
}

void
vsnXVX::Data::ImportFile(FILE* fp, const Header& h, int leafSize)
{
  //
  // データ
  //
  size_t dataLen = h.GetVecLen();
  size_t dLen = h.GetDLen();

  //  const size_t SIZE(s.GetLeafLen());
  const size_t SIZE(leafSize);

  for (size_t i = 0; i < SIZE; ++i) {

    size_t size = dataLen * dLen;
    if (dLen == 1) {
      unsigned char* data = new unsigned char[dataLen];
      if (! data) throw std::runtime_error("XVX:Data Can't Allocate memory");
      if (fread(data, sizeof(char), dataLen, fp) != dataLen)
	throw std::runtime_error("XVX:Data:Read Error");
      for (int i = 0; i < dataLen; ++i) m_values.push_back((float)data[i]);
      delete[] data;
    } else if (dLen == 2) {
      unsigned short* data = new unsigned short[dataLen];
      if (! data) throw std::runtime_error("XVX:Data Can't Allocate memory");
      if (fread(data, sizeof(short), dataLen, fp) != dataLen)
	throw std::runtime_error("XVX:Data:Read Error");
      for (int i = 0; i < dataLen; ++i) m_values.push_back((float)data[i]);
      delete[] data;
    } else if (dLen == 4) {
      float* data = new float[dataLen];
      if (! data) throw std::runtime_error("XVX:Data Can't Allocate memory");
      if (fread(data, sizeof(float), dataLen, fp) != dataLen)
	throw std::runtime_error("XVX:Data:Read Error");
      for (int i = 0; i < dataLen; ++i) m_values.push_back(data[i]);
      delete[] data;
    } else {
      double* data = new double[dataLen];
      if (! data) throw std::runtime_error("XVX:Data Can't Allocate memory");
      if (fread(data, sizeof(double), dataLen, fp) != dataLen)
	throw std::runtime_error("XVX:Data:Read Error");
      for (int i = 0; i < dataLen; ++i) m_values.push_back((float)data[i]);
      delete[] data;
    }

  }

}

vsnXVX::vsnXVX(void)
{
}

void 
vsnXVX::ImportFiles(const std::deque<std::string>& pathList)
{
  BOOST_FOREACH(const std::string& fileName, pathList) {
    ImportFile(fileName);
  }
}

void 
vsnXVX::ImportFiles(const std::vector<VSN::ParallelFileInfo>& pathList)
{
  BOOST_FOREACH(const VSN::ParallelFileInfo& fileInfo, pathList) {
    ImportFile(fileInfo.first, fileInfo.second);
  }
}

/**
 * @param fileName ファイル名
 * @exception std::runtime_error エラー
 */
void
vsnXVX::ImportFile(const std::string& fileName, bool flag)
{
  FILE* fp = NULL;
  try {
    //    boost::timer timer;
    fp = fopen(fileName.c_str(), "rb");
    if (! fp)
      throw std::runtime_error(boost::io::str
			       (boost::format("Can't Open File : %1%") % \
				fileName));

    // 構造だけを読み込むかどうか
    //
    //    bool flag = true;
    //    if (fileName ==
    //        std::string("/Users/sukemura/Data/5PE_18s/output_002.xvx"))
    //      flag = false;

    // ヘッダーの読み込み
    //
    m_header.ImportFile(fp);
    // 構造の読み込み
    //
    int leafSize = m_struct.ImportFile(fp, m_header, flag);
    // データの読み込み
    //
    if (flag)
      m_data.ImportFile(fp, m_header, leafSize);

  } catch (std::runtime_error& e) {
    if (fp) fclose(fp);
    throw e;
  }

  if (fp) fclose(fp);
}

static std::vector<float>
setValue(vsnOctTree::Node* node, int dataLen, bool flag)
{
  std::vector<float> result;

  if (node -> m_hasChildren) {

    int childNum = 0;

    // 存在する子供のデータだけで平均値を作成する
    //
    for (int i = 0; i < 8; ++i) {
      if (node -> p_children[i] != NULL) {
	std::vector<float> data
	  = setValue(node -> p_children[i], dataLen, flag);
	if (! data.empty()) {
	  if (result.empty()) result = data;
	  else for (int j = 0; j < data.size(); ++j) result[j] += data[j];
	  ++childNum;
	}
      } else {
      }
    }

    // 全ての子供の値を取得できなければ、データ無しと見なす
    //
    if (childNum != 8) result.clear();

    if (! result.empty()) {
      if (!node -> HasData()) node -> AllocDataArea(dataLen);
      for (int i = 0; i < result.size(); ++i) {
	result[i] = result[i] / childNum;
	node -> m_pData[i] = result[i];
      }
    }

  } else {
    if (node -> HasData())
      for (int i = 0; i < dataLen; ++i) result.push_back(node -> m_pData[i]);
  }

  return result;
}

class RootAndPedig {
public:
  RootAndPedig(VSN::PedigType pedig, int rootN)
    : m_pedig(pedig), m_rootN(rootN) {}
  bool operator<(const RootAndPedig& lhs) const {
    if      (m_rootN < lhs.m_rootN) return true;
    else if (m_rootN > lhs.m_rootN) return false;
    else {
      if      (m_pedig < lhs.m_pedig) return true;
      else if (m_pedig > lhs.m_pedig) return false;
      else                            return false;
    }
  }
private:
  VSN::PedigType m_pedig;
  int m_rootN;
};

#ifdef WINDOWS
#define MAXFLOAT DBL_MAX
#endif // WINDOWS

#ifndef MAXFLOAT 
#define MAXFLOAT HUGE
#endif

void
vsnXVX::Convert(vsnOctTree* oct, bool flag) const
{
  const CES::Vec3<float>& org = m_header.GetOrg();
  const CES::Vec3<float>& size = m_header.GetSize();
  const CES::Vec3<size_t>& dims = m_header.GetDims();

  // 次元
  //
  oct -> m_rootDims = dims;
  // 最小値
  //
  oct -> m_bbox[0] = org;
  // 最大値
  //
  for (int i = 0; i < 3; ++i) oct -> m_bbox[1][i] = org[i] + size[i];

  // calc pitch
  CES::Vec3<float> bbLen = (oct -> m_bbox[1]) - (oct -> m_bbox[0]);
  for (int i = 0; i < 3; ++i) {
    oct -> m_pitch[i] = bbLen[i] / dims[i];
    if (oct -> m_pitch[i] < 1.0E-08)
      throw std::runtime_error("XVX:Header:Pitch Invalid Value");
  }

  // セルのデータの個数
  //
  oct -> m_dataLen = m_header.GetVecLen();

  // 最小値・最大値
  //
  oct -> m_pMin
    = (float*)CES::ReAllocate(oct -> m_pMin, sizeof(float) * oct -> m_dataLen);
  if (! oct -> m_pMin)
    throw std::runtime_error("XVX:Header:pMin Can't Allocate Memory");
  oct -> m_pMax
    = (float*)CES::ReAllocate(oct -> m_pMax, sizeof(float) * oct -> m_dataLen);
  if (!oct -> m_pMax)
    throw std::runtime_error("XVX:Header:pMin Can't Allocate Memory");

  for (int i = 0; i < oct -> m_dataLen; ++i) {
    oct -> m_pMin[i] = MAXFLOAT;
    oct -> m_pMax[i] = - MAXFLOAT;
  }

  // Root の作成
  //
  for (int k = 0; k < oct -> m_rootDims[2]; ++k) {
    for (int j = 0; j < oct -> m_rootDims[1]; ++j) {
      for (int i = 0; i < oct -> m_rootDims[0]; ++i) {
	//vsnOctTree::RootNode* pnode
	//  = new vsnOctTree::RootNode(CES::Vec3<size_t>(i, j, k),
	//                             oct -> m_dataLen);
	// 最初はデータ量０で作成する
	//
	vsnOctTree::RootNode* pnode
	  = new vsnOctTree::RootNode(CES::Vec3<size_t>(i, j, k), 0);
	if (! pnode) {
	  oct -> Clear();
	  throw
	    std::runtime_error("XVX:Header:RootNode Can't Allocate Memory");
	}
	oct -> m_pWholeLst.insert(pnode);
	oct -> m_pRootLst.push_back(pnode);
      }
    }
  }

  std::map<RootAndPedig, vsnOctTree::Node*> parentMap;
  std::vector<vsnOctTree::Node*> ll;

  BOOST_FOREACH(const Struct::RootInfo& rootInfo, m_struct.GetRootInfoV()) {
    int rootN = rootInfo.GetRootN();

    vsnOctTree::RootNode* rootNode = NULL;
    {
      // ルートを探す
      //
      int kIdx = rootN / (dims[0] * dims[1]);
      int jIdx = (rootN % (dims[0] * dims[1])) / dims[0];
      int iIdx = (rootN % (dims[0] * dims[1])) % dims[0];

      BOOST_FOREACH(vsnOctTree::RootNode* tmp, oct -> m_pRootLst) {
	CES::Vec3<size_t> idx = tmp -> m_idx;
	if (idx[0] == iIdx && idx[1] == jIdx && idx[2] == kIdx) {
	  rootNode = tmp; break;
	}
      }
    }
    if (!rootNode)
      throw std::runtime_error("XVX:Struct:RootNode Can't Find Root Node");
    //    rootNode -> m_use = true;

    BOOST_FOREACH(const Struct::LeafInfo& leafInfo, rootInfo.GetLeafInfoV()) {
      VSN::PedigType pedig = leafInfo.GetPedig();

      int level = leafInfo.GetLevel();
      if (rootNode -> m_maxLevel < level) rootNode -> m_maxLevel = level;

      vsnOctTree::Node* node = NULL;
      if (pedig == 0) {
	// ルートセルがリーフセルの場合
	//
	node = rootNode;
	if (leafInfo.GetVisible() && !node -> HasData())
	  node -> AllocDataArea(oct -> m_dataLen);
      } else {
	// リーフの作成
	//
	node = new vsnOctTree::Node(leafInfo.GetVisible() ? \
				    oct -> m_dataLen : 0);
	if (! node)
	  throw std::runtime_error("XVX:Struct:Node Can't Allocate Memory");
	//	node -> m_visible = leafInfo.GetVisible();
	node -> m_pedigree = pedig;
	oct -> m_pWholeLst.insert(node);
      }

      // データの設定のために登録しておく
      //
      ll.push_back(node);
      // リーフセルに登録する
      //
      oct -> m_pLeafLst.insert(node);

      if (dynamic_cast<vsnOctTree::RootNode*>(node) == NULL) {
	// ルートでなければ
	//

	// 節の登録
	//
	for (;;) {

	  // 親の Pedigree
	  //
	  VSN::PedigType parent_pedig = pedig / 10;

	  // 親に対する自分の位置
	  //
	  int pos = pedig % 10 - 1;
	  if (!(0 <= pos && pos <= 7))
	    throw std::runtime_error(boost::io::str
				     (boost::format
				      ("XVX:Struct:Position Invalid Value %1%")
				      % pos));

	  if (parent_pedig == 0) {
	    // 親がルートセルであれば
	    // 既に子供が登録されていたら
	    //
	    if (rootNode -> p_children[pos] != NULL) {
	      throw std::runtime_error("XVX:Struct:Root Geom Error");
	    }
	    rootNode -> p_children[pos] = node;
	    node -> p_parent = rootNode;
	    rootNode -> m_hasChildren = true;
	    break;
	  } else {
	    vsnOctTree::Node* parent = NULL;

	    // 親を探す
	    //
	    std::map<RootAndPedig, vsnOctTree::Node*>::iterator it
	      = parentMap.find(RootAndPedig(parent_pedig, rootN));
	    if (it != parentMap.end()) {
	      parent = (*it).second;
	    }

	    if (! parent) {
	      // 親が無ければ作成する
	      //
	      //  parent = new vsnOctTree::Node(oct -> m_dataLen);
	      parent = new vsnOctTree::Node(0);
	      if (! parent)
		throw
		  std::runtime_error("XVX:Struct:Node Can't Allocate Memory");
	      parent -> m_pedigree = parent_pedig;
	      parent -> m_hasChildren = true;
	      // 全体の集合に追加する
	      //
	      oct -> m_pWholeLst.insert(parent);

	      // マップに登録する
	      //
	      parentMap.insert(std::pair<RootAndPedig,
			       vsnOctTree::Node*>(RootAndPedig(parent_pedig,
							       rootN), parent));
	    }

	    // 子供に親を設定する
	    //
	    node -> p_parent = parent;
	    // 親に子供を設定する
	    // 既に子供が登録されていたら
	    //
	    if (parent -> p_children[pos] != NULL) {
	      throw
		std::runtime_error("XVX:Struct:Parent Child Already Regist");
	    }
	    parent -> p_children[pos] = node;

	    // 親が親を持っていれば上の階層を作成する必要がない
	    //
	    if (parent -> p_parent != NULL) break;

	    pedig = parent_pedig;
	    node = parent;
	  } // if
	} // for(;;;)

      } // if

    } // BOOST_FOREACH

  } // BOOST_FOREACH

  // maxLevel
  for (std::deque<vsnOctTree::RootNode*>::iterator it
	 = oct -> m_pRootLst.begin(); it != oct -> m_pRootLst.end(); ++it) {
    if (oct -> m_maxLevel < (*it) -> m_maxLevel)
      oct -> m_maxLevel = (*it) -> m_maxLevel;
  }

  // データ
  //
  size_t idx = 0;
  const size_t SIZE(ll.size());
  for (size_t i = 0; i < SIZE; ++i) {
    vsnOctTree::Node* leafNode = ll[i];
    // 値を持たないセル
    //
    //    if (leafNode -> m_pData == NULL) continue;
    if (! leafNode -> HasData()) continue;
    for (size_t j = 0; j < oct -> m_dataLen; ++j) {
      float data = m_data[idx++];
      leafNode -> m_pData[j] = data;

      // 最小値・最大値の設定
      //
      if (oct -> m_pMin[j] > data) oct -> m_pMin[j] = data;
      if (oct -> m_pMax[j] < data) oct -> m_pMax[j] = data;
    }
  }

  // 節への値の設定
  //
  BOOST_FOREACH(vsnOctTree::RootNode* rootNode, oct -> m_pRootLst) {
    std::vector<float> data = setValue(rootNode, oct -> m_dataLen, flag);
    if (! data.empty()) {
      //if (rootNode -> m_pData == NULL)
      //  rootNode -> AllocDataArea(oct -> m_dataLen);
      if (!rootNode -> HasData()) rootNode -> AllocDataArea(oct -> m_dataLen);
      for (int i = 0; i < data.size(); ++i) {
	rootNode -> m_pData[i] = data[i];
      }
    }
  }

}

VSN::PedigType
vsnXVX::Struct::LeafInfo::GetPedig(void) const 
{
  // Pedigreeは親のPedigreeなので親のレベル
  //
  int parent_level = GetLevel() - 1;

  // 親から見た自分のポジション
  //
  int pos = GetPos();

  // 親の Pedigree
  //
  VSN::PedigType result = 0;

  // 親がルートノードの時は
  //
  if (parent_level == -1) {
    return result;
  }

  for (int k = 1; k <= parent_level; ++k) {
    // レベル0は処理しない
    //
    result *= 10;
    int val = 0;
    //	  int shift = level - k - 1;
    int shift = 16 - k - 1;
    if (m_pedigree[0] & (0x1 << shift)) val += 1;
    if (m_pedigree[1] & (0x1 << shift)) val += 2;
    if (m_pedigree[2] & (0x1 << shift)) val += 4;
    val += 1;
    result += val;
  }

  result = result * 10 + (pos + 1);

  return result;
}
