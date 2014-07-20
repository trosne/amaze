CC=gcc
CFLAGS=-I. --std=c99 
DEPS= point.h bmp.h

OBJ = main.o point.o bmp.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

amaze: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

.PHONY: clean ctags tags

clean:  
	rm -rf *.o amaze



