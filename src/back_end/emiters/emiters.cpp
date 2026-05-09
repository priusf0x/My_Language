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

// ============================== SECTION_CONTROL =============================

void 
SectionAddByte(section_t section,
               uint8_t   byte)
{
    assert(section != nullptr);

    *(section->section + section->cur_pos) = byte;
}

#define EMIT(_X_) SectionEmitByte(section, (_X_))

// ================================= EMITERS ==================================

// ----------------------------------------------------------------------------

void 
emit_push(section_t section,
          reg_e     reg)
{
    assert(section != nullptr);

    const uint8_t op_code = 0x50;
    if (reg & REG_EXTENDED)
    {
        EMIT(MAGIC_NUM_REX | B_REX);
    }
    
    EMIT(op_code | ((uint8_t) reg & (~REG_EXTENDED)));
}

void
emit_pop(section_t section,
         reg_e reg)
{
    assert(section != nullptr);

    const uint8_t op_code = 0x58;
    
    if (reg & REG_EXTENDED)
    {
        EMIT(MAGIC_NUM_REX | B_REX);
    }
    
    EMIT(op_code | ((uint8_t) reg & (~REG_EXTENDED)));
}

// ----------------------------------------------------------------------------

void 
emit_call(section_t section,
          uint64_t  address)
{
    assert(section != nullptr);

    const uint8_t op_code = 0xE8;

    EMIT(op_code);
    const size_t rel_start = section->cur_pos + 4; 
    
    int32_t rel_address = (int32_t) ((int64_t) address - (int64_t) rel_start); 

    memcpy(section->section + section->cur_pos, &rel_address, sizeof(int32_t));
    section->cur_pos += sizeof(int32_t);
}

void 
emit_ret(section_t section)
{
    assert(section != nullptr);

    const uint8_t op_code = 0xC3;
    EMIT(op_code);
}

// ----------------------------------------------------------------------------

