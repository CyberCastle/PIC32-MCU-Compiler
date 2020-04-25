
#ifndef _FFTC_H_
#define _FFTC_H_

#include <dsplib_dsp.h>

#define mips_fftc16(N) fft16c##N 

extern const int16c fft16c16[8];
extern const int16c fft16c32[16];
extern const int16c fft16c64[32];
extern const int16c fft16c128[64];
extern const int16c fft16c256[128];
extern const int16c fft16c512[256];
extern const int16c fft16c1024[512];
extern const int16c fft16c2048[1024];
extern const int16c fft16c4096[2048];


#define mips_fftc32(N) fft32c##N 

extern const int32c fft32c16[8];
extern const int32c fft32c32[16];
extern const int32c fft32c64[32];
extern const int32c fft32c128[64];
extern const int32c fft32c256[128];
extern const int32c fft32c512[256];
extern const int32c fft32c1024[512];
extern const int32c fft32c2048[1024];
extern const int32c fft32c4096[2048];


#endif

