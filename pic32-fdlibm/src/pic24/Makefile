# $Id: Makefile,v 1.5 2012/05/08 20:15:13 wilkiec Exp $

# Make C30/XC16 version of FDLIBM 5.3, supports double and "l" routines, "f"
# routines provided in a separate, PIC24/dsPIC-specific library

# Tools and rules

# Do not raise optimization past -O1 on FDLIBM code !!! Need no-short-double
FDLIBM_OPTS=	-D__LITTLE_ENDIAN -D_POSIX_MODE -O1 -fno-short-double
XC_OPTS=	-ffunction-sections -fdata-sections -mtext=.libfd
CFLAGS=		$(FDLIBM_OPTS) $(XC_OPTS) -Wall

AR=		$(PIC30_CD)/bin/xc16-ar
AS=		$(PIC30_CD)/bin/xc16-as
CC=		$(PIC30_CD)/bin/xc16-gcc
RANLIB=		$(PIC30_CD)/bin/xc16-ranlib

# Components

# These are from FDLIBM, everything except the insanely correct argument
# reduction for very large multiples of pi, and square root, which we provide
# separately, in a PIC24/dsPIC-specific way (XC32 makes the same choices)

# Some functions that are beyond C89 are also not included
COBJ=		e_acos.o e_asin.o e_atan2.o e_cosh.o e_exp.o e_fmod.o \
		e_log.o e_log10.o e_pow.o e_rem_pio2.o e_sinh.o k_cos.o \
		k_rem_pio2.o k_sin.o k_standard.o k_tan.o s_atan.o \
		s_ceil.o s_cos.o s_expm1.o s_fabs.o s_floor.o s_finite.o \
		s_frexp.o s_ldexp.o s_matherr.o s_modf.o s_rint.o \
		s_scalbn.o s_sin.o s_tan.o s_tanh.o w_acos.o w_asin.o \
		w_atan2.o w_cosh.o w_exp.o w_fmod.o w_log.o w_log10.o \
		w_pow.o w_sinh.o

# PIC24/dsPIC-specific square root, and "l" pass-throughs to double routines
AOBJ=		acosl.o asinl.o atanl.o atan2l.o ceill.o \
		cosl.o coshl.o expl.o fabsl.o floorl.o \
		fmodl.o frexpl.o ldexpl.o logl.o log10l.o \
		modfl.o powl.o sinl.o sinhl.o tanl.o tanhl.o \
		dsqrt.o

COFF_LIB=       libdpm-coff.a errata_libdpm-coff.a 
ELF_LIB=	libdpm-elf.a  errata_libdpm-elf.a
LIB=		$(COFF_LIB) $(ELF_LIB)

# Targets
all:		$(LIB)

coff:		$(COFF_LIB)

elf:		$(ELF_LIB)

# C30 specific stuff
VPATH = ..

ASM_ERRATA=--defsym EXCH_ERRATA=1
C_ERRATA="-merrata=exch"
CPU=generic-16bit
EPCPU=generic-16bit-ep

INSTALL=../../$(LIBDIR)

.SUFFIXES:
.SUFFIXES: .c .o .eo .s .a .e_o .e_eo .epo .epeo .epe_o .epe_eo .efo

%.o : %.s
	@echo COFF: $<
	@$(AS) -omf=coff -o $@ $(ASFLAGS) $< -p $(CPU)

%.e_o : %.s
	@echo ERRATA COFF: $<
	@$(AS) -omf=coff $(ASM_ERRATA) -o $@ $(ASFLAGS) $< -p $(CPU)

%.eo : %.s
	@echo ELF: $<
	@$(AS) -omf=elf --defsym ffunction=1 -o $@ $(ASFLAGS) $<

%.e_eo : %.s
	@echo ERRATA ELF: $<
	@$(AS) -omf=elf $(ASM_ERRATA) -o $@ $(ASFLAGS) $< -p $(CPU)

%.o : %.c
	@echo COFF: $<
	@$(CC) -omf=coff -c $(CFLAGS) -o $@ $< -mcpu=$(CPU)

