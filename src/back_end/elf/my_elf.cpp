#include "my_elf.h"

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <elf.h>
#include <stdlib.h>
#include <assert.h>
#include <elf.h>

#include "compiler.h"
#include "hash.h"
#include "tools.h"

const size_t OFFSET_AFTER_EHDR = sizeof(Elf64_Ehdr);
const size_t OFFSET_AFTER_SHDR = sizeof(Elf64_Ehdr) 
                    + SECTION_AMOUNT * sizeof(Elf64_Shdr);

const Elf64_Ehdr EHDR_REFERENCE =
{                 //       E     L     F
    .e_ident     = {0x7f, 0x45, 0x4c, 0x46, 0x02, 0x01, 0x01},
    .e_type      = ET_REL,
    .e_machine   = EM_X86_64,
    .e_version   = 0x01,
    .e_entry     = 0x00,
    .e_phoff     = 0x00,
    .e_shoff     = OFFSET_AFTER_EHDR,
    .e_flags     = 0x00,
    .e_ehsize    = 0x40,
    .e_phentsize = 0x38,
    .e_phnum     = 0x00,
    .e_shentsize = 0x40,
    .e_shnum     = 0x06,
    .e_shstrndx  = 0x05
};

// ================================== ELF =====================================

elf_return_e 
ElfCtor(elf_t*    elf,
        section_t text)
{
    assert(elf != nullptr);
    assert(text != nullptr);

    elf_return_e output = ELF_SUCCESS;
    const size_t start_size = 10; 
    const size_t ht_size = 10;
    Elf64_Sym null_sym = {};

    *elf = (elf_t) calloc(1, sizeof(elf_s)); 

    if (*elf == nullptr)
    {
        return ELF_BAD_ALLOC;
    }
    
    (*elf)->stab = (stab_t) calloc(1, sizeof(stab_s));
    if ((*elf)->stab == nullptr)
    {
        output = ELF_BAD_ALLOC;
        goto err;
    }

    if ((output = SectionCtor(&(*elf)->file, start_size))) goto err;
    (*elf)->text = text;
    if ((output = SectionCtor(&(*elf)->rela_text, start_size))) goto err;
    if ((output = SectionCtor(&(*elf)->symtab, start_size))) goto err;
    if ((output = SectionCtor(&(*elf)->strtab, start_size))) goto err;
    if ((output = SectionCtor(&(*elf)->shstrtab, start_size))) goto err;
    if (HashTableCtor(&(*elf)->str_hash, ht_size, HashCRC32)) 
    {
        output = ELF_HASHTABLE_ERR;
        goto err;
    }
    
    // add null byte 
    
    (*elf)->symtab_size++;
    SectionEmitByte((*elf)->shstrtab, 0x00);
    SectionEmitByte((*elf)->strtab, 0x00);
    
    SectionInsertString((*elf)->symtab, {(char*) &null_sym, sizeof(null_sym)});
    
    SymtabAddText(*elf);

    return ELF_SUCCESS;

    err:
    free((*elf)->stab);
    SectionDtor((*elf)->file);
    SectionDtor((*elf)->rela_text);
    SectionDtor((*elf)->symtab);
    SectionDtor((*elf)->strtab);
    SectionDtor((*elf)->shstrtab);
    free(*elf);
    
    return output;
}

elf_return_e
ElfDtor(elf_t elf)
{
    if (elf != nullptr)
    {
        free(elf->stab);
        SectionDtor(elf->file);
        SectionDtor(elf->rela_text);
        SectionDtor(elf->symtab);
        SectionDtor(elf->strtab);
        SectionDtor(elf->shstrtab);
        HashTableDtor(elf->str_hash);
        free(elf);
    }
    
    return ELF_SUCCESS;
}

