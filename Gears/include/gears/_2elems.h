#ifndef TEMPLATE_NAME
# error please define "TEMPLATE_NAME"
#endif
#ifndef ELEM0_NAME
# error please define "ELEM0_NAME"
#endif
#ifndef ELEM1_NAME
# error please define "ELEM1_NAME"
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

	ValueType ELEM0_NAME;
	ValueType ELEM1_NAME;

	// デフォルトコンストラクタ(初期化しない)
	TEMPLATE_NAME() {}
    // 値を二つ指定して初期化
	TEMPLATE_NAME(ValueType x0, ValueType y0) : ELEM0_NAME(x0), ELEM1_NAME(y0) {}
	// 配列を指定して初期化
	template <typename ValueType2> TEMPLATE_NAME(ValueType2 *v) : ELEM0_NAME(v[0]), ELEM1_NAME(v[1]) {}
	template <typename ValueType2> TEMPLATE_NAME(const ValueType2 *v) : ELEM0_NAME(v[0]), ELEM1_NAME(v[1]) {}

	// 他のTEMPLATE_NAME<>型からコピーコンストラクト
	template <typename ValueType2> TEMPLATE_NAME(const TEMPLATE_NAME<ValueType2> &v) : ELEM0_NAME(ValueType(v.ELEM0_NAME)), ELEM1_NAME(ValueType(v.ELEM1_NAME)) {}

	// ポインタへの変換
	operator ValueType *() {return &ELEM0_NAME;}
	operator const ValueType *() const {return &ELEM0_NAME;}

	// アクセサ
	ValueType &operator[](int i) {return (&ELEM0_NAME)[i];}
	const ValueType &operator[](int i) const {return (&ELEM0_NAME)[i];}

	// 配列から読み込む
	TEMPLATE_NAME &load(const ValueType v[2]) {
		ELEM0_NAME = v[0]; ELEM1_NAME = v[1];
		return *this;
	}
	// 配列に書き込む
	void store(ValueType v[2]) const {
		v[0] = ELEM0_NAME; v[1] = ELEM1_NAME;
	}

	// 等価演算子
	bool operator==(const TEMPLATE_NAME<ValueType> &v) const {
		return ELEM0_NAME == v.ELEM0_NAME && ELEM1_NAME == v.ELEM1_NAME;
	}
	// 非等価演算子
	bool operator!=(const TEMPLATE_NAME<ValueType> &v) const {
		return !operator==(v);
	}

	// 別のベクトルを加算
	TEMPLATE_NAME &operator+=(const TEMPLATE_NAME &v) {
		ELEM0_NAME += v.ELEM0_NAME; ELEM1_NAME += v.ELEM1_NAME;
		return *this;
	}
	// 別のベクトルを減算
	TEMPLATE_NAME &operator-=(const TEMPLATE_NAME &v) {
		ELEM0_NAME -= v.ELEM0_NAME; ELEM1_NAME -= v.ELEM1_NAME;
		return *this;
	}

	// スカラ型を乗算
	template <typename ValueType2>
	TEMPLATE_NAME &operator*=(ValueType2 _s) {
		ELEM0_NAME = (ValueType)(ELEM0_NAME * _s);
		ELEM1_NAME = (ValueType)(ELEM1_NAME * _s);
		return *this;
	}
	// スカラ型で除算
	template <typename ValueType2>
	TEMPLATE_NAME &operator/=(ValueType2 _s) {
		const ValueType _s2 = (ValueType)1 / (ValueType)_s;
		ELEM0_NAME *= _s2; ELEM1_NAME *= _s2;
		return *this;
	}

	// 単項+演算子
	const TEMPLATE_NAME operator+() const {
		return TEMPLATE_NAME(*this);
	}
	// 単項-演算子
	const TEMPLATE_NAME operator-() const {
		return TEMPLATE_NAME(-ELEM0_NAME, -ELEM1_NAME);
	}

	// 長さの2乗を求める
	ValueType length2() const {
		return ELEM0_NAME * ELEM0_NAME + ELEM1_NAME * ELEM1_NAME;
	}
	// 長さを求める
	ValueType length() const {
		return (ValueType)sqrt(double(length2()));
	}
	// 正規化する
	TEMPLATE_NAME &normalize() {
		const ValueType s = ValueType(1) / length();
		ELEM0_NAME *= s; ELEM1_NAME *= s;
		return *this;
	}
	TEMPLATE_NAME &normalize(ValueType l) {
		const ValueType s = l / length();
		ELEM0_NAME *= s; ELEM1_NAME *= s;
		return *this;
	}
	// 正規化されたベクトルを得る
	const TEMPLATE_NAME unit() const {
		return TEMPLATE_NAME(*this).normalize();
	}
	const TEMPLATE_NAME unit(ValueType l) const {
		return TEMPLATE_NAME(*this).normalize(l);
	}
};

