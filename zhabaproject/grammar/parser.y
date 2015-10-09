%{
#include <stdio.h>
#include <string.h>
#include "abstract_tree.h"


// stuff from flex that bison needs to know about:
int yylex();
int yyparse();
FILE *yyin;
void yyerror(const char *s);

ZAbstractTree *g_abstract_tree;
%}

%union {
	int ival;
	float fval;
	char *sval;
	
	ZLeafList *leaf_list;
	ZLeaf* leaf;
	ZNode* node;
	
	ZVarDecl* var_decl;
	ZVarList* var_list;
}
%token <ival> INT
%token <fval> FLOAT
%token <sval> IDENT
%token <sval> STRING

%token T_END
%token T_ADD
%token T_SUB
%token T_DIV
%token T_MUL

%token T_ASSIGN

%token T_COMMA

%token T_PRINT

%token T_BRACE_OPEN
%token T_BRACE_CLOSE

%type <leaf_list> stmts;
%type <leaf> stmt;
%type <node> simple_stmt;

%type <node> expr;
%type <node> atom
%type <node> primary
%type <node> factor
%type <node> term
%type <node> ident;
%type <node> assignment;
%type <var_list> variable_list;
%type <var_decl> variable_decl;
%%
start:
	stmts {
		g_abstract_tree = (ZAbstractTree*)malloc(sizeof(ZAbstractTree));
		g_abstract_tree->stmts = $1;
		//put end of program code
		ZNode* node = (ZNode*)malloc(sizeof(ZNode));
		node->contribute = &zh_end_of_program_contribute;
		ZLeaf* leaf = (ZLeaf*)malloc(sizeof(ZLeaf));
		leaf->node = node;
		zh_leaf_push(g_abstract_tree->stmts, leaf);
	}
	;
	
stmts:
	stmts stmt {
		zh_leaf_push($1,$2);
		$$ = $1;
	}
	| stmt {
		$$ = (ZLeafList*)malloc(sizeof(ZLeafList));
		memset($$, 0, sizeof(ZLeafList));
		zh_leaf_push($$,$1);
	}
	;
	
stmt:
	simple_stmt T_END {
		$$ = (ZLeaf*)malloc(sizeof(ZLeaf));
		$$->node = $1;
	}
	;
	

simple_stmt:
	T_PRINT expr {
		ZPrintStmt* stmt = (ZPrintStmt*)malloc(sizeof(ZPrintStmt));
		stmt->self.contribute = &zh_print_stmt_contribute;
		stmt->expr = $2;
		$$ = (ZNode*)stmt;
	}
	| expr {
		ZExprContainer* stmt = (ZExprContainer*)malloc(sizeof(ZExprContainer));
		stmt->self.contribute = &zh_expr_container_contribute;
		stmt->expr = $1;
		$$ = (ZNode*)stmt;
	}
	| IDENT variable_list {
		ZVarDeclaration* stmt = (ZVarDeclaration*)malloc(sizeof(ZVarDeclaration));
		stmt->self.contribute = &zh_var_decl_contribute;
		stmt->type = $1;
		stmt->var_list = $2;
		$$ = (ZNode*)stmt;
	}
	;
variable_list:
	variable_list T_COMMA variable_decl {
		$$ = $1;
		zh_push_vardecl($$, $3);
	}
	| variable_decl {
		$$ = (ZVarList*)malloc(sizeof(ZVarList));
		zh_push_vardecl($$, $1);
	}
	;
variable_decl:
	IDENT {
		$$ = (ZVarDecl*)malloc(sizeof(ZVarDecl));
		$$->name = $1;
	}
	| IDENT T_ASSIGN expr {
		$$ = (ZVarDecl*)malloc(sizeof(ZVarDecl));
		$$->name = $1;
		$$->value = $3;
	}
	;

expr:
	assignment { $$ = $1; }
	
	;

assignment:
	assignment T_ASSIGN term {
		ZOperator* op = (ZOperator*)malloc(sizeof(ZOperator));
		op->type = zhop_assign;
		op->left = $1;
		op->right = $3;
		op->self.contribute = &zh_operator_contribute;
		$$ = (ZNode*)op;
	}
	| term {
		$$ = $1;
	}
	;

term:
	term T_ADD factor {
		ZOperator* op = (ZOperator*)malloc(sizeof(ZOperator));
		op->type = zhop_add;
		op->left = $1;
		op->right = $3;
		op->self.contribute = &zh_operator_contribute;
		$$ = (ZNode*)op;
	}
	| term T_SUB factor {
		ZOperator* op = (ZOperator*)malloc(sizeof(ZOperator));
		op->type = zhop_sub;
		op->left = $1;
		op->right = $3;
		op->self.contribute = &zh_operator_contribute;
		$$ = (ZNode*)op;
	}
	| factor {
		$$ = $1;
	}
	;

factor:
	factor T_MUL primary {
		ZOperator* op = (ZOperator*)malloc(sizeof(ZOperator));
		op->type = zhop_mul;
		op->left = $1;
		op->right = $3;
		op->self.contribute = &zh_operator_contribute;
		$$ = (ZNode*)op;
	}
	| factor T_DIV primary {
		ZOperator* op = (ZOperator*)malloc(sizeof(ZOperator));
		op->type = zhop_div;
		op->left = $1;
		op->right = $3;
		op->self.contribute = &zh_operator_contribute;
		$$ = (ZNode*)op;
	}
	| primary {
		$$ = $1;
	}
	;

primary:
	atom {
		$$ = $1;
	}
	| T_BRACE_OPEN expr T_BRACE_CLOSE {
		$$ = $2;
	}
	;
	
atom:
	INT {
		ZLiteral* lit = (ZLiteral*)malloc(sizeof(ZLiteral));
		lit->self.contribute = &zh_literal_contribute;
		lit->type = ZL_INT;
		lit->data.ival = $1;
		$$ = (ZNode*)lit;
	}
	| FLOAT {
		ZLiteral* lit = (ZLiteral*)malloc(sizeof(ZLiteral));
		lit->self.contribute = &zh_literal_contribute;
		lit->type = ZL_FLOAT;
		lit->data.ival = $1;
		$$ = (ZNode*)lit;
	}
	| ident {
		$$ = $1;
	}
	| string
	;
ident:
	IDENT {
		ZIdent* id = (ZIdent*)malloc(sizeof(ZIdent));
		id->string = $1;
		id->self.contribute = &zh_ident_contribute;
		$$ = (ZNode*)id;
	}
	;
string:
	STRING
	;
	
%%
	
	
void yyerror(const char *s)
{
	printf("Syntax error: %s\n", s);
}
