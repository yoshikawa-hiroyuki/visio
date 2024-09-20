#pragma once
/**
 * @mainpage
 * @section はじめに
 * このドキュメントはSTLの面毎のデータを保持するクラスを説明します。
 * 全ての面で同じ種類の値を保持することを前提とします。
 * 値は、int, float, double の値を保持できます。
 * スカラー、ベクトル、テンソルの形態で保持できます。
 */

/**
 * @file
 * STLのポリゴンデータの保持クラス群ファイル
 */

#include <vector>
#include <string>
#include <ostream>

/**
 * @brief STLのポリゴンデータの保持クラスの名前空間
 */
namespace STLD {

  /**
   * @brief 物理量の情報クラス
   */
  class PhysInfo {
  public:
    /**
     * @brief デフォルトコンストラクタ
     */
    PhysInfo(void) : m_dataKind(0), m_vecLen(0) {}

    /**
     * @brief コンストラクタ
     * @param dataKind データの種類
     * @param vecLen ベクトル長
     */
    PhysInfo(unsigned char dataKind, unsigned char vecLen) :
      m_dataKind(dataKind), m_vecLen(vecLen) {}

    /**
     * @brief データの種類を取得する
     * @result データの種類。0(未定義) or 1(int) or 2(float) or 3(double)
     */
    unsigned char GetDataKind(void) const { return m_dataKind; }

    /**
     * @brief 物理量がintかどうか
     * @retval true 物理量がintである
     * @retval false 物理量はintでない
     */
    bool IsInt(void) const { return m_dataKind == 1; }

    /**
     * @brief 物理量がfloatかどうか
     * @retval true 物理量がfloatである
     * @retval false 物理量はfloatでない
     */
    bool IsFloat(void) const { return m_dataKind == 2; }

    /**
     * @brief 物理量がdoubleかどうか
     * @retval true 物理量がdoubleである
     * @retval false 物理量はdoubleでない
     */
    bool IsDouble(void) const { return m_dataKind == 3; }

    /**
     * @brief データの種類を設定する
     * @param val データの種類
     * @retval true 設定に成功
     * @retval false 設定に失敗
     */  
    bool SetDataKind(unsigned char val) {
      bool result(false);
      if (val == 1 || val == 2 || val == 3) {
	m_dataKind = val; result = true;
      }
      return result;
    }

    /**
     * @brief ベクトル長を取得する
     * @result ベクトル長。0 エラー。
     */
    unsigned char GetVecLen(void) const { return m_vecLen; }

    /**
     * @brief ベクトル長を設定する
     * @param val ベクトル長
     * @retval true 設定に成功
     * @retval false 設定に失敗
     */
    bool SetVecLen(unsigned char val) {
      bool result(false);
      /**
       * val が1, 3, 9 以外であれば、値を設定しない。
       * 失敗にする。
       */
      if (val == 1 || val == 3 || val == 9) {
	m_vecLen = val; result = true;
      }
      return result;
    }

    /**
     * @brief データ種別の文字列を取得する
     * @return データ種別の文字列
     */
    std::string GetDataKindStr(void) const {
      return (m_dataKind == 1) ? "int" : (m_dataKind == 2) ? "float" : (m_dataKind == 3) ? "double" : "unknown";
    }

    /**
     * @brief ベクトル長の文字列を取得する
     * @return ベクトル長の文字列
     */
    std::string GetVecLenStr(void) const {
      return (m_vecLen == 1) ? "scalar" : (m_vecLen == 3) ? "vector" : (m_vecLen == 9) ? "tensor" : "unknown";
    }

  private:
    /**
     * @brief データの種類
     * 0(未定義), 1(int), 2(float), 3(double)
     */
    unsigned char m_dataKind;

    /**
     * @brief ベクトル長
     * 0(未定義), 1(スカラー), 3(ベクトル), 9(テンソル)
     */
    unsigned char m_vecLen;
  };

  /**
   * @brief ポリゴンの情報
   *
   * 値を保持する
   */
  class FaceInfo {
  public:
    /**
     * @brief int の値を取得する
     * @param idx 何番目
     * @return 値
     */
    int GetIntVal(size_t idx) const { return m_intValues[idx]; }
    
