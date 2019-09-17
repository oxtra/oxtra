#ifndef OXTRA_JNO_H
#define OXTRA_JNO_H

namespace codegen {
	class Jno : public Jcc {
	public:
		explicit Jno(const fadec::Instruction& inst)
				: Jcc{inst, flags::overflow} {}

		void generate(CodeBatch& batch) const final;
	};
}

#endif //OXTRA_JNO_H
