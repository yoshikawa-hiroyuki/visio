#ifndef TEMPLATE_NAME
# error please define "TEMPLATE_NAME"
#endif
#ifndef VECTOR_TEMPLATE_NAME
# error please define "VECTOR_TEMPLATE_NAME"
#endif
#ifndef VECTOR0_NAME
# error please define "VECTOR0_NAME"
#endif
#ifndef VECTOR1_NAME
# error please define "VECTOR1_NAME"
#endif


#if WIN32
#pragma warning(push)
#pragma warning(disable: 4244)
#endif


template <typename value_type>
class TEMPLATE_NAME
{
public:
	typedef value_type ValueType;
	typedef VECTOR_TEMPLATE_NAME<ValueType> VectorType;

	VectorType VECTOR0_NAME;
	VectorType VECTOR1_NAME;

	// 定数
	static const TEMPLATE_NAME O; // 零行列
	static const TEMPLATE_NAME I; // 単位行列

	// デフォルトコンストラクタ(初期化しない)
	TEMPLATE_NAME() {}

	// 値を四つ指定して初期化
	TEMPLATE_NAME(
		ValueType m00, ValueType m10,
		ValueType m01, ValueType m11
	) : VECTOR0_NAME(m00, m10), VECTOR1_NAME(m01, m11) {}
    // ベクトルを二つ指定して初期化
	TEMPLATE_NAME(const VectorType &vx0, const VectorType &vy0)
		: VECTOR0_NAME(vx0), VECTOR1_NAME(vy0) {}
	// 配列を指定して初期化
	TEMPLATE_NAME(const ValueType m[4])
		: VECTOR0_NAME(m[0],m[1]), VECTOR1_NAME(m[2],m[3]) {}

	// 他のTEMPLATE_NAME<>型からコピーコンストラクト
	template <typename ValType2> TEMPLATE_NAME(const TEMPLATE_NAME<ValType2> &m) {VECTOR0_NAME = VectorType(m.VECTOR0_NAME); VECTOR1_NAME = VectorType(m.VECTOR1_NAME);}

	// アクセサ
	VectorType &operator[](int i) {return (&VECTOR0_NAME)[i];}
	const VectorType &operator[](int i) const {return (&VECTOR0_NAME)[i];}
	VectorType raw(int i) const {return VectorType(VECTOR0_NAME[i], VECTOR1_NAME[i]);}
	VectorType column(int i) const {return (&VECTOR0_NAME)[i];}
	ValueType &operator()(int raw, int column) {return (*this)[column][raw];}
	const ValueType &operator()(int raw, int column) const {return (*this)[column][raw];}

	// 配列から読み込む
	TEMPLATE_NAME &load(const ValueType v[4]) {
		VECTOR0_NAME.load(v);
		VECTOR1_NAME.load(v+2);
		return *this;
	}
	// 配列に書き込む
	void store(ValueType v[4]) const {
		VECTOR0_NAME.store(v);
		VECTOR1_NAME.store(v+2);
	}
	// 配列[16]から読み込む
	TEMPLATE_NAME &load44(const ValueType v[16]) {
		VECTOR0_NAME.load(v);
		VECTOR1_NAME.load(v+4);
		return *this;
	}
	// 配列[16]に書き込む
	void store44(ValueType v[16]) const {
		VECTOR0_NAME.store(v);
		VECTOR1_NAME.store(v+4);
	}

	// 等価演算子
	bool operator==(const TEMPLATE_NAME<ValueType> &m) const {
		return VECTOR0_NAME == m.VECTOR0_NAME && VECTOR1_NAME == m.VECTOR1_NAME;
	}
	// 非等価演算子
	bool operator!=(const TEMPLATE_NAME<ValueType> &m) const {
		return !operator==(m);
	}

	// 別の行列を加算
	TEMPLATE_NAME &operator+=(const TEMPLATE_NAME &m) {
		VECTOR0_NAME += m.VECTOR0_NAME; VECTOR1_NAME += m.VECTOR1_NAME;
		return *this;
	}
	// 別の行列を減算
	TEMPLATE_NAME &operator-=(const TEMPLATE_NAME &m) {
		VECTOR0_NAME -= m.VECTOR0_NAME; VECTOR1_NAME -= m.VECTOR1_NAME;
		return *this;
	}

