#include "compiler.h"

#include "Assert.h"

static compiler_return_e 
CompileTree(ssize_t lex, compiler_t compiler);

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

compiler_return_e
CompileAST(compiler_t compiler)
{
    ASSERT(compiler != NULL);
//   add verificator 

    SetASMHeader(compiler);

    ssize_t first_element = compiler->compiler_tree
                                ->nodes_array->left_index;
    compiler_return_e output = COMPILER_RETURN_SUCCESS;

    if ((output = CompileTree(first_element, compiler)) != 0)
    {
        return output;
    }

    return COMPILER_RETURN_SUCCESS;
}

static compiler_return_e 
CompileStatement(ssize_t lex, compiler_t compiler);

static compiler_return_e 
CompileTree(ssize_t    lex,
            compiler_t compiler)
{
    ASSERT(compiler != NULL);

    while ()// )
    {
        // compile stirng 
    }

    return COMPILER_RETURN_SUCCESS;
}

// ========================= COMPILING_STATEMENT ==============================

// LEX_TYPE_ID
// LEX_TYPE_CONST
// LEX_TYPE_KEY_WORD
// LEX_TYPE_OPERATOR
// LEX_TYPE_SYNTAX

static compiler_return_e 
CompileStatement(ssize_t    lex, 
                 compiler_t compiler)
{
    // the idea is identifiing lex and seperate it into functions 

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
CompileOp(ssize_t node)
{
    // here op separations is made


    return COMPILER_RETURN_SUCCESS;
}

// ============================== ID_COMPILATION ==============================





