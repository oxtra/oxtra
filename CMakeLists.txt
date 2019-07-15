cmake_minimum_required(VERSION 3.11)

string(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_LOWER)
if (CMAKE_BUILD_TYPE_LOWER STREQUAL "release")
	add_compile_definitions(SPDLOG_ACTIVE_LEVEL=3)
	add_compile_definitions(RELEASE)
else()
	message(STATUS "This project has been created with a debug configuration.")
	add_compile_definitions(SPDLOG_ACTIVE_LEVEL=0)
	add_compile_definitions(DEBUG)
endif()

project(oxtra)

if (CMAKE_COMPILER_IS_GNUCC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wl,-Ttext-segment=0x7F000000")
else()
	message(FATAL_ERROR "Only GNUCC is supported.")
endif()

set(CMAKE_EXE_LINKER_FLAGS "-static")
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)

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