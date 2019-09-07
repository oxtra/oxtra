#ifndef OXTRA_ADC_H
#define OXTRA_ADC_H

#include "oxtra/codegen/binary_operation.h"

namespace codegen {
	class Adc : public BinaryOperation {
	public:
		explicit Adc(const fadec::Instruction& inst)
				: BinaryOperation{inst, flags::all, flags::carry} {}

		void execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, encoding::RiscVRegister src) const override;
	};
}

#endif //OXTRA_ADC_H
