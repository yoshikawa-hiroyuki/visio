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
#ifndef VECTOR2_NAME
# error please define "VECTOR2_NAME"
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
	VectorType VECTOR2_NAME;

	// 定数
	static const TEMPLATE_NAME O; // 零行列
	static const TEMPLATE_NAME I; // 単位行列

	// デフォルトコンストラクタ(初期化しない)
	TEMPLATE_NAME() {}

	// 値を九つ指定して初期化
	TEMPLATE_NAME(
		ValueType m00, ValueType m10, ValueType m20,
		ValueType m01, ValueType m11, ValueType m21,
		ValueType m02, ValueType m12, ValueType m22
	) : VECTOR0_NAME(m00, m10, m20), VECTOR1_NAME(m01, m11, m21), VECTOR2_NAME(m02, m12, m22) {}
    // ベクトルを三つ指定して初期化
	TEMPLATE_NAME(const VectorType &vx0, const VectorType &vy0, const VectorType &vz0)
		: VECTOR0_NAME(vx0), VECTOR1_NAME(vy0), VECTOR2_NAME(vz0) {}
	// 配列を指定して初期化
	TEMPLATE_NAME(const ValueType m[9])
		: VECTOR0_NAME(m[0],m[1],m[2]), VECTOR1_NAME(m[3],m[4],m[5]), VECTOR2_NAME(m[6],m[7],m[8]) {}

	// 他のTEMPLATE_NAME<>型からコピーコンストラクト
	template <typename ValType2> TEMPLATE_NAME(const TEMPLATE_NAME<ValType2> &m) {VECTOR0_NAME = VectorType(m.VECTOR0_NAME); VECTOR1_NAME = VectorType(m.VECTOR1_NAME); VECTOR2_NAME = VectorType(m.VECTOR2_NAME);}

	// アクセサ
	VectorType &operator[](int i) {return (&VECTOR0_NAME)[i];}
	const VectorType &operator[](int i) const {return (&VECTOR0_NAME)[i];}
	VectorType raw(int i) const {return VectorType(VECTOR0_NAME[i], VECTOR1_NAME[i], VECTOR2_NAME[i]);}
	VectorType column(int i) const {return (&VECTOR0_NAME)[i];}
	ValueType &operator()(int raw, int column) {return (*this)[column][raw];}
	const ValueType &operator()(int raw, int column) const {return (*this)[column][raw];}

	// 配列から読み込む
	TEMPLATE_NAME &load(const ValueType v[9]) {
		VECTOR0_NAME.load(v);
		VECTOR1_NAME.load(v+3);
		VECTOR2_NAME.load(v+6);
		return *this;
	}
	// 配列に書き込む
	void store(ValueType v[9]) const {
		VECTOR0_NAME.store(v);
		VECTOR1_NAME.store(v+3);
		VECTOR2_NAME.store(v+6);
	}
	// 配列[16]から読み込む
	TEMPLATE_NAME &load44(const ValueType v[16]) {
		VECTOR0_NAME.load(v);
		VECTOR1_NAME.load(v+4);
		VECTOR2_NAME.load(v+8);
		return *this;
	}
	// 配列[16]に書き込む
	void store44(ValueType v[16]) const {
		VECTOR0_NAME.store(v);
		VECTOR1_NAME.store(v+4);
		VECTOR2_NAME.store(v+8);
	}

	// 等価演算子
	bool operator==(const TEMPLATE_NAME<ValueType> &m) const {
		return VECTOR0_NAME == m.VECTOR0_NAME && VECTOR1_NAME == m.VECTOR1_NAME && VECTOR2_NAME == m.VECTOR2_NAME;
	}
	// 非等価演算子
	bool operator!=(const TEMPLATE_NAME<ValueType> &m) const {
		return !operator==(m);
	}

	// 別の行列を加算
	TEMPLATE_NAME &operator+=(const TEMPLATE_NAME &m) {
		VECTOR0_NAME += m.VECTOR0_NAME; VECTOR1_NAME += m.VECTOR1_NAME; VECTOR2_NAME += m.VECTOR2_NAME;
		return *this;
	}
	// 別の行列を減算
	TEMPLATE_NAME &operator-=(const TEMPLATE_NAME &m) {
		VECTOR0_NAME -= m.VECTOR0_NAME; VECTOR1_NAME -= m.VECTOR1_NAME; VECTOR2_NAME -= m.VECTOR2_NAME;
		return *this;
	}

	// スカラ型を乗算
	template <typename ValueType2>
	TEMPLATE_NAME &operator*=(ValueType2 s) {
		VECTOR0_NAME *= s; VECTOR1_NAME *= s; VECTOR2_NAME *= s;
		return *this;
	}
	// スカラ型で除算
	template <typename ValueType2>
	TEMPLATE_NAME &operator/=(ValueType2 s) {
		const ValueType s2 = (ValueType)1 / s;
		VECTOR0_NAME *= s2; VECTOR1_NAME *= s2; VECTOR2_NAME *= s2;
		return *this;
	}

	// 単項+演算子
	const TEMPLATE_NAME operator+() const {
		return TEMPLATE_NAME(*this);
	}
	// 単項-演算子
	const TEMPLATE_NAME operator-() const {
		return TEMPLATE_NAME(-VECTOR0_NAME, -VECTOR1_NAME, -VECTOR2_NAME);
	}

	// 行列式を求める
	ValueType determinant() const {
		return 
			VECTOR0_NAME[0] * VECTOR1_NAME[1] * VECTOR2_NAME[2] +
			VECTOR0_NAME[1] * VECTOR1_NAME[2] * VECTOR2_NAME[0] +
			VECTOR0_NAME[2] * VECTOR1_NAME[0] * VECTOR2_NAME[1] -
			VECTOR0_NAME[0] * VECTOR1_NAME[2] * VECTOR2_NAME[1] -
			VECTOR0_NAME[2] * VECTOR1_NAME[1] * VECTOR2_NAME[0] -
			VECTOR0_NAME[1] * VECTOR1_NAME[0] * VECTOR2_NAME[2];
	}
	ValueType det() const {return determinant();}
	
	// 正規化する
	TEMPLATE_NAME &normalize() {
		VECTOR2_NAME = cross(VECTOR0_NAME, VECTOR1_NAME);
		VECTOR1_NAME = cross(VECTOR2_NAME, VECTOR0_NAME);
		VECTOR0_NAME.normalize();
		VECTOR1_NAME.normalize();
		VECTOR2_NAME.normalize();
		return *this;
	}

	// 回転行列を作る
	static TEMPLATE_NAME rotate(ValueType angle, const VECTOR_TEMPLATE_NAME<ValueType> &axis) {
		static const ValueType PI = ValueType(3.1415926535897932384626);
		static const ValueType DEG_TO_RAD = PI / 180;
		const ValueType rad = angle * DEG_TO_RAD;
		const ValueType s = (ValueType)sin((double)rad);
		const ValueType c = (ValueType)cos((double)rad);
		const ValueType c1 = 1 - c;
		const ValueType l = axis.length();
		if (l > 0) {
			const ValueType x = axis[0] / l;
			const ValueType y = axis[1] / l;
			const ValueType z = axis[2] / l;
			return TEMPLATE_NAME(
				x*x*c1+c, y*x*c1+z*s, x*z*c1-y*s,
				x*y*c1-z*s, y*y*c1+c, y*z*c1+x*s,
				x*z*c1+y*s, y*z*c1-x*s, z*z*c1+c
			);
		} else {
			return TEMPLATE_NAME<ValueType>::O;
		}
	}
	static TEMPLATE_NAME rotate(ValueType angle, ValueType ax, ValueType ay, ValueType az) {
		return rotate(angle, VectorType(ax, ay, az));
	}

	// 拡大縮小行列を作る
	static TEMPLATE_NAME scale(ValueType x, ValueType y, ValueType z) {
		return TEMPLATE_NAME(x,0,0, 0,y,0, 0,0,z);
	}
	static TEMPLATE_NAME scale(const VECTOR_TEMPLATE_NAME<ValueType> &v) {
		return scale(v[0], v[1], v[2]);
	}
	static TEMPLATE_NAME scale(ValueType s) {
		return scale(s, s, s);
	}

};

