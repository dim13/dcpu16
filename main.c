/* $Id: main.c,v 1.6 2012/05/08 20:37:03 demon Exp $ */
/*
 * Copyright (c) 2012 Dimitri Sokolyuk <demon@dim13.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, dATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "dcpu16.h"

void (*emu)(struct context *);

void
dumpcode(struct context *c)
{
	int i, k, sum;

	for (i = 0; i < MEMSZ; i += 8) {
		sum = 0;
		for (k = 0; k < 8; k++)
			sum += c->mem[i + k];
		if (!sum)
			continue;
		printf("%4.4x:", i);
		for (k = 0; k < 8; k++)
			printf("%5.4x", c->mem[i + k]);
		printf("\n");
	}
}

void
usage(void)
{
	extern char *__progname;

	(void)fprintf(stderr, "Usage: %s [-eg]\n", __progname);
	exit(1);
}

int
main(int argc, char **argv)
{
	struct context c;
	FILE *fd;
	int ch;

	emu = dumpcode;

	while ((ch = getopt(argc, argv, "eg")) != -1)
		switch (ch) {
		case 'e':
			emu = tuiemu;
			break;
		case 'g':
			emu = guiemu;
			break;
		default:
			usage();
			/* NOTREACHED */
		}

	argc -= optind;
	argv += optind;

	if (!argc)
		usage();

	fd = fopen(*argv, "r");
	if (!fd)
		err(1, "cannot open file");

	bzero(&c, sizeof(c));
	if (compile(fd, c.mem, MEMSZ))
		errx(1, "compilation errors");

	fclose(fd);

	emu(&c);

	return 0;
}
