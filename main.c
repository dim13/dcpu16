/* $Id: main.c,v 1.9 2013/12/04 05:49:31 demon Exp $ */
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
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <SDL.h>
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
	int i, k;

	for (i = 0; i < c->proglen; i += 8) {
		printf("DAT ");
		for (k = 0; k < 8; k++) {
			printf("0x%.4x", c->mem[i + k]);
			if ((k + 1) % 8)
				printf(", ");
		}
		printf("; @ 0x%.4x\n", i);
	}
}

void
usage(void)
{
	extern char *__progname;

	(void)fprintf(stderr, "Usage: %s [-eg]\n", __progname);
	exit(1);
}

void
addhw(struct context *c,
	unsigned int id, unsigned short ver, unsigned int manu,
	void (*cb)(struct context *))
{
	struct device *d = &c->dev[c->ndev++];

	d->id = id;
	d->version = ver;
	d->manu = manu;
	d->cb = cb;
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
	if (!(c.proglen = compile(fd, c.mem, MEMSZ)))
		errx(1, "compilation errors");

	fclose(fd);

	addhw(&c, 0x7349f615, 0x1802, 0x1c6c8b36, lem);
	addhw(&c, 0x30cf7406, 1, 0, keyb);
	addhw(&c, 0x12d0b402, 1, 0, clk);

	emu(&c);

	return 0;
}
