%{
#include "abstract_tree.h"
#include "parser.tab.h"  // to get the token types that we return


%}
%%

\;				{ return T_END; }
\+				{ return T_ADD; }
\-				{ return T_SUB; } 
\/				{ return T_DIV; }
\*				{ return T_MUL;}
\=				{ return T_ASSIGN; }
\,				{ return T_COMMA; }
\(				{ return T_BRACE_OPEN; }
\)				{ return T_BRACE_CLOSE; }
print			{ return T_PRINT; }

[ \t\n]   			
[0-9]+\.[0-9]+ 		{ yylval.fval = atof(yytext); return FLOAT; }
[0-9]+            	{ yylval.ival = atoi(yytext); return INT; }
[\_a-zA-Z][\_a-zA-Z0-9]+     	 {
	yylval.sval = strdup(yytext);
	return IDENT;
}
[a-zA-Z]						 {
	yylval.sval = strdup(yytext);
	return IDENT;
}
\"([#-~\!])*\"					{
	yylval.sval = (char*)malloc(sizeof(char)*(strlen(yytext)-2));
	memcpy(yylval.sval, yytext+1, sizeof(char)*(strlen(yytext)-2));
	return STRING;
}

%%
