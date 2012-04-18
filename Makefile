# $Id: Makefile,v 1.6 2012/04/18 16:32:52 demon Exp $

PROG=	dcpu
SRCS=	gramar.y lexer.l emu.c main.c tui.c gui.c y.tab.h
NOMAN=
CFLAGS+=`sdl-config --cflags`
LDADD+=	`sdl-config --libs` -lcurses -lSDL_image
DEBUG+=	-Wall
#DEBUG+=	-ggdb -pg
#YFLAGS=	-dtv

.include <bsd.prog.mk>
