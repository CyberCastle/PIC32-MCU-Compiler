include	$(ROOT)/../defines.mk

_VPATH	:= .

include $(ROOT)/appio/Makefile.inc

vpath	%.c		$(_VPATH)
vpath	%.cc		$(_VPATH)
vpath	%.s		$(_VPATH)
vpath	%.sx		$(_VPATH)
vpath	%.S		$(_VPATH)
vpath	%.h		$(_VPATH)
vpath	Makefile	$(_VPATH)

CPPFLAGS = -I$(ROOT)/include -I$(ROOT)/../include
CFLAGS += $(VAR) -O2 -D__LIBBUILD__ -I$(INCDESTDIR) -I$(ROOT)/include -I$(ROOT)/../include -ffunction-sections -minterlink-mips16
ASFLAGS += $(VAR)

# Optimise math library for speed, not size
# But this library aliases all over the place, so switch off
# strict aliasing rules	
OPTIM		=-O2 -fno-strict-aliasing

all: libdebug.a

libdebug.a: $(LIBOBJ)
	$(STRIP) $(STRIPFLAGS) $(LIBOBJ)
	$(AR) rcs $@ $(LIBOBJ)

install: libdebug.a
	mkdir -p $(LIBDESTDIR)/$(SUBDIR)
	rm -f $(LIBDESTDIR)/$(SUBDIR)/libdebug.a
	cp -p libdebug.a $(LIBDESTDIR)/$(SUBDIR)/libdebug.a

clean:
	rm -f libdebug.a *.o core* *~

