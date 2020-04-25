/*
 * Allows MIPS16 code access to the SYNC instruction
 *
 * Function copied from MTK/kit/share/sync.c with modifications to be
 * specific for the Microchip devices.
 */
void __attribute__((__nomips16__)) _sync(void)
{
  __asm__ __volatile__ ("sync" : : : "memory");
}

