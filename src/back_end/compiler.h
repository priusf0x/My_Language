#ifndef COMPILER_H
#define COMPILER_H

#include "tree.h"
#include "buffer.h"

struct compiler_s 
{  
    buffer_t buffer;
    tree_t   compiler_tree;
    size_t   label_count;
};
typedef compiler_s* compiler_t;

enum compiler_return_e
{   
    COMPILER_RETURN_SUCCESS,
    COMPILER_RETURN_ALLOCATION_ERROR,
    COMPILER_RETURN_BUFFER_ERROR,
    //... 
};

// =========================== MEMORY_CONTROLLING =============================

compiler_return_e
CompilerInit(const char* file_name,
             compiler_t* compiler);

compiler_return_e
CompilerDestroy(compiler_t* compiler);

#endif // COMPILER_H