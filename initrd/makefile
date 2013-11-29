CC=/usr/bin/gcc
CFLAGS=-Wall -ggdb -O2
SRC=$(wildcard *.c)
OBJ=$(SRC:.c=.o)
DEPS=$(wildcard *.h)
PACK=$(wildcard *.[^choi]*)
LIBS=
TARGET=mkinitrd

all: $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LIBS)

build: all
	./$(TARGET) $(PACK) > initrd.img

clean:
	-rm *.o
	-rm $(TARGET)

%.o: %.c $(DEPS)
	$(CC) -c $(CFLAGS) $< -o $@

