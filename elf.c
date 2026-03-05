#include "elf.h"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    Elf32_Shdr* items;
    size_t len;
} shdr_tabl;


typedef struct {
    char* items;
    size_t len;
} fixed_str;


shdr_tabl* shdr_tabl_init(size_t len)
{
    shdr_tabl* arr = (shdr_tabl*)calloc(1, sizeof(shdr_tabl));
    arr->items = (Elf32_Shdr*)calloc(len, sizeof(Elf32_Shdr));
    arr->len = len;
    return arr;
}


void shdr_tabl_free(shdr_tabl* arr)
{
    assert(arr != NULL && "[ERROR] array is null");
    free(arr);
}


fixed_str* fixed_str_init(size_t len)
{
    fixed_str* str = (fixed_str*)calloc(1, sizeof(*str));
    str->items = (char*)calloc(len, sizeof(char*));
    str->len = len;
    return str;
}

// upto end 
fixed_str* str_slice(const fixed_str* str, size_t start, size_t end)
{
    assert(str != NULL && "[ERROR] str is null");
    size_t slice_len = end - start;
    fixed_str* slice = fixed_str_init(slice_len + 1);
    for (size_t i = 0; i < slice_len; i++) {
        slice->items[i] = str->items[start + i];
    }
    slice->items[slice_len] = 0;
    return slice;
}

size_t str_len(fixed_str* str, size_t start) {
    assert(str != NULL && "[ERROR] array is null");
    size_t len = 0;
    while(str->items[start + len] != 0)
        len++;
    return len;
}

void fixed_str_free(fixed_str* str) {
    assert(str != NULL && "[ERROR] array is null");
    free(str);
}

void dump_hdr(Elf32_Ehdr* hdr)
{
    assert(hdr != NULL && "[ERROR] hdr is null...");
    printf("\n");
    size_t i = 0;
    printf("EI_MAG0     |EI_MAG1     |EI_MAG2     |EI_MAG3     |EI_CLASS    |EI_DATA     |EI_VERSION  |\n");
    printf("------------|------------|------------|------------|------------|------------|------------|\n");
    printf("%-12x|", hdr->e_ident[i++]);
    printf("%-12c|", hdr->e_ident[i++]);
    printf("%-12c|", hdr->e_ident[i++]);
    printf("%-12c|", hdr->e_ident[i++]);
    printf("%-12u|", hdr->e_ident[i++]);
    printf("%-12u|", hdr->e_ident[i++]);
    printf("%-12u|\n", hdr->e_ident[i++]);

    printf("------------|------------|------------|------------|------------|------------|------------|\n");
    printf("E_TYPE      |E_MACHINE   |E_VERSION   |E_ENTRY     |E_PHOFF     |E_SHOFF     |E_FLAGS     |\n");
    printf("------------|------------|------------|------------|------------|------------|------------|\n");
    printf("%-12u|", hdr->e_type);
    printf("%-12u|", hdr->e_machine);
    printf("%-12u|", hdr->e_version);
    printf("%-12x|", hdr->e_entry);
    printf("%-12u|", hdr->e_phoff);
    printf("%-12u|", hdr->e_shoff);
    printf("%-12u|\n", hdr->e_flags);


    printf("------------|------------|------------|------------|------------|------------|------------|\n");
    printf("E_EHSIZE    |E_PHENTSIZE |E_PHNUM     |E_SHENTSIZE |E_SHNUM     |E_SHSTRNDX  |\n");
    printf("------------|------------|------------|------------|------------|------------|------------|\n");
    printf("%-12u|", hdr->e_ehsize);
    printf("%-12u|", hdr->e_phentsize);
    printf("%-12u|", hdr->e_phnum);
    printf("%-12u|", hdr->e_shentsize);
    printf("%-12u|", hdr->e_shnum);
    printf("%-12u|\n", hdr->e_shstrndx);
    printf("------------|------------|------------|------------|------------|------------|------------|\n");
}

size_t read_hdr(Elf32_Ehdr* hdr, FILE* file)
{
    size_t n = 0;
    n += fread(&hdr->e_ident, sizeof(*hdr->e_ident), EI_NIDENT, file);
    n += fread(&hdr->e_type, sizeof(hdr->e_type), 1, file);
    n += fread(&hdr->e_machine, sizeof(hdr->e_machine), 1, file);
    n += fread(&hdr->e_version, sizeof(hdr->e_version), 1, file);
    n += fread(&hdr->e_entry, sizeof(hdr->e_entry), 1, file);
    n += fread(&hdr->e_phoff, sizeof(hdr->e_phoff), 1, file);
    n += fread(&hdr->e_shoff, sizeof(hdr->e_shoff), 1, file);
    n += fread(&hdr->e_flags, sizeof(hdr->e_flags), 1, file);
    n += fread(&hdr->e_ehsize, sizeof(hdr->e_ehsize), 1, file);
    n += fread(&hdr->e_phentsize, sizeof(hdr->e_phentsize), 1, file);
    n += fread(&hdr->e_phnum, sizeof(hdr->e_phnum), 1, file);
    n += fread(&hdr->e_shentsize, sizeof(hdr->e_shentsize), 1, file);
    n += fread(&hdr->e_shnum, sizeof(hdr->e_shnum), 1, file);
    n += fread(&hdr->e_shstrndx, sizeof(hdr->e_shstrndx), 1, file);
    return n;
}

