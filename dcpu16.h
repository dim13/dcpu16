/* $Id: dcpu16.h,v 1.18 2012/05/09 10:47:21 demon Exp $ */
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

#ifndef _DCPU16_H
#define _DCPU16_H

enum {	A, B, C, X, Y, Z, I, J, PC, SP, EX, IA,
	Aux, nReg };

enum {	EXT, SET, ADD, SUB, MUL, MLI, DIV, DVI,
	MOD, MDI, AND, BOR, XOR, SHR, ASR, SHL,
	IFB, IFC, IFE, IFN, IFG, IFA, IFL, IFU,
	ADX = 0x1a, SBX,
	STI = 0x1e, STD, nOpt };

enum {	NOP, JSR, BRK,
	HCF = 0x07, INT, IAG, IAS, RFI, IAQ,
	HWN = 0x10, HWQ, HWI, nExt };

#define MEMSZ	0x10000

#define DISP	0x8000
#define DISPEND	0x8180

#define CHARS	0x8180
#define CHAREND	0x8280

#define BORDER	0x8280

#define KEYB	0x9000
#define KEYP	0x9010

struct context {
	unsigned short mem[MEMSZ];
	unsigned short reg[nReg];
	struct device {
		unsigned int id;
		unsigned int version;
		unsigned int manu;
		void (*cb)(struct context *);
	} dev[MEMSZ];
	unsigned short ndev;
	unsigned short proglen;
	unsigned short screen;
	unsigned short font;
	unsigned short palette;
};

/* display: 32x12 (128x96) + 16 pixel boarder, font 8x4 */

int compile(FILE *, unsigned short *, size_t);
int step(struct context *);
void tuiemu(struct context *);
void guiemu(struct context *);
void dumpcode(struct context *);

void lem(struct context *);
void keyb(struct context *);
void clk(struct context *);

#endif