    /**
     * @brief float の値を取得する
     * @param idx 何番目
     * @return 値
     */
    float GetFloatVal(size_t idx) const { return m_floatValues[idx]; }

    /**
     * @brief double の値を取得する
     * @param idx 何番目
     * @return 値
     */
    double GetDoubleVal(size_t idx) const { return m_doubleValues[idx]; }

    /**
     * @brief 値をクリアする
     */
    void Clear(void) {
      /**
       * int の配列をクリアする
       */
      m_intValues.clear();
      /**
       * float の配列をクリアする
       */
      m_floatValues.clear();
      /**
       * double の配列をクリアする
       */
      m_doubleValues.clear();
    }

    /**
     * @brief idx 番目に int の値を設定する
     * @param idx インデックス
     * @param val 値
     */
    void SetVal(size_t idx, int val) {
      /**
       * 配列の大きさが小さければ、配列を大きくし、０で埋める
       */
      for (size_t i = m_intValues.size(); i < idx + 1; ++i) {
	m_intValues.push_back(0);
      }
      m_intValues[idx] = val;
    }

    /**
     * @brief idx 番目に float の値を設定する
     * @param idx インデックス
     * @param val 値
     */
    void SetVal(size_t idx, float val) {
      /**
       * 配列の大きさが小さければ、配列を大きくし、０で埋める
       */
      for (size_t i = m_floatValues.size(); i < idx + 1; ++i) {
	m_floatValues.push_back(0);
      }
      m_floatValues[idx] = val;
    }

    /**
     * @brief idx 番目に double の値を設定する
     * @param idx インデックス
     * @param val 値
     */
    void SetVal(size_t idx, double val) {
      /**
       * 配列の大きさが小さければ、配列を大きくし、０で埋める
       */
      for (size_t i = m_doubleValues.size(); i < idx + 1; ++i) {
	m_doubleValues.push_back(0);
      }
      m_doubleValues[idx] = val;
    }

  private:
    /**
     * @brief intの配列
     */
    std::vector<int> m_intValues;
    /**
     * @brief floatの配列
     */
    std::vector<float> m_floatValues;
    /**
     * @brief doubleの配列
     */
    std::vector<double> m_doubleValues;
  };

  /**
   * @brief STLのポリゴン毎のデータを保持するクラス
   */
  class STLext {

    enum {
      STLD_MODE_UNKNOWN=0,
      STLD_MODE_FACE,
      STLD_MODE_VERTEX
    };

  public:
    /**
     * @brief デフォルトコンストラクタ
     */
    STLext(void) : m_ID(0xFFFF), m_version(1), m_mode(STLD_MODE_UNKNOWN) {}

    /**
     * @brief 定義点のモードを面にする
     */
    void SetFaceMode(void) { m_mode = STLD_MODE_FACE; }

    /**
     * @brief 定義点のモードを頂点にする
     */
    void SetVertexMode(void) { m_mode = STLD_MODE_VERTEX; }

    /**
     * @brief 定義点のモードが面かどうか
     */
    bool IsFaceMode(void) const { return m_mode == STLD_MODE_FACE; }

    /**
     * @brief 定義点のモードが頂点かどうか
     */
    bool IsVertexMode(void) const { return m_mode == STLD_MODE_VERTEX; }

