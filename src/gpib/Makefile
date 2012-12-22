OCT := gpib.oct gpib_timeout.oct gpib_write.oct gpib_close.oct gpib_read.oct
OBJ := gpib_class.o

MKOCTFILE ?= mkoctfile

GPIBLIBS ?= -lgpib

all: $(OBJ) $(OCT)

%.o: %.cc
	$(MKOCTFILE) -c $^

%.oct: %.o
	$(MKOCTFILE) $^ $(OBJ) $(GPIBLIBS) -o ../$@

clean:
	rm -f *.oct *.o

.PHONY: all clean
