# finger and fingerd version
VERSION = 0.2

# paths
PREFIX = /usr/local

# flags
CPPFLAGS = -D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE=700 -DVERSION=\"${VERSION}\"
CFLAGS = -std=c99 -pedantic -Wall ${CPPFLAGS}

# compiler and linker
CC = cc
