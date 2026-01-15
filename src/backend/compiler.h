#ifndef COMPILER_H
#define COMPILER_H

#include "tree.h"
#include "my_string.h"

struct compiler_s 
{  
    tree_t compiler_tree;
};
typedef compiler_s* compiler_t;

enum compiler_return_e
{   
    COMPILER_RETURN_SUCCESS,
    COMPILER_RETURN_ALLOCATION_ERROR,
    //... 
};

#endif // COMPILER_H