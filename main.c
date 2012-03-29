#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "compiler.h"

int main(int argc, char* argv[])
{
	int fd_src = 0;

	if (argc == 1) {
		LexAnalyze(fd_src);
	} else if (argc > 1) {
		int i;
		for (i = 1; i < argc; i++) {
			fd_src = open(argv[i], O_RDONLY, 0);
			if (fd_src == -1) {
				fprintf(stderr, "Cannot open file \"%s\"\n", argv[i]);
				exit(EXIT_FAILURE);
			}
			LexAnalyze(fd_src);
			close(fd_src);
		}
	} else {
		fprintf(stderr, "Open file error\n");
		exit(EXIT_FAILURE);
	}

	return 0;
}
