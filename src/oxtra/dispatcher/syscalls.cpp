#include "syscalls.h"
#include "dispatcher.h"

#include <spdlog/spdlog.h>

#include <fcntl.h>

void dispatcher::syscalls::open(dispatcher::ExecutionContext* context) {
	const auto fd = openat(AT_FDCWD, reinterpret_cast<const char*>(context->guest.map.rdi),
			static_cast<int>(context->guest.map.rsi), static_cast<mode_t>(context->guest.map.rdx));

	spdlog::info("open syscall handled (returned: {}.)", fd);
	context->guest.a0 = fd;
}

void dispatcher::syscalls::exit(dispatcher::ExecutionContext* context) {
	dispatcher::Dispatcher::guest_exit(context->guest.map.rdi);
}

void dispatcher::syscalls::arch_prctl(dispatcher::ExecutionContext* context) {
	static constexpr auto
			arch_set_gs = 0x1001,
			arch_set_fs = 0x1002,
			arch_get_fs = 0x1003,
			arch_get_gs = 0x1004;

	switch (context->guest.map.rdi) {
		case arch_set_gs:
			context->gs_base = context->guest.map.rsi;

			// gs:[0] = gs_base
			*reinterpret_cast<uint64_t*>(context->gs_base) = context->gs_base;
			spdlog::debug("gs base set to {0:x}", context->gs_base);
			break;

		case arch_set_fs:
			context->fs_base = context->guest.map.rsi;

			// fs:[0] = fs_base
			*reinterpret_cast<uint64_t*>(context->fs_base) = context->fs_base;
			spdlog::debug("fs base set to {0:x}", context->fs_base);
			break;

		case arch_get_fs:
			*reinterpret_cast<uint64_t*>(context->guest.map.rsi) = context->fs_base;
			break;

		case arch_get_gs:
			*reinterpret_cast<uint64_t*>(context->guest.map.rsi) = context->gs_base;
			break;

		default:
			context->guest.a0 = static_cast<uintptr_t>(-1);
			return;
	}

	context->guest.a0 = 0;
}

void dispatcher::syscalls::sigaction(dispatcher::ExecutionContext* context) {
	spdlog::info("ignored sigaction syscall.");
	context->guest.a0 = 0;
}

void dispatcher::syscalls::sigprocmask(dispatcher::ExecutionContext* context) {
	spdlog::info("ignored sigprocmask syscall.");
	context->guest.a0 = 0;
}

void dispatcher::syscalls::ioctl(dispatcher::ExecutionContext* context) {
	spdlog::info("ignored ioctl syscall.");
	context->guest.a0 = 0;
}