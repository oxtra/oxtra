#ifndef OXTRA_FLAGS_H
#define OXTRA_FLAGS_H

#include <cstdint>

namespace codegen {
	namespace flags {
		enum : uint8_t {
			none = 0x00,
			carry = 0x01,
			zero = 0x02,
			sign = 0x04,
			overflow = 0x08,
			parity = 0x10,
			all = carry | zero | sign | overflow | parity
		};

		struct Info {
			/*
			 * If the order and the size of these attributes is changed,
			 * the assembly_globals file has to be updated as well.
			 */

			uint64_t zero_value;		// 0x00
			uint64_t sign_value;		// 0x08
			uint64_t overflow_value[2];	// 0x10
			uint64_t carry_value[2];	// 0x20
			uint64_t overflow_pointer;	// 0x30
			uint64_t carry_pointer;		// 0x38
			uint16_t overflow_operation;// 0x40
			uint16_t carry_operation;	// 0x42
			uint8_t sign_size;			// 0x44
			uint8_t parity_value;		// 0x45

			static constexpr uint32_t
					flag_info_offset = 0x200,
					zero_value_offset = flag_info_offset + sizeof(uint64_t) * 0,
					sign_value_offset = flag_info_offset + sizeof(uint64_t) * 1,
					overflow_values_offset = flag_info_offset + sizeof(uint64_t) * 2,
					carry_values_offset = flag_info_offset + sizeof(uint64_t) * 4,
					overflow_ptr_offset = flag_info_offset + sizeof(uint64_t) * 6,
					carry_ptr_offset = flag_info_offset + sizeof(uint64_t) * 7,
					overflow_operation_offset = flag_info_offset + sizeof(uint64_t) * 8,
					carry_operation_offset = flag_info_offset + sizeof(uint64_t) * 8 + sizeof(uint16_t),
					sign_size_offset = flag_info_offset + sizeof(uint64_t) * 8 + sizeof(uint16_t) * 2,
					parity_value_offset = flag_info_offset + sizeof(uint64_t) * 8 + sizeof(uint16_t) * 2 + sizeof(uint8_t);
		};
	}
}

#endif //OXTRA_FLAGS_H
