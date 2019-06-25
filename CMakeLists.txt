cmake_minimum_required(VERSION 3.11)

project(oxtra)

if (CMAKE_COMPILER_IS_GNUCC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif ()
if (MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
endif ()

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