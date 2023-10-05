#ifndef VLD_TYPES_H
#define VLD_TYPES_H

#include <vector>

#include <gears/geometry/vector3.h>
#include <gears/geometry/vector4.h>

#include <gears/geometry/matrix22.h>
#include <gears/geometry/matrix44.h>

namespace VLD{

const float ACCEPTABLE_DELTA = 0.000001f;

const std::size_t MAX_CELL_ISO_POINTS = 12;
const std::size_t MAX_CELL_ISO_LINES  = 12;
const std::size_t MAX_CELL_ISO_CYCLES =  4;
const std::size_t MAX_CELL_CONTOURS   = 12;

const float INVALID_POS  = -1.f; // 各セルの、contourの計算は、ローカル座標系（0.f <= x, y, z <= 1.f）で行われる
const int INVALID_EDGE    = -1; // セルの辺のインデックスは、0 <= edge < 12の範囲
const int INVALID_FACE    = -1; // セルの面のインデックスは、0 <= face < 6の範囲

enum EDGE_POINT{BEGIN = 0, END};
enum XYZ{X = 0, Y, Z};

// Contour::makeBySeedTraverseMethodで使用（セルの状態を示す）
const unsigned char NOT_SEED_CELL = 0x00000000;
const unsigned char SEED_CELL     = 0x00000001;
typedef std::vector<unsigned char> CellState;

enum INTERSECT_STATE{ UP_BOTH = 0, DOWN_BOTH, DOWN_BEGIN, DOWN_END};

namespace gg = gears::geometry;
typedef gg::vector3<float> V3f;
typedef gg::vector3<int> V3i;
typedef gg::vector4<float> V4f;
typedef gg::matrix44<float> M44f;

typedef std::vector<V3i> CellIndices;
typedef std::vector<V3f> Lines; 

struct Volume{
  typedef float DataType;

  float thr;
  // 閾値

  DataType* data;
  // 参照データ

  V3i data_size;
  // 参照データのサイズ
 
  float cell_scale;
  // セルの大きさ（スケール）

  Volume():data(0){}

  bool loadSourceData(int cx, int cy, int cz, float cell_val[8]){
    // この時点で、データからthresholdを引いておく
    const int sxy  = data_size.x * data_size.y;
    const int sx   = data_size.x;
    const int base = ( sxy * cz + sx * cy ) + cx;
    cell_val[0] = data[base] - thr;
    cell_val[1] = data[base +1] - thr;
    cell_val[2] = data[base +sx] - thr;
    cell_val[3] = data[base +sx +1] - thr;

    cell_val[4] = data[base +sxy] - thr;
    cell_val[5] = data[base +1  +sxy] - thr;
    cell_val[6] = data[base +sx +sxy] - thr;
    cell_val[7] = data[base +sx +1 +sxy] - thr;

    // iso - surface（line）を含むか否かをチェック
    int num_minus = 0, num_plus = 0, num_zero = 0;
    for ( int ver = 0; ver < 8; ++ver ) {
      if ( cell_val[ver] < 0.f )  ++num_minus;
      else 
      if ( cell_val[ver] > 0.f ) ++num_plus;
      else ++num_zero;
    }
    return ( num_minus < 8 && num_plus < 8 && num_zero < 8 ) ? true : false;
  }
};

struct IsoPoint{
  
  enum LEVEL{NO_ISO_POINT = 0, MINUS = -1, PLUS = 1};

  V3f pos;
  LEVEL level;
  int belong_face;
  int belong_edge;

  IsoPoint():pos(INVALID_POS, INVALID_POS, INVALID_POS), level(NO_ISO_POINT), belong_face(INVALID_FACE), belong_edge(INVALID_EDGE){}
  IsoPoint(const V3f &p, LEVEL l, int f, int e): pos(p), level(l), belong_face(f), belong_edge(e){}

};

struct IsoLine{
  IsoPoint begin, end;
  IsoLine(): begin(), end(){}
  IsoLine(const IsoPoint &b, const IsoPoint &e): begin(b), end(e){}
  IsoLine(const V3f &b_p, IsoPoint::LEVEL b_l, int b_f, int b_e,
          const V3f &e_p, IsoPoint::LEVEL e_l, int e_f, int e_e):
  begin(b_p, b_l, b_f, b_e), end(e_p, e_l, e_f, e_e){}

};

struct CellFace{
  int num_iso_point;
  IsoPoint edge_point[4];
  CellFace(): num_iso_point(0){}
};

typedef std::vector<IsoLine> IsoLines;

// 高速かのために実装した、stlのvectorと似たインターフェイスを持つクラス
// ただし、要素は固定長配列
template<typename ElemType, std::size_t _max_size>
class LimitedVector{

  ElemType _elem[_max_size];
  std::size_t _size;
 
public:   
  typedef ElemType* iterator;
  typedef const ElemType* const_iterator;

public:
  LimitedVector():_size(0){}

  ElemType& operator[](size_t i){return _elem[i];}
  const ElemType& operator[](size_t i) const {return _elem[i];}

  ElemType& front(){return _elem[0];}
  const ElemType& front() const {return _elem[0];}

  ElemType& back(){return _elem[_size -1];}
  const ElemType& back() const {return _elem[_size -1];}

  const_iterator begin() const { return _elem; }
  iterator begin(){ return _elem; }

  const_iterator end() const { return _elem + _size; }
  iterator end(){ return _elem + _size; }

  void push_back(const ElemType& e){
    _elem[_size] = e;
    ++_size;
  }

  void pop_back(){if ( _max_size > 0 ) --_size;}

  size_t size() const {return _size;}
  size_t max_size() const {return _max_size;}
  bool empty() const {return ( _size > 0 ) ? false : true;}

  iterator erase(iterator pos){
    for ( iterator it = pos; it + 1 !=  end(); ++it ) *it = *(it + 1);
    --_size;
    return pos;
  }

  void clear(){_size = 0;}
};

typedef LimitedVector<IsoPoint,     MAX_CELL_ISO_POINTS> CellIsoPoints;
typedef LimitedVector<IsoLine,      MAX_CELL_ISO_LINES > CellIsoLines;
typedef LimitedVector<CellIsoLines, MAX_CELL_ISO_CYCLES> CellIsoCycles;
typedef LimitedVector<IsoLine,      MAX_CELL_CONTOURS  > CellContours;

}// end of namespace "VLD"

#endif
