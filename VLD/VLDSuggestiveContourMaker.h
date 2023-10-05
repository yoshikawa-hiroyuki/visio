#ifndef VLD_SUGGESTIVE_CONTOUR_MAKER_H
#define VLD_SUGGESTIVE_CONTOUR_MAKER_H

#include <stdexcept>
#include <new>
#include <cmath>

#include "VLDTypes.h"
#include "VLDCellIndexTable.h"
#include "VLDCellFunc.h"

namespace VLD{

class SuggestiveContourMaker{

  Volume _vol;
  // ボリュームデータ

  V3f _view_pos;
  // 視点位置

  const float _eps;
  // suggestive contour(Ds)の判定の際の基準値

public:

  SuggestiveContourMaker():_eps(0.f){}

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
  
private:

  void _make(Lines& lines){
    lines.clear();

    const int scx = _vol.data_size.x -1;
    const int scy = _vol.data_size.y -1;
    const int scz = _vol.data_size.z -1;
    const int scxy = scx * scy;

    float cell_data[8], cell_s[8], cell_ds[8];
    float cell_view_l2[8], cell_nv[8];
    V3f cell_view[8];
    CellIsoLines iso_lines;
    CellContours cell_suggestives;
    CellIsoCycles iso_cycles;
    for ( int cz = 0; cz < scz; ++cz ) {
      for ( int cy = 0; cy < scy; ++cy ) {
        for ( int cx = 0; cx < scx; ++cx ) {
          
          if ( ! _vol.loadSourceData( cx, cy, cz, cell_data) ) continue;
          if ( ! _loadSuggestiveData( cx, cy, cz, cell_view, cell_view_l2, cell_nv, cell_s) ) continue;

          if ( ! CellFunc::buildIsoLines(cell_data, iso_lines) ) continue;
          CellFunc::buildIsoCycles(iso_lines, iso_cycles);
          if ( ! CellFunc::buildContour(cx, cy, cz, cell_s, iso_cycles, cell_suggestives) ) continue;

          _loadDsData(cx, cy, cz, cell_s, cell_view, cell_view_l2, cell_ds);

          const float eps = 0.f;
          for ( std::size_t seg = 0; seg < cell_suggestives.size(); ++seg ) {
            if ( _isSuggestiveContour(cell_suggestives[seg], cell_ds, cell_nv) ) {
              lines.push_back(cell_suggestives[seg].begin.pos);
              lines.push_back(cell_suggestives[seg].end.pos);
            }
          }

        } // end of for(cx) 
      } // end of for(cy)
    } // end of for(cz)
  }

  
  bool _loadSuggestiveData(int cx, int cy, int cz, V3f cell_view[8], float cell_view_l2[8], float cell_nv[8], float cell_s[8]){
 
    for ( int ver = 0; ver < 8; ++ver ) {
      const int x = cx + CELL_VERTEX[ver][0];
      const int y = cy + CELL_VERTEX[ver][1];
      const int z = cz + CELL_VERTEX[ver][2];

      const V3f normal = _getNormal(x, y, z);
      cell_view[ver]   = V3f(_view_pos.x - x * _vol.cell_scale,
                             _view_pos.y - y * _vol.cell_scale,
                             _view_pos.z - z * _vol.cell_scale);
      cell_view_l2[ver] = cell_view[ver].length2();
      const float l2 = normal.length2() * cell_view_l2[ver];
      cell_nv[ver] = gg::dot(normal, cell_view[ver]) / sqrt(l2);
    }

    int num_minus = 0, num_plus = 0, num_zero = 0;
    for ( int ver = 0; ver < 8; ++ver ) {
      const int x = cx + CELL_VERTEX[ver][0];
      const int y = cy + CELL_VERTEX[ver][1];
      const int z = cz + CELL_VERTEX[ver][2];
      const V3f gnv = _getGradientNormalDotView(x, y, z, cx, cy, cz, cell_nv);
      cell_s[ver] = gg::dot(gnv, cell_view[ver]) / sqrt(cell_view_l2[ver]);

      // iso - surface（line）を含むか否かをチェック
      if ( cell_s[ver] < 0.f ) ++num_minus;
      else
      if ( cell_s[ver] > 0.f ) ++num_plus;
      else ++num_zero;
    }

    return ( num_minus < 8 && num_plus < 8 && num_zero < 8 ) ? true : false;
  }