    /**
     * @brief ファイルからデータを読み込む
     * @param filePath ファイルパス
     * @return 読み込み失敗
     */
    bool Load(const std::string& filePath) {
      bool result(false);

      FILE* fp = fopen(filePath.c_str(), "rb");
      if (! fp) return result;

      /**
       * ファイルの識別子の読み込み
       */
      unsigned short id;
      if (fread(&id, sizeof(unsigned short), 1, fp) != 1) {
	if (fp) fclose(fp); return result;
      }
      if (id != m_ID) { if (fp) fclose(fp); return result; }

      /**
       * バージョン情報の読み込み
       */
      unsigned short version;
      if (fread(&version, sizeof(unsigned short), 1, fp) != 1) {
	if (fp) fclose(fp); return result;
      }

      /**
       * モードの読み込み
       */
      if (fread(&m_mode, sizeof(unsigned char), 1, fp) != 1) {
	if (fp) fclose(fp); return result;
      }

      /**
       * 物理量の読み込み
       */
      unsigned short physNum = 0;
      if (fread(&physNum, sizeof(unsigned short), 1, fp) != 1) {
	if (fp) fclose(fp); return result;
      }

      /**
       * データの種別とベクトル長の読み込み
       */
      for (size_t i = 0; i < physNum; ++i) {
	unsigned char data[2];
	if (fread(data, sizeof(unsigned char), 2, fp) != 2) {
	  if (fp) fclose(fp); return result;
	}
	AppendPhysInfo(PhysInfo(data[0], data[1]));
      }

      /**
       * ポリゴン数の読み込み
       */
      unsigned int faceNum = 0;
      if (fread(&faceNum, sizeof(unsigned int), 1, fp) != 1) {
	if (fp) fclose(fp); return result;
      }
      SetFaceNum(faceNum);

      for (size_t i = 0; i < faceNum; ++i) {
	for (size_t j = 0; j < physNum; ++j) {
	  const PhysInfo& physInfo = m_physInfoV[j];
	  unsigned char vecLen = physInfo.GetVecLen();

	  if (physInfo.IsInt()) {
	    int* data = new int[vecLen];
	    if (fread(data, sizeof(int), vecLen, fp) != vecLen) {
	      if (fp) fclose(fp); return result;
	    }
	    std::vector<int> values(vecLen);
	    for (size_t k = 0; k < vecLen; ++k) values[k] = data[k];
	    delete[] data;
	    if (! SetData(i, j, values)) {
	      if (fp) fclose(fp); return result;
	    }
	  } else if (physInfo.IsFloat()) {
	    float* data = new float[vecLen]; 
	    if (fread(data, sizeof(float), vecLen, fp) != vecLen) {
	      if (fp) fclose(fp); return result;
	    }
	    std::vector<float> values(vecLen);
	    for (size_t k = 0; k < vecLen; ++k) values[k] = data[k];
	    delete[] data;
	    if (! SetData(i, j, values)) {
	      if (fp) fclose(fp); return result;
	    }
	  } else if (physInfo.IsDouble()) {
	    double* data = new double[vecLen];
	    if (fread(data, sizeof(double), vecLen, fp) != vecLen) {
	      if (fp) fclose(fp); return result;
	    }
	    std::vector<double> values(vecLen);
	    for (size_t k = 0; k < vecLen; ++k) values[k] = data[k];
	    delete[] data;
	    if (! SetData(i, j, values)) {
	      if (fp) fclose(fp); return result;
	    }
	  }
	}
      }
      result = true;

      if (fp) fclose(fp);
      return result;
    }

