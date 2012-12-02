OCT := tcp.oct tcp_timeout.oct tcp_write.oct tcp_close.oct tcp_read.oct
OBJ := tcp_class.o

MKOCTFILE ?= mkoctfile

all: $(OBJ) $(OCT)

%.o: %.cc
	$(MKOCTFILE) -c $^

%.oct: %.o
	$(MKOCTFILE) $^ $(OBJ) -o ../$@

clean:
	rm -f *.oct *.o

.PHONY: all clean
