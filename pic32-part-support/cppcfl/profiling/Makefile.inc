_VPATH	+=:$(ROOT)/profiling

SUPOBJ  += cp_flp_dc.o cp_dcword_common.o

cp_flp_dc.o: cp_flp_dc.c
	$(CC) $(CFLAGS) -g1 -fframe-base-loclist -Os -c -D__LIBBUILD__ $< -o $@
	$(STRIP) $(STRIPFLAGS) $@
