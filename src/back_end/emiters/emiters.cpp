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

const uint8_t FILL_BETWEEN_INSTR = 0x90;

// ============================== SECTION_CONTROL =============================

void 
SectionAddByte(section_t section,
               uint8_t   byte)
{
    assert(section != nullptr);

    *(section->section + section->cur_pos) = byte;
}

#define EMIT(_X_) SectionEmitByte(section, (_X_))
#define EMIT_D(_X_) SectionEmitDword(section, (_X_))
#define EMIT_Q(_X_) SectionEmitQword(section, (_X_))
#define DEBUG() 
// EMIT(FILL_BETWEEN_INSTR)

// ================================= EMITERS ==================================

// ----------------------------------------------------------------------------

void 
emit_push(section_t section,
          reg_e     reg)
{
    assert(section != nullptr);

    DEBUG();

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
emit_call(section_t section,
          uint64_t  address)
{
    assert(section != nullptr);

    DEBUG();
    const uint8_t op_code = 0xE8;

    EMIT(op_code);
    const size_t rel_start = section->cur_pos + 4; 
    
    int32_t rel_address = (int32_t) ((int64_t) address - (int64_t) rel_start); 

    EMIT_D((uint32_t) rel_address);
}

void 
emit_ret(section_t section)
{
    assert(section != nullptr);
    DEBUG();

    const uint8_t op_code = 0xC3;
    EMIT(op_code);
}

// ----------------------------------------------------------------------------

void 
emit_mov(section_t section,
         reg_e     reg_d,
         reg_e     reg_s)
{
    assert(section != nullptr);
    
    DEBUG();
    const uint8_t op_code = 0x89;
    
    uint8_t rex_byte = MAGIC_NUM_REX | W_REX;

    if (reg_s & REG_EXTENDED) rex_byte |= R_REX;
    if (reg_d & REG_EXTENDED) rex_byte |= B_REX;
    EMIT(rex_byte);
    EMIT(op_code);

    const uint8_t r_to_r = 0b11000000;
    uint8_t rw_byte = (uint8_t) (r_to_r |((reg_d & ~REG_EXTENDED)) | (reg_s & ~REG_EXTENDED) << 3);
    EMIT(rw_byte);
}

void 
emit_mov(section_t section,
         reg_e     reg_d,
         int64_t   constant)
{
    assert(section != nullptr);

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
emit_move_mem(section_t section,
              reg_e     reg_d,
              uint64_t  addr)
{
    assert(section != nullptr);
    
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
emit_mov_mem(section_t section,
              reg_e     reg_d,
              reg_e     reg_b,
              int64_t   offset)
{
    assert(section != nullptr);

    DEBUG();
    const uint8_t op_code = 0x8b;
    
    uint8_t rex_byte = MAGIC_NUM_REX | W_REX;

    if (reg_b & REG_EXTENDED) rex_byte |= B_REX;
    if (reg_d & REG_EXTENDED) rex_byte |= R_REX;
    EMIT(rex_byte);
    EMIT(op_code);
        
    const uint8_t m_to_r = 0b10000000;
    
    const uint8_t mode = uint8_t (offset >= -128) && (offset <= 127) ? 0b01 : 0b10;
    
    EMIT((uint8_t) (mode << 6 | ((reg_d & ~REG_EXTENDED) << 3 | (reg_b & ~REG_EXTENDED))));
    
    if (mode == 0b01) EMIT((uint8_t) offset);
    else if (mode == 0b01) EMIT_D((uint32_t) offset);
}

void 
emit_mov_mem(section_t section,
             reg_e     reg_b,
             int64_t   offset,
             reg_e     reg_d)
{
    assert(section != nullptr);

    DEBUG();
    const uint8_t op_code = 0x89;
    
    uint8_t rex_byte = MAGIC_NUM_REX | W_REX;

    if (reg_b & REG_EXTENDED) rex_byte |= B_REX;
    if (reg_d & REG_EXTENDED) rex_byte |= R_REX;
    EMIT(rex_byte);
    EMIT(op_code);

    const uint8_t mode = uint8_t (offset >= -128) && (offset <= 127) ? 0b01 : 0b10;
    
    EMIT((uint8_t) (mode << 6 | ((reg_d & ~REG_EXTENDED) << 3 | (reg_b & ~REG_EXTENDED))));
    
    if (mode == 0b01) EMIT((uint8_t) offset);
    else if (mode == 0b01) EMIT_D((uint32_t) offset);
}
   
// ----------------------------------------------------------------------------

static void 
emit_set_bl(section_t section, 
            uint8_t   op_code)
{
    assert(section != nullptr);

    const uint8_t op_code_1 = 0x0f;
    EMIT(op_code_1);
    EMIT(op_code);
    const uint8_t sib = 0xc3;
    EMIT(sib);

}

void 
emit_sete_bl(section_t section)
{
    assert(section != nullptr);

    const uint8_t op_code = 0x94;
    emit_set_bl(section, op_code);
}

void 
emit_setne_bl(section_t section)
{ 
    assert(section != nullptr);

    const uint8_t op_code = 0x95;
    emit_set_bl(section, op_code);
}

void 
emit_setg_bl(section_t section)
{
    assert(section != nullptr);

    const uint8_t op_code = 0x9f;
    emit_set_bl(section, op_code);
}

void 
emit_setge_bl(section_t section)
{
    assert(section != nullptr);

    const uint8_t op_code = 0x9d;
    emit_set_bl(section, op_code);
}

void 
emit_setl_bl(section_t section)
{
    assert(section != nullptr);

    const uint8_t op_code = 0x9c;
    emit_set_bl(section, op_code);
}

void 
emit_setle_bl(section_t section)
{
    assert(section != nullptr);

    const uint8_t op_code = 0x9e;
    emit_set_bl(section, op_code);
}

// ----------------------------------------------------------------------------

static void 
emit_arithmetic(section_t section,
                reg_e     reg_d,
                reg_e     reg_s,
                uint8_t   op_code)
{
    assert(section != nullptr);

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
emit_cmp(section_t section,
         reg_e     reg_d,
         reg_e     reg_s)
{
    assert(section != nullptr);

    DEBUG();
    const uint8_t op_code = 0x39;

    emit_arithmetic(section, reg_d, reg_s, op_code);
}

void 
emit_test(section_t section,
          reg_e     reg_l,
          reg_e     reg_r)
{
    assert(section != nullptr);
    
    DEBUG();

    uint8_t rex_byte = MAGIC_NUM_REX | W_REX;

    if (reg_l & REG_EXTENDED) rex_byte |= B_REX;
    if (reg_r & REG_EXTENDED) rex_byte |= R_REX;
    
    EMIT(rex_byte);

    const uint8_t op_code = 0x85; 
    EMIT(op_code);

    const uint8_t r_to_r = 0b11000000; 
    EMIT((r_to_r |((reg_r & ~REG_EXTENDED) << 3) 
                                    | (reg_l & ~REG_EXTENDED)));
}

void
emit_add(section_t section,
         reg_e     reg_d,
         reg_e     reg_s)
{
    assert(section != nullptr);

    DEBUG();
    const uint8_t op_code = 0x01;

    emit_arithmetic(section, reg_d, reg_s, op_code);
}

void
emit_sub(section_t section,
         reg_e     reg_d,
         reg_e     reg_s)
{
    assert(section != nullptr);

    DEBUG();
    const uint8_t op_code = 0x29;

    emit_arithmetic(section, reg_d, reg_s, op_code);
}

void 
emit_imul(section_t section,
          reg_e     reg_d,
          reg_e     reg_s)
{
    assert(section != nullptr);

    DEBUG();
    
    const uint8_t op_code_1 = 0x0f;
    const uint8_t op_code_2 = 0xaf;

    uint8_t rex_byte = MAGIC_NUM_REX | W_REX;

    if (reg_s & REG_EXTENDED) rex_byte |= B_REX;
    if (reg_d & REG_EXTENDED) rex_byte |= R_REX;
    EMIT(rex_byte);
    EMIT(op_code_1);
    EMIT(op_code_2);
    
    const uint8_t r_to_r = 0b11000000;
    uint8_t rw_byte = (uint8_t) (r_to_r |((reg_d & ~REG_EXTENDED) << 3) 
                                    | (reg_s & ~REG_EXTENDED));
    EMIT(rw_byte);
}

void 
emit_sub_rsp_const(section_t section, 
                   int32_t   num)
{
    assert(section != nullptr);
    
    DEBUG();

    const uint8_t rex_byte = 0x48;
    const uint8_t op_code = 0x81;
    const uint8_t sib = 0xec;

    EMIT(rex_byte);
    EMIT(op_code);
    EMIT(sib);
    EMIT_D((uint32_t) num);
}

void 
emit_idiv(section_t section,
          reg_e     reg)
{
    assert(section != nullptr);

    DEBUG();
    const uint8_t op_code_1 = 0xf7;
    const uint8_t op_code_2 = 0xf8;
    uint8_t rex_byte = W_REX | MAGIC_NUM_REX;

    if (reg & REG_EXTENDED) rex_byte |= B_REX;

    EMIT(rex_byte);
    EMIT(op_code_1);
    EMIT(op_code_2 | (~REG_EXTENDED & reg));
}

void 
emit_cqo(section_t section)
{
    assert(section != nullptr);

    const uint8_t op_code_1 = 0x48;
    const uint8_t op_code_2 = 0x99;
    
    EMIT(op_code_1);
    EMIT(op_code_2);
}

void 
emit_syscall(section_t section)
{
    assert(section != nullptr);

    DEBUG();
    const uint8_t op_code_1 = 0x0f;
    const uint8_t op_code_2 = 0x05;

    EMIT(op_code_1);
    EMIT(op_code_2);
}

// ----------------------------------------------------------------------------

void 
emit_jmp(section_t section,
         uint32_t  addr)
{
    assert(section != nullptr);

    DEBUG();

    const uint8_t long_op_code = 0xe9;
    int32_t long_rel_address = (int32_t) (addr) 
                        - (int32_t) (section->cur_pos + 5);  
    EMIT(long_op_code);
    EMIT_D((int32_t) long_rel_address);
}

void 
emit_jz(section_t section,
        uint32_t  addr)
{
    assert(section != nullptr);

    DEBUG();

    const uint8_t long_op_code_1 = 0x0f;
    const uint8_t long_op_code_2 = 0x84;
    
    int32_t long_rel_address = (int32_t) (addr) 
                        - (int32_t) (section->cur_pos + 6);  

    EMIT(long_op_code_1);
    EMIT(long_op_code_2);
    
    EMIT_D((int32_t) long_rel_address);
}