%.eo : %.c
	@echo ELF: $<
	@$(CC) -omf=elf -ffunction-sections -c $(CFLAGS) -o $@ $< -mcpu=$(CPU)

%.e_o : %.c
	@echo ERRATA COFF: $<
	@$(CC) -omf=coff $(C_ERRATA) -c $(CFLAGS) -o $@ $< -mcpu=$(CPU)

%.e_eo : %.c
	@echo ERRATA ELF: $<
	@$(CC) -omf=elf $(C_ERRATA) -c $(CFLAGS) -o $@ $< -mcpu=$(CPU)

# EP

%.epo : %.s
	@echo COFF: $<
	@$(AS) -omf=coff -o $@ $(ASFLAGS) $< -p $(EPCPU)

%.epe_o : %.s
	@echo ERRATA COFF: $<
	@$(AS) -omf=coff $(ASM_ERRATA) -o $@ $(ASFLAGS) $< -p $(EPCPU)

%.epeo : %.s
	@echo ELF: $<
	@$(AS) -omf=elf --defsym ffunction=1 -o $@ $(ASFLAGS) $< -p $(EPCPU)

%.epe_eo : %.s
	@echo ERRATA ELF: $<
	@$(AS) -omf=elf $(ASM_ERRATA) -o $@ $(ASFLAGS) $< -p $(EPCPU)

%.epo : %.c
	@echo COFF: $<
	@$(CC) -omf=coff -c $(CFLAGS) -o $@ $< -mcpu=$(EPCPU)

%.epeo : %.c
	@echo ELF: $<
	@$(CC) -omf=elf -ffunction-sections -c $(CFLAGS) -o $@ $< -mcpu=$(EPCPU)

%.epe_o : %.c
	@echo ERRATA COFF: $<
	@$(CC) -omf=coff $(C_ERRATA) -c $(CFLAGS) -o $@ $< -mcpu=$(EPCPU)

%.epe_eo : %.c
	@echo ERRATA ELF: $<
	@$(CC) -omf=elf $(C_ERRATA) -c $(CFLAGS) -o $@ $< -mcpu=$(EPCPU)


ELF_OBJS=$(COBJ:.o=.eo) $(AOBJ:.o=.eo) $(COBJ:.o=.epeo) $(AOBJ:.o=.epeo)
COFF_OBJS=$(COBJ) $(AOBJ) $(COBJ:.o=.epo) $(AOBJ:.o=.epo)
ERRATA_ELF_OBJS=$(COBJ:.o=.e_eo) $(AOBJ:.o=.e_eo) $(COBJ:.o=.epe_eo) $(AOBJ:.o=.epe_eo)
ERRATA_COFF_OBJS=$(COBJ:.o=.e_o) $(AOBJ:.o=.e_o) $(COBJ:.o=.epe_o) $(AOBJ:.o=.epe_o)

libdpm-coff.a:  $(COFF_OBJS)
	$(AR) -omf=coff rc $@ $(COFF_OBJS)
	$(RANLIB) -omf=coff $@
	mkdir -p $(INSTALL)
	cp $@ $(INSTALL)

libdpm-elf.a:  $(ELF_OBJS)
	$(AR) -omf=elf rc $@ $(ELF_OBJS)
	$(RANLIB) -omf=elf $@
	mkdir -p $(INSTALL)
	cp $@ $(INSTALL)

errata_libdpm-coff.a:  $(ERRATA_COFF_OBJS)
	$(AR) -omf=coff rc $@ $(ERRATA_COFF_OBJS)
	$(RANLIB) -omf=coff $@
	mkdir -p $(INSTALL)
	cp $@ $(INSTALL)

errata_libdpm-elf.a:  $(ERRATA_ELF_OBJS)
	$(AR) -omf=elf rc $@ $(ERRATA_ELF_OBJS)
	$(RANLIB) -omf=elf $@
	mkdir -p $(INSTALL)
	cp $@ $(INSTALL)


clean:
		rm -f $(COFF_OBJS) $(ELF_OBJS) $(ERRATA_ELF_OBJS) $(ERRATA_COFF_OBJS) $(LIB)

# Dependencies
$(COBJ):	../fdlibm.h
