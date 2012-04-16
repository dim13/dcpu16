# $Id: Makefile,v 1.1 2012/04/16 19:07:07 demon Exp $

PROG=	dcpu
SRCS=	gramar.y lexer.l emu.c main.c tui.c
NOMAN=
LDADD+=	-lcurses
#DEBUG=	-Wall -ggdb -pg
#YFLAGS=	-dtv

.include <bsd.prog.mk>
