/* $Id: gramar.y,v 1.25 2012/04/27 14:31:04 demon Exp $ */
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
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

extern int yylineno;
extern FILE *yyin;
int yylex(void);
int yyparse(void);
void yyerror(const char *, ...);
void push(int, char *);
void popop(int);
void popall(void);
void popallr(void);
void addref(char *);

#if YYDEBUG
extern int yydebug;
#endif

struct pair {
	unsigned short val;
	int lineno;
	char *label;
} *stack, *ref;

static int sp = 0;
static int rp = 0;
static int pc = 0;
static int haserrors = 0;

unsigned short *buffer;
struct label {
	char *label;
	int lineno;
} *label;

%}

%union {
	int ival;
	char *sval;
};

%token A B C X Y Z I J

%token PUSH POP PEEK PICK SP PC EX

%token SET ADD SUB MUL MLI DIV DVI MOD MDI AND BOR XOR SHR ASR SHL
%token IFB IFC IFE IFN IFG IFA IFL IFU ADX SBX STI STD
%token NOP BRK DAT ORG
%token JSR HCF INT IAG IAS RFI IAQ HWN HWQ HWI
%token LBR RBR LBRACE RBRACE LPAR RPAR
%token COMMA DP
%token PLUS MINUS EMUL EDIV EMOD ENOT EXOR EAND EOR SHIFTL SHIFTR
%token DOT HASH MACRO INCLUDE
%token <ival> NUMBER
%token <sval> STRING QSTRING

%type <ival> register opcode operand expr extended noarg

%left EOR
%left EXOR
%left EAND
%left SHIFTL SHIFTR
%left PLUS MINUS
%left EMUL EDIV EMOD
%left ENOT NEG

%%

prog	
	: /* empty */
	| prog statement
	;

statement
	: opcode operand COMMA operand
	{
				popop(($4 << 10) | ($2 << 5) | $1);
				popallr();
	}
	| extended operand
	{
				popop(($2 << 10) | ($1 << 5));
				popallr();
	}
	| noarg			{ popop($1 << 5); }
	| DP STRING		{ addref($2); }
	| DAT data		{ popall(); }
	| ORG expr		{ pc = $2; }
	| error
	;

data
	: /* empty */
	| entry
	| data COMMA entry
	;

entry
	: QSTRING
	{
				char *s = $1;
				while (*s)
					push(*s++, NULL);
	}
	| STRING		{ push(0, $1); }
	| expr			{ push($1, NULL); }
	;

expr
	: NUMBER		{ $$ = $1; }
	| MINUS expr %prec NEG	{ $$ = -$2; }
	| ENOT expr		{ $$ = ~$2; }
	| expr PLUS expr	{ $$ = $1 + $3; }
	| expr MINUS expr	{ $$ = $1 - $3; }
	| expr EMUL expr	{ $$ = $1 * $3; }
	| expr EDIV expr	{ $$ = $3 ? $1 / $3 : 0; }
	| expr EMOD expr	{ $$ = $3 ? $1 % $3 : 0; }
	| expr EXOR expr	{ $$ = $1 ^ $3; }
	| expr EAND expr	{ $$ = $1 & $3; }
	| expr EOR expr		{ $$ = $1 | $3; }
	| expr SHIFTL expr	{ $$ = $1 << $3; }
	| expr SHIFTR expr	{ $$ = $1 >> $3; }
	| LPAR expr RPAR	{ $$ = $2; }
	;

