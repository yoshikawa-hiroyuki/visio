#ifndef VLD_CELL_FUNC_H
#define VLD_CELL_FUNC_H

#include<cmath>

#include "VLDTypes.h"
#include "VLDCellIndexTable.h"

namespace VLD{

class CellFunc{
public:

  static 
  bool buildIsoLines(const float val[8], CellIsoLines &iso_lines){

    CellFace cell[6];
    if ( ! _buildIsoPoints(val, cell) ) return false;

    iso_lines.clear();
    for ( int face = 0; face < 6; ++face ) {

      switch ( cell[face].num_iso_point ) {
        // iso-lineは、iso-pointのレベルがマイナスからプラスへと接続する
        case 0:
          break;
        case 2:
          {
            int count = 0;
            IsoPoint *iso_point[2];
            for ( int edge=0; edge < 4 && count < 2; ++edge ) {
              if ( cell[face].edge_point[edge].level == IsoPoint::MINUS ) { 
                iso_point[BEGIN] = &cell[face].edge_point[edge];
                ++count;
                continue;
              }
              else 
              if ( cell[face].edge_point[edge].level == IsoPoint::PLUS ) {
                iso_point[END] = &cell[face].edge_point[edge];
                ++count;
                continue;
              }
            }

            if ( count < 2 ) {
              return false; 
            }

            const IsoLine iso_line( *iso_point[BEGIN], *iso_point[END]);
            iso_lines.push_back(iso_line);
            break;
          }
        case 4:
          {
            const float v0 = val[ VERTEX_INDEX[face][0][BEGIN] ];
            const float v1 = val[ VERTEX_INDEX[face][1][BEGIN] ];
            const float v2 = val[ VERTEX_INDEX[face][2][BEGIN] ];
            const float v3 = val[ VERTEX_INDEX[face][3][BEGIN] ];
            const float v = (v3 * v1 - v2 * v0) / (v3 + v1 - v2 - v0);
          
            int count = 0;
            int minus_edge[2];
            for ( int edge = 0; edge < 4 && count < 2; ++edge ) 
              if ( cell[face].edge_point[edge].level == IsoPoint::MINUS ) 
                minus_edge[count++] = edge;
            
            if ( count < 2 )  
              return false; 

            int b0, e0, b1, e1;
            if ( v < 0.f ) {
              b0 = minus_edge[0];
              e0 = ( b0 -1 >= 0 ) ? b0 - 1 : 3;
              b1 =minus_edge[1];
              e1 = ( b1 -1 >= 0 ) ? b1 - 1 : 3;
            } else {
              b0 = minus_edge[0];
              e0 = ( b0 +1 <  4 ) ? b0 + 1 : 0;
              b1 = minus_edge[1];
              e1 = ( b1 +1 <  4 ) ? b1 + 1 : 0;
            }

            const IsoLine iso_line0(cell[face].edge_point[b0], cell[face].edge_point[e0]);
            iso_lines.push_back(iso_line0);

            const IsoLine iso_line1(cell[face].edge_point[b1], cell[face].edge_point[e1]);
            iso_lines.push_back(iso_line1);
            break;
            }
        default:
          // iso-pointが、1個、または3個の場合は、退化したケース
          return false;
          break;
      }
    }

    return true;
  }

  static
  void buildIsoCycles(CellIsoLines &lines, CellIsoCycles &cycles){
    // iso-lineをサイクルとして、再構築（並び替え） 
    // エッジ上には、iso-pointは、一つしか存在しないという性質を利用
    cycles.clear();
    while ( !lines.empty() ) {
      cycles.push_back(CellIsoLines()); 
      CellIsoLines &cycle = cycles.back();

      cycle.push_back(lines.back());
      lines.pop_back();

      const int cycle_begin = EDGE_INDEX[cycle.front().begin.belong_face][cycle.front().begin.belong_edge];
      
      int cycle_end   = EDGE_INDEX[cycle.back().end.belong_face   ][cycle.back().end.belong_edge   ];
      for ( CellIsoLines::iterator it_l = lines.begin(); it_l != lines.end();  ) {
        const int line_begin = EDGE_INDEX[it_l->begin.belong_face][it_l->begin.belong_edge];
        if ( line_begin == cycle_end ) {
          // サイクルの終点と、対象となるラインの始点が同じ位置（エッジ）だった場合
          cycle.push_back(*it_l);
          cycle_end = EDGE_INDEX[cycle.back().end.belong_face][cycle.back().end.belong_edge];
          lines.erase(it_l);
          // サイクルの始点と、サイクルの終点が同じ位置（エッジ）だった場合
          if ( cycle_begin == cycle_end ) break;
          it_l = lines.begin();
          continue;
        }
        ++it_l;
      }
    }

  }

