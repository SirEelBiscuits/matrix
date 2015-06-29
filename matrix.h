#pragma once

#include <initializer_list>

//Some helper code best moved to its own header.
namespace TypeMagic {

	template<unsigned power>
	class pow {
	public:
		template<typename T, typename impl = pow<power - 1>>
		static CONSTEXPR auto calculate(T const &in)
		-> decltype(in * impl::calculate(in)) {
			return in * impl::calculate(in);
		}
	};

	template<>
	class pow<1> {
	public:
		template<typename T>
		static CONSTEXPR T calculate(T const &in) {
			return in;
		}
	};

	template<typename T>
	static CONSTEXPR auto invert(T const &in)
	-> decltype(in/in/in) {
		return in/in/in;
	}
}

#define X_TO_POWER(x, power) (TypeMagic::pow<power>::calculate(x))
#define X_TO_POWER_TYPE(x, power) decltype(X_TO_POWER(x, power))
#define SAFISH_INVERT(in) (TypeMagic::invert(in))
#define INVERSE_TYPE(in) decltype(SAFISH_INVERT(in))

#define ENABLE_IF_SQUARE template<typename DepBaseType = BaseType, typename = typename std::enable_if<t_w == t_h, DepBaseType>::type>

/*
 * Generic matrix.
 *
 * Should work with any type specified for BaseType, so long as it has basic overloads.
 * Types which have 'interesting' mutations multiplication and dividsion _should_ work
 * I am not currently supporting mutation under addition because wtff.
 *
 * DownCastType is for intermediate operations where types might be accumulating type information.
 * This isn't permissable in C++ usually, but the matrix will maintain return type validity, and
 * just use this value internally. If it compiles with the default, the default is fine!
 *
 * example for when DownCastType is needed:
 *   BaseType = T<int m_pow> where m_pow is the exponent of the meters unit. Multiplying two together
 *   will result in summing their m_pow to maintain the unit validity. If these use float as a storage
 *   type, and can be cast to and from, then DownCastType should be float. If there is no such type,
 *   make one as a helper type to use here. The DownCastType must have its operators return the same
 *   type, always.
 */
template<typename BaseType, unsigned t_w, unsigned t_h, typename DownCastType = BaseType>
class Matrix {
public:

	Matrix()
		: data()
	{}

	Matrix(std::initializer_list<BaseType> l)
	{
		auto elem = l.begin();
		for(auto y = 0u; y < t_h; ++y)
			for(auto x = 0u; x < t_w && elem != l.end(); ++x, ++elem)
				data[x][y] = *elem;
	}

	Matrix(Matrix const& other) {
		for(auto y = 0u; y < t_h; ++y)
			for(auto x = 0u; x < t_w; ++x)
				data[x][y] = other.data[x][y];
	}

	Matrix& operator=(Matrix const &other) {
		for(auto y = 0u; y < t_h; ++y)
			for(auto x = 0u; x < t_w; ++x)
				data[x][y] = other.data[x][y];
		return *this;
	}

	//accessors

	BaseType & operator() (unsigned x, unsigned y) {
	 	return data[x][y];
	}

	BaseType const& operator() (unsigned x, unsigned y) const {
	 	return data[x][y];
	}

	//special for column vectors

	template<typename DepBaseType = BaseType>
	std::enable_if_t<t_w == 1u, DepBaseType> & operator[] (unsigned y) {
		return data[0u][y];
	}

	template<typename DepBaseType = BaseType>
	std::enable_if_t<t_w == 1u, DepBaseType> const& operator[] (unsigned y) const {
		return data[0u][y];
	}

	//special for row vectors

	//template<typename DepBaseType = BaseType>
	//std::enable_if_t<t_h == 1u && t_w != 1u, DepBaseType> & operator[] (unsigned x) {
	//	return data[x][0u];
	//}

	//template<typename DepBaseType = BaseType>
	//std::enable_if_t<t_h == 1u && t_w != 1u, DepBaseType> const& operator[] (unsigned x) const {
	//	return data[x][0u];
	//}

	// Assignment operators

	Matrix& operator+=(Matrix const& other) {
		for(auto y = 0u; y < t_h; ++y)
			for(auto x = 0u; x < t_w; ++x)
				data[x][y] += other.data[x][y];
		return *this;
	}

