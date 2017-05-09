#include "../matrix.h"
#include "tee/tee.hpp"

#include <vector>
#include <string>
#include <tuple>
#include <iostream>
#include <regex>

using namespace std;

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