    /**
     * @brief ファイルにデータを保存する
     * @param filePath ファイルパス
     * @return 書き込み失敗
     */
    bool Save(const std::string& filePath) const {
      bool result(false);
      
      FILE* fp = fopen(filePath.c_str(), "wb");
      if (! fp) return result;

      /**
       * 識別子の出力
       */
      if (fwrite(&m_ID, sizeof(unsigned short), 1, fp) != 1) {
	if (fp) fclose(fp); return result;
      }

      /**
       * バージョン番号の出力
       */
      if (fwrite(&m_version, sizeof(unsigned short), 1, fp) != 1) {
	if (fp) fclose(fp); return result;
      }

      /**
       * モードの出力
       */
      if (fwrite(&m_mode, sizeof(unsigned char), 1, fp) != 1) {
	if (fp) fclose(fp); return result;
      }

      /**
       * 物理量の出力
       */
      unsigned short physNum = m_physInfoV.size();
      if (fwrite(&physNum, sizeof(unsigned short), 1, fp) != 1) {
	if (fp) fclose(fp); return result;
      }

      /**
       * データの種別とベクトル長の出力
       */
      for (size_t i = 0; i < physNum; ++i) {
	unsigned char data[2];
	data[0] = m_physInfoV[i].GetDataKind();
	data[1] = m_physInfoV[i].GetVecLen();
	if (fwrite(data, sizeof(unsigned char), 2, fp) != 2) {
	  if (fp) fclose(fp); return result;
	}
      }

      /**
       * ポリゴン数の出力
       */
      unsigned int faceNum = m_faceInfoV.size();
      if (fwrite(&faceNum, sizeof(unsigned int), 1, fp) != 1) {
	if (fp) fclose(fp); return result;
      }

      /**
       *ポリゴン毎の物理量の出力
       */
      for (size_t i = 0; i < faceNum; ++i) {
	for (size_t j = 0; j < physNum; ++j) {
	  const PhysInfo& physInfo = m_physInfoV[j];
	  unsigned char vecLen = physInfo.GetVecLen();

	  if (physInfo.IsInt()) {
	    std::vector<int> values;
	    if (!(GetData(i, j, &values) && values.size() == vecLen)) {
	      if (fp) fclose(fp); return result;
	    }
	    int* data = new int[vecLen];
	    for (size_t k = 0; k < vecLen; ++k) data[k] = values[k];
	    if (fwrite(data, sizeof(int), vecLen, fp) != vecLen) {
	      if (fp) fclose(fp); return result;
	    }
	    delete[] data;
	  } else if (physInfo.IsFloat()) {
	    std::vector<float> values;
	    if (!(GetData(i, j, &values) && values.size() == vecLen)) {
	      if (fp) fclose(fp); return result;
	    }
	    float* data = new float[vecLen];
	    for (size_t k = 0; k < vecLen; ++k) data[k] = values[k];
	    if (fwrite(data, sizeof(float), vecLen, fp) != vecLen) {
	      if (fp) fclose(fp); return result;
	    }
	    delete[] data;
	  } else if (physInfo.IsDouble()) {
	    std::vector<double> values;
	    if (GetData(i, j, &values) && values.size() == vecLen) {
	      if (fp) fclose(fp); return result;
	    }
	    double* data = new double[vecLen];
	    for (size_t k = 0; k < vecLen; ++k) data[k] = values[k];
	    if (fwrite(data, sizeof(double), vecLen, fp) != vecLen) {
	      if (fp) fclose(fp); return result;
	    }
	    delete[] data;
	  }
	}
      }

      result = true;

      if (fp) fclose(fp);
      return result;
    }

    /**
     * @brief 物理量数の取得
     * @return 物理量数
     */
    size_t GetPhysNum(void) const { return m_physInfoV.size(); }

    /**
     * @brief physNum 番目のデータの種類を取得する
     * @param physNum 物理量番号。0から始める
     * @return データの種類。0(失敗), 1(int), 2(float), 3(double)。
     */
    unsigned char GetDataKind(size_t physNum) const {
      /**
       * 物理量番号が物理量数を超えていたらエラー
       */
      return (m_physInfoV.size() > physNum) ? \
	m_physInfoV[physNum].GetDataKind() : 0;
    }

    /**
     * @brief physNum 番目のデータのベクトル長を取得する
     * @param physNum 物理量番号。0から始める
     * @return データのベクトル長。0(失敗)。
     */
    unsigned char GetVecLen(size_t physNum) const {
      /**
       * 物理量番号が物理量数を超えていたらエラー
       */
      return (m_physInfoV.size() > physNum) ? \
	m_physInfoV[physNum].GetVecLen() : 0;
    }

    /**
     * @brief ポリゴンの数を取得する
     * @return ポリゴンの数
     */
    size_t GetFaceNum(void) const { return m_faceInfoV.size(); }

