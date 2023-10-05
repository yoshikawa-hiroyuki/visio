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
#ifndef VECTOR3_NAME
# error please define "VECTOR3_NAME"
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
	VectorType VECTOR3_NAME;

	// 定数
	static const TEMPLATE_NAME O; // 零行列
	static const TEMPLATE_NAME I; // 単位行列

	// デフォルトコンストラクタ(初期化しない)
	TEMPLATE_NAME() {}

	// 値を16個指定して初期化
	TEMPLATE_NAME(
		ValueType m00, ValueType m10, ValueType m20, ValueType m30,
		ValueType m01, ValueType m11, ValueType m21, ValueType m31,
		ValueType m02, ValueType m12, ValueType m22, ValueType m32,
		ValueType m03, ValueType m13, ValueType m23, ValueType m33
	) : VECTOR0_NAME(m00, m10, m20, m30), VECTOR1_NAME(m01, m11, m21, m31), VECTOR2_NAME(m02, m12, m22, m32), VECTOR3_NAME(m03, m13, m23, m33) {}
    // ベクトルを四つ指定して初期化
	TEMPLATE_NAME(const VectorType &vx0, const VectorType &vy0, const VectorType &vz0, const VectorType &vw0)
		: VECTOR0_NAME(vx0), VECTOR1_NAME(vy0), VECTOR2_NAME(vz0), VECTOR3_NAME(vw0) {}
	// 配列を指定して初期化
	TEMPLATE_NAME(const ValueType m[16])
		: VECTOR0_NAME(m[0],m[1],m[2], m[3]), VECTOR1_NAME(m[4],m[5],m[6], m[7]), VECTOR2_NAME(m[8],m[9],m[10],m[11]), VECTOR3_NAME(m[12],m[13],m[14],m[15]) {}

	// 他のTEMPLATE_NAME<>型からコピーコンストラクト
	template <typename ValType2> TEMPLATE_NAME(const TEMPLATE_NAME<ValType2> &m) {VECTOR0_NAME = VectorType(m.VECTOR0_NAME); VECTOR1_NAME = VectorType(m.VECTOR1_NAME); VECTOR2_NAME = VectorType(m.VECTOR2_NAME); VECTOR3_NAME = VectorType(m.VECTOR3_NAME);}

	// アクセサ
	VectorType &operator[](int i) {return (&VECTOR0_NAME)[i];}
	const VectorType &operator[](int i) const {return (&VECTOR0_NAME)[i];}
	VectorType raw(int i) const {return VectorType(VECTOR0_NAME[i], VECTOR1_NAME[i], VECTOR2_NAME[i], VECTOR3_NAME[i]);}
	VectorType column(int i) const {return (&VECTOR0_NAME)[i];}
	ValueType &operator()(int raw, int column) {return (*this)[column][raw];}
	const ValueType &operator()(int raw, int column) const {return (*this)[column][raw];}

	// 配列から読み込む
	TEMPLATE_NAME &load(const ValueType v[16]) {
		VECTOR0_NAME.load(v);
		VECTOR1_NAME.load(v+4);
		VECTOR2_NAME.load(v+8);
		VECTOR3_NAME.load(v+12);
		return *this;
	}
	// 配列に書き込む
	void store(ValueType v[16]) const {
		VECTOR0_NAME.store(v);
		VECTOR1_NAME.store(v+4);
		VECTOR2_NAME.store(v+8);
		VECTOR3_NAME.store(v+12);
	}

	// 等価演算子
	bool operator==(const TEMPLATE_NAME<ValueType> &m) const {
		return VECTOR0_NAME == m.VECTOR0_NAME && VECTOR1_NAME == m.VECTOR1_NAME && VECTOR2_NAME == m.VECTOR2_NAME && VECTOR3_NAME == m.VECTOR3_NAME;
	}
	// 非等価演算子
	bool operator!=(const TEMPLATE_NAME<ValueType> &m) const {
		return !operator==(m);
	}

	// 別の行列を加算
	TEMPLATE_NAME &operator+=(const TEMPLATE_NAME &m) {
		VECTOR0_NAME += m.VECTOR0_NAME; VECTOR1_NAME += m.VECTOR1_NAME; VECTOR2_NAME += m.VECTOR2_NAME; VECTOR3_NAME += m.VECTOR3_NAME;
		return *this;
	}
	// 別の行列を減算
	TEMPLATE_NAME &operator-=(const TEMPLATE_NAME &m) {
		VECTOR0_NAME -= m.VECTOR0_NAME; VECTOR1_NAME -= m.VECTOR1_NAME; VECTOR2_NAME -= m.VECTOR2_NAME; VECTOR3_NAME -= m.VECTOR3_NAME;
		return *this;
	}

	// スカラ型を乗算
	template <typename ValueType2>
	TEMPLATE_NAME &operator*=(ValueType2 s) {
		VECTOR0_NAME *= s; VECTOR1_NAME *= s; VECTOR2_NAME *= s; VECTOR3_NAME *= s;
		return *this;
	}
	// スカラ型で除算
	template <typename ValueType2>
	TEMPLATE_NAME &operator/=(ValueType2 s) {
		const ValueType s2 = (ValueType)1 / s;
		VECTOR0_NAME *= s2; VECTOR1_NAME *= s2; VECTOR2_NAME *= s2; VECTOR3_NAME *= s2;
		return *this;
	}

	// 単項+演算子
	const TEMPLATE_NAME operator+() const {
		return TEMPLATE_NAME(*this);
	}
	// 単項-演算子
	const TEMPLATE_NAME operator-() const {
		return TEMPLATE_NAME(-VECTOR0_NAME, -VECTOR1_NAME, -VECTOR2_NAME, -VECTOR3_NAME);
	}

	// 行列式を求める
	ValueType determinant() const {
		const ValueType (*a)[4] = (ValueType(*)[4])&VECTOR0_NAME;
		const ValueType d01 = a[0][2]*a[1][3] - a[1][2]*a[0][3];
		const ValueType d02 = a[0][2]*a[2][3] - a[2][2]*a[0][3];
		const ValueType d03 = a[0][2]*a[3][3] - a[3][2]*a[0][3];
		const ValueType d12 = a[1][2]*a[2][3] - a[2][2]*a[1][3];
		const ValueType d13 = a[1][2]*a[3][3] - a[3][2]*a[1][3];
		const ValueType d23 = a[2][2]*a[3][3] - a[3][2]*a[2][3];
		const ValueType d =
			a[0][0]*(
				a[1][1] * d23
				-a[2][1] * d13
				+a[3][1] * d12
			)
			-a[1][0]*(
				a[0][1] * d23
				-a[2][1] * d03
				+a[3][1] * d02
			) 
			+a[2][0]*(
				a[0][1] * d13
				-a[1][1] * d03
				+a[3][1] * d01
			)
			- a[3][0]*(
				a[0][1] * d12
				-a[1][1] * d02
				+a[2][1] * d01
			);

		return d;
	}
	ValueType det() const {return determinant();}

	// 拡大縮小行列を作る
	static TEMPLATE_NAME scale(ValueType x, ValueType y, ValueType z, ValueType w) {
		return TEMPLATE_NAME(x,0,0,0, 0,y,0,0, 0,0,z,0, 0,0,0,w);
	}
	static TEMPLATE_NAME scale(const VECTOR_TEMPLATE_NAME<ValueType> &v) {
		return scale(v[0], v[1], v[2], v[3]);
	}
	static TEMPLATE_NAME scale(ValueType s) {
		return scale(s, s, s, s);
	}

