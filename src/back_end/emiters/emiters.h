#ifndef EMITERS_H
#define EMITERS_H

#include <cstdint>
#include <stdint.h>
#include <stdio.h>

const uint8_t REG_EXTENDED = 0b1000;

struct section_s
{
    uint8_t* section;
    size_t   cur_pos;
};
typedef section_s* section_t;

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
emit_move(section_t section,
          reg_e     reg_d,
          reg_e     reg_s);

// move const to reg
          
void 
emit_move(section_t section,
          reg_e     reg_d,
          int64_t   constant);

// move from/to memory

void 
emit_move_mem(section_t section,
              reg_e     reg_d,
              uint64_t  addr);
              
void 
emit_move_mem(section_t section,
              uint64_t  addr,
              reg_e     reg_d);

void 
emit_move_mem(section_t section,
              reg_e     reg_d,
              reg_e     reg_b,
              int64_t   offset);

void 
emit_move_mem(section_t section,
              reg_e     reg_b,
              int64_t   offset,
              reg_e     reg_s);

// ================================== CMOVE ===================================
          
void  
emit_cmove(section_t section,
           reg_e     reg_d,
           reg_e     reg_s);

void  
emit_cmovne(section_t section,
            reg_e     reg_d,
            reg_e     reg_s);


void  
emit_cmovg(section_t section,
           reg_e     reg_d,
           reg_e     reg_s);

void  
emit_cmovge(section_t section,
            reg_e     reg_d,
            reg_e     reg_s);

void  
emit_cmovl(section_t section,
           reg_e     reg_d,
           reg_e     reg_s);

void  
emit_cmovle(section_t section);

// ================================ ARITHMETIC ================================

void 
emit_cmp(section_t section,
         reg_e     reg_a,
         reg_e     reg_b);

void 
emit_add(section_t section,
         reg_e     reg_d,
         reg_e     reg_s);

void 
emit_sub(section_t section,
         reg_e     reg_d,
         reg_e     reg_s);

void 
emit_div(section_t section,
         reg_e     reg_d,
         reg_e     reg_s);
         
void 
emit_mul(section_t section,
         reg_e     reg_d,
         reg_e     reg_s);

// ================================== JUMPS ===================================
          
// void 
// emit_jmp(section_t section,
//          address   addr);

#endif // EMITERS_H