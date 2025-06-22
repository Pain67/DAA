# GNU Makefile

CC=gcc
LD=ld
INCLUDE=-I../src
CFLAGS=$(INCLUDE) -O0
LIB_HEADERS=../src/DAA.h
LIB_SOURCES=../src/DAA.c
OUTPUT=build/

all: create-output-folder DAA LinearArena SmartArena SpeedTest

create-output-folder:
	mkdir -p ${OUTPUT}

DAA:
	$(CC) $(CFLAGS) -c $(LIB_SOURCES) -o DAA.o

LinearArena: DAA
	$(CC) $(CFLAGS) $(LDFLAGS) LinearArena.c DAA.o -o ${OUTPUT}LinearArena

SmartArena: DAA
	$(CC) $(CFLAGS) $(LDFLAGS) SmartArena.c DAA.o -o ${OUTPUT}SmartArena

SpeedTest: DAA
	$(CC) $(CFLAGS) $(LDFLAGS) SpeedTest.c DAA.o -o ${OUTPUT}SpeedTest

clean:
	rm -r ${OUTPUT}
	rm DAA.o
