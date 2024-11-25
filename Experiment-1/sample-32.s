	.section	__TEXT,__text,regular,pure_instructions
	.build_version macos, 13, 0	sdk_version 13, 3
	.syntax unified
	.globl	_main                           @ -- Begin function main
	.p2align	2
	.code	32                              @ @main
_main:
@ %bb.0:
	sub	sp, sp, #8
	mov	r0, #0
	str	r0, [sp, #4]
	mov	r0, #4
	str	r0, [sp]
	ldr	r0, [sp]
	cmp	r0, #0
	beq	LBB0_2
	b	LBB0_1
LBB0_1:
	ldr	r0, [sp]
	add	r0, r0, #4
	str	r0, [sp]
	b	LBB0_3
LBB0_2:
	ldr	r0, [sp]
	lsl	r0, r0, #2
	str	r0, [sp]
	b	LBB0_3
LBB0_3:
	mov	r0, #0
	add	sp, sp, #8
	bx	lr
                                        @ -- End function
.subsections_via_symbols
