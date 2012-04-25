/* $Id: tui.c,v 1.4 2012/04/25 10:49:39 demon Exp $ */
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
dumpmem(unsigned short *m)
{
	int i, k, sum, lines = 0;

	wmove(code, 0, 0);

	for (lines = 0, i = 0; i < MEMSZ; i += 8) {
		for (sum = 0, k = 0; k < 8; k++)
			sum |= m[i + k];
		if (!sum)
			continue;
		if (++lines > 24)
			break;
		wprintw(code, "%4.4x:", i);
		for (k = 0; k < 8; k++)
			wprintw(code, "%5.4x", m[i + k]);
		wprintw(code, "\n");
	}

	wnoutrefresh(code);
}

void
dumpdisp(unsigned short *m)
{
	int i;
	char c, b, hbg, bg, hfg, fg, col;

	wmove(out, 0, 0);

	for (i = DISP; i < DISPEND; i++) {
		c = m[i] & 0x7f;

		b = (m[i] >> 7) & 0x01;
		bg = (m[i] >> 8) & 0x07;
		hbg = (m[i] >> 11) & 0x01;
		fg = (m[i] >> 12) & 0x07;
		hfg = (m[i] >> 15) & 0x01;

		col = (fg << 3) | bg;

		if (!(isascii(c) && isprint(c)))
			c = ' ';

		if (b)
			wattron(out, A_BLINK);
		if (hfg)
			wattron(out, A_BOLD);
		wattron(out, COLOR_PAIR(col));
		waddch(out, c);
		wattroff(out, COLOR_PAIR(col));
		if (hfg)
			wattroff(out, A_BOLD);
		if (b)
			wattroff(out, A_BLINK);
	}

	wnoutrefresh(out);
}

void
dumpreg(unsigned short *mem, unsigned short *reg)
{
	wmove(regs, 0, 0);
 
	mvwprintw(regs, 0,  0, " A: %4.4x [%4.4x]", reg[A], mem[reg[A]]);
	mvwprintw(regs, 1,  0, " B: %4.4x [%4.4x]", reg[B], mem[reg[B]]);
	mvwprintw(regs, 2,  0, " C: %4.4x [%4.4x]", reg[C], mem[reg[C]]);
 
	mvwprintw(regs, 3,  0, " X: %4.4x [%4.4x]", reg[X], mem[reg[X]]);
	mvwprintw(regs, 4,  0, " Y: %4.4x [%4.4x]", reg[Y], mem[reg[Y]]);
	mvwprintw(regs, 5,  0, " Z: %4.4x [%4.4x]", reg[Z], mem[reg[Z]]);

	mvwprintw(regs, 6,  0, " I: %4.4x [%4.4x]", reg[I], mem[reg[I]]);
	mvwprintw(regs, 7,  0, " J: %4.4x [%4.4x]", reg[J], mem[reg[J]]);

	mvwprintw(regs, 0, 16, "PC: %4.4x [%4.4x]", reg[PC], mem[reg[PC]]);
	mvwprintw(regs, 1, 16, "SP: %4.4x [%4.4x]", reg[SP], mem[reg[SP]]);
	mvwprintw(regs, 2, 16, " O: %4.4x [%4.4x]", reg[O], mem[reg[O]]);

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
tuiemu(unsigned short *m, unsigned short *r)
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

	m[KEYP] = KEYB;

	while (step(m, r) != -1) {
		dumpmem(m);
		dumpdisp(m);
		dumpreg(m, r);

		if ((ch = wgetch(stdscr)) != ERR) {
			m[m[KEYP]] = ch;
			m[KEYP] = KEYB + (m[KEYP] + 1) % 0x10;
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
