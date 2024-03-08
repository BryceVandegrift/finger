/* See LICENSE file for copyright and license details. */
#include <ctype.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "util.h"

void
die(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);

	if (fmt[0] && fmt[strlen(fmt) - 1] == ':') {
		fputc(' ', stderr);
		perror(NULL);
	} else {
		fputc('\n', stderr);
	}

	exit(1);
}

void
trim(char *s)
{
	char *e;

	for (e = s + strlen(s); e > s && isspace((unsigned char)*(e - 1)); e--);

	*e = '\0';
}
