_VPATH	+=:$(ROOT)/../../src:$(ROOT)/../../src/pic32

LIBOBJ=		e_acos.o e_asin.o e_atan2.o e_cosh.o e_exp.o e_fmod.o \
		e_log.o e_log10.o e_pow.o e_rem_pio2.o e_sinh.o e_sqrt.o \
		k_cos.o k_rem_pio2.o k_sin.o k_standard.o k_tan.o s_atan.o \
		s_ceil.o s_cos.o s_expm1.o s_fabs.o s_floor.o s_finite.o \
		s_frexp.o s_ldexp.o s_matherr.o s_modf.o s_rint.o \
		s_scalbn.o s_sin.o s_tan.o s_tanh.o w_acos.o w_asin.o \
		w_atan2.o w_cosh.o w_exp.o w_fmod.o w_log.o w_log10.o \
		w_pow.o w_sinh.o pic32_w_sqrt.o sqrtfd.o strtod.o strtof.o
