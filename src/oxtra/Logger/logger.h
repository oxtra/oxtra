#ifndef OXTRA_LOGGER_H
#define OXTRA_LOGGER_H

#include <cstdint>

namespace logger {
	namespace Level {
		enum : uint8_t {
			riscv = 0x01,
			x86 = 0x02,
			translated = 0x04,
			reroutes = 0x08,
			syscall = 0x10,
			return_value = 0x20
		};
	}

	uint8_t get_level();

	bool get_riscv();

	bool get_x86();

	bool get_translated();

	bool get_reroutes();

	bool get_syscall();

	bool get_return_value();

	void set_level(uint8_t set);

	void set_riscv(bool set);

	void set_x86(bool set);

	void set_translated(bool set);

	void set_reroutes(bool set);

	void set_syscall(bool set);

	void set_return_value(bool set);

	void riscv(const char* str, ...);

	void x86(const char* str, ...);

	void translated(const char* str, ...);

	void reroutes(const char* str, ...);

	void syscall(const char* str, ...);

	void return_value(const char* str, ...);
}

#endif //OXTRA_LOGGER_H