void 
emit_move(section_t section,
          reg_e     reg_d,
          reg_e     reg_s)
{
    assert(section != nullptr);
    
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
emit_move(section_t section,
          reg_e     reg_d,
          uint64_t  constant)
{
    assert(section != nullptr);

    const uint8_t op_code = 0xB8;

    uint8_t rex_byte = MAGIC_NUM_REX | W_REX;
    if (reg_d & REG_EXTENDED)
    {                
        rex_byte |= R_REX;
    }
    EMIT(rex_byte);

    EMIT(op_code | ((uint8_t) reg_d & (~REG_EXTENDED)));
    
    memcpy(section->section + section->cur_pos, &constant, sizeof(int64_t));
    section->cur_pos += sizeof(int64_t);
}

void 
emit_move_mem(section_t section,
              reg_e     reg_d,
              uint64_t  addr)
{
    assert(section != nullptr);
    
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
    
    memcpy(section->section + section->cur_pos, &addr, sizeof(int32_t));
    section->cur_pos += sizeof(int32_t);
}

void 
emit_move_mem(section_t section,
              uint64_t  addr,
              reg_e     reg_d)
{
    assert(section != nullptr);
    
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
    
    memcpy(section->section + section->cur_pos, &addr, sizeof(int32_t));
    section->cur_pos += sizeof(int32_t);
}

void 
emit_move_mem(section_t section,
              reg_e     reg_d,
              reg_e     reg_b,
              int64_t   offset)
{
    assert(section != nullptr);

    const uint8_t op_code = 0x8b;
    
    uint8_t rex_byte = MAGIC_NUM_REX | W_REX;

    if (reg_b & REG_EXTENDED) rex_byte |= B_REX;
    if (reg_d & REG_EXTENDED) rex_byte |= R_REX;
    EMIT(rex_byte);
    EMIT(op_code);
        
    const uint8_t m_to_r = 0b10000000;
    
    EMIT((uint8_t) (m_to_r | ((reg_d & ~REG_EXTENDED) << 3 | (reg_b & ~REG_EXTENDED))));
    
    memcpy(section->section + section->cur_pos, &offset, sizeof(int32_t));
    section->cur_pos += sizeof(int32_t);
}

void 
emit_move_mem(section_t section,
              reg_e     reg_b,
              int64_t   offset,
              reg_e     reg_d)
{
    assert(section != nullptr);

    const uint8_t op_code = 0x89;
    
    uint8_t rex_byte = MAGIC_NUM_REX | W_REX;

    if (reg_b & REG_EXTENDED) rex_byte |= B_REX;
    if (reg_d & REG_EXTENDED) rex_byte |= R_REX;
    EMIT(rex_byte);
    EMIT(op_code);
        
    const uint8_t m_to_r = 0b01000000;
    
    EMIT((uint8_t) (m_to_r | ((reg_d & ~REG_EXTENDED) << 3 | (reg_d & ~REG_EXTENDED))));
    
    memcpy(section->section + section->cur_pos, &offset, sizeof(int32_t));
    section->cur_pos += sizeof(int32_t);
}
   
// ----------------------------------------------------------------------------

static void 
emit_cmov__(section_t section,
            reg_e     reg_d,
            reg_e     reg_s,
            uint8_t   cond_byte)
{
    assert(section != nullptr);

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
emit_cmove(section_t section,
           reg_e     reg_d,
           reg_e     reg_s)
{
    assert(section != nullptr);

    const uint8_t cond_byte = 0x44;
    emit_cmov__(section, reg_d, reg_s, cond_byte);
}

void  
emit_cmovne(section_t section,
           reg_e     reg_d,
           reg_e     reg_s)
{
    assert(section != nullptr);

    const uint8_t cond_byte = 0x45;
    emit_cmov__(section, reg_d, reg_s, cond_byte);
}

void  
emit_cmovg(section_t section,
           reg_e     reg_d,
           reg_e     reg_s)
{
    assert(section != nullptr);

    const uint8_t cond_byte = 0x4f;
    emit_cmov__(section, reg_d, reg_s, cond_byte);
}

void  
emit_cmovge(section_t section,
           reg_e     reg_d,
           reg_e     reg_s)
{
    assert(section != nullptr);

    const uint8_t cond_byte = 0x4c;
    emit_cmov__(section, reg_d, reg_s, cond_byte);
}

void  
emit_cmovl(section_t section,
           reg_e     reg_d,
           reg_e     reg_s)
{
    assert(section != nullptr);

    const uint8_t cond_byte = 0x4c;
    emit_cmov__(section, reg_d, reg_s, cond_byte);
}

void  
emit_cmovle(section_t section,
           reg_e     reg_d,
           reg_e     reg_s)
{
    assert(section != nullptr);

    const uint8_t cond_byte = 0x4e;
    emit_cmov__(section, reg_d, reg_s, cond_byte);
}

// ----------------------------------------------------------------------------

static void 
emit_arithmetic(section_t section,
                reg_e     reg_d,
                reg_e     reg_s,
                uint8_t   op_code)
{
    assert(section != nullptr);

    uint8_t rex_byte = MAGIC_NUM_REX | W_REX;

    if (reg_d & REG_EXTENDED) rex_byte |= B_REX;
    if (reg_s & REG_EXTENDED) rex_byte |= R_REX;
    EMIT(rex_byte);
    EMIT(op_code);
    
    const uint8_t r_to_r = 0b11000000;
    uint8_t rw_byte = (uint8_t) (r_to_r |((reg_s & ~REG_EXTENDED) << 3) | (reg_d & ~REG_EXTENDED));
    EMIT(rw_byte);
    
}

void
emit_cmp(section_t section,
         reg_e     reg_d,
         reg_e     reg_s)
{
    assert(section != nullptr);

    const uint8_t op_code = 0x39;

    emit_arithmetic(section, reg_d, reg_s, op_code);
}

void
emit_add(section_t section,
         reg_e     reg_d,
         reg_e     reg_s)
{
    assert(section != nullptr);

    const uint8_t op_code = 0x01;

    emit_arithmetic(section, reg_d, reg_s, op_code);
}


void
emit_imul(section_t section,
          reg_e     reg_d,
          reg_e     reg_s)
{
    assert(section != nullptr);

    // const uint8_t op_code = 0x01;

    // emit_arithmetic(section, reg_d, reg_s, op_code);
}
