	.section	__TEXT,__text,regular,pure_instructions
	.build_version macos, 12, 0
	.globl	_main                           ## -- Begin function main
	.p2align	4, 0x90
_main:                                  ## @main
Lfunc_begin0:
	.cfi_startproc
	.cfi_personality 155, ___my_personality_fn
	.cfi_lsda 16, Lexception0
## %bb.0:
	pushq	%rbx
	.cfi_def_cfa_offset 16
	subq	$16, %rsp
	.cfi_def_cfa_offset 32
	.cfi_offset %rbx, -16
	leaq	_s2(%rip), %rdi
	xorl	%eax, %eax
	callq	_printf
Ltmp0:
	callq	_test
Ltmp1:
## %bb.1:                               ## %Continue3
	movl	%eax, %ebx
Ltmp2:
	callq	_test
Ltmp3:
## %bb.2:                               ## %Continue
	movl	%ebx, 12(%rsp)
LBB0_3:                                 ## %Continue2
	leaq	_s3(%rip), %rdi
	xorl	%eax, %eax
	callq	_printf
	movl	12(%rsp), %eax
	addq	$16, %rsp
	popq	%rbx
	retq
LBB0_4:                                 ## %TestCleanup
Ltmp4:
	movl	%eax, 12(%rsp)
	leaq	_s1(%rip), %rdi
	xorl	%eax, %eax
	callq	_printf
	jmp	LBB0_3
Lfunc_end0:
	.cfi_endproc
	.section	__TEXT,__gcc_except_tab
	.p2align	2
GCC_except_table0:
Lexception0:
	.byte	255                             ## @LPStart Encoding = omit
	.byte	155                             ## @TType Encoding = indirect pcrel sdata4
	.uleb128 Lttbase0-Lttbaseref0
Lttbaseref0:
	.byte	1                               ## Call site Encoding = uleb128
	.uleb128 Lcst_end0-Lcst_begin0
Lcst_begin0:
	.uleb128 Lfunc_begin0-Lfunc_begin0      ## >> Call Site 1 <<
	.uleb128 Ltmp0-Lfunc_begin0             ##   Call between Lfunc_begin0 and Ltmp0
	.byte	0                               ##     has no landing pad
	.byte	0                               ##   On action: cleanup
	.uleb128 Ltmp0-Lfunc_begin0             ## >> Call Site 2 <<
	.uleb128 Ltmp3-Ltmp0                    ##   Call between Ltmp0 and Ltmp3
	.uleb128 Ltmp4-Lfunc_begin0             ##     jumps to Ltmp4
	.byte	1                               ##   On action: 1
	.uleb128 Ltmp3-Lfunc_begin0             ## >> Call Site 3 <<
	.uleb128 Lfunc_end0-Ltmp3               ##   Call between Ltmp3 and Lfunc_end0
	.byte	0                               ##     has no landing pad
	.byte	0                               ##   On action: cleanup
Lcst_end0:
	.byte	1                               ## >> Action Record 1 <<
                                        ##   Catch TypeInfo 1
	.byte	0                               ##   No further actions
	.p2align	2
                                        ## >> Catch TypeInfos <<
	.long	_catchType2@GOTPCREL+4          ## TypeInfo 1
Lttbase0:
	.p2align	2
                                        ## -- End function
	.section	__TEXT,__text,regular,pure_instructions
	.globl	_test                           ## -- Begin function test
	.p2align	4, 0x90
_test:                                  ## @test
	.cfi_startproc
## %bb.0:
	pushq	%rax
	.cfi_def_cfa_offset 16
	leaq	_unwindThing(%rip), %rdi
	callq	__Unwind_RaiseException
	movl	$67, %eax
	popq	%rcx
	retq
	.cfi_endproc
                                        ## -- End function
	.section	__DATA,__data
	.globl	_catchType1                     ## @catchType1
	.p2align	3
_catchType1:
	.quad	98                              ## 0x62

	.globl	_catchType2                     ## @catchType2
	.p2align	3
_catchType2:
	.quad	99                              ## 0x63

	.globl	_unwindThing                    ## @unwindThing
	.p2align	4
_unwindThing:
	.quad	67                              ## 0x43
	.quad	0
	.quad	0                               ## 0x0
	.quad	0                               ## 0x0
	.quad	99                              ## 0x63

	.globl	_s1                             ## @s1
	.p2align	4
_s1:
	.asciz	"Caught exception in main\n"

	.globl	_s2                             ## @s2
	.p2align	4
_s2:
	.asciz	"Throwing exception in main\n"

	.globl	_s3                             ## @s3
	.p2align	4
_s3:
	.asciz	"After catch in main\n"

.subsections_via_symbols
