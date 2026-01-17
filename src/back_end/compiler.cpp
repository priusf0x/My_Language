#include "compiler.h"

#include "Assert.h"
#include "lexes.h"
#include "tree.h"

#define CHECK_OUTPUT(___X___) do {\
    compiler_return_e ___output = (___X___); \
    if (___output != 0) return ___output;} while (0)

// =============================== MAIN_CYCLE =================================
static compiler_return_e 
SetASMHeader(compiler_t compiler)
{
    ASSERT(compiler != NULL);

    const char* start_code = "call main\n"\
                             "hlt\n\n";

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
    compiler_return_e output = COMPILER_RETURN_SUCCESS;

    switch (array[lex].node_value.lex_type)
    {   
        case LEX_TYPE_KEY_WORD:
            break;

        case LEX_TYPE_ID:
        case LEX_TYPE_OPERATOR:
        case LEX_TYPE_CONST: 
            // compile expression 
            break;
        
        case LEX_TYPE_SYNTAX: return COMPILER_RETURN_SEMANTIC_ERROR;
        case LEX_TYPE_UNDEFINED:
        default: return COMPILER_RETURN_UNDEFINED_ELEMENT;
    }

    return output;
}

// ========================== OPS_COMPILATIONS ================================


static compiler_return_e 
CompileOp(ssize_t    node,
          compiler_t compiler);


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
CompileAriphmetic(ssize_t     lex,
                  const char* ariphmetic,
                  compiler_t  compiler)
{
    ASSERT(compiler != NULL);
    ASSERT(ariphmetic != NULL);    

    compiler_return_e output = COMPILER_RETURN_SUCCESS;
    node_s node = compiler->compiler_tree->nodes_array[lex];

    output = CompileOp(node.left_index, compiler);
    CHECK_OUTPUT(output);
    fprintf(compiler->file_output, "push RDX\n"); 
    output = CompileOp(node.right_index, compiler);
    CHECK_OUTPUT(output);
    fprintf(compiler->file_output, "push RDX\n"
                                   "%s\n"     
                                   "pop RDX\n", ariphmetic);
    
    return COMPILER_RETURN_SUCCESS;
}
// static compiler_return_e 
// CompileAriphmetic(ssize_t     lex,
//                   const char* ariphmetic,
//                   compiler_t  compiler)
// {
//     ASSERT(compiler != NULL);
//     ASSERT(ariphmetic != NULL);    

//     compiler_return_e output = COMPILER_RETURN_SUCCESS;
//     node_s node = compiler->compiler_tree->nodes_array[lex];

//     output = CompileOp(node.left_index, compiler);
//     CHECK_OUTPUT(output);
//     fprintf(compiler->file_output, "push RDX\n"); 
//     output = CompileOp(node.right_index, compiler);
//     CHECK_OUTPUT(output);
//     fprintf(compiler->file_output, "push RDX\n"
//                                    "%s\n"     
//                                    "pop RDX\n", ariphmetic);
    
//     return COMPILER_RETURN_SUCCESS;
// }

static compiler_return_e 
CompilePlus(ssize_t lex, compiler_t compiler)
{   const char* add_str = "add";
    return CompileAriphmetic(lex, add_str, compiler);}

static compiler_return_e 
CompileSub(ssize_t lex, compiler_t compiler)
{   const char* sub_str = "sub";
    return CompileAriphmetic(lex, sub_str, compiler);}

static compiler_return_e 
CompileMul(ssize_t lex, compiler_t compiler)
{   const char* mul_str = "mul";
    return CompileAriphmetic(lex, mul_str, compiler);}

static compiler_return_e 
CompileDiv(ssize_t lex, compiler_t compiler)
{   const char* div_str = "div";
    return CompileAriphmetic(lex, div_str, compiler);}




static compiler_return_e 
CompileOp(ssize_t    lex,
          compiler_t compiler)
{
    ASSERT(compiler != NULL);


    return COMPILER_RETURN_SUCCESS;
}

// ============================== ID_COMPILATION ==============================


// ================================= UNDEFIFINTION ============================

#undef CHECK_OUTPUT 

