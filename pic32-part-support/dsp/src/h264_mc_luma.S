	.file	1 "h264_mc_luma.c"
	.loc 1 0
	.section .mdebug.abi32
	.previous
	.data
	.align	2
	.type	coef, @object
	.size	coef, 48
coef:
	.half	1
	.half	-5
	.half	20
	.half	20
	.half	-5
	.half	1
	.half	1
	.half	-5
	.half	52
	.half	20
	.half	-5
	.half	1
	.half	2
	.half	-10
	.half	40
	.half	40
	.half	-10
	.half	2
	.half	1
	.half	-5
	.half	20
	.half	52
	.half	-5
	.half	1
	.text
	.align	2
	.globl	mips_h264_mc_luma
	.set	nomips16
	.ent	mips_h264_mc_luma
mips_h264_mc_luma:
	.frame	$fp,144,$31		# vars= 112, regs= 3/0, args= 16, gp= 0
	.mask	0xc0010000,-8
	.fmask	0x00000000,0
	.set	noreorder
#ifdef __PIC__
	.cpload	$25
#endif	
	.set	nomacro
	
	addiu	$sp,$sp,-144
	sw	$31,136($sp)
	sw	$fp,132($sp)
	sw	$16,128($sp)
	move	$fp,$sp
	sw	$4,144($fp)
	sw	$5,148($fp)
	sw	$6,152($fp)
	sw	$7,156($fp)
	lw	$2,156($fp)
	sll	$3,$2,4
	lw	$2,160($fp)
	or	$2,$3,$2
	sw	$2,104($fp)
	lw	$2,104($fp)
	sltu	$2,$2,52
	beq	$2,$0,.L1
	nop

	lw	$2,104($fp)
	sll	$3,$2,2
	lw	$2,%got(.L77)
	addiu	$2,$2,%lo(.L77)
	addu	$2,$3,$2
	lw	$2,0($2)
	j	$2
	nop

	.section .rodata,code
	.align	2
.L77:
	.word	.L3
	.word	.L9
	.word	.L9
	.word	.L9
	.word	.L1
	.word	.L1
	.word	.L1
	.word	.L1
	.word	.L1
	.word	.L1
	.word	.L1
	.word	.L1
	.word	.L1
	.word	.L1
	.word	.L1
	.word	.L1
	.word	.L18
	.word	.L63
	.word	.L50
	.word	.L63
	.word	.L1
	.word	.L1
	.word	.L1
	.word	.L1
	.word	.L1
	.word	.L1
	.word	.L1
	.word	.L1
	.word	.L1
	.word	.L1
	.word	.L1
	.word	.L1
	.word	.L18
	.word	.L39
	.word	.L25
	.word	.L39
	.word	.L1
	.word	.L1
	.word	.L1
	.word	.L1
	.word	.L1
	.word	.L1
	.word	.L1
	.word	.L1
	.word	.L1
	.word	.L1
	.word	.L1
	.word	.L1
	.word	.L18
	.word	.L63
	.word	.L50
	.word	.L63
	.text
.L3:
	sw	$0,20($fp)
.L4:
	lw	$2,20($fp)
	slt	$2,$2,4
	beq	$2,$0,.L1
	nop

	lw	$4,148($fp)
#ifdef __PIC__
	lw	$25,%got(mips_ulw)($28)
	jalr	$25
	nop
#else
	jal	mips_ulw
	nop
#endif
	move	$5,$2
	lw	$2,20($fp)
	sll	$3,$2,2
	lw	$2,144($fp)
	addu	$2,$3,$2
	move	$4,$2

#ifdef __PIC__
	lw	$25,%got(mips_usw)($28)
	jalr	$25
	nop
#else
	jal	mips_usw
	nop
#endif
	lw	$3,148($fp)
	lw	$2,152($fp)
	addu	$2,$3,$2
	sw	$2,148($fp)
	lw	$2,20($fp)
	addiu	$2,$2,1
	sw	$2,20($fp)
	b	.L4
	nop

.L9:
	lw	$2,152($fp)
	sll	$3,$2,1
	lw	$2,148($fp)
	subu	$2,$2,$3
	sw	$2,148($fp)
	sw	$0,20($fp)
.L10:
	lw	$2,20($fp)
	slt	$2,$2,4
	beq	$2,$0,.L1
	nop

	sw	$0,16($fp)
