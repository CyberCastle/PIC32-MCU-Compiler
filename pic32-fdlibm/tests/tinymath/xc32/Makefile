# Make tiny test of libm functions and connections to "f", "", and "l" routines

# Tools and rules
CFLAGS=		-fno-builtin -O1
NOSD=		-fno-short-double
SHDBL=		-DSHORT_DOUBLE
RELAX=		-relaxed-math
LIBC=		/Users/CyberCastle/compiler/pic32-mcu-compiler/Darwin-image/pic32-tools/pic32mx/lib/libc.a
FDLINC=		-DFDL_MATH -I../../../src
FDLIBM=		-lmfd

CC=		/Users/CyberCastle/compiler/pic32-mcu-compiler/Darwin-image/pic32-tools/bin/pic32-gcc
#CC=		xc32-gcc
#CC=		pic32-gcc
# Components
TESTS=		tm-fdlbsd tm-fdlbld
SRC=		../tinymath.c

# Targets
all:		$(TESTS)

tm-fdlbsd:	$(SRC)
		$(CC) -mprocessor=32MZ2048EFM144 -msmart-io=0 -Wl,--defsym=_min_heap_size=0xF000 -o $@ $(LIBC) $(FDLINC) $(SHDBL) $(CFLAGS) $(SRC) $(FDLIBM) -save-temps

tm-fdlbld:	$(SRC)
		$(CC) -mprocessor=32MZ2048EFM144 -msmart-io=0 -Wl,--defsym=_min_heap_size=0xF000 -o $@ $(LIBC) $(NOSD) $(FDLINC) $(CFLAGS) $(SRC) $(FDLIBM) -save-temps

clean:
		rm -f $(TESTS)

# Dependencies
