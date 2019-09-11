#include "nop.h"

void codegen::Nop::generate(CodeBatch& batch) const {
	unused_parameter(batch);
}