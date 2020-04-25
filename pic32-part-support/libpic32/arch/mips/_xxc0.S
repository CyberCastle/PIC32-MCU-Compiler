/*
 * _xxc0.sx: generic MIPS Coprocessor 0 support functions copied from
 * MTK/kit/share/mipscp0.sx with modifications to support Microchip
 * devices.
 */

/*	
 * These functions allow unoptimised or MIPS16 code to get access to
 * coprocessor registers.
 */
	
#include <xc.h>
#include <sys/asm.h>

	.set	nomips16

/* unsigned _xxc0 (unsigned reg, unsigned clear, unsigned set) 
      cp0reg[REG] = (cp0reg[REG] & ~CLEAR) | SET
      returns previous value of register
      warning: not atomic in face of interrupts
*/
		
	
SLEAF(.xxc0_template)
	.set noreorder
	mfc0	v0,$0		#  0
	ssnop			#  4
	ssnop			#  8
	and	v1,v0,s0	# 12
	or	v1,s1		# 16
	beq	v1,v0,9f	# 20
	nop			# 24
#define	MTC0_OFF	28
	mtc0	v1,$0		# 28
9:	jr.hb	t7		# 32
	nop			# 36
#define	TEMPLATESZ	40
	.set reorder
SEND(.xxc0_template)

#define ARGSIZE			(NARGSAVE*SZARG)
#define LOCALSIZE		TEMPLATESZ
#define FRAMESIZE(nsave)	((ARGSIZE + LOCALSIZE + ((nsave)*SZREG) + ALSZ) & ALMASK)
#define SAVEOFF(n)		(ARGSIZE + LOCALSIZE + ((n)*SZREG))
	
NESTED(_xxc0, FRAMESIZE(3), ra)
	PTR_SUBU sp,FRAMESIZE(3)
	REG_S	s0,SAVEOFF(0)(sp)
	REG_S	s1,SAVEOFF(1)(sp)
	REG_S	ra,SAVEOFF(2)(sp)
	
	not	s0,a1			# s1 = ~CLEAR
	move	s1,a2			# s2 = SET
	
	/* convert REGNO to the mfc0/mtc0 cop0 rd register and select fields */
	and	t0,a0,0x1f		# RD = (REG % 32)
	sll	t0,11			# RD <<= 11
	srl	a0,5			# SEL = (REG / 32)
	and	a0,7
	or	a0,t0			# a0 = RD | SEL
	
	/* copy the template to the stack */
	la	t0,.xxc0_template
	PTR_ADDU t1,t0,TEMPLATESZ
	PTR_ADDU t2,sp,ARGSIZE
1:	lw	t3,0(t0)
	PTR_ADDU t0,4
	sw	t3,0(t2)
	PTR_ADDU t2,4
	bne	t0,t1,1b
	
	/* patch the mfc0 & mtc0 instructions */
	PTR_ADDU t4,sp,ARGSIZE
	PTR_ADDU t5,t4,MTC0_OFF
	lw	t2,0(t4)
	lw	t3,0(t5)
	or	t2,a0
	or	t3,a0
	sw	t2,0(t4)
	sw	t3,0(t5)
	
	/* do the work */
	addu	t4,sp,ARGSIZE
	jalr	t7,t4
	
	/* return */
	REG_L	ra,SAVEOFF(2)(sp)
	REG_L	s1,SAVEOFF(1)(sp)
	REG_L	s0,SAVEOFF(0)(sp)
	PTR_ADDU sp,FRAMESIZE(3)
	j	ra
END(_xxc0)

