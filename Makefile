BGPM = /bgsys/drivers/ppcfloor/bgpm

vulcan:
	mpixlc -c -std=c99 bench.c get_dim.c
	mpixlc -o bench get_dim.o bench.o bgqncl/libprofiler.a -L $(BGPM)/lib -lbgpm -lrt -lstdc++

