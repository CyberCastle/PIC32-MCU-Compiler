include	$(ROOT)/defines.mk

include $(ROOT)/Makefile.inc

vpath	%.c		$(_VPATH)
vpath	%.cc		$(_VPATH)
vpath	%.s		$(_VPATH)
vpath	%.sx		$(_VPATH)
vpath	%.S		$(_VPATH)
vpath	%.h		$(_VPATH)
vpath	Makefile	$(_VPATH)

# Force optimisation, but no inlining/unrolling

OPTIM	=-O1 -fno-inline-functions -fno-unroll-loops \
	 -foptimize-sibling-calls -ffunction-sections \
	 -fno-common -fno-builtin-exit -fno-builtin-abort -minterlink-mips16 \
	 -fno-jump-tables -fno-strict-aliasing
CFLAGS += $(VAR) $(OPTIM) $(GPOPT) -DNDEBUG=1 -D__LIBBUILD__ -I$(ROOT)/include -I$(ROOT)/../include -g1 -msmart-io=0 -Wcast-align -Wall

FDLIBM_OPTS=    $(OPTIM) -D__LITTLE_ENDIAN -D_POSIX_MODE -O1 -fno-short-double
XC_OPTS=        -DFDL_MATH
FPU_FLAGS=      -mhard-float -mfp64
CFLAGS =       $(VAR) $(OPTIM) $(GPOPT) -DNDEBUG=1 -D__LIBBUILD__ \
                -I$(ROOT)/include -I$(ROOT)/../include -g1 -msmart-io=0 \
                -Wcast-align -Wall $(FDLIBM_OPTS) $(XC_OPTS) $(FPU_FLAGS)

all: libmfd.a

libmfd.a: $(LIBOBJ)
	$(STRIP) $(STRIPFLAGS) $(LIBOBJ)
	$(AR) rcs $@ $(LIBOBJ)

install: libmfd.a
	if [ -d $(LIBDESTDIR)/$(SUBDIR) ]; then \
		rm -f $(LIBDESTDIR)/$(SUBDIR)/libmfd.a; \
	else \
		mkdir -p $(LIBDESTDIR)/$(SUBDIR); \
	fi
	cp -p libmfd.a $(LIBDESTDIR)/$(SUBDIR)/libmfd.a

clean:
	rm -f libmfd.a *.o core* *~
