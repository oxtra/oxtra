enable_language(C ASM)

SET(ASM_OPTIONS "-x assembler-with-cpp")
SET(CMAKE_ASM_FLAGS "${CFLAGS} ${ASM_OPTIONS}")

# In here, all .cpp files are listed that will be added to the executable
add_library(oxtra_libs
		STATIC
		oxtra/dispatcher/dispatcher.s
		oxtra/arguments/arguments.cpp
		oxtra/elf/elf.cpp
		oxtra/codegen/codegen.cpp
		oxtra/codegen/codestore/codestore.cpp
		oxtra/dispatcher/dispatcher.cpp
		oxtra/codegen/encoding/encoding.cpp
		oxtra/codegen/decoding/decoding.cpp
		oxtra/codegen/codegen_instructions.cpp
		oxtra/codegen/helper.cpp
		oxtra/codegen/Test/instruction.cpp
		oxtra/codegen/Test/jumptable/jump_table.cpp)

target_link_libraries(oxtra_libs
		fadec
		spdlog
)

target_include_directories(oxtra_libs
		PUBLIC
		../src
)