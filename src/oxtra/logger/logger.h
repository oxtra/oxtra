#ifndef OXTRA_LOGGER_H
#define OXTRA_LOGGER_H

#include <cstdint>

#include <fmt/format.h>

namespace logger {
	namespace Level {
		enum Value : uint8_t {
			riscv = 0x01,
			x86 = 0x02,
			translated = 0x04,
			reroutes = 0x08,
			syscall = 0x10,
			return_value = 0x20
		};
	}

	uint8_t get_level();

	bool get_level(Level::Value l);

	void set_level(uint8_t set);

	void set_level(Level::Value l, bool state);

	template<class... Args>
	void log(Level::Value level, const char* str, const Args&... args) {
		if ((get_level() & level) == 0)
			return;

		fmt::print(str, args...);
	}

	// Used in assembly code.
	void reroutes(const char* str, uintptr_t address);
}

#endif //OXTRA_LOGGER_H