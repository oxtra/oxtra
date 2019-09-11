#include "../../catch2/catch.hpp"
#include "oxtra/utils/fixed_array.h"

TEST_CASE("fixed array test", "[fixed_array]") {
	utils::FixedArray<size_t> array{5};

	for (size_t i = 0; i < array.size(); ++i) {
		array[i] = i + 1;
	}

	SECTION("adding elements") {
		for (size_t i = 0; i < array.size(); ++i) {
			REQUIRE(array[i] == i + 1);
		}
	}

	SECTION("c++ iterator") {
		size_t result = 0;
		for (auto i : array) {
			result += i;
		}

		REQUIRE(result == 15);
	}
}