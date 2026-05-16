#ifndef COMMON_EMITERS_H
#define COMMON_EMITERS_H

#include "emiters.h"
#include "compiler.h"

const char* const REG_NAMES[] ={
    "rax",
    "rcx",
    "rdx",
    "rbx",
    "rsp",
    "rbp",
    "rsi",
    "rdi",
    "r8",
    "r9",    
    "r10",    
    "r11",    
    "r12",    
    "r13",    
    "r14",    
    "r15",    
};

// ============================== common_emitter ==============================

compiler_return_e 
MovRegVar(compiler_t compiler,
          ssize_t    lex,
          reg_e      reg,
          bool       reverse = false);

#define NASM_EMIT(...) fprintf(compiler->file_output, __VA_ARGS__)

// ---------------------------------- set_e -----------------------------------

#define EMIT_SETE_BL() \
    do {NASM_EMIT("\tsete BL\n");\
    emit_sete_bl(compiler->main_segment);} while(0)

#define EMIT_SETNE_BL() \
    do {NASM_EMIT("\tsetne BL\n");\
    emit_setne_bl(compiler->main_segment);} while(0)
                           
#define EMIT_SETG_BL() \
    do {NASM_EMIT("\tsetg BL\n");\
    emit_setg_bl(compiler->main_segment);} while(0)
                           
#define EMIT_SETGE_BL() \
    do {NASM_EMIT("\tsetge BL\n");\
    emit_setge_bl(compiler->main_segment);} while(0)

#define EMIT_SETGE_BL() \
    do {NASM_EMIT("\tsetge BL\n");\
    emit_setge_bl(compiler->main_segment);} while(0)

#define EMIT_SETLE_BL() \
    do {NASM_EMIT("\tsetle BL\n");\
    emit_setle_bl(compiler->main_segment);} while(0)

#define EMIT_SETL_BL() \
    do {NASM_EMIT("\tsetl BL\n");\
    emit_setl_bl(compiler->main_segment);} while(0)
                           
// ---------------------------------- mov -------------------------------------  

#define EMIT_MOV_REG_VAR(___REG___, ___VAR___) \
    do {MovRegVar(compiler, (___VAR___), (___REG___)); } while(0)

#define EMIT_MOV_VAR_REG(___REG___, ___VAR___) \
    do {MovRegVar(compiler, (___VAR___), (___REG___), true); } while(0)

#define EMIT_MOV_REG_REG(___REG_L___, ___REG_R___) \
    do {NASM_EMIT("\tmov %s, %s\n", REG_NAMES[___REG_L___], REG_NAMES[___REG_R___]);\
    emit_mov(compiler->main_segment, ___REG_L___, ___REG_R___);} while(0)

#define EMIT_MOV_REG_CONST(___REG___, ___CONST___)\
    do {NASM_EMIT("\tmov %s, %ld\n", \
        REG_NAMES[(___REG___)], ___CONST___);    \
    emit_mov(compiler->main_segment, (___REG___), (___CONST___)); } while (0)

// -------------------------------- push/pop ----------------------------------

#define EMIT_PUSH_REG(___REG___) \
    do {NASM_EMIT("\tpush %s\n", REG_NAMES[(___REG___)]);\
    emit_push(compiler->main_segment, (___REG___));} while(0)

#define EMIT_POP_REG(___REG___) \
    do {NASM_EMIT("\tpop %s\n", REG_NAMES[(___REG___)]);\
    emit_pop(compiler->main_segment, (___REG___));} while(0)

// ----------------------------- call/ret -------------------------------------

#define EMIT_CALL(___STRING___)\
do {NASM_EMIT("\tcall %.*s\n", (int) (___STRING___).size, (___STRING___).string);\
    SetPlaceholder(compiler, (___STRING___), compiler->main_segment->cur_pos, \
                        &compiler->placehldr.func);\
    emit_call(compiler->main_segment, 0x0);} while (0)

#define EMIT_RET()\
    do {NASM_EMIT( "\tret\n");\
    emit_ret(compiler->main_segment);} while(0) 

// ------------------------------- arithm -------------------------------------

#define EMIT_ADD_REG_REG(___REG_L___, ___REG_R___) \
    do {NASM_EMIT("\tadd %s, %s\n", \
        REG_NAMES[(___REG_L___)], REG_NAMES[(___REG_R___)]);\
    emit_add(compiler->main_segment, ___REG_L___, ___REG_R___);} while (0)

#define EMIT_SUB_REG_REG(___REG_L___, ___REG_R___) \
    do {NASM_EMIT("\tsub %s, %s\n", \
        REG_NAMES[(___REG_L___)], REG_NAMES[(___REG_R___)]);\
    emit_sub(compiler->main_segment, ___REG_L___, ___REG_R___);} while (0)


#define EMIT_SUB_RSP_CONST(___CONST___) \
    do {NASM_EMIT("\tsub rsp, %ld\n", (___CONST___));\
    emit_sub_rsp_const(compiler->main_segment, (___CONST___));} while (0)
    
#define EMIT_IMUL_REG_REG(___REG_L___, ___REG_R___) \
    do {NASM_EMIT("\timul %s, %s\n", \
        REG_NAMES[(___REG_L___)], REG_NAMES[(___REG_R___)]);\
    emit_imul(compiler->main_segment, ___REG_L___, ___REG_R___);} while (0)

#define EMIT_IDIV_REG(___REG___)\
    do {NASM_EMIT("\tidiv %s\n", REG_NAMES[(___REG___)]);\
    emit_idiv(compiler->main_segment, ___REG___);} while (0)
    
#define EMIT_CQO()\
    do {NASM_EMIT("\tcqo\n");\
    emit_cqo(compiler->main_segment);} while(0)

// ------------------------------- syscall ------------------------------------

#define EMIT_SYSCALL()\
    do {NASM_EMIT("\tsyscall\n");\
    emit_syscall(compiler->main_segment);} while(0)

// -------------------------------- test --------------------------------------

#

#endif // COMMON_EMITERS_H