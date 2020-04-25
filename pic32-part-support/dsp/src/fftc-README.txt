             Precomputed Coefficient Library
           for 16-bit and 32-bit FFT Functions

                    
README
Dec 11, 2008


1. Description of contents
This package contains a generator of precomputed coefficients and
an MPLAB project that illustrates how to create a library containing
the precomputed coefficients. These coefficients are intended for use
with the 16-bit and 32-bit FFT functions optimized for Microchip
PIC32MX micro-controllers and MIPS M4K processor cores.

The following files are included:

- fftc-README.txt     - this file
- fftcgen.c      - coefficient generator
- fft16*.c,
  fft32*.c       - precomputed coefficients output by the generator
- fftc.h         - header file declaring the precomputed coefficients
- fftc.a         - precomputed coefficients library
- fftc.mcp,
  fftc.mcw       - Microchip MPLAB project files

2. Building the library
The following steps outline the process of building the library:

- build an executable from the fftcgen.c file on a workstation
  - for example, on a Linux prompt type: gcc -o fftcgen fftcgen.c -lm
- run the executable
  - on a Linux prompt type: ./fftcgen
- the fftcgen executable will create all fft16*.c and fft32*.c files
- open the fftc.mcw workspace in Microchip's MPLAB IDE
- build the project
  - the final result is fftc.a

The library can be delivered to the users in binary form by providing
fftc.a (the library itself) and fftc.h (the header file). An even
better solution is to merge the precomputed coefficients library into
the existing DSP library together with the optimized FFT functions.

Since every coefficient array resides in a separate C file, the linker
has no trouble picking the correct object file from the library archive
and linking only the coefficients actually referenced by the user code.

3. Using the library

Usage of the library is very straightforward:

- add the library files (fftc.a and fftc.h) to your project in MPLAB
- include fftc.h in the source file that calls the FFT function
- use the appropriate array in place of the "twiddles" parameter
  - for example, fft16c256 should be used for 16-bit 256-point FFT

The following code snippet illustrates the use of the precomputed
coefficient library:

#include "fftc.h"

#define log2N 8
#define N     (1 << log2N)

void some_function(...)
{
	...
	fft16pic32(dout, din, fft16c256, scratch, log2N);
	...
}

Note: Using the approach outlined above the coefficients are accessed
directly from Flash memory, which results in small performance hit. To
ensure maximum performance, copy the coefficients to an array in RAM
during application startup:

	int16c twiddles[N/2];
	...
	memcpy(twiddles, fft16c256, sizeof(twiddles));
	...
	fft16pic32(dout, din, twiddles, scratch, log2N);

As an illustration, it takes 24168 cycles to compute a 256-point FFT
using coefficients in Flash, or 22632 cycles to perform the same
calculation using coefficients in RAM. Copying the coefficients from
Flash to RAM takes 824 cycles but typically needs to performed only
once during application startup.

