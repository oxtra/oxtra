# In here, all .cpp files are listed that will be added to the executable
add_library(oxtra_libs
		STATIC
		oxtra/elf/ELF.cpp
)

target_link_libraries(oxtra_libs
		fadec
)

target_include_directories(oxtra_libs
		PUBLIC
		../src
)