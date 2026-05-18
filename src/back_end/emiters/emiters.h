#ifndef EMITERS_H
#define EMITERS_H

#include <cassert>
#include <cstdint>
#include <stdint.h>
#include <stdio.h>

#include "my_elf.h"

const uint8_t REG_EXTENDED = 0b1000;

enum reg_e 
{
    RAX = 0b000,
    RCX = 0b001,
    RDX = 0b010,
    RBX = 0b011,
    RSP = 0b100,
    RBP = 0b101,
    RSI = 0b110,
    RDI = 0b111,
    R8  = 0b000 | REG_EXTENDED,
    R9  = 0b001 | REG_EXTENDED,
    R10 = 0b010 | REG_EXTENDED,
    R11 = 0b011 | REG_EXTENDED,
    R12 = 0b100 | REG_EXTENDED,
    R13 = 0b101 | REG_EXTENDED,
    R14 = 0b110 | REG_EXTENDED,
    R15 = 0b111 | REG_EXTENDED,
};

void 
emit_syscall(section_t section);
             
// ================================= PUSH/POP =================================

void 
emit_push(section_t section,
          reg_e     reg);

void 
emit_pop(section_t section,
         reg_e     reg);

// ================================== CALL/RET ================================

void 
emit_call(section_t section,
          uint64_t  addr);

void 
emit_ret(section_t section);

// ==================================== MOVE ==================================

// move reg to reg

void 
emit_mov(section_t section,
         reg_e     reg_d,
         reg_e     reg_s);

// move const to reg
          
void 
emit_mov(section_t section,
         reg_e     reg_d,
         int64_t   constant);

// move from/to memory

void 
emit_mov_abs_mem(section_t section,
                 reg_e     reg_d,
                 uint64_t  addr);
              
void 
emit_mov_abs_mem(section_t section,
                 uint64_t  addr,
                 reg_e     reg_d);

void 
emit_mov_mem(section_t section,
             reg_e     reg_d,
             reg_e     reg_b,
             int64_t   offset);

void 
emit_mov_mem(section_t section,
             reg_e     reg_b,
             int64_t   offset,
             reg_e     reg_s);

// ================================== CMOVE ===================================
          
void 
emit_sete_bl(section_t section);

void 
emit_setne_bl(section_t section);

void 
emit_setg_bl(section_t section);

void 
emit_setge_bl(section_t section);

void 
emit_setl_bl(section_t section);

void 
emit_setle_bl(section_t section);

// ================================ ARITHMETIC ================================

void 
emit_cmp(section_t section,
         reg_e     reg_a,
         reg_e     reg_b);

void 
emit_test(section_t section,
          reg_e     reg_l,
          reg_e     reg_r);
         
void 
emit_add(section_t section,
         reg_e     reg_d,
         reg_e     reg_s);

void 
emit_sub(section_t section,
         reg_e     reg_d,
         reg_e     reg_s);

void 
emit_sub_rsp_const(section_t section, 
                   int32_t   num);
         
void 
emit_idiv(section_t section,
          reg_e     reg);
         
void 
emit_imul(section_t section,
          reg_e     reg_d,
          reg_e     reg_s);

void 
emit_cqo(section_t section);

// ================================== JUMPS ===================================
          
void 
emit_jmp(section_t section,
         uint32_t  addr);

void 
emit_jz(section_t section,
        uint32_t  addr);

#endif // EMITERS_H