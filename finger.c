#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int dial(char *host, char *port) {
	struct addrinfo hints;
	struct addrinfo *res, *r;
	int fd;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if (getaddrinfo(host, port, &hints, &res) != 0) {
		exit(1);
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
	if (!r) {
		exit(1);
	}
	return fd;
}

void usage() {
	printf("usage: finger [user@]hostname [-v] [-h]\n");
	exit(1);
}

int main(int argc, char *argv[]) {
	char *port = "79";
	char *host = "localhost";
	char bufout[256];
	char *user = NULL;
	char *ret;
	FILE *srv;
	char c;

	for (int i = 1; i < argc; i++) {
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

	srv = fdopen(dial(host, port), "r+");
	if (!srv) {
		exit(1);
	}

	if (user == NULL) {
		sprintf(bufout, "\r\n");
	} else {
		sprintf(bufout, "%s\r\n", user);
	}
	fprintf(srv, bufout);
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
