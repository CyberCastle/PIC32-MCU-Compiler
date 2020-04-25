
		.text
		.set	noreorder
		.set	noat
		.set    nomips16

		.global	mips_fft16_setup
		.global	mips_fft32_setup
		.ent	mips_fft_setup

#
# Empty deprecated routines for backward compatibility
# void mips_fft16_setup(int16c *twiddles, int log2N)
# void mips_fft32_setup(int32c *twiddles, int log2N)
#
mips_fft_setup:
mips_fft16_setup:
mips_fft32_setup:
		jr		$ra
		move		$0, $16

		.end	mips_fft_setup
		.size mips_fft_setup,.-mips_fft_setup

