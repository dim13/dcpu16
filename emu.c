/* $Id: emu.c,v 1.1 2012/04/16 19:07:07 demon Exp $ */
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

unsigned short *mem;
unsigned short *reg;

unsigned short skip = 0;
unsigned short run = 1;

void ext(unsigned short *a, unsigned short *b);
void set(unsigned short *a, unsigned short *b);
void add(unsigned short *a, unsigned short *b);
void sub(unsigned short *a, unsigned short *b);
void mul(unsigned short *a, unsigned short *b);
void div(unsigned short *a, unsigned short *b);
void mod(unsigned short *a, unsigned short *b);
void shl(unsigned short *a, unsigned short *b);
void shr(unsigned short *a, unsigned short *b);
void and(unsigned short *a, unsigned short *b);
void bor(unsigned short *a, unsigned short *b);
void xor(unsigned short *a, unsigned short *b);
void ife(unsigned short *a, unsigned short *b);
void ifn(unsigned short *a, unsigned short *b);
void ifg(unsigned short *a, unsigned short *b);
void ifb(unsigned short *a, unsigned short *b);

void (*op[nOpt])(unsigned short *a, unsigned short *b) = {
	[EXT] = ext,
	[SET] = set,
	[ADD] = add,
	[SUB] = sub,
	[MUL] = mul,
	[DIV] = div,
	[MOD] = mod,
	[SHL] = shl,
	[SHR] = shr,
	[AND] = and,
	[BOR] = bor,
	[XOR] = xor,
	[IFE] = ife,
	[IFN] = ifn,
	[IFG] = ifg,
	[IFB] = ifb,
};

void jsr(unsigned short *a);
void brk(unsigned short *a);

void (*extop[nExt])(unsigned short *a) = {
	[Res] = brk,	/* die on wrong opcode */
	[JSR] = jsr,
	[BRK] = brk,
};

void
ext(unsigned short *a, unsigned short *b)
{
	extop[*a](b);
}

void
set(unsigned short *a, unsigned short *b)
{
	*a = *b;
}

void
add(unsigned short *a, unsigned short *b)
{
	int tmp = *a;

	tmp += *b;
	reg[O] = tmp > 0xFFFF;

	*a = tmp;
}

void
sub(unsigned short *a, unsigned short *b)
{
	int tmp = *a;

	tmp -= *b;
	reg[O] = tmp < 0;

	*a = tmp;
}

void
mul(unsigned short *a, unsigned short *b)
{
	int tmp = *a;

	tmp *= *b;
	reg[O] = tmp >> 16;

	*a = tmp;
}

void
div(unsigned short *a, unsigned short *b)
{
	int tmp = *a;

	if (*b == 0) {
		reg[O] = 0;
		*a = 0;
	} else {
		reg[O] = ((tmp << 16) / *b);
		*a /= *b;
	}
}

void
mod(unsigned short *a, unsigned short *b)
{
	if (*b == 0)
		*a = 0;
	else
		*a %= *b;
}

void
shl(unsigned short *a, unsigned short *b)
{
	int tmp = *a;

	reg[O] = ((tmp << *b) >> 16);
	*a <<= *b;
}

void
shr(unsigned short *a, unsigned short *b)
{
	int tmp = *a;

	reg[O] = ((tmp << 16) >> *b);
	*a >>= *b;
}

void
and(unsigned short *a, unsigned short *b)
{
	*a &= *b;
}

void
bor(unsigned short *a, unsigned short *b)
{
	*a |= *b;
}

void
xor(unsigned short *a, unsigned short *b)
{
	*a ^= *b;
}

void
ife(unsigned short *a, unsigned short *b)
{
	skip = !(*a == *b);
}

void
ifn(unsigned short *a, unsigned short *b)
{
	skip = !(*a != *b);
}

void
ifg(unsigned short *a, unsigned short *b)
{
	skip = !(*a > *b);
}

void
ifb(unsigned short *a, unsigned short *b)
{
	skip = !(*a & *b);
}

void
jsr(unsigned short *a)
{
	mem[--reg[SP]] = reg[PC];
	reg[PC] = *a;
}

void
brk(unsigned short *a)
{
	run = 0;
}

unsigned short *
fetcharg(int a)
{
	switch (a) {
	case 0x00:
	case 0x01:
	case 0x02:
	case 0x03:
	case 0x04:
	case 0x05:
	case 0x06:
	case 0x07:
		/* register */
		return &reg[a];
	case 0x08:
	case 0x09:
	case 0x0a:
	case 0x0b:
	case 0x0c:
	case 0x0d:
	case 0x0e:
	case 0x0f:
		/* [register] */
		return &mem[reg[a - 0x08]];
	case 0x10:
	case 0x11:
	case 0x12:
	case 0x13:
	case 0x14:
	case 0x15:
	case 0x16:
	case 0x17:
		/* [next word + register] */
		return &mem[mem[reg[PC]++] + reg[a - 0x10]];
	case 0x18:
		/* pop */
		return &mem[reg[SP]++];
	case 0x19:
		/* peek */
		return &mem[reg[SP]];
	case 0x1a:
		/* push */
		return &mem[--reg[SP]];
	case 0x1b:
		/* SP */
		return &reg[SP];
	case 0x1c:
		/* PC */
		return &reg[PC];
	case 0x1d:
		/* O */
		return &reg[O];
	case 0x1e:
		/* [next word] */
		return &mem[mem[reg[PC]++]];
	case 0x1f:
		/* next word */
		return &mem[reg[PC]++];
	default:
		/* literal */
		reg[Aux] = a - 0x20;
		return &reg[Aux];
	}
}

int
step(unsigned short *m, unsigned short *r)
{
	unsigned short c, o, *a, *b, s;

	if (!run)
		return -1;

	mem = m;
	reg = r;

	c = mem[reg[PC]++];
	s = reg[SP];		/* store SP */

	o = c & 0x0f;
	reg[Aux] = (c >> 4) & 0x3f;
	/* don't fetch first arg for extended opcodes */
	a = o ? fetcharg(reg[Aux]) : &reg[Aux];
	b = fetcharg((c >> 10) & 0x3f);

	if (skip) {
		skip = 0;
		reg[SP] = s;	/* restore SP on skipped opcode */
	} else
		op[o](a, b);

	return 0;
}
