#ifndef __ZHABACORE_H_INCLUDED__
#define __ZHABACORE_H_INCLUDED__

#include "../zhabacore/runtime.h"


#define ZHABACORE_VERSION "0.0.1 Beta"


//standart bool
enum bool
{
	false,
	true
};

//Standart callback declaration for feedback of zh
typedef void (*zh_feedback)(const char*);


ZEnvironment* zh_create_environment();
void zh_destroy_environment(ZEnvironment* target);

ZCodeObject* zh_compile(char* filename, zh_feedback callback);

//! - Execute the following code object as fast as possible
void zh_execute(ZEnvironment* vm, ZCodeObject* obj);

#endif
