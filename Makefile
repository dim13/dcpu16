# $Id: Makefile,v 1.4 2012/04/17 19:55:56 demon Exp $

PROG=	dcpu
SRCS=	gramar.y lexer.l emu.c main.c tui.c y.tab.h
NOMAN=
LDADD+=	-lcurses
DEBUG+=	-Wall
#DEBUG+=	-ggdb -pg
#YFLAGS=	-dtv

.include <bsd.prog.mk>
