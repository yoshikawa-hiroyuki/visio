#ifndef GEARS_ARRAY_H
#define GEARS_ARRAY_H

#include <gears/gears.h>
#include <gears/array_access.h>
#include <algorithm>

#pragma warning(disable: 4996)

namespace gears {

template <typename DstType, typename SrcType>
void static_cast_copy(DstType *dst, const SrcType *src, const int n) {
	for (int i = 0; i < n; ++i) dst[i] = static_cast<DstType>(src[i]);
}


template <typename value_type>
class array_1d : public array_access_1d<value_type> {
public:
	typedef value_type ValueType;

	array_1d() : array_access_1d<ValueType>(0, 0) {}

	array_1d(int size_x, const ValueType *p = 0)
		: array_access_1d<ValueType>(size_x, size_x > 0 ? new ValueType[size_x] : 0)
	{
		if (p) std::copy(p, p + size_x, this->data());
	}

	// コピーコンストラクタ
	array_1d(const array_1d &a) : array_access_1d<ValueType>(a.size_x(), a.size() > 0 ? new ValueType[a.size()] : 0) {
		std::copy(a.data(), a.data() + a.size(), this->data());
	}

	// 代入演算子
	array_1d &operator=(const array_1d &a) {
		if (&a == this) return *this;
		if (a.size() == this->size()) {
			*reinterpret_cast<array_access_1d<ValueType> *>(this) = array_access_1d<ValueType>(a.size_x(), this->data());
		} else {
			delete[] this->data();
			*reinterpret_cast<array_access_1d<ValueType> *>(this) = array_access_1d<ValueType>(a.size_x(), a.size() > 0 ? new ValueType[a.size()] : 0);
		}
		std::copy(a.data(), a.data() + a.size(), this->data());
		return *this;
	}

	virtual ~array_1d() {delete[] this->data();}

	// 別のarray_1d<>からのコピーコンストラクタ
	template <typename ValueType2>
	array_1d(const array_access_1d<ValueType2> &a)
		: array_access_1d<ValueType>(a.size_x(), a.size() > 0 ? new ValueType[a.size()] : 0)
	{
		static_cast_copy(this->data(), a.data(), a.size());
	}

	// 別のarray_1d<>からの代入演算子
	template <typename ValueType2>
	array_1d &operator=(const array_access_1d<ValueType2> &a) {
		if (&a == reinterpret_cast<array_access_1d<ValueType2> *>(this)) return *this;
		if (a.size() == this->size()) {
			*reinterpret_cast<array_access_1d<ValueType> *>(this) = array_access_1d<ValueType>(a.size_x(), this->data());
		} else {
			delete[] this->data();
			*reinterpret_cast<array_access_1d<ValueType> *>(this) = array_access_1d<ValueType>(a.size_x(), a.size() > 0 ? new ValueType[a.size()] : 0);
		}
		static_cast_copy(this->data(), a.data(), this->size());
		return *this;
	}
};

template <typename value_type>
class array_2d : public array_access_2d<value_type> {
public:
	typedef value_type ValueType;

	array_2d() : array_access_2d<ValueType>(0, 0, 0) {}

	array_2d(int size_x, int size_y, const ValueType *p = 0)
		: array_access_2d<ValueType>(size_x, size_y, size_x * size_y > 0 ? new ValueType[size_x * size_y] : 0)
	{
		if (p) std::copy(p, p + size_x * size_y, this->data());
	}

	// コピーコンストラクタ
	array_2d(const array_2d &a) : array_access_2d<ValueType>(a.size_x(), a.size_y(), a.size() > 0 ? new ValueType[a.size()] : 0) {
		std::copy(a.data(), a.data() + a.size(), this->data());
	}

	// 代入演算子
	array_2d &operator=(const array_2d &a) {
		if (&a == this) return *this;
		if (a.size() == this->size()) {
			*reinterpret_cast<array_access_2d<ValueType> *>(this) = array_access_2d<ValueType>(a.size_x(), a.size_y(), this->data());
		} else {
			delete[] this->data();
			*reinterpret_cast<array_access_2d<ValueType> *>(this) = array_access_2d<ValueType>(a.size_x(), a.size_y(), a.size() > 0 ? new ValueType[a.size()] : 0);
		}
		std::copy(a.data(), a.data() + a.size(), this->data());
		return *this;
	}

