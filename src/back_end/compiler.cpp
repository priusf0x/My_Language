#include "compiler.h"

#include <assert.h>

#include "lexes.h"
#include "tree.h"
#include "my_lang_lib.h"

// ================================== HELPERS =================================

#define CHECK_OUTPUT(___X___) do {\
    compiler_return_e ___output = (___X___); \
    if (___output != 0) return ___output;} while (0)

static compiler_return_e 
CompileExpr(ssize_t lex, compiler_t compiler);

static compiler_return_e
CompileRDX(ssize_t lex, compiler_t compiler);

static compiler_return_e 
CompileKeyWord(ssize_t lex, compiler_t compiler);


static compiler_return_e
GetVarAdress(ssize_t    lex,
             compiler_t compiler)
{
    assert(compiler != NULL);

    node_s node = compiler->compiler_tree->nodes_array[lex];
    id_s value = node.node_value.value.id;
    
    if (value.is_global)
    {
        fprintf(compiler->file_output, 
                    "[R0X + %ld]", value.memory_location);
    }
    else 
    {
        fprintf(compiler->file_output, 
                    "[RBX - %ld]", value.memory_location);
    }

    return COMPILER_RETURN_SUCCESS;  
}

static compiler_return_e 
CompileStatement(ssize_t lex, compiler_t compiler);

static compiler_return_e
CompileVar(ssize_t    lex,
           compiler_t compiler);

// ============================= ADDING_STDLIB ================================

static compiler_return_e
CompileStdLibFunctions(compiler_t compiler)
{
    assert(compiler != NULL);

    function_s function = {};
    
    for (size_t i = 0; i < FUNCTIONS_AMOUNT; i++)
    {
        function = FUNCTIONS[i];
        fprintf(compiler->file_output, "%s\n", function.asm_code);
    }
    
    return COMPILER_RETURN_SUCCESS;
}

// =============================== MAIN_CYCLE =================================

static compiler_return_e // Little bypass for compiling globals
SetGlobalVariables(ssize_t    lex,
                   compiler_t compiler)
{
    assert(compiler != NULL);

    if (lex == NO_LINK)
    {
        return COMPILER_RETURN_SUCCESS;
    }

    node_s* array = compiler->compiler_tree->nodes_array;
    node_s node = array[lex];
    token_value_u value = node.node_value.value;

    if ((node.node_value.lex_type == LEX_TYPE_KEY_WORD)
            && (value.key_word == KEY_WORD_VAR))
    {
        node_s new_node = array[node.left_index];
        if (new_node.node_value.value.id.is_global)
        {
            CHECK_OUTPUT(CompileVar(lex, compiler));
        }
    }

    CHECK_OUTPUT(SetGlobalVariables(node.left_index, compiler));
    CHECK_OUTPUT(SetGlobalVariables(node.right_index, compiler));

    return COMPILER_RETURN_SUCCESS;
}

static compiler_return_e 
SetASMHeader(compiler_t compiler)
{
    assert(compiler != NULL);

    static const char* const start_processor_settings = 
        "push 1000\n"
        "pop  R0X \n" // globals memory space
        "push 5000\n"
        "pop  RCX \n"; // local variables memory space

    fprintf(compiler->file_output, "%s", start_processor_settings);

    CHECK_OUTPUT(SetGlobalVariables(0, compiler));

    const char* start_code = "call main:\n"\
                             "hlt\n\n";

    fprintf(compiler->file_output, "%s", start_code);
    
    CompileStdLibFunctions(compiler);

    return COMPILER_RETURN_SUCCESS;
}

static compiler_return_e 
CompileBranch(ssize_t    lex,
              compiler_t compiler)
{
    assert(compiler != NULL);
    
    if(lex == NO_LINK)
    {
        return COMPILER_RETURN_SUCCESS;
    }
    
    node_s* array = compiler->compiler_tree->nodes_array;
    node_s node = array[lex];
    compiler_return_e output = COMPILER_RETURN_SUCCESS;

    if ((node.node_value.lex_type == LEX_TYPE_SYNTAX)
            && (node.node_value.value.syntax == SYNTAX_STATEMENT_CONNECTOR))
    {
        while (lex != NO_LINK)
        {
            output = CompileStatement(array[lex].right_index,
                                                        compiler);  
            CHECK_OUTPUT(output);

            lex = array[lex].left_index; // switching to next stmt
        }    
    }

    return COMPILER_RETURN_SUCCESS;
}

