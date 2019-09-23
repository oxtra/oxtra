#ifndef OXTRA_SYSCALLS_H
#define OXTRA_SYSCALLS_H

#define __ARCH_WANT_SYSCALL_NO_AT
#include <sys/syscall.h>
#undef __ARCH_WANT_SYSCALL_NO_AT

#include "execution_context.h"

namespace dispatcher::syscalls {
	/**
	 * Describes an entry in the syscall map.
	 * Either contains the index of the syscall for the riscv kernel or a function that emulates the syscall.
	 */
	union syscall_entry {
		using EmulationFn = void(*)(ExecutionContext*);

		/**
		 * Constructs a syscall entry from a riscv syscall index or an empty syscall entry if the index is -1.
		 */
		constexpr syscall_entry(const int index)
			: riscv_index{static_cast<uintptr_t>(index)} {}

		/**
		 * Constructs a syscall entry from a emulation function.
		 */
		constexpr syscall_entry(EmulationFn emulation_fn)
			: emulation_fn{emulation_fn} {}

		/**
		 * Returns true if the syscall_entry is valid.
		 */
		bool is_valid() const {
			return riscv_index != std::numeric_limits<uintptr_t>::max();
		}

		/**
		 * Returns true if the syscall should be emulated.
		 */
		bool is_emulated() const {
			return riscv_index >= 0x1000;
		}

		/**
		 * Returns true if the syscall should be forwarded.
		 */
		bool is_forwarded() const {
			return riscv_index < 0x1000;
		}

		uintptr_t riscv_index;
		EmulationFn emulation_fn;
	};

	/*
	 * Emulation functions.
	 */
	void exit(ExecutionContext* context);
	void arch_prctl(ExecutionContext* context);
	void sigaction(ExecutionContext* context);
	void sigprocmask(ExecutionContext* context);

