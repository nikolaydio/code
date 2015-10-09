#ifndef __ZHABACORE_ABSTRACT_TREE_H_INCLUDED__
#define __ZHABACORE_ABSTRACT_TREE_H_INCLUDED__


#include "bytecode.h"

struct zhnode;
typedef void (*contr_ptr)(struct zhnode*, ZFragment*);
typedef void (*optimize_ptr)(struct zhnode*);

//******************************************************************************
//Universal structure. Contained by every stmt/expr.
//
typedef struct zhnode
{
	contr_ptr contribute;
	optimize_ptr optimize;
}ZNode;

//End of program function.
void zh_end_of_program_contribute(ZNode* node, ZFragment* frag);

//******************************************************************************
//ZPrintStmt
//
typedef struct zhprintstmt
{
	ZNode self;
	ZNode* expr;
}ZPrintStmt;
void zh_print_stmt_contribute(ZNode* node, ZFragment* frag);


//******************************************************************************
//ZLiteral
//
enum data_type
{
	ZL_INT,
	ZL_FLOAT,
	ZL_STR // special type. the data will simply be a pointer to the str.
};
union lit_data
{
	char * sval;
	float fval;
	int ival;
};
typedef struct zhexprliteral
{
	ZNode self;
	enum data_type type;
	union lit_data data;
}ZLiteral;
void zh_literal_contribute(ZNode* node, ZFragment* frag);

//******************************************************************************
//ZOperator
//
enum zh_operator_type
{
	zhop_add,
	zhop_sub,
	zhop_mul,
	zhop_div,
	zhop_selfadd,
	zhop_selfmid,
	zhop_selfmul,
	zhop_selfdiv,
	zhop_assign
};
typedef struct zhoperatornode
{
	ZNode self;
	enum zh_operator_type type;
	ZNode* left;
	ZNode* right;
}ZOperator;
void zh_operator_contribute(ZNode* node, ZFragment* frag);


//******************************************************************************
//ZIdent
//
typedef struct zhident
{
	ZNode self;
	char* string;
}ZIdent;
void zh_ident_contribute(ZNode* node, ZFragment* frag);
//******************************************************************************
//ZVarDeclaration
//
typedef struct zhvardecl_item
{
	char* name;
	ZNode* value;
	struct zhvardecl_item* next;
}ZVarDecl;
typedef struct zhvardecl_list
{
	ZVarDecl* first;
	ZVarDecl* last;
}ZVarList;
void zh_push_vardecl(ZVarList* list, ZVarDecl* decl);
typedef struct zhvardecl
{
	ZNode self;
	char* type;
	ZVarList* var_list;
}ZVarDeclaration;
void zh_var_decl_contribute(ZNode* node, ZFragment* frag);
//******************************************************************************
//ZExprContainer
//
typedef struct zhexprc
{
	ZNode self;
	ZNode* expr;
}ZExprContainer;
void zh_expr_container_contribute(ZNode* node, ZFragment* frag);
//******************************************************************************
//ZTreeLeafs
//

typedef struct zhtreeleaf
{
	struct zhtreeleaf* next;
	ZNode *node;
}ZLeaf;

typedef struct zhleaflist
{
	ZLeaf* first;
	ZLeaf* last;
	unsigned int count;
}ZLeafList;
void zh_leaf_push(ZLeafList* list, ZLeaf* leaf);

typedef struct zhabstractsyntaxtree
{
	//source of this syntax tree. If multiple files then 0;
	char* filename;
	ZLeafList* stmts;
}ZAbstractTree;

#endif