	virtual ~array_2d() {delete[] this->data();}

	// 別のarray_2d<>からのコピーコンストラクタ
	template <typename ValueType2> 
	array_2d(const array_access_2d<ValueType2> &a) 
		: array_access_2d<ValueType>(a.size_x(), a.size_y(), a.size() > 0 ? new ValueType[a.size()] : 0)
	{
		static_cast_copy(this->data(), a.data(), a.size());
	}

	// 別のarray_2d<>からの代入演算子
	template <typename ValueType2> 
	array_2d &operator=(const array_access_2d<ValueType2> &a) {
		if (&a == reinterpret_cast<array_access_2d<ValueType2> *>(this)) return *this;
		if (a.size() == this->size()) {
			*reinterpret_cast<array_access_2d<ValueType> *>(this) = array_access_2d<ValueType>(a.size_x(), a.size_y(), this->data());
		} else {
			delete[] this->data();
			*reinterpret_cast<array_access_2d<ValueType> *>(this) = array_access_2d<ValueType>(a.size_x(), a.size_y(), a.size() > 0 ? new ValueType[a.size()] : 0);
		}
		static_cast_copy(this->data(), a.data(), this->size());
		return *this;
	}
};

template <typename value_type>
class array_3d : public array_access_3d<value_type> {
public:
	typedef value_type ValueType;

	array_3d() : array_access_3d<ValueType>(0, 0, 0, 0) {}

	array_3d(int size_x, int size_y, int size_z, const ValueType *p = 0)
		: array_access_3d<ValueType>(size_x, size_y, size_z, size_x * size_y * size_z > 0 ? new ValueType[size_x * size_y * size_z] : 0)
	{
		if (p) std::copy(p, p + size_x * size_y * size_z, this->data());
	}

	// コピーコンストラクタ
	array_3d(const array_3d &a) : array_access_3d<ValueType>(a.size_x(), a.size_y(), a.size_z(), a.size() > 0 ? new ValueType[a.size()] : 0) {
		std::copy(a.data(), a.data() + a.size(), this->data());
	}

	// 代入演算子
	array_3d &operator=(const array_3d &a) {
		if (&a == this) return *this;
		if (a.size() == this->size()) {
			*reinterpret_cast<array_access_3d<ValueType> *>(this) = array_access_3d<ValueType>(a.size_x(), a.size_y(), a.size_z(), this->data());
		} else {
			delete[] this->data();
			*reinterpret_cast<array_access_3d<ValueType> *>(this) = array_access_3d<ValueType>(a.size_x(), a.size_y(), a.size_z(), a.size() > 0 ? new ValueType[a.size()] : 0);
		}
		std::copy(a.data(), a.data() + a.size(), this->data());
		return *this;
	}

	virtual ~array_3d() {delete[] this->data();}

	// 別のarray_3d<>からのコピーコンストラクタ
	template <typename ValueType2>
	array_3d(const array_access_3d<ValueType2> &a)
		: array_access_3d<ValueType>(a.size_x(), a.size_y(), a.size_z(), a.size() > 0 ? new ValueType[a.size()] : 0)
	{
		static_cast_copy(this->data(), a.data(), a.size());
	}

	// 別のarray_3d<>からの代入演算子
	template <typename ValueType2>
	array_3d &operator=(const array_access_3d<ValueType2> &a) {
		if (&a == reinterpret_cast<array_access_3d<ValueType2> *>(this)) return *this;
		if (a.size() == this->size()) {
			*reinterpret_cast<array_access_3d<ValueType> *>(this) = array_access_3d<ValueType>(a.size_x(), a.size_y(), a.size_z(), this->data());
		} else {
			delete[] this->data();
			*reinterpret_cast<array_access_3d<ValueType> *>(this) = array_access_3d<ValueType>(a.size_x(), a.size_y(), a.size_z(), a.size() > 0 ? new ValueType[a.size()] : 0);
		}
		static_cast_copy(this->data(), a.data(), this->size());
		return *this;
	}
};

} // end of namespace gears

#endif // GEARS_GL_ARRAY_H
