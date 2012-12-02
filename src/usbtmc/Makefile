OCT := usbtmc.oct usbtmc_close.oct usbtmc_write.oct usbtmc_read.oct
OBJ := usbtmc_class.o

MKOCTFILE ?= mkoctfile

all: $(OBJ) $(OCT)

%.o: %.cc
	$(MKOCTFILE) -c $^

%.oct: %.o
	$(MKOCTFILE) $^ $(OBJ) -o ../$@

clean:
	rm -f *.oct *.o

.PHONY: all clean