.L13:
	lw	$2,16($fp)
	slt	$2,$2,4
	beq	$2,$0,.L14
	nop

	lw	$3,148($fp)
	lw	$2,16($fp)
	addu	$4,$3,$2
	lw	$3,160($fp)
	move	$2,$3
	sll	$2,$2,1
	addu	$2,$2,$3
	sll	$3,$2,2
	lw	$2,%got(coef)($28)
	addiu	$2,$2,%lo(coef)
	addu	$2,$3,$2
	lw	$5,152($fp)
	move	$6,$2

#ifdef __PIC__
	lw	$25,%got(fir8)($28)
	addiu	$25,$25,%lo(fir8)	
	jalr	$25
	nop
#else
	jal	fir8
	nop
#endif

	move	$3,$2
	lw	$2,160($fp)
	andi	$2,$2,0x1
	sll	$2,$2,4
	addu	$2,$3,$2
	sw	$2,108($fp)
	lw	$2,20($fp)
	sll	$3,$2,2
	lw	$2,16($fp)
	addu	$3,$3,$2
	lw	$2,144($fp)
	addu	$16,$3,$2
	lw	$2,108($fp)
	addiu	$2,$2,32
	sra	$2,$2,6
	move	$4,$2

#ifdef __PIC__
	lw	$25,%got(clip)($28)
	addiu	$25,$25,%lo(clip)
	jalr	$25
	nop
#else
	jal	clip
	nop
#endif

	sb	$2,0($16)
	lw	$2,16($fp)
	addiu	$2,$2,1
	sw	$2,16($fp)
	b	.L13
	nop

.L14:
	lw	$3,148($fp)
	lw	$2,152($fp)
	addu	$2,$3,$2
	sw	$2,148($fp)
	lw	$2,20($fp)
	addiu	$2,$2,1
	sw	$2,20($fp)
	b	.L10
	nop

.L18:
	sw	$0,20($fp)
.L19:
	lw	$2,20($fp)
	slt	$2,$2,4
	beq	$2,$0,.L1
	nop

	sw	$0,16($fp)
.L22:
	lw	$2,16($fp)
	slt	$2,$2,4
	beq	$2,$0,.L23
	nop

	lw	$3,148($fp)
	lw	$2,16($fp)
	addu	$2,$3,$2
	addiu	$4,$2,-2
	lw	$3,156($fp)
	move	$2,$3
	sll	$2,$2,1
	addu	$2,$2,$3
	sll	$3,$2,2
	lw	$2,%got(coef)($28)
	addiu	$2,$2,%lo(coef)
	addu	$2,$3,$2
	li	$5,1			# 0x1
	move	$6,$2

#ifdef __PIC__
	lw	$25,%got(fir8)($28)
	addiu	$25,$25,%lo(fir8)	
	jalr	$25
	nop
#else
	jal	fir8
	nop
#endif
	move	$3,$2
	lw	$2,156($fp)
	andi	$2,$2,0x1
	sll	$2,$2,4
	addu	$2,$3,$2
	sw	$2,108($fp)
	lw	$2,20($fp)
	sll	$3,$2,2
	lw	$2,16($fp)
	addu	$3,$3,$2
	lw	$2,144($fp)
	addu	$16,$3,$2
	lw	$2,108($fp)
	addiu	$2,$2,32
	sra	$2,$2,6
	move	$4,$2

#ifdef __PIC__
	lw	$25,%got(clip)($28)
	addiu	$25,$25,%lo(clip)
	jalr	$25
	nop
#else
	jal	clip
	nop
#endif

	sb	$2,0($16)
	lw	$2,16($fp)
	addiu	$2,$2,1
	sw	$2,16($fp)
	b	.L22
	nop

.L23:
	lw	$3,148($fp)
	lw	$2,152($fp)
	addu	$2,$3,$2
	sw	$2,148($fp)
	lw	$2,20($fp)
	addiu	$2,$2,1
	sw	$2,20($fp)
	b	.L19
	nop

.L25:
	lw	$2,152($fp)
	sll	$3,$2,1
	lw	$2,148($fp)
	subu	$2,$2,$3
	sw	$2,148($fp)
	sw	$0,20($fp)
.L26:
	lw	$2,20($fp)
	slt	$2,$2,9
	beq	$2,$0,.L27
	nop

	sw	$0,16($fp)
