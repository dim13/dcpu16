/* $Id: keyboard.c,v 1.1 2012/05/09 00:03:40 demon Exp $ */
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

#include <stdio.h>
#include "dcpu16.h"

enum {	KEYB_CLEAR,
	KEYB_STORE,
	KEYB_PRESSED,
	KEYB_INTR,
};

void
keyb(struct context *c)
{
	switch (c->reg[A]) {
	case KEYB_CLEAR:
		break;
	case KEYB_STORE:
		break;
	case KEYB_PRESSED:
		break;
	case KEYB_INTR:
		break;
	default:
		break;
	}
}

void
register_keyb(struct context *c)
{
	struct device *d = &c->dev[++c->ndev];

	d->id = 0x30cf7406;
	d->version = 0x1;
	d->manu = 0;
	d->cb = keyb;
}
