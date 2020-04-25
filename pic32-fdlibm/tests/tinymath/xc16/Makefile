# Make tiny test of libm functions and connections to "f", "", and "l" routines

# Tools and rules
CFLAGS=		-fno-builtin -O1 -Wl,--heap=0
NOSD=		-fno-short-double
SHDBL=		-DSHORT_DOUBLE
FAST=		-fast-math
RELAX=		-relaxed-math
LIBC=		-legacy-libc
FDLINC=		../../../src
FDLMATH=	-DFDL_MATH -I$(FDLINC) $(FAST)
FDLIBM=		../../../src/pic24/libfdm.a

CC=		xc16-gcc

.SUFFIXES:	.tst .ref

%.tst:
	./run $< > $@

.tst.ref:
	cp $< $@

# Components
TESTS=		tm-dfltsd tm-fastsd tm-rlaxsd tm-fdlbsd tm-dfltld tm-fastld tm-rlaxld tm-fdlbld
RESULTS=	$(TESTS:=.tst)
REFS=		$(RESULTS:.tst=.ref)
SRC=		../tinymath.c

# Targets
all:		$(TESTS)

tm-dfltsd:	$(SRC)
		$(CC) -o $@ $(LIBC) $(SHDBL) $(CFLAGS) $(SRC)

tm-fastsd:	$(SRC)
		$(CC) -o $@ $(LIBC) $(FAST) $(SHDBL) $(CFLAGS) $(SRC)

tm-rlaxsd:	$(SRC)
		$(CC) -o $@ $(LIBC) $(RELAX) $(SHDBL) $(CFLAGS) $(SRC)

tm-fdlbsd:	$(SRC)
		$(CC) -o $@ $(LIBC) $(FDLMATH) $(SHDBL) $(CFLAGS) $(SRC) $(FDLIBM)

tm-dfltld:	$(SRC)
		$(CC) -o $@ $(LIBC) $(NOSD) $(CFLAGS) $(SRC)

tm-fastld:	$(SRC)
		$(CC) -o $@ $(LIBC) $(FAST) $(NOSD) $(CFLAGS) $(SRC)

tm-rlaxld:	$(SRC)
		$(CC) -o $@ $(LIBC) $(RELAX) $(NOSD) $(CFLAGS) $(SRC)

tm-fdlbld:	$(SRC)
		$(CC) -o $@ $(LIBC) $(FDLMATH) $(NOSD) $(CFLAGS) $(SRC) $(FDLIBM)

test:		$(RESULTS)

reference:	$(REFS)

clean:
		rm -f $(TESTS) $(RESULTS)

# Dependencies
tm-fdlbsd:	$(FDLINC)/fdlmath.h
tm-fdlbld:	$(FDLINC)/fdlmath.h
tm-dfltsd.tst:	tm-dfltsd
tm-dfltld.tst:	tm-dfltld
tm-fastsd.tst:	tm-fastsd
tm-fastld.tst:	tm-fastld
tm-rlaxsd.tst:	tm-rlaxsd
tm-rlaxld.tst:	tm-rlaxld
tm-fdlbsd.tst:	tm-fdlbsd
tm-fdlbld.tst:	tm-fdlbld
tm-dfltsd.ref:	tm-dfltsd.tst
tm-dfltld.ref:	tm-dfltld.tst
tm-fastsd.ref:	tm-fastsd.tst
tm-fastld.ref:	tm-fastld.tst
tm-rlaxsd.ref:	tm-rlaxsd.tst
tm-rlaxld.ref:	tm-rlaxld.tst
tm-fdlbsd.ref:	tm-fdlbsd.tst
tm-fdlbld.ref:	tm-fdlbld.tst
