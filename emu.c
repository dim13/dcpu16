/* $Id: emu.c,v 1.15 2012/04/26 19:59:23 demon Exp $ */
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
#include <unistd.h>
#include "dcpu16.h"

static unsigned short *mem;
static unsigned short *reg;

static unsigned short skip = 0;
static unsigned short run = 1;
static unsigned short cycle = 0;

void
nop(unsigned short *a)
{
	cycle += 1;
}

void
jsr(unsigned short *a)
{
	mem[--reg[SP]] = reg[PC];
	reg[PC] = *a;
	cycle += 3;
}

void
stop(unsigned short *a)
{
	run = 0;
	cycle += 1;
}

void
hcf(unsigned short *a)
{
	/* TODO */
	cycle += 9;
}

void
intr(unsigned short *a)
{
	/* TODO */
	cycle += 4;
}

void
iag(unsigned short *a)
{
	*a = reg[IA];
	cycle += 1;
}

void
ias(unsigned short *a)
{
	reg[IA] = *a;
	cycle += 1;
}

void
iap(unsigned short *a)
{
	if (reg[IA]) {
		mem[--reg[SP]] = reg[IA];
		reg[IA] = *a;
	}
	cycle += 3;
}

void
iaq(unsigned short *a)
{
	/* TODO */
	cycle += 2;
}

void
hwn(unsigned short *a)
{
	/* TODO */
	cycle += 2;
}

void
hwq(unsigned short *a)
{
	/* TODO */
	cycle += 4;
}

void
hwi(unsigned short *a)
{
	/* TODO */
	cycle += 4;
}

void (*extop[nExt])(unsigned short *a) = {
	[NOP] = nop,
	[JSR] = jsr,
	[BRK] = stop,
	[HCF] = hcf,
	[INT] = intr,
	[IAG] = iag,
	[IAS] = ias,
	[IAP] = iap,
	[IAQ] = iaq,
	[HWN] = hwn,
	[HWQ] = hwq,
	[HWI] = hwi,
};

void
ext(unsigned short *b, unsigned short *a)
{
	extop[*b](a);
}

void
set(unsigned short *b, unsigned short *a)
{
	*b = *a;
	cycle += 1;
}

void
add(unsigned short *b, unsigned short *a)
{
	int tmp = *b;

	tmp += *a;
	reg[EX] = (tmp > 0xFFFF) ? 0x0001 : 0;

	*b = tmp;
	cycle += 2;
}

void
sub(unsigned short *b, unsigned short *a)
{
	int tmp = *b;

	tmp -= *a;
	reg[EX] = (tmp < 0) ? 0xFFFF : 0;

	*b = tmp;
	cycle += 2;
}

void
mul(unsigned short *b, unsigned short *a)
{
	unsigned int tmp = *b;

	tmp *= *a;
	reg[EX] = tmp >> 16;

	*b = tmp;
	cycle += 2;
}

void
mli(unsigned short *b, unsigned short *a)
{
	int tmp = *b;

	tmp *= *a;
	reg[EX] = tmp >> 16;

	*b = tmp;
	cycle += 2;
}

void
div(unsigned short *b, unsigned short *a)
{
	unsigned int tmp = *b;

	if (*a == 0) {
		reg[EX] = 0;
		*b = 0;
	} else {
		reg[EX] = ((tmp << 16) / *a);
		*b /= *a;
	}
	cycle += 3;
}

void
dvi(unsigned short *b, unsigned short *a)
{
	int tmp = *b;

	if (*a == 0) {
		reg[EX] = 0;
		*b = 0;
	} else {
		reg[EX] = ((tmp << 16) / *a);
		*b /= *a;
	}
	cycle += 3;
}

void
mod(unsigned short *b, unsigned short *a)
{
	if (*a == 0)
		*b = 0;
	else
		*b %= *a;
	cycle += 3;
}

void
mdi(unsigned short *b, unsigned short *a)
{
	/* TODO */
	if (*a == 0)
		*b = 0;
	else
		*b %= *a;
	cycle += 3;
}

void
and(unsigned short *b, unsigned short *a)
{
	*b &= *a;
	cycle += 1;
}

void
bor(unsigned short *b, unsigned short *a)
{
	*b |= *a;
	cycle += 1;
}

void
xor(unsigned short *b, unsigned short *a)
{
	*b ^= *a;
	cycle += 1;
}

void
shr(unsigned short *b, unsigned short *a)
{
	int tmp = *b;

	/* TODO */

	reg[EX] = ((tmp << 16) >> *a);
	*b >>= *a;
	cycle += 2;
}

