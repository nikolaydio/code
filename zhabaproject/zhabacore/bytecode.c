#include "bytecode.h"

void zh_fragment_push_fnc(ZFragment* frag, ZFunction* func)
{
	if(frag->fnc_first == 0)
	{
		frag->fnc_first = func;
	}
	if(frag->fnc_last != 0)
	{
		frag->fnc_last->next = func;
	}
	frag->fnc_last = func;
}
void zh_function_push_instr(ZFunction* func, ZInstruction* instr)
{
	if(func->first == 0)
	{
		func->first = instr;
	}
	if(func->last != 0)
	{
		func->last->next = instr;
	}
	func->last = instr;
}

void zh_active_fnc_push_instr(ZFragment* frag, ZInstruction* instr)
{
	if(frag->context == 0)
	{
		return;
	}
	zh_function_push_instr(frag->context->current_function, instr);
}
