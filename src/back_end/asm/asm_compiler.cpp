#include "buffer.h"
#include "compiler.h"

#include <assert.h>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <stdlib.h>
#include <string.h>

#include "buffer.h"
#include "emiters.h"
#include "lexes.h"
#include "list.h"
#include "my_elf.h"
#include "tree.h"
#include "my_lang_lib.h"
#include "common_emiters.h"

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

#define COMMENT(___X___) fprintf(compiler->file_output, (___X___)) 

// ============================= AST COMPILE ==================================

// ---------------------------- placeholder -----------------------------------

static compiler_return_e 
SetPlaceholder(compiler_t compiler,
               string_s   string,
               size_t     offset,
               size_t*    prev_el)
{
    assert(compiler != nullptr);
    assert(prev_el != nullptr);

    if (*prev_el == 0)
    {
        data_type new_elem = {.string = string, .addr = offset};
        list_t list = compiler->placehldr.list;
        if (ListInitNewElem(list, &new_elem, prev_el))
        {
            return COMPILER_RETURN_LIST_ERROR;
        }
    }
    else 
    {
        data_type new_elem = {.string = string, .addr = offset};
        list_t list = compiler->placehldr.list;
        if (ListAddAfterElement(list, &new_elem, 
                (size_t) compiler->placehldr.func))
        {
            return COMPILER_RETURN_LIST_ERROR;
        }
        *prev_el = (size_t) GetNextElement(list, *prev_el);
    } 

    return COMPILER_RETURN_SUCCESS;
}

static compiler_return_e 
SetCall(compiler_t compiler,
        string_s   func,
        size_t     offset)
{
    assert(compiler != nullptr);

    uint64_t addr = HashTableGetElem(compiler->name_table, func);

    size_t cur_pos = compiler->main_segment->cur_pos;
    compiler->main_segment->cur_pos = offset;
    emit_call(compiler->main_segment, addr);
    compiler->main_segment->cur_pos = cur_pos;

    return COMPILER_RETURN_SUCCESS;
}


static compiler_return_e
FixPlaceHolders(compiler_t compiler)
{
    assert(compiler != nullptr);

    ssize_t prev_el = (ssize_t) compiler->placehldr.func;

    while (prev_el)
    {
        data_type el_val = {}; 
        GetElementValue(compiler->placehldr.list, (size_t) prev_el, &el_val);
        SetCall(compiler, el_val.string, el_val.addr);
        prev_el = GetPreviousElement(compiler->placehldr.list, (size_t) prev_el);
    } 

    return COMPILER_RETURN_SUCCESS;
}

// ----------------------------- compile_id -----------------------------------

static compiler_return_e 
CompileRegArg(size_t     arg_num,
              ssize_t    lex,
              compiler_t compiler)
{
    assert(compiler != nullptr);
    
    if (lex == NO_LINK)
    {
        return COMPILER_RETURN_INCORRECT_AST;
    }
    
    assert(arg_num <= 5);
    
    const reg_e elf_registers[] = 
    {
        RDI,
        RSI,
        RDX,
        RCX,
        R8,
        R9
    };
    
    EMIT_MOV_REG_REG(elf_registers[arg_num], RBX);

    return COMPILER_RETURN_SUCCESS;
}

static compiler_return_e
CompileRValue(ssize_t    lex,
              compiler_t compiler);

static compiler_return_e
CompileFunctionArgs(ssize_t    lex,
                    compiler_t compiler)
{
    assert(lex != NO_LINK);
    assert(compiler != nullptr);
    
    node_s* array = compiler->compiler_tree->nodes_array;
    node_data_t val = array[lex].node_value;
    size_t arg_count = (size_t) val.value.id.info1;
    size_t arg_num = 0;
    lex = array[lex].right_index;

    for (; (arg_num < 6) && (arg_num < arg_count); arg_num++)
    {
        if (lex == NO_LINK)
        {
            return COMPILER_RETURN_INCORRECT_AST;
        }

        CHECK_OUTPUT(CompileRValue(array[lex].right_index, compiler));
        CHECK_OUTPUT(CompileRegArg(arg_num, array[lex].right_index, compiler));
        lex = array[lex].left_index;
    }
    
    return COMPILER_RETURN_SUCCESS;
}

