#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "compiler.h"

static int fd = 0;
static char curr;		// current character

// read 1 byte from buffer; return as read system call
static int readchar() {
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

BEGIN:	readchar();
	if (isalpha(curr) || curr=='_')
		goto ID;
	if (isdigit(curr))
		goto NUM;
	if (isspace(curr))
		goto SPACE;
	goto SYM;
ID:	printf("ID: %c", curr);
	while (readchar()) {
		if (isalnum(curr) || curr=='_')
			printf("%c", curr);
		else {
			printf("\n");
			if (isspace(curr))
				goto SPACE;
			if (curr == '"')
				goto STR;
			goto SYM;
		}
	}
	goto END;
NUM:	printf("NUM: %c", curr);
	while (readchar()) {
		if (isdigit(curr))
			printf("%c", curr);
		else {
			printf("\n");
			if (isalpha(curr) || curr=='_')
				goto ID;
			if (isspace(curr))
				goto SPACE;
			goto SYM;
		}
	}
	goto END;
SYM:	printf("SYMBOL: %c", curr);
	if (curr == '"') {
		printf("\n");
		goto STR;
	}
	while (readchar()) {
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
			goto SPACE;
		}
		if (curr == '"') {
			printf("\n");
			goto SYM;
		}
		printf("%c", curr);
	}
	goto END;
STR:	printf("STRING: ");
	while (readchar()) {
		if (curr == '"') {
			printf("\nSYMBOL: %c\n", curr);
			goto BEGIN;
		}
		printf("%c", curr);
	}
	goto END;
SPACE:	while (readchar()) {
		if (isspace(curr))
			continue;
		if (isalpha(curr) || curr=='_')
			goto ID;
		if (isdigit(curr))
			goto NUM;
		goto SYM;
	}
	goto END;
END:	printf("\n: EOF\n");
}
