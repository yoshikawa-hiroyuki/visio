//
// vsnLut
//
#ifndef _VSN_LUT_H_
#define _VSN_LUT_H_

#include "vfrDatamap.h"
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <sstream>

namespace VSN {
  //! 色成分の格納数の最大値
  enum {LUT_MAX_ENTRY =256};
};

using namespace std;
using namespace VSN;


//! LookUp Table構造体
struct vsnLut {
  //! 色成分の格納数
  unsigned  numEntry;
  //! 色成分
  /*! R,G,B,Aの色成分。値域は[0.0～1.0]、
      格納順序は(R0, G0, B0, A0, R1, G1, B1, ...)
  */
  float lutEntry[LUT_MAX_ENTRY * 4];
  //! データ値域
  float minVal, maxVal;
  //! 変更フラグ
  mutable Bool isStdLut;

  //! デフォルトコンストラクタ
  vsnLut()
    : numEntry(LUT_MAX_ENTRY),minVal(0.0f),maxVal(1.0f),isStdLut(TRUE) {
    vfrDatamap dmap(0.0f, 255.0f); dmap.setAlp(0, 0.0f);
    for ( register int i = 0; i < LUT_MAX_ENTRY; i++ ) {
      vector4 ev; dmap.datamap((float)i, ev);
      lutEntry[i*4  ] = ev[0]; lutEntry[i*4+1] = ev[1];
      lutEntry[i*4+2] = ev[2]; lutEntry[i*4+3] = ev[3];
    }
  }
  //! vfrDatamapオブジェクトからのコンストラクタ
  vsnLut(const vfrDatamap& ref) : numEntry(LUT_MAX_ENTRY),isStdLut(FALSE) {
    vfrDatamap dmap(ref); dmap.setMinMax(0.0f, 255.0f);
    for ( register int i = 0; i < LUT_MAX_ENTRY; i++ ) {
      vector4 ev; dmap.datamap((float)i, ev);
      lutEntry[i*4  ] = ev[0]; lutEntry[i*4+1] = ev[1];
      lutEntry[i*4+2] = ev[2]; lutEntry[i*4+3] = ev[3];
    }
    minVal = ref.getMin(); maxVal = ref.getMax();
  }
  //! コピーコンストラクタ
  vsnLut(const vsnLut& org) {*this = org;}
  //! デストラクタ
  virtual ~vsnLut() {}

  //! 文字列ストリームからの入力
  Bool ImportStream(istream& iss) {
    isStdLut = FALSE;
    numEntry = 0;
    char buff[256]; const char* delim = " ,\t";
    if ( (iss >> ws).eof() ) return FALSE;
    iss.getline(buff, 256); if ( iss.fail() ) return FALSE;
    char* pc = strtok(buff, delim); if ( ! pc ) return FALSE;
    minVal = (float)atof(pc);
    pc = strtok(NULL, delim); if ( ! pc ) return FALSE;
    maxVal = (float)atof(pc);
    while ( numEntry < LUT_MAX_ENTRY ) {
      float r, g, b, a; int nscan;
      if ( (iss >> ws).eof() ) break;
      iss.getline(buff, 256); if ( iss.fail() ) break;
      nscan = sscanf(buff, "%f %f %f %f", &r, &g, &b, &a);
      if ( nscan < 3 ) return FALSE;
      lutEntry[numEntry*4  ] = VFR_CLAMP(0.f, 1.f, r);
      lutEntry[numEntry*4+1] = VFR_CLAMP(0.f, 1.f, g);
      lutEntry[numEntry*4+2] = VFR_CLAMP(0.f, 1.f, b);
      lutEntry[numEntry*4+3] = (nscan == 4) ? VFR_CLAMP(0.f, 1.f, a) : 1.f;
      numEntry++;
    } // end of while(numEntry < LUT_MAX_ENTRY)
    if ( numEntry < 1 ) return FALSE;
    return TRUE;
  }

  //! 文字列ストリームへの出力
  Bool ExportStream(ostream& oss, const size_t ts =0) const {
    if ( numEntry < 1 ) return FALSE;
    std::string idts; register size_t i;
    for ( i = 0; i < ts; i++ ) idts.push_back(' ');
    oss << idts << minVal << " " << maxVal << endl;
    for ( i = 0; i < numEntry; i++ ) {
      oss << idts << VFR_CLAMP(0.f, 1.f, lutEntry[i*4]) << " "
	  << VFR_CLAMP(0.f, 1.f, lutEntry[i*4+1]) << " "
	  << VFR_CLAMP(0.f, 1.f, lutEntry[i*4+2]) << " "
	  << VFR_CLAMP(0.f, 1.f, lutEntry[i*4+3]) << endl;
    }
    return TRUE;
  }

  //! 代入オペレータ
  void operator=(const vsnLut& org) {
    numEntry = org.numEntry;
    memcpy(lutEntry, org.lutEntry, sizeof(float)*numEntry*4);
    minVal = org.minVal; maxVal = org.maxVal;
    isStdLut = org.isStdLut;
  }

  //! 正規化
  /*! 色成分の格納数を256個に正規化する */
  Bool normalize() {
    if ( numEntry == 0 ) return FALSE;
    if ( numEntry == LUT_MAX_ENTRY ) return TRUE;
    vsnLut org(*this);
    numEntry = LUT_MAX_ENTRY;
    for ( register int i = 0; i < LUT_MAX_ENTRY; i++ ) {
      int oi = org.numEntry * i / LUT_MAX_ENTRY;
      lutEntry[i*4  ] = org.lutEntry[oi*4  ];
      lutEntry[i*4+1] = org.lutEntry[oi*4+1];
      lutEntry[i*4+2] = org.lutEntry[oi*4+2];
      lutEntry[i*4+3] = org.lutEntry[oi*4+3];
    }
    return TRUE;
  }

  //! 値に対するインデックス取得
  int getValIdx(const float val) {
    if ( val >= maxVal ) return (numEntry-1);
    if ( val <= minVal ) return 0;
    if ( maxVal<=minVal ) return 0;
    if ( val >= minVal && val <= maxVal )
      ; // failsafe for IEEE Nan/Inf
    else return 0;
    return (int)((numEntry-1)*(val-minVal)/(maxVal-minVal));
  }
};

#endif // _VSN_LUT_H_
