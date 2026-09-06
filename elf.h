#ifndef ELF_H_
#define ELF_H_


#include <stdint.h>
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Execution view
 * |    ELF HEADER    |
 * |------------------|
 * |  Program hdr tbl |
 * |------------------|
 * |    Segment  1    |
 * |    Segment  2    |
 * |    ..........    |
 * |------------------|
 * |  Section hdr tbl | Optional
 */
#define EI_NIDENT (16)
#define EI_NSTRUCT (13)

#define SH_NSTRUCT (10)
#define EI_CLASS (4)
#define ELFCLASS32 (1)
#define EV_CURRENT (1)
#define ET_EXEC (2) // executable
#define EM_RISCV (243) // riscv
#define EM_X86_64 (62)

typedef uint16_t Elf32_Half;
typedef uint32_t Elf32_Word;
typedef uint32_t Elf32_Addr;
typedef uint32_t Elf32_Off;
typedef int32_t  Elf32_Sword;

typedef struct {
    unsigned char e_ident[EI_NIDENT];
    Elf32_Half e_type;
    Elf32_Half e_machine;
    Elf32_Word e_version;
    Elf32_Addr e_entry;
    Elf32_Off  e_phoff;
    Elf32_Off  e_shoff;
    Elf32_Word e_flags;
    Elf32_Half e_ehsize;
    Elf32_Half e_phentsize;
    Elf32_Half e_phnum;
    Elf32_Half e_shentsize;
    Elf32_Half e_shnum;
    Elf32_Half e_shstrndx;
} Elf32_Ehdr;

/* found by ehdr.e_shoff
 * from start of file
 * | Section tabl hdr |
 * |------------------|
 * |     Section 0    |
 * |     Section 1    |
 * |     Section 2    |
 * |    ..........    |
 * |------------------|
 */
typedef struct {
    Elf32_Word sh_name;
    Elf32_Word sh_type;
    Elf32_Word sh_flags;
    Elf32_Addr sh_addr;
    Elf32_Off sh_offset;
    Elf32_Word sh_size;
    Elf32_Word sh_link;
    Elf32_Word sh_info;
    Elf32_Word sh_addralign;
    Elf32_Word sh_entsize;
} Elf32_Shdr;

// section header types
#define SHT_NULL 0
#define SHT_PROGBITS 1
#define SHT_SYMTAB 2
#define SHT_STRTAB 3
#define SHT_RELA 4
#define SHT_HASH 5
#define SHT_DYNAMIC 6
#define SHT_NOTE 7
#define SHT_NOBITS 8
#define SHT_REL 9
#define SHT_SHLIB 10
#define SHT_DYNSYM 11
#define SHT_LOPROC 0x70000000
#define SHT_HIPROC 0x7fffffff
#define SHT_LOUSER 0x80000000
#define SHT_HIUSER 0xffffffff

// section attribute flags
#define SHF_WRITE 0x1
#define SHF_ALLOC 0x2
#define SHF_EXECINSTR 0x4
#define SHF_MASKPROC 0xf0000000

typedef struct {
    Elf32_Word p_type;
    Elf32_Off  p_offset;
    Elf32_Addr p_vaddr;
    Elf32_Addr p_paddr;
    Elf32_Word p_filesz;
    Elf32_Word p_memsz;
    Elf32_Word p_flags;
    Elf32_Word p_align;
} Elf32_Phdr;

#define PT_LOAD 1

typedef struct {
    Elf32_Shdr* items;
    uint64_t len;
    char* names;
    uint64_t names_len;
} shdr_tabl;

typedef struct {
    Elf32_Phdr* items;
    uint64_t len;
} phdr_tabl;

typedef struct {
    uint8_t* items;
    uint64_t addr; // addr to load the segment at
    uint64_t offset; //offset into the file
    uint64_t type; // type of the segment
    uint64_t len;
} segment;

typedef struct {
    segment* items;
    uint64_t cap;
    uint64_t len;
} seg_tabl;

typedef struct {
    uint8_t* items;
    uint64_t cap;
    uint64_t len;
} byte_arr;

typedef struct {
    byte_arr* contents; // file contents
    Elf32_Ehdr* hdr; // elf header
    shdr_tabl* shdr_table; // section header table
    phdr_tabl* phdr_table; // program header table
    seg_tabl* seg_table; // segments table
} elf_file;

