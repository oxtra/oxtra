#include "syscalls.h"
#include "dispatcher.h"

#include <spdlog/spdlog.h>

#include <fcntl.h>
#include <sys/stat.h>
#undef st_atime
#undef st_mtime
#undef st_ctime

void dispatcher::syscalls::open(dispatcher::ExecutionContext* context) {
	const auto file = reinterpret_cast<const char*>(context->guest.map.rdi);
	const auto flag = static_cast<int>(context->guest.map.rsi);
	const auto mode = static_cast<mode_t>(context->guest.map.rdx);

	const auto fd = openat(AT_FDCWD, file, flag, mode);

	spdlog::info("open({}, 0x{:x}, {}) syscall handled (returned: {}).", file, flag, mode, fd);
	context->guest.map.rax = fd;
}

void dispatcher::syscalls::fstat(dispatcher::ExecutionContext* context) {
	struct stat s;
	const auto fd = static_cast<int>(context->guest.map.rdi);

	context->guest.map.rax = static_cast<uintptr_t>(fstat(fd, &s));

	// thank u linux. very cool
	struct ia64_stat {
		unsigned long	st_dev;
		unsigned long	st_ino;
		unsigned long	st_nlink;
		unsigned int	st_mode;
		unsigned int	st_uid;
		unsigned int	st_gid;
		unsigned int	__pad0;
		unsigned long	st_rdev;
		unsigned long	st_size;
		unsigned long	st_atime;
		unsigned long	st_atime_nsec;
		unsigned long	st_mtime;
		unsigned long	st_mtime_nsec;
		unsigned long	st_ctime;
		unsigned long	st_ctime_nsec;
		unsigned long	st_blksize;
		long		st_blocks;
		unsigned long	__unused[3];
	};

	const auto host_stat = reinterpret_cast<ia64_stat*>(context->guest.map.rsi);
	host_stat->st_dev = s.st_dev;
	host_stat->st_ino = s.st_ino;
	host_stat->st_nlink = s.st_nlink;
	host_stat->st_mode = s.st_mode;
	host_stat->st_uid = s.st_uid;
	host_stat->st_gid = s.st_gid;
	host_stat->__pad0 = 0;
	host_stat->st_rdev = s.st_rdev;
	host_stat->st_size = s.st_size;
	host_stat->st_atime = s.st_atim.tv_sec;
	host_stat->st_atime_nsec = s.st_atim.tv_nsec;
	host_stat->st_mtime = s.st_mtim.tv_sec;
	host_stat->st_mtime_nsec = s.st_mtim.tv_nsec;
	host_stat->st_ctime = s.st_ctim.tv_sec;
	host_stat->st_ctime_nsec = s.st_ctim.tv_nsec;
	host_stat->st_blksize = s.st_blksize;
	host_stat->st_blocks = s.st_blocks;
	std::memset(host_stat->__unused, 0, sizeof(host_stat->__unused));
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
			context->guest.map.rax = static_cast<uintptr_t>(-1);
			return;
	}

	context->guest.map.rax = 0;
}

void dispatcher::syscalls::sigaction(dispatcher::ExecutionContext* context) {
	spdlog::info("ignored sigaction syscall.");
	context->guest.map.rax = 0;
}

void dispatcher::syscalls::sigprocmask(dispatcher::ExecutionContext* context) {
	spdlog::info("ignored sigprocmask syscall.");
	context->guest.map.rax = 0;
}

void dispatcher::syscalls::ioctl(dispatcher::ExecutionContext* context) {
	spdlog::info("ignored ioctl syscall.");
	context->guest.map.rax = 0;
}