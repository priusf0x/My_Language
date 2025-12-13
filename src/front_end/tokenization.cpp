#include "recursive_decent.h"

#include "Assert.h"
#include "my_string.h"
#include "lexes.h"

#define CURRENT_POSITION (buffer->buffer + buffer->current_position)

// =============================== READ_HELPER ================================

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
        ReadConstFromBuffer(buffer, &token->value.id.id);
    } 
    else
    {
        token->lex_type = LEX_TYPE_ID;
        ReadIdFromBuffer(buffer, &token->value.id.id);
    }
}

recursive_return_e 
DivideInLexems(read_context_t context)
{
    ASSERT(context != NULL);

    token_s token = {};
    buffer_t buffer = context->input_buffer;
    
    do 
    {
        IdentifyLex(&token, context);
        if (VectorPush(&token, context->lex_vector) != 0)
        {
            return RECURSIVE_RETURN_VECTOR_ERROR;
        }
    } 
    while ((*CURRENT_POSITION != 0) && token.lex_type != LEX_TYPE_UNDEFINED);

    return RECURSIVE_RETURN_SUCCESS;
}

// ================================ UNDEFINITION ===============================

#undef CURRENT_POSITION