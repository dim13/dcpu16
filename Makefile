# $Id: Makefile,v 1.7 2012/04/25 10:11:35 demon Exp $

PROG=	dcpu
SRCS=	gramar.y lexer.l emu.c main.c tui.c gui.c y.tab.h
NOMAN=
CFLAGS+=`sdl-config --cflags`
LDADD+=	`sdl-config --libs` -lcurses -lSDL_image
DEBUG+=	-Wall
#DEBUG+=	-ggdb -pg
YFLAGS+=	-v

.include <bsd.prog.mk>
