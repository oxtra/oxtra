#ifndef OXTRA_JS_H
#define OXTRA_JS_H

namespace codegen {
	class Js : public Jcc {
	public:
		explicit Js(const fadec::Instruction& inst)
				: Jcc{inst, flags::sign} {}

		void generate(CodeBatch& batch) const final;
	};
}

#endif //OXTRA_JS_H
