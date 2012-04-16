/* $Id: lexer.l,v 1.1 2012/04/16 19:07:07 demon Exp $ */
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

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "y.tab.h"
%}

%option yylineno

%x COMMENT QUOTED

WS	[ \t\r\n]+
STR	[A-Za-z0-9_]+
HEX	0x[0-9A-Fa-f]+
OCT	0[0-7]+
DEC	[0-9]+

%%

[Ss][Ee][Tt]		return SET;
[Aa][Dd][Dd]		return ADD;
[Ss][Uu][Bb]		return SUB;
[Mm][Uu][Ll]		return MUL;
[Dd][Ii][Vv]		return DIV;
[Mm][Oo][Dd]		return MOD;
[Ss][Hh][Ll]		return SHL;
[Ss][Hh][Rr]		return SHR;
[Aa][Nn][Dd]		return AND;
[Bb][Oo][Rr]		return BOR;
[Xx][Oo][Rr]		return XOR;
[Ii][Ff][Ee]		return IFE;
[Ii][Ff][Nn]		return IFN;
[Ii][Ff][Gg]		return IFG;
[Ii][Ff][Bb]		return IFB;

[Jj][Ss][Rr]		return JSR;
[Bb][Rr][Kk]		return BRK;
[Dd][Aa][Tt]		return DAT;
[Oo][Rr][Gg]		return ORG;
[Jj][Mm][Pp]		return JMP;

[Pp][Oo][Pp]		return POP;
[Pp][Ee][Ee][Kk]	return PEEK;
[Pp][Uu][Ss][Hh]	return PUSH;
[Ss][Pp]		return SP;
[Pp][Cc]		return PC;
[Oo]			return O;

[Aa]			return A;
[Bb]			return B;
[Cc]			return C;
[Xx]			return X;
[Yy]			return Y;
[Zz]			return Z;
[Ii]			return I;
[Jj]			return J;

"+"			return PLUS;
"-"			return MINUS;
"*"			return MULT;
","			return COMMA;
":"			return DP;
"["			return LBR;
"]"			return RBR;
"("			return LPAR;
")"			return RPAR;
"{"			return LBRACE;
"}"			return RBRACE;
"#"			return HASH;
"."			return DOT;

[Mm][Aa][Cc][Rr][Oo]	return MACRO;
[Ii][Nn][Cc][Ll][Uu][Dd][Ee]	return INCLUDE;

";"			BEGIN(COMMENT);
<COMMENT>[\r\n]+	BEGIN(INITIAL);
<COMMENT>.		/* eat comments */

"\""			BEGIN(QUOTED);
<QUOTED>[^\"]+		{
				yylval.sval = strdup(yytext);
				return QSTRING;
			}
<QUOTED>"\""		BEGIN(INITIAL);

{HEX}|{OCT}|{DEC}	{
				yylval.ival = strtol(yytext, (char **)NULL, 0);
				return NUMBER;
			}

{STR}			{
				yylval.sval = strdup(yytext);
				return STRING;
			}

{WS}			/* eat whitespace */

%%

int
yywrap(void)
{
	return 1;
}