static elf_return_e
ElfCreateEhdr(section_t section)
{
    assert(section != nullptr);

    const string_s elf_header ={(char*) &EHDR_REFERENCE, sizeof(EHDR_REFERENCE)};
    return SectionInsertString(section, elf_header);
}

    
static elf_return_e 
ElfCreateTable(elf_t elf)
{
    assert(elf != nullptr);

// ------------------------------- text_section -------------------------------
    
    const string_s text_name = {".text", 5};
    Elf64_Word text_index = 0;
    AddStrToShstrtab(elf, text_name, &text_index);

    elf->stab->text.sh_name = text_index; 
    elf->stab->text.sh_type = SHT_PROGBITS;
    elf->stab->text.sh_flags = SHF_ALLOC | SHF_EXECINSTR;
    elf->stab->text.sh_size = elf->text->cur_pos;

// ------------------------------- rela_section -------------------------------

    const string_s rela_name = {".rela.text", 10};
    Elf64_Word rela_index = 0;
    AddStrToShstrtab(elf, rela_name, &rela_index);

    elf->stab->rela_text.sh_name = rela_index;
    elf->stab->rela_text.sh_type = SHT_RELA;
    elf->stab->rela_text.sh_link = SYMTAB_SECTION_NUMBER;
    elf->stab->rela_text.sh_info = TEXT_SECTION_NUMBER;
    elf->stab->rela_text.sh_size = elf->rela_text->cur_pos;
    elf->stab->rela_text.sh_entsize = sizeof(Elf64_Rela);

// ---------------------------------- symtab ----------------------------------

    const string_s symtab_name = {".symtab", 7};
    Elf64_Word symtab_index = 0;
    AddStrToShstrtab(elf, symtab_name, &symtab_index);
    
    elf->stab->symtab.sh_name = symtab_index;
    elf->stab->symtab.sh_type = SHT_SYMTAB;
    elf->stab->symtab.sh_link = STRTAB_SECTION_NUMBER;
    elf->stab->symtab.sh_info = RELA_SECTION_NUMBER;
    elf->stab->symtab.sh_size = elf->symtab->cur_pos;
    elf->stab->symtab.sh_entsize = sizeof(Elf64_Sym);

// ---------------------------------- strtab ----------------------------------

    const string_s strtab_name = {".strtab", 7};
    Elf64_Word strtab_index = 0;
    AddStrToShstrtab(elf, strtab_name, &strtab_index);

    elf->stab->strtab.sh_name = strtab_index;
    elf->stab->strtab.sh_type = SHT_STRTAB;
    elf->stab->strtab.sh_size = elf->strtab->cur_pos;

// ---------------------------------- shstrtab --------------------------------
    
    const string_s shstrtab_name = {".shstrtab", 9};
    Elf64_Word shstrtab_index = 0;
    AddStrToShstrtab(elf, shstrtab_name, &shstrtab_index);

    elf->stab->shstrtab.sh_name = shstrtab_index;
    elf->stab->shstrtab.sh_type = SHT_STRTAB;
    elf->stab->shstrtab.sh_size = elf->shstrtab->cur_pos;           

    return ELF_SUCCESS;
}

elf_return_e
ElfCreateLinkable(elf_t elf)
{
    assert(elf != nullptr);

    ElfCreateEhdr(elf->file);
    ElfCreateTable(elf);
    
    elf->file->cur_pos = OFFSET_AFTER_SHDR;

    elf->stab->text.sh_offset = elf->file->cur_pos;
    SectionInsertString(elf->file, {(char*) elf->text->section, 
                        elf->text->cur_pos});

    elf->stab->rela_text.sh_offset = elf->file->cur_pos;
    SectionInsertString(elf->file, {(char*) elf->rela_text->section, 
                        elf->rela_text->cur_pos});
    
    elf->stab->symtab.sh_offset = elf->file->cur_pos;
    SectionInsertString(elf->file, {(char*) elf->symtab->section, 
                        elf->symtab->cur_pos});
                        
    elf->stab->strtab.sh_offset = elf->file->cur_pos;
    SectionInsertString(elf->file, {(char*) elf->strtab->section, 
                        elf->strtab->cur_pos});
    
    elf->stab->shstrtab.sh_offset = elf->file->cur_pos;
    SectionInsertString(elf->file, {(char*) elf->shstrtab->section, 
                        elf->shstrtab->cur_pos});
    
    size_t file_size = elf->file->cur_pos;

    elf->file->cur_pos = OFFSET_AFTER_EHDR;

    SectionInsertString(elf->file, {(char*) elf->stab, 
                        sizeof(*elf->stab)});
    
    elf->file->cur_pos = file_size;
    
    return ELF_SUCCESS;
}

elf_return_e 
ElfWriteInFile(elf_t       elf,
               const char* file_name)
{
    assert(elf != nullptr);
    assert(file_name != nullptr);

    FILE* file_out = fopen(file_name, "w+");
    if (file_out == nullptr)
    {
        return ELF_FILE_OPEN_ERR;
    }

    fwrite(elf->file->section, 1, elf->file->cur_pos, file_out);

    if (fclose(file_out))
    {
        return ELF_FILE_CLOSE_ERR;
    }
    
    return ELF_SUCCESS;
}
