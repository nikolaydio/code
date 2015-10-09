#include "../include/zhabacore.h"
#include "compile.h"
#include <stdio.h>
#include <stdlib.h>

ZEnvironment*
zh_create_environment(ZCodeObject* code)
{
	ZEnvironment* env = (ZEnvironment*)malloc(sizeof(ZEnvironment));
	//keep the default page size at 1024. It really doesn't matter at the
	//current stage of development. Tough it would be nice to be controllable
	//through command line or the executable script itself at later stage.
	env->func_stack = zh_allocate_stack(1024);
	return env;
}


void
zh_destroy_environment(ZEnvironment* target)
{
	zh_free_stack(target->func_stack);
}

//**********************************************************************
//What should we do here?
//Heres' a short list:
//1. Turn the source code into abstract syntax tree, using the open
//source tools flex & bison.
//
//2. Loop through the tree and extract literals(data), code(text) and
//other meta data into code fragments
//
//Code Fragment: This is the pure already processed data from a single
//source file.

//3. Assemble the code framents into one end result.(or linking for C ppl)
//**********************************************************************
ZCodeObject*
zh_compile(char* filename, zh_feedback callback)
{
	//create the abstract tree
	ZAbstractTree* tree = zh_gen_abstract_tree(filename, callback);
	if(tree == 0)
	{
		return 0;
	}
	
	//extract the data
	ZFragment* frag = zh_produce_fragment(tree);
	
	
	//release the tree. This will iterate through the whole tree and dealloc
	//everything needed.
	zh_free_abstract_tree(tree);
	
	if(frag == 0)
	{
		return 0;
	}
	ZCodeObject* obj = zh_link_fragments(frag,filename);
	
	zh_free_fragment(frag);
	
	
	return obj;
}

//! - Execute the following code object as fast as possible
void zh_execute(ZEnvironment* vm, ZCodeObject* obj)
{
	zh_initialize_opcodes();
	zh_run_code(obj, vm);
}