static compiler_return_e
CompileFuncCall(ssize_t    lex,
                compiler_t compiler)
{
    assert(compiler != nullptr);
    assert(lex != NO_LINK);

    node_s* array = compiler->compiler_tree->nodes_array;
    node_data_t val = array[lex].node_value;
    string_s id = val.value.id.id;


    CHECK_OUTPUT(CompileFunctionArgs(lex, compiler));
    EMIT_CALL(id);
    EMIT_MOV_REG_REG(RBX, RAX);

    return COMPILER_RETURN_SUCCESS;
}

static compiler_return_e
CompileID(ssize_t    lex,
          compiler_t compiler)
{
    assert(compiler != nullptr);
    assert(lex != NO_LINK);
    
    node_s* array = compiler->compiler_tree->nodes_array;
    node_data_t val = array[lex].node_value;
    id_s id = val.value.id;
    
    if (id.is_function)
    {
        CHECK_OUTPUT(CompileFuncCall(lex, compiler));
    }
    else
    {
        EMIT_MOV_REG_VAR(RBX, lex);
    }

    return COMPILER_RETURN_SUCCESS;
}


// --------------------------- compile_keyword ------------------------------

static compiler_return_e 
CompileRValue(ssize_t    lex, 
                compiler_t compiler);

static compiler_return_e 
CompileLocalVar(ssize_t    lex, 
                compiler_t compiler)
{
    assert(compiler != nullptr);
    assert(lex != NO_LINK);

    node_s* array = compiler->compiler_tree->nodes_array;
    
    CHECK_OUTPUT(CompileRValue(array[lex].right_index, compiler)); 
    EMIT_MOV_VAR_REG(RBX, array[lex].left_index);
    
    return COMPILER_RETURN_SUCCESS; 
}