	Matrix& operator-=(Matrix const& other) {
		for(auto y = 0u; y < t_h; ++y)
			for(auto x = 0u; x < t_w; ++x)
				data[x][y] -= other.data[x][y];
		return *this;
	}

	template<typename ScalarType>
	Matrix& operator*=(ScalarType const &other) {
		for(auto y = 0u; y < t_h; ++y)
			for(auto x = 0u; x < t_w; ++x)
				data[x][y] *= other;
		return *this;
	}

	template<
		typename ScalarType
	>
	Matrix& operator/=(ScalarType const &other) {
		for(auto y = 0u; y < t_h; ++y)
			for(auto x = 0u; x < t_w; ++x)
				data[x][y] /= other;
		return *this;
	}

	//Casts

	template<
		typename DepBaseType = BaseType,
		std::enable_if_t<t_w == 1 && t_h == 1, DepBaseType>
	>
	operator BaseType () const {
		return data[0u][0u];
	}

	//Matrix operations

	//in-place transpose
	ENABLE_IF_SQUARE
	void Transpose() {
		for(auto x = 0u; x < t_w; ++x)
			for(auto y = x + 1; y < t_h; ++y)
				std::swap(data[x][y], data[y][x]);
	}

	//functional transpose
	static Matrix<BaseType, t_h, t_w>
	Transpose(Matrix<BaseType, t_w, t_h> const& other) {
		auto ret = Matrix<BaseType, t_h, t_w>{};
		for(auto x = 0u; x < t_w; ++x)
			for(auto y = 0u; y < t_h; ++y)
				ret[x][y] = other[y][x];
		return ret;
	}

	template<
		typename DepBaseType = BaseType,
		typename = std::enable_if_t<t_w == t_h, DepBaseType>,
		typename PowType = X_TO_POWER_TYPE(BaseType(0), t_w),
		typename InverseDowncastType = INVERSE_TYPE(DownCastType(0))
	>
	auto Inverse(PowType &determinant) const
	-> Matrix<INVERSE_TYPE(BaseType(0)), t_w, t_h, InverseDowncastType> {
		auto ret = Matrix<INVERSE_TYPE(BaseType(0)), t_w, t_h, InverseDowncastType>::Identity();
		auto cpy = *this;

		//determinant = BaseType(1);
		auto detInternal = DownCastType{1};

		for(auto y = 0u; y < t_h; ++y) {
			auto maxCol = cpy.MaxInSubColumn(y, y);
			if(maxCol != y) {
				cpy.RowSwap(y, maxCol);
				ret.RowSwap(y, maxCol);
				detInternal = -detInternal;
			}
		}

		for(auto y = 0u; y < t_h; ++y) {
			//Start by dividing the row to have a leading 1
			auto div = static_cast<DownCastType>(data[0u][y]);
			if(div == DownCastType{0}) {
				determinant = PowType{0};
				return ret;
			}
			detInternal /= div; //what if this operator isn't defined?
			for(auto x = 0u; x < t_w; ++x) {
				ret.data[x][y] /= div;
				cpy.data[x][y] /= div;
			}
			//get an extra zero up front on the subsequent rows
			for(auto yy = y + 1; yy < t_h; ++yy) {
				auto div = static_cast<DownCastType>(data[y][yy]);
				if(div == DownCastType{0}) {
					determinant = PowType{0};
					return ret;
				}
				detInternal /= div;
				for(auto x = 0u; x < t_w; ++x) {
					ret.data[x][yy] -= ret.data[x][y] / div;
					cpy.data[x][yy] -= cpy.data[x][y] / div;
				}
			}
		}

		//then back substitute
		for(auto y = t_h - 1; y > 0u; --y) {
			for(auto x = 0u; x < t_w; ++x) {
				ret.data[x][y-1] = static_cast<INVERSE_TYPE(BaseType(0))>(
					(InverseDowncastType)ret.data[x][y-1]
					- (InverseDowncastType)ret.data[x][y] * (InverseDowncastType)data[x][y]
				);
			}
		}
		determinant = PowType{detInternal};
		return ret;
	}