.L29:
	lw	$2,16($fp)
	slt	$2,$2,4
	beq	$2,$0,.L30
	nop

	lw	$3,16($fp)
	move	$2,$3
	sll	$2,$2,3
	addu	$2,$2,$3
	lw	$3,20($fp)
	addu	$2,$2,$3
	sll	$3,$2,1
	addiu	$2,$fp,16
	addu	$16,$3,$2
	lw	$3,148($fp)
	lw	$2,16($fp)
	addu	$2,$3,$2
	addiu	$2,$2,-2
	move	$4,$2
	li	$5,1			# 0x1
	lw	$2,%got(coef)($28)
	addiu	$6,$2,%lo(coef)

#ifdef __PIC__
	lw	$25,%got(fir8)($28)
	addiu	$25,$25,%lo(fir8)	
	jalr	$25
	nop
#else
	jal	fir8
	nop
#endif

	sh	$2,16($16)
	lw	$2,16($fp)
	addiu	$2,$2,1
	sw	$2,16($fp)
	b	.L29
	nop

.L30:
	lw	$3,148($fp)
	lw	$2,152($fp)
	addu	$2,$3,$2
	sw	$2,148($fp)
	lw	$2,20($fp)
	addiu	$2,$2,1
	sw	$2,20($fp)
	b	.L26
	nop

.L27:
	sw	$0,16($fp)
.L32:
	lw	$2,16($fp)
	slt	$2,$2,4
	beq	$2,$0,.L1
	nop

	sw	$0,20($fp)
.L35:
	lw	$2,20($fp)
	slt	$2,$2,4
	beq	$2,$0,.L34
	nop

	lw	$3,16($fp)
	move	$2,$3
	sll	$2,$2,3
	addu	$2,$2,$3
	lw	$3,20($fp)
	addu	$2,$2,$3
	sll	$3,$2,1
	addiu	$2,$fp,32
	addu	$2,$3,$2
	move	$4,$2
	lw	$2,%got(coef)($28)
	addiu	$5,$2,%lo(coef)

#ifdef __PIC__
	lw	$25,%got(fir16)($28)
	addiu	$25,$25,%lo(fir16)	
	jalr	$25
	nop
#else
	jal	fir16
	nop
#endif

	sw	$2,108($fp)
	lw	$2,20($fp)
	sll	$3,$2,2
	lw	$2,16($fp)
	addu	$3,$3,$2
	lw	$2,144($fp)
	addu	$16,$3,$2
	lw	$2,108($fp)
	addiu	$2,$2,512
	sra	$2,$2,10
	move	$4,$2

#ifdef __PIC__
	lw	$25,%got(clip)($28)
	addiu	$25,$25,%lo(clip)
	jalr	$25
	nop
#else
	jal	clip
	nop
#endif

	sb	$2,0($16)
	lw	$2,20($fp)
	addiu	$2,$2,1
	sw	$2,20($fp)
	b	.L35
	nop

.L34:
	lw	$2,16($fp)
	addiu	$2,$2,1
	sw	$2,16($fp)
	b	.L32
	nop

.L39:
	lw	$2,152($fp)
	sll	$3,$2,1
	lw	$2,148($fp)
	subu	$2,$2,$3
	sw	$2,148($fp)
	sw	$0,16($fp)
.L40:
	lw	$2,16($fp)
	slt	$2,$2,4
	beq	$2,$0,.L1
	nop

	lw	$2,148($fp)
	sw	$2,108($fp)
	sw	$0,20($fp)
.L43:
	lw	$2,20($fp)
	slt	$2,$2,9
	beq	$2,$0,.L44
	nop

	lw	$2,20($fp)
	sll	$3,$2,1
	addiu	$2,$fp,16
	addu	$16,$3,$2
	lw	$3,108($fp)
	lw	$2,16($fp)
	addu	$2,$3,$2
	addiu	$2,$2,-2
	move	$4,$2
	li	$5,1			# 0x1
	lw	$2,%got(coef)($28)
	addiu	$6,$2,%lo(coef)

#ifdef __PIC__
	lw	$25,%got(fir8)($28)
	addiu	$25,$25,%lo(fir8)	
	jalr	$25
	nop
#else
	jal	fir8
	nop
#endif
	sh	$2,16($16)
	lw	$3,108($fp)
	lw	$2,152($fp)
	addu	$2,$3,$2
	sw	$2,108($fp)
	lw	$2,20($fp)
	addiu	$2,$2,1
	sw	$2,20($fp)
	b	.L43
	nop

