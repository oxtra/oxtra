#ifndef OXTRA_JNS_H
#define OXTRA_JNS_H

namespace codegen {
	class Jns : public Jcc {
	public:
		explicit Jns(const fadec::Instruction& inst)
				: Jcc{inst, flags::sign} {}

		void generate(CodeBatch& batch) const final;
	};
}

#endif //OXTRA_JNS_H
