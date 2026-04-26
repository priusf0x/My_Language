#include "compiler.h"

#include <assert.h>
#include <stdlib.h>

#include "lexes.h"
#include "tree.h"

// ============================== MACROS/STRUCTS ==============================

#define CHECK_OUTPUT(___X___) do {\
    compiler_return_e ___output = (___X___); \
    if (___output != 0) return ___output;} while (0)

struct handler_s
{
    lex_types_e         lex_type;
    compiler_return_e (*handler) (ssize_t, compiler_t) = {};
};
    
// ============================= AST COMPILE ==================================

// --------------------------- compile_keyword ------------------------------

static compiler_return_e
CompileID(ssize_t    lex,
          compiler_t compiler)
{
    assert(compiler != nullptr);

    if (lex == NO_LINK)
    {
        return COMPILER_RETURN_SUCCESS;
    }

    return COMPILER_RETURN_SUCCESS;
}


// --------------------------- compile_keyword ------------------------------

// static compiler_return_e

    // KEY_WORD_UNDEFINED,
    // KEY_WORD_IF,
    // KEY_WORD_VAR,
    // KEY_WORD_WHILE,
    // KEY_WORD_FUNCTION,
    // KEY_WORD_RETURN,
    // KEY_WORD_ELSE

static compiler_return_e
CompileKW(ssize_t    lex,
          compiler_t compiler)
{
    assert(compiler != nullptr);
    assert(lex != NO_LINK);

    node_s* array = compiler->compiler_tree->nodes_array;
    node_s cur_node = array[lex];
    node_data_t val = cur_node.node_value;
    
    switch ()
    {
        case KEY_WORD_IF:
        case KEY_WORD_VAR:
        case KEY_WORD_WHILE:
        case KEY_WORD_FUNCTION:
        case KEY_WORD_RETURN:
        case KEY_WORD_UNDEFINED: return COMPILER_RETURN_INCORRECT_AST;
        default: assert(0);
    }

    return COMPILER_RETURN_SUCCESS;
}

// --------------------------- compile_operation ------------------------------

static compiler_return_e
CompileOp(ssize_t    lex,
          compiler_t compiler)
{
    assert(compiler != nullptr);

    if (lex == NO_LINK)
    {
        return COMPILER_RETURN_SUCCESS;
    }

    return COMPILER_RETURN_SUCCESS;
}

// --------------------------- compile_statement ------------------------------

static compiler_return_e
CompileStatement(ssize_t    lex,
                 compiler_t compiler)
{
    assert(compiler != nullptr);

    if (lex == NO_LINK)
    {
        return COMPILER_RETURN_SUCCESS;
    }

    node_s* array = compiler->compiler_tree->nodes_array;
    node_s cur_node = array[lex];
    node_data_t val = cur_node.node_value;

    switch (val.lex_type)
    {
        case LEX_TYPE_ID:
            return CompileID(cur_node.right_index, compiler);
        case LEX_TYPE_KEY_WORD:
            return CompileKW(cur_node.right_index, compiler);
        case LEX_TYPE_OPERATOR:
            return CompileOp(cur_node.right_index, compiler);
        case LEX_TYPE_SYNTAX: 
            return CompileStatement(cur_node.right_index, compiler);
        case LEX_TYPE_CONST: return COMPILER_RETURN_SUCCESS;
        case LEX_TYPE_UNDEFINED: return COMPILER_RETURN_INCORRECT_AST;
        default: assert(0);
    }

    return COMPILER_RETURN_SUCCESS;
}

// ---------------------------- compile_branch --------------------------------

static compiler_return_e 
CompileBranch(ssize_t    lex,
              compiler_t compiler)
{
    assert(compiler != nullptr);

    if (lex == NO_LINK)
    {
        return COMPILER_RETURN_INCORRECT_AST;
    }
    
    node_s* array = compiler->compiler_tree->nodes_array;
    node_data_t val = array[lex].node_value;
    compiler_return_e output = COMPILER_RETURN_SUCCESS;

    while (lex != NO_LINK)
    {
        if ((val.lex_type == LEX_TYPE_SYNTAX)
                && (val.value.syntax == SYNTAX_STATEMENT_CONNECTOR))
        {
            return COMPILER_RETURN_SUCCESS;
        }

        output = CompileStatement(array[lex].right_index, compiler);  
        CHECK_OUTPUT(output);
        lex = array[lex].left_index;
        val = array[lex].node_value;
    }    


    return COMPILER_RETURN_SUCCESS;
}

// --------------------------- set_asm_header ---------------------------------

static compiler_return_e 
SetASMHeader(compiler_t compiler)
{
    assert(compiler != nullptr);

    const char* asm_header = "_start:\n";
    fprintf(compiler->file_output, "%s", asm_header);
    
    return COMPILER_RETURN_SUCCESS;
}

// ---------------------------- main_interface --------------------------------

compiler_return_e
CompileAST(compiler_t compiler)
{
    assert(compiler != NULL);
    
    SetASMHeader(compiler); 

    node_s* array = compiler->compiler_tree->nodes_array;
    ssize_t start_node= array[0].left_index;

    return CompileBranch(start_node, compiler);
}