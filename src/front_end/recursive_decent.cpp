#include "recursive_decent.h"

#include <cstdio>
#include <stdlib.h>
#include <stdio.h>

#include "Assert.h"
#include "buffer.h"
#include "lexes.h"
#include "recursive_decent_defines.h"
#include "tools.h"
#include "state_machine_functions.h"
#include "vector.h"
#include "tree.h"

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

static ssize_t GetStatement(read_context_t context);

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

        return RECURSIVE_RETURN_STATE_MACHINE_ERROR;
    }

    DivideInLexems(*context);

    TreeInit(&(*context)->lex_tree, 10);

    ssize_t meow =  GetStatement(*context);
    (*context)->lex_tree->nodes_array[0].left_index = meow; 
    
    fprintf(stderr, "%zu", (*context)->last_read_pos);

    TreeDump((*context)->lex_tree);
    
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
    TreeDestroy(&(*context)->lex_tree);
    free(*context);
    *context = NULL;

    return RECURSIVE_RETURN_SUCCESS;
}

// ======================== RECURSIVE_DECENT_ALGORITHM ========================

#define RETURN_IF_ERROR do {if (context->status != 0) {return NO_LINK;}} while(0)
#define VECTOR_VIEW(_T0KEN_) do{RETURN_IF_ERROR;\
                                if (!VectorViewValue(&_T0KEN_, context->lex_vector))\
                                { \
                                  context->last_read_pos = (_T0KEN_).buf_pos;\
                                }\
                                } while (0)
#define VECTOR_ERASE do{RETURN_IF_ERROR;\
                        if (VectorEraseFirst(context->lex_vector))\
                        {\
                          context->status = RECURSIVE_RETURN_READ_ERROR;\
                          return NO_LINK;}\
                        } while (0)

static ssize_t GetAssigmentExpression(read_context_t);
static ssize_t GetExpression(read_context_t context);
static ssize_t GetTerm(read_context_t context);
static ssize_t GetBool(read_context_t context);
static ssize_t GetPrimary(read_context_t context);
static ssize_t GetFunctionArg(read_context_t context);
static ssize_t GetReturn(read_context_t context);
static ssize_t GetInitVar(read_context_t context);
static ssize_t GetFuncDefinition(read_context_t context);
static ssize_t GetIfWhile(read_context_t context);
// static ssize_t GetStatement(read_context_t context);

static ssize_t 
GetFunctionArg(read_context_t context)
{
    ASSERT(context != NULL);
    RETURN_IF_ERROR;

    ssize_t return_node = GetExpression(context); 
    ssize_t arg_connector = NO_LINK; 
    token_s token = {};
    VectorViewValue(&token, context->lex_vector);

    while ((token.lex_type == LEX_TYPE_SYNTAX)
                && (token.value.syntax == SYNTAX_ARG_CONNECTOR))
    {
        VECTOR_ERASE;
        arg_connector = ARG_CON;
        CONNECT_LEXES(arg_connector, GetExpression(context), return_node);
        return_node = arg_connector;
        VECTOR_VIEW(token);
    }

    return return_node;
}

static ssize_t
GetPrimary(read_context_t context)
{
    ASSERT(context != NULL);
    RETURN_IF_ERROR;

    token_s token = {};
    VECTOR_VIEW(token);

    if ((token.lex_type == LEX_TYPE_SYNTAX) 
            && (token.value.syntax == SYNTAX_START_BRACKET))
    {
        VECTOR_ERASE;

        ssize_t new_node = GetExpression(context);

        VECTOR_VIEW(token);
        if ((token.lex_type != LEX_TYPE_SYNTAX) 
                || (token.value.syntax!= SYNTAX_END_BRACKET))
        {
            context->status = RECURSIVE_RETURN_READ_ERROR;
            
            return NO_LINK;
        }
        
        VECTOR_ERASE;
        return new_node;
    }
    else if (token.lex_type == LEX_TYPE_ID)
    {
        ssize_t id_node = ADD__(token);
        VECTOR_ERASE;
        VECTOR_VIEW(token);
        
        if ((token.lex_type == LEX_TYPE_SYNTAX) 
                && (token.value.syntax == SYNTAX_START_BRACKET))
        {
            VECTOR_ERASE;
            ssize_t new_node = GetFunctionArg(context);
            CONNECT_LEXES(id_node, new_node, NO_LINK);
            VECTOR_VIEW(token);

            if ((token.lex_type != LEX_TYPE_SYNTAX) 
                    || (token.value.syntax != SYNTAX_END_BRACKET))
            {
                context->status = RECURSIVE_RETURN_READ_ERROR;
                
                return NO_LINK;
            }

            VECTOR_ERASE;
        }
        
        return id_node;
    }
    else if (token.lex_type == LEX_TYPE_CONST)
    {
        VECTOR_ERASE;

        return ADD__(token);
    }

    return NO_LINK;
}

