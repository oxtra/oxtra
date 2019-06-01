#ifndef OXTRA_TYPES_H
#define OXTRA_TYPES_H

#include <cstdint>
#include <cstddef>

namespace oxtra {
	/*
	 * describes all addresses referenced by the guest-program.
	 */
	using virt_t = uintptr_t;

	/*
	 * describes all addresses which result from translated guest-addresses and
	 * and are runnable on the host.
	 */
	using real_t = uintptr_t;
}

#endif //OXTRA_TYPES_H
