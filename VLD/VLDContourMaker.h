#ifndef VLD_CONTOUR_MAKER_H
#define VLD_CONTOUR_MAKER_H

#include <stdexcept>
#include <new>

#include "VLDTypes.h"
#include "VLDCellIndexTable.h"
#include "VLDCellFunc.h"

namespace VLD{

class ContourMaker{

  Volume _vol;
  // ボリュームデータ

  V3f _view_pos;
  // 視点位置

  CellIndices _seed_cells;
  // seed traverse methodにおいて、検索起点となるセル

  CellState _seed_state;
  // 各セルがseedセルか否かの状態
  
public:

  void setData(float thr, Volume::DataType* const data, const V3i &data_size, float cell_scale, const V3f &origin, const V3f &view_pos){
    _vol.thr        = thr;
    _vol.data       = data;
    _vol.data_size  = data_size;
    _vol.cell_scale = cell_scale;

    // 変換：ボリュームデータの原点（0.f, 0.f, 0.f）
    this->_view_pos = view_pos - origin;
  }

  bool make(Lines& lines){
    if ( ! _vol.data ) return false;
    try {
      _make(lines);
      return true;
    }
    catch (const std::bad_alloc &e) {
      return false;
    }
    catch (const std::logic_error &e) {
      return false;
    }
  }

  bool makeBySeedTraverseMethod(Lines& lines){
    if ( ! _vol.data ) return false;
    try {
      _makeBySeedTraverseMethod(lines);
      return true;
    }
    catch (const std::bad_alloc &e) {
      return false;
    }
    catch (const std::logic_error &e) {
      return false;
    }
  }
 
private:

  void _make(Lines &lines){
    lines.clear();

    const int scx = _vol.data_size.x -1;
    const int scy = _vol.data_size.y -1;
    const int scz = _vol.data_size.z -1;
    const int scxy = scx * scy;

    _seed_cells.clear();
    _seed_state.clear();
    _seed_state.resize(scx * scy * scz, NOT_SEED_CELL);
      
    float cell_data[8], cell_nv[8];
    CellIsoLines iso_lines;
    CellContours cell_contours;
    CellIsoCycles iso_cycles;
    for ( int cz = 0; cz < scz; ++cz ) {
      for ( int cy = 0; cy < scy; ++cy ) {
        for ( int cx = 0; cx < scx; ++cx ) {

          if ( ! _vol.loadSourceData( cx, cy, cz, cell_data) ) continue;
          if ( ! _loadNnormalDotViewData( cx, cy, cz, cell_nv) ) continue;

          if ( ! CellFunc::buildIsoLines(cell_data, iso_lines) ) continue;
          CellFunc::buildIsoCycles(iso_lines, iso_cycles);
          if ( ! CellFunc::buildContour(cx, cy, cz, cell_nv, iso_cycles, cell_contours) ) continue;

          for ( std::size_t seg = 0; seg < cell_contours.size(); ++seg ) {
            lines.push_back(cell_contours[seg].begin.pos );
            lines.push_back(cell_contours[seg].end.pos );
          }

          _seed_state[scxy * cz + scx * cy + cx] = SEED_CELL;
          _seed_cells.push_back(V3i(cx, cy, cz));
          
        } // end of for(cx) 
      } // end of for(cy)
    } // end of for(cz)
  }

