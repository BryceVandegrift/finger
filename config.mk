# finger and fingerd version
VERSION = 0.6

# paths
PREFIX = /usr/local
MANPREFIX = ${PREFIX}/share/man

# flags
CPPFLAGS = -D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE=700L -DVERSION=\"${VERSION}\" -D__BSD_VISIBLE -D_BSD_SOURCE
#CFLAGS = -std=c89 -pedantic -Wall -Wextra -g ${CPPFLAGS}
CFLAGS = -std=c89 -pedantic -Wall -Wextra -Os ${CPPFLAGS}

# compiler and linker
CC = cc
