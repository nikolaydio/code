#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "runtime.h"

typedef char* (*opcode_logic)(ZEnvironment*, char*);

opcode_logic opcode_map[B_INVALID];


char* zh_execute_opcode(ZEnvironment* env, char* buffer)
{
	if(*buffer >= B_INVALID && *buffer < 0)
	{
		exit(0);
	}
	return opcode_map[*buffer](env, buffer);
}
void zh_run_code(ZCodeObject* obj, ZEnvironment* env)
{
	env->instr_pointer = obj->text;
	while(env->instr_pointer != 0)
	{
		env->instr_pointer = zh_execute_opcode(env, env->instr_pointer);
	}
}


char zh_pop_byte(ZStack* stack)
{
	stack->top--;
	if(stack->top < stack->current->buffer)
	{
		//the data is on the previous page if it exist
		if(stack->current->prev == 0)
		{
			//no previous pages. Put here error handling.
			return 0;
		}else
		{
			stack->current = stack->current->prev;
			stack->top = stack->current->buffer + stack->size - 1;
		}
	}
	return *stack->top;
}
short zh_pop_word(ZStack* stack)
{
	stack->top-=2;
	char offset = 0;
	char ret[2];
	if(stack->top < stack->current->buffer)
	{
		//the data is on the previous page if it exist
		if(stack->current->prev == 0)
		{
			//no previous pages. Put here error handling.
			return 0;
		}else
		{
			offset = stack->current->buffer - stack->top;
			stack->current = stack->current->prev;
			stack->top = stack->current->buffer + stack->size - offset;

			memcpy(ret, stack->current->buffer + stack->size - offset, offset);
			memcpy(ret + offset, stack->current->next->buffer, 2 - offset);
			return *(short*)ret;
		}
	}
	return *(short*)stack->top;
}
int zh_pop_dword(ZStack* stack)
{
	stack->top-=4;
	char offset = 0;
	char ret[4];
	if(stack->top < stack->current->buffer)
	{
		//the data is on the previous page if it exist
		if(stack->current->prev == 0)
		{
			//no previous pages. Put here error handling.
			return 0;
		}else
		{
			offset = stack->current->buffer - stack->top;
			stack->current = stack->current->prev;
			stack->top = stack->current->buffer + stack->size - offset;

			memcpy(ret, stack->current->buffer + stack->size - offset, offset);
			memcpy(ret + offset, stack->current->next->buffer, 4 - offset);
			return *(int*)ret;
		}
	}
	return *(int*)stack->top;
}
void zh_push_byte(ZStack* stack, char byte)
{
	*stack->top = byte;
	stack->top++;
	if(stack->top >= stack->current->buffer + stack->size)
	{
		if(stack->current->next == 0)
		{
			stack->current->next = zh_allocate_stack_page(stack);
			stack->current->next->prev = stack->current;
		}
		stack->current = stack->current->next;
		stack->top = stack->current->buffer;
	}
}
void zh_push_word(ZStack* stack, short word)
{
	stack->top+=2;
	char offset;
	if(stack->top >= stack->current->buffer + stack->size)
	{
		offset = stack->top - (stack->current->buffer + stack->size);
		if(stack->current->next == 0)
		{
			stack->current->next = zh_allocate_stack_page(stack);
			stack->current->next->prev = stack->current;
		}
		stack->current = stack->current->next;
		stack->top = stack->current->buffer + offset;
		memcpy(stack->current->prev->buffer + stack->size - (2-offset), (char*)&word, 2-offset);
		memcpy(stack->current->buffer, (char*)(&word)+2-offset, offset);
	}else
	{
		*(short*)(stack->top-2) = word;
	}
}
void zh_push_dword(ZStack* stack, int dword)
{
	stack->top+=4;
	char offset;
	if(stack->top >= stack->current->buffer + stack->size)
	{
		offset = stack->top - (stack->current->buffer + stack->size);
		if(stack->current->next == 0)
		{
			stack->current->next = zh_allocate_stack_page(stack);
			stack->current->next->prev = stack->current;
		}
		stack->current = stack->current->next;
		stack->top = stack->current->buffer + offset;
		memcpy(stack->current->prev->buffer + stack->size - (4-offset), (char*)&dword, 4-offset);
		memcpy(stack->current->buffer, (char*)(&dword)+4-offset, offset);
	}else
	{
		*(int*)(stack->top-4) = dword;
	}
}

