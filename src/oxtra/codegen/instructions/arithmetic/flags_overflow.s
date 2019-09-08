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

.global overflow_add_pos_8
.global overflow_add_pos_16
.global overflow_add_pos_32
.global overflow_add_pos_64

.global overflow_add_neg_8
.global overflow_add_neg_16
.global overflow_add_neg_32
.global overflow_add_neg_64

.global overflow_sub_8
.global overflow_sub_16
.global overflow_sub_32
.global overflow_sub_64

.global overflow_inc_8
.global overflow_inc_16
.global overflow_inc_32
.global overflow_inc_64

.global overflow_dec_neg_8
.global overflow_dec_neg_16
.global overflow_dec_neg_32
.global overflow_dec_neg_64

.global overflow

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

# handle add if the immediate added is positive
overflow_add_pos_8:
	lb t4, value0(s11) # src
	lb t5, value1(s11) # dst
	slt t4, t5, t4 # of = (dst < src) positive number added and the result is less than the operand
	ret
overflow_add_pos_16:
	lh t4, value0(s11) # src
	lh t5, value1(s11) # dst
	slt t4, t5, t4 # of = (dst < src) positive number added and the result is less than the operand
	ret
overflow_add_pos_32:
	lw t4, value0(s11) # src
	lw t5, value1(s11) # dst
	slt t4, t5, t4 # of = (dst < src) positive number added and the result is less than the operand
	ret
overflow_add_pos_64:
	ld t4, value0(s11) # src
	ld t5, value1(s11) # dst
	slt t4, t5, t4 # of = (dst < src) positive number added and the result is less than the operand
	ret

# handle add if the immediate added is negative
overflow_add_neg_8:
	lb t4, value0(s11) # src
	lb t5, value1(s11) # dst
	slt t4, t4, t5 # of = (src < dst) negative number added and the result is bigger than the operand
	ret
overflow_add_neg_16:
	lh t4, value0(s11) # src
	lh t5, value1(s11) # dst
	slt t4, t4, t5 # of = (src < dst) negative number added and the result is bigger than the operand
	ret
overflow_add_neg_32:
	lw t4, value0(s11) # src
	lw t5, value1(s11) # dst
	slt t4, t4, t5 # of = (src < dst) negative number added and the result is bigger than the operand
	ret
overflow_add_neg_64:
	ld t4, value0(s11) # src
	ld t5, value1(s11) # dst
	slt t4, t4, t5 # of = (src < dst) negative number added and the result is bigger than the operand
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
	lb t4, value0(s11) # src1
    lb t5, value1(s11) # src2
	slli t4, t4, 24
	slli t5, t5, 24
    subw t6, t5, t4 # dstw = src2 - src1 (32 bit)
    sub t5, t5, t4 # dst = src2 - src1
    sub t5, t5, t6 # of = (dstw != dst)
    snez t4, t5
    ret
overflow_sub_16:
	lh t4, value0(s11) # src1
	lh t5, value1(s11) # src2
	slli t4, t4, 16
	slli t5, t5, 16
    subw t6, t5, t4 # dstw = src2 - src1 (32 bit)
    sub t5, t5, t4 # dst = src2 - src1
    sub t5, t5, t6 # of = (dstw != dst)
    snez t4, t5
    ret
overflow_sub_32:
	lw t4, value0(s11) # src1
    lw t5, value1(s11) # src2
    subw t6, t5, t4 # dstw = src2 - src1 (32 bit)
    sub t5, t5, t4 # dst = src2 - src1
    sub t5, t5, t6 # of = (dstw != dst)
    snez t4, t5
    ret
overflow_sub_64: # add rax, -x = sub rax, x
	ld t4, value0(s11) # src1
	ld t5, value1(s11) # src2
	sub t6, t5, t4 # dst = src2 - src1
	slti t4, t4, 0 # t4 = sgn(src1)
	slt t5, t5, t6 # t5 = (src2 < dst) == (dst > src2)
	sub t4, t4, t5 # sgn(src1) == (dst > src2)
	snez t4, t4
	ret

# handle inc
overflow_inc_8:
	lbu t4, value0(s11)
	addi t5, t4, 1
	slt t4, t5, t4 # of = (dst + 1 < dst)
	ret
overflow_inc_16:
	lhu t4, value0(s11)
	addi t5, t4, 1
	slt t4, t5, t4 # of = (dst + 1 < dst)
	ret
overflow_inc_32:
	lwu t4, value0(s11)
	addi t5, t4, 1
	slt t4, t5, t4 # of = (dst + 1 < dst)
	ret
overflow_inc_64:
	ld t4, value0(s11)
	addi t5, t4, 1
	slt t4, t5, t4 # of = (dst + 1 < dst)
	ret

# handle dec and neg (the of has the exact same value function for each input)
overflow_dec_neg_8:
	lbu t4, value0(s11)
	addi t5, t4, -1
	slt t4, t4, t5 # of = (dst < dst - 1)
	ret
overflow_dec_neg_16:
	lhu t4, value0(s11)
	addi t5, t4, -1
	slt t4, t4, t5 # of = (dst < dst - 1)
	ret
overflow_dec_neg_32:
	lwu t4, value0(s11)
	addi t5, t4, -1
	slt t4, t4, t5 # of = (dst < dst - 1)
	ret
overflow_dec_neg_64:
	ld t4, value0(s11)
	addi t5, t4, -1
	slt t4, t4, t5 # of = (dst < dst - 1)
	ret
