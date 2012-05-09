# $Id: Makefile,v 1.9 2012/05/09 00:03:40 demon Exp $

PROG=	dcpu
SRCS=	gramar.y lexer.l emu.c main.c tui.c gui.c y.tab.h \
	lem1802.c keyboard.c clock.c
NOMAN=
CFLAGS+=`sdl-config --cflags`
LDADD+=	`sdl-config --libs` -lcurses -lSDL_image
DEBUG+=	-Wall
DEBUG+=	-ggdb
#DEBUG+=	-pg
YFLAGS+=	-v

.include <bsd.prog.mk>
