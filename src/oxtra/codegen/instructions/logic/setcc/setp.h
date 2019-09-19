#ifndef OXTRA_SETP_H
#define OXTRA_SETP_H

namespace codegen {
	class Setp : public Setcc {
	public:
		explicit Setp(const fadec::Instruction& inst)
				: Setcc{inst, flags::parity} {}

		void generate(CodeBatch& batch) const final;
	};
}

#endif //OXTRA_SETP_H
