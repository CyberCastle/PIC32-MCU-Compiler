/* Function-Level profiling support functions */

#define DATA_CAPTURE_WORD_SIZE  sizeof(int)
#define _CPSTAT (*((volatile int *)(0xBF880060)))
#define DATA_CAPTURE1_BUSY  0x00000001

/* Global Variables */
volatile unsigned int __DCWord __attribute__ ((persistent)) ;
 
void  _HAS_FUNCTIONLEVELPROF_(void) __attribute__ ((alias("__function_level_profiling_long"))) ;

register int __pic32_function_level_profiling_long_donotuse2 __asm__("$2");
register int __pic32_function_level_profiling_long_donotuse3 __asm__("$3");
register int __pic32_function_level_profiling_long_donotuse4 __asm__("$4");
register int __pic32_function_level_profiling_long_donotuse5 __asm__("$5");
register int __pic32_function_level_profiling_long_donotuse6 __asm__("$6");
register int __pic32_function_level_profiling_long_donotuse7 __asm__("$7");
  
void __attribute__((optimize("-Os"),nomips16,section(".libcppcfl")))
__function_level_profiling_long (void)
{
  register unsigned int save_status;

  asm volatile("di    %0" : "=r"(save_status));
  asm volatile ("ehb");
  // wait for previous data capture to complete
  while (_CPSTAT & DATA_CAPTURE1_BUSY) ;
  
  __DCWord = (unsigned int)__builtin_return_address(0);
   
  if(save_status)
    asm volatile("ei");
}

void __attribute__((optimize("-Os"),nomips16,section(".libcppcfl")))
__function_level_profiling_long_zero (void)
{
  register unsigned int save_status;
  
  asm volatile("di    %0" : "=r"(save_status));
  asm volatile ("ehb");
  // wait for previous data capture to complete
  while (_CPSTAT & DATA_CAPTURE1_BUSY) ;
  
  __DCWord = 0;
   
  if(save_status)
    asm volatile("ei");
}
