#include <stdlib.h>
#include <stdio.h>
#include "../include/zhabacore.h"
#include "../zhabacore/runtime.h"


int main()
{
	ZStack* stack = zh_allocate_stack(8);
	zh_push_dword(stack, 2);
	zh_push_dword(stack, 3);
	int ret = zh_pop_dword(stack);
	int ret2 = zh_pop_dword(stack);
	return 0;
}
