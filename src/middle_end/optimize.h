#ifndef OPTIMIZE_H
#define OPTIMIZE_H

#include "tree.h"
#include "buffer.h"

enum optimize_return_e
{
    OPTIMIZE_RETURN_SUCCESS,
    OPTIMIZE_RETURN_ALLOCATION_ERROR,
    OPTIMIZE_RETURN_BUFFER_ERROR,
    OPTIMIZE_RETURN_TREE_ERROR,
    OPTIMIZE_RETURN_AST_STANDARD_ERROR,
    OPTIMIZE_RETURN_UNEXPECTED_ELEMENT,
    OPTIMIZE_RETURN_ZERO_DIVISION
};

struct optimizer_s 
{   
    buffer_t buffer;
    tree_t   ast_tree;      
    optimize_return_e state;
};
typedef optimizer_s* optimizer_t; 

// ============================ MEMORY_CONTROLLING ============================

optimize_return_e
OptimizerCtor(const char* input_name, optimizer_t* optimizer);

optimize_return_e 
OptimizerDtor(optimizer_t* optimizer);

optimize_return_e
OptimizeAST(optimizer_t optimizer);

// ============================================================================

#endif // OPTIMIZE_H