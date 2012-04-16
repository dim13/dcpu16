/* $Id: gramar.y,v 1.1 2012/04/16 19:07:07 demon Exp $ */
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
 * WHATSOEVER RESULTING FROM LOSS OF USE, dATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

%{
#include <sys/queue.h>
#include <stdio.h>
#include <string.h>
#include "dcpu16.h"

extern int yylineno;
extern FILE *yyin;
int yylex(void);
int yyparse(void);

#if YYDEBUG
extern int yydebug;
#endif

struct {
	unsigned short val;
	char *label;
} stack[0x100], ref[0x100];

int sp = 0;
int rp = 0;
int pc = 0;

unsigned short buffer[MEMSZ];
char *label[MEMSZ];

%}

%union {
	int ival;
	char *sval;
};

%token A B C X Y Z I J
%token POP PEEK PUSH SP PC O
%token SET ADD SUB MUL DIV MOD SHL SHR AND BOR XOR IFE IFN IFG IFB
%token DAT JSR BRK ORG JMP
%token LBR RBR LBRACE RBRACE LPAR RPAR
%token COMMA DP PLUS MINUS MULT
%token DOT HASH MACRO INCLUDE
%token <ival> NUMBER
%token <sval> STRING QSTRING

%type <ival> register opcode extended value expr

%left PLUS MINUS
%left MULT

%%

prog	
	: /* empty */
	| prog statement
	;

statement
	: opcode value COMMA value
	{
				popop(($4 << 10) | ($2 << 4) | $1);
				popall();
	}
	| opcode value
	{
				popop(($2 << 10) | $1);
				popall();
	}
	| opcode		{ popop($1); }
	| DP STRING		{ addref($2); }
	| DAT data		{ popall(); }
	| ORG expr		{ pc = $2; }
	;

data
	: /* empty */
	| data block
	| data COMMA block
	;

expr
	: NUMBER		{ $$ = $1; }
	| expr PLUS expr	{ $$ = $1 + $3; }
	| expr MINUS expr	{ $$ = $1 - $3; }
	| expr MULT expr	{ $$ = $1 * $3; }
	| LPAR expr RPAR	{ $$ = $2; }
	;

block
	: QSTRING
	{
				char *s = $1;
				while (*s)
					push(*s++, NULL);
	}
	| STRING		{ push(0, $1); }
	| expr			{ push($1, NULL); }
	;

value
	: register		{ $$ = $1; }
	| LBR register RBR	{ $$ = 0x08 + $2; }
	| POP			{ $$ = 0x18; }
	| PEEK			{ $$ = 0x19; }
	| PUSH			{ $$ = 0x1a; }
	| SP			{ $$ = 0x1b; }
	| PC			{ $$ = 0x1c; }
	| O			{ $$ = 0x1d; }
	| LBR expr RBR
	{
				$$ = 0x1e;
				push($2, NULL);
	}
	| LBR STRING RBR
	{
				$$ = 0x1e;
				push(0, $2);
	}
	| LBR expr PLUS register RBR
	{
				$$ = 0x10 + $4;
				push($2, NULL);
	}
	| LBR register PLUS expr RBR
	{
				$$ = 0x10 + $2;
				push($4, NULL);
	}
	| LBR STRING PLUS register RBR
	{
				$$ = 0x10 + $4;
				push(0, $2);
	}
	| LBR register PLUS STRING RBR
	{
				$$ = 0x10 + $2;
				push(0, $4);
	}
	| expr
	{
				if ($1 < 0x20)
					$$ = 0x20 + $1;
				else {
					$$ = 0x1f;
					push($1, NULL);
				}
	}
	| STRING
	{
				$$ = 0x1f;
				push(0, $1);
	}
	;

register
	: A			{ $$ = 0x00; }
	| B			{ $$ = 0x01; }
	| C			{ $$ = 0x02; }
	| X			{ $$ = 0x03; }
	| Y			{ $$ = 0x04; }
	| Z			{ $$ = 0x05; }
	| I			{ $$ = 0x06; }
	| J			{ $$ = 0x07; }
	; 

opcode	
	: extended		{ $$ = $1 << 4; }
	| SET			{ $$ = 0x01; }
	| ADD			{ $$ = 0x02; }
	| SUB			{ $$ = 0x03; }
	| MUL			{ $$ = 0x04; }
	| DIV			{ $$ = 0x05; }
	| MOD			{ $$ = 0x06; }
	| SHL			{ $$ = 0x07; }
	| SHR			{ $$ = 0x08; }
	| AND			{ $$ = 0x09; }
	| BOR			{ $$ = 0x0a; }
	| XOR			{ $$ = 0x0b; }
	| IFE			{ $$ = 0x0c; }
	| IFN			{ $$ = 0x0d; }
	| IFG			{ $$ = 0x0e; }
	| IFB			{ $$ = 0x0f; }
	;

extended
	: JSR			{ $$ = 0x01; }
	| BRK			{ $$ = 0x02; }
	;


%%

void
yyerror(const char *s)
{
	fprintf(stderr, "Line %d: %s\n", yylineno, s);
	exit(1);
}

void
push(int i, char *s)
{
	stack[sp].val = i;
	stack[sp++].label = s;
}

void
popop(int n)
{
	buffer[pc++] = n;
}

void
popall(void)
{
	int n = sp;

	while (sp > 0) {
		buffer[pc] = stack[n - sp].val;
		label[pc++] = stack[n - sp--].label;
	}
}

void
addref(char *s)
{
	ref[rp].label = s;
	ref[rp++].val = pc;
}

int
findref(char *s)
{
	int i;

	for (i = 0; i < rp; i++)
		if (strcmp(ref[i].label, s) == 0)
			return ref[i].val;

	return 0;
}

void
restorerefs(void)
{
	int i;

	for (i = 0; i < pc; i++)
		if (label[i])
			buffer[i] = findref(label[i]);
}

unsigned short *
compile(FILE *fd)
{
#if YYDEBUG
	yydebug = 1;
#endif
	yyin = fd;
	yyparse();
	restorerefs();

	return buffer;
}
