/*
 * Copyright (c) 2015, Quentin Schwerkolt
 * All rights reserved.
 *
 */

#include <err.h>
#include <stdio.h>
#include <stdlib.h>

#include "rtrack.h"

static void	usage(void);

int
main(int argc, char **argv)
{
	int idx;
	rtrack_t *rtrack;

	if (argc < 2)
		usage();

	for (idx = 1; idx < argc; ++idx) {
		rtrack = rtrack_new(argv[idx]);
		rtrack_analyse(rtrack);
		rtrack_free(rtrack);
	}
	return (0);
}

static void
usage(void)
{
	printf("Usage:\n"
	       "%s filename...\n", getprogname());
	exit(2);
}