  static
  bool buildContour(int cx, int cy, int cz, const float val[8], const CellIsoCycles& cycles, CellContours &contours){
    contours.clear();

    for ( std::size_t cyc = 0; cyc < cycles.size(); ++cyc ) {
      CellIsoPoints points;
    
      const CellIsoLines &cycle = cycles[cyc];
      for ( std::size_t seg = 0; seg < cycle.size(); ++seg ) {
        const IsoPoint &begin = cycle[seg].begin;
        const IsoPoint &end   = cycle[seg].end;
    
        // iso-point(begin)が所属するエッジの方向（x = 0, y = 1, z = 1）を取得
        const int component_b = EDGE_DIRECTION[begin.belong_face][begin.belong_edge];
        // iso-point(begin)を含むエッジの始点の座標値（エッジの方向のみ）を取得
        const float bb_ver = (float)CELL_VERTEX[ VERTEX_INDEX[begin.belong_face][begin.belong_edge][BEGIN] ][component_b];  
        // iso-pointが、エッジの始点から、どれくらい離れているか（比）を計算（ 0.f <= b_fac <= 1.f ）
        const float b_fac = fabs(begin.pos[component_b] - bb_ver);
        // iso-point上で、もう一方のデータ（n dot v, suggestive, cutting plane）の値を計算
        const float bb_val = val[ VERTEX_INDEX[begin.belong_face][begin.belong_edge][BEGIN] ];
        const float be_val = val[ VERTEX_INDEX[begin.belong_face][begin.belong_edge][END  ] ];
        const float b_val  = bb_val + (be_val - bb_val) * b_fac;
        
        // iso-point(end)が所属するエッジの方向（x = 0, y = 1, z = 1）を取得
        const int component_e = EDGE_DIRECTION[end.belong_face][end.belong_edge];
        // iso-point(end)を含むエッジの始点の座標値（エッジの方向のみ）を取得
        const float eb_ver = (float)CELL_VERTEX[ VERTEX_INDEX[end.belong_face][end.belong_edge][BEGIN] ][component_e];
        // iso-pointが、エッジの始点から、どれくらい離れているか（比）を計算（ 0.f <= e_fac <= 1.f ）
        const float e_fac = fabs(end.pos[component_e] - eb_ver);
        // iso-point上で、もう一方のデータの値を計算
        const float eb_val = val[ VERTEX_INDEX[end.belong_face][end.belong_edge][BEGIN] ];
        const float ee_val = val[ VERTEX_INDEX[end.belong_face][end.belong_edge][END  ] ];
        const float e_val  = eb_val + (ee_val - eb_val) * e_fac;
        
        float factor;
        IsoPoint::LEVEL level;
        // レベルは、終点のiso-valueの符号
        if ( ! _haveZeroPosition(b_val, e_val, &factor, &level) ) continue;
        
        IsoPoint point;
        point.level = level;
        point.belong_face = begin.belong_face;
        // iso-line上で、もう一方のデータ（n dot v, suggestive, cutting plane）がゼロになる点を計算        
        point.pos.x = begin.pos.x + (end.pos.x - begin.pos.x) * factor;
        point.pos.y = begin.pos.y + (end.pos.y - begin.pos.y) * factor;
        point.pos.z = begin.pos.z + (end.pos.z - begin.pos.z) * factor;
        points.push_back(point);
      }

      // コンタの作成、レベルがマイナスからプラスへと接続する
      for ( std::size_t b = 0; b < points.size(); ++b ) {
        const std::size_t e = ( b + 1 < points.size() ) ? b + 1 : 0;
        if ( ! ( points[b].level == IsoPoint::MINUS && points[e].level == IsoPoint::PLUS ) ) continue;
        points[b].pos.x += cx;
        points[b].pos.y += cy;
        points[b].pos.z += cz;
        points[e].pos.x += cx;
        points[e].pos.y += cy;
        points[e].pos.z += cz;

        contours.push_back(IsoLine(points[b].pos, points[b].level, points[b].belong_face, points[b].belong_edge,
                                   points[e].pos, points[e].level, points[e].belong_face, points[e].belong_edge));
      }
    }

    return contours.empty() ? false : true;
  }

private:

