#include "emiters.h"

#include <assert.h>
#include <cstdint>
#include <cstring>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>

#include "my_elf.h"

[[maybe_unused]] const uint8_t MAGIC_NUM_REX = 0b0100'0000;
[[maybe_unused]] const uint8_t W_REX = 0b1000;
[[maybe_unused]] const uint8_t R_REX = 0b0100;
[[maybe_unused]] const uint8_t X_REX = 0b0010;
[[maybe_unused]] const uint8_t B_REX = 0b0001;

const uint8_t FILL_BETWEEN_INSTR = 0xcc;

// ============================== SECTION_CONTROL =============================

void 
SectionAddByte(segment_t segment,
               uint8_t   byte)
{
    assert(segment != nullptr);

    *(segment->segment + segment->cur_pos) = byte;
}

#define EMIT(_X_) SegmentEmitByte(segment, (_X_))
#define EMIT_D(_X_) SegmentEmitDword(segment, (_X_))
#define EMIT_Q(_X_) SegmentEmitQword(segment, (_X_))
#define DEBUG() EMIT(FILL_BETWEEN_INSTR)

// ================================= EMITERS ==================================

// ----------------------------------------------------------------------------

void 
emit_push(segment_t segment,
          reg_e     reg)
{
    assert(segment != nullptr);

    DEBUG();

    const uint8_t op_code = 0x50;
    if (reg & REG_EXTENDED)
    {
        EMIT(MAGIC_NUM_REX | B_REX);
    }
    
    EMIT(op_code | ((uint8_t) reg & (~REG_EXTENDED)));
}

void
emit_pop(segment_t segment,
         reg_e reg)
{
    assert(segment != nullptr);

    DEBUG();
    const uint8_t op_code = 0x58;

    if (reg & REG_EXTENDED)
    {
        EMIT(MAGIC_NUM_REX | B_REX);
    }
    
    EMIT(op_code | ((uint8_t) reg & (~REG_EXTENDED)));
}

// ----------------------------------------------------------------------------

void 
emit_call(segment_t segment,
          uint64_t  address)
{
    assert(segment != nullptr);

    DEBUG();
    const uint8_t op_code = 0xE8;

    EMIT(op_code);
    const size_t rel_start = segment->cur_pos + 4; 
    
    int32_t rel_address = (int32_t) ((int64_t) address - (int64_t) rel_start); 

    EMIT_D((uint32_t) rel_address);
}

void 
emit_ret(segment_t segment)
{
    assert(segment != nullptr);
    DEBUG();

    const uint8_t op_code = 0xC3;
    EMIT(op_code);
}

// ----------------------------------------------------------------------------

void 
emit_mov(segment_t segment,
          reg_e     reg_d,
          reg_e     reg_s)
{
    assert(segment != nullptr);
    
    DEBUG();
    const uint8_t op_code = 0x89;
    
    uint8_t rex_byte = MAGIC_NUM_REX | W_REX;

    if (reg_d & REG_EXTENDED) rex_byte |= R_REX;
    if (reg_s & REG_EXTENDED) rex_byte |= B_REX;
    EMIT(rex_byte);
    EMIT(op_code);

    const uint8_t r_to_r = 0b11000000;
    uint8_t rw_byte = (uint8_t) (r_to_r |((reg_d & ~REG_EXTENDED) << 3) | (reg_s & ~REG_EXTENDED));
    EMIT(rw_byte);
}

void 
emit_mov(segment_t segment,
         reg_e     reg_d,
         int64_t   constant)
{
    assert(segment != nullptr);

    DEBUG();
    const uint8_t op_code = 0xB8;

    uint8_t rex_byte = MAGIC_NUM_REX | W_REX;
    if (reg_d & REG_EXTENDED)
    {                
        rex_byte |= R_REX;
    }
    EMIT(rex_byte);

    EMIT(op_code | ((uint8_t) reg_d & (~REG_EXTENDED)));
    
    EMIT_Q(constant);
}

void 
emit_move_mem(segment_t segment,
              reg_e     reg_d,
              uint64_t  addr)
{
    assert(segment != nullptr);
    
    DEBUG();
    const uint8_t op_code = 0x8b;
    
    uint8_t rex_byte = MAGIC_NUM_REX | W_REX;
    if (reg_d & REG_EXTENDED)
    {                
        rex_byte |= R_REX;
    }
    EMIT(rex_byte);
    EMIT(op_code);

    uint8_t rw_byte = (uint8_t) ((reg_d & ~REG_EXTENDED) << 3 | 0b100);
    EMIT(rw_byte);

    const uint8_t magic_byte = 0x25;
    EMIT(magic_byte);
    
    EMIT_D((uint32_t) addr);
}

void 
emit_mov_abs_mem(segment_t segment,
              uint64_t  addr,
              reg_e     reg_d)
{
    assert(segment != nullptr);
    
    DEBUG();
    const uint8_t op_code = 0x89;
    
    uint8_t rex_byte = MAGIC_NUM_REX | W_REX;
    if (reg_d & REG_EXTENDED)
    {                
        rex_byte |= R_REX;
    }
    EMIT(rex_byte);
    EMIT(op_code);

    uint8_t rw_byte = (uint8_t) ((reg_d & ~REG_EXTENDED) << 3 | 0b100);
    EMIT(rw_byte);

    const uint8_t magic_byte = 0x25;
    EMIT(magic_byte);
    
    EMIT_D((uint32_t) addr);
}

