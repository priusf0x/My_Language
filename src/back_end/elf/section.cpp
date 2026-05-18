#include "hashtable.h"
#include "my_elf.h"

#include <assert.h>
#include <cstdint>
#include <elf.h>
#include <fstream>
#include <string.h>

#include "tools.h"

// --------------------------------- helpers ----------------------------------

static elf_return_e
SetSectionSize(section_t section,
               size_t    new_size)
{
    assert(section != nullptr);

    uint8_t* new_ptr = (uint8_t*) recalloc(section->section, 
                                    section->max_size, new_size);

    if (new_ptr == nullptr)
    {
        return ELF_BAD_ALLOC;
    }
    
    section->section = new_ptr;
    section->max_size = new_size;

   return ELF_SUCCESS; 
}

elf_return_e
SectionCtor(section_t* section,
            size_t     start_size)
{
    assert(section != nullptr);

    elf_return_e output = ELF_SUCCESS;

    (*section) = (section_t) calloc(1, sizeof(section_s));
    if (*section == nullptr)
    {
       return ELF_BAD_ALLOC; 
    }

    output = SetSectionSize(*section, start_size);
    if (output != ELF_SUCCESS)
    {
        return output;
    }
   
    return ELF_SUCCESS;
}

elf_return_e 
SectionDtor(section_t section)
{
    if (section != nullptr)
    {
        free(section->section);
        free(section);
    }
    
    return ELF_SUCCESS;
}

elf_return_e
SectionEmitByte(section_t section,
                uint8_t   byte)
{
    assert(section != nullptr);
    
    elf_return_e output = ELF_SUCCESS;
    if (section->cur_pos + 1 >= section->max_size)
    {
        output = SetSectionSize(section, 2 * section->max_size);
        if (output != ELF_SUCCESS)
        {
            return output;
        }
    }

    *(section->section + section->cur_pos) = byte;
    section->cur_pos++;

    return ELF_SUCCESS;
}

elf_return_e
SectionEmitDword(section_t section,
                 uint32_t  dword)
{
    assert(section != nullptr);
    
    elf_return_e output = ELF_SUCCESS;
    if (section->cur_pos + 4 >= section->max_size)
    {
        output = SetSectionSize(section, 2 * section->max_size);
        if (output != ELF_SUCCESS)
        {
            return output;
        }
    }
    
    memcpy(section->section + section->cur_pos, &dword, sizeof(int32_t));
    section->cur_pos += sizeof(int32_t);
    
    return ELF_SUCCESS;
}


elf_return_e
SectionEmitQword(section_t section,
                 uint64_t  qword)
{
    assert(section != nullptr);
    
    elf_return_e output = ELF_SUCCESS;
    if (section->cur_pos + 8 >= section->max_size)
    {
        output = SetSectionSize(section, 2 * section->max_size);
        if (output != ELF_SUCCESS)
        {
            return output;
        }
    }
    
    memcpy(section->section + section->cur_pos, &qword, sizeof(int64_t));
    section->cur_pos += sizeof(int64_t);

    return ELF_SUCCESS;
}

elf_return_e 
SectionInsertString(section_t section,
                    string_s  string)
{
    assert(section != nullptr);
    assert(string.string != nullptr);

    elf_return_e output = ELF_SUCCESS;
    if ((section->cur_pos + string.size) >= section->max_size)
    {
        output = SetSectionSize(section, 2 * (section->max_size + string.size));
        if (output != ELF_SUCCESS)
        {
            return output;
        }
    }

    if (memcpy(section->section + section->cur_pos, string.string,
                string.size) == nullptr)
    {
        return ELF_COPY_ERR;
    }
    section->cur_pos += string.size;

   return ELF_SUCCESS; 
}

// ------------------------------- code_section -------------------------------

// nothing to do lol 

// ---------------------------------- relatab ---------------------------------

elf_return_e 
RelatabAddFunc(elf_t     elf, 
               string_s  string,
               size_t    offset)
{
    assert(elf != nullptr);

    Elf64_Rela rel_el = {};

    const int32_t rel_addr_addend = 4;

    uint64_t sym_tab_index = HashTableGetElem(elf->str_hash, string);
    if (sym_tab_index == ~ (uint64_t) 0)
    {
        SymtabAddGlobFunc(elf, string, 0, 0, false); 
        sym_tab_index = HashTableGetElem(elf->str_hash, string);
    }
        
    rel_el.r_offset = offset + rel_addr_addend; 
    rel_el.r_info   = ELF64_R_INFO(sym_tab_index, R_X86_64_PC32);
    rel_el.r_addend = -rel_addr_addend;
    
    SectionInsertString(elf->rela_text, {(char*) &rel_el, sizeof(rel_el)});

    return ELF_SUCCESS;
}

// ---------------------------------- symtab ----------------------------------

elf_return_e
SymtabAddGlobFunc(elf_t    elf, 
                  string_s string, 
                  size_t   offset, 
                  size_t   symb_size,
                  bool     is_defined)
{
    assert(elf != nullptr);

    Elf64_Sym symbol = {};

    AddStrToStrtab(elf, string, &symbol.st_name);
    HashTableAddElem(elf->str_hash, string, elf->symtab_size);
    elf->symtab_size++;

    symbol.st_info  = ELF64_ST_INFO(STB_GLOBAL, STT_FUNC);
    symbol.st_other = 0x00;
    symbol.st_shndx = is_defined? TEXT_SECTION_NUMBER : 0;
    symbol.st_value = offset;
    symbol.st_size  = symb_size;
    
    SectionInsertString(elf->symtab, {(char*) &symbol, sizeof(symbol)});
    
    return ELF_SUCCESS;
}

// ---------------------------------- strtab ----------------------------------

elf_return_e
AddStrToStrtab(elf_t       elf,
               string_s    string,
               Elf64_Word* index)
{
    assert(elf != nullptr);
    assert(index != nullptr);

    *index = (uint32_t) elf->strtab->cur_pos;

    SectionInsertString(elf->strtab, string);
    SectionEmitByte(elf->strtab, 0x00);
    
    return ELF_SUCCESS;
}

// --------------------------------- shstrtab ---------------------------------

elf_return_e
AddStrToShstrtab(elf_t       elf,
                 string_s    string,
                 Elf64_Word* index)
{
    assert(elf != nullptr);
    assert(index != nullptr);

    *index = (uint32_t) elf->shstrtab->cur_pos;

    SectionInsertString(elf->shstrtab, string);
    SectionEmitByte(elf->shstrtab, 0x00);
    
    return ELF_SUCCESS;
}