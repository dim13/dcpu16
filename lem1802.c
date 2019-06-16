/* $Id: lem1802.c,v 1.2 2012/05/09 01:34:43 demon Exp $ */
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

#include <stdio.h>
#include "dcpu16.h"

enum {	MEM_MAP_SCREEN,
	MEM_MAP_FONT,
	MEM_MAP_PALETTE,
	SET_BORDER_COLOR,
	MEM_DUMP_FONT,
	MEM_DUMP_PALETTE,
};

void
lem(struct context *c)
{
	switch (c->reg[A]) {
	case MEM_MAP_SCREEN:
		break;
	case MEM_MAP_FONT:
		break;
	case MEM_MAP_PALETTE:
		break;
	case SET_BORDER_COLOR:
		break;
	case MEM_DUMP_FONT:
		break;
	case MEM_DUMP_PALETTE:
		break;
	default:
		break;
	}
}
