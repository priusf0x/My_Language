#ifndef ELF_H
#define ELF_H

#include <stdint.h>
#include <stdlib.h>

enum section_return_e
{
    SECTION_SUCCESS,
    SECTION_BAD_ALLOC,
    SECTION_COPY_ERR
};

struct section_s 
{
    uint8_t* section;
    size_t   cur_pos;
    size_t   max_size;
};
typedef section_s* section_t;

section_return_e
SectionCtor(section_t* section,
            size_t     start_size);

section_return_e 
SectionDtor(section_t section);

section_return_e
SectionEmitByte(section_t section,
                uint8_t   byte);

section_return_e
SectionEmitQword(section_t section,
                 uint64_t  qword);

section_return_e
SectionEmitDword(section_t section,
                 uint32_t  dword);

section_return_e
SectionCreateFileH(section_t section);

#endif // ELF_H