#pragma once

#include <initializer_list>
#include <type_traits>

template<
	typename BaseType,
	int t_w,
	int t_h,
	typename DowncastType = BaseType
> class Matrix {
public:
	constexpr Matrix() = default;
	constexpr Matrix(std::initializer_list<BaseType> list) {
		auto elem = list.begin();
		for(auto y = 0u; y < t_h; ++y)
			for(auto x = 0u; x < t_w && elem != list.end(); ++x, ++elem)
				data[x][y] = *elem;
	}

	constexpr BaseType const& operator()(unsigned x, unsigned y) const {
		return data[x][y];
	}

	constexpr BaseType& operator()(unsigned x, unsigned y) {
		return data[x][y];
	}

	// Static Constants
	static constexpr Matrix Zero() {
		return Matrix{};
	}

	template<
		typename DepType = BaseType,
		typename = std::enable_if_t<t_w == t_h, DepType>
	>
	static constexpr Matrix Identity() {
		auto ret = Matrix{};
		for(auto i = 0u; i < t_w; ++i)
			ret(i, i) = BaseType{1};
		return ret;
	}

private:
	BaseType data[t_w][t_h]{};
};

// unary operators
template<
	typename BaseType,
	int t_w,
	int t_h,
	typename DowncastType = BaseType
>
constexpr Matrix<BaseType, t_w, t_h, DowncastType> operator- (
	Matrix<BaseType, t_w, t_h, DowncastType> const& op
) {
	return Matrix<BaseType, t_w, t_h, DowncastType>::Zero() - op;
}

// binary operators
template<
	typename BaseType,
	int t_w,
	int t_h,
	typename DowncastType = BaseType
>
constexpr Matrix<BaseType, t_w, t_h, DowncastType> operator+ (
	Matrix<BaseType, t_w, t_h, DowncastType> const& left,
	Matrix<BaseType, t_w, t_h, DowncastType> const& right
) {
	auto ret = Matrix<BaseType, t_w, t_h, DowncastType>{};
	for(auto x = 0u; x < t_w; ++x)
		for(auto y = 0u; y < t_h; ++y)
			ret(x, y) = left(x, y) + right(x, y);
	return ret;
}

template<
	typename BaseType,
	int t_w,
	int t_h,
	typename DowncastType = BaseType
>
constexpr Matrix<BaseType, t_w, t_h, DowncastType> operator- (
	Matrix<BaseType, t_w, t_h, DowncastType> const& left,
	Matrix<BaseType, t_w, t_h, DowncastType> const& right
) {
	auto ret = Matrix<BaseType, t_w, t_h, DowncastType>{};
	for(auto x = 0u; x < t_w; ++x)
		for(auto y = 0u; y < t_h; ++y)
			ret(x, y) = left(x, y) - right(x, y);
	return ret;
}

//Scalars
template<
	typename T,
	typename BaseType,
	int t_w,
	int t_h,
	typename DowncastType = BaseType
>
constexpr auto operator* (
	T const& left,
	Matrix<BaseType, t_w, t_h, DowncastType> const& right
)
{
	auto ret = Matrix<
		std::remove_const_t<decltype(left * right(0,0))>,
		t_w,
		t_h,
		DowncastType
	>{};
	for(auto x = 0u; x < t_w; ++x)
		for(auto y = 0u; y < t_h; ++y)
			ret(x, y) = left * right(x, y);
	return ret;
}

template<
	typename T,
	typename BaseType,
	int t_w,
	int t_h,
	typename DowncastType = BaseType
>
constexpr auto operator* (
	Matrix<BaseType, t_w, t_h, DowncastType> const& left,
	T const& right
)
{
	auto ret = Matrix<
		std::remove_const_t<decltype(left(0,0) * right)>,
		t_w,
		t_h,
		DowncastType
	>{};
	for(auto x = 0u; x < t_w; ++x)
		for(auto y = 0u; y < t_h; ++y)
			ret(x, y) = left(x, y) * right;
	return ret;
}

//Comparison operators

template<
	typename BaseType,
	int t_w,
	int t_h,
	typename DowncastType = BaseType
>
constexpr bool operator== (
	Matrix<BaseType, t_w, t_h, DowncastType> const& left,
	Matrix<BaseType, t_w, t_h, DowncastType> const& right
) {
	for(auto x = 0u; x < t_w; ++x)
		for(auto y = 0u; y < t_h; ++y)
			if(left(x, y) != right(x, y))
				return false;
	return true;
}

template<
	typename BaseType,
	int t_w,
	int t_h,
	typename DowncastType
>
constexpr bool operator!= (
	Matrix<BaseType, t_w, t_h, DowncastType> const& left,
	Matrix<BaseType, t_w, t_h, DowncastType> const& right
) {
	for(auto x = 0u; x < t_w; ++x)
		for(auto y = 0u; y < t_h; ++y)
			if(left(x, y) == right(x, y))
				return false;
	return true;
}