	//vector operations

	template<typename DepBaseType = BaseType>
	std::enable_if_t<t_w == 1u, DepBaseType> Length() {
		return BaseType(
			std::sqrt(
				DownCastType(
					data[0u][0u] * data[0u][0u] + data[0u][1u] * data[0u][1u]
				)
			)
		);
	}

	//Constants

	ENABLE_IF_SQUARE
	static CONSTEXPR Matrix Identity() {
		Matrix ret;
		for(auto x = 0u; x < t_w; ++x)
			for(auto y = 0u; y < t_h; ++y)
				ret.data[x][y] = x == y? BaseType(1) : BaseType(0);
		return ret;
	}

	static CONSTEXPR Matrix Zero() {
		Matrix ret;
		for(auto x = 0u; x < t_w; ++x)
			for(auto y = 0u; y < t_h; ++y)
				ret.data[x][y] = BaseType(0);
		return ret;
	}

	//Helpers

	void RowSwap(unsigned r1, unsigned r2) {
		for(auto x = 0u; x < t_w; ++x) {
			std::swap(data[x][r1], data[x][r2]);
		}
	}

	unsigned MaxInSubColumn(unsigned column, unsigned startRow) {
		auto col = 1u;
		auto max = data[startRow][column];
		for(auto y = startRow + 1u; y < t_h; ++y) {
			if(data[column][y] > max) {
				max = data[column][y];
				col = y;
			}
		}
		return col;
	}

	BaseType data[t_w][t_h];
};

template<typename BaseType, unsigned t_h>
using Vector = Matrix<BaseType, 1u, t_h>;

//type traits

template<typename T>
class is_matrix {
public:
	static bool const value = false;
};

template<typename BaseType, unsigned t_w, unsigned t_h, typename DownCastType>
class is_matrix<Matrix<BaseType, t_w, t_h, DownCastType>> {
public:
	static bool const value = true;
};

//operators

//unary
template<typename BaseType, unsigned t_w, unsigned t_h, typename DownCastType>
CONSTEXPR Matrix<BaseType, t_w, t_h, DownCastType> operator+(Matrix<BaseType, t_w, t_h, DownCastType> const &op) {
	return op;
}

template<typename BaseType, unsigned t_w, unsigned t_h, typename DownCastType>
CONSTEXPR Matrix<BaseType, t_w, t_h, DownCastType> operator-(Matrix<BaseType, t_w, t_h, DownCastType> const &op) {
	return Matrix<BaseType, t_w, t_h, DownCastType>::Zero() - op;
}

//scalar
template<
	typename ScalarType,
	typename = std::enable_if_t<!is_matrix<ScalarType>::value, ScalarType>,
	typename BaseType,
	unsigned t_w,
	unsigned t_h,
	typename DownCastType
>
CONSTEXPR auto operator*(Matrix<BaseType, t_w, t_h, DownCastType> const& left, ScalarType const &right)
-> Matrix<std::remove_const_t<decltype(left(0u, 0u) * right)>, t_w, t_h, DownCastType> {
	Matrix<std::remove_const_t<decltype(left(0u, 0u) * right)>, t_w, t_h, DownCastType> ret;

	for(auto y = 0u; y < t_h; ++y)
		for(auto x = 0u; x < t_w; ++x)
			ret(x, y) = left(x, y) * right;
	return ret;
}

template<
	typename ScalarType,
	typename = std::enable_if_t<!is_matrix<ScalarType>::value, ScalarType>,
	typename BaseType,
	unsigned t_w,
	unsigned t_h,
	typename DownCastType
>
CONSTEXPR auto operator*(ScalarType left, Matrix<BaseType, t_w, t_h, DownCastType> const& right)
-> Matrix<std::remove_const_t<decltype(left * right(0u, 0u))>, t_w, t_h, DownCastType> {
	Matrix<std::remove_const_t<decltype(left * right(0u, 0u))>, t_w, t_h, DownCastType> ret;

	for(auto y = 0u; y < t_h; ++y)
		for(auto x = 0u; x < t_w; ++x)
			ret(x, y) = left * right(x, y);
	return ret;
}