char* zh_opcode_add_dword(ZEnvironment* env, char* opcode)
{
	zh_push_dword(env->func_stack, zh_pop_dword(env->func_stack)+zh_pop_dword(env->func_stack));
	return env->instr_pointer+1;
}
char* zh_opcode_sub_dword(ZEnvironment* env, char* opcode)
{
	int right = zh_pop_dword(env->func_stack);
	zh_push_dword(env->func_stack, zh_pop_dword(env->func_stack) - right);
	return env->instr_pointer+1;
}
char* zh_opcode_mul_dword(ZEnvironment* env, char* opcode)
{
	zh_push_dword(env->func_stack, zh_pop_dword(env->func_stack) * zh_pop_dword(env->func_stack));
	return env->instr_pointer+1;
}
char* zh_opcode_div_dword(ZEnvironment* env, char* opcode)
{
	int right = zh_pop_dword(env->func_stack);
	zh_push_dword(env->func_stack, zh_pop_dword(env->func_stack) / right);
	return env->instr_pointer+1;
}

char* zh_opcode_print_dword(ZEnvironment* env, char* opcode)
{
	printf("%i\n", zh_pop_dword(env->func_stack));
	return env->instr_pointer+1;
}
char* zh_opcode_push_dword(ZEnvironment* env, char* opcode)
{
	zh_push_dword(env->func_stack, *(int*)(opcode+1));
	return env->instr_pointer+5;
}
char* zh_opcode_pop_dword(ZEnvironment* env, char* opcode)
{
	zh_pop_dword(env->func_stack);
	return env->instr_pointer+1;
}

//------------------------------------------------------------------------------
//Data Stack manipulation
//------------------------------------------------------------------------------
//special function. Only one variant with one dword
char* zh_opcode_reloc_dword(ZEnvironment* env, char* opcode)
{
	
}
char* zh_opcode_store_dword(ZEnvironment* env, char* opcode)
{
}
char* zh_opcode_load_dword(ZEnvironment* env, char* opcode)
{
	
}


char* zh_opcode_end_program(ZEnvironment* env, char* opcode)
{
	return 0;
}

void zh_initialize_opcodes()
{
	//general purpose
	opcode_map[B_PRINT_DWORD] = &zh_opcode_print_dword;
	//function stack manipulation
	opcode_map[B_PUSH_DWORD] = &zh_opcode_push_dword;
	opcode_map[B_POP_DWORD] = &zh_opcode_pop_dword;
	//arithmetic
	opcode_map[B_ADD_DWORD] = &zh_opcode_add_dword;
	opcode_map[B_SUB_DWORD] = &zh_opcode_sub_dword;
	opcode_map[B_MUL_DWORD] = &zh_opcode_mul_dword;
	opcode_map[B_DIV_DWORD] = &zh_opcode_div_dword;
	//data stack manipulation
	opcode_map[B_RELOC_DWORD] = &zh_opcode_reloc_dword;
	opcode_map[B_LOAD_WORD] = &zh_opcode_load_dword;
	opcode_map[B_STORE_DWORD] = &zh_opcode_store_dword;
	//program end
	opcode_map[B_END_PROGRAM] = &zh_opcode_end_program;
}

ZStackPage* zh_allocate_stack_page(ZStack* stack)
{
	char* buffer = (char*)malloc(sizeof(ZStackPage) + stack->size);
	ZStackPage* page = (ZStackPage*)buffer;
	page->buffer = buffer + sizeof(ZStackPage);
	page->prev = 0;
	page->next = 0;
	return page;
}
void zh_free_stack_page(ZStackPage* page)
{
	//single function though I have allocated the page not really traditional
	//(with two calls for buffer and struct, but one)
	//so this should stay just for less confusion.
	free(page);
}
ZStack* zh_allocate_stack(short page_size)
{
	ZStackPage* page;
	ZStack* stack = (ZStack*)malloc(sizeof(ZStack));
	stack->size = page_size;
	page = zh_allocate_stack_page(stack);
	stack->current = page;
	stack->top = page->buffer;
	return stack;
}
void zh_free_stack(ZStack* stack)
{
	//loop through all pages and free them. Then the stack struct itself
	ZStackPage* now = stack->current->next; // current iterator
	ZStackPage* next; //helper showing what will be destoryed
	while(now != 0)
	{
		next = now->next;
		free(now);
		now = next;
	}
	//now backwards
	now = stack->current;
	while(now != 0)
	{
		next = now->prev;
		free(now);
		now = next;
	}
}
