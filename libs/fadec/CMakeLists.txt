# Set the project name
project(fadec LANGUAGES C)

# Add a library with the above sources
add_library(${PROJECT_NAME}
		src/decode.c
		src/format.c
)

target_include_directories(${PROJECT_NAME}
		PUBLIC include
)

set(CMAKE_CXX_FLAGS "-fpermissive -std=c++0x")