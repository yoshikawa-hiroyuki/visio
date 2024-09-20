//
// vsnKeyFrameAnim
//
#ifndef _VSN_KEYFRAMEANIM_H_
#define _VSN_KEYFRAMEANIM_H_

#include <map>
#include <libxml/tree.h>
#include "utilMath.h"
#include "vsnError.h"


//----------------------------------------------------------------
// class vsnAnimFrame
//----------------------------------------------------------------
//! キーフレームアニメーション用フレームデータ
struct vsnAnimFrame {
  //! 回転クォータニオン
  CES::Quat4<float> rotQuat;
  //! 移動量ベクトル
  CES::Vec3<float>  transVec;
  //! スケーリングファクター
  CES::Vec3<float>  scaleVec;
  //! 回転・スケーリング中心
  CES::Vec3<float>  centerVec;
  //! タイムステップ
  int               step;

  vsnAnimFrame() : step(0) {}
  ~vsnAnimFrame() {}

  //! 回転マトリックスの設定
  bool setRotMat(const CES::Mat4<float>& m);
  //! XMLノードのパース
  bool parseXML(xmlNodePtr node, double& tm);
  //! XMLノードの出力
  bool outputXML(std::ostream& os, const double tm, const size_t ts =0);

  //! XMLパースユーティリティ
  bool ParseRotMat(xmlNodePtr node, CES::Mat4<float>& M);
  bool ParsePosture(xmlNodePtr node, CES::Mat4<float>& M);
  bool ParseVector(xmlNodePtr node, CES::Vec3<float>& V);
};

//! キーフレームアニメーション用キーフレーム
typedef std::pair<double, vsnAnimFrame> vsnKeyframe;


//----------------------------------------------------------------
// class vsnKeyFrameAnim
//----------------------------------------------------------------

//! キーフレームアニメーションデータ
class vsnKeyFrameAnim {
  double m_totalTime;
  double m_initialTime;
  unsigned int m_fps;
  bool m_stepMode, m_loopMode;
  std::map<double, vsnAnimFrame> m_keyFrames;

public:
  vsnKeyFrameAnim() : m_totalTime(0.), m_initialTime(0.),
    m_fps(30), m_stepMode(false), m_loopMode(false) {}
  virtual ~vsnKeyFrameAnim() {m_keyFrames.clear();}

  //! アニメーションのトータル時間を返す
  double getTotalTime() const {return m_totalTime;}
  //! アニメーションの総フレーム数を返す
  size_t getTotalFrames() const;
  //! アニメーションの開始時間を返す
  double getInitialTime() const {return m_initialTime;}

  //! 一単位時間あたりのフレーム数を返す
  unsigned int getFps() const {return m_fps;}
  //! 一単位時間あたりのフレーム数を設定する
  bool setFps(const unsigned int n) {
    if ( n < 1 ) return false;
    m_fps = n; return true;
  }

  //! タイムステップ補間モードを返す
  bool getStepMode() const {return m_stepMode;}
  //! タイムステップ補間モードを設定する
  void setStepMode(const bool smd) {m_stepMode = smd;}

  //! ループモードを返す
  bool getLoopMode() const {return m_loopMode;}
  //! ループモードを設定する
  void setLoopMode(const bool lmd) {m_loopMode = lmd;}

  //! キーフレームリストの取得
  const std::map<double, vsnAnimFrame>& getFrameList() const {
    return m_keyFrames;
  }
  //! キーフレーム数を返す
  size_t getNumKeyframes() const {return m_keyFrames.size();}
  //! idx番目のキーフレームの取得
  bool getKeyframe(const size_t idx, double& tm, vsnAnimFrame& frame) const;

  //! キーフレームの追加
  bool addKeyframe(const double tm, const vsnAnimFrame& frame);
  //! キーフレームの削除
  bool delKeyframe(const size_t idx);
  bool delKeyframe(const double tm);
  //! キーフレームの置き換え
  bool replaceKeyframe(const size_t idx, const vsnAnimFrame& frame);

  //! 指定時刻のフレームの取得
  bool getFrame(const double tm, vsnAnimFrame& frame) const;

  //! XMLノードのパース
  bool parseXML(xmlNodePtr node);
  //! XMLノードの出力
  bool outputXML(std::ostream& os, const size_t ts =0);

  //! 初期化
  void Reset() {
    m_totalTime = 0; m_initialTime = 0.; m_fps = 30;
    m_stepMode = false; m_loopMode = false;
    m_keyFrames.clear();
  }
};

#endif // _VSN_KEYFRAMEANIM_H_