void
asr(unsigned short *b, unsigned short *a)
{
	int tmp = *b;
	int top = *b | 0x8000;

	/* TODO */

	reg[EX] = ((tmp << 16) >> *a);
	*b >>= *a;
	*b |= top;
	cycle += 2;
}

void
shl(unsigned short *b, unsigned short *a)
{
	int tmp = *b;

	/* TODO */

	reg[EX] = ((tmp << *a) >> 16);
	*b <<= *a;
	cycle += 2;
}

void
ifb(unsigned short *b, unsigned short *a)
{
	skip = !(*b & *a);
	cycle += 2 + skip;
}

void
ifc(unsigned short *b, unsigned short *a)
{
	skip = (*b & *a);
	cycle += 2 + skip;
}

void
ife(unsigned short *b, unsigned short *a)
{
	skip = !(*b == *a);
	cycle += 2 + skip;
}

void
ifn(unsigned short *b, unsigned short *a)
{
	skip = (*b == *a);
	cycle += 2 + skip;
}

void
ifg(unsigned short *b, unsigned short *a)
{
	skip = !(*b > *a);
	cycle += 2 + skip;
}

void
ifa(unsigned short *b, unsigned short *a)
{
	skip = !((signed short)*b > (signed short)*a);
	cycle += 2 + skip;
}

void
ifl(unsigned short *b, unsigned short *a)
{
	skip = !(*b < *a);
	cycle += 2 + skip;
}

void
ifu(unsigned short *b, unsigned short *a)
{
	skip = !((signed short)*b < (signed short)*a);
	cycle += 2 + skip;
}

void
adx(unsigned short *b, unsigned short *a)
{
	int tmp = *b;

	/* TODO */
	tmp += *a + reg[EX];
	reg[EX] = tmp > 0xFFFF ? 0x0001 : 0;
	cycle += 3;
}

void
sbx(unsigned short *b, unsigned short *a)
{
	int tmp = *b;

	/* TODO */
	tmp -= *a + reg[EX];
	reg[EX] = tmp < 0 ? 0x0001 : 0;
	cycle += 3;
}


void
sti(unsigned short *b, unsigned short *a)
{
	*b = *a;
	++reg[I];
	++reg[J];
	cycle += 2;
}

void
std(unsigned short *b, unsigned short *a)
{
	*b = *a;
	--reg[I];
	--reg[J];
	cycle += 2;
}

void (*op[nOpt])(unsigned short *a, unsigned short *b) = {
	[EXT] = ext,
	[SET] = set,
	[ADD] = add,
	[SUB] = sub,
	[MUL] = mul,
	[MLI] = mli,
	[DIV] = div,
	[DVI] = dvi,
	[MOD] = mod,
	[MDI] = mdi,
	[AND] = and,
	[BOR] = bor,
	[XOR] = xor,
	[SHR] = shr,
	[ASR] = asr,
	[SHL] = shl,
	[STI] = sti,
	[IFB] = ifb,
	[IFC] = ifc,
	[IFE] = ife,
	[IFN] = ifn,
	[IFG] = ifg,
	[IFA] = ifa,
	[IFU] = ifu,
	[IFL] = ifl,
	[ADX] = adx,
	[SBX] = sbx,
	[STI] = sti,
	[STD] = std,
};

unsigned short *
fetcharg(int a, int barg)
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
		cycle += 1;
		return &mem[mem[reg[PC]++] + reg[a - 0x10]];
	case 0x18:
		/* TODO push or pop */
		if (barg)
			return &mem[--reg[SP]];	/* push */
		else
			return &mem[reg[SP]++];	/* pop */
	case 0x19:
		/* peek */
		return &mem[reg[SP]];
	case 0x1a:
		/* pick */
		return &mem[reg[SP] + reg[PC]++];
	case 0x1b:
		/* SP */
		return &reg[SP];
	case 0x1c:
		/* PC */
		return &reg[PC];
	case 0x1d:
		/* EX */
		return &reg[EX];
	case 0x1e:
		/* [next word] */
		cycle += 1;
		return &mem[mem[reg[PC]++]];
	case 0x1f:
		/* next word */
		cycle += 1;
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
	cycle = 0;

	c = mem[reg[PC]++];
	s = reg[SP];		/* store SP */

	o = c & 0x1f;
		
	/* don't fetch first arg for extended opcodes */
	reg[Aux] = (c >> 5) & 0x1f;
	b = o ? fetcharg(reg[Aux], 1) : &reg[Aux];
	a = fetcharg((c >> 10) & 0x3f, 0);

	if (skip) {
		skip = 0;
		reg[SP] = s;	/* restore SP on skipped opcode */
	} else
		op[o](b, a);

	usleep(10 * cycle);	/* 100kHz */

	return cycle;
}
