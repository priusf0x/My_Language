#include "compiler.h"

#include "Assert.h"
#include "lexes.h"
#include "tree.h"

// ================================== HELPERS =================================

#define CHECK_OUTPUT(___X___) do {\
    compiler_return_e ___output = (___X___); \
    if (___output != 0) return ___output;} while (0)

static compiler_return_e 
CompileExpr(ssize_t    lex,
            compiler_t compiler);

static compiler_return_e
GetVarAdress(ssize_t    lex,
             compiler_t compiler)
{
    ASSERT(compiler != NULL);

    node_s node = compiler->compiler_tree->nodes_array[lex];
    id_s value = node.node_value.value.id;
    
    if (value.is_global)
    {
        fprintf(compiler->file_output, 
                    "[%ld]", value.number_in_scope);
    }
    else 
    {
        fprintf(compiler->file_output, 
                    "[RBX-%ld]", value.number_in_scope);
    }

    return COMPILER_RETURN_SUCCESS;  
}

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
        CHECK_OUTPUT(output);

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
        case LEX_TYPE_KEY_WORD:
            return COMPILER_RETURN_SUCCESS;

        case LEX_TYPE_ID:
        case LEX_TYPE_OPERATOR:
        case LEX_TYPE_CONST: 
            return CompileExpr(lex, compiler);
        
        case LEX_TYPE_SYNTAX: return COMPILER_RETURN_SEMANTIC_ERROR;
        case LEX_TYPE_UNDEFINED:
        default: return COMPILER_RETURN_UNDEFINED_ELEMENT;
    }
}

// ========================= KEY_WORD_COMPILATION =============================


static compiler_return_e 
CompileVarKW(ssize_t    lex,
             compiler_t compiler)
{
    ASSERT(compiler != NULL);

    node_s node = compiler->compiler_tree->nodes_array[lex];
    compiler_return_e output = COMPILER_RETURN_SUCCESS;

    if (node.right_index != NO_LINK)
    {
        output = CompileExpr(node.right_index, compiler);
        CHECK_OUTPUT(output);
        fprintf(compiler->file_output, 
                    "push RDX\n"
                    "pop ");
        GetVarAdress(node.right_index, compiler);
        fprintf(compiler->file_output, "\n");
    }

    return COMPILER_RETURN_SUCCESS;
}

static compiler_return_e 
CompileKeyWord(ssize_t    lex,
               compiler_t compiler)
{
    ASSERT(compiler != NULL);


    return COMPILER_RETURN_SUCCESS;
}

// ========================== EXPR_COMPILATIONS ===============================

static compiler_return_e 
CompileAriphmetic(ssize_t     lex,
                  const char* ariphmetic,
                  compiler_t  compiler)
{
    ASSERT(compiler != NULL);
    ASSERT(ariphmetic != NULL);    

    compiler_return_e output = COMPILER_RETURN_SUCCESS;
    node_s node = compiler->compiler_tree->nodes_array[lex];

    output = CompileExpr(node.left_index, compiler);
    CHECK_OUTPUT(output);
    fprintf(compiler->file_output, "push RDX\n"); 
    output = CompileExpr(node.right_index, compiler);
    CHECK_OUTPUT(output);
    fprintf(compiler->file_output, "push RDX\n"
                                   "%s\n"     
                                   "pop RDX\n", ariphmetic);
    
    return COMPILER_RETURN_SUCCESS;
}

static compiler_return_e
CompileBoolean(ssize_t     lex,
               const char* bool_str, 
               compiler_t  compiler)
{
    ASSERT(compiler != NULL);
    ASSERT(bool_str != NULL);    

    compiler_return_e output = COMPILER_RETURN_SUCCESS;
    node_s node = compiler->compiler_tree->nodes_array[lex];

    output = CompileExpr(node.left_index, compiler);
    CHECK_OUTPUT(output);
    fprintf(compiler->file_output, "push RDX\n"); 
    output = CompileExpr(node.right_index, compiler);
    CHECK_OUTPUT(output);
    fprintf(compiler->file_output, "push RDX\n");

  /*je .L32
    push 0
    jmp .L33
    .L32:
    push 1 
    .L33:*/
    fprintf(compiler->file_output, 
        "%s .L%zu:\n"
        "push 0\n"
        "jmp .L%zu:\n"
        ".L%zu:\n"
        "push 1\n"
        ".L%zu:\n", bool_str, compiler->label_count, 
                    compiler->label_count + 1, compiler->label_count,
                        compiler->label_count + 1);
    compiler->label_count += 2;
    fprintf(compiler->file_output, "pop RDX\n");

    return COMPILER_RETURN_SUCCESS;
}

static compiler_return_e 
CompilePlus(ssize_t lex, compiler_t compiler)
{   const char* add_str = "add";
    return CompileAriphmetic(lex, add_str, compiler);}

