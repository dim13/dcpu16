/* $Id: gui.c,v 1.1 2012/04/18 03:47:31 demon Exp $ */
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
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "dcpu16.h"
#include "font.h"

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
Uint32 	rmask = 0xff000000;
Uint32 	gmask = 0x00ff0000;
Uint32 	bmask = 0x0000ff00;
Uint32 	amask = 0x000000ff;
#else
Uint32 	rmask = 0x000000ff;
Uint32 	gmask = 0x0000ff00;
Uint32 	bmask = 0x00ff0000;
Uint32 	amask = 0xff000000;
#endif

SDL_Surface *screen;

SDL_Rect disp = { 0, 0, 640, 480 };
SDL_Rect out = { 64, 48, 640, 480 };

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
decodecolor(unsigned short u, SDL_Color **fg, SDL_Color **bg)
{
	*bg = &color[(u >> 8) & 0x0f];
	*fg = &color[(u >> 12) & 0x0f];
}

void
setpixel(SDL_Surface *s, int x, int y, SDL_Color *c)
{
	Uint32 *buf = (Uint32 *)s->pixels + y * (s->pitch >> 2) + x;
	Uint32 pixel = SDL_MapRGB(s->format, c->r, c->g, c->b);
	*buf = pixel;
}

void
fillrect(SDL_Surface *s, SDL_Rect *r, SDL_Color *c)
{
	SDL_FillRect(s, r, SDL_MapRGB(s->format, c->r, c->g, c->b));
}

SDL_Surface *
mkglyph(unsigned char ch, SDL_Color *fg, SDL_Color *bg, unsigned short *m)
{
	SDL_Surface *g;
	SDL_Color *c;
	int i;

	g = SDL_CreateRGBSurface(SDL_SWSURFACE, gl.w, gl.h, 32,
		rmask, gmask, bmask, amask);

	ch &= 0x7f;
	ch <<= 1;

	for (i = 0; i < 8; i++) {
		c = m[CHARS + ch] & (0x0100 << i) ? fg : bg;
		setpixel(g, 0, i, c);

		c = m[CHARS + ch] & (0x0001 << i) ? fg : bg;
		setpixel(g, 1, i, c);

		c = m[CHARS + ch + 1] & (0x0100 << i) ? fg : bg;
		setpixel(g, 2, i, c);

		c = m[CHARS + ch + 1] & (0x0001 << i) ? fg : bg;
		setpixel(g, 3, i, c);
	}
	
	return g;
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
	SDL_Surface *glyph;
	SDL_Rect to;
	SDL_Color *fg, *bg;
	SDL_Event event;
	int ch, x, y, k = 0, n = 0;

	screen = SDL_SetVideoMode(scr.w, scr.h, 32, SDL_SWSURFACE);
	setfont(m);

	to.w = gl.w;
	to.h = gl.h;

	while (step(m, r) != -1) {
		if (++n % 100)
			continue;

		decodecolor(m[MISC], &fg, &bg);

		fillrect(screen, &scr, bg);

		for (x = 0; x < 32; x++) {
			for (y = 0; y < 12; y++) {
				to.x = draw.x + x * to.w;
				to.y = draw.y + y * to.h;

				ch = m[DISP + y * 32 + x];

				decodecolor(ch, &fg, &bg);
				glyph = mkglyph(ch, fg, bg, m);

				SDL_BlitSurface(glyph, &gl, screen, &to);
				SDL_FreeSurface(glyph);
			}
		}

		SDL_Flip(screen);

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				goto leave;
			case SDL_KEYDOWN:
				m[KEYB + k] = event.key.keysym.sym;
				printf("%x\n", m[KEYB + k]);
				k = (k + 1) % 0x10;
				break;
			}
		}
	}

leave:
	SDL_FreeSurface(screen);
}
