/* $Id: tui.c,v 1.7 2012/05/08 20:37:03 demon Exp $ */
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

#include <ctype.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <unistd.h>
#include "dcpu16.h"

WINDOW	*outbox, *out, *regbox, *regs, *code;

void
dumpmem(struct context *c)
{
	int i, k, sum, lines = 0;

	wmove(code, 0, 0);

	for (lines = 0, i = 0; i < MEMSZ; i += 8) {
		for (sum = 0, k = 0; k < 8; k++)
			sum |= c->mem[i + k];
		if (!sum)
			continue;
		if (++lines > 24)
			break;
		wprintw(code, "%4.4x:", i);
		for (k = 0; k < 8; k++)
			wprintw(code, "%5.4x", c->mem[i + k]);
		wprintw(code, "\n");
	}

	wnoutrefresh(code);
}

void
dumpdisp(struct context *c)
{
	int i;
	char ch, b, hbg, bg, hfg, fg, col;

	wmove(out, 0, 0);

	for (i = DISP; i < DISPEND; i++) {
		ch = c->mem[i] & 0x7f;

		b = (c->mem[i] >> 7) & 0x01;
		bg = (c->mem[i] >> 8) & 0x07;
		hbg = (c->mem[i] >> 11) & 0x01;
		fg = (c->mem[i] >> 12) & 0x07;
		hfg = (c->mem[i] >> 15) & 0x01;

		col = (fg << 3) | bg;

		if (!(isascii(ch) && isprint(ch)))
			ch = ' ';

		if (b)
			wattron(out, A_BLINK);
		if (hfg)
			wattron(out, A_BOLD);
		wattron(out, COLOR_PAIR(col));
		waddch(out, ch);
		wattroff(out, COLOR_PAIR(col));
		if (hfg)
			wattroff(out, A_BOLD);
		if (b)
			wattroff(out, A_BLINK);
	}

	wnoutrefresh(out);
}

void
dumpreg(struct context *c)
{
	wmove(regs, 0, 0);
 
	mvwprintw(regs, 0,  0, " A: %4.4x [%4.4x]",
		c->reg[A], c->mem[c->reg[A]]);
	mvwprintw(regs, 1,  0, " B: %4.4x [%4.4x]",
		c->reg[B], c->mem[c->reg[B]]);
	mvwprintw(regs, 2,  0, " C: %4.4x [%4.4x]",
		c->reg[C], c->mem[c->reg[C]]);
 
	mvwprintw(regs, 3,  0, " X: %4.4x [%4.4x]",
		c->reg[X], c->mem[c->reg[X]]);
	mvwprintw(regs, 4,  0, " Y: %4.4x [%4.4x]",
		c->reg[Y], c->mem[c->reg[Y]]);
	mvwprintw(regs, 5,  0, " Z: %4.4x [%4.4x]",
		c->reg[Z], c->mem[c->reg[Z]]);

	mvwprintw(regs, 6,  0, " I: %4.4x [%4.4x]",
		c->reg[I], c->mem[c->reg[I]]);
	mvwprintw(regs, 7,  0, " J: %4.4x [%4.4x]",
		c->reg[J], c->mem[c->reg[J]]);

	mvwprintw(regs, 0, 16, "PC: %4.4x [%4.4x]",
		c->reg[PC], c->mem[c->reg[PC]]);
	mvwprintw(regs, 1, 16, "SP: %4.4x [%4.4x]",
		c->reg[SP], c->mem[c->reg[SP]]);
	mvwprintw(regs, 2, 16, "EX: %4.4x [%4.4x]",
		c->reg[EX], c->mem[c->reg[EX]]);

	wnoutrefresh(regs);
}

void
init_colors()
{
	int i, j;
	int I, J;

	for (i = 0; i < 8; i++)
		for (j = 0; j < 8; j++) {
			/* Notch is jerk, why RGB istead of BGR??? */
			I = (i >> 2) | (i & 0x02) | ((i << 2) & 0x04);
			J = (j >> 2) | (j & 0x02) | ((j << 2) & 0x04);
			init_pair((i << 3) | j, I, J);
		}
}

void
tuiemu(struct context *c)
{
	int ch;

	initscr();

	noecho();
	nodelay(stdscr, TRUE);

	start_color();
	init_colors();

	outbox = derwin(stdscr, 14, 34, 0, 46);
	out = derwin(outbox, 12, 32, 1, 1);
	box(outbox, 0, 0);
	wnoutrefresh(outbox);

	regbox = derwin(stdscr, 10, 34, 14, 46);
	regs = derwin(regbox, 8, 32, 1, 1);
	box(regbox, 0, 0);
	wnoutrefresh(regbox);

	code = derwin(stdscr, 24, 46, 0, 0);

	c->mem[KEYP] = KEYB;

	while (step(c) != -1) {
		dumpmem(c);
		dumpdisp(c);
		dumpreg(c);

		if ((ch = wgetch(stdscr)) != ERR) {
			c->mem[c->mem[KEYP]] = ch;
			c->mem[KEYP] = KEYB + (c->mem[KEYP] + 1) % 0x10;
		}

		doupdate();
	}

	delwin(code);
	delwin(regs);
	delwin(regbox);
	delwin(out);
	delwin(outbox);

	endwin();
}
