cmake_minimum_required(VERSION 3.5)

project(unit_tests)

enable_testing()

add_executable(${PROJECT_NAME}
		oxtra/elf/elf_test.cpp
		oxtra/utils/static_list_test.cpp
		oxtra/codegen/codestore/codestore_test.cpp)

target_link_libraries(${PROJECT_NAME} PUBLIC
		fadec
		oxtra_libs
)

target_include_directories(${PROJECT_NAME} INTERFACE catch2)

add_test(
		NAME ${PROJECT_NAME}
		COMMAND ${PROJECT_NAME}
)