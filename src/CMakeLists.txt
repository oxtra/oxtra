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
		oxtra/codegen/code_batch.cpp
		oxtra/codegen/instruction.cpp
		oxtra/codegen/transform_instruction.cpp
		oxtra/codegen/jumptable/jump_table.cpp
		oxtra/codegen/jumptable/jump_table.s
		oxtra/codegen/binary_operation.cpp
		oxtra/codegen/unary_operation.cpp
		oxtra/codegen/instructions/mov.cpp
		oxtra/codegen/instructions/add.cpp
		oxtra/codegen/instructions/shl.cpp
		oxtra/codegen/instructions/shr.cpp
		oxtra/codegen/instructions/sar.cpp
		oxtra/codegen/instructions/sub.cpp
		oxtra/codegen/instructions/inc.cpp
		oxtra/codegen/instructions/dec.cpp
		oxtra/codegen/instructions/imul.cpp
		oxtra/codegen/instructions/syscall.cpp)

target_link_libraries(oxtra_libs
		fadec
		spdlog
)

target_include_directories(oxtra_libs
		PUBLIC
		../src
)