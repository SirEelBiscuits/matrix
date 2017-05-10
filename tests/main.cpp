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

Tee_Test(test_matrix_scaling_with_mutating_types) {

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
