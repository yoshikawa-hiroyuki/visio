#ifndef VLD_VISIBILITY_TESTER_H
#define VLD_VISIBILITY_TESTER_H

#include <stdexcept>
#include <new>
#include <cmath>

#include "VLDTypes.h"
#include "VLDCellIndexTable.h"

namespace VLD {

class VisibilityTester {
   
  Volume _vol;
  // ボリュームデータ

  V3f _view_pos;
  // 視点位置

  V3f _plane_normal;
  // Cutting Plane の法線ベクトル

  float _plane_offset;
  // Cutting Plane内の位置と法線ベクトルの内積値

  bool _enable_cutting_plane;

public:
  void setData(float thr, Volume::DataType* const data, const V3i &data_size, float cell_scale, const V3f origin, const V3f view_pos, const V3f &plane_normal, const V3f &plane_pos, bool enable_cutting_plane){
    _vol.thr             = thr;
    _vol.data            = data;
    _vol.data_size       = data_size;
    _vol.cell_scale      = cell_scale;
    // 変換：ボリュームデータの原点（0.f, 0.f, 0.f）、セルの大きさ(1.f, 1.f, 1.f)
    this->_view_pos        = view_pos - origin;
    this->_view_pos.x     /= _vol.cell_scale;
    this->_view_pos.y     /= _vol.cell_scale;
    this->_view_pos.z     /= _vol.cell_scale;
    // 変換：ボリュームデータの原点（0.f, 0.f, 0.f）、セルの大きさ(1.f, 1.f, 1.f)
    this->_plane_normal    = plane_normal.unit();
    this->_plane_normal.x *= _vol.cell_scale;
    this->_plane_normal.y *= _vol.cell_scale;
    this->_plane_normal.z *= _vol.cell_scale;
    // 変換：ボリュームデータの原点（0.f, 0.f, 0.f）、セルの大きさ(1.f, 1.f, 1.f)
    this->_plane_offset    = _plane_normal.x * ((plane_pos.x - origin.x) / _vol.cell_scale) +
                             _plane_normal.y * ((plane_pos.y - origin.y) / _vol.cell_scale) +
                             _plane_normal.z * ((plane_pos.z - origin.z) / _vol.cell_scale);

    this->_enable_cutting_plane = enable_cutting_plane;
  }

  bool testIsoValue(const Lines &lines, Lines &visible_lines) {
    if ( ! _vol.data ) return false;
    try {
      _testIsoValue(lines, visible_lines);
      return true;
    }
    catch (const std::bad_alloc &e) {
      return false;
    }
    catch (const std::logic_error &e) {
      return false;
    }

  }

