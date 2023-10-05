#ifndef VLD_INTERSECT_PLANE_MAKER_H
#define VLD_INTERSECT_PLANE_MAKER_H

#include <stdexcept>
#include <new>
#include <cmath>

#include "VLDTypes.h"
#include "VLDCellIndexTable.h"
#include "VLDCellFunc.h"

namespace VLD{

class IntersectPlaneMaker{

  Volume _vol;
  // ボリュームデータ

  V3f _plane_normal;
  // Cutting Plane の法線ベクトル

  float _plane_offset;
  // Cutting Plane内の位置と法線ベクトルの内積値
  
public:

  void setData(float thr, Volume::DataType* const data, const V3i &data_size, float cell_scale, const V3f &origin, const V3f &plane_normal, const V3f &plane_pos){
    _vol.thr        = thr;
    _vol.data       = data;
    _vol.data_size  = data_size;
    _vol.cell_scale = cell_scale;

    // 変換：ボリュームデータの原点（0.f, 0.f, 0.f）、セルの大きさ(1.f, 1.f, 1.f)
    this->_plane_normal    = plane_normal.unit();
    this->_plane_normal.x *= _vol.cell_scale;
    this->_plane_normal.y *= _vol.cell_scale;
    this->_plane_normal.z *= _vol.cell_scale;
    this->_plane_offset    = _plane_normal.x * ((plane_pos.x - origin.x) / _vol.cell_scale) +
                             _plane_normal.y * ((plane_pos.y - origin.y) / _vol.cell_scale) +
                             _plane_normal.z * ((plane_pos.z - origin.z) / _vol.cell_scale);
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
  
private:

  void _make(Lines& lines){
    lines.clear();
    const float abs_nx = fabs(_plane_normal.x);
    const float abs_ny = fabs(_plane_normal.y);
    const float abs_nz = fabs(_plane_normal.z);

    const int scx = _vol.data_size.x -1;
    const int scy = _vol.data_size.y -1;
    const int scz = _vol.data_size.z -1;
    float cell_data[8], cell_plane[8];
    CellIsoLines iso_lines;
    CellContours cell_intersections;
    CellIsoCycles iso_cycles;
    if ( abs_nx > abs_ny && abs_nx > abs_nz ) {
      // for x direction
      for ( int cz = 0; cz < scz; ++cz ) {
        for ( int cy = 0; cy < scy; ++cy ) {

          const int vx[4] = {(int)((- _plane_normal.y * (cy    ) - _plane_normal.z * (cz    ) + _plane_offset) / _plane_normal.x),
                             (int)((- _plane_normal.y * (cy + 1) - _plane_normal.z * (cz    ) + _plane_offset) / _plane_normal.x),
                             (int)((- _plane_normal.y * (cy    ) - _plane_normal.z * (cz + 1) + _plane_offset) / _plane_normal.x),
                             (int)((- _plane_normal.y * (cy + 1) - _plane_normal.z * (cz + 1) + _plane_offset) / _plane_normal.x)};
          
          int max_x = vx[0];
          int min_x = vx[0];
          for ( int v = 1; v < 4; ++v ) {
            if ( max_x < vx[v] ) max_x = vx[v];
            if ( min_x > vx[v] ) min_x = vx[v];
          }

          for ( int cx = min_x; cx <= max_x; ++cx ) {
            if ( cx < 0 || cx >= scx ) continue;

            if ( ! _vol.loadSourceData( cx, cy, cz, cell_data) ) continue;
            if ( ! _loadPlaneData( cx, cy, cz, cell_plane) ) continue;
            if ( ! CellFunc::buildIsoLines(cell_data, iso_lines) ) continue;
            CellFunc::buildIsoCycles(iso_lines, iso_cycles);
            if ( ! CellFunc::buildContour(cx, cy, cz, cell_plane, iso_cycles, cell_intersections) ) continue;

            for ( std::size_t seg = 0; seg < cell_intersections.size(); ++seg ) {
              lines.push_back(cell_intersections[seg].begin.pos);
              lines.push_back(cell_intersections[seg].end.pos);
            }
            
          } // end of for(cx)
        } // end fo for(cy)
      } // end of for(cz)
    }
    else if ( abs_ny > abs_nx && abs_ny > abs_nz )  {
      // for y direction
      for ( int cz = 0; cz < scz; ++cz ) {
        for ( int cx = 0; cx < scx; ++cx ) {
          const int vy[4] = {(int)((- _plane_normal.x * (cx    ) - _plane_normal.z * (cz    ) + _plane_offset) / _plane_normal.y),
                             (int)((- _plane_normal.x * (cx + 1) - _plane_normal.z * (cz    ) + _plane_offset) / _plane_normal.y),
                             (int)((- _plane_normal.x * (cx    ) - _plane_normal.z * (cz + 1) + _plane_offset) / _plane_normal.y),
                             (int)((- _plane_normal.x * (cx + 1) - _plane_normal.z * (cz + 1) + _plane_offset) / _plane_normal.y)};
          
          int max_y = vy[0];
          int min_y = vy[0];
          for ( int v = 1; v < 4; ++v ) {
            if ( max_y < vy[v] ) max_y = vy[v];
            if ( min_y > vy[v] ) min_y = vy[v];
          }

          for ( int cy = min_y; cy <= max_y; ++cy ) {
            if ( cy < 0 || cy >= scy ) continue;

            if ( ! _vol.loadSourceData( cx, cy, cz, cell_data) ) continue;
            if ( ! _loadPlaneData( cx, cy, cz, cell_plane) ) continue;
            if ( ! CellFunc::buildIsoLines(cell_data, iso_lines) ) continue;
            CellFunc::buildIsoCycles(iso_lines, iso_cycles);
            if ( ! CellFunc::buildContour(cx, cy, cz, cell_plane, iso_cycles, cell_intersections) ) continue;

            for ( std::size_t seg = 0; seg < cell_intersections.size(); ++seg ) {
              lines.push_back(cell_intersections[seg].begin.pos);
              lines.push_back(cell_intersections[seg].end.pos);
            }

          } // end of for(cy)
        } // end fo for(cx)
      } // end of for(cz)
    }
    else {
      // for z direction
      for ( int cy = 0; cy < scy; ++cy ) {
        for ( int cx = 0; cx < scx; ++cx ) {
          const int vz[4] = {(int)((- _plane_normal.x * (cx    ) - _plane_normal.y * (cy    ) + _plane_offset) / _plane_normal.z),
                             (int)((- _plane_normal.x * (cx + 1) - _plane_normal.y * (cy    ) + _plane_offset) / _plane_normal.z),
                             (int)((- _plane_normal.x * (cx    ) - _plane_normal.y * (cy + 1) + _plane_offset) / _plane_normal.z),
                             (int)((- _plane_normal.x * (cx + 1) - _plane_normal.y * (cy + 1) + _plane_offset) / _plane_normal.z)};
          
          int max_z = vz[0];
          int min_z = vz[0];
          for ( int v = 1; v < 4; ++v ) {
            if ( max_z < vz[v] ) max_z = vz[v];
            if ( min_z > vz[v] ) min_z = vz[v];
          }

          for ( int cz = min_z; cz <= max_z; ++cz ) {
            if ( cz < 0 || cz >= scz ) continue;
          
            if ( ! _vol.loadSourceData( cx, cy, cz, cell_data) ) continue;
            if ( ! _loadPlaneData( cx, cy, cz, cell_plane) ) continue;
            if ( ! CellFunc::buildIsoLines(cell_data, iso_lines) ) continue;
            CellFunc::buildIsoCycles(iso_lines, iso_cycles);
            if ( ! CellFunc::buildContour(cx, cy, cz, cell_plane, iso_cycles, cell_intersections) ) continue;

            for ( std::size_t seg = 0; seg < cell_intersections.size(); ++seg ) {
              lines.push_back(cell_intersections[seg].begin.pos);
              lines.push_back(cell_intersections[seg].end.pos);
            }

          } // end of for(cz)
        } // end fo for(cx)
      } // end of for(cy)
    }
  }


  bool _loadPlaneData(int cx, int cy, int cz, float cell_plane[8]){
    int num_minus = 0, num_plus = 0, num_zero = 0;
    for ( int ver = 0; ver < 8; ++ver ) {
      const int x = cx + CELL_VERTEX[ver][0];
      const int y = cy + CELL_VERTEX[ver][1];
      const int z = cz + CELL_VERTEX[ver][2];

      cell_plane[ver] = (x * _plane_normal.x + y * _plane_normal.y + z * _plane_normal.z) - _plane_offset;
      // iso - surface（line）を含むか否かをチェック
      if ( cell_plane[ver] < 0.f ) ++num_minus;
      else
      if ( cell_plane[ver] > 0.f ) ++num_plus;
      else ++num_zero;
    }
    return ( num_minus < 8 && num_plus < 8 && num_zero < 8 ) ? true : false;
  }

};

}// end of namespace "VLD"

#endif