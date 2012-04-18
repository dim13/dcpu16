# $Id: Makefile,v 1.5 2012/04/18 03:47:31 demon Exp $

PROG=	dcpu
SRCS=	gramar.y lexer.l emu.c main.c tui.c gui.c y.tab.h
NOMAN=
CFLAGS+=`sdl-config --cflags`
LDADD+=	`sdl-config --libs` -lcurses
DEBUG+=	-Wall
#DEBUG+=	-ggdb -pg
#YFLAGS=	-dtv

.include <bsd.prog.mk>
