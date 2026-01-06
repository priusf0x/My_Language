#include "recursive_decent.h"

#include <cstddef>
#include <stdlib.h>
#include <stdio.h>

#include "Assert.h"
#include "buffer.h"
#include "lexes.h"
#include "name_space.h"
#include "recursive_decent_defines.h"
#include "tools.h"
#include "state_machine_functions.h"
#include "vector.h"
#include "tree.h"

const char* INPUT_FILE_NAME = "pletnev.zov";

struct scope_context_s
{
    ssize_t scope;
    size_t  id_num;
};
typedef scope_context_s* scope_t;


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

static ssize_t GetGlobal(read_context_t context);

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

    const size_t name_table_size = 10;
    if (InitNameTable(&(*context)->name_table, name_table_size) != 0)
    {
        BufferDestroy(&(*context)->input_buffer);
        VectorDestroy(&(*context)->lex_vector);
        free(*context);
        *context = NULL;
        
        return RECURSIVE_RETURN_NAME_SPACE_ERROR;
    }

    if (InitMachine(KW_FILE_NAME, &(*context)->key_word_machine) 
        || InitMachine(OP_FILE_NAME, &(*context)->operator_machine)
        || InitMachine(SYNT_FILE_NAME, &(*context)->syntax_machine))
    {
        BufferDestroy(&(*context)->input_buffer);
        VectorDestroy(&(*context)->lex_vector);
        DestroyNameTable(&(*context)->name_table);
        free(*context);
        *context = NULL;

        return RECURSIVE_RETURN_STATE_MACHINE_ERROR;
    }

    DivideInLexems(*context);

    TreeInit(&(*context)->lex_tree, 10);

    ssize_t meow =  GetGlobal(*context);
    (*context)->lex_tree->nodes_array[0].left_index = meow; 
    
    NameTableDump((*context)->name_table);

    fprintf(stderr, "%zu", (*context)->last_read_pos);

    TreeDump((*context)->lex_tree);
    TreeBaseDump((*context)->lex_tree);
    
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
    DestroyNameTable(&(*context)->name_table);
    
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

static ssize_t GetAssigmentExpression(read_context_t context, scope_s* scope);
static ssize_t GetExpression         (read_context_t context, scope_s* scope);
static ssize_t GetTerm               (read_context_t context, scope_s* scope);
static ssize_t GetBool               (read_context_t context, scope_s* scope);
static ssize_t GetPrimary            (read_context_t context, scope_s* scope);
static ssize_t GetFunctionArg        (read_context_t context, scope_s* scope);
static ssize_t GetReturn             (read_context_t context, scope_s* scope);
static ssize_t GetInitVar            (read_context_t context, scope_s* scope);
static ssize_t GetFuncDefinition     (read_context_t context, scope_s* scope);
static ssize_t GetIfWhile            (read_context_t context, scope_s* scope);
static ssize_t GetStatement          (read_context_t context, scope_s* scope);

static ssize_t 
GetFunctionArg(read_context_t context,
               scope_s*       scope)
{
    ASSERT(context != NULL);
    ASSERT(scope != NULL);

    ssize_t arg_connector = ARG_CON;
    CONNECT_LEXES(arg_connector, NO_LINK, GetExpression(context, scope)); 
    ssize_t return_node = arg_connector; 
    ssize_t last_connector = arg_connector;
    token_s token = {};
    VECTOR_VIEW(token);

    while ((token.lex_type == LEX_TYPE_SYNTAX)
                && (token.value.syntax == SYNTAX_ARG_CONNECTOR))
    {
        VECTOR_ERASE;
        arg_connector = ARG_CON;
        CONNECT_LEXES(arg_connector, NO_LINK, GetExpression(context, scope));
        CONNECT_LEXES(last_connector, arg_connector, NO_LINK);
        last_connector = arg_connector;
        VECTOR_VIEW(token);
    }
    
    return return_node;
}