    /**
     * @brief データの取得(int)
     * @param faceNum ポリゴン番号、０から始める
     * @param physNum 物理量番号、０から始める
     * @param val データ
     * @retval true 取得に成功
     * @retval false 取得に失敗
     */
    bool GetData(size_t faceNum, size_t physNum, std::vector<int>* val) const {
      /**
       * ポリゴン番号がポリゴン数を超えていたらエラー。
       * 物理量番号が物理量数を超えていたらエラー。
       * 物理量番号がintでなければエラー。
       */
      bool result(false);
      if (m_faceInfoV.size() > faceNum &&
	  m_physInfoV.size() > physNum &&
	  m_physInfoV[physNum].IsInt() &&
	  val) {
	const FaceInfo& faceInfo = m_faceInfoV[faceNum];
	const PhysInfo& physInfo = m_physInfoV[physNum];

	unsigned int prevLen = 0;
	if (physNum > 0) {
	  for (size_t i = 0; i < physNum - 1; ++i) {
	    if (m_physInfoV[i].IsInt()) {
	      prevLen += m_physInfoV[i].GetVecLen();
	    }
	  }
	}

	/**
	 * 物理量番号のベクトル長分のデータが設定される
	 */
	unsigned int len = physInfo.GetVecLen();
	val -> clear();
	val -> resize(len);
	for (size_t i = 0; i < len; ++i) {
	  (*val)[i] = faceInfo.GetIntVal(prevLen + i);
	}
	result = true;
      } 
      return result;
    }

    /**
     * @brief データの取得(float)
     * @param faceNum ポリゴン番号、０から始める
     * @param physNum 物理量番号、０から始める
     * @param val データ
     * @retval true 取得に成功
     * @retval false 取得に失敗
     */
    bool GetData(size_t faceNum, size_t physNum, std::vector<float>* val) const
    {
      /**
       * ポリゴン番号がポリゴン数を超えていたらエラー。
       * 物理量番号が物理量数を超えていたらエラー。
       * 物理量番号がfloatでなければエラー。
       */
      bool result(false);
      if (m_faceInfoV.size() > faceNum &&
	  m_physInfoV.size() > physNum &&
	  m_physInfoV[physNum].IsFloat() &&
	  val) {
	const FaceInfo& faceInfo = m_faceInfoV[faceNum];
	const PhysInfo& physInfo = m_physInfoV[physNum];

	unsigned int prevLen = 0;
	if (physNum > 0) {
	  for (size_t i = 0; i < physNum - 1; ++i) {
	    if (m_physInfoV[i].IsFloat()) {
	      prevLen += m_physInfoV[i].GetVecLen();
	    }
	  }
	}

	/**
	 * 物理量番号のベクトル長分のデータが設定される
	 */
	unsigned int len = physInfo.GetVecLen();
	val -> clear();
	val -> resize(len);
	for (size_t i = 0; i < len; ++i) { (*val)[i] = faceInfo.GetFloatVal(prevLen + i); }

	result = true;
      } 
      return result;
    }

    /**
     * @brief データの取得(double)
     * @param faceNum ポリゴン番号、０から始める
     * @param physNum 物理量番号、０から始める
     * @param val データ
     * @retval true 取得に成功
     * @retval false 取得に失敗
     */
    bool GetData(size_t faceNum, size_t physNum, std::vector<double>* val) const
    {
      /**
       * ポリゴン番号がポリゴン数を超えていたらエラー。
       * 物理量番号が物理量数を超えていたらエラー。
       * 物理量番号がintでなければエラー。
       */
      bool result(false);
      if (m_faceInfoV.size() > faceNum &&
	  m_physInfoV.size() > physNum &&
	  m_physInfoV[physNum].IsDouble() &&
	  val) {
	const FaceInfo& faceInfo = m_faceInfoV[faceNum];
	const PhysInfo& physInfo = m_physInfoV[physNum];
	
	unsigned int prevLen = 0;
	if (physNum > 0) {
	  for (size_t i = 0; i < physNum - 1; ++i) {
	    if (m_physInfoV[i].IsDouble()) {
	      prevLen += m_physInfoV[i].GetVecLen();
	    }
	  }
	}

	/**
	 * 物理量番号のベクトル長分のデータが設定される
	 */
	unsigned int len = physInfo.GetVecLen();
	val -> clear();
	val -> resize(len);
	for (size_t i = 0; i < len; ++i) {
	  (*val)[i] = faceInfo.GetDoubleVal(prevLen + i);
	}
	result = true;
      } 
      return result;
    }

    /**
     * @brief データのクリア
     */
    void Clear(void) {
      /**
       * 物理量情報のクリア
       */
      m_physInfoV.clear();
      /**
       * ポリゴン情報のクリア
       */
      m_faceInfoV.clear();
    }

