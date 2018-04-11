//
// vsnXVX
//
#ifndef _VSN_XVX_HPP_
#define _VSN_XVX_HPP_

#include <deque>
#include "utilMath.h"
#include "vsnDataParallelFiles.h"


#ifndef _VSN_OCT_TREE_H_
namespace VSN {
  typedef unsigned long long  PedigType;
};
#endif // _VSN_OCT_TREE_H_

/**
 * XVXクラス
 */
class vsnXVX {
public:

  /**
   * ヘッダー
   */
  class Header {
  public:
    /**
     * 物理量
     */
    struct Phys {
      Phys(int kind, const std::string& name, const std::string& unit)
	: m_kind(kind), m_name(name), m_unit(unit) {}
      /**
       * スカラー、ベクトル、テンソル
       */
      int m_kind;
      /**
       * 物理量名
       */
      std::string m_name;
      /**
       * 単位名
       */
      std::string m_unit;
    };

    Header(void);

    /**
     * ファイルの読み込み
     * @param fp ファイルポインター
     * @exception std::runtime_error エラー
     */
    void ImportFile(FILE* fp);

    int GetRLen(void) const { return m_rLen; }
    int GetStep(void) const { return m_step; }
    float GetTime(void) const { return m_time; }
    const CES::Vec3<size_t>& GetDims(void) const { return m_dims; }
    const CES::Vec3<float>& GetOrg(void) const { return m_org; }
    const CES::Vec3<float>& GetSize(void) const { return m_size; }
    int GetDLen(void) const { return m_dLen; }
    int GetVecLen(void) const { return m_vecLen; }
    size_t GetPhysLen(void) const { return m_physV.size(); }
    const Phys& GetPhys(size_t idx) const { return m_physV[idx]; }

  private:
    /**
     * 実数データのサイズ
     */
    int m_rLen;
    /**
     * ステップ番号
     */
    int m_step;
    /**
     * 時刻
     */
    float m_time;
    /**
     * ルートセルのIJK方向分割数
     */    
    CES::Vec3<size_t> m_dims;
    /**
     * 原点の座標値
     */
    CES::Vec3<float> m_org;
    /**
     * 計算領域のIJK方向の長さ
     */
    CES::Vec3<float> m_size;
    /**
     * データのサイズ
     */
    int m_dLen;
    /**
     * セルで定義される物理量の数
     */
    int m_vecLen;
    /**
     * 物理量
     */
    std::vector<Phys> m_physV;
  };

  /**
   * 構造
   */
  class Struct {
  public:

    /**
     * Leaf
     */
    class LeafInfo {
    public:
      LeafInfo(const CES::Vec3<unsigned short>& pedigree,
	       unsigned char info, bool visible)
	: m_pedigree(pedigree), m_info(info), m_visible(visible) {}

      int GetLevel(void) const { return (m_info >> 3) & 0x1F; }
      int GetPos(void) const { return m_info & 0x7; }

      VSN::PedigType GetPedig(void) const;

      bool GetVisible(void) const { return m_visible; }

    private:
      /**
       * Pedigree
       */
      CES::Vec3<unsigned short> m_pedigree;
      /**
       * Info
       */
      unsigned char m_info;

      bool m_visible;
    };

    /**
     * ルート
     */
    class RootInfo {
    public:
      RootInfo(int rootN) : m_rootN(rootN) {}
      void Add(const LeafInfo& leafInfo) { m_leafInfoV.push_back(leafInfo); }
      size_t GetLeafLen(void) const { return m_leafInfoV.size(); }
      int GetRootN(void) const { return m_rootN; }
      const std::vector<LeafInfo>& GetLeafInfoV(void) const { return m_leafInfoV; }

    private:
      /**
       * ルート番号
       */
      int m_rootN;
      /**
       * Leafリスト
       */
      std::vector<LeafInfo> m_leafInfoV;
    };

    /**
     * コンストラクタ
     */
    Struct(void);

    /**
     * ファイルの読み込み
     * @param fp ファイルポインター
     * @exception std::runtime_error エラー
     * @return 読み込んだリーフの個数
     */
    int ImportFile(FILE* fp, const Header& h, bool visible);

    /**
     * リーフセルの個数
     */
    size_t GetLeafLen(void) const;

    const std::vector<RootInfo>& GetRootInfoV(void) const {
      return m_rootInfoV;
    }

  private:
    std::vector<RootInfo> m_rootInfoV;
  };

  class Data {
  public:
    Data(void);
    void ImportFile(FILE* fp, const Header& header, int leafSize);
    float operator[](size_t idx) const { return m_values[idx]; }
  private:
    std::vector<float> m_values;
  };

  vsnXVX(void);
  void ImportFile(const std::string& fileName, bool data=true);
  void ImportFiles(const std::deque<std::string>& path_list);
  void ImportFiles(const std::vector<VSN::ParallelFileInfo>& path_list);

  void Convert(class vsnOctTree* oct, bool flag=true) const;

private:
  
  Header m_header;
  Struct m_struct;
  Data m_data;
};

#endif // _VSN_XVX_HPP_
