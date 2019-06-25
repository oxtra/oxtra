#include "../../catch2/catch.hpp"
#include "oxtra/utils/static_list.h"

using namespace utils;

TEST_CASE("static_list supports adding elements", "[static_list]") {
	const size_t MAX_ELEMENTS = 16;
	auto list = StaticList<uint64_t>(MAX_ELEMENTS);

	SECTION("add non-consecutive element") {
		auto element = list.add(nullptr, 42);
		REQUIRE(*element == 42);
	}
	SECTION("add non-consecutive pointer") {
		auto ptr = std::make_unique<uint64_t>();
		*ptr = 1337;
		auto element = list.add(nullptr, ptr.get(), 1);
		REQUIRE(*element == 1337);
	}
	SECTION ("add multiple elements non-consecutive") {
		auto ptr = static_cast<uint64_t*>(malloc(sizeof(uint64_t) * 3));
		ptr[0] = 10;
		ptr[1] = 11;
		ptr[2] = 12;

		auto element = list.add(nullptr, ptr, 3);

		free(ptr);

		REQUIRE(element[0] == 10);
		REQUIRE(element[1] == 11);
		REQUIRE(element[2] == 12);
	}
	SECTION("add consecutive single element") {
		auto element = list.add(nullptr, 42);
		list.add(element, 43);

		REQUIRE(element[0] == 42);
		REQUIRE(element[1] == 43);
	}
	SECTION("add multiple consecutive elements") {
		auto ptr = static_cast<uint64_t*>(malloc(sizeof(uint64_t) * 5));
		for (int i = 0; i < 5; i++) {
			ptr[i] = i + 10;
		}

		auto element = list.add(nullptr, 9);
		element = list.add(element, ptr, 5);

		free(ptr);

		for (int i = 0; i < 6; i++) {
			REQUIRE(element[i] == (9 + i));
		}
	}
}

TEST_CASE("static_list supports copying elements", "[static_list]") {
	//TODO: support check for overflow
}

//TODO: test allocate_entry