static bool
CheckIfBoolOp(const token_s* token)
{
    ASSERT(token != NULL);

    if (token->lex_type != LEX_TYPE_OPERATOR)
    {
        return false;
    }

    operator_type_e op = token->value.op;

    if (   (op == OPERATOR_EQUALITY)     || (op == OPERATOR_N_EQUALITY)
        || (op == OPERATOR_MORE)         || (op == OPERATOR_MORE_OR_EQ)   
        || (op == OPERATOR_LESS)         || (op == OPERATOR_LESS_OR_EQUAL))
    {
        return true;
    }

    return false;
}

static ssize_t 
GetBool(read_context_t context)
{
    ASSERT(context != NULL);
    RETURN_IF_ERROR;
    
    ssize_t return_node = GetPrimary(context); 
    ssize_t bool_op = NO_LINK; 
    token_s token = {};
    VectorViewValue(&token, context->lex_vector);

    while (CheckIfBoolOp(&token))
    {
        VECTOR_ERASE;
        bool_op = ADD__(token);
        CONNECT_LEXES(bool_op, GetPrimary(context), return_node);
        return_node = bool_op;
        VECTOR_VIEW(token);
    }

    return return_node;
}

static bool
CheckIfMulDivOp(const token_s* token)
{
    ASSERT(token != NULL);

    if (token->lex_type != LEX_TYPE_OPERATOR)
    {
        return false;
    }

    operator_type_e op = token->value.op;

    if ((op == OPERATOR_DIV) || (op == OPERATOR_MUL))
    {
        return true;
    }

    return false;
}

static ssize_t 
GetTerm(read_context_t context)
{
    ASSERT(context != NULL);
    RETURN_IF_ERROR;

    ssize_t return_node = GetBool(context); 
    ssize_t mul_div_op = NO_LINK; 
    token_s token = {};
    VECTOR_VIEW(token);

    while (CheckIfMulDivOp(&token))
    {
        VECTOR_ERASE;
        mul_div_op = ADD__(token);
        CONNECT_LEXES(mul_div_op, return_node, GetBool(context));
        return_node = mul_div_op;
        VECTOR_VIEW(token);
    }

    return return_node;
}

static bool
CheckIfPlusMinusOp(const token_s* token)
{
    ASSERT(token != NULL);

    if (token->lex_type != LEX_TYPE_OPERATOR)
    {
        return false;
    }

    operator_type_e op = token->value.op;

    if ((op == OPERATOR_PLUS) || (op == OPERATOR_MINUS))
    {
        return true;
    }

    return false;
}

static ssize_t 
GetAssigmentExpression(read_context_t context)
{
    ASSERT(context != NULL);
    
    token_s assigment_token = {};
    VECTOR_VIEW(assigment_token);
    ssize_t assigment_node = ADD__(assigment_token);
    VECTOR_ERASE;

    CONNECT_LEXES(assigment_node, NO_LINK, GetExpression(context));

    return assigment_node;
}

