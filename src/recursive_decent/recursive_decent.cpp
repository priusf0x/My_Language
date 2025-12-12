#include "recursive_decent.h"

#include <cstddef>
#include <exception>
#include <stdlib.h>
#include <stdio.h>

#include "Assert.h"
#include "buffer.h"
#include "lexes.h"
#include "state_machine_functions.h"
#include "tools.h"

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
InitVector(const char*      file_name,
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

    if(BufferInit(&(*context)->input_buffer, INPUT_FILE_NAME) != 0)
    {
        free(*context);
        *context = NULL;

        return RECURSIVE_RETURN_BUFFER_ERROR;
    }

    RETURN_IF_RECURSIVE_ERROR(InitVector(KW_FILE_NAME, 
                                &(*context)->key_word_vector));
    RETURN_IF_RECURSIVE_ERROR(InitVector(OP_FILE_NAME, 
                                &(*context)->operator_vector));
    RETURN_IF_RECURSIVE_ERROR(InitVector(SYNT_FILE_NAME, 
                                &(*context)->syntax_vector));
    
    return RECURSIVE_RETURN_SUCCESS;
} 

recursive_return_e
DestroyReadContext(read_context_t* context)
{
    if ((context == NULL) || (*context == NULL))
    {
        return RECURSIVE_RETURN_SUCCESS;
    }

    StateMachineDestroy(&(*context)->key_word_vector);
    StateMachineDestroy(&(*context)->operator_vector);
    StateMachineDestroy(&(*context)->syntax_vector);
    BufferDestroy(&(*context)->input_buffer);
    free(*context);
    *context = NULL;

    return RECURSIVE_RETURN_SUCCESS;
}

// =============================== READ_HELPER ================================

#define CURRENT_POSITION (buffer->buffer + buffer->current_position)

// temp // temp // temp // temp // temp // temp // temp // temp // temp // temp

static size_t  
CheckIfDigit(const char* string)
{
    ASSERT(string != NULL);

    char character = *string;

    if (('0' <= character) && (character <= '9'))
    {
        return 1; 
    }
    
    return 0;
}

static size_t
CheckIfSymb(const char* string)
{   
    ASSERT(string != NULL);

    char character = *string;

    if ((('a' <= character) && (character <= 'z'))
            || (('A' <= character) && (character <= 'Z')))
    {
        return 1;
    }

    return 0;
}

static size_t 
CheckIfAlNum(const char* string)
{
    ASSERT(string != NULL);

    size_t check_num = CheckIfDigit(string);
    size_t check_symb = CheckIfSymb(string);

    return check_num > check_symb ? check_num : check_symb;
}

// temp // temp // temp // temp // temp // temp // temp // temp // temp // temp

static void
ReadConstFromBuffer(buffer_t  buffer,
                    string_s* const_string)
{
    ASSERT(buffer != NULL);
    ASSERT(const_string != NULL);

    size_t check_output = 0;
    size_t string_length = 0;
    const_string->string_source = CURRENT_POSITION;

    do 
    {
        check_output = CheckIfDigit(CURRENT_POSITION);
        SkipNSymbols(buffer, check_output);
        string_length += check_output;
    } 
    while (check_output);

    const_string->string_size += string_length;
}

static void
ReadIdFromBuffer(buffer_t  buffer,
                  string_s* const_string)
{
    ASSERT(buffer != NULL);
    ASSERT(const_string != NULL);

    size_t check_output = 0;
    size_t string_length = 0;
    const_string->string_source = CURRENT_POSITION;

    do 
    {
        check_output = CheckIfAlNum(CURRENT_POSITION);
        SkipNSymbols(buffer, check_output);
        string_length += check_output;
    } 
    while (check_output);

    const_string->string_size += string_length;
}

// ================================== LEXER ===================================

static int
CheckIfType(buffer_t        buffer,
            state_machine_t state_machine)
{
    ASSERT(buffer != NULL);
    ASSERT(state_machine != NULL);
    
    state_t  current_state = 0;
    char*    string = CURRENT_POSITION;
    size_t   read_amount = 0;

    while (true)
    {
        current_state = GetNextState((unsigned char) *string, 
                                        current_state, state_machine);
        if (current_state & END_STATE_FLAG)
        {
            break;
        }

        read_amount++;
        string++;
    } 
    
    if (current_state & (~END_STATE_FLAG))
    {
        SkipNSymbols(buffer, read_amount);
        SkipSpacesInBuffer(buffer);
    }

    return current_state & (~END_STATE_FLAG);
}

static void
IdentifyLex(token_s*       token,
            read_context_t context)
{
    ASSERT(token != NULL);
    ASSERT(context != NULL);

    int output = 0;

    buffer_t buffer = context->input_buffer;

    if ((output = CheckIfType(buffer, context->key_word_vector)))
    {   
        token->lex_type = LEX_TYPE_KEY_WORD;
        token->value.key_word = (key_word_type_e) output;
    }
    else if ((output = CheckIfType(buffer, context->operator_vector)))
    {
        token->lex_type = LEX_TYPE_OPERATOR;
        token->value.op = (operator_type_e) output;
    }
    else if ((output = CheckIfType(buffer, context->syntax_vector)))
    {
        token->lex_type = LEX_TYPE_SYNTAX;
        token->value.syntax = (syntax_type_e) output;
    }
    else if (CheckIfDigit(CURRENT_POSITION))
    {
        token->lex_type = LEX_TYPE_CONST;
        ReadConstFromBuffer(buffer, &token->value.string);
    } 
    else
    {
        token->lex_type = LEX_TYPE_ID;
        ReadIdFromBuffer(buffer, &token->value.string);
    }
}

recursive_return_e 
DivideInLexems(read_context_t context)
{
    ASSERT(input_string != NULL);

    token_s token = {};

    IdentifyLex(&token, context);

    return RECURSIVE_RETURN_SUCCESS;
}


// =============================== UNDEFINITION ===============================

#undef RETURN_IF_RECURSIVE_ERROR
#undef CURRENT_POSITION