// 定数
template <typename ValueType> const TEMPLATE_NAME<ValueType> TEMPLATE_NAME<ValueType>::O(VectorType(0, 0, 0), VectorType(0, 0, 0), VectorType(0, 0, 0));
template <typename ValueType> const TEMPLATE_NAME<ValueType> TEMPLATE_NAME<ValueType>::I(VectorType(1, 0, 0), VectorType(0, 1, 0), VectorType(0, 0, 1));


// 整数系用スカラ除算
template<> template <typename ValueType2> TEMPLATE_NAME<char> &TEMPLATE_NAME<char>::operator/=(ValueType2 s) {VECTOR0_NAME /= s; VECTOR1_NAME /= s; VECTOR2_NAME /= s; return *this;}
template<> template <typename ValueType2> TEMPLATE_NAME<unsigned char> &TEMPLATE_NAME<unsigned char>::operator/=(ValueType2 s) {VECTOR0_NAME /= s; VECTOR1_NAME /= s; VECTOR2_NAME /= s; return *this;}
template<> template <typename ValueType2> TEMPLATE_NAME<short> &TEMPLATE_NAME<short>::operator/=(ValueType2 s) {VECTOR0_NAME /= s; VECTOR1_NAME /= s; VECTOR2_NAME /= s; return *this;}
template<> template <typename ValueType2> TEMPLATE_NAME<unsigned short> &TEMPLATE_NAME<unsigned short>::operator/=(ValueType2 s) {VECTOR0_NAME /= s; VECTOR1_NAME /= s; VECTOR2_NAME /= s; return *this;}
template<> template <typename ValueType2> TEMPLATE_NAME<int> &TEMPLATE_NAME<int>::operator/=(ValueType2 s) {VECTOR0_NAME /= s; VECTOR1_NAME /= s; VECTOR2_NAME /= s; return *this;}
template<> template <typename ValueType2> TEMPLATE_NAME<unsigned int> &TEMPLATE_NAME<unsigned int>::operator/=(ValueType2 s) {VECTOR0_NAME /= s; VECTOR1_NAME /= s; VECTOR2_NAME /= s; return *this;}
#if WIN32
template<> template <typename ValueType2> TEMPLATE_NAME<__int64> &TEMPLATE_NAME<__int64>::operator/=(ValueType2 s) {VECTOR0_NAME /= s; VECTOR1_NAME /= s; VECTOR2_NAME /= s; return *this;}
template<> template <typename ValueType2> TEMPLATE_NAME<unsigned __int64> &TEMPLATE_NAME<unsigned __int64>::operator/=(ValueType2 s) {VECTOR0_NAME /= s; VECTOR1_NAME /= s; VECTOR2_NAME /= s; return *this;}
#else
template<> template <typename ValueType2> TEMPLATE_NAME<long long> &TEMPLATE_NAME<long long>::operator/=(ValueType2 s) {VECTOR0_NAME /= s; VECTOR1_NAME /= s; VECTOR2_NAME /= s; return *this;}
template<> template <typename ValueType2> TEMPLATE_NAME<unsigned long long> &TEMPLATE_NAME<unsigned long long>::operator/=(ValueType2 s) {VECTOR0_NAME /= s; VECTOR1_NAME /= s; VECTOR2_NAME /= s; return *this;}
#endif