static compiler_return_e
CompileVarKW(ssize_t    lex,
             compiler_t compiler)
{
    assert(compiler != nullptr);
    assert(lex != NO_LINK);

    node_s* array = compiler->compiler_tree->nodes_array;
    node_s id_node = array[array[lex].left_index];
    node_data_t val_id = id_node.node_value;

    assert(val_id.lex_type == LEX_TYPE_ID);

    if (val_id.value.id.is_global)
    {
        if (array[lex].right_index != NO_LINK)
        {
            assert(0); // sorry not added yet
        }
        else
        {
            assert(0);
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
CompileArguments(ssize_t    lex, 
                 compiler_t compiler)
{
    assert(compiler != nullptr);
    assert(lex != NO_LINK);
    
    node_s* array = compiler->compiler_tree->nodes_array;
    lex = array[lex].left_index;
    ssize_t arg_count = array[lex].node_value.value.id.info1; 
    
    COMMENT("\n;compile args\n");

    assert(arg_count <= 6); // not added yet  

    switch (arg_count)
    {
        default:
        case 6:
            EMIT_MOV_MEM_OFF_REG(RBP, -48, R9);
        case 5:
            EMIT_MOV_MEM_OFF_REG(RBP, -40, R8);
        case 4:
            EMIT_MOV_MEM_OFF_REG(RBP, -32, RCX);
        case 3:
            EMIT_MOV_MEM_OFF_REG(RBP, -24, RDX);
        case 2: 
            EMIT_MOV_MEM_OFF_REG(RBP, -16, RSI);
        case 1:
            EMIT_MOV_MEM_OFF_REG(RBP, -8, RDI);
        case 0:;
    }

    return COMPILER_RETURN_SUCCESS;
}

static compiler_return_e 
SetFunctionName(ssize_t    lex,
                compiler_t compiler)
{
    assert(compiler != nullptr);
    assert(lex != NO_LINK);

    node_s* array = compiler->compiler_tree->nodes_array;
    ssize_t id_ind = array[lex].left_index;
    if (id_ind == NO_LINK)
    {
        return COMPILER_RETURN_INCORRECT_AST;
    }
            
    string_s id = array[id_ind].node_value.value.id.id;
    
    fprintf(compiler->file_output, "\n%.*s:\n", 
                (int) id.size, id.string);

    HashTableAddElem(compiler->name_table, id, 
                        compiler->main_segment->cur_pos);

    return COMPILER_RETURN_SUCCESS;
}

static compiler_return_e
CompilePrologue(ssize_t    lex,
                compiler_t compiler)
{
    assert(compiler != nullptr);
    assert(lex != NO_LINK);
    
    COMMENT("\n;prologue\n");

    EMIT_PUSH_REG(RBP);
    EMIT_MOV_REG_REG(RBP, RSP);
    
    node_s* array = compiler->compiler_tree->nodes_array;
    size_t local_count = (size_t) array[array[lex].left_index] 
                                    .node_value.value.id.info2;

    EMIT_SUB_RSP_CONST(8 * (local_count + 6));
    CompileArguments(lex, compiler);

    EMIT_PUSH_REG(RBX);

    return COMPILER_RETURN_SUCCESS;
}


static compiler_return_e
CompileEpilogue(ssize_t    lex,
                compiler_t compiler)
{
    assert(compiler != nullptr);
    assert(lex != NO_LINK);
    
    COMMENT("\n;epilogue\n");
    
    EMIT_POP_REG(RBX);
    EMIT_MOV_REG_REG(RSP, RBP);
    EMIT_POP_REG(RBP);

    return COMPILER_RETURN_SUCCESS;
}


static compiler_return_e 
CompileBranch(ssize_t    lex,
              compiler_t compiler);

static compiler_return_e 
CompileFunction(ssize_t    lex,
                compiler_t compiler)
{
    assert(lex != NO_LINK);
    assert(compiler != nullptr);
    
    node_s* array = compiler->compiler_tree->nodes_array;
    
    assert(array[lex].node_value.lex_type == LEX_TYPE_KEY_WORD);
    assert(array[lex].node_value.value.key_word == KEY_WORD_FUNCTION);
    
    SetFunctionName(lex, compiler);
    CHECK_OUTPUT(CompilePrologue(lex, compiler));

    COMMENT("\n;body\n");
    CHECK_OUTPUT(CompileBranch(array[lex].right_index, compiler));

    
    return COMPILER_RETURN_SUCCESS;
}

// --------------------------- compile_keyword ------------------------------

static compiler_return_e 
CompileReturn(ssize_t    lex,
              compiler_t compiler)
{
    assert(compiler != nullptr);
    assert(lex != NO_LINK);
    
    node_s* array = compiler->compiler_tree->nodes_array;
    node_s cur_node = array[lex];
    
    CHECK_OUTPUT(CompileRValue(cur_node.right_index, compiler)); 
    EMIT_MOV_REG_REG(RAX, RBX);
    CHECK_OUTPUT(CompileEpilogue(lex, compiler));
    EMIT_RET();

    return COMPILER_RETURN_SUCCESS;
}
 
static compiler_return_e
CompileIf(ssize_t    lex,
          compiler_t compiler)
{
    assert(lex != NO_LINK);
    assert(compiler != nullptr);

    node_s* array = compiler->compiler_tree->nodes_array;
    node_s cur_node = array[lex];
    
    COMMENT("\n;if condition\n");

    CHECK_OUTPUT(CompileRValue(cur_node.left_index, compiler));
    
    size_t cond_jmp = compiler->label_count;
    compiler->label_count += 2;
    EMIT_TEST(RBX, RBX);
    
    NASM_EMIT("\tjz .L%zu\n", cond_jmp); 

    const size_t jz_placeholder = compiler->main_segment->cur_pos;  
    emit_jz(compiler->main_segment, 0x0);

    COMMENT("\n;if body\n");
    
    CHECK_OUTPUT(CompileBranch(cur_node.right_index, compiler));

    const size_t jmp_placeholder = compiler->main_segment->cur_pos;  
    emit_jmp(compiler->main_segment, 0x0);
    NASM_EMIT("\tjmp .L%zu\n", cond_jmp + 1); 

    NASM_EMIT(".L%zu:\n", cond_jmp);
    const size_t if_skip_label = compiler->main_segment->cur_pos;
    compiler->main_segment->cur_pos = jz_placeholder;
    emit_jz(compiler->main_segment, if_skip_label);
    compiler->main_segment->cur_pos = if_skip_label;
    
    if (array[cur_node.parent_index].right_index != NO_LINK) 
    {
        CHECK_OUTPUT(CompileBranch(array[cur_node.parent_index].right_index, compiler));
    }
    
    NASM_EMIT(".L%zu:\n", cond_jmp + 1); 
    const size_t else_skip_label = compiler->main_segment->cur_pos;
    compiler->main_segment->cur_pos = jmp_placeholder;
    emit_jmp(compiler->main_segment, else_skip_label);
    compiler->main_segment->cur_pos = else_skip_label;
    
    /*
        // condition 
        jz .L123:
        // right part of the if node
        .L123:
    */

    return COMPILER_RETURN_SUCCESS;
}

static compiler_return_e
CompileWhile(ssize_t    lex,
             compiler_t compiler)
{
    assert(lex != NO_LINK);
    assert(compiler != nullptr);

    node_s* array = compiler->compiler_tree->nodes_array;
    node_s cur_node = array[lex];
    
    COMMENT("\n;while condition\n");
    
    size_t cond_jmp = compiler->label_count;
    size_t test_jmp = compiler->label_count + 1;
    compiler->label_count += 2;
    
    fprintf(compiler->file_output, ".L%zu:\n", test_jmp);
    const size_t test_jmp_pos = compiler->main_segment->cur_pos; 

    CHECK_OUTPUT(CompileRValue(cur_node.left_index, compiler));
    EMIT_TEST(RBX, RBX);
    NASM_EMIT("\tjz .L%zu\n", cond_jmp); 
    
    const size_t jz_placeholder = compiler->main_segment->cur_pos;  
    emit_jz(compiler->main_segment, 0);

    COMMENT("\n;while body\n");
    CHECK_OUTPUT(CompileBranch(cur_node.right_index, compiler));
    NASM_EMIT("\tjmp .L%zu\n" 
              ".L%zu:\n", test_jmp, cond_jmp);
    emit_jmp(compiler->main_segment, (uint32_t) test_jmp_pos);
    
    const size_t skip_label = compiler->main_segment->cur_pos;
    compiler->main_segment->cur_pos = jz_placeholder;
    emit_jz(compiler->main_segment, skip_label);
    compiler->main_segment->cur_pos = skip_label;

    /* 
        .L124:
        // condition 
        je .L123:
        // right part of the if node
        jmp .L124:
        .L123:
    */

    return COMPILER_RETURN_SUCCESS;
}


static compiler_return_e
CompileStatement(ssize_t    lex,
                 compiler_t compiler);

static compiler_return_e 
CompileElse(ssize_t    lex,
            compiler_t compiler)
{
    assert(lex != NO_LINK);
    assert(compiler != nullptr);

    node_s* array = compiler->compiler_tree->nodes_array;
    node_s cur_node = array[lex];

    ssize_t kw_node = cur_node.left_index;
    assert(kw_node != NO_LINK);

    switch (array[kw_node].node_value.value.key_word)
    {
        case KEY_WORD_IF:        
            CHECK_OUTPUT(CompileIf(kw_node, compiler));
            break;
        case KEY_WORD_WHILE:     
            CHECK_OUTPUT(CompileWhile(kw_node, compiler));
            break;
        case KEY_WORD_VAR:       
        case KEY_WORD_FUNCTION:  
        case KEY_WORD_RETURN:    
        case KEY_WORD_ELSE:      
        case KEY_WORD_UNDEFINED: return COMPILER_RETURN_INCORRECT_AST;
        default: assert(0);
    }

    return COMPILER_RETURN_SUCCESS;
}

static compiler_return_e
CompileKW(ssize_t    lex,
          compiler_t compiler)
{
    assert(compiler != nullptr);
    assert(lex != NO_LINK);

    node_s* array = compiler->compiler_tree->nodes_array;
    node_s cur_node = array[lex];
    node_data_t val = cur_node.node_value;
    
    assert(val.lex_type == LEX_TYPE_KEY_WORD);

    switch (val.value.key_word)
    {
        case KEY_WORD_VAR:       return CompileVarKW(lex, compiler);
        case KEY_WORD_FUNCTION:  return CompileFunction(lex, compiler);
        case KEY_WORD_RETURN:    return CompileReturn(lex, compiler);
        case KEY_WORD_ELSE:      return CompileElse(lex, compiler);
        case KEY_WORD_IF:     
        case KEY_WORD_WHILE:     
        case KEY_WORD_UNDEFINED: return COMPILER_RETURN_INCORRECT_AST;
        default: assert(0);
    }

    return COMPILER_RETURN_SUCCESS;
}

// --------------------------- compile_constant ----------------------------

static compiler_return_e
CompileConst(ssize_t    lex,
             compiler_t compiler)
{
    assert(compiler != nullptr);
    assert(lex != NO_LINK);
    
    node_s* array = compiler->compiler_tree->nodes_array;
    node_s cur_node = array[lex];

    assert(cur_node.node_value.lex_type == LEX_TYPE_CONST);

    EMIT_MOV_REG_CONST(RBX, cur_node.node_value.value.constant);

    return COMPILER_RETURN_SUCCESS;
}

// --------------------------- compile_rvalue ------------------------------

static compiler_return_e
CompileOp(ssize_t    lex,
          compiler_t compiler);

static compiler_return_e
CompileRValue(ssize_t    lex,
              compiler_t compiler)
{
    assert(compiler != nullptr);
    assert(lex != NO_LINK);

    node_s* array = compiler->compiler_tree->nodes_array;
    node_s cur_node = array[lex];
    node_data_t val = cur_node.node_value;

    switch (val.lex_type)
    {
        case LEX_TYPE_CONST:    return CompileConst(lex, compiler);
        case LEX_TYPE_OPERATOR: return CompileOp(lex, compiler);
        case LEX_TYPE_ID:       return CompileID(lex, compiler);

        case LEX_TYPE_UNDEFINED:
        case LEX_TYPE_KEY_WORD:
        case LEX_TYPE_SYNTAX:   return COMPILER_RETURN_INCORRECT_AST;
        default: assert(0);
    }

    return COMPILER_RETURN_SUCCESS;
}

static compiler_return_e
CompileAssignment(ssize_t    lex,
                  compiler_t compiler)
{
    assert(compiler != nullptr);
    assert(lex != NO_LINK);
    
    node_s* array = compiler->compiler_tree->nodes_array;
    node_s cur_node = array[lex];
    CHECK_OUTPUT(CompileRValue(cur_node.right_index, compiler));
    EMIT_MOV_VAR_REG(RBX, cur_node.left_index);
    
    return COMPILER_RETURN_SUCCESS;
}

              
static compiler_return_e
CompileBool(ssize_t         lex,
            operator_type_e op,
            compiler_t      compiler)
{
    assert(lex != NO_LINK);
    assert(compiler != nullptr);
    
    assert(op > OPERATOR_DIV);
    assert(op < OPERATOR_ASSIGNMENT);

    node_s* array = compiler->compiler_tree->nodes_array;
    node_s cur_node = array[lex];

    CHECK_OUTPUT(CompileRValue(cur_node.left_index, compiler));

    EMIT_PUSH_REG(RBX);

    CHECK_OUTPUT(CompileRValue(cur_node.right_index, compiler));

    EMIT_MOV_REG_REG(RAX, RBX);
    EMIT_POP_REG(RBX);

    fprintf(compiler->file_output, "\tcmp rbx, rax\n");
    emit_cmp(compiler->main_segment, RBX, RAX);
    fprintf(compiler->file_output, "\tmov rbx, 0\n");
    emit_mov(compiler->main_segment, RBX, 0);
    
    switch(op)
    {
        case OPERATOR_EQUALITY:
            EMIT_SETE_BL();
            break;
        case OPERATOR_N_EQUALITY:
            EMIT_SETNE_BL();
            break;
        case OPERATOR_MORE:
            EMIT_SETG_BL();
            break;
        case OPERATOR_MORE_OR_EQ:
            EMIT_SETGE_BL();
            break;
        case OPERATOR_LESS:
            EMIT_SETL_BL();
            break;
        case OPERATOR_LESS_OR_EQUAL:
            EMIT_SETLE_BL();
            break;
        default: assert(0);
    }

    
    return COMPILER_RETURN_SUCCESS;
}

static compiler_return_e
CompileArithm(ssize_t         lex,
              operator_type_e op,
              compiler_t      compiler)
{
    assert(lex != NO_LINK);
    assert(compiler != nullptr);
    
    assert(op <= OPERATOR_DIV);
    assert(op > 0); 

    node_s* array = compiler->compiler_tree->nodes_array;
    node_s cur_node = array[lex];
    
    CHECK_OUTPUT(CompileRValue(cur_node.left_index, compiler));
    EMIT_PUSH_REG(RBX);
    CHECK_OUTPUT(CompileRValue(cur_node.right_index, compiler));
    EMIT_MOV_REG_REG(RAX, RBX);
    EMIT_POP_REG(RBX);
    
    switch(op)
    {
        case OPERATOR_PLUS:
            EMIT_ADD_REG_REG(RBX, RAX);
            break;
        case OPERATOR_MINUS:
            EMIT_SUB_REG_REG(RBX, RAX);
            break;
        case OPERATOR_MUL:
            EMIT_IMUL_REG_REG(RBX, RAX);
            break;
        case OPERATOR_DIV:
            EMIT_MOV_REG_REG(RCX, RAX);
            EMIT_MOV_REG_REG(RAX, RBX);
            EMIT_CQO();
            EMIT_IDIV_REG(RCX);
            EMIT_MOV_REG_REG(RBX, RAX);
            break;
        default: assert(0);
    } 
    
    
    return COMPILER_RETURN_SUCCESS;
}

static compiler_return_e
CompileOp(ssize_t    lex,
          compiler_t compiler)
{
    assert(compiler != nullptr);
    assert(lex != NO_LINK);

    node_s* array = compiler->compiler_tree->nodes_array;
    node_s cur_node = array[lex];
    node_data_t val = cur_node.node_value;

    switch (val.value.op)
    {
        case OPERATOR_PLUS:    
        case OPERATOR_MINUS:
        case OPERATOR_MUL:
        case OPERATOR_DIV: 
            return CompileArithm(lex, val.value.op, compiler);
        case OPERATOR_EQUALITY:
        case OPERATOR_N_EQUALITY:
        case OPERATOR_MORE:
        case OPERATOR_MORE_OR_EQ:
        case OPERATOR_LESS:
        case OPERATOR_LESS_OR_EQUAL:
            return CompileBool(lex, val.value.op, compiler);
        case OPERATOR_ASSIGNMENT:
            return CompileAssignment(lex, compiler);

        case OPERATOR_UNDEFINED: return COMPILER_RETURN_INCORRECT_AST;
        default: assert(0);
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
        case LEX_TYPE_ID:        return CompileID(lex, compiler);
        case LEX_TYPE_KEY_WORD:  return CompileKW(lex, compiler);
        case LEX_TYPE_OPERATOR:  return CompileOp(lex, compiler);
        case LEX_TYPE_SYNTAX:    return COMPILER_RETURN_INCORRECT_AST;
        case LEX_TYPE_CONST:     return COMPILER_RETURN_SUCCESS;
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

    while (lex != NO_LINK)
    {
        if (!((val.lex_type == LEX_TYPE_SYNTAX)
                && (val.value.syntax == SYNTAX_STATEMENT_CONNECTOR)))
        {
            return CompileStatement(lex, compiler);
        }

        CHECK_OUTPUT(CompileStatement(array[lex].right_index, compiler));  
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

    NASM_EMIT("global _start\n");
    NASM_EMIT("_start:\n");
    
    SegmentCreateFileH(compiler->main_segment);
    SegmentCreateProgramH(compiler->main_segment);

    string_s main_name = {(char*) "main", strlen(main_name.string)};
   
    EMIT_CALL(main_name);

    emit_mov(compiler->main_segment, RDI, RAX);
    EMIT_MOV_REG_REG(RDI, RAX);
    const uint64_t exit_syscall = 0x3c;
    EMIT_MOV_REG_CONST(RAX, exit_syscall);
    EMIT_SYSCALL();

    return COMPILER_RETURN_SUCCESS;
}

// ------------------------------- std_lib ------------------------------------

static compiler_return_e 
CompileStdLib(compiler_t compiler)
{
    assert(compiler != nullptr);

    for (size_t i = 0; i < FUNCTIONS_AMOUNT; i++)
    {
        buffer_t buffer = nullptr;
        if (BufferCtor(&buffer, FUNCTIONS[i].file))
        {
            return COMPILER_RETURN_BUFFER_ERROR;
        }

        size_t cur_pos = compiler->main_segment->cur_pos;
        SectionInsertString(compiler->main_segment, {buffer->buffer, buffer->max_buffer});
        HashTableAddElem(compiler->name_table, FUNCTIONS[i].function_name, cur_pos);
        
        if (BufferDtor(&buffer))
        {
            return COMPILER_RETURN_BUFFER_ERROR;
        }
    }
        
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

    CompileStdLib(compiler);

    CHECK_OUTPUT(CompileBranch(start_node, compiler)); 

    FixPlaceHolders(compiler);

    return COMPILER_RETURN_SUCCESS;
}

#pragma clang diagnostic warning "-Wformat-nonliteral"
