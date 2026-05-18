#ifndef COMPILER_H
#define COMPILER_H

#include <cstdint>
#include <stdlib.h>

#include "my_elf.h"
#include "tree.h"
#include "buffer.h"
#include "list.h"
#include "hashtable.h"

struct placeholders_s 
{
    list_t list;
    size_t func;
    size_t var;
};

struct compiler_s 
{
    buffer_t       buffer;
    tree_t         compiler_tree;
    FILE*          file_output;
    size_t         label_count;
    section_t      code_section;
    elf_t          elf;
    placeholders_s placehldr;
    hashtable_t    name_table;
};
typedef compiler_s* compiler_t;

enum compiler_return_e
{   
    COMPILER_RETURN_SUCCESS,
    COMPILER_RETURN_ALLOCATION_ERROR,
    COMPILER_RETURN_BUFFER_ERROR,
    COMPILER_RETURN_TREE_ERROR,
    COMPILER_RETURN_AST_STANDARD_ERROR,
    COMPILER_RETURN_FILE_OPEN_ERROR,
    COMPILER_RETURN_FILE_CLOSE_ERROR,
    COMPILER_RETURN_INCORRECT_AST,
    COMPILER_RETURN_SECTION_ERROR,
    COMPILER_RETURN_LIST_ERROR,
    COMPILER_RETURN_HT_ERROR
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

// ============================================================================

#endif // COMPILER_H