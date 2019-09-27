#ifndef OXTRA_ROTATION_H
#define OXTRA_ROTATION_H

namespace codegen {
	class Rotation : public codegen::Instruction {
	public:
		explicit Rotation(const fadec::Instruction& inst)
			: Instruction{inst, flags::carry | flags::overflow, flags::none} {}

		void generate(CodeBatch& batch) const final;

	protected:
		virtual encoding::RiscVRegister rotate(CodeBatch& batch, encoding::RiscVRegister reg) const = 0;
	};
}

#include "rol.h"
#include "ror.h"

#endif //OXTRA_ROTATION_H
