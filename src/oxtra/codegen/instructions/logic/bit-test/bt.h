#ifndef OXTRA_BT_H
#define OXTRA_BT_H

namespace codegen {
	class Bt : public codegen::Instruction {
	public:
		explicit Bt(const fadec::Instruction& inst)
			: codegen::Instruction{inst, flags::carry, flags::none} {}

		virtual void generate(CodeBatch& batch) const override;

		// bt doesn't modify the bit which allows for some optimization which is why we don't inherit from BitTest
	};
}

#endif //OXTRA_BT_H