template<
	typename ScalarType,
	typename BaseType,
	unsigned t_w,
	unsigned t_h,
	typename DownCastType
>
CONSTEXPR auto operator/(Matrix<BaseType, t_w, t_h, DownCastType> const& left, ScalarType const &right)
-> Matrix<std::remove_const_t<decltype(left(0u, 0u) / right)>, t_w, t_h, DownCastType> {
	Matrix<std::remove_const_t<decltype(left(0u, 0u) / right)>, t_w, t_h, DownCastType> ret;

	for(auto y = 0u; y < t_h; ++y)
		for(auto x = 0u; x < t_w; ++x)
			ret(x, y) = left(x, y) / right;
	return ret;
}

//arithmatic

template<typename BaseType, unsigned t_w, unsigned t_h, typename DownCastType>
CONSTEXPR Matrix<BaseType, t_w, t_h, DownCastType> operator+(
	Matrix<BaseType, t_w, t_h, DownCastType> const &left,
	Matrix<BaseType, t_w, t_h, DownCastType> const &right
) {
	auto ret = left;
	ret += right;
	return ret;
}

template<typename BaseType, unsigned t_w, unsigned t_h, typename DownCastType>
CONSTEXPR Matrix<BaseType, t_w, t_h, DownCastType> operator-(Matrix<BaseType, t_w, t_h, DownCastType> const &left, Matrix<BaseType, t_w, t_h, DownCastType> const &right) {
	auto ret = left;
	ret -= right;
	return ret;
}

template<
	typename BaseType1,
	typename BaseType2,
	unsigned t_a,
	unsigned t_b,
	unsigned t_c,
	typename DownCastType,
	typename DownCastType2
>
CONSTEXPR auto operator*(
		Matrix<BaseType1, t_a, t_b, DownCastType> const &left,
	   	Matrix<BaseType2, t_c, t_a, DownCastType2> const &right
) -> Matrix<
	std::remove_const_t<decltype(left(0u, 0u) * right(0u, 0u))>,
	t_c, t_b,
	std::remove_const_t<decltype(DownCastType(0) * DownCastType2(0))>
> {
	Matrix<
		std::remove_const_t<decltype(left(0u, 0u) * right(0u, 0u))>,
		t_c, t_b,
		std::remove_const_t<decltype(DownCastType(0) * DownCastType2(0))>
	> ret;

	for(auto y = 0u; y < t_b; ++y) {
		for(auto x = 0u; x < t_c; ++x) {
			decltype(left(0u, 0u) * right(0u, 0u)) acc(0);
			for(auto i = 0u; i < t_a; ++i)
				acc += left(i, y) * right(x, i);
			ret(x, y) = acc;
		}
	}

	return ret;
}

//comparison

template<typename BaseType, unsigned t_w, unsigned t_h, typename DownCastType>
CONSTEXPR bool operator==(Matrix<BaseType, t_w, t_h, DownCastType> const &left, Matrix<BaseType, t_w, t_h, DownCastType> const& right) {
	for(auto y = 0u; y < t_h; ++y)
		for(auto x = 0u; x < t_w; ++x)
			if(left(x,y) != right(x,y))
				return false;
	return true;
}

//vector (row)

template<typename BaseType1, typename BaseType2, unsigned t_w, typename DownCastType, typename DownCastType2>
auto operator|(Matrix<BaseType1, t_w, 1u, DownCastType> const &left, Matrix<BaseType2, t_w, 1u, DownCastType2> const &right)
	-> decltype(left[0u] * right[0u]) {
	decltype(left[0u] * right[0u]) acc{0};
	for(auto x = 0u; 0u < t_w; ++x)
		acc += left[x] * right[x];
	return acc;
}

template< typename BaseType1, typename BaseType2, typename DownCastType, typename DownCastType2>
auto operator^(Matrix<BaseType1, 3u, 1u, DownCastType> const &left, Matrix<BaseType2, 3u, 1u, DownCastType> const &right)
	-> Matrix<decltype(left[0u] * right[0u]), 1u, 3u, decltype(DownCastType(0) * DownCastType2(0))>
{
	Matrix<decltype(left[0u] * right[0u]), 1u, 3u> ret;
	ret[0u] = left[1u] * right[2u] - left[2u] * right[1u];
	ret[1u] = left[2u] * right[0u] - left[0u] * right[2u];
	ret[2u] = left[0u] * right[1u] - left[1u] * right[0u];
	return ret;
}

