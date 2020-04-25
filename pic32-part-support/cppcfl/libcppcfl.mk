include	$(ROOT)/../defines.mk

_VPATH	:= .

include $(ROOT)/profiling/Makefile.inc

vpath	%.c		$(_VPATH)
vpath	%.cc		$(_VPATH)
vpath	%.s		$(_VPATH)
vpath	%.sx		$(_VPATH)
vpath	%.S		$(_VPATH)
vpath	%.h		$(_VPATH)
vpath	Makefile	$(_VPATH)

CPPFLAGS = -I$(ROOT)/include -I$(ROOT)/../include
CFLAGS += $(VAR) -O2 -D__LIBBUILD__ -D__DEBUG -I$(INCDESTDIR) -I$(ROOT)/include -I$(ROOT)/../include -ffunction-sections -fdata-sections
ASFLAGS += $(VAR)

# Optimise math library for speed, not size
# But this library aliases all over the place, so switch off
# strict aliasing rules	
OPTIM		=-O2 -fno-strict-aliasing

all: libcppcfl.a

libcppcfl.a: $(SUPOBJ)
	$(STRIP) $(STRIPFLAGS) $(SUPOBJ)
	$(AR) rcs $@ $(SUPOBJ)

install: libcppcfl.a
	mkdir -p $(LIBDESTDIR)/$(SUBDIR)
	rm -f $(LIBDESTDIR)/$(SUBDIR)/libcppcfl.a
	cp -p libcppcfl.a $(LIBDESTDIR)/$(SUBDIR)/libcppcfl.a

clean:
	rm -f libcppcfl.a *.o core* *~

