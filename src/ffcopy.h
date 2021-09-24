#ifndef _FFCOPY_H_
#define _FFCOPY_H_

/* This Source Code is a modified variation on Donald Lewine's ffcopy.c from the
 * POSIX Programmers Guide. Credit is given to where it belongs. */

#define _POSIX_SOURCE 1

#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#include "panic.h"	/* define the PANIC macro */

#define HUNK_MAX INT_MAX
#define MAX_PATH 2048

void ffcopy(char * ifpath, char * ofpath) { /* NOTE: was previously an int function, and had no args */
	/* char ifpath[MAX_PATH+1]; */ /* name of input file */
	/* char ofpath[MAX_PATH+1]; */ /* name of output file */
	char scanf_string[10]; /* argument string for scanf */

	struct stat ifstat;
	char *bigbuf;
	int ifdes,ofdes; /* input/output file descriptors */
	size_t hunk; /*num of bytes to transfer in one piece */
	size_t left; /*num of bytes left left to transfer */

	/* initially wipe variables for security */
	memset(scanf_string, '\0', sizeof(scanf_string));
	ifdes = 0;
	ofdes= 0;

	/* Build the string "%2048s" */
	(void)sprintf(scanf_string, "%%%ds", MAX_PATH); /* NOTE: Do I need to change MAX_PATH here? */

	/* Get the input path */
	(void)printf("Input file: ");
	if (scanf(scanf_string, ifpath) != 1) PANIC;

	/* See if the file exists and how big it is */
	if (stat(ifpath, &ifstat) != 0) {
		(void)perror("? Cannot stat file");
		exit(EXIT_FAILURE);
	}
	left = hunk = ifstat.st_size; 
	if (hunk > HUNK_MAX) hunk = HUNK_MAX;

	/* Get a buffer for the whole file (or 1 hunk if the file is too big. */
	if((bigbuf = (char *)malloc(hunk)) == NULL) {
		(void)fprintf(stderr, "? File is too big for fast copy\n");
		exit(EXIT_FAILURE);
	}

	/* Open the input file */
	if ((ifdes = open(ifpath,O_RDONLY)) == -1) PANIC;
	/* Now that we have the input file open, ask for the path for the
	 * output file. */
	(void)printf("Output file: ");
	if(scanf(scanf_string,ofpath) != 1) PANIC;
	/* Open the output file */
	if ((ofdes = open(ofpath,O_WRONLY|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR)) == -1) {
		PANIC;
	}

	while (left > 0) {
		/* Read the file in one big bite */
		if (read(ifdes,bigbuf,hunk) != hunk) {
			(void)printf(stderr, "? error reading file %s\n", ifpath);
			exit(EXIT_FAILURE);
		}

		/* Write out the copy */
		if(write(ofdes,bigbuf,hunk) != hunk) {
			(void)fprintf(stderr, " Error writing file %s\n", ofpath);
			exit(EXIT_FAILURE);

		}

		left -= hunk; /* double check this for syntax error! */
		if (left < hunk) hunk = left;
	}

	/* Close the files */
	if (close(ifdes) != 0) PANIC;
	if (close(ofdes) != 0) PANIC;

	/* wipe variables after use */
	memset(scanf_string, '\0', sizeof(scanf_string));
	ifdes = 0;
	ofdes = 0;

	/* This code intentionally commented out. Intended to print
	 * status message.

	(void)printf("%s copied to %s (%d bytes)\n", ifpath,ofpath,ifstat.st_size); */

	return();
}

#endif