static ssize_t
GetPrimary(read_context_t context,
           scope_s*       scope)
{
    ASSERT(context != NULL);
    ASSERT(scope != NULL);

    token_s token = {};
    VECTOR_VIEW(token);

    if ((token.lex_type == LEX_TYPE_SYNTAX) 
            && (token.value.syntax == SYNTAX_START_BRACKET))
    {
        VECTOR_ERASE;

        ssize_t new_node = GetExpression(context, scope);

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
        ssize_t name_table_number = GetNameNum(&token.value.id.id, scope->scope, 
                                                        context->name_table);
        if (name_table_number == NO_LINK)
        {
            // TODO: error system;
            return NO_LINK;
        }
                                        
        name_s name = context->name_table->name_array[name_table_number];
        token.value.id.number_in_scope = (ssize_t) name.info_num;
        token.value.id.is_global = name.is_global;

        ssize_t id_node = ADD__(token);
        
        VECTOR_ERASE;
        VECTOR_VIEW(token);
        
        if ((token.lex_type == LEX_TYPE_SYNTAX) 
                && (token.value.syntax == SYNTAX_START_BRACKET))
        { 
            VECTOR_ERASE;

            node_s* array = context->lex_tree->nodes_array; 
            array[id_node].node_value.value.id.is_function = true;
            
            ssize_t new_node = GetFunctionArg(context, scope);
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
GetBool(read_context_t context,
        scope_s*       scope)
{
    ASSERT(context != NULL);
    ASSERT(scope != NULL);
    
    ssize_t return_node = GetPrimary(context, scope); 
    ssize_t bool_op = NO_LINK; 
    token_s token = {};
    VectorViewValue(&token, context->lex_vector);

    while (CheckIfBoolOp(&token))
    {
        VECTOR_ERASE;
        bool_op = ADD__(token);
        CONNECT_LEXES(bool_op, GetPrimary(context, scope), return_node);
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
GetTerm(read_context_t context,
        scope_s*       scope)
{
    ASSERT(context != NULL);
    ASSERT(scope != NULL);

    ssize_t return_node = GetBool(context, scope); 
    ssize_t mul_div_op = NO_LINK; 
    token_s token = {};
    VECTOR_VIEW(token);

    while (CheckIfMulDivOp(&token))
    {
        VECTOR_ERASE;
        mul_div_op = ADD__(token);
        CONNECT_LEXES(mul_div_op, return_node, GetBool(context, scope));
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
GetAssigmentExpression(read_context_t context,
                       scope_s*       scope)
{
    ASSERT(context != NULL);
    ASSERT(scope != NULL);
    
    token_s assigment_token = {};
    VECTOR_VIEW(assigment_token);
    ssize_t assigment_node = ADD__(assigment_token);
    VECTOR_ERASE;

    CONNECT_LEXES(assigment_node, NO_LINK, GetExpression(context, scope));

    return assigment_node;
}

static ssize_t 
GetExpression(read_context_t context,
              scope_s*       scope)
{
    ASSERT(context != NULL);
    ASSERT(scope != NULL);

    token_s token = {};
    ssize_t return_node = GetTerm(context, scope); 
    ssize_t plus_minus_op = NO_LINK; 
    VECTOR_VIEW(token);

    if ((token.lex_type == LEX_TYPE_OPERATOR)
            && (token.value.op == OPERATOR_ASSIGNMENT))
    {
        ssize_t assigment_node = GetAssigmentExpression(context, scope); 
        CONNECT_LEXES(assigment_node, return_node, NO_LINK);  
        
        return assigment_node;
    }

    while (CheckIfPlusMinusOp(&token))
    {
        VECTOR_ERASE;
        plus_minus_op = ADD__(token);
        CONNECT_LEXES(plus_minus_op, return_node, GetTerm(context, scope));
        return_node = plus_minus_op;
        VECTOR_VIEW(token);
    }

    return return_node;
}

static ssize_t 
GetReturn(read_context_t context,
          scope_s*       scope)
{
    ASSERT(context != NULL);
    ASSERT(scope != NULL);

    token_s return_token = {};
    VECTOR_VIEW(return_token);
    VECTOR_ERASE;
    ssize_t return_node = ADD__(return_token);
    CONNECT_LEXES(return_node, GetExpression(context, scope), NO_LINK);

    return return_node;
}

static ssize_t 
GetInitVar(read_context_t context,
           scope_s*       scope)
{
    ASSERT(context != NULL);
    ASSERT(scope != NULL);

    token_s var_kw_token = {};
    VECTOR_VIEW(var_kw_token);

    if ((var_kw_token.lex_type != LEX_TYPE_KEY_WORD)
            || (var_kw_token.value.key_word != KEY_WORD_VAR))
    {
        return NO_LINK;
    }

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

    node_s* node_array = context->lex_tree->nodes_array;
    node_array[var_node].node_value.value.id.is_global = scope->is_global;
    node_array[var_node].node_value.value.id.number_in_scope 
                                                = (ssize_t)scope->variable_count;
    InitNewVar(var_node, scope, context);

    token_s a_token = {};             
    VECTOR_VIEW(a_token);
    if ((a_token.lex_type == LEX_TYPE_OPERATOR)
            && (a_token.value.op == OPERATOR_ASSIGNMENT))
    {
        VECTOR_ERASE;
        CONNECT_LEXES(var_kw_node, NO_LINK, GetExpression(context, scope));
    }
    
    CONNECT_LEXES(var_kw_node, var_node, NO_LINK);
    
    return var_kw_node;
}

static ssize_t 
GetFuncDefinition(read_context_t context,
                  scope_s*       scope)
{
    ASSERT(context != NULL);
    ASSERT(scope != NULL);

    token_s function_kw_token = {};
    VECTOR_VIEW(function_kw_token);
    VECTOR_ERASE;
    ssize_t function_kw_node = ADD__(function_kw_token);
    
    token_s id_token = {};
    VECTOR_VIEW(id_token);
    ssize_t id_node = ADD__(id_token);
    node_s* node_array = context->lex_tree->nodes_array;
    node_array[id_node].node_value.value.id.is_function = true;

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
    
    scope_s local_scope = *scope;
    local_scope.is_global = false;
    local_scope.variable_count = 0;

    ssize_t var_node = GetInitVar(context, &local_scope);
    if (var_node != NO_LINK)
    {
        ssize_t arg_connector = ARG_CON;
        CONNECT_LEXES(arg_connector, NO_LINK, var_node); 
        var_node = arg_connector;

        ssize_t last_arg_connector = arg_connector;
        token_s token = {};
        VECTOR_VIEW(token);
        while ((token.lex_type == LEX_TYPE_SYNTAX)
                    && (token.value.syntax == SYNTAX_ARG_CONNECTOR))
        {
            VECTOR_ERASE;
            arg_connector = ARG_CON;
            CONNECT_LEXES(arg_connector, NO_LINK, GetInitVar(context, &local_scope));
            CONNECT_LEXES(last_arg_connector, arg_connector, NO_LINK);
            last_arg_connector = arg_connector;
            VECTOR_VIEW(token);
        }

        CONNECT_LEXES(id_node, var_node, NO_LINK);
    }

    InitNewFunction(id_node, local_scope.variable_count, scope, context);
    node_array = context->lex_tree->nodes_array;
    node_array[id_node].node_value.value
        .id.number_in_scope = (ssize_t) local_scope.variable_count;

    VECTOR_VIEW(syntax_token);
    if ((syntax_token.lex_type != LEX_TYPE_SYNTAX)
            || (syntax_token.value.syntax != SYNTAX_END_BRACKET))
    {
        context->status = RECURSIVE_RETURN_READ_ERROR;
        
        return NO_LINK;
    }
    VECTOR_ERASE;

    CONNECT_LEXES(function_kw_node, id_node, GetStatement(context, &local_scope));

    return function_kw_node;
}

static ssize_t 
GetIfWhile(read_context_t context,
           scope_s*       scope)
{
    ASSERT(context != NULL);
    ASSERT(scope != NULL);

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

    ssize_t condition_node = GetExpression(context, scope);

    VECTOR_VIEW(syntax_token);
    if ((syntax_token.lex_type != LEX_TYPE_SYNTAX)
            || (syntax_token.value.syntax != SYNTAX_END_BRACKET))
    {
        context->status = RECURSIVE_RETURN_READ_ERROR;
        
        return NO_LINK;
    }
    VECTOR_ERASE;
    
    scope_s local_scope = *scope;
    local_scope.is_global = false;

    CONNECT_LEXES(whileif_kw_node, condition_node, 
                        GetStatement(context, &local_scope));

    return whileif_kw_node;
}

static ssize_t 
GetStatement(read_context_t context,
             scope_s*       scope)
{
    ASSERT(context != NULL);
    ASSERT(scope != NULL);
    
    token_s token = {};
    ssize_t return_node = NO_LINK;
    VECTOR_VIEW(token);

    if (token.lex_type == LEX_TYPE_KEY_WORD)
    {
        if ((token.value.key_word == KEY_WORD_IF)
                || (token.value.key_word == KEY_WORD_WHILE))
        {
            return_node = GetIfWhile(context, scope);
        }
        else if (token.value.key_word == KEY_WORD_VAR)
        {
            return_node = GetInitVar(context, scope);

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
            return_node = GetFuncDefinition(context, scope);
        }
        else if (token.value.key_word == KEY_WORD_RETURN)
        {
            return_node = GetReturn(context, scope);
                
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
        scope_s local_scope = *scope;
        if (scope->is_global)
        {
            local_scope.variable_count = 0;
            local_scope.is_global = false;
        }
        
        ssize_t statement_connector = STMT_CON;
        CONNECT_LEXES(statement_connector, NO_LINK, 
                            GetStatement(context, &local_scope));
        return_node = statement_connector;
        ssize_t last_stmt_connector = statement_connector;

        VECTOR_VIEW(token);
        while ((token.lex_type != LEX_TYPE_SYNTAX) 
                    ||   (token.value.syntax != SYNTAX_END_BODY))
        {
            statement_connector = STMT_CON;
            CONNECT_LEXES(statement_connector, NO_LINK, 
                                GetStatement(context, &local_scope));
            CONNECT_LEXES(last_stmt_connector, statement_connector, 
                            NO_LINK);
            last_stmt_connector = statement_connector;
            VECTOR_VIEW(token);
        }

        VECTOR_ERASE;
    }
    else
    {
        return_node = GetExpression(context, scope);

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

static ssize_t 
GetGlobal(read_context_t context)
{
    ASSERT(context != NULL);

    token_s token = {};
    VECTOR_VIEW(token);
    ssize_t connector_node = STMT_CON;

    scope_s global_scope = {.scope = NO_LINK, .variable_count = 0, 
                                .is_global = true};
    CONNECT_LEXES(connector_node, NO_LINK, 
                        GetStatement(context, &global_scope));

    ssize_t return_node = connector_node;
    ssize_t last_connector_node = connector_node;

    VECTOR_VIEW(token);
    while(token.lex_type != LEX_TYPE_UNDEFINED)
    {
        connector_node = STMT_CON;
        CONNECT_LEXES(connector_node, NO_LINK, 
                            GetStatement(context, &global_scope));
        CONNECT_LEXES(last_connector_node, connector_node, 
                        NO_LINK);
        last_connector_node = connector_node;
        VECTOR_VIEW(token);
    }

    return return_node;
}

// =============================== UNDEFINITION ===============================

#undef RETURN_IF_ERROR
#undef RETURN_IF_RECURSIVE_ERROR