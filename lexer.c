#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "compiler.h"

static int fd = 0;
static char curr;		// current character

// read 1 byte from buffer; return as read system call
static int read_char() {
	static int n;
	static char buf[BUFSIZ];
	static int i = 0;

	for (;;) {
		if (i == 0) {
			if ((n=read(fd, buf, BUFSIZ)) < 0) {
				fprintf(stderr, "read_char failed\n");
				exit(EXIT_FAILURE);
			}
			if (n == 0) {
				curr = EOF;
				return 0;
			}
		}
		if (i < n) {
			curr = buf[i++];
			return 1;
		}
		// resets i if program reaches here
		i = 0;
	}
}

void LexAnalyze(int fd_src) {
	fd = fd_src;

	if (fd < 0) {
		fprintf(stderr, "negative file descriptor encountered\n");
		exit(EXIT_FAILURE);
	}

BEGIN:	while (read_char()) {
		if (isalpha(curr) || curr=='_')
			goto ID;
		if (isdigit(curr))
			goto NUM;
		if (isspace(curr))
			continue;
		goto SYM;
	}
	goto END;
ID:	printf("ID: %c", curr);
	while (read_char()) {
		if (isalnum(curr) || curr=='_')
			printf("%c", curr);
		else {
			printf("\n");
			if (isspace(curr))
				goto BEGIN;
			if (curr == '"')
				goto STR;
			goto SYM;
		}
	}
	goto END;
NUM:	printf("NUM: %c", curr);
	while (read_char()) {
		if (isdigit(curr))
			printf("%c", curr);
		else {
			printf("\n");
			if (isalpha(curr) || curr=='_')
				goto ID;
			if (isspace(curr))
				goto BEGIN;
			goto SYM;
		}
	}
	goto END;
SYM:	printf("SYMBOL: %c", curr);
	if (curr == '"') {
		printf("\n");
		goto STR;
	}
	while (read_char()) {
		if (isalpha(curr) || curr=='_') {
			printf("\n");
			goto ID;
		}
		if (isdigit(curr)) {
			printf("\n");
			goto NUM;
		}
		if (isspace(curr)) {
			printf("\n");
			goto BEGIN;
		}
		if (curr == '"') {
			printf("\n");
			goto SYM;
		}
		printf("%c", curr);
	}
	goto END;
STR:	printf("STRING: ");
	while (read_char()) {
		if (curr == '"') {
			printf("\nSYMBOL: %c\n", curr);
			goto BEGIN;
		}
		printf("%c", curr);
	}
	goto END;
END:	printf("\n: EOF\n");
}