operand
	: register		{ $$ = $1; }
	| LBR register RBR	{ $$ = 0x08 + $2; }
	| PUSH			{ $$ = 0x18; }
	| POP			{ $$ = 0x18; }
	| PEEK			{ $$ = 0x19; }
	| PICK			{ $$ = 0x1a; }
	| SP			{ $$ = 0x1b; }
	| PC			{ $$ = 0x1c; }
	| EX			{ $$ = 0x1d; }
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
				if ($1 <= 30 && $1 >= -1)
					$$ = 0x20 + $1 + 1;
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
	: SET			{ $$ = 0x01; }
	| ADD			{ $$ = 0x02; }
	| SUB			{ $$ = 0x03; }
	| MUL			{ $$ = 0x04; }
	| MLI			{ $$ = 0x05; }
	| DIV			{ $$ = 0x06; }
	| DVI			{ $$ = 0x07; }
	| MOD			{ $$ = 0x08; }
	| MDI			{ $$ = 0x09; }
	| AND			{ $$ = 0x0a; }
	| BOR			{ $$ = 0x0b; }
	| XOR			{ $$ = 0x0c; }
	| SHR			{ $$ = 0x0d; }
	| ASR			{ $$ = 0x0e; }
	| SHL			{ $$ = 0x0f; }
	| IFB			{ $$ = 0x10; }
	| IFC			{ $$ = 0x11; }
	| IFE			{ $$ = 0x12; }
	| IFN			{ $$ = 0x13; }
	| IFG			{ $$ = 0x14; }
	| IFA			{ $$ = 0x15; }
	| IFL			{ $$ = 0x16; }
	| IFU			{ $$ = 0x17; }
	| ADX			{ $$ = 0x1a; }
	| SBX			{ $$ = 0x1b; }
	| STI			{ $$ = 0x1e; }
	| STD			{ $$ = 0x1f; }
	;

extended
	: JSR			{ $$ = 0x01; }
	| HCF			{ $$ = 0x07; }
	| INT			{ $$ = 0x08; }
	| IAG			{ $$ = 0x09; }
	| IAS			{ $$ = 0x0a; }
	| RFI			{ $$ = 0x0b; }
	| IAQ			{ $$ = 0x0c; }
	| HWN			{ $$ = 0x10; }
	| HWQ			{ $$ = 0x11; }
	| HWI			{ $$ = 0x12; }
	;

noarg
	: NOP			{ $$ = 0x00; }
	| BRK			{ $$ = 0x02; }
	;


%%

void
yyerror(const char *s, ...)
{
	va_list ap;

	va_start(ap, s);
	fprintf(stderr, "Line %d: ", yylineno);
	vfprintf(stderr, s, ap);
	fprintf(stderr, "\n");
	va_end(ap);
	haserrors = 1;
}

void
push(int i, char *s)
{
	stack[sp].val = i;
	stack[sp].lineno = yylineno;
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

	while (sp) {
		buffer[pc] = stack[n - sp].val;
		label[pc].label = stack[n - sp].label;
		label[pc++].lineno = stack[n - sp--].lineno;
	}
}

void
popallr(void)
{
	while (sp) {
		buffer[pc] = stack[--sp].val;
		label[pc].label = stack[sp].label;
		label[pc++].lineno = stack[sp].lineno;
	}
}

void
addref(char *s)
{
	ref[rp].label = s;
	ref[rp].lineno = yylineno;
	ref[rp++].val = pc;
}

int
findref(struct label *l)
{
	int i;

	for (i = 0; i < rp; i++)
		if (strcmp(ref[i].label, l->label) == 0)
			return ref[i].val;

	yylineno = l->lineno;
	yyerror("missing label reference: %s", l->label);
	return 0;
}

void
restorerefs(void)
{
	int i;

	for (i = 0; i < pc; i++)
		if (label[i].label)
			buffer[i] = findref(&label[i]);
}

unsigned short *
compile(FILE *fd, size_t sz)
{
	buffer = calloc(sz, sizeof(unsigned short));
	label = calloc(sz, sizeof(struct label));
	stack = calloc(sz, sizeof(struct pair));
	ref = calloc(sz, sizeof(struct pair));

#if YYDEBUG
	yydebug = 1;
#endif

	yyin = fd;
	yyparse();
	restorerefs();

	free(ref);
	free(stack);
	free(label);

	if (haserrors) {
		free(buffer);
		buffer = NULL;
	}

	return buffer;
}