static compiler_return_e 
CompileSub(ssize_t lex, compiler_t compiler)
{   const char* sub_str = "sub";
    return CompileAriphmetic(lex, sub_str, compiler);}

static inline compiler_return_e 
CompileMul(ssize_t lex, compiler_t compiler)
{   const char* mul_str = "mul";
    return CompileAriphmetic(lex, mul_str, compiler);}

static compiler_return_e 
CompileDiv(ssize_t lex, compiler_t compiler)
{   const char* div_str = "div";
    return CompileAriphmetic(lex, div_str, compiler);}

static compiler_return_e 
CompileEq(ssize_t lex, compiler_t compiler)
{   const char* eq_asm = "je";
    return CompileBoolean(lex, eq_asm, compiler);}

static inline compiler_return_e 
CompileNEq(ssize_t lex, compiler_t compiler)
{   const char* neq_asm = "jne";
    return CompileBoolean(lex, neq_asm, compiler);}

static compiler_return_e 
CompileAbove(ssize_t lex, compiler_t compiler)
{   const char* ab_asm = "ja";
    return CompileBoolean(lex, ab_asm, compiler);}

static compiler_return_e 
CompileAboveOrEq(ssize_t lex, compiler_t compiler)
{   const char* abe_asm = "jae";
    return CompileBoolean(lex, abe_asm, compiler);}

static compiler_return_e 
CompileBelow(ssize_t lex, compiler_t compiler)
{   const char* be_asm = "jb";
    return CompileBoolean(lex, be_asm, compiler);}

static compiler_return_e 
CompileBelowOrEq(ssize_t lex, compiler_t compiler)
{   const char* be_asm = "jbe";
    return CompileBoolean(lex, be_asm, compiler);}

struct op_compiler_s 
{
    operator_type_e op;
    compiler_return_e (*op_function) (ssize_t lex, compiler_t compiler);
};

static compiler_return_e 
CompileOp(ssize_t lex, 
          compiler_t compiler)
{
    ASSERT(compiler != NULL);

    op_compiler_s op_asm[] = 
    {
        {OPERATOR_UNDEFINED,     NULL            },
        {OPERATOR_PLUS,          CompilePlus     },
        {OPERATOR_PLUS,          CompileSub      },
        {OPERATOR_MUL,           CompileMul      },
        {OPERATOR_DIV,           CompileDiv      },
        {OPERATOR_EQUALITY,      CompileEq       },
        {OPERATOR_N_EQUALITY,    CompileNEq      },
        {OPERATOR_ASSIGNMENT,    NULL            },
        {OPERATOR_MORE,          CompileAbove    },
        {OPERATOR_MORE_OR_EQ,    CompileAboveOrEq},
        {OPERATOR_LESS,          CompileBelow    },
        {OPERATOR_LESS_OR_EQUAL, CompileBelowOrEq}
    };

    node_s node = compiler->compiler_tree->nodes_array[lex];

    return op_asm[node.node_value.value.op]
                            .op_function(lex, compiler);
}

static compiler_return_e
CompileID(ssize_t    lex,
          compiler_t compiler)
{
    ASSERT(compiler != NULL);

    node_s node = compiler->compiler_tree->nodes_array[lex];
    id_s value = node.node_value.value.id;

    if (value.is_function)
    {
        return COMPILER_RETURN_SUCCESS;
    }
    else
    {
        fprintf(compiler->file_output, "push ");
        GetVarAdress(lex, compiler);
        fprintf(compiler->file_output, "\npop RDX\n");
    }

    return COMPILER_RETURN_SUCCESS;
}

static compiler_return_e
CompileConst(ssize_t    lex,
             compiler_t compiler)
{
    ASSERT(compiler != NULL);
    
    node_s node = compiler->compiler_tree->nodes_array[lex];

    fprintf(compiler->file_output, 
                "push %d\n"
                "pop RDX\n", node.node_value.value.constant);

    return COMPILER_RETURN_SUCCESS;
}

static compiler_return_e 
CompileExpr(ssize_t    lex,
            compiler_t compiler)
{
    ASSERT(compiler != NULL);
    
    node_s node = compiler->compiler_tree->nodes_array[lex];

    switch (node.node_value.lex_type)
    {
        case LEX_TYPE_ID:
            return CompileID(lex, compiler);
                
        case LEX_TYPE_OPERATOR:
            return CompileOp(lex, compiler);

        case LEX_TYPE_CONST:
            return CompileConst(lex, compiler);

        case LEX_TYPE_KEY_WORD: return COMPILER_RETURN_SEMANTIC_ERROR;
        case LEX_TYPE_SYNTAX: return COMPILER_RETURN_SEMANTIC_ERROR;
        case LEX_TYPE_UNDEFINED:
        default: return COMPILER_RETURN_UNDEFINED_ELEMENT;
    }
}

// ============================== ID_COMPILATION ==============================


// ==================================== UNDEF ================================

#undef CHECK_OUTPUT 