.L44:
	sw	$0,20($fp)
.L46:
	lw	$2,20($fp)
	slt	$2,$2,4
	beq	$2,$0,.L42
	nop

	addiu	$3,$fp,32
	lw	$2,20($fp)
	sll	$2,$2,1
	addu	$2,$3,$2
	move	$4,$2
	lw	$2,%got(coef)($28)
	addiu	$5,$2,%lo(coef)

#ifdef __PIC__
	lw	$25,%got(fir16)($28)
	addiu	$25,$25,%lo(fir16)	
	jalr	$25
	nop
#else
	jal	fir16
	nop
#endif
	sw	$2,112($fp)
	lw	$2,112($fp)
	addiu	$2,$2,512
	sra	$2,$2,10
	move	$4,$2

#ifdef __PIC__
	lw	$25,%got(clip)($28)
	addiu	$25,$25,%lo(clip)
	jalr	$25
	nop
#else
	jal	clip
	nop
#endif

	sw	$2,116($fp)
	lw	$2,160($fp)
	sra	$3,$2,1
	lw	$2,20($fp)
	addu	$2,$3,$2
	sll	$3,$2,1
	addiu	$2,$fp,16
	addu	$2,$3,$2
	lh	$2,20($2)
	addiu	$2,$2,16
	sra	$2,$2,5
	move	$4,$2

#ifdef __PIC__
	lw	$25,%got(clip)($28)
	addiu	$25,$25,%lo(clip)
	jalr	$25
	nop
#else
	jal	clip
	nop
#endif

	sw	$2,120($fp)
	lw	$2,20($fp)
	sll	$3,$2,2
	lw	$2,16($fp)
	addu	$3,$3,$2
	lw	$2,144($fp)
	addu	$4,$3,$2
	lw	$3,116($fp)
	lw	$2,120($fp)
	addu	$2,$3,$2
	addiu	$2,$2,1
	sra	$2,$2,1
	sb	$2,0($4)
	lw	$2,20($fp)
	addiu	$2,$2,1
	sw	$2,20($fp)
	b	.L46
	nop

.L42:
	lw	$2,16($fp)
	addiu	$2,$2,1
	sw	$2,16($fp)
	b	.L40
	nop

.L50:
	lw	$2,152($fp)
	sll	$3,$2,1
	lw	$2,148($fp)
	subu	$2,$2,$3
	sw	$2,148($fp)
	sw	$0,20($fp)
.L51:
	lw	$2,20($fp)
	slt	$2,$2,4
	beq	$2,$0,.L1
	nop

	sw	$0,16($fp)
.L54:
	lw	$2,16($fp)
	slt	$2,$2,9
	beq	$2,$0,.L55
	nop

	lw	$2,16($fp)
	sll	$3,$2,1
	addiu	$2,$fp,16
	addu	$16,$3,$2
	lw	$3,148($fp)
	lw	$2,16($fp)
	addu	$2,$3,$2
	addiu	$2,$2,-2
	move	$4,$2
	lw	$5,152($fp)
	lw	$2,%got(coef)($28)
	addiu	$6,$2,%lo(coef)

#ifdef __PIC__
	lw	$25,%got(fir8)($28)
	addiu	$25,$25,%lo(fir8)	
	jalr	$25
	nop
#else
	jal	fir8
	nop
#endif

	sh	$2,16($16)
	lw	$2,16($fp)
	addiu	$2,$2,1
	sw	$2,16($fp)
	b	.L54
	nop

.L55:
	sw	$0,16($fp)
.L57:
	lw	$2,16($fp)
	slt	$2,$2,4
	beq	$2,$0,.L58
	nop

	addiu	$3,$fp,32
	lw	$2,16($fp)
	sll	$2,$2,1
	addu	$2,$3,$2
	move	$4,$2
	lw	$2,%got(coef)($28)
	addiu	$5,$2,%lo(coef)
#ifdef __PIC__
	lw	$25,%got(fir16)($28)
	addiu	$25,$25,%lo(fir16)	
	jalr	$25
	nop
#else
	jal	fir16
	nop
#endif
	sw	$2,120($fp)
	lw	$2,120($fp)
	addiu	$2,$2,512
	sra	$2,$2,10
	move	$4,$2
