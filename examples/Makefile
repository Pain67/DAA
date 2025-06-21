# GNU Makefile

CC=gcc
LD=ld
INCLUDE=-I../src
CFLAGS=${INCLUDE} -O0
LIB_SOURCES=../src/DAA.h ../src/DAA.c
OUTPUT=build/

all: create-output-folder DAA LinearArena SmartArena SpeedTest

create-output-folder:
	mkdir -p ${OUTPUT}

DAA:
	$(CC) $(CFLAGS) $(LDFLAGS) $(LIB_SOURCES) -c DAA.o

LinearArena: DAA
	$(CC) $(CFLAGS) $(LDFLAGS) LinearArena.c DAA.o -o ${OUTPUT}LinearArena

SmartArena: DAA
	$(CC) $(CFLAGS) $(LDFLAGS) SmartArena.c DAA.o -o ${OUTPUT}SmartArena.o

SpeedTest: DAA
	$(CC) $(CFLAGS) $(LDFLAGS) SpeedTest.c DAA.o -o ${OUTPUT}SpeedTest.o

clean:
	rm -r ${OUTPUT}
