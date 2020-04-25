all: libdspr2.a

include		$(ROOT)/../defines.mk

_VPATH		:=.
include 	$(ROOT)/src/Makefile.inc

vpath	%.S	$(_VPATH)
vpath	%.c	$(_VPATH)

CPPFLAGS	=-I$(INCDESTDIR) -I$(ROOT)/../include -minterlink-mips16 -mno-jals -mno-smart-io
CFLAGS		+=$(VAR) -DNDEBUG=1 -minterlink-mips16 -mno-jals -mno-smart-io
CFLAGS      +=-mdspr2 -mno-mips16
ASFLAGS		+=$(VAR)

# Optimise library for speed, not size
# But this library aliases all over the place, so switch off
# strict aliasing rules	
OPTIM		=-O2 -fno-strict-aliasing 

libdspr2.a: $(LIBOBJ)
	$(STRIP) $(STRIPFLAGS) $(LIBOBJ)
	$(AR) rcs $@ $?

install: libdspr2.a
	if [ -d $(LIBDESTDIR)/$(SUBDIR) ]; then \
		rm -f $(LIBDESTDIR)/$(SUBDIR)/libdspr2.a; \
	else \
		mkdir -p $(LIBDESTDIR)/$(SUBDIR); \
	fi
	cp -p libdspr2.a $(LIBDESTDIR)/$(SUBDIR)/libdspr2.a

clean:
	rm -f libdspr2.a *.o core*
