#ifndef OXTRA_BTS_H
#define OXTRA_BTS_H

namespace codegen {
	class Bts : public BitTest {
	public:
		explicit Bts(const fadec::Instruction& inst)
				: BitTest{inst} {}

	private:
		void manipulate_bit(CodeBatch& batch, encoding::RiscVRegister bit_base) const final;
	};
}

#endif //OXTRA_BTS_H