#ifdef MATRIX33_TEMPLATE_NAME
	// 指定した要素を軸とした行と列を取り除いた3x3行列を返す
	MATRIX33_TEMPLATE_NAME<ValueType> reduction(int column, int raw) const {
		if (column == 0) return MATRIX33_TEMPLATE_NAME<ValueType>(VECTOR1_NAME.reduction(raw), VECTOR2_NAME.reduction(raw), VECTOR3_NAME.reduction(raw));
		if (column == 1) return MATRIX33_TEMPLATE_NAME<ValueType>(VECTOR0_NAME.reduction(raw), VECTOR2_NAME.reduction(raw), VECTOR3_NAME.reduction(raw));
		if (column == 2) return MATRIX33_TEMPLATE_NAME<ValueType>(VECTOR0_NAME.reduction(raw), VECTOR1_NAME.reduction(raw), VECTOR3_NAME.reduction(raw));
		if (column == 3) return MATRIX33_TEMPLATE_NAME<ValueType>(VECTOR0_NAME.reduction(raw), VECTOR1_NAME.reduction(raw), VECTOR2_NAME.reduction(raw));
		return MATRIX33_TEMPLATE_NAME<ValueType>(0,0,0,0,0,0,0,0,0);
	}
#endif

};

// 定数
template <typename ValueType> const TEMPLATE_NAME<ValueType> TEMPLATE_NAME<ValueType>::O(VectorType(0,0,0,0), VectorType(0,0,0,0), VectorType(0,0,0,0), VectorType(0,0,0,0));
template <typename ValueType> const TEMPLATE_NAME<ValueType> TEMPLATE_NAME<ValueType>::I(VectorType(1,0,0,0), VectorType(0,1,0,0), VectorType(0,0,1,0), VectorType(0,0,0,1));


