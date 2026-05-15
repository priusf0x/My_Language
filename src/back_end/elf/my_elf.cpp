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
    "\x01\x00"                             /*amount of program headers*/
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

    const string_s elf_header ={(char*) ELF_FILE_HEADER, ELF_FILE_H_SIZE};
    return SectionInsertString(segment, elf_header);
}


segment_return_e
SegmentCreateProgramH(segment_t segment)
{
    assert(segment != nullptr);
    
    segment->program_header = segment->cur_pos;
    const string_s elf_header ={(char*) ELF_PROGRAM_HEADER, ELF_PROGRAM_H_SIZE};
    return SectionInsertString(segment, elf_header);
}

static segment_return_e 
SegmentSetSegmentSize(segment_t segment,
                      size_t    size)
{
    assert(segment != nullptr);

    const size_t segment_size_pos = 32;
    const size_t segment_mem_size_pos = 40;

    memcpy(segment->segment + segment->program_header + segment_size_pos, 
                &size, sizeof(size_t));
    memcpy(segment->segment + segment->program_header + segment_mem_size_pos, 
                &size, sizeof(size_t));

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