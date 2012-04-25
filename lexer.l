/* $Id: lexer.l,v 1.5 2012/04/25 23:06:58 demon Exp $ */
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

WS	[ \t]+
NL	[\r\n]+
STR	[A-Za-z0-9_]+
HEX	0x[0-9A-Fa-f]+
OCT	0[0-7]+
DEC	[0-9]+

%%

[Ss][Ee][Tt]		return SET;
[Aa][Dd][Dd]		return ADD;
[Ss][Uu][Bb]		return SUB;
[Mm][Uu][Ll]		return MUL;
[Mm][Ll][Ii]		return MLI;
[Dd][Ii][Vv]		return DIV;
[Dd][Vv][Ii]		return DVI;
[Mm][Oo][Dd]		return MOD;
[Aa][Nn][Dd]		return AND;
[Bb][Oo][Rr]		return BOR;
[Xx][Oo][Rr]		return XOR;
[Ss][Hh][Rr]		return SHR;
[Aa][Ss][Rr]		return ASR;
[Ss][Hh][Ll]		return SHL;
[Mm][Vv][Ii]		return MVI;
[Ii][Ff][Bb]		return IFB;
[Ii][Ff][Cc]		return IFC;
[Ii][Ff][Ee]		return IFE;
[Ii][Ff][Nn]		return IFN;
[Ii][Ff][Gg]		return IFG;
[Ii][Ff][Aa]		return IFA;
[Ii][Ff][Ll]		return IFL;
[Ii][Ff][Uu]		return IFU;
[Aa][Dd][Xx]		return ADX;
[Ss][Uu][Xx]		return SUX;

[Nn][Oo][Pp]		return NOP;
[Jj][Ss][Rr]		return JSR;
[Bb][Rr][Kk]		return BRK;
[Ii][Nn][Tt]		return INT;
[Ii][Aa][Gg]		return IAG;
[Ii][Aa][Ss]		return IAS;
[Hh][Ww][Nn]		return HWN;
[Hh][Ww][Qq]		return HWQ;
[Hh][Ww][Ii]		return HWI;

[Dd][Aa][Tt]		return DAT;
[Oo][Rr][Gg]		return ORG;

[Pp][Uu][Ss][Hh]	return PUSH;
[Pp][Oo][Pp]		return POP;
[Pp][Ee][Ee][Kk]	return PEEK;
[Pp][Ii][Cc][Kk]	return PICK;
[Ss][Pp]		return SP;
[Pp][Cc]		return PC;
[Ee][Xx]		return EX;

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
<INITIAL,COMMENT>{NL}	BEGIN(INITIAL);
<COMMENT>.		/* eat comments */

"\""			BEGIN(QUOTED);
<QUOTED>[^\"]+		{
				if (strlen(yytext) == 1) {
					yylval.ival = *yytext;
					return NUMBER;
				} else  {
					yylval.sval = strdup(yytext);
					return QSTRING;
				}
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