  void _makeBySeedTraverseMethod(Lines& lines){
    lines.clear();
    lines.reserve(_seed_cells.size() * 4);

    const int scx = _vol.data_size.x - 1;
    const int scy = _vol.data_size.y - 1;
    const int scz = _vol.data_size.z - 1;
    const int scxy = scx * scy;

    CellIsoLines iso_lines;
    CellContours cell_contours;
    CellIsoCycles iso_cycles;
    CellIndices new_seeds;
    new_seeds.reserve(_seed_cells.size() * 2);
    for ( std::size_t seed = 0; seed < _seed_cells.size(); ++seed ) {

      const V3i seed_cell = _seed_cells[seed];
      const int ic = scxy * seed_cell.z + scx * seed_cell.y + seed_cell.x;

      // SEEDセルでないとする
      _seed_state[ic] = NOT_SEED_CELL;

      // コンタの作成
      float cell_data[8], cell_nv[8];
      if ( ! _vol.loadSourceData(seed_cell.x, seed_cell.y, seed_cell.z, cell_data) ) continue;
      if ( ! _loadNnormalDotViewData(seed_cell.x, seed_cell.y, seed_cell.z, cell_nv) ) continue;
      if ( ! CellFunc::buildIsoLines(cell_data, iso_lines) ) continue;

      CellFunc::buildIsoCycles(iso_lines, iso_cycles);
      if ( ! CellFunc::buildContour(seed_cell.x, seed_cell.y, seed_cell.z, cell_nv, iso_cycles, cell_contours) ) continue;
      
      for ( std::size_t seg = 0; seg < cell_contours.size(); ++seg ) {
        lines.push_back(cell_contours[seg].begin.pos);
        lines.push_back(cell_contours[seg].end.pos);
      }
      
      // 新たなSEEDセルを追加
      new_seeds.push_back(seed_cell);
      // SEEDセルとする
      _seed_state[ic] = SEED_CELL;

      for ( std::size_t seg = 0; seg < cell_contours.size(); ++seg ) {
        // 始点に関して
        const V3i b_neighbor(seed_cell.x + FACE_NORMAL[cell_contours[seg].begin.belong_face][0],
                             seed_cell.y + FACE_NORMAL[cell_contours[seg].begin.belong_face][1],
                             seed_cell.z + FACE_NORMAL[cell_contours[seg].begin.belong_face][2]);
        // グリッドの境界に達していない場合
        if ( b_neighbor.x <  scx && b_neighbor.y <  scy && b_neighbor.z <  scz &&
             b_neighbor.x >= 0   && b_neighbor.y >= 0   && b_neighbor.z >= 0      ) {
          const int ic_b = scxy * b_neighbor.z + scx * b_neighbor.y + b_neighbor.x;
          // SEEDセルでない場合
          if ( !_seed_state[ic_b] ) {
            _seed_cells.push_back(b_neighbor);
            _seed_state[ic_b] = SEED_CELL;
          }
        }
        // 終点に関して
        const V3i e_neighbor(seed_cell.x + FACE_NORMAL[cell_contours[seg].end.belong_face][0],
                             seed_cell.y + FACE_NORMAL[cell_contours[seg].end.belong_face][1],
                             seed_cell.z + FACE_NORMAL[cell_contours[seg].end.belong_face][2]);
        // グリッドの境界に達していない場合
        if ( e_neighbor.x <  scx && e_neighbor.y <  scy && e_neighbor.z <  scz &&
             e_neighbor.x >= 0   && e_neighbor.y >= 0   && e_neighbor.z >= 0      ) {
          const int ic_e = scxy * e_neighbor.z + scx * e_neighbor.y + e_neighbor.x;
          // SEEDセルでない場合
          if ( !_seed_state[ic_e] ) {
            _seed_cells.push_back(e_neighbor);
            _seed_state[ic_e] = SEED_CELL;
          }
        } 

      }
    }
    // seedセルの更新
    _seed_cells = new_seeds;
  }


  bool _loadNnormalDotViewData(int cx, int cy, int cz, float cell_nv[8]){

    // iso - surface（line）を含むか否かをチェック     
    int num_minus = 0, num_plus = 0, num_zero = 0;
    for ( int ver = 0; ver < 8; ++ver ) {
      const int x = cx + CELL_VERTEX[ver][0];
      const int y = cy + CELL_VERTEX[ver][1];
      const int z = cz + CELL_VERTEX[ver][2];

      const V3f normal = _getNormal(x, y, z);
      const V3f view(_view_pos.x - x * _vol.cell_scale,
                     _view_pos.y - y * _vol.cell_scale,
                     _view_pos.z - z * _vol.cell_scale);

      cell_nv[ver] = gg::dot(normal, view);

      if ( cell_nv[ver] < 0.f ) ++num_minus;
      else
      if ( cell_nv[ver] > 0.f ) ++num_plus;
      else ++num_zero;
    }

    return ( num_minus < 8 && num_plus < 8 && num_zero < 8 ) ? true : false;
  }

  V3f _getNormal(int x, int y, int z){
    const int bx = ( x - 1 >= 0 ) ? x - 1 : 0;
    const int by = ( y - 1 >= 0 ) ? y - 1 : 0;
    const int bz = ( z - 1 >= 0 ) ? z - 1 : 0;
    
    const int fx = ( x + 1 < _vol.data_size.x ) ? x + 1 : _vol.data_size.x - 1;
    const int fy = ( y + 1 < _vol.data_size.y ) ? y + 1 : _vol.data_size.y - 1;
    const int fz = ( z + 1 < _vol.data_size.z ) ? z + 1 : _vol.data_size.z - 1;

    const int sxy = _vol.data_size.x * _vol.data_size.y;
    const int sx  = _vol.data_size.x;
    const float dif_x = (float)_vol.data[sxy*z  +sx*y  +fx] - _vol.data[sxy*z  +sx*y  +bx];
    const float dif_y = (float)_vol.data[sxy*z  +sx*fy +x ] - _vol.data[sxy*z  +sx*by +x ];
    const float dif_z = (float)_vol.data[sxy*fz +sx*y  +x ] - _vol.data[sxy*bz +sx*y  +x ];

    const float dx = (fx - bx) * _vol.cell_scale;
    const float dy = (fy - by) * _vol.cell_scale;
    const float dz = (fz - bz) * _vol.cell_scale;

    return V3f(- dif_x / dx, - dif_y / dy, - dif_z / dz);
  }

};



}// end of namespace "VLD"

#endif