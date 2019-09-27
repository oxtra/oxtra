#ifndef OXTRA_NOP_H
#define OXTRA_NOP_H

namespace codegen {
	class Nop : public codegen::Instruction {
	public:
		explicit Nop(const fadec::Instruction& inst)
				: codegen::Instruction{inst, flags::none, flags::none} {}

		void generate(CodeBatch& batch) const override;
	};
}

#endif //OXTRA_NOP_H
