#ifndef OXTRA_ADD_H
#define OXTRA_ADD_H

#include "oxtra/codegen/Test/Instruction.h"
#include "../DoubleOperation.h"

class Adc : public Instruction {

	void generate(CodeBatch& batch) override {
		compute_zero(encoding::RiscVRegister::t4);

		evaluate_zero(5);
		evaluate_zero(6, encoding::RiscVRegister::t5, encoding::RiscVRegister::t6);
	}

};

#endif //OXTRA_ADD_H
