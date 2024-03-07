# finger and fingerd version
VERSION = 0.5

# paths
PREFIX = /usr/local
MANPREFIX = ${PREFIX}/share/man

# flags
CPPFLAGS = -D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE=700 -DVERSION=\"${VERSION}\"
CFLAGS = -std=c89 -pedantic -Wall -Wextra -g ${CPPFLAGS}

# compiler and linker
CC = cc
