#ifndef __ZHABACORE_COMPILE_H_INCLUDED__
#define __ZHABACORE_COMPILE_H_INCLUDED__

#include "../include/zhabacore.h"
#include "abstract_tree.h"




//Abstract Syntax Tree related functions
ZAbstractTree* zh_gen_abstract_tree(char* filename, zh_feedback callback);
void zh_free_abstract_tree(ZAbstractTree* tree);


//Build the object list
ZFragment* zh_produce_fragment(ZAbstractTree* tree);
void zh_free_fragment(ZFragment* fragment);

//Link all fragments into one end product. The filename should be name
//of the main file. The start function is a special function that has to be
//build in that stage. It has to be build from different files, so the second
//argument helps to link them in the right order.
ZCodeObject* zh_link_fragments(ZFragment* fragments, char* filename);

#endif
