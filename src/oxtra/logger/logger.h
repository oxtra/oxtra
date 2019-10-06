#ifndef OXTRA_LOGGER_H
#define OXTRA_LOGGER_H

#include <cstdint>

#include <fmt/format.h>

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

	template<class... Args>
	void riscv(const char* str, const Args&... args) {
		if (!get_riscv())
			return;

		fmt::print(str, args...);
	}

	template<class... Args>
	void x86(const char* str, const Args&... args) {
		if (!get_x86())
			return;

		fmt::print(str, args...);
	}

	template<class... Args>
	void translated(const char* str, const Args&... args) {
		if (!get_translated())
			return;

		fmt::print(str, args...);
	}

	void reroutes(const char* str, uintptr_t address);

	template<class... Args>
	void syscall(const char* str, const Args&... args) {
		if (!get_syscall())
			return;

		fmt::print(str, args...);
	}

	template<class... Args>
	void return_value(const char* str, const Args&... args) {
		if (!get_return_value())
			return;

		fmt::print(str, args...);
	}
}

#endif //OXTRA_LOGGER_H