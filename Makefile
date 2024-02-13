include config.mk

REQ = util
BIN = finger fingerd

all: ${BIN} docs

finger: finger.o ${REQ:=.o}
fingerd: fingerd.o ${REQ:=.o}

finger.o: finger.c ${REQ:=.h}
fingerd.o: fingerd.c ${REQ:=.h}

.o:
	${CC} -o $@ $< ${REQ:=.o}

.c.o:
	${CC} -c ${CFLAGS} $<

docs:
	scdoc < finger.1.scd > finger.1
	scdoc < fingerd.8.scd > fingerd.8

install: all
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f finger fingerd ${DESTDIR}${PREFIX}/bin
	mkdir -p ${DESTDIR}${MANPREFIX}/man1
	mkdir -p ${DESTDIR}${MANPREFIX}/man8
	cp -f finger.1 ${DESTDIR}${MANPREFIX}/man1
	cp -f fingerd.8 ${DESTDIR}${MANPREFIX}/man8

uninstall:
	rm -f ${DESTDIR}${PREFIX}/bin/finger
	rm -f ${DESTDIR}${PREFIX}/bin/fingerd
	rm -f ${DESTDIR}${MANPREFIX}/man1/finger.1
	rm -f ${DESTDIR}${MANPREFIX}/man8/fingerd.8

clean:
	rm -f ${BIN} ${BIN:=.o} ${REQ:=.o}
	rm -f finger.1 fingerd.8

.PHONY: all clean install uninstall docs
