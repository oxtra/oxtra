cmake_minimum_required(VERSION 3.5)

project(oxtra)

set(CMAKE_EXE_LINKER_FLAGS "-static")
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)

enable_testing()

# Add sub directories
add_subdirectory(libs/fadec)
add_subdirectory(src)
add_subdirectory(test)