elf_file* read_elf(char* path);
void free_elf(elf_file* f);

#ifdef ELF_IMPLEMENTATION

byte_arr* init_arr(uint64_t len);
byte_arr* read_entire_file(char* path);
Elf32_Ehdr* read_hdr(const byte_arr* arr);
Elf32_Shdr read_shdr(const byte_arr* arr, uint64_t offset);
shdr_tabl* read_shdr_tabl(Elf32_Ehdr* hdr, const byte_arr* arr);
Elf32_Phdr read_phdr(const byte_arr* arr, uint64_t offset);
phdr_tabl* read_ph_tabl(Elf32_Ehdr* hdr, const byte_arr* arr);
segment init_seg(uint64_t len, uint64_t offset, uint64_t addr, uint64_t type);
seg_tabl* read_seg_tabl(phdr_tabl* tabl, const byte_arr* arr);
void free_arr(byte_arr* arr);
void free_hdr(Elf32_Ehdr* hdr);
void free_shdr_tabl(shdr_tabl* tabl);
void free_phdr_tabl(phdr_tabl* tabl);
void free_seg(segment* seg);
void free_seg_tabl(seg_tabl* tabl);

elf_file* read_elf(char* path)
{
    byte_arr* arr = read_entire_file(path);
    if (arr == NULL) {
        return NULL;
    }
    Elf32_Ehdr* hdr = read_hdr(arr);
    if (strncmp((const char*)hdr->e_ident, "\x7f" "ELF", EI_NIDENT) == 0) {
        fprintf(stderr, "[ERROR] Magic numbers do not match up...\n");
        goto fail;
    }
    if (hdr->e_ident[EI_CLASS] != ELFCLASS32) {
        fprintf(stderr, "[ERROR] only 32 bit executables are supported...\n");
        goto fail;
    }
    if (hdr->e_type != ET_EXEC) {
        fprintf(stderr, "[ERROR] not an executable...\n");
        goto fail;
    }
    if (hdr->e_version != EV_CURRENT) {
        fprintf(stderr, "[ERROR] incorrect elf version...\n");
        goto fail;
    }
    shdr_tabl* sh_tabl = read_shdr_tabl(hdr, arr);
    phdr_tabl* ph_tabl = read_ph_tabl(hdr, arr);
    seg_tabl* seg_tabl = read_seg_tabl(ph_tabl, arr);

    elf_file* f = calloc(1, sizeof(elf_file));
    f->contents = arr;
    f->hdr = hdr;
    f->shdr_table = sh_tabl;
    f->phdr_table = ph_tabl;
    f->seg_table = seg_tabl;
    return f;

fail:
    free_hdr(hdr);
    free_arr(arr);
    return NULL;
}

void free_elf(elf_file* f)
{
    free_arr(f->contents);
    free_hdr(f->hdr);
    free_shdr_tabl(f->shdr_table);
    free_phdr_tabl(f->phdr_table);
    free_seg_tabl(f->seg_table);
    free(f);
}
/* byte array*/
byte_arr* init_arr(uint64_t len)
{
    byte_arr* arr = calloc(1, sizeof(byte_arr));
    arr->items = calloc(len, sizeof(uint8_t));
    arr->len = len;
    return arr;
}

byte_arr* read_entire_file(char* path)
{
    FILE* f = fopen(path, "rb");
    if (f == NULL) {
        fprintf(stderr, "[ERROR] File not found...\n");
        return NULL;
    }
    fseek(f, 0, SEEK_END);
    int len  = ftell(f);
    fseek(f, 0, SEEK_SET);
    byte_arr* arr = init_arr(len);
    fread(arr->items, sizeof(uint8_t), arr->len, f);
    fclose(f);
    return arr;
}

/* elf header*/
Elf32_Ehdr* read_hdr(const byte_arr* arr)
{
    Elf32_Ehdr* hdr = calloc(1, sizeof(Elf32_Ehdr));
    memcpy(hdr, arr->items, sizeof(Elf32_Ehdr));
    return hdr;
}

/* elf section table*/
Elf32_Shdr read_shdr(const byte_arr* arr, uint64_t offset)
{
    Elf32_Shdr shdr = {0};
    memcpy(&shdr, arr->items + offset, sizeof(Elf32_Shdr));
    return shdr;
}

