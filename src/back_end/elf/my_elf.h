#ifndef ELF_H
#define ELF_H

#include <cstdio>
#include <stdint.h>
#include <stdlib.h>
#include <elf.h>

#include "my_string.h"
#include "hashtable.h"

const size_t TEXT_SECTION_NUMBER = 1;
const size_t RELA_SECTION_NUMBER = 2;
const size_t SYMTAB_SECTION_NUMBER = 3;
const size_t STRTAB_SECTION_NUMBER = 4;
const size_t SHSTRTAB_SECTION_NUMBER = 5;
const size_t SECTION_AMOUNT = 6;

enum elf_return_e
{
    ELF_SUCCESS,
    ELF_BAD_ALLOC,
    ELF_COPY_ERR,
    ELF_FILE_OPEN_ERR,
    ELF_FILE_CLOSE_ERR,
    ELF_HASHTABLE_ERR
};

struct section_s 
{
    uint8_t* section;
    size_t   cur_pos;
    size_t   max_size;
};
typedef section_s* section_t;
typedef section_s* file_buffer_t;

struct stab_s
{
    Elf64_Shdr null;
    Elf64_Shdr text;
    Elf64_Shdr rela_text;
    Elf64_Shdr symtab;
    Elf64_Shdr strtab;
    Elf64_Shdr shstrtab;
} __attribute__((packed));
typedef stab_s* stab_t;

struct elf_s 
{
    
    file_buffer_t file;
    stab_t        stab;
    section_t     text;
    section_t     rela_text;
    section_t     symtab;
    size_t        symtab_size;
    section_t     strtab;
    section_t     shstrtab;
    hashtable_t   str_hash;
};
typedef elf_s* elf_t;

elf_return_e
SectionCtor(section_t* section,
            size_t     start_size);

elf_return_e 
SectionDtor(section_t section);

elf_return_e
SectionEmitByte(section_t section,
                uint8_t   byte);

elf_return_e
SectionEmitQword(section_t section,
                 uint64_t  qword);

elf_return_e
SectionEmitDword(section_t section,
                 uint32_t  dword);

elf_return_e
SectionCreateFileH(section_t section);

elf_return_e
SectionCreateProgramH(section_t section);

elf_return_e 
SectionInsertString(section_t section,
                    string_s  string);

// --------------------------------- elf_construct ----------------------------

elf_return_e 
ElfCtor(elf_t*    elf,
        section_t text);

elf_return_e
ElfDtor(elf_t elf);


elf_return_e
AddStrToShstrtab(elf_t       elf,
                 string_s    string,
                 Elf64_Word* index);

elf_return_e
AddStrToStrtab(elf_t       elf,
               string_s    string,
               Elf64_Word* index);

elf_return_e
SymtabAddGlobFunc(elf_t    elf, 
                  string_s string, 
                  size_t   offset, 
                  size_t   symb_size,
                  bool     is_defined);
                  
elf_return_e
SymtabAddText(elf_t elf);

elf_return_e 
RelatabAddFunc(elf_t     elf, 
               string_s  string,
               size_t    offset);

elf_return_e
ElfCreateLinkable(elf_t elf);


elf_return_e 
ElfWriteInFile(elf_t       elf,
               const char* file_name);

                  
#endif // ELF_H