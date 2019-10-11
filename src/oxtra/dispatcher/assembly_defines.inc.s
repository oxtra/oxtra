# global important offsets
.equ guest_offset, 0
.equ guest_t0_offset, 0x20
.equ guest_t1_offset, 0x28
.equ guest_t2_offset, 0x30
.equ guest_a0_offset, 0x48
.equ guest_rax_offset, 0x48
.equ guest_rbx_offset, 0x50
.equ guest_rcx_offset, 0x58
.equ guest_rdx_offset, 0x60
.equ guest_rsi_offset, 0x68
.equ guest_rdi_offset, 0x70
.equ guest_r8_offset, 0x78
.equ guest_r9_offset, 0x80
.equ guest_r10_offset, 0x88
.equ guest_s2_offset, 0x88
.equ guest_s4_offset, 0x98
.equ host_offset, 0x0F8
.equ codegen_offset, 0x1F0
.equ debugger_offset, 0x1F8
.equ syscall_map_offset, 0x270
.equ syscall_map_size, 322
.equ syscall_map_threshold, 0x1000
.equ flag_info_offset, 0x200
.equ flag_info_zero_value, 0x200
.equ flag_info_sign_value, 0x208
.equ flag_info_overflow_value0, 0x210
.equ flag_info_overflow_value1, 0x218
.equ flag_info_carry_value0, 0x220
.equ flag_info_carry_value1, 0x228
.equ flag_info_overflow_pointer, 0x230
.equ flag_info_carry_pointer, 0x238
.equ flag_info_overflow_operation, 0x240
.equ flag_info_carry_operation, 0x242
.equ flag_info_sign_size, 0x244
.equ flag_info_parity_value, 0x245
.equ debug_bp_count, 0x00
.equ debug_halt, 0x02
.equ debug_step_riscv, 0x03
.equ debug_bp_array, 0x08
.equ red_zone, -128
.equ debug_sig_address, 0x08
.equ debug_count_riscv, 0x10
.equ debug_count_x86, 0x18
.equ debug_sig_registers, 0x20
.equ debug_sig_t0, 0x40
.equ debug_sig_t1, 0x48
.equ debug_sig_t2, 0x50
.equ debug_bp_array, 0x118

# global important strings
.section .rodata
reroute_static_fmt: .string "reroute_static: {0:#x}\n"
reroute_dynamic_fmt: .string "reroute_dynamic: {0:#x}\n"
reroute_return_fmt: .string "reroute_return: {0:#x}\n"
unsupported_overflow_string: .string "the overflow flag of an instruction which doesn't update it it yet, has been used: "
unsupported_carry_string: .string "the carry flag of an instruction which doesn't update it it yet, has been used: "

# global macro's to capture parts of the context or even the whole context
.macro capture_context_tlb reg
	sd ra, 0x00(\reg)
	sd sp, 0x08(\reg)
	sd gp, 0x10(\reg)
	sd tp, 0x18(\reg)
	sd fp, 0x38(\reg)
	sd s1, 0x40(\reg)
	sd a0, 0x48(\reg)
	sd a1, 0x50(\reg)
	sd a2, 0x58(\reg)
	sd a3, 0x60(\reg)
	sd a4, 0x68(\reg)
	sd a5, 0x70(\reg)
	sd a6, 0x78(\reg)
	sd a7, 0x80(\reg)
	sd s3, 0x90(\reg)
	sd s5, 0xA0(\reg)
	sd s6, 0xA8(\reg)
	sd s7, 0xB0(\reg)
	sd s8, 0xB8(\reg)
	sd s9, 0xC0(\reg)
	sd s10, 0xC8(\reg)
	sd s11, 0xD0(\reg)
.endm
.macro capture_context reg
	capture_context_tlb \reg
	sd s2, 0x88(\reg)
	sd s4, 0x98(\reg)
.endm
.macro capture_context_high_level reg
	capture_context \reg
	sd t0, 0x20(\reg)
	sd t1, 0x28(\reg)
	sd t2, 0x30(\reg)
	sd t3, 0xD8(\reg)
	sd t5, 0xE8(\reg)
	sd t6, 0xF0(\reg)
.endm
.macro capture_context_debug reg
	capture_context \reg
	sd t3, 0xD8(\reg)
	sd t4, 0xE0(\reg)
	sd t5, 0xE8(\reg)
	sd t6, 0xF0(\reg)
.endm

# global macro's to restore parts of the context or even the whole context
.macro restore_context reg
	ld ra, 0x00(\reg)
	ld sp, 0x08(\reg)
	ld gp, 0x10(\reg)
	ld tp, 0x18(\reg)
	ld fp, 0x38(\reg)
	ld s1, 0x40(\reg)
	ld a0, 0x48(\reg)
	ld a1, 0x50(\reg)
	ld a2, 0x58(\reg)
	ld a3, 0x60(\reg)
	ld a4, 0x68(\reg)
	ld a5, 0x70(\reg)
	ld a6, 0x78(\reg)
	ld a7, 0x80(\reg)
	ld s2, 0x88(\reg)
	ld s3, 0x90(\reg)
	ld s4, 0x98(\reg)
	ld s5, 0xA0(\reg)
	ld s6, 0xA8(\reg)
	ld s7, 0xB0(\reg)
	ld s8, 0xB8(\reg)
	ld s9, 0xC0(\reg)
	ld s10, 0xC8(\reg)
	ld s11, 0xD0(\reg)
.endm
.macro restore_context_high_level reg
	ld t0, 0x20(\reg)
	ld t1, 0x28(\reg)
	ld t2, 0x30(\reg)
	ld t3, 0xD8(\reg)
	ld t5, 0xE8(\reg)
	ld t6, 0xF0(\reg)
	restore_context \reg
.endm
.macro restore_context_debug reg
	ld t0, 0x20(\reg)
	ld t1, 0x28(\reg)
	ld t2, 0x30(\reg)
	ld t3, 0xD8(\reg)
	ld t4, 0xE0(\reg)
    ld t5, 0xE8(\reg)
	ld t6, 0xF0(\reg)
	restore_context \reg
.endm

# global macro to compute the address of the tlb-entry (s9 = tlb, t0,s2,s4 = usable, t3 = x86-address)
# if in tlb: jump to target, s2 = address
# else: continue with execution, s4 = tlb-entry-address
.macro compute_tlb_address target
	# hash the address (stored in t3) (hash-width: 7bits)
	srli t0, t3, 11
	xor t0, t0, t3
	andi t0, t0, 0x7f0  # hash: (addr[17:11] ^ addr[10:4]) & 0b0111'1111'0000

	# as the four lower bits of the hash are zero, it can also be used as the offset into the tlb
	add s4, t0, s9

	# load the x86-address from the tlb and compare it to the target-address
	ld t0, 0(s4)
	bne t0, t3, not_\target

	# store the riscv-address into s2
	ld s2, 8(s4)
	j \target
	not_\target:
.endm

# global macro to update the tlb (s2 = x86-address, a0 = riscv-address, s4 = tlb-entry-address)
.macro update_tlb_entry
	# write the new addresses into the tlb
	sd s2, 0(s4)
    sd a0, 8(s4)
.endm
