/* $Id: main.c,v 1.1 2012/04/16 19:07:07 demon Exp $ */
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
#include <unistd.h>
#include "dcpu16.h"

void
dumpcode(unsigned short *m)
{
	int i, k, sum;

	for (i = 0; i < MEMSZ; i += 8) {
		sum = 0;
		for (k = 0; k < 8; k++)
			sum += m[i + k];
		if (!sum)
			continue;
		printf("%4.4x:", i);
		for (k = 0; k < 8; k++)
			printf("%5.4x", m[i + k]);
		printf("\n");
	}
}

void
usage(void)
{
	extern char *__progname;

	(void)fprintf(stderr, "Usage: %s [-gt]\n", __progname);
	exit(1);
}

int
main(int argc, char **argv)
{
	unsigned short *m;
	unsigned short r[nReg] = { 0 };
	FILE *fd;
	int e_flag = 0;
	int g_flag = 0;
	int ch;

	while ((ch = getopt(argc, argv, "gt")) != -1)
		switch (ch) {
		case 'g':
			g_flag = 1;
			break;
		case 't':
			e_flag = 1;
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

	m = compile(fd);
	fclose(fd);

	if (e_flag)
		tuiemu(m, r);
	else
		dumpcode(m);

	return 0;
}