template<typename BaseType1, typename BaseType2, typename DownCastType, typename DownCastType2>
auto operator^(Matrix<BaseType1, 2u, 1u, DownCastType> const &left, Matrix<BaseType2, 2u, 1u, DownCastType2> const &right)
	-> decltype(left[0u] * right[0u])
{
	return left[0u] * right[1u] - left[1u] * right[0u];
}

//vector (column)

template<typename BaseType1, typename BaseType2, unsigned t_h, typename DownCastType, typename DownCastType2>
auto operator|(Matrix<BaseType1, 1u, t_h, DownCastType> const &left, Matrix<BaseType2, 1u, t_h, DownCastType2> const &right)
	-> decltype(left[0u] * right[0u]) {
	decltype(left[0u] * right[0u]) acc{0};
	for(auto y = 0u; 0u < t_h; ++y)
		acc += left[y] * right[y];
	return acc;
}

template< typename BaseType1, typename BaseType2, typename DownCastType, typename DownCastType2>
auto operator^(Matrix<BaseType1, 1u, 3u, DownCastType> const &left, Matrix<BaseType2, 1u, 3u, DownCastType2> const &right)
	-> Matrix<decltype(left[0u] * right[0u]), 1u, 3u, decltype(DownCastType(0) * DownCastType2(0))>
{
	Matrix<decltype(left[0u] * right[0u]), 1u, 3u> ret;
	ret[0u] = left[1u] * right[2u] - left[2u] * right[1u];
	ret[1u] = left[2u] * right[0u] - left[0u] * right[2u];
	ret[2u] = left[0u] * right[1u] - left[1u] * right[0u];
	return ret;
}

template<typename BaseType1, typename BaseType2, typename DownCastType, typename DownCastType2>
auto operator^(Matrix<BaseType1, 1u, 2u, DownCastType> const &left, Matrix<BaseType2, 1u, 2u, DownCastType2> const &right)
	-> decltype(left[0u] * right[0u])
{
	return left[0u] * right[1u] - left[1u] * right[0u];
}


//class specialisations

template<typename BaseType, typename DownCastType>
class Matrix<BaseType, 1, 2, DownCastType> {
public:
	//accessors

	Matrix() : x(0), y(0) {}
	Matrix(std::initializer_list<BaseType> const &init)
	{
		auto it = init.begin();
		x = *it;
		++it;
		y = *it;
	}
	Matrix(BaseType x, BaseType y)
		: x(x)
		, y(y)
	{}

	Matrix& operator=(Matrix const &other) = default;

	BaseType & operator() (unsigned, unsigned y) {
	 	return y & 1? this->y : this->x;
	}

	BaseType const& operator() (unsigned, unsigned y) const {
	 	return y & 1? this->y : this->x;
	}

	//special for column vectors

	BaseType & operator[] (unsigned y) {
		return y & 1? this->y : this->x;
	}

	BaseType const& operator[] (unsigned y) const {
		return y & 1? this->y : this->x;
	}

	// Assignment operators

	Matrix& operator+=(Matrix const& other) {
		this->x += other.x;
		this->y += other.y;
		return *this;
	}

	Matrix& operator-=(Matrix const& other) {
		this->x -= other.x;
		this->y -= other.y;
		return *this;
	}

	template<
		typename ScalarType
	>
	Matrix& operator*=(ScalarType const &other) {
		this->x *= other;
		this->y *= other;
		return *this;
	}

	template<
		typename ScalarType
	>
	Matrix& operator/=(ScalarType const &other) {
		this->x /= other;
		this->y /= other;
		return *this;
	}

	//Matrix operations

	//functional transpose
	static Matrix<BaseType, 2, 1, DownCastType>
	Transpose(Matrix<BaseType, 1, 2, DownCastType> const& other) {
		auto ret = Matrix<BaseType, 2, 1, DownCastType>{};
		for(auto y = 0u; y < 2; ++y)
			ret[0u][y] = other[y][0u];
		return ret;
	}

