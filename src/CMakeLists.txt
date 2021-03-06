enable_language(C ASM)

SET(ASM_OPTIONS "-x assembler-with-cpp")
SET(CMAKE_ASM_FLAGS "${CFLAGS} ${ASM_OPTIONS}")

# In here, all .cpp files are listed that will be added to the executable
add_library(oxtra_libs
		STATIC
		oxtra/codegen/jump-table/jump_table.s
		oxtra/arguments/arguments.cpp
		oxtra/elf/elf.cpp
		oxtra/codegen/codegen.cpp
		oxtra/codegen/codestore/codestore.cpp
		oxtra/dispatcher/dispatcher.cpp
		oxtra/dispatcher/syscalls.cpp
		oxtra/debugger/debugger.cpp
		oxtra/debugger/debugger_menu.cpp
		oxtra/debugger/debugger_print.cpp
		oxtra/codegen/encoding/encoding.cpp
		oxtra/codegen/decoding/decoding.cpp
		oxtra/codegen/helper.cpp
		oxtra/codegen/code_batch.cpp
		oxtra/codegen/instruction.cpp
		oxtra/codegen/transform_instruction.cpp
		oxtra/codegen/jump-table/jump_table.cpp
		oxtra/codegen/instructions/unsupported_instruction.cpp
		oxtra/codegen/instructions/binary_operation.cpp
		oxtra/codegen/instructions/unary_operation.cpp
		oxtra/codegen/instructions/arithmetic/add.cpp
		oxtra/codegen/instructions/arithmetic/adc.cpp
		oxtra/codegen/instructions/arithmetic/neg.cpp
		oxtra/codegen/instructions/arithmetic/sub.cpp
		oxtra/codegen/instructions/arithmetic/idiv.cpp
		oxtra/codegen/instructions/arithmetic/inc.cpp
		oxtra/codegen/instructions/arithmetic/dec.cpp
		oxtra/codegen/instructions/arithmetic/mul.cpp
		oxtra/codegen/instructions/control-flow/syscall.cpp
		oxtra/codegen/instructions/control-flow/call.cpp
		oxtra/codegen/instructions/control-flow/jmp.cpp
		oxtra/codegen/instructions/control-flow/ret.cpp
		oxtra/codegen/instructions/memory/leave.cpp
		oxtra/codegen/instructions/memory/pop.cpp
		oxtra/codegen/instructions/memory/push.cpp
		oxtra/codegen/instructions/memory/mov.cpp
		oxtra/codegen/instructions/memory/movsx.cpp
		oxtra/codegen/instructions/memory/movzx.cpp
		oxtra/codegen/instructions/memory/cex.cpp
		oxtra/codegen/instructions/memory/csep.cpp
		oxtra/codegen/instructions/memory/lea.cpp
		oxtra/codegen/instructions/control-flow/conditional-jumps/jcc.cpp
		oxtra/codegen/instructions/comparable/comparable.cpp
		oxtra/codegen/instructions/comparable/cmp.cpp
		oxtra/codegen/instructions/comparable/test.cpp
		oxtra/codegen/instructions/flags/clc.cpp
		oxtra/codegen/instructions/flags/stc.cpp
		oxtra/codegen/instructions/string/stos.cpp
		oxtra/codegen/instructions/string/repeatable.cpp
		oxtra/codegen/instructions/string/scas.cpp
		oxtra/codegen/instructions/string/cmps.cpp
		oxtra/codegen/instructions/string/lods.cpp
		oxtra/codegen/instructions/string/movs.cpp
		oxtra/codegen/instructions/memory/cmov/cmov.cpp
		oxtra/codegen/instructions/logic/bit-test/bittest.cpp
		oxtra/codegen/instructions/logic/setcc/setcc.cpp
		oxtra/codegen/instructions/logic/logic.cpp
		oxtra/logger/logger.cpp
)

target_link_libraries(oxtra_libs
		fadec
		fmt
)

target_include_directories(oxtra_libs
		PUBLIC
		../src
)
