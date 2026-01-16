#include "compiler.h"

#include "Assert.h"
#include "lexes.h"
#include "tree.h"

// =============================== MAIN_CYCLE =================================

static compiler_return_e 
SetASMHeader(compiler_t compiler)
{
    ASSERT(compiler != NULL);

    const char* start_code = "call main\n"\
                             "hlt\n";

    fprintf(compiler->file_output, "%s", start_code);

    return COMPILER_RETURN_SUCCESS;
}

static compiler_return_e 
CompileStatement(ssize_t lex, compiler_t compiler);

compiler_return_e
CompileAST(compiler_t compiler)
{
    ASSERT(compiler != NULL);

    SetASMHeader(compiler); // calling main function 

    ssize_t lex = compiler->compiler_tree
                                ->nodes_array->left_index;
    compiler_return_e output = COMPILER_RETURN_SUCCESS;
    node_s* array = compiler->compiler_tree->nodes_array;

    while (lex != NO_LINK)
    {
        output = CompileStatement(array[lex].right_index,
                                                    compiler);  
        if (output != 0)
        {
            return output;
        }

        lex = array[lex].left_index; // switching to next stmt
    }

    return COMPILER_RETURN_SUCCESS;
}

// ========================= COMPILING_STATEMENT ==============================

static compiler_return_e 
CompileStatement(ssize_t    lex, 
                 compiler_t compiler)
{
    node_s* array = compiler->compiler_tree->nodes_array;

    switch (array[lex].node_value.lex_type)
    {   
        case LEX_TYPE_ID:
            break;
        case LEX_TYPE_CONST:
            break;
        case LEX_TYPE_KEY_WORD:
            break;
        case LEX_TYPE_OPERATOR:
            break;

        case LEX_TYPE_SYNTAX: return COMPILER_RETURN_SEMANTIC_ERROR;
        case LEX_TYPE_UNDEFINED:
        default: return COMPILER_RETURN_UNDEFINED_ELEMENT;
    }

    return COMPILER_RETURN_SUCCESS;
}

// ========================== OPS_COMPILATIONS ================================

// OPERATOR_UNDEFINED
// OPERATOR_PLUS
// OPERATOR_MINUS
// OPERATOR_MUL
// OPERATOR_DIV
// OPERATOR_EQUALITY
// OPERATOR_N_EQUALITY
// OPERATOR_ASSIGNMENT
// OPERATOR_MORE
// OPERATOR_MORE_OR_EQ
// OPERATOR_LESS
// OPERATOR_LESS_OR_EQUAL

static compiler_return_e 
CompileOp(ssize_t    node,
          compiler_t compiler)
{
    ASSERT(compiler != NULL);
    // here op separations is made


    return COMPILER_RETURN_SUCCESS;
}

// ============================== ID_COMPILATION ==============================





