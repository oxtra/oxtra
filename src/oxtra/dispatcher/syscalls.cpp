#include "syscalls.h"
#include "dispatcher.h"
#include "execution_context.h"

#include "oxtra/logger/logger.h"

#include <cstring>

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

	logger::syscall("open({}, 0x{:x}, {}) syscall handled (returned: {}).\n", file, flag, mode, fd);
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

void dispatcher::syscalls::brk(dispatcher::ExecutionContext* context) {
	const auto address = context->guest.map.rdi;

	logger::syscall("brk(0x{:x}) (current break: 0x{:x})\n", address, context->program_break);

	// we neither allocate nor free
	if (address < context->initial_break) {
		// just return the current break
		// glib and musl call brk(0) to get the current break
		context->guest.map.rax = context->program_break;
		return;
	}

	// if the address is smaller than the current break then we free memory
	if (address <= context->program_break) {
		// if there is at least a page to free then free it
		if (const auto addr_page = utils::page_align(address); addr_page < context->last_break_page) {
			munmap(reinterpret_cast<void*>(addr_page), context->last_break_page - addr_page);
			context->last_break_page = addr_page;
		}

		context->program_break = address;
		context->guest.map.rax = context->program_break;
		return;
	}

	// the page aligned size that we have to allocate
	const auto alloc_size = utils::page_align(address - context->last_break_page);

	const auto mem = reinterpret_cast<uintptr_t>(mmap(reinterpret_cast<void*>(context->last_break_page), alloc_size,
			PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));

	// failed to allocate the memory
	if (reinterpret_cast<void*>(mem) == MAP_FAILED) {
		logger::syscall("failed to allocate memory for brk (size: 0x{:x})\n", alloc_size);
		context->guest.map.rax = context->program_break;
		return;
	}

	// allocated the memory but at a wrong address
	else if (mem != context->last_break_page) {
		munmap(reinterpret_cast<void*>(mem), alloc_size);
		logger::syscall("failed to allocate memory for brk (size: 0x{:x})\n", alloc_size);
		context->guest.map.rax = context->program_break;
		return;
	}

	logger::syscall("allocated 0x{:x} bytes for brk. new program break: 0x{:x}.\n", alloc_size, address);

	context->last_break_page += alloc_size;
	context->program_break = address;
	context->guest.map.rax = context->program_break;
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
			logger::syscall("gs base set to {0:x}\n", context->gs_base);
			break;

		case arch_set_fs:
			context->fs_base = context->guest.map.rsi;

			// fs:[0] = fs_base
			*reinterpret_cast<uint64_t*>(context->fs_base) = context->fs_base;
			logger::syscall("fs base set to {0:x}\n", context->fs_base);
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
	logger::syscall("ignored sigaction syscall.\n");
	context->guest.map.rax = 0;
}

void dispatcher::syscalls::sigprocmask(dispatcher::ExecutionContext* context) {
	logger::syscall("ignored sigprocmask syscall.\n");
	context->guest.map.rax = 0;
}

void dispatcher::syscalls::ioctl(dispatcher::ExecutionContext* context) {
	logger::syscall("ignored ioctl syscall.\n");
	context->guest.map.rax = 0;
}