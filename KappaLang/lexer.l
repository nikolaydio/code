%option noyywrap
%{
#include "stdio.h"
#include "main.h"
extern void handle_token(int token);
extern void set_num(int);
%}
%%

\.				{ return TOKEN_END; }
Kappa			{ return TOKEN_KAPPA; }
NoCopy			{ return TOKEN_NOCOPY; }
Pasterino		{ return TOKEN_PASTERINO; }
pasterino 		{ return TOKEN_INLINE;}
[ \t\n]

[0-9]+            	{ set_num(atoi(yytext)); return TOKEN_NUM; }
END				{ return -1; }
%%
