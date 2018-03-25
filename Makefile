CFLAGS += -Wall -Werror

all: test
	./test; hexdump -C test.nvm	

test: gpnvm.o test.o CuTest.o

gpnvm.o: gpnvm.h

test.o: gpnvm.h CuTest.h

CuTest.o: CuTest.h

clean:
	rm -rf test *.o
