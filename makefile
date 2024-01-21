CC ?= gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c11 -D_POSIX_C_SOURCE=199309L

dog: build/dog.o
	$(CC) $(CFLAGS) -o build/dog build/dog.o

build/dog.o: dog.c build/
	$(CC) $(CFLAGS) -c -o build/dog.o dog.c

build/:
	mkdir -p build/

.PHONY: clean
clean:
	rm -rf build/