// 整数系用スカラ除算
template<> template <typename ValueType2> TEMPLATE_NAME<char> &TEMPLATE_NAME<char>::operator/=(ValueType2 s) {ELEM0_NAME /= s; ELEM1_NAME /= s; return *this;}
template<> template <typename ValueType2> TEMPLATE_NAME<unsigned char> &TEMPLATE_NAME<unsigned char>::operator/=(ValueType2 s) {ELEM0_NAME /= s; ELEM1_NAME /= s; return *this;}
template<> template <typename ValueType2> TEMPLATE_NAME<short> &TEMPLATE_NAME<short>::operator/=(ValueType2 s) {ELEM0_NAME /= s; ELEM1_NAME /= s; return *this;}
template<> template <typename ValueType2> TEMPLATE_NAME<unsigned short> &TEMPLATE_NAME<unsigned short>::operator/=(ValueType2 s) {ELEM0_NAME /= s; ELEM1_NAME /= s; return *this;}
template<> template <typename ValueType2> TEMPLATE_NAME<int> &TEMPLATE_NAME<int>::operator/=(ValueType2 s) {ELEM0_NAME /= s; ELEM1_NAME /= s; return *this;}
template<> template <typename ValueType2> TEMPLATE_NAME<unsigned int> &TEMPLATE_NAME<unsigned int>::operator/=(ValueType2 s) {ELEM0_NAME /= s; ELEM1_NAME /= s; return *this;}
#if WIN32
template<> template <typename ValueType2> TEMPLATE_NAME<__int64> &TEMPLATE_NAME<__int64>::operator/=(ValueType2 s) {ELEM0_NAME /= s; ELEM1_NAME /= s; return *this;}
template<> template <typename ValueType2> TEMPLATE_NAME<unsigned __int64> &TEMPLATE_NAME<unsigned __int64>::operator/=(ValueType2 s) {ELEM0_NAME /= s; ELEM1_NAME /= s; return *this;}
#else
template<> template <typename ValueType2> TEMPLATE_NAME<long long> &TEMPLATE_NAME<long long>::operator/=(ValueType2 s) {ELEM0_NAME /= s; ELEM1_NAME /= s; return *this;}
template<> template <typename ValueType2> TEMPLATE_NAME<unsigned long long> &TEMPLATE_NAME<unsigned long long>::operator/=(ValueType2 s) {ELEM0_NAME /= s; ELEM1_NAME /= s; return *this;}
#endif

// ベクトル加算
template <typename ValueType>
inline TEMPLATE_NAME<ValueType> operator+(TEMPLATE_NAME<ValueType> v0, TEMPLATE_NAME<ValueType> v1) {
	return v0 += v1;
}

// ベクトル減算
template <typename ValueType>
inline TEMPLATE_NAME<ValueType> operator-(TEMPLATE_NAME<ValueType> v0, TEMPLATE_NAME<ValueType> v1) {
	return v0 -= v1;
}

// スカラ乗算
template <typename ValueType, typename ValueType2>
inline TEMPLATE_NAME<ValueType> operator*(ValueType2 s, TEMPLATE_NAME<ValueType> v) {
	return v *= s;
}
template <typename ValueType, typename ValueType2>
inline TEMPLATE_NAME<ValueType> operator*(TEMPLATE_NAME<ValueType> v, ValueType2 s) {
	return v *= s;
}

// スカラ除算
template <typename ValueType, typename ValueType2>
inline const TEMPLATE_NAME<ValueType> operator/(TEMPLATE_NAME<ValueType> v, ValueType2 s) {
	return v /= (ValueType)s;
}

// 内積
template <typename ValueType>
inline ValueType dot(const TEMPLATE_NAME<ValueType> &v0, const TEMPLATE_NAME<ValueType> &v1) {
	return v0.ELEM0_NAME*v1.ELEM0_NAME + v0.ELEM1_NAME*v1.ELEM1_NAME;
}

// 外積
template <typename ValueType>
inline ValueType cross(const TEMPLATE_NAME<ValueType> &v0, const TEMPLATE_NAME<ValueType> &v1) {
	return v0.ELEM0_NAME * v1.ELEM1_NAME - v0.ELEM1_NAME * v1.ELEM0_NAME;
}

// 要素ごと積算
template <typename ValueType>
inline TEMPLATE_NAME<ValueType> mul(const TEMPLATE_NAME<ValueType> &v0, const TEMPLATE_NAME<ValueType> &v1) {
	return TEMPLATE_NAME<ValueType>(
		v0.ELEM0_NAME * v1.ELEM0_NAME,
		v0.ELEM1_NAME * v1.ELEM1_NAME
	);
}

// 要素ごと除算
template <typename ValueType>
inline TEMPLATE_NAME<ValueType> div(const TEMPLATE_NAME<ValueType> &v0, const TEMPLATE_NAME<ValueType> &v1) {
	return TEMPLATE_NAME<ValueType>(
		v0.ELEM0_NAME / v1.ELEM0_NAME,
		v0.ELEM1_NAME / v1.ELEM1_NAME
	);
}


#if WIN32
#pragma warning(pop)
#endif