compiler_return_e
CompileAST(compiler_t compiler)
{
    assert(compiler != NULL);

    SetASMHeader(compiler); // calling main function 
    
    node_s* array = compiler->compiler_tree->nodes_array;
    ssize_t start_node= array[0].left_index;

    return CompileBranch(start_node, compiler);
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
            return CompileKeyWord(lex, compiler);

        case LEX_TYPE_ID:
        case LEX_TYPE_OPERATOR:
        case LEX_TYPE_CONST: 
            return CompileRDX(lex, compiler);
        
        case LEX_TYPE_SYNTAX: return COMPILER_RETURN_SEMANTIC_ERROR;
        case LEX_TYPE_UNDEFINED:
        default: return COMPILER_RETURN_UNDEFINED_ELEMENT;
    }
}

// ========================= KEY_WORD_COMPILATION =============================

static compiler_return_e
CompileVar(ssize_t    lex,
           compiler_t compiler)
{
    assert(compiler != NULL);

    node_s node = compiler->compiler_tree->nodes_array[lex];
    compiler_return_e output = COMPILER_RETURN_SUCCESS;

    if (node.right_index != NO_LINK)
    {
        output = CompileExpr(node.right_index, compiler);
        CHECK_OUTPUT(output);
        fprintf(compiler->file_output, 
                    "pop ");
        GetVarAdress(node.left_index, compiler);
        fprintf(compiler->file_output, "\n");
    }
    
    return COMPILER_RETURN_SUCCESS;
}

static compiler_return_e 
CompileLocalVarKW(ssize_t    lex,
                  compiler_t compiler)
{
    assert(compiler != NULL);
    
    node_s* array = compiler->compiler_tree->nodes_array;
    node_s node = array[lex];
    node = array[node.left_index];

    if ((!node.node_value.value.id.is_global))
    {
        return CompileVar(lex, compiler);
    }

    return COMPILER_RETURN_SUCCESS;
}

static compiler_return_e 
CompileIfKW(ssize_t    lex,     
            compiler_t compiler)
{
    assert(compiler != NULL);

    node_s node = compiler->compiler_tree->nodes_array[lex];
    compiler_return_e output = COMPILER_RETURN_SUCCESS;

    if (node.left_index == NO_LINK)
    {
        return COMPILER_RETURN_AST_STANDARD_ERROR;
    }

    size_t if_label = compiler->label_count;    
    compiler->label_count++;

    output = CompileExpr(node.left_index, compiler);
    CHECK_OUTPUT(output);
    fprintf(compiler->file_output, 
                "push 0\n"
                "je .L%zu:\n", if_label);
    output = CompileBranch(node.right_index, compiler);
    fprintf(compiler->file_output, 
                ".L%zu:\n", if_label);

    /*
        // condition 
        push 0
        je .L123:
        // right part of the if node
        .L123:
    */

    return COMPILER_RETURN_SUCCESS;
}

static compiler_return_e 
CompileWhileKW(ssize_t    lex,
               compiler_t compiler)
{
    assert(compiler != NULL);

    node_s node = compiler->compiler_tree->nodes_array[lex];
    compiler_return_e output = COMPILER_RETURN_SUCCESS;

    if (node.left_index == NO_LINK)
    {
        return COMPILER_RETURN_AST_STANDARD_ERROR;
    }

    size_t while_label = compiler->label_count;
    size_t cond_label = compiler->label_count + 1; 
    compiler->label_count += 2;

    fprintf(compiler->file_output, 
                ".L%zu:\n", while_label);

    output = CompileExpr(node.left_index, compiler);
    CHECK_OUTPUT(output);
    fprintf(compiler->file_output, 
                "push 0\n"
                "je .L%zu:\n", cond_label);
    output = CompileBranch(node.right_index, compiler);
    
    fprintf(compiler->file_output, 
                "jmp .L%zu:\n", while_label);
    fprintf(compiler->file_output, 
                ".L%zu:\n", cond_label);

    /* 
        .L124:
        // condition 
        push 0
        je .L123:
        // right part of the if node
        jmp .L124:
        .L123:
    */

    return COMPILER_RETURN_SUCCESS;
}