static ssize_t 
GetExpression(read_context_t context)
{
    ASSERT(context != NULL);
    RETURN_IF_ERROR;

    token_s token = {};
    ssize_t return_node = GetTerm(context); 
    ssize_t plus_minus_op = NO_LINK; 
    VECTOR_VIEW(token);

    if ((token.lex_type == LEX_TYPE_OPERATOR)
            && (token.value.op == OPERATOR_ASSIGNMENT))
    {
        ssize_t assigment_node = GetAssigmentExpression(context); 
        CONNECT_LEXES(assigment_node, return_node, NO_LINK);  
        
        return assigment_node;
    }

    while (CheckIfPlusMinusOp(&token))
    {
        VECTOR_ERASE;
        plus_minus_op = ADD__(token);
        CONNECT_LEXES(plus_minus_op, return_node, GetTerm(context));
        return_node = plus_minus_op;
        VECTOR_VIEW(token);
    }

    return return_node;
}

static ssize_t 
GetReturn(read_context_t context)
{
    ASSERT(context != NULL);

    token_s return_token = {};
    VECTOR_VIEW(return_token);
    VECTOR_ERASE;
    ssize_t return_node = ADD__(return_token);
    CONNECT_LEXES(return_node, GetExpression(context), NO_LINK);

    return return_node;
}

static ssize_t 
GetInitVar(read_context_t context)
{
    ASSERT(context != NULL);

    token_s var_kw_token = {};
    VECTOR_VIEW(var_kw_token);
    VECTOR_ERASE;
    ssize_t var_kw_node = ADD__(var_kw_token);
    
    token_s id_token = {};
    VECTOR_VIEW(id_token);
    if (id_token.lex_type != LEX_TYPE_ID)
    {   
        context->status = RECURSIVE_RETURN_READ_ERROR;
        
        return NO_LINK;
    }
    VECTOR_ERASE;
    ssize_t var_node = ADD__(id_token);


    token_s a_token = {};             
    VECTOR_VIEW(a_token);
    if ((a_token.lex_type == LEX_TYPE_OPERATOR)
            && (a_token.value.op == OPERATOR_ASSIGNMENT))
    {
        VECTOR_ERASE;
        CONNECT_LEXES(var_kw_node, NO_LINK, GetExpression(context));
    }
    
    CONNECT_LEXES(var_kw_node, var_node, NO_LINK);
    
    return var_kw_node;
}

static ssize_t 
GetFuncDefinition(read_context_t context)
{
    ASSERT(context != NULL);

    token_s function_kw_token = {};
    VECTOR_VIEW(function_kw_token);
    VECTOR_ERASE;
    ssize_t function_kw_node = ADD__(function_kw_token);
    
    token_s id_token = {};
    VECTOR_VIEW(id_token);
    ssize_t id_node = ADD__(id_token);
    if (id_token.lex_type != LEX_TYPE_ID)
    {
        context->status = RECURSIVE_RETURN_READ_ERROR;
        
        return NO_LINK;
    } 
    VECTOR_ERASE;

    token_s syntax_token = {};
    VECTOR_VIEW(syntax_token);
    if ((syntax_token.lex_type != LEX_TYPE_SYNTAX)
            || (syntax_token.value.syntax != SYNTAX_START_BRACKET))
    {
        context->status = RECURSIVE_RETURN_READ_ERROR;
        
        return NO_LINK;
    }
    VECTOR_ERASE;

    ssize_t var_node = GetInitVar(context);
    ssize_t arg_connector = NO_LINK; 
    token_s token = {};
    VECTOR_VIEW(token);
    while ((token.lex_type == LEX_TYPE_SYNTAX)
                && (token.value.syntax == SYNTAX_ARG_CONNECTOR))
    {
        VECTOR_ERASE;
        arg_connector = ARG_CON;
        CONNECT_LEXES(arg_connector, GetInitVar(context), var_node);
        var_node = arg_connector;
        VECTOR_VIEW(token);
    }
    CONNECT_LEXES(id_node, var_node, NO_LINK);

    VECTOR_VIEW(syntax_token);
    if ((syntax_token.lex_type != LEX_TYPE_SYNTAX)
            || (syntax_token.value.syntax != SYNTAX_END_BRACKET))
    {
        context->status = RECURSIVE_RETURN_READ_ERROR;
        
        return NO_LINK;
    }
    VECTOR_ERASE;

    CONNECT_LEXES(function_kw_node, id_node, GetStatement(context));

    return function_kw_node;
}