  bool testCuttingPlane(const Lines &lines, Lines &visible_lines) {
    if ( ! _vol.data ) return false;
    try {
      _testCuttingPlane(lines, visible_lines);
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

  void _testIsoValue(const Lines &lines, Lines &visible_lines) {
    visible_lines.reserve(lines.size());
    
    const int sv = (int)lines.size();
    const int sl = sv / 2;
    for ( int seg = 0; seg < sl; ++seg ) {
      const int begin = seg * 2;
      const int end   = begin + 1;
      if ( _isVisible(&lines[begin][0]) && _isVisible(&lines[end][0]) ) {
        visible_lines.push_back(lines[begin]);
        visible_lines.push_back(lines[end]);
      }

    }
  }

  void _testCuttingPlane(const Lines &lines, Lines &visible_lines) {
    visible_lines.reserve(lines.size());

    const int sv = (int)lines.size();
    const int sl = sv / 2;
    for ( int seg = 0; seg < sl; ++seg ) {
      const int begin = seg * 2;
      const int end   = begin + 1;
      V3f b_ver = lines[begin];
      V3f e_ver = lines[end];
      if ( _isDown( &b_ver[0], &e_ver[0] ) ) {
        visible_lines.push_back(b_ver);
        visible_lines.push_back(e_ver);
      }
    }
  }

  bool _isVisible(const float begin[3]){

    // どちらの方向に向かって進むかを決定
    const int d_cx = ( begin[0] < _view_pos[0] ) ? 1 : ( begin[0] > _view_pos[0] ) ? - 1 : 0;
    const int d_cy = ( begin[1] < _view_pos[1] ) ? 1 : ( begin[1] > _view_pos[1] ) ? - 1 : 0;
    const int d_cz = ( begin[2] < _view_pos[2] ) ? 1 : ( begin[2] > _view_pos[2] ) ? - 1 : 0;

    const int face[3] = {( d_cx < 0 ) ? 0 : 1,
                         ( d_cy < 0 ) ? 2 : 3,
                         ( d_cz < 0 ) ? 4 : 5};

    const float vec_eb[3] = {_view_pos[0] - begin[0],
                             _view_pos[1] - begin[1],
                             _view_pos[2] - begin[2]};

    const float abs_eb[3] = {fabs(vec_eb[0]),
                             fabs(vec_eb[1]),
                             fabs(vec_eb[2])};

    int cx = (int)(begin[0]);
    int cy = (int)(begin[1]);
    int cz = (int)(begin[2]);

    const float min_x = (float)cx;
    const float max_x = min_x + 1.f;
    const float min_y = (float)cy;
    const float max_y = min_y + 1.f;
    const float min_z = (float)cz;
    const float max_z = min_z + 1.f;
    float t[3] = {( ( begin[0] < _view_pos[0] ) ? (max_x - begin[0]) : (begin[0] - min_x) ) / abs_eb[0],
                  ( ( begin[1] < _view_pos[1] ) ? (max_y - begin[1]) : (begin[1] - min_y) ) / abs_eb[1],
                  ( ( begin[2] < _view_pos[2] ) ? (max_z - begin[2]) : (begin[2] - min_z) ) / abs_eb[2]};

    const float d_x = 1.f / abs_eb[0];
    const float d_y = 1.f / abs_eb[1];
    const float d_z = 1.f / abs_eb[2];

    const int end_x = (int)_view_pos[0];
    const int end_y = (int)_view_pos[1];
    const int end_z = (int)_view_pos[2];
    const int scx   = _vol.data_size.x - 1;
    const int scy   = _vol.data_size.y - 1;
    const int scz   = _vol.data_size.z - 1;

    int count = 0;
    int begin_sign = 0, cur_sign = 0;
    for ( ; ; ) {
      if ( cx == end_x &&
           cy == end_y &&
           cz == end_z    ) return true;

      if ( t[0] <= t[1] && t[0] <= t[2] ) {
        t[0] += d_x;
        cx  += d_cx;
      } 
      else
      if ( t[1] <= t[0] && t[1] <= t[2] ) {
        t[1] += d_y;
        cy  += d_cy;
      }
      else {
        t[2] += d_z;
        cz  += d_cz;
      }

      if ( count++ < 2  ) continue;

      if ( cx < 0 || cx >= scx ||
           cy < 0 || cy >= scy ||
           cz < 0 || cz >= scz    ) return true;


      const int direction = ( t[0] <= t[1] && t[0] <= t[2] ) ? 0 : ( t[1] <= t[0] && t[1] <= t[2] ) ? 1 : 2;
      const float pos[3] = {begin[0] + vec_eb[0] * t[direction],
                            begin[1] + vec_eb[1] * t[direction],
                            begin[2] + vec_eb[2] * t[direction]};

      cur_sign = _signIsoValue(cx, cy, cz, face[direction], pos);
      // 符号が代わった場合、見えないものとする
      if( cur_sign * begin_sign < 0 ) return false;
      begin_sign = cur_sign;

      // cutting planeに達した場合は、見えるものとする
      if ( _enable_cutting_plane ) 
        if ( (pos[0] * _plane_normal.x + 
              pos[1] * _plane_normal.y + 
              pos[2] * _plane_normal.z -
              _plane_offset) > 0.f ) 
          return true;

    } // end of cell visit

    return true;
  }

  int _signIsoValue(int cx, int cy, int cz, int face, const float pos[3]){

    const int u_compornent = EDGE_DIRECTION[face][0];
    const int v_compornent = EDGE_DIRECTION[face][1];
    const float u = pos[u_compornent] - (int)pos[u_compornent];
    const float v = pos[v_compornent] - (int)pos[v_compornent];

    const int ver0 = VERTEX_INDEX[face][0][BEGIN];
    const int ver1 = VERTEX_INDEX[face][1][BEGIN];
    const int ver2 = VERTEX_INDEX[face][2][BEGIN];
    const int ver3 = VERTEX_INDEX[face][3][BEGIN];

    const int sxy  = _vol.data_size.x * _vol.data_size.y;
    const int sx   = _vol.data_size.x;
    const int base = ( sxy * cz + sx * cy ) + cx;
    const float val0 = _vol.data[base + ((ver0 >> 2) & 0x00000001) * sxy + ((ver0 >> 1) & 0x00000001) * sx + (ver0 & 0x00000001) ] - _vol.thr;
    const float val1 = _vol.data[base + ((ver1 >> 2) & 0x00000001) * sxy + ((ver1 >> 1) & 0x00000001) * sx + (ver1 & 0x00000001) ] - _vol.thr;
    const float val2 = _vol.data[base + ((ver2 >> 2) & 0x00000001) * sxy + ((ver2 >> 1) & 0x00000001) * sx + (ver2 & 0x00000001) ] - _vol.thr;
    const float val3 = _vol.data[base + ((ver3 >> 2) & 0x00000001) * sxy + ((ver3 >> 1) & 0x00000001) * sx + (ver3 & 0x00000001) ] - _vol.thr;
    const float val  = val0 * (1.f - u) * (1.f - v) + val1 * u * (1.f - v) + val3 * (1.f - u) * v + val2 * u * v;
    // 0.fは、プラスとする
    return ( val < 0.f ) ? -1 : 1;
  }

  bool _isDown(float begin[3], float end[3]) {
    const float b_dot = begin[0] * _plane_normal.x + 
                        begin[1] * _plane_normal.y + 
                        begin[2] * _plane_normal.z - 
                        _plane_offset;

    const float e_dot = end[0] * _plane_normal.x + 
                        end[1] * _plane_normal.y + 
                        end[2] * _plane_normal.z - 
                        _plane_offset;

    if ( b_dot >= 0.f && e_dot >= 0.f ) return false;
    else 
    if ( b_dot  < 0.f && e_dot  < 0.f ) return true;
    else
    if ( b_dot  < 0.f && e_dot >= 0.f ) {
      const float factor = fabs( e_dot / (b_dot - e_dot) );
      end[0] += (begin[0] - end[0]) * factor;        
      end[1] += (begin[1] - end[1]) * factor;
      end[2] += (begin[2] - end[2]) * factor;
      return true;
    }
    else {
      const float factor = fabs( b_dot / (b_dot - e_dot) );       
      begin[0] += (end[0] - begin[0]) * factor;        
      begin[1] += (end[1] - begin[1]) * factor;
      begin[2] += (end[2] - begin[2]) * factor;
      return true;
    }
  }

};

} // end of namespace VLD

#endif