	//vector operations

	BaseType Length() {
		return BaseType(
			std::sqrt(DownCastType(x*x + y*y))
		);
	}

	//Constants

	static CONSTEXPR Matrix Zero() {
		Matrix ret;
		ret.x = BaseType(0);
		ret.y = BaseType(0);
		return ret;
	}

	BaseType x, y;
};



template<typename BaseType, typename DownCastType>
class Matrix<BaseType, 1, 3, DownCastType> {
public:
	//accessors

	Matrix() : x(0), y(0), z(0) {}
	Matrix(std::initializer_list<BaseType> const &init)
	{
		auto it = init.begin();
		x = *it;
		++it;
		y = *it;
		++it;
		z = *it;
	}
	Matrix(BaseType x, BaseType y, BaseType z)
		: x(x)
		, y(y)
		, z(z)
	{}
	Matrix(Matrix<BaseType, 1, 2, DownCastType> vec2, BaseType z)
		: x(vec2.x)
		, y(vec2.y)
		, z(z)
	{}

	Matrix& operator=(Matrix const &other) = default;

	BaseType & operator() (unsigned, unsigned y) {
	 	switch(y) {
		case 0:
			return x;
		case 1:
			return this->y;
		case 2:
			return z;
		default:
			return BaseType(0);
		}
	}

	BaseType const& operator() (unsigned, unsigned y) const {
	 	switch(y) {
		case 0:
			return x;
		case 1:
			return this->y;
		case 2:
			return z;
		default:
			return BaseType(0);
		}
	}

	//special for column vectors

	BaseType & operator[] (unsigned y) {
	 	switch(y) {
		case 0:
			return x;
		case 1:
			return this->y;
		case 2:
			return z;
		default:
			return BaseType(0);
		}
	}

	BaseType const& operator[] (unsigned y) const {
	 	switch(y) {
		case 0:
			return x;
		case 1:
			return this->y;
		case 2:
			return z;
		default:
			return BaseType(0);
		}
	}

	// Assignment operators

	Matrix& operator+=(Matrix const& other) {
		this->x += other.x;
		this->y += other.y;
		this->z += other.z;
		return *this;
	}

	Matrix& operator-=(Matrix const& other) {
		this->x -= other.x;
		this->y -= other.y;
		this->z -= other.z;
		return *this;
	}

	template<
		typename ScalarType
	>
	Matrix& operator*=(ScalarType const &other) {
		this->x *= other;
		this->y *= other;
		this->z *= other;
		return *this;
	}

	template<
		typename ScalarType
	>
	Matrix& operator/=(ScalarType const &other) {
		this->x /= other;
		this->y /= other;
		this->z /= other;
		return *this;
	}

	//Matrix operations

	//functional transpose
	static Matrix<BaseType, 3, 1, DownCastType>
	Transpose(Matrix<BaseType, 1, 3, DownCastType> const& other) {
		auto ret = Matrix<BaseType, 3, 1, DownCastType>{};
		ret(0, 0) = other.x;
		ret(1, 0) = other.y;
		ret(2, 0) = other.z;
		return ret;
	}

	//vector operations

	BaseType Length() {
		return BaseType(
			std::sqrt(static_cast<DownCastType>(x*x + y*y + z*z))
		);
	}

	//Constants

	static CONSTEXPR Matrix Zero() {
		Matrix ret;
		ret.x = BaseType(0);
		ret.y = BaseType(0);
		ret.z = BaseType(0);
		return ret;
	}

	BaseType x, y, z;
};



template<typename BaseType, typename DownCastType>
class Matrix<BaseType, 1, 4, DownCastType> {
public:
	//accessors

