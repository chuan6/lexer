#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int	fd_src = 0;		// default to stdin
int	is_string = 0;
int	n_read;
char	curr;			// current character

void read_id() {
	while ((n_read=read(fd_src, &curr, 1)) == 1) {
		if (!isalnum(curr) && curr!='_')
			break;
		printf("%c", curr);
	}
	printf("\n");
}

void read_num() {
	while ((n_read=read(fd_src, &curr, 1)) == 1) {
		if (!isdigit(curr))
			break;
		printf("%c", curr);
	}
	printf("\n");
}

void read_sym() {
	while ((n_read=read(fd_src, &curr, 1)) == 1) {
		if (isspace(curr) || isalnum(curr) || curr=='_' || curr=='"')
			break;
		printf("%c", curr);
	}
	printf("\n");
}

int main(int argc, char* argv[])
{
	if (argc > 2) {
		fprintf(stderr, "Use \"prog [input file]\"\n");
		exit(1);
	}
	if (argc == 2) {
		fd_src = open(argv[1], O_RDONLY, 0);
		if (fd_src == -1) {
			fprintf(stderr, "Cannot open file \"%s\"\n", argv[1]);
			exit(1);
		}
	}

	while ((n_read=read(fd_src, &curr, 1)) == 1) {
		if (is_string) {
			if (curr == '"') {
				printf("\n");
				is_string = 0;
				printf("SYMBOL: %c\n", curr);
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
					printf("SYMBOL: %c\n", curr);
					
					is_string = 1;
					printf("STRING: ");
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

	return 0;
}
