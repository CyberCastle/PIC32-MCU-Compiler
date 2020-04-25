/*
 * _xchsrspss.sx: Generic operations on PSS field in the SRSCTL register.
 * Copied out of MTK/kit/share/mipscp0.sx with modifications to support
 * Microchip specifics.
 */

	
/*	
 * These functions allow unoptimised or MIPS16 code to get access to
 * coprocessor registers.
 */
	
#include <xc.h>
#include <sys/asm.h>

	.set	nomips16
	
/* unsigned _xchsrspss (unsigned set) 
      SRSCTL<PSS> = SET
      returns previous value of PSS field
*/

LEAF(_xchsrspss)
	.set noreorder
	mfc0	t0,_CP0_SRSCTL
	ext	v0,t0,_CP0_SRSCTL_PSS_POSITION,_CP0_SRSCTL_PSS_LENGTH
	ins	t0,a0,_CP0_SRSCTL_PSS_POSITION,_CP0_SRSCTL_PSS_LENGTH
	mtc0	t0,_CP0_SRSCTL
	jr.hb	ra
	ehb	
	.set reorder
END(_xchsrspss)

