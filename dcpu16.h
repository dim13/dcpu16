/* $Id: dcpu16.h,v 1.5 2012/04/24 18:16:32 demon Exp $ */
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

#ifndef _DCPU16_H
#define _DCPU16_H

enum { A, B, C, X, Y, Z, I, J, PC, SP, O, Aux, nReg };

enum { EXT, SET, ADD, SUB, MUL, DIV, MOD, SHL,
	SHR, AND, BOR, XOR, IFE, IFN, IFG, IFB, nOpt };

enum { NOP, JSR, BRK, nExt };

#define MEMSZ	0x10000

#define DISP	0x8000
#define DISPEND	0x8180

#define CHARS	0x8180
#define CHAREND	0x8280

#define BORDER	0x8280

#define KEYB	0x9000
#define KEYP	0x9010

/* display: 32x12 (128x96) + 16 pixel boarder, font 8x4 */

unsigned short *compile(FILE *, size_t);
int step(unsigned short *, unsigned short *);
void tuiemu(unsigned short *, unsigned short *);
void guiemu(unsigned short *, unsigned short *);
void dumpcode(unsigned short *, unsigned short *);

#endif
