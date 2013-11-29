PREFIX=i586-elf
CC=$(PREFIX)-gcc
FLAGS=-ffreestanding -O2
MACROS=
CFLAGS=-std=gnu99 $(FLAGS) -Wall $(MACROS)
LFLAGS=$(FLAGS) -nostdlib
LIBS=-lgcc
LINK=link.ld
DEPS=$(wildcard *.h)
OBJ=$(wildcard *.c)
OBJS=$(OBJ:.c=.o)

OSNAME=myos
TARGET=$(OSNAME).bin
ISO=$(OSNAME).iso

.PHONY:boot.o

%.o: %.c $(DEPS)
	$(CC) -c $(CFLAGS) $< -o $@

all: $(ISO)

debug: clean
	make MACROS=-DDEBUG all

$(ISO): $(TARGET)
	cp $(TARGET) isodir/boot/
	cd initrd; export PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games; make build
	mv initrd/initrd.img isodir/boot/
	grub-mkrescue -o ../shared/$(ISO) isodir/

$(TARGET): boot.o $(OBJS)
	$(CC) -T $(LINK) -o $(TARGET) $(LFLAGS) boot.o $(OBJS) $(LIBS)

boot.o:
	nasm -f elf -o boot.o boot.s

clean:
	-rm *.o

