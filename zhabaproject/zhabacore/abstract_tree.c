#include "abstract_tree.h"
#include <stdlib.h>
void
zh_leaf_push(ZLeafList* list, ZLeaf* leaf)
{
	leaf->next = 0;
	if(list->first == 0 && list->last == 0)
	{
		list->first = leaf;
		list->last = leaf;
		list->count = 1;
	}else
	{
		list->last->next = leaf;
		list->last = leaf;
		++list->count;
	}
}
void zh_push_vardecl(ZVarList* list, ZVarDecl* decl)
{
	if(list->first == 0)
	{
		list->first = decl;
	}
	if(list->last != 0)
	{
		list->last->next = decl;
	}
	list->last = decl;
}

void zh_end_of_program_contribute(ZNode* node, ZFragment* frag)
{
	ZInstruction* instr = (ZInstruction*)malloc(sizeof(ZInstruction));

	instr->opcode = B_END_PROGRAM;
	instr->operand.type = ZH_EMPTY;
	
	zh_active_fnc_push_instr(frag, instr);
}

void
zh_print_stmt_contribute(ZNode* node, ZFragment* frag)
{
	ZPrintStmt* print = (ZPrintStmt*)node;
	print->expr->contribute(print->expr, frag);
	ZInstruction* instr = (ZInstruction*)malloc(sizeof(ZInstruction));
	instr->opcode = B_PRINT_DWORD;
	zh_active_fnc_push_instr(frag, instr);
}
void
zh_literal_contribute(ZNode* node, ZFragment* frag)
{
	ZLiteral* lit = (ZLiteral*)node;
	ZInstruction* instr = (ZInstruction*)malloc(sizeof(ZInstruction));

	instr->opcode = B_PUSH_DWORD;
	instr->operand.data.fixed = lit->data.ival;
	instr->operand.type = ZH_FIXED;
	
	zh_active_fnc_push_instr(frag, instr);
}
void
zh_operator_contribute(ZNode* node, ZFragment* frag)
{
	ZOperator* oper = (ZOperator*)node;
	oper->left->contribute(oper->left, frag);
	oper->right->contribute(oper->right, frag);
	ZInstruction* instr = (ZInstruction*)malloc(sizeof(ZInstruction));
	if(oper->type == zhop_add)
	{
		instr->opcode = B_ADD_DWORD;
	}else if(oper->type == zhop_sub)
	{
		instr->opcode = B_SUB_DWORD;
	}else if(oper->type == zhop_mul)
	{
		instr->opcode = B_MUL_DWORD;
	}else if(oper->type == zhop_div)
	{
		instr->opcode = B_DIV_DWORD;
	}else if(oper->type == zhop_assign)
	{
		instr->opcode = B_STORE_DWORD;
		instr->operand.data.fixed = 0;
		instr->operand.type = ZH_EMPTY;
	}
	zh_active_fnc_push_instr(frag, instr);
}
void zh_expr_container_contribute(ZNode* node, ZFragment* frag)
{
	
}
void zh_ident_contribute(ZNode* node, ZFragment* frag)
{
	ZIdent* id = (ZIdent*)node; int offset = 0;
	ZLocalVariable* var = frag->context->current_function->first_local;
	while(var != 0)
	{
		if(!strcmp(var->name, id->string))
		{
			ZInstruction* instr = (ZInstruction*)malloc(sizeof(ZInstruction));

			instr->opcode = B_LOAD_DWORD;
			instr->operand.data.fixed = offset;
			instr->operand.type = ZH_FIXED;
	
			zh_active_fnc_push_instr(frag, instr);
		}
		offset = var->size;
		var = var->next;
	}
}
void zh_var_decl_contribute(ZNode* node, ZFragment* frag)
{
	
}