  static
  bool _haveZeroPosition(float b_val, float e_val, float *factor, IsoPoint::LEVEL *level){
    *level = IsoPoint::NO_ISO_POINT;
  
    if ( (( b_val <= 0.f ) && ( e_val <= 0.f )) ||
         (( b_val >  0.f ) && ( e_val >  0.f ))    )
      return false;
    else 
      if ( e_val > 0.f ) *level = IsoPoint::PLUS;
    else 
      *level = IsoPoint::MINUS;

    *factor = fabs(b_val / (e_val - b_val));
    return true;
  }

  static
  bool _buildIsoPoints(const float val[8], CellFace cell[6]){

    cell[0].num_iso_point = cell[1].num_iso_point = 
      cell[2].num_iso_point = cell[3].num_iso_point = 
        cell[4].num_iso_point = cell[5].num_iso_point = 0;

		int num_point = 0;
    for ( int edge = 0; edge < 12; ++edge ) {

      // 辺（edge）を共有する面を取得
      const int face0 = NEIGBER_FACE[edge][0];
      const int face1 = NEIGBER_FACE[edge][1];

      // 辺（edge）を共有する面内の辺（面を構成する辺のうち、何番目の辺かという情報）を取得
      const int face_edge0 = NEIGBER_FACE_EDGE[edge][0];
      const int face_edge1 = NEIGBER_FACE_EDGE[edge][1];

      cell[face0].edge_point[face_edge0].level = 
        cell[face1].edge_point[face_edge1].level = IsoPoint::NO_ISO_POINT;

      
      // valのBEGIN、ENDの順番は、face0のface_edge0と同じ
      const float b_val = val[EDGE_VERTEX_INDEX[edge][BEGIN]];
			const float e_val = val[EDGE_VERTEX_INDEX[edge][END  ]];
      IsoPoint::LEVEL level;
      float factor;
      // レベルは、終点のiso-valueの符号(0.fの際は、マイナス)
			if ( ! _haveZeroPosition(b_val, e_val, &factor, &level) ) continue;
      
      cell[face0].edge_point[face_edge0].level = level;
      cell[face1].edge_point[face_edge1].level = ( level == IsoPoint::PLUS ) ? IsoPoint::MINUS : IsoPoint::PLUS;

      cell[face0].edge_point[face_edge0].belong_face = face0;
			cell[face1].edge_point[face_edge1].belong_face = face1;

      cell[face0].edge_point[face_edge0].belong_edge = face_edge0;
      cell[face1].edge_point[face_edge1].belong_edge = face_edge1;
				
			const V3f b_ver((float)CELL_VERTEX[EDGE_VERTEX_INDEX[edge][BEGIN]][0],
			                (float)CELL_VERTEX[EDGE_VERTEX_INDEX[edge][BEGIN]][1],
                      (float)CELL_VERTEX[EDGE_VERTEX_INDEX[edge][BEGIN]][2]);
			const V3f e_ver((float)CELL_VERTEX[EDGE_VERTEX_INDEX[edge][END  ]][0],
			                (float)CELL_VERTEX[EDGE_VERTEX_INDEX[edge][END  ]][1],
                      (float)CELL_VERTEX[EDGE_VERTEX_INDEX[edge][END  ]][2]);


			cell[face0].edge_point[face_edge0].pos.x = 
        cell[face1].edge_point[face_edge1].pos.x = b_ver.x + (e_ver.x - b_ver.x) * factor;
			cell[face0].edge_point[face_edge0].pos.y = 
        cell[face1].edge_point[face_edge1].pos.y = b_ver.y + (e_ver.y - b_ver.y) * factor;
			cell[face0].edge_point[face_edge0].pos.z = 
        cell[face1].edge_point[face_edge1].pos.z = b_ver.z + (e_ver.z - b_ver.z) * factor;

      ++cell[face0].num_iso_point;
			++cell[face1].num_iso_point;
			num_point += 2;
		}
		return ( num_point > 0 ) ? true : false;
  }

};

}// end of namespace "VLD"

#endif