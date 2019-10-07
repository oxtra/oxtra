cmake_minimum_required(VERSION 3.11)

string(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_LOWER)
if (CMAKE_BUILD_TYPE_LOWER STREQUAL "release")
	set(CMAKE_BUILD_TYPE Release)
	add_compile_definitions(RELEASE)

	add_compile_definitions(SPDLOG_ACTIVE_LEVEL=3)
else ()
	message(AUTHOR_WARNING "This project has been created with a debug configuration. Use -DCMAKE_BUILD_TYPE=release if this is not a debug environment.")
	set(CMAKE_BUILD_TYPE Debug)
	add_compile_definitions(DEBUG)

	add_compile_definitions(SPDLOG_ACTIVE_LEVEL=0)
endif ()

project(oxtra)

if (NOT CMAKE_COMPILER_IS_GNUCC)
	message(FATAL_ERROR "Only GNUCC is supported.")
endif ()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wl,-Ttext-segment=0x7F000000 -ffixed-s11")

set(CMAKE_EXE_LINKER_FLAGS "-static")
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

# Add sub directories
add_subdirectory(libs/fadec)
add_subdirectory(libs/spdlog)

add_subdirectory(src)

add_executable(${PROJECT_NAME} src/main.cpp)

target_link_libraries(${PROJECT_NAME}
		fadec
		oxtra_libs
)

add_subdirectory(test)