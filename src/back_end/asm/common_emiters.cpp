#include "common_emiters.h"

#include <assert.h>
#include <stdlib.h>

#include "compiler.h"
#include "emiters.h"

compiler_return_e 
MovRegVar(compiler_t compiler,
          ssize_t    lex,
          reg_e      reg,
          bool       reverse)
{
    assert(compiler != nullptr);
    assert(lex != NO_LINK);

    node_s* array = compiler->compiler_tree->nodes_array;
    node_s cur_node = array[lex];
    node_data_t val = cur_node.node_value;

    assert(val.lex_type == LEX_TYPE_ID);    
    assert(!val.value.id.is_function);    

    if (val.value.id.is_global)
    {
        assert(0); // not added yet 
    }
    else 
    {
        if (val.value.id.info2) // check if argument 
        {
            if (val.value.id.info1 < 7)
            {
                reverse ?   emit_mov_mem(compiler->main_segment, RBP, 
                                - (int64_t) (val.value.id.info1 * 8 + 8), reg) :
                            emit_mov_mem(compiler->main_segment, reg, RBP, 
                                - (int64_t) (val.value.id.info1 * 8 + 8));
                reverse ?   NASM_EMIT("\tmov [rbp - %ld], %s\n", 
                                 (int64_t) (val.value.id.info1 * 8 + 8), REG_NAMES[reg]) :
                            NASM_EMIT("\tmov %s, [rbp - %ld]\n", REG_NAMES[reg],
                                        (int64_t) (val.value.id.info1 * 8 + 8));
            }
            else 
            {
                reverse ?   emit_mov_mem(compiler->main_segment, RBP, 
                                 (int64_t) (val.value.id.info1 * 8 + 16), reg) :
                            emit_mov_mem(compiler->main_segment, reg, RBP, 
                                 (int64_t) (val.value.id.info1 * 8 + 16));
                reverse ?   NASM_EMIT("\tmov [rbp + %ld], %s\n", 
                                 (int64_t) (val.value.id.info1 * 8 + 16), REG_NAMES[reg]) :
                            NASM_EMIT("\tmov %s, [rbp + %ld]\n", REG_NAMES[reg],
                                        (int64_t) (val.value.id.info1 * 8 + 16));
            }
        }
        else 
        {
            reverse ?   emit_mov_mem(compiler->main_segment, RBP, 
                            - (int64_t) (val.value.id.info1 * 8 + 56), reg) :
                        emit_mov_mem(compiler->main_segment, reg, RBP, 
                            - (int64_t) (val.value.id.info1 * 8  + 56));
            reverse ?   NASM_EMIT("\tmov [rbp - %ld], %s\n", 
                             (int64_t) (val.value.id.info1 * 8 + 56), REG_NAMES[reg]) :
                        NASM_EMIT("\tmov %s, [rbp - %ld]\n", REG_NAMES[reg],
                             (int64_t) (val.value.id.info1 * 8 + 56));
        }
    }
    
    return COMPILER_RETURN_SUCCESS;
}