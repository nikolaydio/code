#include <stdlib.h>
#include <stdio.h>
#include "../include/zhabacore.h"

void zh_exit()
{
	printf("zhabaexec [FLAGS] [INPUT]\n");
	printf("zhabacore version: %s\n", ZHABACORE_VERSION);
	exit(0);
}

char* zh_get_argument(int arg, int argc, char* argv[])
{
	if(arg < 0 || arg >= argc)
	{
		zh_exit();
		return 0;
	}
	return argv[arg];
}



void std_output(const char* msg)
{
	printf("%s\n", msg);
}

int main(int argc, char* argv[])
{
	char* target = 0; char* output = 0;
	char* bytecode = 0; int buff_size = 0;
	ZCodeObject* obj = 0; FILE* file;
	ZEnvironment* env;
	int i = 0;
	if(argc < 2)
	{
		zh_exit();
	}
	for(i = 1; i < argc; ++i)
	{
		if(!strcmp(argv[i], "-o"))
		{
			i+=1;
			output = zh_get_argument(i, argc, argv);
		}else if(!strcmp(argv[i], "-b"))
		{
			i+=1;
			bytecode = zh_get_argument(i, argc, argv);
		}else
		{
			target = zh_get_argument(i, argc, argv);
		}
	}
	
	if(target != 0)
	{
		obj = zh_compile(target, &std_output);
		if(output != 0)
		{
			/* Write your buffer to disk. */
			file = fopen(output,"wb");

			if (file){
				fwrite(obj->buffer, obj->buffer_size, 1, file);
			}

			fclose(file);
		}else
		{
			env = zh_create_environment();
			zh_execute(zh_create_environment(), obj);
			zh_destroy_environment(env);
		}
	}else if(bytecode != 0)
	{
		file = fopen(bytecode, "r");
		if (file != NULL) {
			if (fseek(file, 0L, SEEK_END) == 0) {
				buff_size = ftell(file);
			}
			target = (char*)malloc(sizeof(char) * (buff_size));
			fseek(file, 0L, SEEK_SET);
			fread(target, sizeof(char), buff_size, file);
		}
		fclose(file);
		obj = (ZCodeObject*)malloc(sizeof(ZCodeObject));
		obj->text = target;
		obj->text_size = buff_size;
		obj->buffer = target;
		obj->buffer_size = buff_size;
		env = zh_create_environment();
		zh_execute(env, obj);
		zh_destroy_environment(env);
	}
	return 0;
}
