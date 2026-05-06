#ifndef EMITERS_H
#define EMITERS_H

#include <cstdint>
#include <stdint.h>
#include <stdio.h>

typedef uint64_t imm;
typedef int64_t  offset;
typedef uint64_t address;

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
    SKIP
};

// ================================= PUSH/POP =================================

size_t 
emit_push(char* section,
          reg_e reg);

size_t 
emit_pop(char* section,
         reg_e reg);

// ================================== CALL/RET ================================

size_t 
emit_call(char*   section,
          address address);

size_t 
emit_ret(char* section);

// ==================================== MOVE ==================================

size_t 
emit_move(char* section,
          reg_e reg_d,
          reg_e reg_s);

size_t 
emit_move(char* section,
          reg_e reg_d,
          imm   constant);

size_t 
emit_move(char*   section,
          reg_e   reg_d,
          address addr);

size_t 
emit_move(char*  section,
          reg_e  reg_d,
          reg_e  reg_b,
          offset offset);

size_t 
emit_move(char*   section,
          reg_e   reg_b,
          offset  offset,
          reg_e   reg_s);

// ================================== CMOVE ===================================
          
size_t  
emit_cmove(char* section);

size_t  
emit_cmovne(char* section);

size_t  
emit_cmovg(char* section);

size_t  
emit_cmovge(char* section);

size_t  
emit_cmovl(char* section);

size_t  
emit_cmovle(char* section);

// ================================ ARITHMETIC ================================

size_t 
emit_cmp(char* section,
         reg_e reg_a,
         reg_e reg_b);

size_t 
emit_add(char* section,
         reg_e reg_d,
         reg_e reg_s);

size_t 
emit_sub(char* section,
         reg_e reg_d,
         reg_e reg_s);

size_t 
emit_div(char* section,
         reg_e reg_d,
         reg_e reg_s);
         
size_t 
emit_mul(char* section,
         reg_e reg_d,
         reg_e reg_s);

// ================================== JUMPS ===================================
          
size_t 
emit_jmp(char*   section,
         address addr);

#endif // EMITERS_H