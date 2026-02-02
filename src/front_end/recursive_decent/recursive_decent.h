#ifndef RECURSIVE_DECENT_H
#define RECURSIVE_DECENT_H

#include <stdlib.h>
#include <system_error>

#include "name_space.h"
#include "state_machine_functions.h" 
#include "buffer.h"
#include "vector.h"
#include "tree.h"

enum recursive_return_e 
{
    RECURSIVE_RETURN_SUCCESS,
    RECURSIVE_RETURN_ALLOCATION_ERROR,
    RECURSIVE_RETURN_VECTOR_ERROR,
    RECURSIVE_RETURN_BUFFER_ERROR,
    RECURSIVE_RETURN_FILE_OPEN_ERROR,
    RECURSIVE_RETURN_FILE_CLOSE_ERROR,
    RECURSIVE_RETURN_STATE_MACHINE_ERROR,
    RECURSIVE_RETURN_TREE_ERROR,
    RECURSIVE_RETURN_READ_ERROR,
    RECURSIVE_RETURN_NAME_TABLE_ERROR
};

struct read_context_s 
{
    buffer_t           input_buffer;
    state_machine_t    key_word_machine;
    state_machine_t    operator_machine;
    state_machine_t    syntax_machine;
    vector_t           lex_vector;
    tree_t             lex_tree;
    name_table_t       name_table;
    recursive_return_e status;
    size_t             last_read_pos;
    const char*        file_name;
};

struct scope_s
{
    ssize_t scope;
    size_t  memory_size;
    bool    is_global;
};
typedef read_context_s* read_context_t;

// ============================= MEMORY_CONTROLLING ===========================

recursive_return_e 
ReadContextCtor(read_context_t* context,
                const char*     input_file_name);

recursive_return_e
ReadContextDtor(read_context_t* context);

// ================================== ANALYSIS ================================

recursive_return_e 
DivideInLexems(read_context_t context);

recursive_return_e 
DoSyntaxAnalysis(read_context_t context);
    
// ============================================================================

#endif // RECURSIVE_DECENT_H