	// スカラ型を乗算
	template <typename ValueType2>
	TEMPLATE_NAME &operator*=(ValueType2 s) {
		VECTOR0_NAME *= s; VECTOR1_NAME *= s;
		return *this;
	}
	// スカラ型で除算
	template <typename ValueType2>
	TEMPLATE_NAME &operator/=(ValueType2 s) {
		const ValueType s2 = (ValueType)1 / s;
		VECTOR0_NAME *= s2; VECTOR1_NAME *= s2;
		return *this;
	}

	// 単項+演算子
	const TEMPLATE_NAME operator+() const {
		return TEMPLATE_NAME(*this);
	}
	// 単項-演算子
	const TEMPLATE_NAME operator-() const {
		return TEMPLATE_NAME(-VECTOR0_NAME, -VECTOR1_NAME);
	}

	// 行列式を求める
	ValueType determinant() const {
		return VECTOR0_NAME[0] * VECTOR1_NAME[1] - VECTOR0_NAME[1] * VECTOR1_NAME[0];
	}
	ValueType det() const {return determinant();}
	
	// 正規化する
	TEMPLATE_NAME &normalize() {
		VECTOR0_NAME.normalize();
		VECTOR1_NAME.normalize();
		return *this;
	}

	// 回転行列を作る
	static TEMPLATE_NAME rotate(ValueType angle) {
		static const ValueType PI = ValueType(3.1415926535897932384626);
		static const ValueType DEG_TO_RAD = PI / 180;
		const ValueType rad = angle * DEG_TO_RAD;
		const ValueType s = (ValueType)sin((double)rad);
		const ValueType c = (ValueType)cos((double)rad);
		return TEMPLATE_NAME(c, s, -s, c);
	}

	// 拡大縮小行列を作る
	static TEMPLATE_NAME scale(ValueType x, ValueType y) {
		return TEMPLATE_NAME(x, 0, 0, y);
	}
	static TEMPLATE_NAME scale(const VECTOR_TEMPLATE_NAME<ValueType> &v) {
		return scale(v[0], v[1]);
	}
	static TEMPLATE_NAME scale(ValueType s) {
		return scale(s, s);
	}

};

// 定数
template <typename ValueType> const TEMPLATE_NAME<ValueType> TEMPLATE_NAME<ValueType>::O(VectorType(0, 0), VectorType(0, 0));
template <typename ValueType> const TEMPLATE_NAME<ValueType> TEMPLATE_NAME<ValueType>::I(VectorType(1, 0), VectorType(0, 1));


// 整数系用スカラ除算
template<> template <typename ValueType2> TEMPLATE_NAME<char> &TEMPLATE_NAME<char>::operator/=(ValueType2 s) {VECTOR0_NAME /= s; VECTOR1_NAME /= s; return *this;}
template<> template <typename ValueType2> TEMPLATE_NAME<unsigned char> &TEMPLATE_NAME<unsigned char>::operator/=(ValueType2 s) {VECTOR0_NAME /= s; VECTOR1_NAME /= s; return *this;}
template<> template <typename ValueType2> TEMPLATE_NAME<short> &TEMPLATE_NAME<short>::operator/=(ValueType2 s) {VECTOR0_NAME /= s; VECTOR1_NAME /= s; return *this;}
template<> template <typename ValueType2> TEMPLATE_NAME<unsigned short> &TEMPLATE_NAME<unsigned short>::operator/=(ValueType2 s) {VECTOR0_NAME /= s; VECTOR1_NAME /= s; return *this;}
template<> template <typename ValueType2> TEMPLATE_NAME<int> &TEMPLATE_NAME<int>::operator/=(ValueType2 s) {VECTOR0_NAME /= s; VECTOR1_NAME /= s; return *this;}
template<> template <typename ValueType2> TEMPLATE_NAME<unsigned int> &TEMPLATE_NAME<unsigned int>::operator/=(ValueType2 s) {VECTOR0_NAME /= s; VECTOR1_NAME /= s; return *this;}
#if WIN32
template<> template <typename ValueType2> TEMPLATE_NAME<__int64> &TEMPLATE_NAME<__int64>::operator/=(ValueType2 s) {VECTOR0_NAME /= s; VECTOR1_NAME /= s; return *this;}
template<> template <typename ValueType2> TEMPLATE_NAME<unsigned __int64> &TEMPLATE_NAME<unsigned __int64>::operator/=(ValueType2 s) {VECTOR0_NAME /= s; VECTOR1_NAME /= s; return *this;}
#else
template<> template <typename ValueType2> TEMPLATE_NAME<long long> &TEMPLATE_NAME<long long>::operator/=(ValueType2 s) {VECTOR0_NAME /= s; VECTOR1_NAME /= s; return *this;}
template<> template <typename ValueType2> TEMPLATE_NAME<unsigned long long> &TEMPLATE_NAME<unsigned long long>::operator/=(ValueType2 s) {VECTOR0_NAME /= s; VECTOR1_NAME /= s; return *this;}
#endif

