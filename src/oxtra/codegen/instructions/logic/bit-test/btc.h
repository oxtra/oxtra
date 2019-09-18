#ifndef OXTRA_BTC_H
#define OXTRA_BTC_H

namespace codegen {
	class Btc : public BitTest {
	public:
		explicit Btc(const fadec::Instruction& inst)
			: BitTest{inst} {}

	private:
		void manipulate_bit(CodeBatch& batch, encoding::RiscVRegister bit_base) const final;
	};
}

#endif //OXTRA_BTC_H