// 整数系用スカラ除算
template<> template <typename ValueType2> TEMPLATE_NAME<char> &TEMPLATE_NAME<char>::operator/=(ValueType2 s) {VECTOR0_NAME /= s; VECTOR1_NAME /= s; VECTOR2_NAME /= s; VECTOR3_NAME /= s; return *this;}
template<> template <typename ValueType2> TEMPLATE_NAME<unsigned char> &TEMPLATE_NAME<unsigned char>::operator/=(ValueType2 s) {VECTOR0_NAME /= s; VECTOR1_NAME /= s; VECTOR2_NAME /= s; VECTOR3_NAME /= s; return *this;}
template<> template <typename ValueType2> TEMPLATE_NAME<short> &TEMPLATE_NAME<short>::operator/=(ValueType2 s) {VECTOR0_NAME /= s; VECTOR1_NAME /= s; VECTOR2_NAME /= s; VECTOR3_NAME /= s; return *this;}
template<> template <typename ValueType2> TEMPLATE_NAME<unsigned short> &TEMPLATE_NAME<unsigned short>::operator/=(ValueType2 s) {VECTOR0_NAME /= s; VECTOR1_NAME /= s; VECTOR2_NAME /= s; VECTOR3_NAME /= s; return *this;}
template<> template <typename ValueType2> TEMPLATE_NAME<int> &TEMPLATE_NAME<int>::operator/=(ValueType2 s) {VECTOR0_NAME /= s; VECTOR1_NAME /= s; VECTOR2_NAME /= s; VECTOR3_NAME /= s; return *this;}
template<> template <typename ValueType2> TEMPLATE_NAME<unsigned int> &TEMPLATE_NAME<unsigned int>::operator/=(ValueType2 s) {VECTOR0_NAME /= s; VECTOR1_NAME /= s; VECTOR2_NAME /= s; VECTOR3_NAME /= s; return *this;}
#if WIN32
template<> template <typename ValueType2> TEMPLATE_NAME<__int64> &TEMPLATE_NAME<__int64>::operator/=(ValueType2 s) {VECTOR0_NAME /= s; VECTOR1_NAME /= s; VECTOR2_NAME /= s; VECTOR3_NAME /= s; return *this;}
template<> template <typename ValueType2> TEMPLATE_NAME<unsigned __int64> &TEMPLATE_NAME<unsigned __int64>::operator/=(ValueType2 s) {VECTOR0_NAME /= s; VECTOR1_NAME /= s; VECTOR2_NAME /= s; VECTOR3_NAME /= s; return *this;}
#else
template<> template <typename ValueType2> TEMPLATE_NAME<long long> &TEMPLATE_NAME<long long>::operator/=(ValueType2 s) {VECTOR0_NAME /= s; VECTOR1_NAME /= s; VECTOR2_NAME /= s; VECTOR3_NAME /= s; return *this;}
template<> template <typename ValueType2> TEMPLATE_NAME<unsigned long long> &TEMPLATE_NAME<unsigned long long>::operator/=(ValueType2 s) {VECTOR0_NAME /= s; VECTOR1_NAME /= s; VECTOR2_NAME /= s; VECTOR3_NAME /= s; return *this;}
#endif

// 行列の和
template <typename ValueType>
inline TEMPLATE_NAME<ValueType> operator+(const TEMPLATE_NAME<ValueType> &m0, const TEMPLATE_NAME<ValueType> &m1) {
	return TEMPLATE_NAME<ValueType>(m0.VECTOR0_NAME + m1.VECTOR0_NAME, m0.VECTOR1_NAME + m1.VECTOR1_NAME, m0.VECTOR2_NAME + m1.VECTOR2_NAME, m0.VECTOR3_NAME + m1.VECTOR3_NAME);
}

// 行列の差
template <typename ValueType>
inline TEMPLATE_NAME<ValueType> operator-(const TEMPLATE_NAME<ValueType> &m0, const TEMPLATE_NAME<ValueType> &m1) {
	return TEMPLATE_NAME<ValueType>(m0.VECTOR0_NAME - m1.VECTOR0_NAME, m0.VECTOR1_NAME - m1.VECTOR1_NAME, m0.VECTOR2_NAME - m1.VECTOR2_NAME, m0.VECTOR3_NAME - m1.VECTOR3_NAME);
}

