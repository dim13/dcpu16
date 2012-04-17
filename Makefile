# $Id: Makefile,v 1.3 2012/04/17 19:54:01 demon Exp $

PROG=	dcpu
SRCS=	gramar.y lexer.l emu.c main.c tui.c y.tab.h
NOMAN=
LDADD+=	-lcurses
DEBUG=	-Wall -ggdb -pg
#YFLAGS=	-dtv

.include <bsd.prog.mk>
