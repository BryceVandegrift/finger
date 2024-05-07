/* See LICENSE file for copyright and license details. */
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#include "util.h"

static int dial(char *host, char *port);
static void usage(void);

int
dial(char *host, char *port)
{
	struct addrinfo hints;
	struct addrinfo *res, *r;
	int fd = -1;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if (getaddrinfo(host, port, &hints, &res) != 0) {
		die("cannot resolve hostname '%s':", host);
	}

	for (r = res; r; r = r->ai_next) {
		if ((fd = socket(r->ai_family, r->ai_socktype, r->ai_protocol)) == -1) {
			continue;
		}
		if (connect(fd, r->ai_addr, r->ai_addrlen) == 0) {
			break;
		}

		close(fd);
	}

	freeaddrinfo(res);
	if (!r || fd == -1) {
		die("cannot connect to host '%s':", host);
	}
	return fd;
}

void
usage()
{
	die("usage: finger [user@]hostname [-v] [-h]");
}

int
main(int argc, char *argv[])
{
	char *port = "79", *host = "localhost", *user = NULL;
	char bufout[256];
	char *ret;
	FILE *srv;
	int i, c;

	for (i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "-v")) {
			fprintf(stderr, "finger-"VERSION"\n");
			return 0;
		} else if (!strcmp(argv[i], "-h")) {
			usage();
		} else {
			/* Parse finger address */
			ret = strchr(argv[i], '@');
			if (ret == NULL) {
				host = argv[i];
				break;
			}
			host = ++ret;
			user = strtok(argv[i], "@");
		}
	}

#ifdef __OpenBSD__
	if (pledge("stdio inet unix dns", NULL) == -1) {
		die("pledge");
	}
#endif /* __OpenBSD__ */

	srv = fdopen(dial(host, port), "r+");
	if (!srv) {
		die("fdopen:");
	}

	if (user == NULL) {
		snprintf(bufout, sizeof("\r\n"), "\r\n");
	} else {
		snprintf(bufout, sizeof(user) + sizeof("\r\n"), "%s\r\n", user);
	}
	fprintf(srv, "%s", bufout);
	fflush(srv);

	for (;;) {
		c = fgetc(srv);
		if (c == EOF) {
			break;
		}
		putchar(c);
	}

	fclose(srv);

	return 0;
}
