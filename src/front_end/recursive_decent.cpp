#include "recursive_decent.h"

#include <stdlib.h>
#include <stdio.h>

#include "Assert.h"
#include "buffer.h"
#include "lexes.h"
#include "tools.h"
#include "state_machine_functions.h"
#include "vector.h"

const char* INPUT_FILE_NAME = "pletnev.zov";

#define RETURN_IF_RECURSIVE_ERROR(___X___) \
do\
{\
    recursive_return_e output = ___X___;\
    if (output != RECURSIVE_RETURN_SUCCESS)\
    { return output; }\
} while (0);

// ================================= INIT_HELPERS =============================

static recursive_return_e 
InitMachine(const char*      file_name,
            state_machine_t* state_machine)
{   
    ASSERT(file_name != NULL);
    ASSERT(state_machine != NULL);

    FILE* vector_file = fopen(file_name, "r");
    if (vector_file == NULL)
    {
        return RECURSIVE_RETURN_FILE_OPEN_ERROR;
    }

    state_t state_amount = 0;
    fread(&state_amount, sizeof(state_t), 1, vector_file);
    
    if (StateMachineInit(state_machine, state_amount) != 0)
    {
        return RECURSIVE_RETURN_ALLOCATION_ERROR;
    }

    fread((*state_machine)->data, sizeof(state_t), state_amount 
                * MAX_CHAR_AMOUNT, vector_file);     
    
    if (fclose(vector_file))
    {
        return RECURSIVE_RETURN_FILE_CLOSE_ERROR;
    }
    
    return RECURSIVE_RETURN_SUCCESS;
}

// ================================ INITIALIZATION ============================ 

recursive_return_e 
InitReadContext(read_context_t* context)
{
    ASSERT(context != NULL);

    *context = (read_context_t) calloc(1, sizeof(read_context_s));

    if (*context == NULL)
    {
        return RECURSIVE_RETURN_ALLOCATION_ERROR;
    }

    const size_t start_vector_size = 10;
    if (VectorInit(&(*context)->lex_vector, start_vector_size,
                        sizeof(token_s)) != 0)
    {
        free(*context);
        *context = NULL;

        return RECURSIVE_RETURN_VECTOR_ERROR;
    }

    if(BufferInit(&(*context)->input_buffer, INPUT_FILE_NAME) != 0)
    {
        VectorDestroy(&(*context)->lex_vector);
        free(*context);
        *context = NULL;

        return RECURSIVE_RETURN_BUFFER_ERROR;
    }

    if (InitMachine(KW_FILE_NAME, &(*context)->key_word_machine) 
        || InitMachine(OP_FILE_NAME, &(*context)->operator_machine)
        || InitMachine(SYNT_FILE_NAME, &(*context)->syntax_machine))
    {
        BufferDestroy(&(*context)->input_buffer);
        VectorDestroy(&(*context)->lex_vector);
        free(*context);
        *context = NULL;
    }
    
    return RECURSIVE_RETURN_SUCCESS;
} 

recursive_return_e
DestroyReadContext(read_context_t* context)
{
    if ((context == NULL) || (*context == NULL))
    {
        return RECURSIVE_RETURN_SUCCESS;
    }

    StateMachineDestroy(&(*context)->key_word_machine);
    StateMachineDestroy(&(*context)->operator_machine);
    StateMachineDestroy(&(*context)->syntax_machine);
    BufferDestroy(&(*context)->input_buffer);
    VectorDestroy(&(*context)->lex_vector);
    free(*context);
    *context = NULL;

    return RECURSIVE_RETURN_SUCCESS;
}

// ======================== RECURSIVE_DECENT_ALGORITHM ========================


// =============================== UNDEFINITION ===============================

#undef RETURN_IF_RECURSIVE_ERROR