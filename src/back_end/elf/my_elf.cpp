#include "my_elf.h"

#include <cstdint>
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
static const size_t ELF_HEADER_SIZE = sizeof(ELF_FILE_HEADER);

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
static const size_t ELF_PROGRAM_SIZE = sizeof(ELF_PROGRAM_HEADER);

// ============================== SECTION_METHODS =============================

static compiler_return_e
SetSectionSize(section_t section,
               size_t    new_size)
{
    assert(section != nullptr);

    uint8_t* new_ptr = (uint8_t*) recalloc(section->section, 
                                    section->max_size, new_size);

    if (new_ptr == nullptr)
    {
        return COMPILER_RETURN_ALLOCATION_ERROR;
    }
    
    section->section = new_ptr;
    section->max_size = new_size;

   return COMPILER_RETURN_ALLOCATION_ERROR; 
}


compiler_return_e
SectionCtor(section_t* section,
            size_t     start_size)
{
    assert(section != nullptr);

    compiler_return_e output = COMPILER_RETURN_SUCCESS;

    (*section) = (section_t) calloc(1, sizeof(section_s));
    if (*section == nullptr)
    {
       return COMPILER_RETURN_ALLOCATION_ERROR; 
    }

    output = SetSectionSize(*section, start_size);
    if (output != COMPILER_RETURN_SUCCESS)
    {
        return output;
    }
   
    return COMPILER_RETURN_SUCCESS;
}

compiler_return_e 
SectionDtor(section_t section)
{
    if (section != nullptr)
    {
        free(section->section);
        free(section);
    }
    
    return COMPILER_RETURN_SUCCESS;
}

compiler_return_e
SectionEmitByte(section_t section,
                uint8_t   byte)
{
    assert(section != nullptr);
    
    compiler_return_e output = COMPILER_RETURN_SUCCESS;
    if (section->cur_pos + 1 >= section->max_size)
    {
        output = SetSectionSize(section, 2 * section->max_size);
        if (output != COMPILER_RETURN_SUCCESS)
        {
            return output;
        }
    }

    *(section->section + section->cur_pos) = byte;
    section->cur_pos++;

    return COMPILER_RETURN_SUCCESS;
}

compiler_return_e
SectionCreateFileHeader(section_t section)
{
    assert(section != nullptr);
    
    compiler_return_e output = COMPILER_RETURN_SUCCESS;
    if ((section->max_size + ELF_HEADER_SIZE) >= section->max_size)
    {
        output = SetSectionSize(section, 2 * (section->max_size + ELF_HEADER_SIZE));
        if (output != COMPILER_RETURN_SUCCESS)
        {
            return output;
        }
    }

    if (memcpy(section->section + section->cur_pos, ELF_FILE_HEADER,
                ELF_HEADER_SIZE) == nullptr)
    {
        return COMPILER_RETURN_ALLOCATION_ERROR;
    }

    return COMPILER_RETURN_SUCCESS;
}