#ifdef __PIC__
	lw	$25,%got(clip)($28)
	addiu	$25,$25,%lo(clip)
	jalr	$25
	nop
#else
	jal	clip
	nop
#endif
	sw	$2,116($fp)
	lw	$2,156($fp)
	sra	$3,$2,1
	lw	$2,16($fp)
	addu	$2,$3,$2
	sll	$3,$2,1
	addiu	$2,$fp,16
	addu	$2,$3,$2
	lh	$2,20($2)
	addiu	$2,$2,16
	sra	$2,$2,5
	move	$4,$2
#ifdef __PIC__
	lw	$25,%got(clip)($28)
	addiu	$25,$25,%lo(clip)
	jalr	$25
	nop
#else
	jal	clip
	nop
#endif
	sw	$2,112($fp)
	lw	$2,20($fp)
	sll	$3,$2,2
	lw	$2,16($fp)
	addu	$3,$3,$2
	lw	$2,144($fp)
	addu	$4,$3,$2
	lw	$3,116($fp)
	lw	$2,112($fp)
	addu	$2,$3,$2
	addiu	$2,$2,1
	sra	$2,$2,1
	sb	$2,0($4)
	lw	$2,16($fp)
	addiu	$2,$2,1
	sw	$2,16($fp)
	b	.L57
	nop

.L58:
	lw	$3,148($fp)
	lw	$2,152($fp)
	addu	$2,$3,$2
	sw	$2,148($fp)
	lw	$2,20($fp)
	addiu	$2,$2,1
	sw	$2,20($fp)
	b	.L51
	nop

.L63:
	lw	$2,152($fp)
	sll	$3,$2,1
	lw	$2,148($fp)
	subu	$2,$2,$3
	sw	$2,148($fp)
	sw	$0,20($fp)
.L64:
	lw	$2,20($fp)
	slt	$2,$2,4
	beq	$2,$0,.L65
	nop

	sw	$0,16($fp)
.L67:
	lw	$2,16($fp)
	slt	$2,$2,4
	beq	$2,$0,.L68
	nop

	lw	$2,156($fp)
	sra	$3,$2,1
	lw	$2,16($fp)
	addu	$3,$3,$2
	lw	$2,148($fp)
	addu	$2,$3,$2
	move	$4,$2
	lw	$5,152($fp)
	lw	$2,%got(coef)($28)
	addiu	$6,$2,%lo(coef)

#ifdef __PIC__
	lw	$25,%got(fir8)($28)
	addiu	$25,$25,%lo(fir8)	
	jalr	$25
	nop
#else
	jal	fir8
	nop
#endif

	sw	$2,120($fp)
	lw	$3,20($fp)
	move	$2,$3
	sll	$2,$2,3
	addu	$2,$2,$3
	lw	$3,16($fp)
	addu	$2,$2,$3
	sll	$3,$2,1
	addiu	$2,$fp,16
	addu	$16,$3,$2
	lw	$2,120($fp)
	addiu	$2,$2,16
	sra	$2,$2,5
	move	$4,$2

#ifdef __PIC__
	lw	$25,%got(clip)($28)
	addiu	$25,$25,%lo(clip)
	jalr	$25
	nop
#else
	jal	clip
	nop
#endif

	sh	$2,16($16)
	lw	$2,16($fp)
	addiu	$2,$2,1
	sw	$2,16($fp)
	b	.L67
	nop

.L68:
	lw	$3,148($fp)
	lw	$2,152($fp)
	addu	$2,$3,$2
	sw	$2,148($fp)
	lw	$2,20($fp)
	addiu	$2,$2,1
	sw	$2,20($fp)
	b	.L64
	nop

.L65:
	lw	$2,152($fp)
	sll	$3,$2,1
	lw	$2,148($fp)
	subu	$2,$2,$3
	sw	$2,148($fp)
	lw	$3,160($fp)
	li	$2,3			# 0x3
	bne	$3,$2,.L70
	nop

	lw	$2,148($fp)
	lw	$3,152($fp)
	addu	$2,$2,$3
	sw	$2,148($fp)
.L70:
	sw	$0,20($fp)
.L71:
	lw	$2,20($fp)
	slt	$2,$2,4
	beq	$2,$0,.L1
	nop

	sw	$0,16($fp)
