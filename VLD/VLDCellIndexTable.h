#ifndef VLD_CELL_INDEX_TABLE_H
#define VLD_CELL_INDEX_TABLE_H

namespace VLD{

// セルの頂点のインデックスから、頂点の座標（8個、正規化された値）への変換
const int CELL_VERTEX[8][3] = {{0, 0, 0},
                               {1, 0, 0},
                               {0, 1, 0},
                               {1, 1, 0},
                               {0, 0, 1},
                               {1, 0, 1},
                               {0, 1, 1},
                               {1, 1, 1}};

// 面のインデックスから、セルを構成する６面の法線ベクトル（6個）への変換
const int FACE_NORMAL[6][3] = {{-1,  0,  0},  // X_MINUS face 
                               { 1,  0,  0},  // X_PLUS  face 
                               { 0, -1,  0},  // Y_MINUS face 
                               { 0,  1,  0},  // Y_PLUS  face 
                               { 0,  0, -1},  // Z_MINUS face 
                               { 0,  0,  1}}; // Z_PLUS  face         


// 面、辺、点（始点、終点）のインデックスから、セルの頂点（8個）のインデックスへの変換
// インデックスは、データ（1D）の並びに対応
const int VERTEX_INDEX[6][4][2] = {{{0, 4},{4, 6},{6, 2},{2, 0}},  // X_MINUS face 
                                   {{1, 3},{3, 7},{7, 5},{5, 1}},  // X_PLUS  face
                                   {{0, 1},{1, 5},{5, 4},{4, 0}},  // Y_MINUS face
                                   {{2, 6},{6, 7},{7, 3},{3, 2}},  // Y_PLUS  face
                                   {{0, 2},{2, 3},{3, 1},{1, 0}},  // Z_MINUS face
                                   {{4, 5},{5, 7},{7, 6},{6, 4}}}; // Z_PLUS  face

// 面、辺のインデックスから、辺のインデックス（12個）への変換
const int EDGE_INDEX[6][4] = {{ 8,  6, 10,  4},  // X_MINUS face  
                              { 5, 11,  7,  9},  // X_PLUS  face 
                              { 0,  9,  1,  8},  // Y_MINUS face 
                              {10,  3, 11,  2},  // Y_PLUS  face 
                              { 4,  2,  5,  0},  // Z_MINUS face 
                              { 1,  7,  3,  6}}; // Z_PLUS  face 

// 面、辺のインデックスから、辺の方向（x = 0, y = 1, z = 2）
const int EDGE_DIRECTION[6][4] = {{Z, Y, Z, Y},  // X_MINUS face 
                                  {Y, Z, Y, Z},  // X_PLUS  face
                                  {X, Z, X, Z},  // Y_MINUS face
                                  {Z, X, Z, X},  // Y_PLUS  face
                                  {Y, X, Y, X},  // Z_MINUS face
                                  {X, Y, X, Y}}; // Z_PLUS  face



// 辺のインデックス（12個）から、セルの頂点（8個）のインデックスへの変換
const int EDGE_VERTEX_INDEX[12][2] = {{0, 1},{2, 3},{4, 5},{6, 7},  // parallel to x axis
                                      {0, 2},{1, 3},{4, 6},{5, 7},  // parallel to y axis
                                      {0, 4},{1, 5},{2, 6},{3, 7}}; // parallel to z axis


// 辺のインデックス（12個）から、その辺を共有する面（6個）のインデックスへの変換
const int NEIGBER_FACE[12][2] = {{2,4},{4,3},{5,2},{3,5},
                                 {4,0},{1,4},{0,5},{5,1},
                                 {0,2},{2,1},{3,0},{1,3}};

// 辺のインデックス（12個）から、その辺を共有する面内の辺（4個）のインデックスへの変換
// ※面の構成する辺のうち、（0,0,0）から法線ベクトルが外を向く方向に数えて、何番目の辺かという情報
const int NEIGBER_FACE_EDGE[12][2] = {{0,3},{1,3},{0,2},{1,2},
                                      {0,3},{0,2},{1,3},{1,2},
                                      {0,3},{1,3},{0,2},{1,2}};


}// end of namespace "VLD"

#endif