static compiler_return_e
SetArgsUseRegs(size_t     arg_amount,
               compiler_t compiler)
{
    assert(compiler != NULL);

    #pragma GCC diagnostic ignored "-Wimplicit-fallthrough"

    switch (arg_amount)
    {
        case 3:
            fprintf(compiler->file_output, "push RGX\n"
                                           "pop [RBX - 2]\n");
        case 2:
            fprintf(compiler->file_output, "push RFX\n"
                                           "pop [RBX - 1]\n");
        case 1:
            fprintf(compiler->file_output, "push REX\n"
                                           "pop [RBX - 0]\n");
        
        case 0:
        default: break;
    }
    
    #pragma GCC diagnostic warning "-Wimplicit-fallthrough"

    return COMPILER_RETURN_SUCCESS;
}

static compiler_return_e
SetArgsUseStack(size_t     arg_amount,
                ssize_t    lex,
                compiler_t compiler)
{
    assert(compiler != NULL);
    
    node_s* array = compiler->compiler_tree->nodes_array;
    lex = array[lex].left_index; // to funct name
    lex = array[lex].left_index; // to arg connector
    ssize_t next_lex = lex;

    fprintf(compiler->file_output, 
        "pop REX\n"
        "pop RFX\n"
        "pop RDX\n");

    while (next_lex != NO_LINK)
    {
        lex = next_lex;
        next_lex = array[lex].left_index; 
    }

    for (; arg_amount > 0; arg_amount--)
    {
        fprintf(compiler->file_output, 
                    "pop [RBX - %zu]\n", arg_amount - 1);
    }
    
    fprintf(compiler->file_output,
        "push RDX\n" 
        "push RFX\n"
        "push REX\n");

    return COMPILER_RETURN_SUCCESS;
}

static size_t
CountArgs(ssize_t    lex,
          compiler_t compiler)
{
    assert(compiler != NULL);

    node_s* array = compiler->compiler_tree->nodes_array;
    size_t arg_amount = 0;
    lex = array[lex].left_index; 

    while (lex != NO_LINK)
    {
        arg_amount++;
        lex = array[lex].left_index;
    }

    return arg_amount;
}

static compiler_return_e
SetArguments(ssize_t    lex,
             compiler_t compiler)
{
    assert(compiler != NULL);
    
    size_t arg_amount = CountArgs(lex, compiler);
    
    if (arg_amount <= 3)
    {
        SetArgsUseRegs(arg_amount, compiler);
    }
    else 
    {
        SetArgsUseStack(arg_amount, lex, compiler);
    }

    return COMPILER_RETURN_SUCCESS;
}

static compiler_return_e
SetOverhead(ssize_t    lex,
            compiler_t compiler)
{
    assert(compiler != NULL);
    
    node_s* array = compiler->compiler_tree->nodes_array;
    lex = array[lex].left_index; // to funct name
    id_s id = array[lex].node_value.value.id;
    
    fprintf(compiler->file_output, 
                "%.*s:\n", (int) id.id.string_size, 
                            id.id.string_source);
    
    fprintf(compiler->file_output, 
                "push RBX\n"
                "push RCX\n"
                "push RCX\n"
                "pop RBX\n"
                "push RBX\n"
                "push %ld\n"
                "sub\n"
                "pop RCX\n", id.memory_location);

    SetArguments(lex, compiler);

    return COMPILER_RETURN_SUCCESS;
}

static compiler_return_e 
CompileFunction(ssize_t    lex,
                compiler_t compiler)
{
    assert(compiler != NULL);

    SetOverhead(lex, compiler);

    node_s* array = compiler->compiler_tree->nodes_array;
    lex = array[lex].right_index;
    compiler_return_e output = CompileBranch(lex, compiler);
    CHECK_OUTPUT(output);
    fprintf(compiler->file_output, "hlt\n");

    return COMPILER_RETURN_SUCCESS;
}

static compiler_return_e
CompileReturn(ssize_t    lex,
              compiler_t compiler)
{
    assert(compiler != NULL);

    node_s* array = compiler->compiler_tree->nodes_array;
    lex = array[lex].right_index;
    compiler_return_e output = CompileExpr(lex, compiler);
    CHECK_OUTPUT(output);
    fprintf(compiler->file_output, 
                "pop RAX\n"
                "pop RCX\n"
                "pop RBX\n"
                "ret\n");

    return COMPILER_RETURN_SUCCESS;
}

