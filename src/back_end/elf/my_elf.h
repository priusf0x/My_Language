#ifndef ELF_H
#define ELF_H

#include <stdint.h>
#include <stdlib.h>

#include "compiler.h"

struct section_s 
{
    uint8_t* section;
    size_t   cur_pos;
    size_t   max_size;
};
typedef section_s* section_t;

compiler_return_e
SectionCtor(section_t* section,
            size_t     start_size);

compiler_return_e 
SectionDtor(section_t section);

compiler_return_e
SectionEmitByte(section_t section,
                uint8_t   byte);

compiler_return_e
SectionEmitQword(section_t section,
                 uint64_t  qword);

compiler_return_e
SectionEmitDword(section_t section,
                 uint32_t  dword);

compiler_return_e
SectionCreateFileH(section_t section);

#endif // ELF_H