static ssize_t 
GetIfWhile(read_context_t context)
{
    ASSERT(context != NULL);

    token_s function_kw_token = {};
    VECTOR_VIEW(function_kw_token);
    VECTOR_ERASE;
    ssize_t whileif_kw_node = ADD__(function_kw_token);

    token_s syntax_token = {};
    VECTOR_VIEW(syntax_token);
    if ((syntax_token.lex_type != LEX_TYPE_SYNTAX)
            || (syntax_token.value.syntax != SYNTAX_START_BRACKET))
    {
        context->status = RECURSIVE_RETURN_READ_ERROR;
        
        return NO_LINK;
    }
    VECTOR_ERASE;

    ssize_t condition_node = GetExpression(context);

    VECTOR_VIEW(syntax_token);
    if ((syntax_token.lex_type != LEX_TYPE_SYNTAX)
            || (syntax_token.value.syntax != SYNTAX_END_BRACKET))
    {
        context->status = RECURSIVE_RETURN_READ_ERROR;
        
        return NO_LINK;
    }
    VECTOR_ERASE;

    CONNECT_LEXES(whileif_kw_node, condition_node, GetStatement(context));

    return whileif_kw_node;
}

static ssize_t 
GetStatement(read_context_t context)
{
    ASSERT(context != NULL);

    token_s token = {};
    ssize_t return_node = NO_LINK;
    VECTOR_VIEW(token);

    if (token.lex_type == LEX_TYPE_KEY_WORD)
    {
        if ((token.value.key_word == KEY_WORD_IF)
                || (token.value.key_word == KEY_WORD_WHILE))
        {
            return_node = GetIfWhile(context);
        }
        else if (token.value.key_word == KEY_WORD_VAR)
        {
            return_node = GetInitVar(context);

            VECTOR_VIEW(token);
            if ((token.lex_type != LEX_TYPE_SYNTAX)
                 && (token.value.syntax != SYNTAX_STATEMENT_CONNECTOR))
            { 
                context->status = RECURSIVE_RETURN_READ_ERROR;
                
                return NO_LINK;
            }
            VECTOR_ERASE;
        }
        else if (token.value.key_word == KEY_WORD_FUNCTION)
        {
            return_node = GetFuncDefinition(context);         
            
            VECTOR_VIEW(token);
        }
        else if (token.value.key_word == KEY_WORD_RETURN)
        {
            return_node = GetReturn(context);
                
            VECTOR_VIEW(token);
            if ((token.lex_type != LEX_TYPE_SYNTAX)
                 && (token.value.syntax != SYNTAX_STATEMENT_CONNECTOR))
            {
                context->status = RECURSIVE_RETURN_READ_ERROR;

                return NO_LINK;
            }
            VECTOR_ERASE;
            
        }
        else
        {
            context->status = RECURSIVE_RETURN_READ_ERROR;
        
            return NO_LINK;
        }
    }
    else if ((token.lex_type == LEX_TYPE_SYNTAX)
                && (token.value.syntax == SYNTAX_START_BODY))
    {
        VECTOR_ERASE;
        return_node = GetStatement(context);
        ssize_t statement_connector = NO_LINK;
        VECTOR_VIEW(token);
        
        while ((token.lex_type != LEX_TYPE_SYNTAX) 
                    ||   (token.value.syntax != SYNTAX_END_BODY))
        {
            statement_connector = STMT_CON;
            CONNECT_LEXES(statement_connector, GetStatement(context), 
                            return_node);
            return_node = statement_connector;
            VECTOR_VIEW(token);
        }

        VECTOR_ERASE;
    }
    else
    {
        return_node = GetExpression(context);

        VECTOR_VIEW(token); 
        if ((token.lex_type != LEX_TYPE_SYNTAX)
             || (token.value.syntax != SYNTAX_STATEMENT_CONNECTOR))
        {
            context->status = RECURSIVE_RETURN_READ_ERROR;
            
            return NO_LINK;
        }
        VECTOR_ERASE;
    }

    return return_node;
}

static 

// =============================== UNDEFINITION ===============================

#undef RETURN_IF_ERROR
#undef RETURN_IF_RECURSIVE_ERROR