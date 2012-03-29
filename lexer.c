#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "compiler.h"

static	int	fd = 0;
static	int	is_within_str = 0;
static	char	curr;			// current character

// read 1 byte from buffer
static int readchar(char* c) {
	static int n;
	static char buf[BUFSIZ];
	static int i = 0;

	for (;;) {
		if (i == 0) {
			if ((n=read(fd, buf, BUFSIZ)) < 0) {
				fprintf(stderr, "readchar failed\n");
				exit(EXIT_FAILURE);
			}
			if (n == 0) {
				*c = EOF;
				return 0;
			}
		}
		if (i < n) {
			*c = buf[i++];
			return 1;
		}
		// resets i if program reaches here
		i = 0;
	}
}

static void read_id() {
	while (readchar(&curr)) {
		if (!isalnum(curr) && curr!='_')
			break;
		printf("%c", curr);
	}
	printf("\n");
}

static void read_num() {
	while (readchar(&curr)) {
		if (!isdigit(curr))
			break;
		printf("%c", curr);
	}
	printf("\n");
}

static void read_sym() {
	while (readchar(&curr)) {
		if (isspace(curr) || isalnum(curr) || curr=='_' || curr=='"')
			break;
		printf("%c", curr);
	}
	printf("\n");
}

void LexAnalyze(int fd_src) {
	fd = fd_src;

	if (fd < 0) {
		fprintf(stderr, "negative file descriptor encountered\n");
		exit(EXIT_FAILURE);
	}

	while (readchar(&curr)) {
		if (is_within_str) {
			if (curr == '"') {
				is_within_str = 0;
				printf("\nSYMBOL: %c\n", curr);
			} else {
				printf("%c", curr);
			}
		} else {
			do {
				if (curr=='_' || isalpha(curr)) {
					printf("ID: %c", curr);
					read_id();
				} else if (isdigit(curr)) {
					printf("NUMBER: %c", curr);
					read_num();
				} else if (curr == '"') {
					is_within_str = 1;
					printf("SYMBOL: %c\nSTRING: ", curr);
					break;
				} else if (!isspace(curr)) {
					printf("SYMBOL: %c", curr);
					read_sym();
				}
			} while (!isspace(curr));
		}
		fflush(stdout);
	}
	
	printf("\n: EOF\n");
}
