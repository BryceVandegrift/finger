#include <errno.h>
#include <grp.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pwd.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/resource.h>
#include <unistd.h>

#include "util.h"

#define MAXNPROCS 64

static void sendplan(char *user, FILE *out);
static void serv(int req, char *group);
static int conn(char *host, char *port);
static void usage(void);

void
sendplan(char *user, FILE *out)
{
	struct passwd *usrinfo;
	FILE *plan;
	char *path;
	char c;

	for (usrinfo = getpwent(); usrinfo != NULL; usrinfo = getpwent()) {
		if (!strcmp(usrinfo->pw_name, user)) {
			fprintf(out, "user: %s\r\n", usrinfo->pw_name);
			fprintf(out, "name: %s\r\n", usrinfo->pw_gecos);
			fprintf(out, "dir: %s\r\n", usrinfo->pw_dir);
			fprintf(out, "shell: %s\r\n", usrinfo->pw_shell);
			fprintf(out, "plan:\r\n");

			path = strncat(usrinfo->pw_dir, "/.plan", sizeof(usrinfo->pw_dir) - strlen(usrinfo->pw_dir) - 1);
			plan = fopen(path, "r");
			if (!plan) {
				fprintf(out, "no plan\r\n");
			} else {
				for (;;) {
					c = fgetc(plan);
					if (c == EOF) {
						break;
					}
					fputc(c, out);
				}
				fclose(plan);
			}
			fclose(out);
			goto end;
		}
	}
	fprintf(out, "could not find user: '%s'\r\n", user);

end:
	endpwent();
}

void
serv(int req, char *grp)
{
	struct group *grinfo;
	char user[64];
	size_t usrsize;
	FILE *fp;
	int i;

	fp = fdopen(req, "r+");
	if (!fp) {
		fprintf(stderr, "fopen: %s\n", strerror(errno));
		return;
	}

	/* check if a username is sent */
	fgets(user, sizeof(user), fp);
	if ((usrsize = strcspn(user, "\r\n")) > 0) {
		trim(user);

		sendplan(user, fp);
	} else {
		fprintf(fp, "users on this server:\r\n");

		grinfo = getgrnam(grp);
		if (grinfo == NULL) {
			fprintf(stderr, "getgrnam: %s\n", strerror(errno));
			return;
		}

		for (i = 0; grinfo->gr_mem[i] != NULL; i++) {
			fprintf(fp, "%s\r\n", grinfo->gr_mem[i]);
		}
	}

	fflush(fp);
	fclose(fp);

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
		} else {
			usage();
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
			fprintf(stderr, "accept: %s\n", strerror(errno));
			continue;
		}

		pid = fork();

		if (pid == -1) {
			fprintf(stderr, "fork: %s\n", strerror(errno));
			continue;
		} else if (pid == 0) {
			close(fd);

			/* Set timeout to 10 seconds */
			timeout.tv_sec = 10;
			timeout.tv_usec = 0;
			if (setsockopt(req, SOL_SOCKET, SO_RCVTIMEO, &timeout,
						sizeof(timeout)) == -1) {
				fprintf(stderr, "setsockopt: %s\n", strerror(errno));
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
