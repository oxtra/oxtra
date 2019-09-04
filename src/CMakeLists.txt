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
		oxtra/codegen/helper.cpp
		oxtra/codegen/code_batch.cpp
		oxtra/codegen/instruction.cpp
		oxtra/codegen/transform_instruction.cpp
		oxtra/codegen/jump-table/jump_table.cpp
		oxtra/codegen/jump-table/jump_table.s
		oxtra/codegen/binary_operation.cpp
		oxtra/codegen/unary_operation.cpp
		oxtra/codegen/instructions/arithmetic/add.cpp
		oxtra/codegen/instructions/arithmetic/sub.cpp
		oxtra/codegen/instructions/arithmetic/inc.cpp
		oxtra/codegen/instructions/arithmetic/dec.cpp
		oxtra/codegen/instructions/arithmetic/imul.cpp
		oxtra/codegen/instructions/logic/shl.cpp
		oxtra/codegen/instructions/logic/shr.cpp
		oxtra/codegen/instructions/logic/sar.cpp
		oxtra/codegen/instructions/logic/nop.cpp
		oxtra/codegen/instructions/control-flow/syscall.cpp
		oxtra/codegen/instructions/control-flow/call.cpp
		oxtra/codegen/instructions/control-flow/jmp.cpp
		oxtra/codegen/instructions/control-flow/ret.cpp
		oxtra/codegen/instructions/control-flow/leave.cpp
		oxtra/codegen/instructions/memory/pop.cpp
		oxtra/codegen/instructions/memory/push.cpp
		oxtra/codegen/instructions/memory/mov.cpp
		oxtra/codegen/instructions/memory/movsx.cpp
		oxtra/codegen/instructions/memory/movzx.cpp
		oxtra/codegen/instructions/memory/cex.cpp
		oxtra/codegen/instructions/memory/lea.cpp
		oxtra/codegen/instructions/control-flow/conditional-jumps/jz.cpp
		oxtra/codegen/instructions/control-flow/conditional-jumps/jc.cpp
		oxtra/codegen/instructions/control-flow/conditional-jumps/jo.cpp
		oxtra/codegen/instructions/control-flow/conditional-jumps/js.cpp
		oxtra/codegen/instructions/control-flow/conditional-jumps/jp.cpp
		oxtra/codegen/instructions/control-flow/conditional-jumps/jnz.cpp
		oxtra/codegen/instructions/control-flow/conditional-jumps/jnc.cpp
		oxtra/codegen/instructions/control-flow/conditional-jumps/jno.cpp
		oxtra/codegen/instructions/control-flow/conditional-jumps/jns.cpp
		oxtra/codegen/instructions/control-flow/conditional-jumps/jnp.cpp
		oxtra/codegen/instructions/control-flow/conditional-jumps/ja.cpp
		oxtra/codegen/instructions/control-flow/conditional-jumps/jbe.cpp
		oxtra/codegen/instructions/control-flow/conditional-jumps/jg.cpp
		oxtra/codegen/instructions/control-flow/conditional-jumps/jge.cpp
		oxtra/codegen/instructions/control-flow/conditional-jumps/jl.cpp
		oxtra/codegen/instructions/control-flow/conditional-jumps/jle.cpp
		oxtra/codegen/instructions/control-flow/conditional-jumps/jcxz.cpp
		)

target_link_libraries(oxtra_libs
		fadec
		spdlog
)

target_include_directories(oxtra_libs
		PUBLIC
		../src
)