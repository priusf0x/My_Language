#ifndef RECURSIVE_DECENT_H
#define RECURSIVE_DECENT_H

#include <stdlib.h>

#include "state_machine_functions.h" 
#include "buffer.h"

enum recursive_return_e 
{
    RECURSIVE_RETURN_SUCCESS,
    RECURSIVE_RETURN_ALLOCATION_ERROR,
    RECURSIVE_RETURN_BUFFER_ERROR,
    RECURSIVE_RETURN_FILE_OPEN_ERROR,
    RECURSIVE_RETURN_FILE_CLOSE_ERROR,
    RECURSIVE_RETURN_STATE_MACHINE_ERRORS
};


struct read_context_s 
{
    buffer_t        input_buffer;
    state_machine_t key_word_vector;
    state_machine_t operator_vector;
    state_machine_t syntax_vector;
};

typedef read_context_s* read_context_t;

recursive_return_e 
InitReadContext(read_context_t* context);

recursive_return_e
DestroyReadContext(read_context_t* context);

recursive_return_e 
DivideInLexems(read_context_t context);
    
#endif // RECURSIVE_DECENT_H