// 行列の和
template <typename ValueType>
inline TEMPLATE_NAME<ValueType> operator+(const TEMPLATE_NAME<ValueType> &m0, const TEMPLATE_NAME<ValueType> &m1) {
	return TEMPLATE_NAME<ValueType>(m0.VECTOR0_NAME + m1.VECTOR0_NAME, m0.VECTOR1_NAME + m1.VECTOR1_NAME, m0.VECTOR2_NAME + m1.VECTOR2_NAME);
}

// 行列の差
template <typename ValueType>
inline TEMPLATE_NAME<ValueType> operator-(const TEMPLATE_NAME<ValueType> &m0, const TEMPLATE_NAME<ValueType> &m1) {
	return TEMPLATE_NAME<ValueType>(m0.VECTOR0_NAME - m1.VECTOR0_NAME, m0.VECTOR1_NAME - m1.VECTOR1_NAME, m0.VECTOR2_NAME - m1.VECTOR2_NAME);
}

// 行列の積
template <typename ValueType>
inline TEMPLATE_NAME<ValueType> operator*(const TEMPLATE_NAME<ValueType> &m1, const TEMPLATE_NAME<ValueType> &m2) {
	typedef typename TEMPLATE_NAME<ValueType>::VectorType VectorType;
	const VectorType rv0(m1.VECTOR0_NAME[0], m1.VECTOR1_NAME[0], m1.VECTOR2_NAME[0]);
	const VectorType rv1(m1.VECTOR0_NAME[1], m1.VECTOR1_NAME[1], m1.VECTOR2_NAME[1]);
	const VectorType rv2(m1.VECTOR0_NAME[2], m1.VECTOR1_NAME[2], m1.VECTOR2_NAME[2]);
	return TEMPLATE_NAME<ValueType>(
		VectorType(dot(rv0, m2.VECTOR0_NAME), dot(rv1, m2.VECTOR0_NAME), dot(rv2, m2.VECTOR0_NAME)),
		VectorType(dot(rv0, m2.VECTOR1_NAME), dot(rv1, m2.VECTOR1_NAME), dot(rv2, m2.VECTOR1_NAME)),
		VectorType(dot(rv0, m2.VECTOR2_NAME), dot(rv1, m2.VECTOR2_NAME), dot(rv2, m2.VECTOR2_NAME))
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
		m.VECTOR0_NAME[0] * v[0] + m.VECTOR1_NAME[0] * v[1] + m.VECTOR2_NAME[0] * v[2],
		m.VECTOR0_NAME[1] * v[0] + m.VECTOR1_NAME[1] * v[1] + m.VECTOR2_NAME[1] * v[2],
		m.VECTOR0_NAME[2] * v[0] + m.VECTOR1_NAME[2] * v[1] + m.VECTOR2_NAME[2] * v[2]
	);
}