void 
emit_mov_mem(segment_t segment,
              reg_e     reg_d,
              reg_e     reg_b,
              int64_t   offset)
{
    assert(segment != nullptr);

    DEBUG();
    const uint8_t op_code = 0x8b;
    
    uint8_t rex_byte = MAGIC_NUM_REX | W_REX;

    if (reg_b & REG_EXTENDED) rex_byte |= B_REX;
    if (reg_d & REG_EXTENDED) rex_byte |= R_REX;
    EMIT(rex_byte);
    EMIT(op_code);
        
    const uint8_t m_to_r = 0b10000000;
    
    EMIT((uint8_t) (m_to_r | ((reg_d & ~REG_EXTENDED) << 3 | (reg_b & ~REG_EXTENDED))));
    
    EMIT_D((uint32_t) offset);
}

void 
emit_mov_mem(segment_t segment,
              reg_e     reg_b,
              int64_t   offset,
              reg_e     reg_d)
{
    assert(segment != nullptr);

    DEBUG();
    const uint8_t op_code = 0x89;
    
    uint8_t rex_byte = MAGIC_NUM_REX | W_REX;

    if (reg_b & REG_EXTENDED) rex_byte |= B_REX;
    if (reg_d & REG_EXTENDED) rex_byte |= R_REX;
    EMIT(rex_byte);
    EMIT(op_code);
        
    const uint8_t m_to_r = 0b01000000;
    
    EMIT((uint8_t) (m_to_r | ((reg_d & ~REG_EXTENDED) << 3 | (reg_d & ~REG_EXTENDED))));
    
    EMIT_D((uint32_t) offset);
}
   
// ----------------------------------------------------------------------------

static void 
emit_cmov__(segment_t segment,
            reg_e     reg_d,
            reg_e     reg_s,
            uint8_t   cond_byte)
{
    assert(segment != nullptr);

    DEBUG();
    const uint8_t op_code = 0x0f;

    uint8_t rex_byte = MAGIC_NUM_REX | W_REX;

    if (reg_d & REG_EXTENDED) rex_byte |= B_REX;
    if (reg_s & REG_EXTENDED) rex_byte |= R_REX;
    EMIT(rex_byte);
    EMIT(op_code);
    EMIT(cond_byte);

    const uint8_t r_to_r = 0b11000000;
    uint8_t rw_byte = (uint8_t) (r_to_r |((reg_d & ~REG_EXTENDED) << 3) | (reg_s & ~REG_EXTENDED));
    EMIT(rw_byte);

}

void  
emit_cmove(segment_t segment,
           reg_e     reg_d,
           reg_e     reg_s)
{
    assert(segment != nullptr);

    const uint8_t cond_byte = 0x44;
    emit_cmov__(segment, reg_d, reg_s, cond_byte);
}

void  
emit_cmovne(segment_t segment,
           reg_e     reg_d,
           reg_e     reg_s)
{
    assert(segment != nullptr);

    const uint8_t cond_byte = 0x45;
    emit_cmov__(segment, reg_d, reg_s, cond_byte);
}

void  
emit_cmovg(segment_t segment,
           reg_e     reg_d,
           reg_e     reg_s)
{
    assert(segment != nullptr);

    const uint8_t cond_byte = 0x4f;
    emit_cmov__(segment, reg_d, reg_s, cond_byte);
}

void  
emit_cmovge(segment_t segment,
           reg_e     reg_d,
           reg_e     reg_s)
{
    assert(segment != nullptr);

    const uint8_t cond_byte = 0x4c;
    emit_cmov__(segment, reg_d, reg_s, cond_byte);
}

void  
emit_cmovl(segment_t segment,
           reg_e     reg_d,
           reg_e     reg_s)
{
    assert(segment != nullptr);

    const uint8_t cond_byte = 0x4c;
    emit_cmov__(segment, reg_d, reg_s, cond_byte);
}

void  
emit_cmovle(segment_t segment,
           reg_e     reg_d,
           reg_e     reg_s)
{
    assert(segment != nullptr);

    const uint8_t cond_byte = 0x4e;
    emit_cmov__(segment, reg_d, reg_s, cond_byte);
}

// ----------------------------------------------------------------------------

static void 
emit_arithmetic(segment_t segment,
                reg_e     reg_d,
                reg_e     reg_s,
                uint8_t   op_code)
{
    assert(segment != nullptr);

    DEBUG();
    uint8_t rex_byte = MAGIC_NUM_REX | W_REX;

    if (reg_d & REG_EXTENDED) rex_byte |= B_REX;
    if (reg_s & REG_EXTENDED) rex_byte |= R_REX;
    EMIT(rex_byte);
    EMIT(op_code);
    
    const uint8_t r_to_r = 0b11000000;
    uint8_t rw_byte = (uint8_t) (r_to_r |((reg_s & ~REG_EXTENDED) << 3) 
                                    | (reg_d & ~REG_EXTENDED));
    EMIT(rw_byte);
}

void
emit_cmp(segment_t segment,
         reg_e     reg_d,
         reg_e     reg_s)
{
    assert(segment != nullptr);

    DEBUG();
    const uint8_t op_code = 0x39;

    emit_arithmetic(segment, reg_d, reg_s, op_code);
}

void
emit_add(segment_t segment,
         reg_e     reg_d,
         reg_e     reg_s)
{
    assert(segment != nullptr);

    DEBUG();
    const uint8_t op_code = 0x01;

    emit_arithmetic(segment, reg_d, reg_s, op_code);
}

void 
emit_syscall(segment_t segment)
{
    assert(segment != nullptr);

    DEBUG();
    const uint8_t op_code_1 = 0x0f;
    const uint8_t op_code_2 = 0x05;

    EMIT(op_code_1);
    EMIT(op_code_2);
}

// void
// emit_imul(section_t segment,
//           reg_e     reg_d,
//           reg_e     reg_s)
// {
//     assert(segment != nullptr);

    // const uint8_t op_code = 0x01;

    // emit_arithmetic(segment, reg_d, reg_s, op_code);
// }
