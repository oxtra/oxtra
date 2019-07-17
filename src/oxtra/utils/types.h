#ifndef OXTRA_TYPES_H
#define OXTRA_TYPES_H

#include <cstdint>
#include <cstddef>

namespace utils {
	/**
	 * describes all addresses referenced by the guest-program.
	 */
	using guest_addr_t = uintptr_t;

	/**
	 * describes all addresses which result from translated guest-addresses and
	 * and are runnable on the host.
	 */
	using host_addr_t = uintptr_t;

	/**
	 * represents a risc-v instruction
	 */
	using riscv_instruction_t = uint32_t;
}

#define unused_parameter(x) ((void)x)

#endif //OXTRA_TYPES_H