// 転置行列
template <typename ValueType>
TEMPLATE_NAME<ValueType> transpose(const TEMPLATE_NAME<ValueType> &m) {
	typedef typename TEMPLATE_NAME<ValueType>::VectorType VectorType;
	return TEMPLATE_NAME<ValueType>(
		VectorType(m.VECTOR0_NAME[0], m.VECTOR1_NAME[0], m.VECTOR2_NAME[0]),
		VectorType(m.VECTOR0_NAME[1], m.VECTOR1_NAME[1], m.VECTOR2_NAME[1]),
		VectorType(m.VECTOR0_NAME[2], m.VECTOR1_NAME[2], m.VECTOR2_NAME[2])
	);
}

// 逆行列
template <typename ValueType>
TEMPLATE_NAME<ValueType> inverse(const TEMPLATE_NAME<ValueType> &m) {
	const ValueType det = m.determinant();
	if (det == 0) return TEMPLATE_NAME<ValueType>(0,0,0,0,0,0,0,0,0);
	const ValueType det_r = ValueType(1) / det;
	return
		det_r *
		TEMPLATE_NAME<ValueType>(
			m.VECTOR1_NAME[1] * m.VECTOR2_NAME[2] - m.VECTOR2_NAME[1] * m.VECTOR1_NAME[2],
			m.VECTOR2_NAME[1] * m.VECTOR0_NAME[2] - m.VECTOR0_NAME[1] * m.VECTOR2_NAME[2],
			m.VECTOR0_NAME[1] * m.VECTOR1_NAME[2] - m.VECTOR1_NAME[1] * m.VECTOR0_NAME[2],
			m.VECTOR2_NAME[0] * m.VECTOR1_NAME[2] - m.VECTOR1_NAME[0] * m.VECTOR2_NAME[2],
			m.VECTOR0_NAME[0] * m.VECTOR2_NAME[2] - m.VECTOR2_NAME[0] * m.VECTOR0_NAME[2],
			m.VECTOR1_NAME[0] * m.VECTOR0_NAME[2] - m.VECTOR0_NAME[0] * m.VECTOR1_NAME[2],
			m.VECTOR1_NAME[0] * m.VECTOR2_NAME[1] - m.VECTOR2_NAME[0] * m.VECTOR1_NAME[1],
			m.VECTOR2_NAME[0] * m.VECTOR0_NAME[1] - m.VECTOR0_NAME[0] * m.VECTOR2_NAME[1],
			m.VECTOR0_NAME[0] * m.VECTOR1_NAME[1] - m.VECTOR1_NAME[0] * m.VECTOR0_NAME[1]
		);
}


#if WIN32
#pragma warning(pop)
#endif