	Matrix() : x(0.0), y(0.0), z(0.0), w(0.0) {}
	Matrix(std::initializer_list<BaseType> const &init)
	{
		auto it = init.begin();
		x = *it;
		++it;
		y = *it;
		++it;
		z = *it;
		++it;
		w = *it;
	}
	Matrix(BaseType x, BaseType y, BaseType z, BaseType w)
		: x(x)
		, y(y)
		, z(z)
		, w(w)
	{}
	Matrix(Matrix<BaseType, 1, 2, DownCastType> xy, BaseType z, BaseType w)
		: x(xy.x)
		, y(xy.y)
		, z(z)
		, w(w)
	{}
	Matrix(Matrix<BaseType, 1, 2, DownCastType> &xy, Matrix<BaseType, 1, 2, DownCastType> &zw)
		: x(xy.x)
		, y(xy.y)
		, z(zw.x)
		, w (zw.y)
	{}
	Matrix(Matrix<BaseType, 1, 3, DownCastType> &xyz, BaseType w)
		: x(xyz.x)
		, y(xyz.y)
		, z(xyz.z)
		, w(w)
	{}
	Matrix(Matrix<BaseType, 1, 3, DownCastType> &xyz)
		: x(xyz.x)
		, y(xyz.y)
		, z(xyz.z)
		, w(0.0)
	{}

//compatibility is hard :(
#ifdef _MSC_VER
#else
	Matrix(Matrix<BaseType, 1, 4, DownCastType> &) = default;
	Matrix(Matrix<BaseType, 1, 4, DownCastType> &&) = default;
#endif

	Matrix& operator=(Matrix const &other) = default;

	explicit operator Matrix<BaseType, 1, 3, DownCastType>() const {
		return Matrix<BaseType, 1, 3, DownCastType>(ret(x/w, y/w, z/w));
	}

	void Project() {
		*this /= w;
	}

	void Project2d() {
		*this /= w;
		*this /= z;
	}

	BaseType & operator() (unsigned, unsigned y) {
	 	switch(y) {
		default:
		case 0:
			return x;
		case 1:
			return this->y;
		case 2:
			return z;
		case 3:
			return w;
		}
	}

	BaseType const& operator() (unsigned, unsigned y) const {
	 	switch(y) {
		default:
		case 0:
			return x;
		case 1:
			return this->y;
		case 2:
			return z;
		case 3:
			return w;
		}
	}

	//special for column vectors

	BaseType & operator[] (unsigned y) {
	 	switch(y) {
		default:
		case 0:
			return x;
		case 1:
			return this->y;
		case 2:
			return z;
		case 3:
			return w;
		}
	}

	BaseType const& operator[] (unsigned y) const {
	 	switch(y) {
		default:
		case 0:
			return x;
		case 1:
			return this->y;
		case 2:
			return z;
		case 3:
			return w;
		}
	}

	// Assignment operators

	Matrix& operator+=(Matrix const& other) {
		this->x += other.x;
		this->y += other.y;
		this->z += other.z;
		this->w += other.w;
		return *this;
	}

	Matrix& operator-=(Matrix const& other) {
		this->x -= other.x;
		this->y -= other.y;
		this->z -= other.z;
		this->w -= other.w;
		return *this;
	}

	template<
		typename ScalarType
	>
	Matrix& operator*=(ScalarType const &other) {
		this->x *= other;
		this->y *= other;
		this->z *= other;
		this->w *= other;
		return *this;
	}

	template<
		typename ScalarType
	>
	Matrix& operator/=(ScalarType const &other) {
		this->x = this->x / other;
		this->y = this->y / other;
		this->z = this->z / other;
		this->w = this->w / other;
		return *this;
	}

	//Matrix operations

	//functional transpose
	static Matrix<BaseType, 3, 1, DownCastType>
	Transpose(Matrix<BaseType, 1, 3, DownCastType> const& other) {
		auto ret = Matrix<BaseType, 3, 1, DownCastType>{};
		ret(0, 0) = other.x;
		ret(1, 0) = other.y;
		ret(2, 0) = other.z;
		ret(3, 0) = other.w;
		return ret;
	}

	//vector operations

	BaseType Length() {
		return BaseType(
			std::sqrt(static_cast<DownCastType>(x*x + y*y + z*z + w*w))
		);
	}

	//Constants

	static CONSTEXPR Matrix Zero() {
		Matrix ret;
		ret.x = BaseType(0);
		ret.y = BaseType(0);
		ret.z = BaseType(0);
		ret.w = BaseType(0);
		return ret;
	}

	//accessors

	Matrix<BaseType, 1, 2, DownCastType> GetXY() {
		return {x,y};
	}

	BaseType x, y, z, w;
};
