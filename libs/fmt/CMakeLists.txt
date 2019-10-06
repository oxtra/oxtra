# Set the project name
project(fmt LANGUAGES CXX)

# Add a library with the above sources
add_library(${PROJECT_NAME}
		src/format.cc
		)

target_include_directories(${PROJECT_NAME}
		PUBLIC include
		)

set(CMAKE_CXX_FLAGS "-fpermissive -std=c++0x")