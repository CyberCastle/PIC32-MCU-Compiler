_VPATH	+=:$(ROOT)/src

LIBOBJ  += fft_setup.o
LIBOBJ  += fir16_setup.o iir16_setup.o h264_iqt_setup.o 

LIBOBJ	+= fft16.o
LIBOBJ  += fft16c1024.o fft16c128.o fft16c16.o fft16c2048.o 
LIBOBJ  += fft16c256.o fft16c32.o fft16c4096.o fft16c512.o fft16c64.o 

LIBOBJ	+= fir16.o
LIBOBJ  += h264_iqt.o h264_mc_luma.o
LIBOBJ  += iir16.o lms16.o
LIBOBJ	+= vec_abs16.o vec_add16.o  vec_addc16.o  vec_dotp16.o
LIBOBJ	+= vec_mul16.o vec_mulc16.o vec_sub16.o
LIBOBJ	+= vec_sum_squares16.o

LIBOBJ	+= fft32.o 
LIBOBJ  += fft32c1024.o fft32c128.o fft32c16.o fft32c2048.o 
LIBOBJ  += fft32c256.o fft32c32.o fft32c4096.o fft32c512.o fft32c64.o 
LIBOBJ	+= vec_abs32.o vec_add32.o  vec_addc32.o  vec_dotp32.o
LIBOBJ	+= vec_mul32.o vec_mulc32.o vec_sub32.o
LIBOBJ	+= vec_sum_squares32.o

