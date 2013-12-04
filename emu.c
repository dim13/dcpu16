/* $Id: emu.c,v 1.31 2013/12/04 06:13:35 demon Exp $ */
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
#include <unistd.h>
#include "dcpu16.h"

static unsigned short skip = 0;
static unsigned short run = 1;
static unsigned short cycle = 0;
static int errors = 0;

void
nop(struct context *c, unsigned short *a)
{
	cycle += 1;
}

void
jsr(struct context *c, unsigned short *a)
{
	c->mem[--c->reg[SP]] = c->reg[PC];
	c->reg[PC] = *a;
	cycle += 3;
}

void
stop(struct context *c, unsigned short *a)
{
	run = 0;
	cycle += 1;
}

void
hcf(struct context *c, unsigned short *a)
{
	/* TODO: halt catch fire */
	cycle += 9;
}

void
intr(struct context *c, unsigned short *a)
{
	/* TODO */
	cycle += 4;
}

void
iag(struct context *c, unsigned short *a)
{
	*a = c->reg[IA];
	cycle += 1;
}

void
ias(struct context *c, unsigned short *a)
{
	c->reg[IA] = *a;
	cycle += 1;
}

void
rfi(struct context *c, unsigned short *a)
{
	/* TODO */
	*a = c->mem[c->reg[SP]++];
	c->reg[PC] = c->mem[c->reg[SP]++];
	cycle += 3;
}

void
iaq(struct context *c, unsigned short *a)
{
	/* TODO */
	cycle += 2;
}

void
hwn(struct context *c, unsigned short *a)
{
	/* TODO */
	*a = c->ndev;
	cycle += 2;
}

void
hwq(struct context *c, unsigned short *a)
{
	struct device *d = &c->dev[*a];

	if (d) {
		c->reg[A] = d->id;
		c->reg[B] = d->id >> 16;
		c->reg[C] = d->version;
		c->reg[X] = d->manu;
		c->reg[Y] = d->manu >> 16;
	} else {
		warnx("invalid device %d", *a);
		errors++;
	}
	
	cycle += 4;
}

void
hwi(struct context *c, unsigned short *a)
{
	struct device *d = &c->dev[*a];

	if (d && d->cb) {
		d->cb(c);
	} else {
		warnx("invalid device %d", *a);
		errors++;
	}

	cycle += 4;
}

void (*extop[nExt])(struct context *, unsigned short *) = {
	[NOP] = nop,
	[JSR] = jsr,
	[BRK] = stop,
	[HCF] = hcf,
	[INT] = intr,
	[IAG] = iag,
	[IAS] = ias,
	[RFI] = rfi,
	[IAQ] = iaq,
	[HWN] = hwn,
	[HWQ] = hwq,
	[HWI] = hwi,
};

void
ext(struct context *c, unsigned short *b, unsigned short *a)
{
	if (extop[*b])
		extop[*b](c, a);
	else {
		warnx("wrong extended opcode 0x%x (0x%x)", *b, *a);
		++errors;
	}
}

void
set(struct context *c, unsigned short *b, unsigned short *a)
{
	*b = *a;
	cycle += 1;
}

void
add(struct context *c, unsigned short *b, unsigned short *a)
{
	int tmp = *b + *a;

	c->reg[EX] = tmp >> 16;
	*b = tmp;
	cycle += 2;
}

void
sub(struct context *c, unsigned short *b, unsigned short *a)
{
	int tmp = *b - *a;

	c->reg[EX] = tmp >> 16;
	*b = tmp;
	cycle += 2;
}

void
mul(struct context *c, unsigned short *b, unsigned short *a)
{
	int tmp = *b * *a;

	c->reg[EX] = tmp >> 16;
	*b = tmp;
	cycle += 2;
}

void
mli(struct context *c, unsigned short *b, unsigned short *a)
{
	int tmp = (signed short)*b * (signed short)*b;
	c->reg[EX] = tmp >> 16;
	*b = tmp;
	cycle += 2;
}

void
div(struct context *c, unsigned short *b, unsigned short *a)
{
	int tmp;

	if (*a == 0) {
		c->reg[EX] = 0;
		*b = 0;
	} else {
		tmp = ((unsigned int)*b << 16) / *a;
		c->reg[EX] = tmp;
		*b = tmp >> 16;
	}
	cycle += 3;
}

void
dvi(struct context *c, unsigned short *b, unsigned short *a)
{
	int tmp;

	if (*a == 0) {
		c->reg[EX] = 0;
		*b = 0;
	} else {
		tmp = ((signed int)*b << 16) / (signed short)*a;
		c->reg[EX] = tmp;
		*b = tmp >> 16;
	}
	cycle += 3;
}

void
mod(struct context *c, unsigned short *b, unsigned short *a)
{
	if (*a == 0)
		*b = 0;
	else
		*b %= *a;
	cycle += 3;
}

void
mdi(struct context *c, unsigned short *b, unsigned short *a)
{
	/* TODO */
	if (*a == 0)
		*b = 0;
	else
		*b = (signed short)*b % *a;
	cycle += 3;
}

void
and(struct context *c, unsigned short *b, unsigned short *a)
{
	*b &= *a;
	cycle += 1;
}