static compiler_return_e 
CompileKeyWord(ssize_t    lex,
               compiler_t compiler)
{
    assert(compiler != NULL);
    
    struct key_word_compile 
    {
        key_word_type_e key_word;
        compiler_return_e (*kw_function) (ssize_t, compiler_t);
    };
    key_word_compile kw_asm[] = 
    {
        {KEY_WORD_UNDEFINED, NULL             },
        {KEY_WORD_IF,        CompileIfKW      },
        {KEY_WORD_VAR,       CompileLocalVarKW},
        {KEY_WORD_WHILE,     CompileWhileKW   },
        {KEY_WORD_FUNCTION,  CompileFunction  },
        {KEY_WORD_RETURN,    CompileReturn    }
    };

    node_s node = compiler->compiler_tree->nodes_array[lex];

    return kw_asm[node.node_value.value.op]
                            .kw_function(lex, compiler);
}

// ========================== EXPR_COMPILATIONS ===============================

static compiler_return_e 
CompileAriphmetic(ssize_t     lex,
                  const char* ariphmetic,
                  compiler_t  compiler)
{
    assert(compiler != NULL);
    assert(ariphmetic != NULL);    

    compiler_return_e output = COMPILER_RETURN_SUCCESS;
    node_s node = compiler->compiler_tree->nodes_array[lex];

    output = CompileExpr(node.left_index, compiler);
    CHECK_OUTPUT(output);
    output = CompileExpr(node.right_index, compiler);
    CHECK_OUTPUT(output);

    fprintf(compiler->file_output, "%s\n", ariphmetic);
    
    return COMPILER_RETURN_SUCCESS;
}

