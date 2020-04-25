include $(SDEMAKECONFIG)

CDEBUGFLAGS=-g1 -fframe-base-loclist
CFLAGS=$(CDEBUGFLAGS)

SRCS=setjmp.S memcpy.S memset.S libgcc2.c
OBJS=setjmp.o memcpy.o memset.o

#SRCS+=gmon.S
#OBJS+=gmon.o

# Library members defined in libgcc2.c.
LIB2FUNCS = _divdi3 _moddi3 _udivdi3 _umoddi3 \
     _udiv_w_sdiv _udivmoddi4 _floatdidf _floatdisf \
     _fixunsdfdi _fixdfdi _fixunssfdi _fixsfdi \
     _op_new _new_handler _op_delete _trampoline

stamp-r3000: $(SRCS) $(OBJS)
	$(STRIP) $(STRIPFLAGS) $(OBJS)
	$(AR) cr ../libc.a $(OBJS)
# -e causes any failing command to make this rule fail.
# -e doesn't work in certain shells, so we test $$? as well.
	set -e; \
	for name in $(LIB2FUNCS); \
	do \
	  echo $${name}; \
	  rm -f $${name}.o; \
	  $(CC) $(CDEBUGFLAGS) -I/usr2/gnu/src/gcc -c -DL$${name} libgcc2.c; \
	  if [ $$? -eq 0 ] ; then true; else exit 1; fi; \
	  mv libgcc2.o $${name}.o; \
	  $(STRIP) $(STRIPFLAGS) $${name}.o \
	  $(AR) rc ../libc.a $${name}.o; \
	  rm -f $${name}.o; \
	done
	date >stamp-r3000

setjmp.o: setjmp.S
	$(CC) $(CFLAGS) -c setjmp.S

clean:
	rm -f $(OBJS)
	rm -f stamp-r3000
