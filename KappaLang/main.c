#include <stdio.h>
#include <stdlib.h>
#include "main.h"

typedef enum _OPCODE {
	OP_PUSH, //1
	OP_POP, // 2
	OP_DUP, // 3
	OP_ROT, // 4
	OP_ADD, // 5
	OP_SUB, // 6
	OP_MUL, // 7
	OP_DIV, // 8
	OP_PRINT, // 9
	//END
	OP_CALL, // 10
	OP_CALL_NE, // 11
	OP_CALL_E, // 12
	OP_CALL_L, // 13
	OP_RET //14
}OPCODE;

typedef struct _OP {
	OPCODE opcode;
	int data;
}OP;
typedef struct _FUNC {
	OP* bytecode;
	int bytecode_size;
	int scope;
}FUNC;
FUNC *buckets = 0;
int bucket_size = 0;
FUNC* get_func(int s) {
	int i = 0;
	for(i = 0; i < bucket_size; ++i) {
		if(buckets[i].scope == s)
			return &buckets[i];
	}
	return 0;
}

int* stack = 0;
int stack_size = 0;
int stack_top = 0;
int last_num = 0;


int scope = -1;
int highest_scope = 0;

void new_env() {
	stack = (int*)malloc(4096*sizeof(int));
	stack_size = 0;
	stack_top = 0;
	last_num = 0;
}


void set_num(int a) {
	last_num = a;
}





void add_opcode(int token, int num) {
	FUNC* f = get_func(scope); void* nmem = 0;
	if(f == 0) {
		bucket_size++;
		nmem = (FUNC*)realloc(buckets, sizeof(FUNC)*bucket_size);
		if(nmem != 0)
			buckets = (FUNC*)nmem;
		buckets[bucket_size-1].scope = scope;
		f = &buckets[bucket_size-1];
		f->bytecode_size = 0;
		f->bytecode = 0;
	}
	f->bytecode_size++;
	f->bytecode = (OP*)realloc(f->bytecode, sizeof(OP)*f->bytecode_size);
	f->bytecode[f->bytecode_size-1].data = num;
	f->bytecode[f->bytecode_size-1].opcode = token;
}
void add_simple_statement(int kappas, int num) {
	kappas--;
	add_opcode(kappas, num);
}


extern int yylex();
extern int yyparse();
extern FILE *yyin;
int get_next_token() {
	return yylex();
}
int last_token = 0;
int accept(int token) {
	if(last_token == token) {
		last_token = get_next_token();
		return 1;
	}
	return 0;
}
int expect(int token) {
	if(accept(token))
		return 1;
	printf("Missing symbol.\n");
	return 0;
}

void simple_statement()
{
	int kappas = 0, num = 0;
	do {
		kappas++;
	}while(accept(TOKEN_KAPPA));
	if(accept(TOKEN_NUM)) {
		num = last_num;
	}
	//printf("Kappas %i, %i\n", kappas, num);
	add_simple_statement(kappas, num);
}
void statement();
void func_declaration() {
	int old = scope;
	scope = highest_scope++;
	do {
		statement();
	}while(!accept(TOKEN_PASTERINO));
	scope = old;
}
void statement() {
	if(accept(TOKEN_KAPPA)) {
		simple_statement();
	}else if(accept(TOKEN_NOCOPY)) {
		func_declaration();
	}
	expect(TOKEN_END);
}
void statements() {
	do {
		statement();
	}while(!accept(-1));
}
void execute(FUNC* f);
void exec_opcode(OP* op) {
	int a,b,c;
	switch(op->opcode) {
	case OP_PUSH:
		stack[stack_top++] = op->data;
		break;
	case OP_POP:
		--stack_top;
		break;
	case OP_ROT:
		a = stack[--stack_top];
		b = stack[--stack_top];
		stack[stack_top++] = a;
		stack[stack_top++] = b;
		break;
	case OP_DUP:
		a = stack[--stack_top];
		stack[stack_top++] = a;
		stack[stack_top++] = a;
		break;
	case OP_ADD:
		a = stack[--stack_top];
		b = stack[--stack_top];
		stack[stack_top++] = b + a;
		break;
	case OP_SUB:
		a = stack[--stack_top];
		b = stack[--stack_top];
		stack[stack_top++] = b - a;
		break;
	case OP_MUL:
		a = stack[--stack_top];
		b = stack[--stack_top];
		stack[stack_top++] = b * a;
		break;
	case OP_DIV:
		a = stack[--stack_top];
		b = stack[--stack_top];
		stack[stack_top++] = b / a;
		break;
	case OP_PRINT:
		a = stack[--stack_top];
		if(op->data == 1){
			printf("%s\n", &((char*)(stack))[(stack_top*4)-a]);
		}else{
			printf("%i\n", a);
		}
		break;
	case OP_CALL:
		a = stack[--stack_top];
		FUNC* f = get_func(a);
		if(f != 0) {
			execute(f);
		}
		break;
	case OP_CALL_NE:
		a = stack[--stack_top];
		b = stack[--stack_top];
		c = stack[--stack_top];
		if(b != c)
		{
			FUNC* f = get_func(a);
			if(f != 0) {
				execute(f);
			}
		}
		break;
	case OP_CALL_E:
		a = stack[--stack_top];
		b = stack[--stack_top];
		c = stack[--stack_top];
		if(b == c)
		{
			FUNC* f = get_func(a);
			if(f != 0) {
				execute(f);
			}
		}
		break;
	case OP_CALL_L:
		a = stack[--stack_top];
		b = stack[--stack_top];
		c = stack[--stack_top];
		if(b > c)
		{
			FUNC* f = get_func(a);
			if(f != 0) {
				execute(f);
			}
		}
		break;
	}
}
void execute(FUNC* f) {
	int i = 0;
	for(i = 0; i < f->bytecode_size; ++i) {
		exec_opcode(&f->bytecode[i]);
	}
}
void program_start() {
	//parse
	last_token = get_next_token();
	statements();
	//exec
	execute(get_func(-1));
}

int main(int argc, char* argv[]) {
	int i = 0;
	for(i = 1; i < argc; ++i) {
		new_env();
		FILE* f = fopen(argv[i], "r");
		yyin = f;
		do {
			program_start();
		}while(!feof(yyin));
	}
	return 0;
}