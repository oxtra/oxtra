.global overflow_clear
.global overflow_set

.global overflow_add_8
.global overflow_add_16
.global overflow_add_32
.global overflow_add_64

.global overflow_adc_8
.global overflow_adc_16
.global overflow_adc_32
.global overflow_adc_64

.global overflow_sub_8
.global overflow_sub_16
.global overflow_sub_32
.global overflow_sub_64

.section .text

# execution context defintions
.equ value0, 0x208
.equ value1, 0x210
.equ operation, 0x228

# returns the value of the overflow flag in t4

overflow_clear:
	li t4, 0
	ret

overflow_set:
	li t4, 1
	ret

# handle add, adc with carry = 0
overflow_add_8:
	lb t4, value0(s11) # src1
    lb t5, value1(s11) # src2
	slli t4, t4, 24
	slli t5, t5, 24
	addw t6, t4, t5 # dstw = src1 + src2 (32 bit)
	add t5, t4, t5 # dst = src1 + src2
	sub t5, t5, t6 # of = (dstw != dst)
	snez t4, t5
	ret
overflow_add_16:
	lh t4, value0(s11) # src1
	lh t5, value1(s11) # src2
	slli t4, t4, 16
	slli t5, t5, 16
	addw t6, t4, t5 # dstw = src1 + src2 (32 bit)
	add t5, t4, t5 # dst = src1 + src2
	sub t5, t5, t6 # of = (dstw != dst)
	snez t4, t5
	ret
overflow_add_32:
	lw t4, value0(s11) # src1
    lw t5, value1(s11) # src2
    addw t6, t4, t5 # dstw = src1 + src2 (32 bit)
    add t5, t4, t5 # dst = src1 + src2
    sub t5, t5, t6 # of = (dstw != dst)
    snez t4, t5
    ret
overflow_add_64:
	ld t4, value0(s11) # src1
	ld t5, value1(s11) # src2
	add t6, t5, t4 # dst = src1 + src2
	slti t4, t4, 0
	slt t5, t6, t5
	sub t4, t4, t5
	snez t4, t4
	ret

# handle adc operation with carry flag set
overflow_adc_8:
	lb t4, value0(s11) # src1
    lb t5, value1(s11) # src2
    addi t5, t5, 1 # add the carry bit
	slli t4, t4, 24
	slli t5, t5, 24
	addw t6, t4, t5 # dstw = src1 + src2 (32 bit)
	add t5, t4, t5 # dst = src1 + src2
	sub t5, t5, t6 # of = (dstw != dst)
	snez t4, t5
	ret
overflow_adc_16:
	lh t4, value0(s11) # src1
	lh t5, value1(s11) # src2
	addi t5, t5, 1 # add the carry bit
	slli t4, t4, 16
	slli t5, t5, 16
	addw t6, t4, t5 # dstw = src1 + src2 (32 bit)
	add t5, t4, t5 # dst = src1 + src2
	sub t5, t5, t6 # of = (dstw != dst)
	snez t4, t5
	ret
overflow_adc_32:
	lw t4, value0(s11) # src1
    lw t5, value1(s11) # src2
    addi t5, t5, 1 # add the carry bit
    addw t6, t4, t5 # dstw = src1 + src2 (32 bit)
    add t5, t4, t5 # dst = src1 + src2
    sub t5, t5, t6 # of = (dstw != dst)
    snez t4, t5
    ret
overflow_adc_64: # sgn(s1) != sgn(s2) -> of = 0 else of = sgn(s1+s2+1) != sgn(s1)
	ld t4, value0(s11) # src1
	ld t5, value1(s11) # src2
	add t6, t5, t4 # dst = src1 + src2
	addi t6, t6, 1 # dst += carry
	srli t4, t4, 63
	srli t5, t5, 63
	srli t6, t6, 63
	bne t4, t5, overflow_clear
	xor t4, t6, t4
	ret

# handle sub
overflow_sub_8:
	lbu t4, value0(s11) # src
    lbu t5, value1(s11) # dst
    sgtu t4, t5, t4 # carry = (dst > src)
    srli t5, t5, 7 # sign bit of dst
    xor t4, t4, t5 # of = (cf ^ sign)
    ret
overflow_sub_16:
	lhu t4, value0(s11) # src
    lhu t5, value1(s11) # dst
    sgtu t4, t5, t4 # carry = (dst > src)
    srli t5, t5, 7 # sign bit of dst
    xor t4, t4, t5 # of = (cf ^ sign)
    ret
overflow_sub_32:
	lwu t4, value0(s11) # src
    lwu t5, value1(s11) # dst
    sgtu t4, t5, t4 # carry = (dst > src)
    srli t5, t5, 7 # sign bit of dst
    xor t4, t4, t5 # of = (cf ^ sign)
    ret
overflow_sub_64:
	ld t4, value0(s11) # src
    ld t5, value1(s11) # dst
    sgtu t4, t5, t4 # carry = (dst > src)
    srli t5, t5, 7 # sign bit of dst
    xor t4, t4, t5 # of = (cf ^ sign)
    ret