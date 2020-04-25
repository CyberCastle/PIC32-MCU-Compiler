PREFIX=pic32
TARGET=pic32mx
AR	=$(PREFIX)-ar
AS	=$(PREFIX)-gcc
CC	=$(PREFIX)-gcc
STRIP   =$(PREFIX)-strip

ifndef SDE_MAJOR
# Guess major SDE version based on compiler version
SDE_MAJOR := $(shell \
	if $(CC) -v 2>&1 | fgrep 'gcc version 3.' >/dev/null; then \
	  echo "6"; \
	else \
	  echo "5"; \
	fi)
endif

# Export to sub-makes
export SDE_MAJOR

SKIPARCHS=none

#EXCEPT	=-fexceptions

# Default to optimisation for size
OPTIM	=-O1 -fno-optimize-sibling-calls

# Don't generate gp-relative addresses
GPOPT	=-mno-gpopt

# Minimal backtrace debug info, no assertion checking
DEBUG	=-g1
ASSERT	=-DNDEBUG=1
CFLAGS	= $(VAR) $(OPTIM) $(GPOPT) $(DEBUG) $(ASSERT) $(EXCEPT) -ffreestanding -ffunction-sections
ASFLAGS	=-c $(VAR) -v
STRIPFLAGS =-X
ARCH	=mips

DESTROOT	=$(HOME)/xc32/install-image
TGTDESTDIR	=$(DESTROOT)/$(TARGET)
TGTLIBSRC	=$(DESTROOT)/pic32-libs
LIBDESTDIR	=$(TGTDESTDIR)/lib
INCDESTDIR	=$(TGTDESTDIR)/include

# flags to pass to recursive makes
F		=ROOT=$(ROOT)