void dump_shdr(Elf32_Shdr* shdr)
{
    assert(shdr != NULL && "[ERROR] hdr is null...");
    printf("\n");
    printf("SH_NAME     |SH_TYPE     |SH_FLAGS    |SH_ADDR     |SH_OFFSET   |SH_SIZE     |SH_LINK     |\n");
    printf("------------|------------|------------|------------|------------|------------|------------|\n");
    printf("%-12u|", shdr->sh_name);
    printf("%-12x|", shdr->sh_type);
    printf("%-12u|", shdr->sh_flags);
    printf("%-12x|", shdr->sh_addr);
    printf("%-12u|", shdr->sh_offset);
    printf("%-12u|", shdr->sh_size);
    printf("%-12u|\n", shdr->sh_link);

    printf("------------|------------|------------|------------|------------|------------|------------|\n");
    printf("SH_INFO     |SH_ADDRALIGN|SH_ENTSIZE  |\n");
    printf("------------|------------|------------|------------|------------|------------|------------|\n");
    printf("%-12u|", shdr->sh_info);
    printf("%-12u|", shdr->sh_addralign);
    printf("%-12u|\n", shdr->sh_link);
    printf("------------|------------|------------|------------|------------|------------|------------|\n");
}

size_t read_shdr(Elf32_Shdr* shdr, FILE* file)
{
    assert(shdr != NULL && "[ERROR] shdr is null");
    size_t n = 0;
    n += fread(&shdr->sh_name, sizeof(shdr->sh_name), 1, file);
    n += fread(&shdr->sh_type, sizeof(shdr->sh_type), 1, file);
    n += fread(&shdr->sh_flags, sizeof(shdr->sh_flags), 1, file);
    n += fread(&shdr->sh_addr, sizeof(shdr->sh_addr), 1, file);
    n += fread(&shdr->sh_offset, sizeof(shdr->sh_offset), 1, file);
    n += fread(&shdr->sh_size, sizeof(shdr->sh_size), 1, file);
    n += fread(&shdr->sh_link, sizeof(shdr->sh_link), 1, file);
    n += fread(&shdr->sh_info, sizeof(shdr->sh_info), 1, file);
    n += fread(&shdr->sh_addralign,sizeof(shdr->sh_addralign), 1, file);
    n += fread(&shdr->sh_entsize, sizeof(shdr->sh_entsize), 1, file);
    return n;
}


void print_section_names(shdr_tabl* tabl, size_t str_tabl_i, FILE* file)
{
    assert(tabl != NULL && "[ERROR] tabl is null");
    
    //TODO: remove this with a mallox buffer
    Elf32_Word buffer_size = tabl->items[str_tabl_i].sh_size;
    fixed_str* str  = fixed_str_init(buffer_size);

    fseek(file, tabl->items[str_tabl_i].sh_offset, SEEK_SET);
    fread(str->items, sizeof(*str->items), buffer_size, file);
    for (size_t i = 0; i < tabl->len; i++) {
        Elf32_Word element = tabl->items[i].sh_name;
        size_t slice_len = str_len(str, element);
        fixed_str* slice  = str_slice(str, element, element + slice_len);
        printf("Section %d name at %d: %s\n", (int)i, element, slice->items);
        fixed_str_free(slice);
    }
    fixed_str_free(str);
}

int main(int argc, char** argv)
{
    char* file_name;
    FILE* file;
    size_t n = 0;
    shdr_tabl* arr = NULL;
    assert(argc == 2 && "[Usage] ./elf <file-name>");
    file_name = argv[argc - 1];
    file = fopen(file_name, "rb");
    assert(file != NULL && "[ERROR] failed to open file..");

    Elf32_Ehdr hdr = {0};
    n = read_hdr(&hdr, file);
    if (n != (EI_NIDENT + EI_NSTRUCT)) {
        fprintf(stderr, "[ERROR] did not read the correct amount of elf header elements\n");
        goto fail;
    }
    if (strncmp((const char*)hdr.e_ident, "\x7f" "ELF", EI_NIDENT) == 0) {
        fprintf(stderr, "[ERROR] Magic numbers do not match up..\n");
        goto fail;
    }
    dump_hdr(&hdr);
    if (hdr.e_type != ET_EXEC) {
        fprintf(stderr, "[ERROR] not an executable\n");
        goto fail;
    }
    if (hdr.e_machine != EM_RISCV) {
        fprintf(stderr, "[ERROR] only riscv executable\n");
        goto fail;
    }
    if (hdr.e_version != EV_CURRENT) {
        fprintf(stderr, "[ERROR] incorrect elf version\n");
        goto fail;
    }

    // seek until start of section header table
    fseek(file, hdr.e_shoff, SEEK_SET);
    arr = shdr_tabl_init(hdr.e_shnum);
    // every section header is stored sequentially in the table,
    // first entry always null and zeroed out
    for (size_t i = 0; i < hdr.e_shnum; i++) {
        printf("\nSection %d\n", (int) i);
        n = read_shdr(&arr->items[i], file);
        if (n != SH_NSTRUCT) {
            fprintf(stderr, "[ERROR] did not read the correct amount of section header elements\n");
            goto fail;
        }
        dump_shdr(&arr->items[i]);
    }
    
    size_t prev_loc = ftell(file);
    print_section_names(arr, hdr.e_shstrndx, file);
    fseek(file, prev_loc, SEEK_SET);


    shdr_tabl_free(arr);
    fclose(file);
    return 0;

fail:
    shdr_tabl_free(arr);
    fclose(file);
    return 1;
}
