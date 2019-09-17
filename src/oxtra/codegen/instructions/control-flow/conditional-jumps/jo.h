#ifndef OXTRA_JO_H
#define OXTRA_JO_H

namespace codegen {
	class Jo : public Jcc {
	public:
		explicit Jo(const fadec::Instruction& inst)
				: Jcc{inst, flags::overflow} {}

		void generate(CodeBatch& batch) const final;
	};
}

#endif //OXTRA_JO_H
