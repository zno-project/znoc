	.section	__TEXT,__text,regular,pure_instructions
	.build_version macos, 12, 0
	.globl	__Z5raisev                      ## -- Begin function _Z5raisev
	.p2align	4, 0x90
__Z5raisev:                             ## @_Z5raisev
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	$1, %edi
	callq	___cxa_allocate_exception
	movq	%rax, %rdi
	movq	__ZTI9Exception@GOTPCREL(%rip), %rsi
	xorl	%eax, %eax
	movl	%eax, %edx
	callq	___cxa_throw
	.cfi_endproc
                                        ## -- End function
	.globl	__Z18try_but_dont_catchv        ## -- Begin function _Z18try_but_dont_catchv
	.p2align	4, 0x90
__Z18try_but_dont_catchv:               ## @_Z18try_but_dont_catchv
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	callq	__Z5raisev
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.globl	__Z7catchitv                    ## -- Begin function _Z7catchitv
	.p2align	4, 0x90
__Z7catchitv:                           ## @_Z7catchitv
Lfunc_begin0:
	.cfi_startproc
	.cfi_personality 155, ___gxx_personality_v0
	.cfi_lsda 16, Lexception0
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
Ltmp0:
	callq	__Z18try_but_dont_catchv
Ltmp1:
	jmp	LBB2_1
LBB2_1:
	jmp	LBB2_6
LBB2_2:
Ltmp2:
	movq	%rax, %rcx
	movl	%edx, %eax
	movq	%rcx, -8(%rbp)
	movl	%eax, -12(%rbp)
## %bb.3:
	movl	-12(%rbp), %eax
	movl	$1, %ecx
	cmpl	%ecx, %eax
	jne	LBB2_9
## %bb.4:
	movq	-8(%rbp), %rdi
	callq	___cxa_begin_catch
	movq	%rax, -24(%rbp)
Ltmp3:
	leaq	L_.str(%rip), %rdi
	xorl	%eax, %eax
                                        ## kill: def $al killed $al killed $eax
	callq	_printf
                                        ## kill: def $ecx killed $eax
Ltmp4:
	jmp	LBB2_5
LBB2_5:
	callq	___cxa_end_catch
LBB2_6:
	leaq	L_.str.1(%rip), %rdi
	movb	$0, %al
	callq	_printf
	addq	$32, %rsp
	popq	%rbp
	retq
LBB2_7:
Ltmp5:
	movq	%rdx, %rcx
	movq	%rax, %rsi
                                        ## kill: def $ecx killed $ecx killed $rcx
	movq	%rsi, -8(%rbp)
	movl	%ecx, -12(%rbp)
Ltmp6:
	callq	___cxa_end_catch
Ltmp7:
	jmp	LBB2_8
LBB2_8:
	jmp	LBB2_9
LBB2_9:
	movq	-8(%rbp), %rdi
	callq	__Unwind_Resume
LBB2_10:
Ltmp8:
	movq	%rax, %rdi
	callq	___clang_call_terminate
Lfunc_end0:
	.cfi_endproc
	.section	__TEXT,__gcc_except_tab
	.p2align	2
GCC_except_table2:
Lexception0:
	.byte	255                             ## @LPStart Encoding = omit
	.byte	155                             ## @TType Encoding = indirect pcrel sdata4
	.uleb128 Lttbase0-Lttbaseref0
Lttbaseref0:
	.byte	1                               ## Call site Encoding = uleb128
	.uleb128 Lcst_end0-Lcst_begin0
Lcst_begin0:
	.uleb128 Ltmp0-Lfunc_begin0             ## >> Call Site 1 <<
	.uleb128 Ltmp1-Ltmp0                    ##   Call between Ltmp0 and Ltmp1
	.uleb128 Ltmp2-             ##     jumps to Ltmp2
	.byte	1                               ##   On action: 1
	.uleb128 Ltmp1-Lfunc_begin0             ## >> Call Site 2 <<
	.uleb128 Ltmp3-Ltmp1                    ##   Call between Ltmp1 and Ltmp3
	.byte	0                               ##     has no landing pad
	.byte	0                               ##   On action: cleanup
	.uleb128 Ltmp3-Lfunc_begin0             ## >> Call Site 3 <<
	.uleb128 Ltmp4-Ltmp3                    ##   Call between Ltmp3 and Ltmp4
	.uleb128 Ltmp5-Lfunc_begin0             ##     jumps to Ltmp5
	.byte	0                               ##   On action: cleanup
	.uleb128 Ltmp4-Lfunc_begin0             ## >> Call Site 4 <<
	.uleb128 Ltmp6-Ltmp4                    ##   Call between Ltmp4 and Ltmp6
	.byte	0                               ##     has no landing pad
	.byte	0                               ##   On action: cleanup
	.uleb128 Ltmp6-Lfunc_begin0             ## >> Call Site 5 <<
	.uleb128 Ltmp7-Ltmp6                    ##   Call between Ltmp6 and Ltmp7
	.uleb128 Ltmp8-Lfunc_begin0             ##     jumps to Ltmp8
	.byte	3                               ##   On action: 2
	.uleb128 Ltmp7-Lfunc_begin0             ## >> Call Site 6 <<
	.uleb128 Lfunc_end0-Ltmp7               ##   Call between Ltmp7 and Lfunc_end0
	.byte	0                               ##     has no landing pad
	.byte	0                               ##   On action: cleanup
Lcst_end0:
	.byte	1                               ## >> Action Record 1 <<
                                        ##   Catch TypeInfo 1
	.byte	0                               ##   No further actions
	.byte	2                               ## >> Action Record 2 <<
                                        ##   Catch TypeInfo 2
	.byte	0                               ##   No further actions
	.p2align	2
                                        ## >> Catch TypeInfos <<
	.long	0                               ## TypeInfo 2
	.long	__ZTI9Exception@GOTPCREL+4      ## TypeInfo 1
Lttbase0:
	.p2align	2
                                        ## -- End function
	.section	__TEXT,__text,regular,pure_instructions
	.private_extern	___clang_call_terminate ## -- Begin function __clang_call_terminate
	.globl	___clang_call_terminate
	.weak_definition	___clang_call_terminate
	.p2align	4, 0x90
___clang_call_terminate:                ## @__clang_call_terminate
## %bb.0:
	pushq	%rax
	callq	___cxa_begin_catch
	callq	__ZSt9terminatev
                                        ## -- End function
	.globl	_seppuku                        ## -- Begin function seppuku
	.p2align	4, 0x90
_seppuku:                               ## @seppuku
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	callq	__Z7catchitv
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.globl	_main                           ## -- Begin function main
	.p2align	4, 0x90
_main:                                  ## @main
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movl	$0, -4(%rbp)
	callq	_seppuku
	xorl	%eax, %eax
	addq	$16, %rsp
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.section	__TEXT,__const
	.globl	__ZTS9Exception                 ## @_ZTS9Exception
	.weak_definition	__ZTS9Exception
__ZTS9Exception:
	.asciz	"9Exception"

	.section	__DATA,__const
	.globl	__ZTI9Exception                 ## @_ZTI9Exception
	.weak_definition	__ZTI9Exception
	.p2align	3
__ZTI9Exception:
	.quad	__ZTVN10__cxxabiv117__class_type_infoE+16
	.quad	__ZTS9Exception

	.section	__TEXT,__cstring,cstring_literals
L_.str:                                 ## @.str
	.asciz	"Caught an Exception!\n"

L_.str.1:                               ## @.str.1
	.asciz	"catchit handled the exception\n"

.subsections_via_symbols
