VXI := vxi11_clnt.o vxi11_xdr.o
OCT := vxi11.oct vxi11_write.oct vxi11_close.oct vxi11_read.oct
OBJ := vxi11_class.o

MKOCTFILE  ?= mkoctfile
RPCGEN     ?= rpcgen
RPCINCLIDE ?= 

all: $(VXI) $(OBJ) $(OCT) 

vxi11.h vxi11_clnt.c vxi11_xdr.c : vxi11.x
	$(RPCGEN) -M vxi11.x

%.o: %.c
	$(MKOCTFILE) $(RPCINCLUDE) -c $^
	
%.o: %.cc
	$(MKOCTFILE) $(RPCINCLUDE) -c $^

%.oct: %.o
	$(MKOCTFILE) $^ $(OBJ) $(VXI) $(RPCLIBS) -o ../$@

clean:
	rm -f *.oct *.o
	rm -f vxi11.h vxi11_clnt.c vxi11_xdr.c vxi11_svc.c 

.PHONY: all clean
