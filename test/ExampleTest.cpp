#define CATCH_CONFIG_MAIN

#include "catch2/catch.hpp"

#include "oxtra/elf/Elf.h"

TEST_CASE("simple") {
	REQUIRE(read_elf(nullptr, nullptr, sizeof(int)) == 0);
}