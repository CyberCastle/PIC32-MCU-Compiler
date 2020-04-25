all: libdsp.a

include		$(ROOT)/../defines.mk

_VPATH		:=.
include 	$(ROOT)/src/Makefile.inc
include 	$(ROOT)/wrapper/Makefile.inc

vpath	%.S	$(_VPATH)
vpath	%.c	$(_VPATH)

CPPFLAGS	= -I$(ROOT)/../include -I$(INCDESTDIR) -minterlink-mips16
CFLAGS		+=$(VAR) -DNDEBUG=1 -minterlink-mips16
CFLAGS		+=-mno-mips16
ASFLAGS		+=$(VAR)

# Optimise library for speed, not size
# But this library aliases all over the place, so switch off
# strict aliasing rules	
OPTIM		=-O2 -fno-strict-aliasing 

libdsp.a: $(LIBOBJ)
	$(STRIP) $(STRIPFLAGS) $(LIBOBJ)
	$(AR) rcs $@ $?

install: libdsp.a
	if [ -d $(LIBDESTDIR)/$(SUBDIR) ]; then \
		rm -f $(LIBDESTDIR)/$(SUBDIR)/libdsp.a; \
	else \
		mkdir -p $(LIBDESTDIR)/$(SUBDIR); \
	fi
	cp -p libdsp.a $(LIBDESTDIR)/$(SUBDIR)/libdsp.a

clean:
	rm -f libdsp.a *.o core*
