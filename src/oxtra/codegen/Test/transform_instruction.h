#ifndef OXTRA_TRANSFORM_INSTRUCTION_H
#define OXTRA_TRANSFORM_INSTRUCTION_H

namespace codegen {
	/**
	 * Transforms a fadec instruction object into our own representation.
	 * @param inst The fadec instruction object.
	 * @return A pointer to our instruction object.
	 */
	std::unique_ptr<Instruction> transform_instruction(const fadec::Instruction& inst);
}

#endif //OXTRA_TRANSFORM_INSTRUCTION_H
