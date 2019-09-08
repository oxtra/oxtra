# global important offsets
.equ guest_offset, 0
.equ guest_t0_offset, 0x20
.equ guest_t1_offset, 0x28
.equ guest_t2_offset, 0x30
.equ guest_a0_offset, 0x48
.equ guest_rdi_offset, 0x70
.equ guest_rsi_offset, 0x68
.equ guest_rdx_offet, 0x60
.equ guest_r8_offset, 0x78
.equ guest_r9_offset, 0x80
.equ guest_r10_offset, 0x88
.equ host_offset, 0x0F8
.equ codegen_offset, 0x1F0
.equ debugger_offset, 0x1F8
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
.equ debug_halt, 0x00
.equ debug_bp_count, 0x01
.equ debug_bp_array, 0x08

# global important strings
.section .rodata
reroute_static_fmt: .string "reroute_static: {0:#x}"
reroute_dynamic_fmt: .string "reroute_dynamic: {0:#x}"
unsupported_overflow_string: .string "the overflow flag of an instruction which doesn't update it it yet, has been used: "
unsupported_carry_string: .string "the carry flag of an instruction which doesn't update it it yet, has been used: "

# global macro's to capture parts of the context or even the whole context
.macro capture_context reg
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
	sd s2, 0x88(\reg)
	sd s3, 0x90(\reg)
	sd s4, 0x98(\reg)
	sd s5, 0xA0(\reg)
	sd s6, 0xA8(\reg)
	sd s7, 0xB0(\reg)
	sd s8, 0xB8(\reg)
	sd s9, 0xC0(\reg)
	sd s10, 0xC8(\reg)
	sd s11, 0xD0(\reg)
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