  void _loadDsData(int cx, int cy, int cz, const float cell_s[8], const V3f view[8], const float view_l2[8], float cell_ds[8]){
    for ( int ver = 0; ver < 8; ++ver ) {
      const int x = cx + CELL_VERTEX[ver][0];
      const int y = cy + CELL_VERTEX[ver][1];
      const int z = cz + CELL_VERTEX[ver][2];
      const V3f gs = _getGradientS(x, y, z, cx, cy, cz, cell_s);
      cell_ds[ver]  = gg::dot(gs, view[ver]) / sqrt(view_l2[ver]);
    }
  }

  bool _isSuggestiveContour(const IsoLine &line, const float cell_ds[8], const float cell_nv[8]){

    const int b_compornent_u = EDGE_DIRECTION[line.begin.belong_face][0];
    const int b_compornent_v = EDGE_DIRECTION[line.begin.belong_face][1];
    const float b_u = line.begin.pos[b_compornent_u] - (int)line.begin.pos[b_compornent_u];
    const float b_v = line.begin.pos[b_compornent_v] - (int)line.begin.pos[b_compornent_v];
    const int b_ver0 = VERTEX_INDEX[line.begin.belong_face][0][BEGIN];
    const int b_ver1 = VERTEX_INDEX[line.begin.belong_face][1][BEGIN];
    const int b_ver2 = VERTEX_INDEX[line.begin.belong_face][2][BEGIN];
    const int b_ver3 = VERTEX_INDEX[line.begin.belong_face][3][BEGIN];
    const float b_ds = cell_ds[b_ver0] * (1.f - b_u) * (1.f - b_v) + cell_ds[b_ver1] * b_u * (1.f - b_v) + 
                       cell_ds[b_ver3] * (1.f - b_u) * b_v         + cell_ds[b_ver2] * b_u * b_v;
    const float b_nv = cell_nv[b_ver0] * (1.f - b_u) * (1.f - b_v) + cell_nv[b_ver1] * b_u * (1.f - b_v) + 
                       cell_nv[b_ver3] * (1.f - b_u) * b_v         + cell_nv[b_ver2] * b_u * b_v;

    const int e_compornent_u = EDGE_DIRECTION[line.end.belong_face][0];
    const int e_compornent_v = EDGE_DIRECTION[line.end.belong_face][1];
    const float e_u = line.end.pos[e_compornent_u] - (int)line.end.pos[e_compornent_u];
    const float e_v = line.end.pos[e_compornent_v] - (int)line.end.pos[e_compornent_v];
    const int e_ver0 = VERTEX_INDEX[line.end.belong_face][0][BEGIN];
    const int e_ver1 = VERTEX_INDEX[line.end.belong_face][1][BEGIN];
    const int e_ver2 = VERTEX_INDEX[line.end.belong_face][2][BEGIN];
    const int e_ver3 = VERTEX_INDEX[line.end.belong_face][3][BEGIN];
    const float e_ds = cell_ds[e_ver0] * (1.f - e_u) * (1.f - e_v) + cell_ds[e_ver1] * e_u * (1.f - e_v) + 
                       cell_ds[e_ver3] * (1.f - e_u) * e_v         + cell_ds[e_ver2] * e_u * e_v;
    const float e_nv = cell_nv[e_ver0] * (1.f - e_u) * (1.f - e_v) + cell_nv[e_ver1] * e_u * (1.f - e_v) + 
                       cell_nv[e_ver3] * (1.f - e_u) * e_v         + cell_nv[e_ver2] * e_u * e_v;
    
    return ( ( ( b_nv > _eps && e_nv > _eps ) &&  ( b_ds >   _eps && e_ds  >  _eps ) ) || 
             ( ( b_nv < _eps && e_nv < _eps ) &&  ( b_ds < - _eps && e_ds < - _eps ) )    );
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
  
 V3f _getGradientNormalDotView(int x, int y, int z){
    const int bx = ( x - 1 >= 0 ) ? x - 1 : 0;
    const int by = ( y - 1 >= 0 ) ? y - 1 : 0;
    const int bz = ( z - 1 >= 0 ) ? z - 1 : 0;
    
    const int fx = ( x + 1 < _vol.data_size.x ) ? x + 1 : _vol.data_size.x - 1;
    const int fy = ( y + 1 < _vol.data_size.y ) ? y + 1 : _vol.data_size.y - 1;
    const int fz = ( z + 1 < _vol.data_size.z ) ? z + 1 : _vol.data_size.z - 1;

    V3f normal, view;
    float l2;
    float dif[6];
    normal = _getNormal(bx, y, z);
    view   = V3f(_view_pos.x - bx * _vol.cell_scale,
                 _view_pos.y - y  * _vol.cell_scale,
                 _view_pos.z - z  * _vol.cell_scale);
    l2 = normal.length2() * view.length2();
    dif[0] = gg::dot(normal, view) / sqrt(l2);

    normal = _getNormal(fx, y, z);
    view   = V3f(_view_pos.x - fx * _vol.cell_scale,
                 _view_pos.y - y  * _vol.cell_scale,
                 _view_pos.z - z  * _vol.cell_scale);
    l2 = normal.length2() * view.length2();
    dif[1] = gg::dot(normal, view) / sqrt(l2);

    normal = _getNormal(x, by, z);
    view   = V3f(_view_pos.x - x  * _vol.cell_scale,
                 _view_pos.y - by * _vol.cell_scale,
                 _view_pos.z - z  * _vol.cell_scale);
    l2 = normal.length2() * view.length2();
    dif[2] = gg::dot(normal, view) / sqrt(l2);

    normal = _getNormal(x, fy, z);
    view   = V3f(_view_pos.x - x  * _vol.cell_scale,
                 _view_pos.y - fy * _vol.cell_scale,
                 _view_pos.z - z  * _vol.cell_scale);
    l2 = normal.length2() * view.length2();
    dif[3] = gg::dot(normal, view) / sqrt(l2);

    normal = _getNormal(x, y, bz);
    view   = V3f(_view_pos.x - x  * _vol.cell_scale,
                 _view_pos.y - y  * _vol.cell_scale,
                 _view_pos.z - bz * _vol.cell_scale);
    l2 = normal.length2() * view.length2();
    dif[4] = gg::dot(normal, view) / sqrt(l2);

    normal = _getNormal(x, y, fz);
    view   = V3f(_view_pos.x - x  * _vol.cell_scale,
                 _view_pos.y - y  * _vol.cell_scale,
                 _view_pos.z - fz * _vol.cell_scale);
    l2 = normal.length2() * view.length2();
    dif[5] = gg::dot(normal, view) / sqrt(l2);

    const float dif_x = dif[1] - dif[0];
		const float dif_y = dif[3] - dif[2];
		const float dif_z = dif[5] - dif[4];

		const float dx = (fx - bx) * _vol.cell_scale;
		const float dy = (fy - by) * _vol.cell_scale;
		const float dz = (fz - bz) * _vol.cell_scale;

    return V3f(dif_x / dx, dif_y / dy, dif_z / dz);
  }

  V3f _getGradientNormalDotView(int x, int y, int z, int cx, int cy, int cz, const float cell_val[8]){
    const int bx = ( x - 1 >= 0 ) ? x - 1 : 0;
    const int by = ( y - 1 >= 0 ) ? y - 1 : 0;
    const int bz = ( z - 1 >= 0 ) ? z - 1 : 0;
    
    const int fx = ( x + 1 < _vol.data_size.x ) ? x + 1 : _vol.data_size.x - 1;
    const int fy = ( y + 1 < _vol.data_size.y ) ? y + 1 : _vol.data_size.y - 1;
    const int fz = ( z + 1 < _vol.data_size.z ) ? z + 1 : _vol.data_size.z - 1;


    const int ver_x = x - cx;
    const int ver_y = y - cy;
    const int ver_z = z - cz;
    const int ver_bx = bx - cx;
    const int ver_fx = fx - cx;
    const int ver_by = by - cy;
    const int ver_fy = fy - cy;
    const int ver_bz = bz - cz;
    const int ver_fz = fz - cz;

    float dif[6];
    // direction X
    if ( bx != cx ) {
      const V3f normal = _getNormal(bx, y, z);
      const V3f view(_view_pos.x - bx * _vol.cell_scale,
                     _view_pos.y - y  * _vol.cell_scale,
                     _view_pos.z - z  * _vol.cell_scale);
      const float l2 = normal.length2() * view.length2();
      dif[0] = gg::dot(normal, view) / sqrt(l2);
    } 
    else 
      dif[0] = cell_val[4*ver_z +2*ver_y +ver_bx];

    if ( fx != cx + 1) {
      const V3f normal = _getNormal(fx, y, z);
      const V3f view(_view_pos.x - fx * _vol.cell_scale,
                     _view_pos.y - y  * _vol.cell_scale,
                     _view_pos.z - z  * _vol.cell_scale);
      const float l2 = normal.length2() * view.length2();
      dif[1] = gg::dot(normal, view) / sqrt(l2);
    } 
    else 
      dif[1] = cell_val[4*ver_z +2*ver_y +ver_fx];

    // direction Y
    if ( by != cy ) {
      const V3f normal = _getNormal(x, by, z);
      const V3f view(_view_pos.x - x  * _vol.cell_scale,
                     _view_pos.y - by * _vol.cell_scale,
                     _view_pos.z - z  * _vol.cell_scale);
      const float l2 = normal.length2() * view.length2();
      dif[2] = gg::dot(normal, view) / sqrt(l2);
    } 
    else 
      dif[2] = cell_val[4*ver_z +2*ver_by +ver_x];

    if ( fy != cy + 1 ) {
      const V3f normal = _getNormal(x, fy, z);
      const V3f view(_view_pos.x - x  * _vol.cell_scale,
                     _view_pos.y - fy * _vol.cell_scale,
                     _view_pos.z - z  * _vol.cell_scale);
      const float l2 = normal.length2() * view.length2();
      dif[3] = gg::dot(normal, view) / sqrt(l2);
    } 
    else 
      dif[3] = cell_val[4*ver_z +2*ver_fy +ver_x];

    // direction Z
    if ( bz != cz ) {
      const V3f normal = _getNormal(x, y, bz);
      const V3f view(_view_pos.x - x  * _vol.cell_scale,
                     _view_pos.y - y  * _vol.cell_scale,
                     _view_pos.z - bz * _vol.cell_scale);
      const float l2 = normal.length2() * view.length2();
      dif[4] = gg::dot(normal, view) / sqrt(l2);
    } 
    else 
      dif[4] = cell_val[4*ver_bz +2*ver_y +ver_x];

    if ( fz != cz + 1 ) {
      const V3f normal = _getNormal(x, y, fz);
      const V3f view(_view_pos.x - x  * _vol.cell_scale,
                     _view_pos.y - y  * _vol.cell_scale,
                     _view_pos.z - fz * _vol.cell_scale);
      const float l2 = normal.length2() * view.length2();
      dif[5] = gg::dot(normal, view) / sqrt(l2);
    }
    else 
      dif[5] = cell_val[4*ver_fz +2*ver_y +ver_x];

    const float dif_x = dif[1] - dif[0];
		const float dif_y = dif[3] - dif[2];
		const float dif_z = dif[5] - dif[4];

		const float dx = (fx - bx) * _vol.cell_scale;
		const float dy = (fy - by) * _vol.cell_scale;
		const float dz = (fz - bz) * _vol.cell_scale;

    return V3f(dif_x / dx, dif_y / dy, dif_z / dz);
  }


  V3f _getGradientS(int x, int y, int z, int cx, int cy, int cz, const float cell_val[8]){
    // s = grad( (normal dot view) dot view)
    const int bx = ( x - 1 >= 0 ) ? x - 1 : 0;
    const int by = ( y - 1 >= 0 ) ? y - 1 : 0;
    const int bz = ( z - 1 >= 0 ) ? z - 1 : 0;
    
    const int fx = ( x + 1 < _vol.data_size.x ) ? x + 1 : _vol.data_size.x - 1;
    const int fy = ( y + 1 < _vol.data_size.y ) ? y + 1 : _vol.data_size.y - 1;
    const int fz = ( z + 1 < _vol.data_size.z ) ? z + 1 : _vol.data_size.z - 1;

    const int ver_x = x - cx;
    const int ver_y = y - cy;
    const int ver_z = z - cz;
    const int ver_bx = bx - cx;
    const int ver_fx = fx - cx;
    const int ver_by = by - cy;
    const int ver_fy = fy - cy;
    const int ver_bz = bz - cz;
    const int ver_fz = fz - cz;

    float dif[6];
    // direction X
    if ( bx != cx ) {
      const V3f gnv  = _getGradientNormalDotView(bx, y, z);
      const V3f view(_view_pos.x - bx * _vol.cell_scale,
                     _view_pos.y - y  * _vol.cell_scale,
                     _view_pos.z - z  * _vol.cell_scale);
      const float l2 = view.length2();
      dif[0] = gg::dot(gnv, view) / sqrt(l2);
    } 
    else
      dif[0] = cell_val[4*ver_z +2*ver_y +ver_bx];


    if ( fx != cx + 1 ) {
      const V3f gnv  = _getGradientNormalDotView(fx, y, z);
      const V3f view(_view_pos.x - fx * _vol.cell_scale,
                     _view_pos.y - y  * _vol.cell_scale,
                     _view_pos.z - z  * _vol.cell_scale);
      const float l2 = view.length2();
      dif[1] = gg::dot(gnv, view) / sqrt(l2);
    } 
    else 
      dif[1] = cell_val[4*ver_z +2*ver_y +ver_fx];

    // direction Y
    if ( by != cy ) {
      const V3f gnv  = _getGradientNormalDotView(x, by, z);
      const V3f view(_view_pos.x - x  * _vol.cell_scale,
                     _view_pos.y - by * _vol.cell_scale,
                     _view_pos.z - z  * _vol.cell_scale);
      const float l2 = view.length2();
      dif[2] = gg::dot(gnv, view) / sqrt(l2);
    } 
    else 
      dif[2] = cell_val[4*ver_z +2*ver_by +ver_x];

    if ( fy != cy + 1 ) {
      const V3f gnv  = _getGradientNormalDotView(x, fy, z);
      const V3f view(_view_pos.x - x  * _vol.cell_scale,
                     _view_pos.y - fy * _vol.cell_scale,
                     _view_pos.z - z  * _vol.cell_scale);
      const float l2 = view.length2();
      dif[3] = gg::dot(gnv, view) / sqrt(l2);
    } 
    else 
      dif[3] = cell_val[4*ver_z +2*ver_fy +ver_x];

    // direction Z
    if ( bz != cz ) {
      const V3f gnv  = _getGradientNormalDotView(x, y, bz);
      const V3f view(_view_pos.x - x  * _vol.cell_scale,
                     _view_pos.y - y  * _vol.cell_scale,
                     _view_pos.z - bz * _vol.cell_scale);
      const float l2 = view.length2();
      dif[4] = gg::dot(gnv, view) / sqrt(l2);
    } 
    else 
      dif[4] = cell_val[4*ver_bz +2*ver_y +ver_x];

    if ( fz != cz + 1 ) {
      const V3f gnv  = _getGradientNormalDotView(x, y, fz);
      const V3f view(_view_pos.x - x  * _vol.cell_scale,
                     _view_pos.y - y  * _vol.cell_scale,
                     _view_pos.z - fz * _vol.cell_scale);
      const float l2 = view.length2();
      dif[5] = gg::dot(gnv, view) / sqrt(l2);
    } 
    else 
      dif[5] = cell_val[4*ver_fz +2*ver_y +ver_x];

    const float dif_x = dif[1] - dif[0];
    const float dif_y = dif[3] - dif[2];
    const float dif_z = dif[5] - dif[4];

    const float dx = (fx - bx) * _vol.cell_scale;
    const float dy = (fy - by) * _vol.cell_scale;
    const float dz = (fz - bz) * _vol.cell_scale;

    return V3f(dif_x / dx, dif_y / dy, dif_z / dz);
  }

};

}// end of namespace "VLD"

#endif