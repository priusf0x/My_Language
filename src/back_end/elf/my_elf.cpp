#include "my_elf.h"

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <elf.h>
#include <stdlib.h>
#include <assert.h>

#include "compiler.h"
#include "tools.h"

static const char* const ELF_FILE_HEADER = 
    "\x7f\x45\x4c\x46"                     /*magic numbers*/
    "\x02"                                 /*ELF class (32b or 64b)*/
    "\x01"                                 /*Little- or Big- endian*/
    "\x01"                                 /*ELF version(always 1)*/ 
    "\x00"                                 /*Specific ABI(none)*/ 
    "\x00"                                 /*ABI version*/
    "\x00\x00\x00\x00\x00\x00\x00"         /*Padding*/
    "\x02\x00"                             /*File type*/
    "\x3e\x00"                             /*machine type*/
    "\x01\x00\x00\x00"                     /*format version*/
    "\x78\x00\x40\x00\x00\x00\x00\x00"     /*virtual entry(78 for now)*/
    "\x40\x00\x00\x00\x00\x00\x00\x00"     /*program header offset*/
    "\x00\x00\x00\x00\x00\x00\x00\x00"     /*section header offset*/
    "\x00\x00\x00\x00"                     /*processors flags*/
    "\x40\x00"                             /*file header size*/
    "\x38\x00"                             /*program header size*/
    "\x01\x00"                             /*amount of program header*/
    "\x40\x00"                             /*size of section*/
    "\x00\x00"                             /*amount of sections*/
    "\x00\x00"                             /*index of shrtab*/
    ;
static const size_t ELF_FILE_H_SIZE = 64;

static const char* const ELF_PROGRAM_HEADER = 
    "\x01\x00\x00\x00"                     /*type of segment*/
    "\x07\x00\x00\x00"                     /*access*/
    "\x00\x00\x00\x00\x00\x00\x00\x00"     /*offset*/
    "\x00\x00\x40\x00\x00\x00\x00\x00"     /*load address in vm*/
    "\x00\x00\x40\x00\x00\x00\x00\x00"     /*phys addr*/
    "\x00\x00\x00\x00\x00\x00\x00\x00"     /*segment file size*/
    "\x00\x00\x00\x00\x00\x00\x00\x00"     /*segment mem size*/
    "\x00\x00\x00\x00\x00\x00\x00\x00"     /*align*/
    ;
static const size_t ELF_PROGRAM_H_SIZE = 56;

// ============================== SECTION_METHODS =============================

static section_return_e
SetSectionSize(section_t section,
               size_t    new_size)
{
    assert(section != nullptr);

    uint8_t* new_ptr = (uint8_t*) recalloc(section->section, 
                                    section->max_size, new_size);

    if (new_ptr == nullptr)
    {
        return SECTION_BAD_ALLOC;
    }
    
    section->section = new_ptr;
    section->max_size = new_size;

   return SECTION_SUCCESS; 
}


section_return_e
SectionCtor(section_t* section,
            size_t     start_size)
{
    assert(section != nullptr);

    section_return_e output = SECTION_SUCCESS;

    (*section) = (section_t) calloc(1, sizeof(section_s));
    if (*section == nullptr)
    {
       return SECTION_BAD_ALLOC; 
    }

    output = SetSectionSize(*section, start_size);
    if (output != SECTION_SUCCESS)
    {
        return output;
    }
   
    return SECTION_SUCCESS;
}

section_return_e 
SectionDtor(section_t section)
{
    if (section != nullptr)
    {
        free(section->section);
        free(section);
    }
    
    return SECTION_SUCCESS;
}

section_return_e
SectionEmitByte(section_t section,
                uint8_t   byte)
{
    assert(section != nullptr);
    
    section_return_e output = SECTION_SUCCESS;
    if (section->cur_pos + 1 >= section->max_size)
    {
        output = SetSectionSize(section, 2 * section->max_size);
        if (output != SECTION_SUCCESS)
        {
            return output;
        }
    }

    *(section->section + section->cur_pos) = byte;
    section->cur_pos++;

    return SECTION_SUCCESS;
}

section_return_e
SectionEmitDword(section_t section,
                 uint32_t  dword)
{
    assert(section != nullptr);
    
    section_return_e output = SECTION_SUCCESS;
    if (section->cur_pos + 4 >= section->max_size)
    {
        output = SetSectionSize(section, 2 * section->max_size);
        if (output != SECTION_SUCCESS)
        {
            return output;
        }
    }
    
    memcpy(section->section + section->cur_pos, &dword, sizeof(int32_t));
    section->cur_pos += sizeof(int32_t);
    
    return SECTION_SUCCESS;
}


section_return_e
SectionEmitQword(section_t section,
                 uint64_t  qword)
{
    assert(section != nullptr);
    
    section_return_e output = SECTION_SUCCESS;
    if (section->cur_pos + 8 >= section->max_size)
    {
        output = SetSectionSize(section, 2 * section->max_size);
        if (output != SECTION_SUCCESS)
        {
            return output;
        }
    }
    
    memcpy(section->section + section->cur_pos, &qword, sizeof(int64_t));
    section->cur_pos += sizeof(int64_t);

    return SECTION_SUCCESS;
}

section_return_e
SectionCreateFileH(section_t section)
{
    assert(section != nullptr);
    
    section_return_e output = SECTION_SUCCESS;
    if ((section->cur_pos + ELF_FILE_H_SIZE) >= section->max_size)
    {
        output = SetSectionSize(section, 2 * (section->max_size + ELF_FILE_H_SIZE));
        if (output != SECTION_SUCCESS)
        {
            return output;
        }
    }

    if (memcpy(section->section + section->cur_pos, ELF_FILE_HEADER,
                ELF_FILE_H_SIZE) == nullptr)
    {
        return SECTION_COPY_ERR;
    }
    section->cur_pos += ELF_FILE_H_SIZE;

    return SECTION_SUCCESS;
}