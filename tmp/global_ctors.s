	.text
	.file	"global_ctors.ll"
	.globl	ctor                            # -- Begin function ctor
	.p2align	4, 0x90
	.type	ctor,@function
ctor:                                   # @ctor
	.cfi_startproc
# %bb.0:
	movq	a_stack@GOTPCREL(%rip), %rax
	movb	$5, (%rax)
	retq
.Lfunc_end0:
	.size	ctor, .Lfunc_end0-ctor
	.cfi_endproc
                                        # -- End function
	.globl	main                            # -- Begin function main
	.p2align	4, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# %bb.0:
	pushq	%rax
	.cfi_def_cfa_offset 16
	movq	a_stack@GOTPCREL(%rip), %rax
	movsbl	(%rax), %edi
	callq	printI@PLT
	xorl	%eax, %eax
	popq	%rcx
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end1:
	.size	main, .Lfunc_end1-main
	.cfi_endproc
                                        # -- End function
	.type	a_stack,@object                 # @a_stack
	.bss
	.globl	a_stack
a_stack:
	.byte	0                               # 0x0
	.size	a_stack, 1

	.section	.init_array,"aGw",@init_array,a_stack,comdat
	.p2align	3
	.quad	ctor
	.section	".note.GNU-stack","",@progbits
