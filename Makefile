include config.mk

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

install: all
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f finger fingerd ${DESTDIR}${PREFIX}/bin

uninstall:
	rm -f ${DESTDIR}${PREFIX}/bin/finger
	rm -f ${DESTDIR}${PREFIX}/bin/fingerd

clean:
	rm -f ${BIN} ${BIN:=.o} ${REQ:=.o}

.PHONY: all clean install uninstall