// 行列の積
template <typename ValueType>
inline TEMPLATE_NAME<ValueType> operator*(const TEMPLATE_NAME<ValueType> &m1, const TEMPLATE_NAME<ValueType> &m2) {
	typedef typename TEMPLATE_NAME<ValueType>::VectorType VectorType;
	const VectorType rv0(m1.VECTOR0_NAME[0], m1.VECTOR1_NAME[0], m1.VECTOR2_NAME[0], m1.VECTOR3_NAME[0]);
	const VectorType rv1(m1.VECTOR0_NAME[1], m1.VECTOR1_NAME[1], m1.VECTOR2_NAME[1], m1.VECTOR3_NAME[1]);
	const VectorType rv2(m1.VECTOR0_NAME[2], m1.VECTOR1_NAME[2], m1.VECTOR2_NAME[2], m1.VECTOR3_NAME[2]);
	const VectorType rv3(m1.VECTOR0_NAME[3], m1.VECTOR1_NAME[3], m1.VECTOR2_NAME[3], m1.VECTOR3_NAME[3]);
	return TEMPLATE_NAME<ValueType>(
		VectorType(dot(rv0, m2.VECTOR0_NAME), dot(rv1, m2.VECTOR0_NAME), dot(rv2, m2.VECTOR0_NAME), dot(rv3, m2.VECTOR0_NAME)),
		VectorType(dot(rv0, m2.VECTOR1_NAME), dot(rv1, m2.VECTOR1_NAME), dot(rv2, m2.VECTOR1_NAME), dot(rv3, m2.VECTOR1_NAME)),
		VectorType(dot(rv0, m2.VECTOR2_NAME), dot(rv1, m2.VECTOR2_NAME), dot(rv2, m2.VECTOR2_NAME), dot(rv3, m2.VECTOR2_NAME)),
		VectorType(dot(rv0, m2.VECTOR3_NAME), dot(rv1, m2.VECTOR3_NAME), dot(rv2, m2.VECTOR3_NAME), dot(rv3, m2.VECTOR3_NAME))
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
		m.VECTOR0_NAME[0] * v[0] + m.VECTOR1_NAME[0] * v[1] + m.VECTOR2_NAME[0] * v[2] + m.VECTOR3_NAME[0] * v[3],
		m.VECTOR0_NAME[1] * v[0] + m.VECTOR1_NAME[1] * v[1] + m.VECTOR2_NAME[1] * v[2] + m.VECTOR3_NAME[1] * v[3],
		m.VECTOR0_NAME[2] * v[0] + m.VECTOR1_NAME[2] * v[1] + m.VECTOR2_NAME[2] * v[2] + m.VECTOR3_NAME[2] * v[3],
		m.VECTOR0_NAME[3] * v[0] + m.VECTOR1_NAME[3] * v[1] + m.VECTOR2_NAME[3] * v[2] + m.VECTOR3_NAME[3] * v[3]
	);
}

// 転置行列
template <typename ValueType>
TEMPLATE_NAME<ValueType> transpose(const TEMPLATE_NAME<ValueType> &m) {
	typedef typename TEMPLATE_NAME<ValueType>::VectorType VectorType;
	return TEMPLATE_NAME<ValueType>(
		VectorType(m.VECTOR0_NAME[0], m.VECTOR1_NAME[0], m.VECTOR2_NAME[0], m.VECTOR3_NAME[0]),
		VectorType(m.VECTOR0_NAME[1], m.VECTOR1_NAME[1], m.VECTOR2_NAME[1], m.VECTOR3_NAME[1]),
		VectorType(m.VECTOR0_NAME[2], m.VECTOR1_NAME[2], m.VECTOR2_NAME[2], m.VECTOR3_NAME[2]),
		VectorType(m.VECTOR0_NAME[3], m.VECTOR1_NAME[3], m.VECTOR2_NAME[3], m.VECTOR3_NAME[3])
	);
}

#ifdef MATRIX33_TEMPLATE_NAME
// 逆行列
template <typename ValueType>
TEMPLATE_NAME<ValueType> inverse(const TEMPLATE_NAME<ValueType> &m) {
	const ValueType det = m.determinant();
	if (det == 0) return TEMPLATE_NAME<ValueType>(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
	const ValueType det_r = ValueType(1) / det;
	const ValueType (*a)[4] = (ValueType(*)[4])&m.VECTOR0_NAME;
	return
		det_r *
		TEMPLATE_NAME<ValueType>(
			m.reduction(0,0).determinant(),
			-m.reduction(1,0).determinant(),
			m.reduction(2,0).determinant(),
			-m.reduction(3,0).determinant(),

			-m.reduction(0,1).determinant(),
			m.reduction(1,1).determinant(),
			-m.reduction(2,1).determinant(),
			m.reduction(3,1).determinant(),

			m.reduction(0,2).determinant(),
			-m.reduction(1,2).determinant(),
			m.reduction(2,2).determinant(),
			-m.reduction(3,2).determinant(),

			-m.reduction(0,3).determinant(),
			m.reduction(1,3).determinant(),
			-m.reduction(2,3).determinant(),
			m.reduction(3,3).determinant()
		);
}
#endif


#if WIN32
#pragma warning(pop)
#endif
