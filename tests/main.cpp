#include "../matrix.h"
#include "tee/tee.hpp"

#include <vector>
#include <string>
#include <tuple>
#include <iostream>
#include <regex>

using namespace std;

Tee_Test(test_matrix_constants) {
	Tee_SubTest(test_matrix_identity) {
		auto i = Matrix<float, 4, 4>::Identity();

		for(auto x = 0u; x < 4; ++x)
			for(auto y = 0u; y < 4; ++y)
				assert(
					i(x,y) == (x == y ? 1.0f : 0.0f)
				);
	}

	Tee_SubTest(test_matrix_zero) {
		auto z = Matrix<float, 4, 4>::Zero();
		for(auto x = 0u; x < 4; ++x)
			for(auto y = 0u; y < 4; ++y)
				assert(z(x, y) == 0.0f);
	}
}

Tee_Test(test_matrix_comparison_operators) {
	Matrix<float, 2, 2> m1 {1, 2, 3, 4};
	Matrix<float, 2, 2> m2 {1, 2, 3, 4};
	Matrix<float, 2, 2> m3 {4, 3, 2, 1};
	Tee_SubTest(test_compare_equals) {
		assert(m1 == m2);
		assert(!(m1 == m3));
	}

	Tee_SubTest(test_compare_not_equals) {
		assert(m1 != m3);
		assert(!(m1 != m2));
	}

	Tee_SubTest(test_comparison_commutativity) {
		assert((m1 == m2) == (m2 == m1));
		assert((m1 != m3) == (m3 != m1));
	}
}

Tee_Test(test_matrix_transpose) {
	Matrix<float, 1, 4> m1 {1, 2, 3, 4};
	Matrix<float, 4, 1> m2 {1, 2, 3, 4};
	Matrix<float, 2, 3> m2x3 {1, 2, 2, 4, 3, 6};

	Tee_SubTest(test_basic_transpose_types) {
		assert(m1.Transpose() == m2);

		auto m3x2 = m2x3.Transpose();
		for(auto x = 0u; x < 3; ++x)
			for(auto y = 0u; y < 2; ++y)
				assert(m3x2(x, y) == (x+1) * (y+1));
	}

	Tee_SubTest(test_transpose_is_involuting) {
		assert(m1.Transpose().Transpose() == m1);
		assert(m2x3.Transpose().Transpose() == m2x3);
	}
}

Tee_Test(test_basic_matrix_addition) {
	auto m = Matrix<float, 2, 2>{1, 2, 3, 4};
	auto m2 = Matrix<float, 2, 2>{5, 6, 7, 8};
	auto m3 = Matrix<float, 2, 2>{1, 3, 6, 7};

	Tee_SubTest(test_adding_2x2) {
		auto r = m + m2;
		assert(
			r(0, 0) == 6
			&& r(1, 0) == 8
			&& r(0, 1) == 10
			&& r(1, 1) == 12
		);
	}

	Tee_SubTest(test_addition_is_commutative) {
		assert(m + m2 == m2 + m);
	}

	Tee_SubTest(test_addition_is_associative) {
		assert((m + m2) + m3 == m + (m2 + m3));
	}

	Tee_SubTest(test_negation_is_involuting) {
		assert(-(-m) == m);
	}

	Tee_SubTest(test_subtraction_negates_addition) {
		assert(m2 + m - m2 == m);
	}
}

Tee_Test(test_matrix_scaling_with_simple_types) {
	using m3x3 = Matrix<float, 3, 3>;
	m3x3 m{
		1, 2, 3,
		4, 5, 6,
		7, 8, 9
	};

	Tee_SubTest(test_scaling_simple) {
		auto m2 = 3 * m;
		for(auto x = 0u; x < 3; ++x)
			for(auto y = 0u; y < 3; ++y)
				assert(m(x, y) * 3 == m2(x, y));
	}

	Tee_SubTest(test_scaling_is_commutative) {
		assert(m * 2 == 2 * m);
	}

	Tee_SubTest(test_scaling_is_associative) {
		assert((3 * m) * 4 == 3 * (m * 4));
	}

	Tee_SubTest(test_scaling_by_zero_zeros_matrix) {
		assert(m * 0 == m3x3::Zero());
	}
}