static compiler_return_e
CompileBoolean(ssize_t     lex,
               const char* bool_str, 
               compiler_t  compiler)
{
    assert(compiler != NULL);
    assert(bool_str != NULL);    

    compiler_return_e output = COMPILER_RETURN_SUCCESS;
    node_s node = compiler->compiler_tree->nodes_array[lex];

    output = CompileExpr(node.left_index, compiler);
    CHECK_OUTPUT(output);
    output = CompileExpr(node.right_index, compiler);
    CHECK_OUTPUT(output);

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

static compiler_return_e
CompileAssign(ssize_t    lex,
              compiler_t compiler)
{
    assert(compiler != NULL);
    
    node_s node = compiler->compiler_tree->nodes_array[lex];
    compiler_return_e output = CompileExpr(node.right_index, compiler);
    CHECK_OUTPUT(output);
    fprintf(compiler->file_output, 
                "pop ");
    GetVarAdress(node.left_index, compiler);
    fprintf(compiler->file_output, "\n");
    fprintf(compiler->file_output, 
                "push ");
    GetVarAdress(node.left_index, compiler);
    fprintf(compiler->file_output, "\n");

    return COMPILER_RETURN_SUCCESS;
}

static compiler_return_e 
CompileOp(ssize_t    lex, 
          compiler_t compiler)
{
    assert(compiler != NULL);

    struct op_compiler_s 
    {
        operator_type_e op;
        compiler_return_e (*op_function) (ssize_t lex, compiler_t compiler);
    };

    op_compiler_s op_asm[] = 
    {
        {OPERATOR_UNDEFINED,     NULL            },
        {OPERATOR_PLUS,          CompilePlus     },
        {OPERATOR_PLUS,          CompileSub      },
        {OPERATOR_MUL,           CompileMul      },
        {OPERATOR_DIV,           CompileDiv      },
        {OPERATOR_EQUALITY,      CompileEq       },
        {OPERATOR_N_EQUALITY,    CompileNEq      },
        {OPERATOR_ASSIGNMENT,    CompileAssign   },
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
PushArgsInRegisters(size_t     arg_amount,
                    ssize_t    lex,
                    compiler_t compiler)
{
    assert(compiler != NULL);
    
    node_s* array = compiler->compiler_tree->nodes_array;
    lex = array[lex].right_index; // to arg_connector 
    ssize_t next_lex = lex;
    compiler_return_e output = COMPILER_RETURN_SUCCESS;

    while (next_lex != NO_LINK)
    {
        lex = next_lex;
        next_lex = array[lex].left_index; 
    }

    #pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
    
    switch (arg_amount)
    {
        case 3:
            output = CompileExpr(array[lex].right_index, compiler);
            CHECK_OUTPUT(output);
            fprintf(compiler->file_output, "pop RGX\n");
            lex = array[lex].parent_index;

        case 2:
            output = CompileExpr(array[lex].right_index, compiler);
            CHECK_OUTPUT(output);
            fprintf(compiler->file_output, "pop RFX\n");
            lex = array[lex].parent_index;

        case 1:
            output = CompileExpr(array[lex].right_index, compiler);
            CHECK_OUTPUT(output);
            fprintf(compiler->file_output, "pop REX\n");

        case 0:
        default: return COMPILER_RETURN_SUCCESS;
    }
        
    #pragma GCC diagnostic warning "-Wimplicit-fallthrough"
    
    return COMPILER_RETURN_SUCCESS;
}

static compiler_return_e 
PushArgsInStack(size_t     arg_amount,
                ssize_t    lex,
                compiler_t compiler)
{
    assert(compiler != NULL);
    
    node_s* array = compiler->compiler_tree->nodes_array;
    lex = array[lex].right_index; // to arg_connector
    compiler_return_e output = COMPILER_RETURN_SUCCESS;

    for (; arg_amount > 0; arg_amount--)
    {
        output = CompileExpr(array[lex].right_index, compiler);
        CHECK_OUTPUT(output);
        lex = array[lex].left_index;
    }
    
    return COMPILER_RETURN_SUCCESS;
}

static compiler_return_e 
PushArgs(ssize_t    lex,
         compiler_t compiler)
{
    assert(compiler != NULL);
    
    node_s* array = compiler->compiler_tree->nodes_array;
    size_t arg_amount = (size_t) array[lex].node_value
                                    .value.id.memory_location;

    if (arg_amount <= 3)
    {
        return PushArgsInRegisters(arg_amount, lex, compiler);
    } 
    else 
    {
        return PushArgsInStack(arg_amount, lex, compiler);
    }

    return COMPILER_RETURN_SUCCESS;
}

static compiler_return_e
CompileFunctionCall(ssize_t    lex,
                    compiler_t compiler)
{
    assert(compiler != NULL);
    
    PushArgs(lex, compiler);
    
    node_s* array = compiler->compiler_tree->nodes_array;
    id_s id = array[lex].node_value.value.id;

    fprintf(compiler->file_output, 
                "call %.*s:\n", (int) id.id.string_size,
                             id.id.string_source);

    return COMPILER_RETURN_SUCCESS;
}

static compiler_return_e
CompileID(ssize_t    lex,
          compiler_t compiler)
{
    assert(compiler != NULL);

    node_s node = compiler->compiler_tree->nodes_array[lex];
    id_s value = node.node_value.value.id;
    compiler_return_e output = COMPILER_RETURN_SUCCESS;

    if (value.is_function)
    {   
        output = CompileFunctionCall(lex, compiler);
        CHECK_OUTPUT(output);
        fprintf(compiler->file_output, "push RAX\n");
    }
    else
    {
        fprintf(compiler->file_output, "push ");
        GetVarAdress(lex, compiler);
        fprintf(compiler->file_output, "\n");
    }

    return COMPILER_RETURN_SUCCESS;
}

static compiler_return_e
CompileConst(ssize_t    lex,
             compiler_t compiler)
{
    assert(compiler != NULL);
    
    node_s node = compiler->compiler_tree->nodes_array[lex];

    fprintf(compiler->file_output, 
                "push %d\n", node.node_value.value.constant);

    return COMPILER_RETURN_SUCCESS;
}

static compiler_return_e 
CompileExpr(ssize_t    lex,
            compiler_t compiler)
{
    assert(compiler != NULL);
    
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

static compiler_return_e
CompileRDX(ssize_t    lex,
           compiler_t compiler)
{
    assert(compiler != NULL);
    
    compiler_return_e output = CompileExpr(lex, compiler);
    fprintf(compiler->file_output, "pop RDX\n");

    return output;
}

// ==================================== UNDEF ================================

#undef CHECK_OUTPUT 

