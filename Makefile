VERSION = 0.1

CC = cc
CFLAGS = -std=c99 -pedantic -Wall -D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE=700 -DVERSION=\"$(VERSION)\"

SRC = finger.c
OBJ = ${SRC:.c=.o}

all: finger

.c.o:
	${CC} -c ${CFLAGS} $<

finger: ${OBJ}
	${CC} -o $@ ${OBJ} ${LDFLAGS}

clean:
	rm -f finger ${OBJ}

.PHONY: all clean
