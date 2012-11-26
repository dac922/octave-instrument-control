OCT := gpib.oct gpib_timeout.oct gpib_write.oct gpib_close.oct gpib_read.oct
OBJ := gpib_class.o

MKOCTFILE ?= mkoctfile

GPIBPATH = /data/devel/octave/oic-toolbox/gpib_tb/linux-gpib
CFLAGS = -I$(GPIBPATH)/include
GPIBLIBS = $(GPIBPATH)/lib/.libs/*.o

all: $(OBJ) $(OCT)

%.o: %.cc
	$(MKOCTFILE) $(CFLAGS) -c $^

%.oct: %.o
	$(MKOCTFILE) $^ $(OBJ) $(GPIBLIBS) -o ../$@

clean:
	rm -f *.oct *.o

.PHONY: all clean