.L74:
	lw	$2,16($fp)
	slt	$2,$2,4
	beq	$2,$0,.L75
	nop

	lw	$3,148($fp)
	lw	$2,16($fp)
	addu	$2,$3,$2
	addiu	$2,$2,-2
	move	$4,$2
	li	$5,1			# 0x1
	lw	$2,%got(coef)($28)
	addiu	$6,$2,%lo(coef)

#ifdef __PIC__
	lw	$25,%got(fir8)($28)
	addiu	$25,$25,%lo(fir8)	
	jalr	$25
	nop
#else
	jal	fir8
	nop
#endif
	sw	$2,120($fp)
	lw	$2,20($fp)
	sll	$3,$2,2
	lw	$2,16($fp)
	addu	$3,$3,$2
	lw	$2,144($fp)
	addu	$16,$3,$2
	lw	$2,120($fp)
	addiu	$2,$2,16
	sra	$2,$2,5
	move	$4,$2

#ifdef __PIC__
	lw	$25,%got(clip)($28)
	addiu	$25,$25,%lo(clip)
	jalr	$25
	nop
#else
	jal	clip
	nop
#endif

	move	$4,$2
	lw	$3,20($fp)
	move	$2,$3
	sll	$2,$2,3
	addu	$2,$2,$3
	lw	$3,16($fp)
	addu	$2,$2,$3
	sll	$3,$2,1
	addiu	$2,$fp,16
	addu	$2,$3,$2
	lh	$2,16($2)
	addu	$2,$4,$2
	addiu	$2,$2,1
	srl	$2,$2,1
	sb	$2,0($16)
	lw	$2,16($fp)
	addiu	$2,$2,1
	sw	$2,16($fp)
	b	.L74
	nop

.L75:
	lw	$3,148($fp)
	lw	$2,152($fp)
	addu	$2,$3,$2
	sw	$2,148($fp)
	lw	$2,20($fp)
	addiu	$2,$2,1
	sw	$2,20($fp)
	b	.L71
	nop

.L1:
	move	$sp,$fp
	lw	$31,136($sp)
	lw	$fp,132($sp)
	lw	$16,128($sp)
	addiu	$sp,$sp,144
	j	$31
	nop

	.set	macro
	.set	reorder
	.end	mips_h264_mc_luma
	.size	mips_h264_mc_luma, .-mips_h264_mc_luma
	.align	2
	.set	nomips16
	.ent	fir16
fir16:
	.frame	$fp,24,$31		# vars= 16, regs= 1/0, args= 0, gp= 0
	.mask	0x40000000,-8
	.fmask	0x00000000,0
	addiu	$sp,$sp,-24
	sw	$fp,16($sp)
	move	$fp,$sp
	sw	$4,24($fp)
	sw	$5,28($fp)
	lw	$2,24($fp)
	lh	$3,0($2)
	lw	$2,28($fp)
	lh	$2,0($2)
 #APP
	mult $3, $2
 #NO_APP
	mflo	$2
	mfhi	$3
	sw	$2,8($fp)
	sw	$3,12($fp)
	li	$2,1			# 0x1
	sw	$2,0($fp)
.L79:
	lw	$2,0($fp)
	slt	$2,$2,6
	beq	$2,$0,.L80
	lw	$4,8($fp)
	lw	$5,12($fp)
	lw	$2,0($fp)
	sll	$3,$2,1
	lw	$2,24($fp)
	addu	$2,$3,$2
	lh	$6,0($2)
	lw	$2,0($fp)
	sll	$3,$2,1
	lw	$2,28($fp)
	addu	$2,$3,$2
	lh	$2,0($2)
	mtlo	$4
	mthi	$5
 #APP
	madd $6, $2
 #NO_APP
	mflo	$4
	mfhi	$5
	sw	$4,8($fp)
	sw	$5,12($fp)
	lw	$2,0($fp)
	addiu	$2,$2,1
	sw	$2,0($fp)
	b	.L79
.L80:
	lw	$2,8($fp)
	move	$sp,$fp
	lw	$fp,16($sp)
	addiu	$sp,$sp,24
	j	$31
	.end	fir16
	.size	fir16, .-fir16
	.align	2
	.set	nomips16
	.ent	clip
clip:
	.frame	$fp,24,$31		# vars= 16, regs= 1/0, args= 0, gp= 0
	.mask	0x40000000,-8
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-24
	sw	$fp,16($sp)
	move	$fp,$sp
	sw	$4,24($fp)
	lw	$2,24($fp)
	sw	$2,4($fp)
	lw	$2,4($fp)
	bgez	$2,.L83
	nop

	sw	$0,4($fp)
