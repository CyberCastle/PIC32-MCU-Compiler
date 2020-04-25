_VPATH	+=:$(ROOT)/src/

LIBOBJ += fft16.o h264_iqt-le.o vec_add16.o vec_mulc16.o
LIBOBJ += fft16_setup.o h264_iqt_setup.o 
LIBOBJ += vec_add32.o vec_mulc32.o
LIBOBJ += fft32.o 
LIBOBJ += h264_mc_luma.o 
LIBOBJ += vec_addc16.o vec_sub16.o
LIBOBJ += fft32_setup.o 
LIBOBJ += iir16.o vec_addc32.o vec_sub32.o
LIBOBJ += fir16-be.o 
LIBOBJ += iir16_setup.o 
LIBOBJ += vec_dotp16.o vec_sum_squares16.o
LIBOBJ += fir16-le.o lms16.o vec_dotp32.o vec_sum_squares32.o
LIBOBJ += fir16_setup.o 
LIBOBJ += vec_abs16.o vec_mul16.o
LIBOBJ += h264_iqt-be.o vec_abs32.o vec_mul32.o

