#ifndef OPTIMIZE_H
#define OPRIMIZE_H

#include "tree.h"
#include "buffer.h"

enum optimize_return_e
{
    OPTIMIZE_RETURN_SUCCESS,
    OPTIMIZE_RETURN_ALLOCATION_ERROR,
    OPTIMIZE_RETURN_BUFFER_ERROR,
    OPTIMIZE_RETURN_TREE_ERROR
};

struct optimizer_s 
{   
    buffer_t buffer;
    tree_t   ast_tree;      
};
typedef optimizer_s* optimizer_t; 

optimize_return_e
OptimizerCtor(const char* input_name, optimizer_t* optimizer);

optimize_return_e 
OptimizerDtor(optimizer_t* optimizer);

#endif // OPTIMIZE_H