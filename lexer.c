#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "compiler.h"

int	fd = 0;
int	is_within_str = 0;
int	n_read;			// expecting to be 0, or 1
char	curr;			// current character

void read_id() {
	while ((n_read=read(fd, &curr, 1)) == 1) {
		if (!isalnum(curr) && curr!='_')
			break;
		printf("%c", curr);
	}
	printf("\n");
}

void read_num() {
	while ((n_read=read(fd, &curr, 1)) == 1) {
		if (!isdigit(curr))
			break;
		printf("%c", curr);
	}
	printf("\n");
}

void read_sym() {
	while ((n_read=read(fd, &curr, 1)) == 1) {
		if (isspace(curr) || isalnum(curr) || curr=='_' || curr=='"')
			break;
		printf("%c", curr);
	}
	printf("\n");
}

void LexAnalyze(int fd_src) {
	fd = fd_src;

	if (fd < 0)
		return;

	while ((n_read=read(fd, &curr, 1)) == 1) {
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
			} while (!isspace(curr) && n_read==1);
		}
		fflush(stdout);
	}
	
	if (n_read == 0)
		printf("\n: EOF\n");
	else
		fprintf(stderr, "read file failure\n");
}