template<int e_t>
class Meters {
public:
	float val{};

	Meters() = default;
	Meters& operator=(Meters other) {
		val = other.val;
	}
};

template<int e1, int e2>
Meters<e1 + e2>
operator*(Meters<e1> const& left, Meters<e2> const& right) {
	return Meters<e1 + e2>{left.val * right.val};
}

template<int e1>
bool
operator==(Meters<e1> const& left, Meters<e1> const& right) {
	return left.val == right.val;
}

template<int e1>
bool
operator!=(Meters<e1> const& left, Meters<e1> const& right) {
	return left.val != right.val;
}

Tee_Test(test_matrix_scaling_with_mutating_types) {
	using m2x2 = Matrix<Meters<1>, 2, 2>;
	m2x2 m{Meters<1>{1}, Meters<1>{2}, Meters<1>{3}, Meters<1>{4}};

	Tee_SubTest(test_scaling_simple) {
		auto m2 = Meters<1>{2} * m;
		for(auto x = 0u; x < 2; ++x)
			for(auto y = 0u; y < 2; ++y)
				assert((m(x, y) * Meters<1>{2}).val == m2(x, y).val);
	}

	Tee_SubTest(test_scaling_is_commutative) {
		assert(m * Meters<2>{2} == Meters<2>{2} * m);
	}

	Tee_SubTest(test_scaling_is_associative) {
		assert((Meters<3>{3} * m) * Meters<0>{4} == Meters<3>{3} * (m * Meters<0>{4}));
	}

	Tee_SubTest(test_scaling_by_zero_zeros_matrix) {
		assert(m * Meters<0>{0} == m2x2::Zero());
	}
}

Tee_Test(test_matrix_matrix_multiplication) {
	Matrix<float, 2, 2> m2x2 {4, 2, 8, 1};
	Matrix<float, 2, 2> m2x2ii {7, 3, 4, 9};

	Matrix<float, 3, 2> m3x2 {3, 8, 2, 4, 8, 0};
	Matrix<float, 1, 2> v {2, 2};
	Matrix<float, 1, 3> v3 {7, 3, 1};

	Tee_SubTest(test_basic_multiplication) {
		auto m = m2x2 * m2x2ii;
		assert(m == Matrix<float, 2, 2>{34, 30, 60, 33});
	}

	Tee_SubTest(test_mismatched_size_multiplication) {
		auto v2 = m2x2 * v;
		assert(v2 == Matrix<float, 1, 2> {12, 18});
		auto m2x2iii = m2x2 * m3x2;
		assert(m2x2iii(0,0) == 20);
	}

	Tee_SubTest(test_associativity) {
		assert((m2x2 * m2x2ii) * v == m2x2 * (m2x2ii * v));
		assert((m3x2 *  m3x2.Transpose()) * v3 == m3x2 * (m3x2.Transpose() * v3));
	}

	Tee_SubTest(test_identity_multiplication) {
		assert(m2x2 * Matrix<float, 2, 2>::Identity() == m2x2);
		assert(m2x2ii * Matrix<float, 2, 2>::Identity() == m2x2ii);
		assert(
			m3x2 * m3x2.Transpose() * Matrix<float, 3, 3>::Identity()
			== m3x2 * m3x2.Transpose()
		);
	}

	Tee_Subtest(test_zero_multiplication) {
		assert(m2x2 * decltype(m2x2)::Zero() == decltype(m2x2)::Zero());
		assert(m2x2ii * decltype(m2x2ii)::Zero() == decltype(m2x2ii)::Zero());
		assert(m3x2 * decltype(m3x2)::Zero() == decltype(m3x2)::Zero());
	}
}

Tee_Test(test_matrix_inverse) {

}

int main() {
	int successes;
	vector<string> fails;
	tie(successes, fails) = Tee::RunAllTests();
	for(auto f : fails)
		cout << f;
	cout << successes << " successes, "
		<< fails.size() << " failures." << endl;
	return fails.size() == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
