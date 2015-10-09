#ifndef __ZHABACORE_BYTECODE_H_INCLUDED__
#define __ZHABACORE_BYTECODE_H_INCLUDED__

enum zh_bytecode
{
	//STORE [offset] - writes the last item to the data stack.
	//offset -> bytes relative to the frame pointer
	B_STORE_BYTE,
	B_STORE_WORD,
	B_STORE_DWORD,
	//LOAD [offset] - reads data from the stack and push it on the
	//stack
	B_LOAD_BYTE,
	B_LOAD_WORD,
	B_LOAD_DWORD,
	//RELOC [size] - resizes and moves the virtual machine stack pointers
	B_RELOC_DWORD,
	//WRITE - pop the last element and write it to the address(top element)
	B_WRITE_BYTE,
	B_WRITE_WORD,
	B_WRITE_DWORD,
	//READ - reads the data from the memory(top element) and push it as an new
	// element
	B_READ_BYTE,
	B_READ_WORD,
	B_READ_DWORD,
	//ADDs the last 2 bytes/words/dwords and pushes the result
	B_ADD_BYTE,
	B_ADD_WORD,
	B_ADD_DWORD,
	//SUBs the last 2 values and push result
	B_SUB_BYTE,
	B_SUB_WORD,
	B_SUB_DWORD,
	//MULs the last 2 values and push result
	B_MUL_BYTE,
	B_MUL_WORD,
	B_MUL_DWORD,
	//DIVs the last 2 values and push result
	B_DIV_BYTE,
	B_DIV_WORD,
	B_DIV_DWORD,
	//PUSH [value] - pushes the value as element in the function stack
	//element type is chosen to minimize memory usage
	B_PUSH_BYTE,
	B_PUSH_WORD,
	B_PUSH_DWORD,
	//removes the top element from the function stack
	B_POP_BYTE,
	B_POP_WORD,
	B_POP_DWORD,
	
	//Advanced codes
	//---------------------------------------------------------------------
	
	// Prints the last item on the function stack and pop it
	// If the last item is one byte, it interpret it as an ascii char
	// otherwise it print the content.
	B_PRINT_BYTE,
	B_PRINT_WORD,
	B_PRINT_DWORD,
	
	
	//Special opcodes
	//---------------------------------------------------------------------
	B_END_PROGRAM,
	
	
	B_INVALID
};

//------------------------------------------------------------------------------
//ZOperand
//------------------------------------------------------------------------------
typedef struct zhoperand
{
	enum
	{
		ZH_EMPTY,
		ZH_FIXED,
		ZH_RELATIVE,
		ZH_INVALID
	}type;
	union
	{
		//fixed means that it can be copied directly
		int fixed;
		//relative means it need some kind of link operating before
		//getting in the end bytecode
		//this is usually some kind of label
		char* relative;
	}data;
}ZOperand;

//------------------------------------------------------------------------------
//ZInstruction
//------------------------------------------------------------------------------
typedef struct zhinstr
{
	enum zh_bytecode opcode;
	ZOperand operand; // if opcode require operand
	struct zhinstr* next;
}ZInstruction;


//------------------------------------------------------------------------------
//ZLocalVariable
//------------------------------------------------------------------------------
typedef struct zhlocalvar
{
	struct zhlocalvar* next;
	char* name;
	char size;
}ZLocalVariable;

//------------------------------------------------------------------------------
//ZFunction
//------------------------------------------------------------------------------
typedef struct zhfunc
{
	ZInstruction* first;
	ZInstruction* last;
	char* fname;
	int size;
	ZLocalVariable* first_local;
	ZLocalVariable* last_local;
	struct zhfunc* next;
}ZFunction;

typedef struct zhbuildcontext
{
	ZFunction* current_function; // 0 means the global one
}ZBuildContext;

typedef struct zhfragment
{
	//function list
	ZFunction* fnc_first;
	ZFunction* fnc_last;
	//build context - contains data used for building the fragment
	ZBuildContext* context;
	struct zhfragment* next;
}ZFragment;

//fragment management function
void zh_fragment_push_fnc(ZFragment* frag, ZFunction* func);
void zh_function_push_instr(ZFunction* func, ZInstruction* instr);
void zh_active_fnc_push_instr(ZFragment* frag, ZInstruction* instr);

#endif
