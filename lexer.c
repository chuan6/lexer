#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "compiler.h"

static int fd = 0;
static char curr;		// current character
static const int n_state = 5;
enum state {	// do NOT change the sequence
	ALPHA = 0,
	DIGIT,
	SPACE,			// defined the same as of isspace()
	QUOTE,
	USCORE,
	SYMBOL			// currently, negatively defined
};
static int state_arr[] = {// -1, unknown; 0, negative; >0, positive
	-1,			// ALPHA
	-1,			// DIGIT
	-1,			// SPACE
	-1,			// QUOTE
	-1,			// USCORE
	-1			// SYMBOL
};

static void reset_state() {
	state_arr[5]
	= state_arr[4]
	= state_arr[3]
	= state_arr[2]
	= state_arr[1]
	= state_arr[0]
	= -1;
}

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

static int is_space() {
	return state_arr[SPACE] = isspace(curr);
}

static int is_digit() {
	return state_arr[DIGIT] = isdigit(curr);
}

static int is_alpha() {
	return state_arr[ALPHA] = isalpha(curr);
}

static int is_uscore() {
	return state_arr[USCORE] = (curr=='_');
}

static int is_quote() {
	return state_arr[QUOTE] = (curr=='"');
}

static void read_id() {
	while (readchar()) {
		if (is_alpha() || is_digit() || is_uscore())
			printf("%c", curr);
		else
			break;
	}
	printf("\n");
}

static void read_num() {
	while (readchar()) {
		if (is_digit())
			printf("%c", curr);
		else
			break;
	}
	printf("\n");
}

static void read_sym() {
	while (readchar()) {
		if (is_space()
		 || is_alpha()
		 || is_digit()
		 || is_uscore()
		 || is_quote())
			break;
		printf("%c", curr);
	}
	printf("\n");
}

static int check_state() {//printf("check_state() ...\n");
	int i;

	for (i = 0; i < n_state; i++)
		if (state_arr[i] > 0)
			goto RETURN;

	i = 0;
	if (state_arr[i] == -1 && is_alpha())
		goto RETURN;
	i++;
	if (state_arr[i] == -1 && is_digit())
		goto RETURN;
	i++;
	if (state_arr[i] == -1 && is_space())
		goto RETURN;
	i++;
	if (state_arr[i] == -1 && is_quote())
		goto RETURN;
	i++;
	if (state_arr[i] == -1 && is_uscore())
		goto RETURN;
	i++;

RETURN:	reset_state();
	return i;
}

// read untile SPACE or QUOTE is encountered
static void read_til_sq(int* is_instring) {
	static int i;

	for (;;) {
		i = check_state();
		switch (i) {
		case ALPHA: case USCORE:
			printf("ID: %c", curr);
			read_id();
			break;
		case DIGIT:
			printf("NUMBER: %c", curr);
			read_num();
			break;
		case QUOTE:
			*is_instring = 1;
			printf("SYMBOL: %c\nSTRING: ", curr);
			return;
		case SPACE:
			return;
		case SYMBOL:
			printf("SYMBOL: %c", curr);
			read_sym();
			break;
		default:
			fprintf(stderr, "unexpected state\n");
			exit(EXIT_FAILURE);
		}
	}
}

void LexAnalyze(int fd_src) {
	fd = fd_src;

	if (fd < 0) {
		fprintf(stderr, "negative file descriptor encountered\n");
		exit(EXIT_FAILURE);
	}

	int is_instring = 0;
	while (readchar()) {
		if (is_instring) {
			if (curr == '"') {
				is_instring = 0;
				printf("\nSYMBOL: %c\n", curr);
			} else {
				printf("%c", curr);
			}
		} else {
			read_til_sq(&is_instring);
		}
		fflush(stdout);
	}
	
	printf("\n: EOF\n");
}
