#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/resource.h>
#include <unistd.h>

#include "util.h"

#define MAXNPROCS 64

static void serv(int req, char *group);
static int conn(char *host, char *port);
static void usage(void);

void
serv(int req, char *group)
{
	FILE *fp;

	fp = fdopen(req, "r+");
	if (!fp) {
		fprintf(stderr, "fdopen: "); //REPLACE
		perror(NULL);
		return;
	}

	fprintf(fp, "This is a test\r\n");
	fflush(fp);

	return;
}

int
conn(char *host, char *port)
{
	struct addrinfo hints;
	struct addrinfo *res;
	int fd;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if (getaddrinfo(host, port, &hints, &res) != 0) {
		die("cannot resolve hostname '%s':", host);
	}

	if ((fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) {
		die("cannot open socket:");
	}

	if (bind(fd, res->ai_addr, res->ai_addrlen) == -1) {
		die("cannot bind socket:");
	}

	if (listen(fd, SOMAXCONN) == -1) {
		die("cannot listen on address '%s':", host);
	}

	freeaddrinfo(res);
	return fd;
}

void
usage()
{
	die("usage: fingerd [-a addr] [-p port] [-g group] [-v] [-h]");
}

int main(int argc, char *argv[])
{
	char *host = "127.0.0.1", *port = "79", *group = "finger";
	struct sockaddr sa;
	struct rlimit rlim;
	struct timeval timeout;
	pid_t pid;
	socklen_t slen;
	int fd, req, i;

	for (i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "-v")) {
			fprintf(stderr, "fingerd-"VERSION"\n");
			return 0;
		} else if (!strcmp(argv[i], "-h")) {
			usage();
		} else if (!strcmp(argv[i], "-a")) {
			host = argv[++i];
		} else if (!strcmp(argv[i], "-p")) {
			port = argv[++i];
		} else if (!strcmp(argv[i], "-g")) {
			group = argv[++i];
		}
	}

	/* Ignore child exit signal */
	if (signal(SIGCHLD, SIG_IGN) == SIG_ERR) {
		die("could not ignore SIGCHLD");
	}

	fd = conn(host, port);

	rlim.rlim_cur = rlim.rlim_max = MAXNPROCS;
	if (setrlimit(RLIMIT_NPROC, &rlim) == -1) {
		die("cannot limit number of processes:");
	}

	for (;;) {
		slen = sizeof(sa);
		if ((req = accept(fd, &sa, &slen)) == -1) {
			fprintf(stderr, "accept: "); //REPLACE
			perror(NULL);
			continue;
		}

		pid = fork();

		if (pid == -1) {
			fprintf(stderr, "fork: "); //REPLACE
			perror(NULL);
			continue;
		} else if (pid == 0) {
			close(fd);

			/* Set timeout to 10 seconds */
			timeout.tv_sec = 10;
			timeout.tv_usec = 0;
			if (setsockopt(req, SOL_SOCKET, SO_RCVTIMEO, &timeout,
						sizeof(timeout)) == -1) {
				fprintf(stderr, "setsockopt: "); //REPLACE
				perror(NULL);
			}

			serv(req, group);

			shutdown(req, SHUT_RDWR);
			close(req);
			exit(0);
		} else {
			close(req);
		}
	}

	close(fd);

	return 0;
}
