#ifndef __ZHABACORE_RUNTIME_H_INCLUDED__
#define __ZHABACORE_RUNTIME_H_INCLUDED__

#include "bytecode.h"

//------------------------------------------------------------------------------
//This is populating the global opcode map. (Mapping opcode with function)
void zh_initialize_opcodes();

//------------------------------------------------------------------------------
//Stack Structures
//------------------------------------------------------------------------------
typedef struct zhstackpage
{
	char* buffer;
	struct zhstackpage* prev;
	struct zhstackpage* next;
}ZStackPage;
typedef struct zhstack
{
	ZStackPage* current;
	char* top;
	short size;
}ZStack;
ZStackPage* zh_allocate_stack_page(ZStack* stack);
void zh_free_stack_page(ZStackPage* page);
ZStack* zh_allocate_stack(short page_size);
void zh_free_stack(ZStack* stack);
//------------------------------------------------------------------------------
//Stack Manipulation Functions
//------------------------------------------------------------------------------
char zh_pop_byte(ZStack* stack);
short zh_pop_word(ZStack* stack);
int zh_pop_dword(ZStack* stack);
void zh_push_byte(ZStack* stack, char byte);
void zh_push_word(ZStack* stack, short word);
void zh_push_dword(ZStack* stack, int dword);



typedef struct zhenvironment
{
	char* instr_pointer;
	ZStack* func_stack;
	ZStack* data_stack;
}ZEnvironment;

//-------------------------------------------------------------
//struct ZCodeObject
//Contains the data for a single program.
//
typedef struct zhcodeobject
{
	//end product
	char* text;
	int text_size;
	char* buffer;
	int buffer_size;
}ZCodeObject;

char* zh_execute_opcode(ZEnvironment* env, char* buffer);
void zh_run_code(ZCodeObject* obj, ZEnvironment* env);

#endif