	/*
	 * This map contains information about how syscalls should be handled.
	 * (Either remapped and forwarded or emulated).
	 */
	static constexpr std::array<syscall_entry, 322> syscall_map = {
		SYS_read,
		SYS_write,
		-1, //__NR_open,
		SYS_close,
		-1, //__NR_stat,
		SYS_fstat,
		-1, //__NR_lstat,
		-1, // SYS_poll,
		SYS_lseek,
		SYS_mmap,
		SYS_mprotect,
		SYS_munmap,
		SYS_brk,
		sigaction, //SYS_rt_sigaction,
		sigprocmask, //SYS_rt_sigprocmask,
		-1, //SYS_rt_sigreturn,
		SYS_ioctl,
		SYS_pread64,
		SYS_pwrite64,
		SYS_readv,
		SYS_writev,
		-1, //SYS_access,
		-1, //SYS_pipe,
		-1, //SYS_select,
		SYS_sched_yield,
		SYS_mremap,
		SYS_msync,
		SYS_mincore,
		SYS_madvise,
		SYS_shmget,
		SYS_shmat,
		SYS_shmctl,
		SYS_dup,
		-1, //SYS_dup2,
		-1, //SYS_pause,
		SYS_nanosleep,
		SYS_getitimer,
		-1, //SYS_alarm,
		SYS_setitimer,
		SYS_getpid,
		SYS_sendfile,
		SYS_socket,
		SYS_connect,
		SYS_accept,
		SYS_sendto,
		SYS_recvfrom,
		SYS_sendmsg,
		SYS_recvmsg,
		SYS_shutdown,
		SYS_bind,
		SYS_listen,
		SYS_getsockname,
		SYS_getpeername,
		SYS_socketpair,
		SYS_setsockopt,
		SYS_getsockopt,
		SYS_clone,
		-1, //SYS_fork,
		-1, //SYS_vfork,
		SYS_execve,
		exit,
		SYS_wait4,
		SYS_kill,
		SYS_uname,
		SYS_semget,
		SYS_semop,
		SYS_semctl,
		SYS_shmdt,
		SYS_msgget,
		SYS_msgsnd,
		SYS_msgrcv,
		SYS_msgctl,
		SYS_fcntl,
		SYS_flock,
		SYS_fsync,
		SYS_fdatasync,
		SYS_truncate,
		SYS_ftruncate,
		-1, //SYS_getdents,
		SYS_getcwd,
		SYS_chdir,
		SYS_fchdir,
		-1, //__NR_rename,
		-1, //__NR_mkdir,
		-1, //__NR_rmdir,
		-1, //SYS_creat,
		-1, //__NR_link,
		-1, //__NR_unlink,
		-1, //__NR_symlink,
		-1, //__NR_readlink,
		-1, //__NR_chmod,
		SYS_fchmod,
		-1, //SYS_chown,
		SYS_fchown,
		-1, //SYS_lchown,
		SYS_umask,
		SYS_gettimeofday,
		SYS_getrlimit,
		SYS_getrusage,
		SYS_sysinfo,
		SYS_times,
		SYS_ptrace,
		SYS_getuid,
		SYS_syslog,
		SYS_getgid,
		SYS_setuid,
		SYS_setgid,
		SYS_geteuid,
		SYS_getegid,
		SYS_setpgid,
		SYS_getppid,
		-1, //SYS_getpgrp,
		SYS_setsid,
		SYS_setreuid,
		SYS_setregid,
		SYS_getgroups,
		SYS_setgroups,
		SYS_setresuid,
		SYS_getresuid,
		SYS_setresgid,
		SYS_getresgid,
		SYS_getpgid,
		SYS_setfsuid,
		SYS_setfsgid,
		SYS_getsid,
		SYS_capget,
		SYS_capset,
		SYS_rt_sigpending,
		SYS_rt_sigtimedwait,
		SYS_rt_sigqueueinfo,
		SYS_rt_sigsuspend,
		SYS_sigaltstack,
		-1, //SYS_utime,
		-1, //SYS_mknod,
		-1, //SYS_uselib,
		SYS_personality,
		-1, //SYS_ustat,
		SYS_statfs,
		SYS_fstatfs,
		-1, //SYS_sysfs,
		SYS_getpriority,
		SYS_setpriority,
		SYS_sched_setparam,
		SYS_sched_getparam,
		SYS_sched_setscheduler,
		SYS_sched_getscheduler,
		SYS_sched_get_priority_max,
		SYS_sched_get_priority_min,
		SYS_sched_rr_get_interval,
		SYS_mlock,
		SYS_munlock,
		SYS_mlockall,
		SYS_munlockall,
		SYS_vhangup,
		-1, //SYS_modify_ldt,
		SYS_pivot_root,
		-1, //SYS__sysctl,
		SYS_prctl,
		arch_prctl, //SYS_arch_prctl,
		SYS_adjtimex,
		SYS_setrlimit,
		SYS_chroot,
		SYS_sync,
		SYS_acct,
		SYS_settimeofday,
		SYS_mount,
		SYS_umount2,
		SYS_swapon,
		SYS_swapoff,
		SYS_reboot,
		SYS_sethostname,
		SYS_setdomainname,
		-1, //SYS_iopl,
		-1, //SYS_ioperm,
		-1, //SYS_create_module,
		SYS_init_module,
		SYS_delete_module,
		-1, //SYS_get_kernel_syms,
		-1, //SYS_query_module,
		SYS_quotactl,
		SYS_nfsservctl,
		-1, //SYS_getpmsg,
		-1, //SYS_putpmsg,
		-1, //SYS_afs_syscall,
		-1, //SYS_tuxcall,
		-1, //SYS_security,
		SYS_gettid,
		SYS_readahead,
		SYS_setxattr,
		SYS_lsetxattr,
		SYS_fsetxattr,
		SYS_getxattr,
		SYS_lgetxattr,
		SYS_fgetxattr,
		SYS_listxattr,
		SYS_llistxattr,
		SYS_flistxattr,
		SYS_removexattr,
		SYS_lremovexattr,
		SYS_fremovexattr,
		SYS_tkill,
		-1, //SYS_time,
		SYS_futex,
		SYS_sched_setaffinity,
		SYS_sched_getaffinity,
		-1, //SYS_set_thread_area,
		SYS_io_setup,
		SYS_io_destroy,
		SYS_io_getevents,
		SYS_io_submit,
		SYS_io_cancel,
		-1, //SYS_get_thread_area,
		SYS_lookup_dcookie,
		-1, //SYS_epoll_create,
		-1, //SYS_epoll_ctl_old,
		-1, //SYS_epoll_wait_old,
		SYS_remap_file_pages,
		SYS_getdents64,
		SYS_set_tid_address,
		SYS_restart_syscall,
		SYS_semtimedop,
		SYS_fadvise64,
		SYS_timer_create,
		SYS_timer_settime,
		SYS_timer_gettime,
		SYS_timer_getoverrun,
		SYS_timer_delete,
		SYS_clock_settime,
		SYS_clock_gettime,
		SYS_clock_getres,
		SYS_clock_nanosleep,
		SYS_exit_group,
		-1, //SYS_epoll_wait,
		SYS_epoll_ctl,
		SYS_tgkill,
		-1, //SYS_utimes,
		-1, //SYS_vserver,
		SYS_mbind,
		SYS_set_mempolicy,
		SYS_get_mempolicy,
		SYS_mq_open,
		SYS_mq_unlink,
		SYS_mq_timedsend,
		SYS_mq_timedreceive,
		SYS_mq_notify,
		SYS_mq_getsetattr,
		SYS_kexec_load,
		SYS_waitid,
		SYS_add_key,
		SYS_request_key,
		SYS_keyctl,
		SYS_ioprio_set,
		SYS_ioprio_get,
		-1, //SYS_inotify_init,
		SYS_inotify_add_watch,
		SYS_inotify_rm_watch,
		SYS_migrate_pages,
		SYS_openat,
		SYS_mkdirat,
		SYS_mknodat,
		SYS_fchownat,
		-1, //SYS_futimesat,
		SYS_newfstatat,
		SYS_unlinkat,
		-1, //SYS_renameat,
		SYS_linkat,
		SYS_symlinkat,
		SYS_readlinkat,
		SYS_fchmodat,
		SYS_faccessat,
		SYS_pselect6,
		SYS_ppoll,
		SYS_unshare,
		SYS_set_robust_list,
		SYS_get_robust_list,
		SYS_splice,
		SYS_tee,
		SYS_sync_file_range,
		SYS_vmsplice,
		SYS_move_pages,
		SYS_utimensat,
		SYS_epoll_pwait,
		-1, //SYS_signalfd,
		SYS_timerfd_create,
		-1, //SYS_eventfd,
		SYS_fallocate,
		SYS_timerfd_settime,
		SYS_timerfd_gettime,
		SYS_accept4,
		SYS_signalfd4,
		SYS_eventfd2,
		SYS_epoll_create1,
		SYS_dup3,
		SYS_pipe2,
		SYS_inotify_init1,
		SYS_preadv,
		SYS_pwritev,
		SYS_rt_tgsigqueueinfo,
		SYS_perf_event_open,
		SYS_recvmmsg,
		SYS_fanotify_init,
		SYS_fanotify_mark,
		SYS_prlimit64,
		SYS_name_to_handle_at,
		SYS_open_by_handle_at,
		SYS_clock_adjtime,
		SYS_syncfs,
		SYS_sendmmsg,
		SYS_setns,
		SYS_getcpu,
		SYS_process_vm_readv,
		SYS_process_vm_writev,
		SYS_kcmp,
		SYS_finit_module,
		SYS_sched_setattr,
		SYS_sched_getattr,
		SYS_renameat2,
		SYS_seccomp,
		SYS_getrandom,
		SYS_memfd_create,
		-1, //SYS_kexec_file_load,
		SYS_bpf
	};
}

#endif //OXTRA_SYSCALLS_H
