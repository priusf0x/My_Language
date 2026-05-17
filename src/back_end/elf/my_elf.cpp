#include "my_elf.h"

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <elf.h>
#include <stdlib.h>
#include <assert.h>
#include <elf.h>

#include "compiler.h"
#include "tools.h"

const Elf64_Addr VIRTUAL_START = 0x400000;
const Elf64_Addr ENTRY_POINT = 0x400078;

const Elf64_Ehdr EHDR_REFERENCE =
{                 // .     E     L     F
    .e_ident     = {0x7f, 0x45, 0x4c, 0x46, 0x02, 0x01, 0x01},
    .e_type      = ET_EXEC,
    .e_machine   = EM_X86_64,
    .e_version   = 0x01,
    .e_entry     = ENTRY_POINT,
    .e_phoff     = 0x40,
    .e_shoff     = 0x00,
    .e_flags     = 0x00,
    .e_ehsize    = 0x40,
    .e_phentsize = 0x38,
    .e_phnum     = 0x01,
    .e_shentsize = 0x40,
    .e_shnum     = 0x00,
    .e_shstrndx  = 0x00  
};

const Elf64_Phdr PHDR_REFERENCE = 
{
    .p_type = PT_LOAD,
    .p_flags = PF_X | PF_R | PF_W,
    .p_offset = 0x00,
    .p_vaddr = VIRTUAL_START,
    .p_paddr = VIRTUAL_START,
    .p_filesz = 0x00,
    .p_memsz = 0x00,
    .p_align = 0x00
};

// ============================== SECTION_METHODS =============================

static segment_return_e
SetSectionSize(segment_t segment,
               size_t    new_size)
{
    assert(segment != nullptr);

    uint8_t* new_ptr = (uint8_t*) recalloc(segment->segment, 
                                    segment->max_size, new_size);

    if (new_ptr == nullptr)
    {
        return SEGMENT_BAD_ALLOC;
    }
    
    segment->segment = new_ptr;
    segment->max_size = new_size;

   return SEGMENT_SUCCESS; 
}


segment_return_e
SegmentCtor(segment_t* segment,
            size_t     start_size)
{
    assert(segment != nullptr);

    segment_return_e output = SEGMENT_SUCCESS;

    (*segment) = (segment_t) calloc(1, sizeof(section_s));
    if (*segment == nullptr)
    {
       return SEGMENT_BAD_ALLOC; 
    }

    output = SetSectionSize(*segment, start_size);
    if (output != SEGMENT_SUCCESS)
    {
        return output;
    }
   
    return SEGMENT_SUCCESS;
}

segment_return_e 
SegmentDtor(segment_t segment)
{
    if (segment != nullptr)
    {
        free(segment->segment);
        free(segment);
    }
    
    return SEGMENT_SUCCESS;
}

segment_return_e
SegmentEmitByte(segment_t segment,
                uint8_t   byte)
{
    assert(segment != nullptr);
    
    segment_return_e output = SEGMENT_SUCCESS;
    if (segment->cur_pos + 1 >= segment->max_size)
    {
        output = SetSectionSize(segment, 2 * segment->max_size);
        if (output != SEGMENT_SUCCESS)
        {
            return output;
        }
    }

    *(segment->segment + segment->cur_pos) = byte;
    segment->cur_pos++;

    return SEGMENT_SUCCESS;
}

segment_return_e
SegmentEmitDword(segment_t segment,
                 uint32_t  dword)
{
    assert(segment != nullptr);
    
    segment_return_e output = SEGMENT_SUCCESS;
    if (segment->cur_pos + 4 >= segment->max_size)
    {
        output = SetSectionSize(segment, 2 * segment->max_size);
        if (output != SEGMENT_SUCCESS)
        {
            return output;
        }
    }
    
    memcpy(segment->segment + segment->cur_pos, &dword, sizeof(int32_t));
    segment->cur_pos += sizeof(int32_t);
    
    return SEGMENT_SUCCESS;
}


segment_return_e
SegmentEmitQword(segment_t segment,
                 uint64_t  qword)
{
    assert(segment != nullptr);
    
    segment_return_e output = SEGMENT_SUCCESS;
    if (segment->cur_pos + 8 >= segment->max_size)
    {
        output = SetSectionSize(segment, 2 * segment->max_size);
        if (output != SEGMENT_SUCCESS)
        {
            return output;
        }
    }
    
    memcpy(segment->segment + segment->cur_pos, &qword, sizeof(int64_t));
    segment->cur_pos += sizeof(int64_t);

    return SEGMENT_SUCCESS;
}

segment_return_e 
SectionInsertString(segment_t segment,
                    string_s  string)
{
    assert(segment != nullptr);
    assert(string.string != nullptr);

    segment_return_e output = SEGMENT_SUCCESS;
    if ((segment->cur_pos + string.size) >= segment->max_size)
    {
        output = SetSectionSize(segment, 2 * (segment->max_size + string.size));
        if (output != SEGMENT_SUCCESS)
        {
            return output;
        }
    }

    if (memcpy(segment->segment + segment->cur_pos, string.string,
                string.size) == nullptr)
    {
        return SEGMENT_COPY_ERR;
    }
    segment->cur_pos += string.size;

   return SEGMENT_SUCCESS; 
}

segment_return_e
SegmentCreateFileH(segment_t segment)
{
    assert(segment != nullptr);

    const string_s elf_header ={(char*) &EHDR_REFERENCE, sizeof(EHDR_REFERENCE)};
    return SectionInsertString(segment, elf_header);
}


segment_return_e
SegmentCreateProgramH(segment_t segment)
{
    assert(segment != nullptr);
    
    segment->program_header = segment->cur_pos;
    const string_s elf_header ={(char*) &PHDR_REFERENCE, sizeof(PHDR_REFERENCE)};
    return SectionInsertString(segment, elf_header);
}

static segment_return_e 
SegmentSetSegmentSize(segment_t segment,
                      size_t    size)
{
    assert(segment != nullptr);

    Elf64_Phdr* phdr = (Elf64_Phdr*) (segment->segment + segment->program_header);
    
    phdr->p_filesz = size;
    phdr->p_memsz = size;

    return SEGMENT_SUCCESS;
}

segment_return_e 
SegmentWriteInFile(segment_t   segment,
                   const char* file_name)
{
    assert(segment != nullptr);

    FILE* file_out = fopen(file_name, "w+");
    if (file_out == nullptr)
    {
        return SEGMENT_FILE_OPEN_ERR;
    }

    SegmentSetSegmentSize(segment, segment->cur_pos);
    fwrite(segment->segment, 1, segment->cur_pos, file_out);

    if (fclose(file_out))
    {
        return SEGMENT_FILE_CLOSE_ERR;
    }
    
    return SEGMENT_SUCCESS;
}