    /**
     * @brief 物理量情報の追加
     * @param physInfo 物理量情報
     * @return 物理量番号
     */
    size_t AppendPhysInfo(const PhysInfo& physInfo) {
      m_physInfoV.push_back(physInfo);
      return m_physInfoV.size() - 1;
    }
    
    /**
     * @brief ポリゴンの数の設定
     * @param faceNum ポリゴンの数
     */
    void SetFaceNum(size_t faceNum) {
      m_faceInfoV.clear();
      m_faceInfoV.resize(faceNum);
    }

    /**
     * @brief データの設定(int)
     * @param faceNum ポリゴン番号
     * @param physNum 物理量番号
     * @param val 設定する値
     * @retval true 設定に成功
     * @retval false 設定に失敗
     */
    bool SetData(size_t faceNum, size_t physNum, const std::vector<int>& val) {
      bool result(false);
      /**
       * ポリゴン番号がポリゴン数を超えていたらエラー。
       * 物理量番号が物理量数を超えていたらエラー。
       * 物理量番号がintでなければエラー。
       * 物理量番号のベクトル長と値のベクトル長が一致しなければエラー。
       */
      if (m_faceInfoV.size() > faceNum &&
	  m_physInfoV.size() > physNum &&
	  m_physInfoV[physNum].IsInt() &&
	  m_physInfoV[physNum].GetVecLen() == val.size()) {
      
	size_t prevLen = 0;
	if (physNum > 0) {
	  for (size_t i = 0; i < physNum - 1; ++i) {
	    if (m_physInfoV[i].IsInt()) prevLen += m_physInfoV[i].GetVecLen();
	  }
	}

	for (size_t i = 0; i < val.size(); ++i) {
	  m_faceInfoV[faceNum].SetVal(prevLen + i, val[i]);
	}

	result = true;
      }
      return result;
    }

    /**
     * @brief データの設定(float)
     * @param faceNum ポリゴン番号
     * @param physNum 物理量番号
     * @param val 設定する値
     * @retval true 設定に成功
     * @retval false 設定に失敗
     */
    bool SetData(size_t faceNum, size_t physNum, const std::vector<float>& val)
    {
      /**
       * ポリゴン番号がポリゴン数を超えていたらエラー。
       * 物理量番号が物理量数を超えていたらエラー。
       * 物理量番号がfloatでなければエラー。
       * 物理量番号のベクトル長と値のベクトル長が一致しなければエラー。
       */
      bool result(false);
      if (m_faceInfoV.size() > faceNum &&
	  m_physInfoV.size() > physNum &&
	  m_physInfoV[physNum].IsFloat() &&
	  m_physInfoV[physNum].GetVecLen() == val.size()) {
      
	size_t prevLen = 0;
	if (physNum > 0) {
	  for (size_t i = 0; i < physNum - 1; ++i) {
	    if (m_physInfoV[i].IsFloat()) prevLen += m_physInfoV[i].GetVecLen();
	  }
	}

	for (size_t i = 0; i < val.size(); ++i) {
	  m_faceInfoV[faceNum].SetVal(prevLen + i, val[i]);
	}

	result = true;
      }
      return result;
    }

    /**
     * @brief データの設定(double)
     * @param faceNum ポリゴン番号
     * @param physNum 物理量番号
     * @param val 設定する値
     * @retval true 設定に成功
     * @retval false 設定に失敗
     */
    bool SetData(size_t faceNum, size_t physNum, const std::vector<double>& val)
    {
      /**
       * ポリゴン番号がポリゴン数を超えていたらエラー。
       * 物理量番号が物理量数を超えていたらエラー。
       * 物理量番号がdoubleでなければエラー。
       * 物理量番号のベクトル長と値のベクトル長が一致しなければエラー。
       */
      bool result(false);
      if (m_faceInfoV.size() > faceNum &&
	  m_physInfoV.size() > physNum &&
	  m_physInfoV[physNum].IsFloat() &&
	  m_physInfoV[physNum].GetVecLen() == val.size()) {
      
	size_t prevLen = 0;
	if (physNum > 0) {
	  for (size_t i = 0; i < physNum - 1; ++i) {
	    if (m_physInfoV[i].IsDouble()) {
	      prevLen += m_physInfoV[i].GetVecLen();
	    }
	  }
	}

	for (size_t i = 0; i < val.size(); ++i) {
	  m_faceInfoV[faceNum].SetVal(prevLen + i, val[i]);
	}

	result = true;
      }
      return result;
    }