void
bor(struct context *c, unsigned short *b, unsigned short *a)
{
	*b |= *a;
	cycle += 1;
}

void
xor(struct context *c, unsigned short *b, unsigned short *a)
{
	*b ^= *a;
	cycle += 1;
}

void
shr(struct context *c, unsigned short *b, unsigned short *a)
{
	c->reg[EX] = (((unsigned int)*b << 16) >> *a);
	*b >>= *a;
	cycle += 1;
}

void
asr(struct context *c, unsigned short *b, unsigned short *a)
{
	c->reg[EX] = (((unsigned int)*b << 16) >> *a);
	*b = (signed short)*b >> *a;
	cycle += 1;
}

void
shl(struct context *c, unsigned short *b, unsigned short *a)
{
	c->reg[EX] = (((unsigned int)*b << *a) >> 16);
	*b <<= *a;
	cycle += 1;
}

void
ifb(struct context *c, unsigned short *b, unsigned short *a)
{
	skip = !(*b & *a);
	cycle += 2 + skip;
}

void
ifc(struct context *c, unsigned short *b, unsigned short *a)
{
	skip = (*b & *a);
	cycle += 2 + skip;
}

void
ife(struct context *c, unsigned short *b, unsigned short *a)
{
	skip = !(*b == *a);
	cycle += 2 + skip;
}

void
ifn(struct context *c, unsigned short *b, unsigned short *a)
{
	skip = (*b == *a);
	cycle += 2 + skip;
}

void
ifg(struct context *c, unsigned short *b, unsigned short *a)
{
	skip = !(*b > *a);
	cycle += 2 + skip;
}

void
ifa(struct context *c, unsigned short *b, unsigned short *a)
{
	skip = !((signed short)*b > (signed short)*a);
	cycle += 2 + skip;
}

void
ifl(struct context *c, unsigned short *b, unsigned short *a)
{
	skip = !(*b < *a);
	cycle += 2 + skip;
}

void
ifu(struct context *c, unsigned short *b, unsigned short *a)
{
	skip = !((signed short)*b < (signed short)*a);
	cycle += 2 + skip;
}

void
adx(struct context *c, unsigned short *b, unsigned short *a)
{
	int tmp = *b + *a + c->reg[EX];

	c->reg[EX] = tmp > 16;
	*b = tmp;
	cycle += 3;
}

void
sbx(struct context *c, unsigned short *b, unsigned short *a)
{
	int tmp = *b - *a + c->reg[EX];

	c->reg[EX] = tmp >> 16;
	*b = tmp;
	cycle += 3;
}

void
sti(struct context *c, unsigned short *b, unsigned short *a)
{
	*b = *a;
	++c->reg[I];
	++c->reg[J];
	cycle += 2;
}

void
std(struct context *c, unsigned short *b, unsigned short *a)
{
	*b = *a;
	--c->reg[I];
	--c->reg[J];
	cycle += 2;
}

void (*op[nOpt])(struct context *, unsigned short *, unsigned short *) = {
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
fetcharg(struct context *c, int a, int barg)
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
		return &c->reg[a];
	case 0x08:
	case 0x09:
	case 0x0a:
	case 0x0b:
	case 0x0c:
	case 0x0d:
	case 0x0e:
	case 0x0f:
		/* [register] */
		return &c->mem[c->reg[a - 0x08]];
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
		return &c->mem[c->mem[c->reg[PC]++] + c->reg[a - 0x10]];
	case 0x18:
		/* TODO push or pop */
		if (barg)
			return &c->mem[--c->reg[SP]];	/* push */
		else
			return &c->mem[c->reg[SP]++];	/* pop */
	case 0x19:
		/* peek */
		return &c->mem[c->reg[SP]];
	case 0x1a:
		/* pick */
		return &c->mem[c->reg[SP] + c->reg[PC]++];
	case 0x1b:
		/* SP */
		return &c->reg[SP];
	case 0x1c:
		/* PC */
		return &c->reg[PC];
	case 0x1d:
		/* EX */
		return &c->reg[EX];
	case 0x1e:
		/* [next word] */
		cycle += 1;
		return &c->mem[c->mem[c->reg[PC]++]];
	case 0x1f:
		/* next word */
		cycle += 1;
		return &c->mem[c->reg[PC]++];
	default:
		/* literal */
		c->reg[Aux] = a - 0x20 - 1;
		return &c->reg[Aux];
	}
}

int
step(struct context *c)
{
	unsigned short ch, o, *a, *b, s, tmp;

	if (!run || errors > 3)
		return -1;

	cycle = 0;

	ch = c->mem[c->reg[PC]++];
	s = c->reg[SP];		/* store SP */

	o = ch & 0x1f;
	a = fetcharg(c, (ch >> 10) & 0x3f, 0);
	tmp = (ch >> 5) & 0x1f;
	b = o ? fetcharg(c, tmp, 1) : &tmp;

	if (skip) {
		skip = 0;
		c->reg[SP] = s;	/* restore SP on skipped opcode */
	} else {
		if (op[o])
			op[o](c, b, a);
		else {
			warnx("wrong opcode 0x%x (0x%x, 0x%x)", o, *b, *a);
			++errors;
		}
	}

	return cycle;
}