// 行列の和
template <typename ValueType>
inline TEMPLATE_NAME<ValueType> operator+(const TEMPLATE_NAME<ValueType> &m0, const TEMPLATE_NAME<ValueType> &m1) {
	return TEMPLATE_NAME<ValueType>(m0.VECTOR0_NAME + m1.VECTOR0_NAME, m0.VECTOR1_NAME + m1.VECTOR1_NAME);
}

// 行列の差
template <typename ValueType>
inline TEMPLATE_NAME<ValueType> operator-(const TEMPLATE_NAME<ValueType> &m0, const TEMPLATE_NAME<ValueType> &m1) {
	return TEMPLATE_NAME<ValueType>(m0.VECTOR0_NAME - m1.VECTOR0_NAME, m0.VECTOR1_NAME - m1.VECTOR1_NAME);
}

// 行列の積
template <typename ValueType>
inline TEMPLATE_NAME<ValueType> operator*(const TEMPLATE_NAME<ValueType> &m1, const TEMPLATE_NAME<ValueType> &m2) {
	typedef typename TEMPLATE_NAME<ValueType>::VectorType VectorType;
	const VectorType rv0(m1.VECTOR0_NAME[0], m1.VECTOR1_NAME[0]);
	const VectorType rv1(m1.VECTOR0_NAME[1], m1.VECTOR1_NAME[1]);
	return TEMPLATE_NAME<ValueType>(
		dot(rv0, m2.VECTOR0_NAME), dot(rv1, m2.VECTOR0_NAME),
		dot(rv0, m2.VECTOR1_NAME), dot(rv1, m2.VECTOR1_NAME)
	);
}

// スカラとの積
template <typename ValueType>
inline TEMPLATE_NAME<ValueType> operator*(ValueType s, TEMPLATE_NAME<ValueType> m) {
	return m *= s;
}
template <typename ValueType>
inline TEMPLATE_NAME<ValueType> operator*(TEMPLATE_NAME<ValueType> m, ValueType s) {
	return m *= s;
}

// ベクトルの変換
template <typename ValueType>
inline VECTOR_TEMPLATE_NAME<ValueType> operator*(const TEMPLATE_NAME<ValueType> &m, const VECTOR_TEMPLATE_NAME<ValueType> &v) {
	typedef typename TEMPLATE_NAME<ValueType>::VectorType VectorType;
	return VectorType(
		m.VECTOR0_NAME[0] * v[0] + m.VECTOR1_NAME[0] * v[1],
		m.VECTOR0_NAME[1] * v[0] + m.VECTOR1_NAME[1] * v[1]
	);
}

// 転置行列
template <typename ValueType>
TEMPLATE_NAME<ValueType> transpose(const TEMPLATE_NAME<ValueType> &m) {
	typedef typename TEMPLATE_NAME<ValueType>::VectorType VectorType;
	return TEMPLATE_NAME<ValueType>(
		m.VECTOR0_NAME[0], m.VECTOR1_NAME[0],
		m.VECTOR0_NAME[1], m.VECTOR1_NAME[1]
	);
}

// 逆行列
template <typename ValueType>
TEMPLATE_NAME<ValueType> inverse(const TEMPLATE_NAME<ValueType> &m) {
	typedef typename TEMPLATE_NAME<ValueType>::VectorType VectorType;
	const ValueType det = m.determinant();
	if (det == 0) return TEMPLATE_NAME<ValueType>(VectorType(0, 0), VectorType(0, 0));
	const ValueType det_r = ValueType(1) / det;
	return det_r * TEMPLATE_NAME<ValueType>(VectorType(m.VECTOR1_NAME[1], -m.VECTOR0_NAME[1]), VectorType(-m.VECTOR1_NAME[0], m.VECTOR0_NAME[0]));
}


#if WIN32
#pragma warning(pop)
#endif
