#include "compiler.h"

#include <assert.h>
#include <cstddef>
#include <cstdio>
#include <stdlib.h>

#include "lexes.h"
#include "tree.h"

// ============================== MACROS/STRUCTS ==============================

#pragma clang diagnostic ignored "-Wformat-nonliteral"

#define CHECK_OUTPUT(___X___) do {\
    compiler_return_e ___output = (___X___); \
    if (___output != 0) return ___output;} while (0)

struct handler_s
{
    lex_types_e         lex_type;
    compiler_return_e (*handler) (ssize_t, compiler_t) = {};
};

// ================================ HELPERS ===================================

static compiler_return_e 
GetVarPos(ssize_t    lex,
          compiler_t compiler)
{
    assert(compiler != nullptr);

    node_s* array = compiler->compiler_tree->nodes_array;
    node_s cur_node = array[lex];
    node_data_t val = cur_node.node_value;

    assert(val.lex_type != LEX_TYPE_ID);    
    assert(!val.value.id.is_function);    

    if (val.value.id.is_global)
    {
        fprintf(compiler->file_output, "qword [%*.s]", 
                    (int) val.value.id.id.string_size,
                    val.value.id.id.string_source);
    }
    else 
    {
        if (val.value.id.info2) // check if argument 
        {
            fprintf(compiler->file_output, "qword [rbp + %ld]",
                    val.value.id.info1 + 16);
        }
        else 
        {
            fprintf(compiler->file_output, "qword [rbp - %ld]",
                    val.value.id.info1 + 8);
        }
    }

    return COMPILER_RETURN_SUCCESS;
}
    
// ============================= AST COMPILE ==================================

// --------------------------- compile_keyword --------------------------------

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

static compiler_return_e 
CompileGlobalData(ssize_t    lex, 
           compiler_t compiler)
{
    assert(compiler != nullptr);
    assert(lex != NO_LINK);

    // fprintf(compiler-> );

    
    return COMPILER_RETURN_SUCCESS;
}


static compiler_return_e 
CompileGlobalBSS(ssize_t    lex, 
                 compiler_t compiler)
{
    assert(compiler != nullptr);
    assert(lex != NO_LINK);

    const char* bss_template = 
        ".bss\n" 
        "\t.%*.s resq 1\n" 
        ".code\n";
    
    node_s* array = compiler->compiler_tree->nodes_array;
    node_s cur_node = array[lex];
    node_data_t val = cur_node.node_value;
        
    fprintf(compiler->file_output, bss_template, 
                val.value.id.id.string_size, val.value.id.id.string_source);

    return COMPILER_RETURN_SUCCESS;
}

static compiler_return_e 
CompileLocalVar(ssize_t    lex, 
                compiler_t compiler)
{
    assert(compiler != nullptr);
    assert(lex != NO_LINK);

    node_s* array = compiler->compiler_tree->nodes_array;
    
    if (array[array[lex].right_index].node_value.lex_type
            == LEX_TYPE_CONST)
    {
        fprintf(compiler->file_output, "\tmov ");
        GetVarPos(array[lex].left_index, compiler);
        fprintf(compiler->file_output, ", %ld\n", 
                    array[array[lex].right_index].node_value.value.constant);
    }

    // CompileRValue(lex, compiler); 
    
    return COMPILER_RETURN_SUCCESS; 
}


static compiler_return_e
CompileVar(ssize_t    lex,
           compiler_t compiler)
{
    assert(compiler != nullptr);
    assert(lex != NO_LINK);

    node_s* array = compiler->compiler_tree->nodes_array;
    node_s id_node = array[array[lex].left_index];
    node_data_t val_id = id_node.node_value;

    assert(val_id.lex_type != LEX_TYPE_ID);

    if (val_id.value.id.is_global)
    {
        if (array[lex].right_index != NO_LINK)
        {
            // array[cur_node.right_index];
        }
        else
        {
        
        }
    }
    else 
    {
        if (array[lex].right_index != NO_LINK)
        {
            CHECK_OUTPUT(CompileLocalVar(lex, compiler));
        }
    }

    return COMPILER_RETURN_SUCCESS;
}

// ----------------------------- compile_function -----------------------------

static compiler_return_e 
CompileFunction(ssize_t    lex,
                compiler_t compiler)
{
    assert(lex != NO_LINK);
    assert(compiler != nullptr);
    
    node_s* array = compiler->compiler_tree->nodes_array;
    node_s cur_node = array[lex];
    node_data_t val = cur_node.node_value;
    
    

    
    return COMPILER_RETURN_SUCCESS;
}

// --------------------------- compile_keyword ------------------------------

static compiler_return_e
CompileKW(ssize_t    lex,
          compiler_t compiler)
{
    assert(compiler != nullptr);
    assert(lex != NO_LINK);

    node_s* array = compiler->compiler_tree->nodes_array;
    node_s cur_node = array[lex];
    node_data_t val = cur_node.node_value;
    
    assert(val.lex_type != LEX_TYPE_KEY_WORD);

    switch (val.value.key_word)
    {
        case KEY_WORD_IF: return COMPILER_RETURN_SUCCESS;
        case KEY_WORD_VAR:       return CompileVar(lex, compiler);
        case KEY_WORD_WHILE:
        case KEY_WORD_FUNCTION:  return CompileFunction(lex, compiler);
        case KEY_WORD_RETURN: 
        case KEY_WORD_ELSE: 
                                 return COMPILER_RETURN_SUCCESS;
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
        if (!((val.lex_type == LEX_TYPE_SYNTAX)
                && (val.value.syntax == SYNTAX_STATEMENT_CONNECTOR)))
        {
            return COMPILER_RETURN_INCORRECT_AST;
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
    assert(compiler != nullptr);
    
    SetASMHeader(compiler); 

    node_s* array = compiler->compiler_tree->nodes_array;
    ssize_t start_node= array[0].left_index;

    return CompileBranch(start_node, compiler);
}

#pragma clang diagnostic warning "-Wformat-nonliteral"