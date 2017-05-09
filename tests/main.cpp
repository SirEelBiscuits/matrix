#include "../matrix.h"
#include "tee/tee.hpp"

#include <vector>
#include <string>
#include <tuple>
#include <iostream>
#include <regex>

using namespace std;

Tee_Test(test_basic_matrix_addition) {
	auto m = Matrix<float, 2, 2>{1, 2, 3, 4};
	auto m2 = Matrix<float, 2, 2>{5, 6, 7, 8};
	auto m3 = Matrix<float, 2, 2>{1, 3, 6, 7};

	Tee_SubTest(test_adding_2x2) {
		auto r = m + m2;
		assert(
			r(0,0) == 6
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