shdr_tabl* read_shdr_tabl(Elf32_Ehdr* hdr, const byte_arr* arr)
{
    shdr_tabl* tabl = calloc(1, sizeof(shdr_tabl));
    tabl->items = calloc(hdr->e_shnum, sizeof(Elf32_Shdr));
    tabl->len = hdr->e_shnum;

    // every section header is stored sequentially in the table,
    // first entry always null and zeroed out
    // seek until start of section header table
    for (size_t i = 0; i < tabl->len; i++) {
        tabl->items[i] = read_shdr(arr, hdr->e_shoff + (sizeof(Elf32_Shdr) * i));
    }
    // copy over the names
    Elf32_Word buffer_size = tabl->items[hdr->e_shstrndx].sh_size;
    tabl->names = calloc(1, buffer_size + 1);
    tabl->names_len = buffer_size + 1;
    // hdr->e_shstrndx is the section that holds the info for string table
    size_t offset = tabl->items[hdr->e_shstrndx].sh_offset; 
    memcpy(tabl->names, arr->items + offset, buffer_size);
    
    return tabl;
}

/* elf program header*/
Elf32_Phdr read_phdr(const byte_arr* arr, uint64_t offset)
{
    Elf32_Phdr phdr = {0};
    memcpy(&phdr, arr->items + offset, sizeof(Elf32_Phdr));
    return phdr;
}

phdr_tabl* read_ph_tabl(Elf32_Ehdr* hdr, const byte_arr* arr)
{
    phdr_tabl* tabl = (phdr_tabl*)calloc(1, sizeof(phdr_tabl));
    tabl->items = (Elf32_Phdr*)calloc(hdr->e_phnum, sizeof(Elf32_Phdr));
    tabl->len = hdr->e_phnum;
    uint64_t phdr_offset = hdr->e_phoff;
    for (size_t i = 0; i < tabl->len; i++) {
        tabl->items[i] = read_phdr(arr, phdr_offset + (sizeof(Elf32_Phdr) * i));
    }
    return tabl;
}

/* segment table*/
segment init_seg(uint64_t len, uint64_t offset, uint64_t addr, uint64_t type)
{
    segment seg = {0};
    // segment* seg = calloc(1, sizeof(segment));
    seg.len = len;
    seg.addr = addr;
    seg.offset = offset;
    seg.type = type;
    seg.items = calloc(len, sizeof(uint8_t));
    return seg;
}

seg_tabl* read_seg_tabl(phdr_tabl* tabl, const byte_arr* arr)
{
    seg_tabl* seg_table = calloc(1, sizeof(seg_tabl));
    seg_table->cap = 0;
    seg_table->len = tabl->len;
    seg_table->items = calloc(seg_table->len, sizeof(segment));
    for (uint64_t i = 0; i < seg_table->len; i++) {
        // if (tabl->items[i].p_type != PT_LOAD) continue;
        uint64_t addr   = tabl->items[i].p_paddr;
        uint64_t offset = tabl->items[i].p_offset;
        uint64_t type   = tabl->items[i].p_type;
        segment seg    = init_seg(tabl->items[i].p_filesz, offset, addr, type);
        for (uint64_t j = 0; j < seg.len; j++) {
            uint8_t byte = arr->items[offset + j];
            seg.items[j] = byte;
        }
        seg_table->items[(seg_table->cap)++] = seg;
    }
    return seg_table;
}

/* free structs*/
void free_arr(byte_arr* arr)
{
    free(arr->items);
    free(arr);
}


void free_hdr(Elf32_Ehdr* hdr)
{
    free(hdr);
}

void free_shdr_tabl(shdr_tabl* tabl)
{
    free(tabl->items);
    free(tabl->names);
    free(tabl);
}

void free_phdr_tabl(phdr_tabl* tabl)
{
    free(tabl->items);
    free(tabl);
}

void free_seg(segment* seg)
{
    free(seg->items);
}

void free_seg_tabl(seg_tabl* tabl)
{
    for (uint64_t i = 0; i < tabl->len; i++) {
        free_seg(&tabl->items[i]);
    }
    free(tabl->items);
    free(tabl);
}

#endif // ELF_IMPLEMENTATION
#endif // ELF_H_