.L83:
	lw	$2,4($fp)
	sw	$2,0($fp)
	lw	$2,24($fp)
	slt	$2,$2,256
	beq	$2,$0,.L84
	nop

	lw	$2,0($fp)
	sw	$2,8($fp)
	b	.L85
	nop

.L84:
	li	$2,255			# 0xff
	sw	$2,8($fp)
.L85:
	lw	$2,8($fp)
	sw	$2,0($fp)
	lw	$2,0($fp)
	move	$sp,$fp
	lw	$fp,16($sp)
	addiu	$sp,$sp,24
	j	$31
	nop

	.set	macro
	.set	reorder
	.end	clip
	.size	clip, .-clip
	.align	2
	.set	nomips16
	.ent	fir8
fir8:
	.frame	$fp,24,$31		# vars= 16, regs= 1/0, args= 0, gp= 0
	.mask	0x40000000,-8
	.fmask	0x00000000,0
	addiu	$sp,$sp,-24
	sw	$fp,16($sp)
	move	$fp,$sp
	sw	$4,24($fp)
	sw	$5,28($fp)
	sw	$6,32($fp)
	lw	$2,24($fp)
	lbu	$3,0($2)
	lw	$2,32($fp)
	lh	$2,0($2)
 #APP
	mult $3, $2
 #NO_APP
	mflo	$2
	mfhi	$3
	sw	$2,8($fp)
	sw	$3,12($fp)
	li	$2,1			# 0x1
	sw	$2,0($fp)
.L87:
	lw	$2,0($fp)
	slt	$2,$2,6
	beq	$2,$0,.L88
	lw	$3,24($fp)
	lw	$2,28($fp)
	addu	$2,$3,$2
	sw	$2,24($fp)
	lw	$4,8($fp)
	lw	$5,12($fp)
	lw	$2,24($fp)
	lbu	$6,0($2)
	lw	$2,0($fp)
	sll	$3,$2,1
	lw	$2,32($fp)
	addu	$2,$3,$2
	lh	$2,0($2)
	mtlo	$4
	mthi	$5
 #APP
	madd $6, $2
 #NO_APP
	mflo	$4
	mfhi	$5
	sw	$4,8($fp)
	sw	$5,12($fp)
	lw	$2,0($fp)
	addiu	$2,$2,1
	sw	$2,0($fp)
	b	.L87
.L88:
	lw	$2,8($fp)
	move	$sp,$fp
	lw	$fp,16($sp)
	addiu	$sp,$sp,24
	j	$31
	.end	fir8
	.size	fir8, .-fir8
	.align	2
	.set	nomips16
	.ent	mips_usw
mips_usw:
	.frame	$fp,16,$31		# vars= 8, regs= 1/0, args= 0, gp= 0
	.mask	0x40000000,-8
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-16
	sw	$fp,8($sp)
	move	$fp,$sp
	sw	$4,16($fp)
	sw	$5,20($fp)
	lw	$2,16($fp)
	sw	$2,0($fp)
	lw	$2,0($fp)
	lw	$3,20($fp)
	swl	$3,3($2)
	swr	$3,0($2)
	move	$sp,$fp
	lw	$fp,8($sp)
	addiu	$sp,$sp,16
	j	$31
	nop

	.set	macro
	.set	reorder
	.end	mips_usw
	.size	mips_usw, .-mips_usw
	.align	2
	.set	nomips16
	.ent	mips_ulw
mips_ulw:
	.frame	$fp,16,$31		# vars= 8, regs= 1/0, args= 0, gp= 0
	.mask	0x40000000,-8
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-16
	sw	$fp,8($sp)
	move	$fp,$sp
	sw	$4,16($fp)
	lw	$2,16($fp)
	sw	$2,0($fp)
	lw	$2,0($fp)
	lwl	$3,3($2)
	lwr	$3,0($2)
	move	$2,$3
	move	$sp,$fp
	lw	$fp,8($sp)
	addiu	$sp,$sp,16
	j	$31
	nop

	.set	macro
	.set	reorder
	.end	mips_ulw
	.size	mips_ulw, .-mips_ulw
# MCHP vector dispatch table
# MCHP configuration words
	.ident	"GCC: (GNU) 3.4.4   Microchip MPLAB C Compiler for PIC32 MCUs"
