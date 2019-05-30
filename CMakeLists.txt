cmake_minimum_required(VERSION 3.11)

project(oxtra)

set(CMAKE_EXE_LINKER_FLAGS "-static")
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)

# Add sub directories
add_subdirectory(libs/fadec)

add_subdirectory(src)

add_executable(${PROJECT_NAME} src/main.cpp)

target_link_libraries(${PROJECT_NAME}
		fadec
		oxtra_libs
)

add_subdirectory(test)