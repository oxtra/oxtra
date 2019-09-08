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

.section .text

overflow_clear:
	li t4, 0
	ret

overflow_set:
	li t4, 1
	ret

# handle add, adc with carry = 0
overflow_add_8:
	lbu t4, flag_info_overflow_value0(s11) # src
    lbu t5, flag_info_overflow_value1(s11) # dst
    slt t4, t5, t4 # carry = (dst < src)
    srli t5, t5, 7 # sign bit of dst
    xor t4, t4, t5 # of = (cf ^ sign)
    ret
overflow_add_16:
	lhu t4, flag_info_overflow_value0(s11) # src
    lhu t5, flag_info_overflow_value1(s11) # dst
    slt t4, t5, t4 # carry = (dst < src)
    srli t5, t5, 15 # sign bit of dst
    xor t4, t4, t5 # of = (cf ^ sign)
    ret
overflow_add_32:
	lwu t4, flag_info_overflow_value0(s11) # src
    lwu t5, flag_info_overflow_value1(s11) # dst
    slt t4, t5, t4 # carry = (dst < src)
    srli t5, t5, 31 # sign bit of dst
    xor t4, t4, t5 # of = (cf ^ sign)
    ret
overflow_add_64:
	ld t4, flag_info_overflow_value0(s11) # src
    ld t5, flag_info_overflow_value1(s11) # dst
    slt t4, t5, t4 # carry = (dst < src)
    srli t5, t5, 63 # sign bit of dst
    xor t4, t4, t5 # of = (cf ^ sign)
    ret

# handle adc operation with carry flag set
overflow_adc_8:
	lbu t4, flag_info_overflow_value0(s11) # src
	lbu t5, flag_info_overflow_value1(s11) # dst
	slt t4, t4, t5 # (dst <= src) == !(dst > src) == !(src < dst)
	xori t4, t4, 1 # carry = !t4
	srli t5, t5, 7 # sign bit of dst
	xor t4, t4, t5 # of = (cf ^ sign)
	ret
overflow_adc_16:
	lhu t4, flag_info_overflow_value0(s11) # src
	lhu t5, flag_info_overflow_value1(s11) # dst
	slt t4, t4, t5 # (dst <= src) == !(dst > src) == !(src < dst)
	xori t4, t4, 1 # carry = !t4
	srli t5, t5, 15 # sign bit of dst
	xor t4, t4, t5 # of = (cf ^ sign)
	ret
overflow_adc_32:
	lwu t4, flag_info_overflow_value0(s11) # src
	lwu t5, flag_info_overflow_value1(s11) # dst
	slt t4, t4, t5 # (dst <= src) == !(dst > src) == !(src < dst)
	xori t4, t4, 1 # carry = !t4
	srli t5, t5, 31 # sign bit of dst
	xor t4, t4, t5 # of = (cf ^ sign)
	ret
overflow_adc_64:
	ld t4, flag_info_overflow_value0(s11) # src
	ld t5, flag_info_overflow_value1(s11) # dst
	slt t4, t4, t5 # (dst <= src) == !(dst > src) == !(src < dst)
	xori t4, t4, 1 # carry = !t4
	srli t5, t5, 63 # sign bit of dst
	xor t4, t4, t5 # of = (cf ^ sign)
	ret
