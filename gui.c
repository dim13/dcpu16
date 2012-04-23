/* $Id: gui.c,v 1.9 2012/04/23 02:14:45 demon Exp $ */
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

#include <SDL.h>
#include <SDL_image.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "dcpu16.h"
#include "font.h"

SDL_Surface *screen;

SDL_Rect draw = { 16, 12, 128, 96 };
SDL_Rect scr = { 0, 0, 160, 120 };
SDL_Rect gl = { 0, 0, 4, 8 };
SDL_Color black = { 0x00, 0x00, 0x00 };
SDL_Color white = { 0xff, 0xff, 0xff };

SDL_Color color[0x10] = {
	{ 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0xaa },
	{ 0x00, 0xaa, 0x00 },
	{ 0x00, 0xaa, 0xaa },
	{ 0xaa, 0x00, 0x00 },
	{ 0xaa, 0x00, 0xaa },
	{ 0xaa, 0x55, 0x00 },
	{ 0xaa, 0xaa, 0xaa },
	{ 0x55, 0x55, 0x55 },
	{ 0x55, 0x55, 0xff },
	{ 0x55, 0xff, 0x55 },
	{ 0x55, 0xff, 0xff },
	{ 0xff, 0x55, 0x55 },
	{ 0xff, 0x55, 0xff },
	{ 0xff, 0xff, 0x55 },
	{ 0xff, 0xff, 0xff },
};

void
setpixel(SDL_Surface *s, int x, int y, Uint8 c)
{
	Uint8 *p = (Uint8 *)s->pixels + y * s->pitch + x;

	*p = c;
}

Uint8
getpixel(SDL_Surface *s, int x, int y)
{
	Uint8 *p = (Uint8 *)s->pixels + y * s->pitch + x;

	return *p;
}

void
drawglyph(SDL_Surface *screen, int x, int y, unsigned short *m)
{
	SDL_Rect to;
	Uint8 bg, fg, c, i;
	unsigned short ch;

	ch = m[DISP + y * 32 + x];
	bg = (ch >> 8) & 0x0f;
	fg = (ch >> 12) & 0x0f;
	ch = (ch & 0x7f) << 1;

	to.w = gl.w;
	to.h = gl.h;
	to.x = draw.x + x * to.w;
	to.y = draw.y + y * to.h;

	for (i = 0; i < 8; i++) {
		c = m[CHARS + ch] & (0x0100 << i) ? fg : bg;
		setpixel(screen, to.x + 0, to.y + i, c);

		c = m[CHARS + ch] & (0x0001 << i) ? fg : bg;
		setpixel(screen, to.x + 1, to.y + i, c);

		c = m[CHARS + ch + 1] & (0x0100 << i) ? fg : bg;
		setpixel(screen, to.x + 2, to.y + i, c);

		c = m[CHARS + ch + 1] & (0x0001 << i) ? fg : bg;
		setpixel(screen, to.x + 3, to.y + i, c);
	}
}

void
drawscreen(SDL_Surface *screen, unsigned short *m)
{
	int x, y;

	SDL_FillRect(screen, &scr, (m[MISC] >> 8) & 0x0f);

	for (x = 0; x < 32; x++)
		for (y = 0; y < 12; y++)
			drawglyph(screen, x, y, m);

	SDL_Flip(screen);
}

int
keyboard(unsigned short *m)
{
	SDL_Event event;
	static int k = 0;

	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
			return -1;
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
			case SDLK_UP:
				m[KEYB + k] = '&';	/* 38 */
				break;
			case SDLK_DOWN:
				m[KEYB + k] = '(';	/* 40 */
				break;
			case SDLK_LEFT:
				m[KEYB + k] = '%';	/* 37 */
				break;
			case SDLK_RIGHT:
				m[KEYB + k] = '\'';	/* 39 */
				break;
			case SDLK_RETURN:
				m[KEYB + k] = '\n';	/* 10 */
				break;
			default:
				m[KEYB + k] = event.key.keysym.sym;
				break;
			}
			k = (k + 1) % 0x10;
			break;
		}
	}

	return 0;
}

void
loadfont(unsigned short *m, char *font)
{
	int w, h, x, y, ch, i, p;

	SDL_Surface *img;
	SDL_Rect frame;

	img = IMG_Load(font);

	w = img->w / gl.w;
	h = img->h / gl.h;

	for (x = 0; x < img->h; x += 8) {
		for (y = 0; y < img->w; y += 4) {
			ch = 2 * (y / gl.w + x / gl.h * w);
			for (i = 0; i < gl.h; i++) {
				p = !!getpixel(img, x + i, y + 0);
				p = !!getpixel(img, x + i, y + 1);
				p = !!getpixel(img, x + i, y + 2);
				p = !!getpixel(img, x + i, y + 3);
			}
		}
	}

	SDL_FreeSurface(img);
}

void
setfont(unsigned short *m)
{
	int i;

	for (i = 0; i < 0x80; i++) {
		m[CHARS + 2 * i] = atari_small[i][0];
		m[CHARS + 2 * i + 1] = atari_small[i][1];
	}

	m[MISC] = 0x8000;	/* set default fg/bg */
}

void
guiemu(unsigned short *m, unsigned short *r)
{
	int n = 0;

	screen = SDL_SetVideoMode(scr.w, scr.h, 8, SDL_HWSURFACE|SDL_HWPALETTE);
	SDL_SetColors(screen, color, 0, 16);

	setfont(m);

	while (step(m, r) != -1) {
		if (++n % 100)
			continue;
		drawscreen(screen, m);
		if (keyboard(m) == -1)
			break;
	}

	SDL_FreeSurface(screen);
}
