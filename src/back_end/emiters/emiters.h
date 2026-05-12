#ifndef EMITERS_H
#define EMITERS_H

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
    NONE = 0b10000
};

void 
emit_syscall(segment_t segment);
             
// ================================= PUSH/POP =================================

void 
emit_push(segment_t segment,
          reg_e     reg);

void 
emit_pop(segment_t segment,
         reg_e     reg);

// ================================== CALL/RET ================================

void 
emit_call(segment_t segment,
          uint64_t  addr);

void 
emit_ret(segment_t segment);

// ==================================== MOVE ==================================

// move reg to reg

void 
emit_mov(segment_t segment,
         reg_e     reg_d,
         reg_e     reg_s);

// move const to reg
          
void 
emit_mov(segment_t segment,
         reg_e     reg_d,
         int64_t   constant);

// move from/to memory

void 
emit_mov_abs_mem(segment_t segment,
                 reg_e     reg_d,
                 uint64_t  addr);
              
void 
emit_mov_abs_mem(segment_t segment,
                 uint64_t  addr,
                 reg_e     reg_d);

void 
emit_mov_mem(segment_t segment,
             reg_e     reg_d,
             reg_e     reg_b,
             int64_t   offset);

void 
emit_mov_mem(segment_t segment,
             reg_e     reg_b,
             int64_t   offset,
             reg_e     reg_s);

// ================================== CMOVE ===================================
          
void  
emit_cmove(segment_t segment,
           reg_e     reg_d,
           reg_e     reg_s);

void  
emit_cmovne(segment_t segment,
            reg_e     reg_d,
            reg_e     reg_s);


void  
emit_cmovg(segment_t segment,
           reg_e     reg_d,
           reg_e     reg_s);

void  
emit_cmovge(segment_t segment,
            reg_e     reg_d,
            reg_e     reg_s);

void  
emit_cmovl(segment_t segment,
           reg_e     reg_d,
           reg_e     reg_s);

void  
emit_cmovle(segment_t segment);

// ================================ ARITHMETIC ================================

void 
emit_cmp(segment_t segment,
         reg_e     reg_a,
         reg_e     reg_b);

void 
emit_add(segment_t segment,
         reg_e     reg_d,
         reg_e     reg_s);

void 
emit_sub(segment_t segment,
         reg_e     reg_d,
         reg_e     reg_s);

void 
emit_div(segment_t segment,
         reg_e     reg_d,
         reg_e     reg_s);
         
void 
emit_mul(segment_t segment,
         reg_e     reg_d,
         reg_e     reg_s);

// ================================== JUMPS ===================================
          
// void 
// emit_jmp(section_t segment,
//          address   addr);

#endif // EMITERS_H