CC=gcc
CFLAGS=-I. --std=c99
DEPS=

OBJ = main.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

amaze: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:  
	rm -rf *.o amaze



