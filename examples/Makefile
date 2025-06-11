# GNU Makefile

CC=gcc
INCLUDE=-I../src
CFLAGS=${INCLUDE} ${SRC}
LDFLAGS=

LIB_SOURCES=../src/DAA.h ../src/DAA.c
SOURCES= $(LIB_SOURCES)

OUTPUT=build/

all: create-output-folder LinearArena SmartArena SpeedTest

create-output-folder:
	mkdir -p ${OUTPUT}

LinearArena:
	$(CC) $(CFLAGS) $(LDFLAGS) $(SOURCES) LinearArena.c -o ${OUTPUT}LinearArena

SmartArena:
	$(CC) $(CFLAGS) $(LDFLAGS) $(SOURCES) SmartArena.c -o ${OUTPUT}SmartArena

SpeedTest:
	$(CC) $(CFLAGS) $(LDFLAGS) $(SOURCES) SpeedTest.c -o ${OUTPUT}SpeedTest

clean:
	rm -r ${OUTPUT}