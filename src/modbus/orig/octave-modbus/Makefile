OCT := oct-modbus.oct

TEST_TARGET := ./test/bandwidth-server-one

TEST2_TARGET := ./test/random-test-server

MKOCTFILE ?= mkoctfile

OCTAVE ?= octave

CC ?= gcc

CFLAGS = `pkg-config --cflags libmodbus`

LDFLAGS = `pkg-config --libs libmodbus`

all: $(OCT) $(TEST_TARGET) $(TEST2_TARGET)

./src/%.o: ./src/%.cpp
	$(MKOCTFILE) $(CFLAGS) -o ./$@ -c $^

./test/%.o: ./test/%.c
	$(CC) $(CFLAGS) -o ./$@ -c $^

%.oct: ./src/%.o
	$(MKOCTFILE) $(LDFLAGS) -o ./$@ $^
	rm $^

$(TEST_TARGET): ./test/bandwidth-server-one.o 
	$(CC) -o ./$@ $^ $(LDFLAGS) 
	rm $^

$(TEST2_TARGET): ./test/random-test-server.o 
	$(CC) -o ./$@ $^ $(LDFLAGS) 
	rm $^

check:
	$(OCTAVE) -f -q test/random_test_client.m
	$(OCTAVE) -f -q test/bandwidth_client.m

clean:
	rm -f *.oct ./src/*.o ./test/*.o $(TEST_TARGET) $(TEST2_TARGET)

.PHONY: all clean
