#include "../../catch2/catch.hpp"
#include "oxtra/utils/static_list.h"

using namespace utils;

TEST_CASE("static_list supports adding elements", "[static_list]") {
	auto list = StaticList<uint64_t>(16);

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
		REQUIRE(ptr != nullptr);

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
		REQUIRE(ptr != nullptr);

		for (int i = 0; i < 5; i++) {
			ptr[i] = i + 10;
		}

		auto element = list.add(nullptr, 9);
		element = list.add(element, ptr, 5);

		free(ptr);

		for (uint8_t i = 0; i < 6; i++) {
			REQUIRE(element[i] == (9 + i));
		}
	}
}

TEST_CASE("static_list supports copying elements", "[static_list]") {
	auto list = StaticList<uint32_t>(2);
	const auto list_head = list.add(nullptr, -1);

	SECTION("add overflowing element (reallocation required)") {
		auto start = list.add(nullptr, 42);
		REQUIRE(*start == 42);

		auto newStart = list.add(start, 43);

		REQUIRE(start != newStart);
		REQUIRE(list_head[0] == (uint32_t) -1);
		REQUIRE(newStart[0] == 42);
		REQUIRE(newStart[1] == 43);
	}

	SECTION("add too many consecutive elements") {
		auto start = list.add(nullptr, 42);
		REQUIRE(start[0] == 42);

		REQUIRE(list.get_size_left() == 0);

		start = list.add(start, 43);
		REQUIRE(start[1] == 43);

		REQUIRE_THROWS(list.add(start, 44));
	}
}

TEST_CASE("static_list supports allocating entries", "[static_list]") {
	auto list = StaticList<uint8_t>(3);
	auto& start = list.allocate_entry();
	start = 99;

	SECTION("use allocate_entry as start") {
		list.allocate_entry() = 100;
		list.allocate_entry() = 200;

		REQUIRE((&start)[0] == 99);
		REQUIRE((&start)[1] == 100);
		REQUIRE((&start)[2] == 200);

		REQUIRE(list.get_size_left() == 0);

		auto& new_entry = list.allocate_entry();
		new_entry = 42;

		REQUIRE(list.get_size_left() > 0);

		REQUIRE((&start)[0] == 99);
		REQUIRE((&start)[1] == 100);
		REQUIRE((&start)[2] == 200);
		REQUIRE(new_entry == 42);
	}
}