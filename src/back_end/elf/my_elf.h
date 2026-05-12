#ifndef ELF_H
#define ELF_H

#include <stdint.h>
#include <stdlib.h>

enum segment_return_e
{
    SEGMENT_SUCCESS,
    SEGMENT_BAD_ALLOC,
    SEGMENT_COPY_ERR,
    SEGMENT_FILE_OPEN_ERR,
    SEGMENT_FILE_CLOSE_ERR
};

struct section_s 
{
    uint8_t* segment;
    size_t   cur_pos;
    size_t   max_size;
    size_t   program_header;
};
typedef section_s* segment_t;

segment_return_e
SegmentCtor(segment_t* segment,
            size_t     start_size);

segment_return_e 
SegmentDtor(segment_t segment);

segment_return_e
SegmentEmitByte(segment_t segment,
                uint8_t   byte);

segment_return_e
SegmentEmitQword(segment_t segment,
                 uint64_t  qword);

segment_return_e
SegmentEmitDword(segment_t segment,
                 uint32_t  dword);

segment_return_e
SegmentCreateFileH(segment_t segment);

segment_return_e
SegmentCreateProgramH(segment_t segment);

segment_return_e 
SegmentWriteInFile(segment_t   segment,
                   const char* file_name);


#endif // ELF_H