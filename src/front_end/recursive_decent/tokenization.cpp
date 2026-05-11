#include "recursive_decent.h"

#include <assert.h>

#include "buffer.h"
#include "my_string.h"
#include "lexes.h"
#include "tree.h"

#define CURRENT_POSITION (buffer->buffer + buffer->current_position)

// =============================== READ_HELPER ================================

// temp // temp // temp // temp // temp // temp // temp // temp // temp // temp

static size_t  
CheckIfDigit(const char* string)
{
    assert(string != NULL);

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
    assert(string != NULL);

    char character = *string;

    if ((('a' <= character) && (character <= 'z'))
            || (('A' <= character) && (character <= 'Z'))
            || (character == '_'))
    {
        return 1;
    }

    return 0;
}

static size_t 
CheckIfAlNum(const char* string)
{
    assert(string != NULL);

    size_t check_num = CheckIfDigit(string);
    size_t check_symb = CheckIfSymb(string);

    return check_num > check_symb ? check_num : check_symb;
}

// temp // temp // temp // temp // temp // temp // temp // temp // temp // temp

static void
ReadConstFromBuffer(buffer_t  buffer,
                    long*      constant)
{
    assert(buffer != NULL);
    assert(constant != NULL);

    char* end_ptr = NULL;

    *constant = (int) strtol(CURRENT_POSITION, &end_ptr, 0);

    buffer->current_position += (size_t) (end_ptr - CURRENT_POSITION);
}

static void
ReadIdFromBuffer(buffer_t  buffer,
                 string_s* id_string)
{
    assert(buffer != NULL);
    assert(id_string != NULL);

    size_t check_output = 0;
    size_t string_length = 0;
    id_string->string = CURRENT_POSITION;

    do 
    {
        check_output = CheckIfAlNum(CURRENT_POSITION);
        SkipNSymbolsB(buffer, check_output);
        string_length += check_output;
    }
    while (check_output);

    id_string->size = string_length;
}

// ================================== LEXER ===================================

static int
CheckIfType(buffer_t        buffer,
            state_machine_t state_machine)
{
    assert(buffer != NULL);
    assert(state_machine != NULL);
    
    state_t current_state = 0;
    char*   string = CURRENT_POSITION;
    size_t  read_amount = 0;

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
        SkipNSymbolsB(buffer, read_amount);
    }
    
    return current_state & (~END_STATE_FLAG);
}

static void
IdentifyLex(token_s*       token,
            read_context_t context)
{
    assert(token != NULL);
    assert(context != NULL);

    int output = 0;

    buffer_t buffer = context->input_buffer;

    if ((output = CheckIfType(buffer, context->key_word_machine)))
    {   
        token->lex_type = LEX_TYPE_KEY_WORD;
        token->value.key_word = (key_word_type_e) output;
    }
    else if ((output = CheckIfType(buffer, context->operator_machine)))
    {
        token->lex_type = LEX_TYPE_OPERATOR;
        token->value.op = (operator_type_e) output;
    }
    else if ((output = CheckIfType(buffer, context->syntax_machine)))
    {
        token->lex_type = LEX_TYPE_SYNTAX;
        token->value.syntax = (syntax_type_e) output;
    }
    else if (CheckIfDigit(CURRENT_POSITION))
    {
        token->lex_type = LEX_TYPE_CONST;
        ReadConstFromBuffer(buffer, &token->value.constant);
    } 
    else if (CheckIfSymb(CURRENT_POSITION))
    {  
        token->lex_type = LEX_TYPE_ID;
        ReadIdFromBuffer(buffer, &token->value.id.id);
        token->value.id.info1 = NO_LINK;
        token->value.id.info2 = NO_LINK;
    }
    else
    {
        token->lex_type = LEX_TYPE_UNDEFINED;
    }
}

recursive_return_e 
DivideInLexems(read_context_t context)
{
    assert(context != NULL);

    token_s token = {};
    buffer_t buffer = context->input_buffer;
    SkipSpacesB(context->input_buffer);

    do 
    {
        token.buf_pos = context->input_buffer->current_position;
        IdentifyLex(&token, context);
        if (VectorPush(&token, context->lex_vector) != 0)
        {
            return RECURSIVE_RETURN_VECTOR_ERROR;
        }
        SkipSpacesB(buffer);
    } 
    while ((*CURRENT_POSITION != 0) 
                && (token.lex_type != LEX_TYPE_UNDEFINED));

    return RECURSIVE_RETURN_SUCCESS;
}

// ================================ UNDEFINITION ===============================

#undef CURRENT_POSITION