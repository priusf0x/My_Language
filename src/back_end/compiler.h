#ifndef COMPILER_H
#define COMPILER_H

#include <stdlib.h>

#include "tree.h"
#include "buffer.h"

struct compiler_s 
{  
    buffer_t buffer;
    tree_t   compiler_tree;
    FILE*    file_output;
    size_t   label_count;
};
typedef compiler_s* compiler_t;

enum compiler_return_e
{   
    COMPILER_RETURN_SUCCESS,
    COMPILER_RETURN_ALLOCATION_ERROR,
    COMPILER_RETURN_BUFFER_ERROR,
    COMPILER_RETURN_AST_STANDARD_ERROR,
    COMPILER_RETURN_FILE_OPEN_ERROR,
    COMPILER_RETURN_FILE_CLOSE_ERROR,
    COMPILER_RETURN_UNDEFINED_ELEMENT,
    COMPILER_RETURN_SEMANTIC_ERROR
};

// =========================== MEMORY_CONTROLLING =============================

compiler_return_e
CompilerCtor(const char* input_name,
             const char* output_name,
             compiler_t* compiler);

compiler_return_e
CompilerDtor(compiler_t* compiler);

// =============================== COMPILE_AST ================================

compiler_return_e
CompileAST(compiler_t compiler);

#endif // COMPILER_H