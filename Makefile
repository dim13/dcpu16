# $Id: Makefile,v 1.8 2012/04/27 04:27:36 demon Exp $

PROG=	dcpu
SRCS=	gramar.y lexer.l emu.c main.c tui.c gui.c y.tab.h
NOMAN=
CFLAGS+=`sdl-config --cflags`
LDADD+=	`sdl-config --libs` -lcurses -lSDL_image
DEBUG+=	-Wall
DEBUG+=	-ggdb
#DEBUG+=	-pg
YFLAGS+=	-v

.include <bsd.prog.mk>
