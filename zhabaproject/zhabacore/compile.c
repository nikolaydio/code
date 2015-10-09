#include "compile.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//Externs to communicate with flex&bison
extern int yylex();
extern int yyparse();
extern FILE *yyin;
//current context, stored in bison's file
extern ZAbstractTree* g_abstract_tree;

//Abstract Syntax Tree related functions
ZAbstractTree*
zh_gen_abstract_tree(char* filename, zh_feedback callback)
{
	// open the file in read-only mode
	yyin = fopen(filename, "r");

	// parse through the input until there is no more:
	do {
	yyparse();
	} while (!feof(yyin));
	
	if(g_abstract_tree == 0)
	{
		char* msg = (char*)malloc( sizeof(char) * 
			( strlen("Failed to generate abstract syntax tree of file: ")
			+ strlen(filename) ) );
		msg[0] = 0;
		strcpy(msg, "Failed to generate abstract syntax tree of file: ");
		strcat(msg, filename);
		callback(msg);
		free(msg);
	}
	return g_abstract_tree;
}


void
zh_free_abstract_tree(ZAbstractTree* tree)
{
	if(tree == 0)
	{
		return;
	}
	free(tree);
}


//Build the object list
ZFragment*
zh_produce_fragment(ZAbstractTree* tree)
{
	ZFragment* frag = (ZFragment*)malloc(sizeof(ZFragment));
	//we are building here. we should load build context too
	frag->context = (ZBuildContext*)malloc(sizeof(ZBuildContext));
	//now create a global function and add it in the function chain
	frag->context->current_function = (ZFunction*)malloc(sizeof(ZFunction));
	zh_fragment_push_fnc(frag, frag->context->current_function);
	
	ZLeaf* leaf = tree->stmts->first;
	while(leaf != 0)
	{
		leaf->node->contribute(leaf->node, frag);
		leaf = leaf->next;
	}
	return frag;
}

void
zh_free_fragment(ZFragment* fragment)
{
}



//For now just create a solid block of the first fragment.
//This should be coded property when the fragment producer is ready.
ZCodeObject* zh_link_fragments(ZFragment* fragments, char* filename)
{
	//Calc the size of the buffer.
	int bytes = 0; ZCodeObject* obj = (ZCodeObject*)malloc(sizeof(ZCodeObject));
	char* buffer = 0;
	ZInstruction* curr = fragments->fnc_first->first;
	while(curr != 0)
	{
		switch(curr->opcode)
		{
			case B_PUSH_DWORD:
				bytes += 5; // (opcode + operand)
				break;
			case B_PRINT_DWORD: case B_MUL_DWORD: case B_ADD_DWORD:
			case B_SUB_DWORD: case B_DIV_DWORD:
			case B_POP_DWORD: case B_END_PROGRAM:
				bytes += 1;
				break;
		}
		curr = curr->next;	
	}
	obj->buffer = (char*)malloc(bytes);
	curr = fragments->fnc_first->first;
	buffer = obj->buffer;
	while(curr != 0)
	{
		*buffer = (char)curr->opcode;
		buffer += 1;
		switch(curr->opcode)
		{
			case B_PUSH_DWORD:
				*(int*)buffer = curr->operand.data.fixed;
				buffer += 4;
				break;
			default:
				break;
		}
		curr = curr->next;	
	}
	obj->text = obj->buffer;
	obj->text_size = bytes;
	obj->buffer_size = bytes;
	return obj;
}






