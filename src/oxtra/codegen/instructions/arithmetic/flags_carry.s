.global carry_clear
.global carry_set

.global carry_add_8
.global carry_add_16
.global carry_add_32
.global carry_add_64

.global carry_adc_8
.global carry_adc_16
.global carry_adc_32
.global carry_adc_64

.section .text

# execution context defintions
.equ value0, 0x218
.equ value1, 0x220
.equ operation, 0x22A

# returns the value of the carry flag in t4

# sb t4, operation(s11) # cache carry flag state

# clear carry flag
carry_clear:
	li t4, 0
	ret

# set carry flag
carry_set:
	li t4, 1
	ret

# handle add, adc with carry = 0
carry_add_8:
	lbu t4, value0(s11) # src
	lbu t5, value1(s11) # dst
	slt t4, t5, t4 # result = (dst < src)
	ret
carry_add_16:
	lhu t4, value0(s11) # src
	lhu t5, value1(s11) # dst
	slt t4, t5, t4 # result = (dst < src)
	ret
carry_add_32:
	lwu t4, value0(s11) # src
	lwu t5, value1(s11) # dst
	slt t4, t5, t4 # result = (dst < src)
	ret
carry_add_64:
	ld t4, value0(s11) # src
	ld t5, value1(s11) # dst
	slt t4, t5, t4 # result = (dst < src)
	ret

# handle adc with carry flag set
carry_adc_8:
	lbu t4, value0(s11) # src
	lbu t5, value1(s11) # dst
	slt t4, t4, t5 # (dst <= src) == !(dst > src) == !(src < dst)
	xori t4, t4, 1 # carry = !t4
	ret
carry_adc_16:
	lhu t4, value0(s11) # src
	lhu t5, value1(s11) # dst
	slt t4, t4, t5 # (dst <= src) == !(dst > src) == !(src < dst)
	xori t4, t4, 1 # carry = !t4
	ret
carry_adc_32:
	lwu t4, value0(s11) # src
	lwu t5, value1(s11) # dst
	slt t4, t4, t5 # (dst <= src) == !(dst > src) == !(src < dst)
	xori t4, t4, 1 # carry = !t4
	ret
carry_adc_64:
	ld t4, value0(s11) # src
	ld t5, value1(s11) # dst
	slt t4, t4, t5 # (dst <= src) == !(dst > src) == !(src < dst)
	xori t4, t4, 1 # carry = !t4
	ret