#include "movsx.h"
#include "oxtra/codegen/helper.h"

void codegen::Movsx::execute_operation(codegen::CodeBatch& batch, encoding::RiscVRegister dst, encoding::RiscVRegister src) const {
	/* Thus they will have to be sign-extended/zero-extended.
	 * Otherwise the optimization will fail (load full 8-byte register, and store the interesting parts).
	 * [It will fail, because the interesting, stored parts, are larger than they should be]
	 * With a simple hack of shifting all the way up, and down again, we can fill the space with the
	 * highest bit. */
	const auto shift_amount = 64 - 8 * get_operand(1).get_size();
	batch += encoding::SLLI(dst, src, shift_amount);
	batch += encoding::SRAI(dst, dst, shift_amount);
}