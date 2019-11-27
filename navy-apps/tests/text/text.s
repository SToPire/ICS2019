	.file	"text.c"
	.text
	.section	.rodata
.LC0:
	.string	"r+"
.LC1:
	.string	"/share/texts/num"
.LC2:
	.string	"text.c"
.LC3:
	.string	"fp"
.LC4:
	.string	"size == 5000"
.LC5:
	.string	"%d"
.LC6:
	.string	"n == i + 1"
.LC7:
	.string	"%4d\n"
.LC8:
	.string	"n == i + 1 + 1000"
.LC9:
	.string	"PASS!!!"
	.text
	.globl	main
	.type	main, @function
main:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	leaq	.LC0(%rip), %rsi
	leaq	.LC1(%rip), %rdi
	call	fopen@PLT
	movq	%rax, -16(%rbp)
	cmpq	$0, -16(%rbp)
	jne	.L2
	leaq	__PRETTY_FUNCTION__.2375(%rip), %rcx
	movl	$6, %edx
	leaq	.LC2(%rip), %rsi
	leaq	.LC3(%rip), %rdi
	call	__assert_fail@PLT
.L2:
	movq	-16(%rbp), %rax
	movl	$2, %edx
	movl	$0, %esi
	movq	%rax, %rdi
	call	fseek@PLT
	movq	-16(%rbp), %rax
	movq	%rax, %rdi
	call	ftell@PLT
	movq	%rax, -24(%rbp)
	cmpq	$5000, -24(%rbp)
	je	.L3
	leaq	__PRETTY_FUNCTION__.2375(%rip), %rcx
	movl	$10, %edx
	leaq	.LC2(%rip), %rsi
	leaq	.LC4(%rip), %rdi
	call	__assert_fail@PLT
.L3:
	movq	-16(%rbp), %rax
	movl	$0, %edx
	movl	$2500, %esi
	movq	%rax, %rdi
	call	fseek@PLT
	movl	$500, -4(%rbp)
	jmp	.L4
.L6:
	leaq	-28(%rbp), %rdx
	movq	-16(%rbp), %rax
	leaq	.LC5(%rip), %rsi
	movq	%rax, %rdi
	movl	$0, %eax
	call	__isoc99_fscanf@PLT
	movl	-4(%rbp), %eax
	leal	1(%rax), %edx
	movl	-28(%rbp), %eax
	cmpl	%eax, %edx
	je	.L5
	leaq	__PRETTY_FUNCTION__.2375(%rip), %rcx
	movl	$16, %edx
	leaq	.LC2(%rip), %rsi
	leaq	.LC6(%rip), %rdi
	call	__assert_fail@PLT
.L5:
	addl	$1, -4(%rbp)
.L4:
	cmpl	$999, -4(%rbp)
	jle	.L6
	movq	-16(%rbp), %rax
	movl	$0, %edx
	movl	$0, %esi
	movq	%rax, %rdi
	call	fseek@PLT
	movl	$0, -4(%rbp)
	jmp	.L7
.L8:
	movl	-4(%rbp), %eax
	leal	1001(%rax), %edx
	movq	-16(%rbp), %rax
	leaq	.LC7(%rip), %rsi
	movq	%rax, %rdi
	movl	$0, %eax
	call	fprintf@PLT
	addl	$1, -4(%rbp)
.L7:
	cmpl	$499, -4(%rbp)
	jle	.L8
	movl	$500, -4(%rbp)
	jmp	.L9
.L11:
	leaq	-28(%rbp), %rdx
	movq	-16(%rbp), %rax
	leaq	.LC5(%rip), %rsi
	movq	%rax, %rdi
	movl	$0, %eax
	call	__isoc99_fscanf@PLT
	movl	-4(%rbp), %eax
	leal	1(%rax), %edx
	movl	-28(%rbp), %eax
	cmpl	%eax, %edx
	je	.L10
	leaq	__PRETTY_FUNCTION__.2375(%rip), %rcx
	movl	$25, %edx
	leaq	.LC2(%rip), %rsi
	leaq	.LC6(%rip), %rdi
	call	__assert_fail@PLT
.L10:
	addl	$1, -4(%rbp)
.L9:
	cmpl	$999, -4(%rbp)
	jle	.L11
	movq	-16(%rbp), %rax
	movl	$0, %edx
	movl	$0, %esi
	movq	%rax, %rdi
	call	fseek@PLT
	movl	$0, -4(%rbp)
	jmp	.L12
.L14:
	leaq	-28(%rbp), %rdx
	movq	-16(%rbp), %rax
	leaq	.LC5(%rip), %rsi
	movq	%rax, %rdi
	movl	$0, %eax
	call	__isoc99_fscanf@PLT
	movl	-4(%rbp), %eax
	leal	1001(%rax), %edx
	movl	-28(%rbp), %eax
	cmpl	%eax, %edx
	je	.L13
	leaq	__PRETTY_FUNCTION__.2375(%rip), %rcx
	movl	$31, %edx
	leaq	.LC2(%rip), %rsi
	leaq	.LC8(%rip), %rdi
	call	__assert_fail@PLT
.L13:
	addl	$1, -4(%rbp)
.L12:
	cmpl	$499, -4(%rbp)
	jle	.L14
	movq	-16(%rbp), %rax
	movq	%rax, %rdi
	call	fclose@PLT
	leaq	.LC9(%rip), %rdi
	call	puts@PLT
	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.section	.rodata
	.type	__PRETTY_FUNCTION__.2375, @object
	.size	__PRETTY_FUNCTION__.2375, 5
__PRETTY_FUNCTION__.2375:
	.string	"main"
	.ident	"GCC: (Debian 8.3.0-6) 8.3.0"
	.section	.note.GNU-stack,"",@progbits
