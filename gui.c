/* $Id: gui.c,v 1.21 2013/12/04 06:13:35 demon Exp $ */
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

#include <sys/time.h>
#include <SDL.h>
#include <SDL_image.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "dcpu16.h"

SDL_Surface *screen;
SDL_Surface *scratch;

SDL_Rect draw = { 16, 16, 128, 96 };
SDL_Rect scr = { 0, 0, 160, 128 };
SDL_Rect real = { 0, 0, 480, 384 };
SDL_Rect gl = { 0, 0, 4, 8 };

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
drawglyph(SDL_Surface *s, int x, int y, unsigned short *m, int usec)
{
	SDL_Rect to;
	Uint8 color[2], c, i;
	unsigned short ch, blink, *p;

	ch = m[DISP + y * 32 + x];
	color[0] = (ch >> 8) & 0x0f;		/* bg */
	color[1] = (ch >> 12) & 0x0f;		/* fg */
	blink = ch & 0x80;
	ch = (ch & 0x7f) << 1;
	p = &m[CHARS + ch];

	if (blink && (usec / 500000) % 2)
		color[1] = color[0];

	to.w = gl.w;
	to.h = gl.h;
	to.x = draw.x + x * to.w;
	to.y = draw.y + y * to.h;

	for (i = 0; i < gl.h; i++) {
		c = color[!!(*p & (0x0100 << i))];
		setpixel(s, to.x + 0, to.y + i, c);
		c = color[!!(*p & (0x0001 << i))];
		setpixel(s, to.x + 1, to.y + i, c);
		c = color[!!(*(p + 1) & (0x0100 << i))];
		setpixel(s, to.x + 2, to.y + i, c);
		c = color[!!(*(p + 1) & (0x0001 << i))];
		setpixel(s, to.x + 3, to.y + i, c);
	}
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
	int w, h, x, y, ch, i;
	unsigned short *p;

	SDL_Surface *img;
	SDL_Rect frame;

	img = IMG_Load(font);

	w = img->w / gl.w;
	h = img->h / gl.h;

	for (x = 0; x < w; ++x) {
		for (y = 0; y < h; ++y) {
			ch = 2 * (x + y * w);
			frame.x = x * gl.w;
			frame.y = y * gl.h;
			p = &m[CHARS + ch];
			for (i = 0; i < gl.h; i++) {
				if (getpixel(img, frame.x + 0, frame.y + i))
					*p |= (0x0100 << i);
				if (getpixel(img, frame.x + 1, frame.y + i))
					*p |= (0x0001 << i);
				if (getpixel(img, frame.x + 2, frame.y + i))
					*(p + 1) |= (0x0100 << i);
				if (getpixel(img, frame.x + 3, frame.y + i))
					*(p + 1) |= (0x0001 << i);
			}
		}
	}

	SDL_FreeSurface(img);
}

void
guiemu(struct context *c)
{
	int x, y, cnt, n = 0;
	struct timeval tv;

	screen = SDL_SetVideoMode(real.w, real.h, 8, SDL_HWSURFACE|SDL_HWPALETTE);
	SDL_SetColors(screen, color, 0, 16);

	scratch = SDL_CreateRGBSurface(SDL_HWSURFACE, scr.w, scr.h, 8, 0, 0, 0, 0);
	SDL_SetColors(scratch, color, 0, 16);

	atexit(SDL_Quit);

	loadfont(c->mem, "font.xpm");

	while ((cnt = step(c)) != -1) {
		if ((n += cnt) < 1000)
			continue;

		gettimeofday(&tv, NULL);

		n = 0;

		if (SDL_MUSTLOCK(screen) && SDL_LockSurface(screen))
			continue;

		SDL_FillRect(scratch, &scr, c->mem[BORDER] & 0x0f);

		for (x = 0; x < 32; x++)
			for (y = 0; y < 12; y++)
				drawglyph(scratch, x, y, c->mem, tv.tv_usec);

		SDL_SoftStretch(scratch, &scr, screen, &real);

		SDL_Flip(screen);

		if (SDL_MUSTLOCK(screen))
			SDL_UnlockSurface(screen);

		if (keyboard(c->mem) == -1)
			break;
	}

	SDL_FreeSurface(scratch);
	SDL_FreeSurface(screen);
}
