#ifndef OXTRA_BTR_H
#define OXTRA_BTR_H

namespace codegen {
	class Btr : public BitTest {
	public:
		explicit Btr(const fadec::Instruction& inst)
				: BitTest{inst} {}

	private:
		void manipulate_bit(CodeBatch& batch, encoding::RiscVRegister bit_base) const final;
	};
}

#endif //OXTRA_BTR_H