    /**
     * @brief 情報出力
     *
     * 主にデバッグ用
     */
    void PrettyPrint(std::ostream& os) const {
      os << "ID = " << m_ID << std::endl;
      os << "Version = " << m_version << std::endl;
      std::string modeStr = (m_mode == STLD_MODE_FACE) ? \
	"Face" : (m_mode == STLD_MODE_FACE) ? "Vertex" : "Unknown";
      os << "Mode = " << modeStr << std::endl;
      os << "PhysNum = " << m_physInfoV.size() << std::endl;
      for (size_t i = 0; i < m_physInfoV.size(); ++i) {
	os << "  ";
	os << m_physInfoV[i].GetDataKindStr() << " "
	   << m_physInfoV[i].GetVecLenStr();
	os << std::endl;
      }
      os << "FaceNum = " << m_faceInfoV.size() << std::endl;
      for (size_t i = 0; i < m_faceInfoV.size(); ++i) {
	os << "  ";
	for (size_t j = 0; j < m_physInfoV.size(); ++j) {
	  const PhysInfo& physInfo = m_physInfoV[j];

	  if (j != 0) os << ", ";

	  if (physInfo.IsInt()) {
	    std::vector<int> values;
	    GetData(i, j, &values);
	    if (values.size() == 1) {
	      os << values[0];
	    } else if (values.size() == 3) {
	      os << "(" << values[0] << ", " << values[1] << ", "
		 << values[2] << ")";
	    } else if (values.size() == 9) {
	      os << "("
		 << values[0] << ", " << values[1] << ", " << values[2] << ", "
		 << values[3] << ", " << values[4] << ", " << values[5] << ", " 
		 << values[6] << ", " << values[7] << ", " << values[8]
		 << ")";
	    }

	  } else if (physInfo.IsFloat()) {
	    std::vector<float> values;
	    GetData(i, j, &values);
	    if (values.size() == 1) {
	      os << values[0];
	    } else if (values.size() == 3) {
	      os << "(" << values[0] << ", " << values[1] << ", "
		 << values[2] << ")";
	    } else if (values.size() == 9) {
	      os << "("
		 << values[0] << ", " << values[1] << ", " << values[2] << ", "
		 << values[3] << ", " << values[4] << ", " << values[5] << ", " 
		 << values[6] << ", " << values[7] << ", " << values[8]
		 << ")";
	    }
	  } else if (physInfo.IsDouble()) {
	    std::vector<double> values;
	    GetData(i, j, &values);
	    if (values.size() == 1) {
	      os << values[0];
	    } else if (values.size() == 3) {
	      os << "(" << values[0] << ", " << values[1] << ", "
		 << values[2] << ")";
	    } else if (values.size() == 9) {
	      os << "("
		 << values[0] << ", " << values[1] << ", " << values[2] << ", "
		 << values[3] << ", " << values[4] << ", " << values[5] << ", " 
		 << values[6] << ", " << values[7] << ", " << values[8]
		 << ")";
	    }
	  }
	}
	os << std::endl;
      }
    }

    const std::vector<PhysInfo>& GetPhysInfoV(void) const {
      return m_physInfoV;
    }

  private:
    /**
     * @brief ファイルの識別子
     */
    unsigned short m_ID;

    /**
     * @brief バージョン番号
     */
    unsigned short m_version;

    /**
     * @brief 値の定義点が面の頂点かのフラグ
     * 1 は定義点が面
     * 2 は定義点が頂点
     */
    unsigned char m_mode;

    /**
     * @brief 物理量の情報の配列
     *
     * ポリゴン毎に、この並びで物理量が格納される
     */
    std::vector<PhysInfo> m_physInfoV;

    /**
     * @brief ポリゴンの物理量の配列
     */
    std::vector<FaceInfo> m_faceInfoV;
  };
};
