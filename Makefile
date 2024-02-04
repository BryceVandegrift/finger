VERSION = 0.1

CC = cc
CFLAGS = -std=c99 -pedantic -Wall -D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE=700 -DVERSION=\"$(VERSION)\"

REQ = util
BIN = finger fingerd

all: ${BIN}

finger: finger.o ${REQ:=.o}
fingerd: fingerd.o ${REQ:=.o}

finger.o: finger.c ${REQ:=.h}
fingerd.o: fingerd.c ${REQ:=.h}

.o:
	${CC} -o $@ $< ${REQ:=.o}

.c.o:
	${CC} -c ${CFLAGS} $<

clean:
	rm -f ${BIN} ${BIN:=.o} ${REQ:=.o}

.PHONY: all clean
