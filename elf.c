#include "elf.h"

#define FIXED_STR_IMPLEMENTATION
#include "fixed_str.h"


typedef struct {
    Elf32_Shdr* items;
    size_t len;
} shdr_tabl;

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
    free(arr->items);
    free(arr);
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

void read_shdr_tabl(shdr_tabl* tabl, size_t n, FILE* file)
{
    // every section header is stored sequentially in the table,
    // first entry always null and zeroed out
    assert(tabl != NULL && "[ERROR] tabl is null");
    for (size_t i = 0; i < n; i++) {
        printf("\nSection %d\n", (int) i);
        read_shdr(&tabl->items[i], file);
        dump_shdr(&tabl->items[i]);
    }
}

void print_section_names(shdr_tabl* tabl, size_t str_tabl_i, FILE* file)
{
    assert(tabl != NULL && "[ERROR] tabl is null");
    
    Elf32_Word buffer_size = tabl->items[str_tabl_i].sh_size;
    fs_t* str_buff = fs_init(buffer_size);

    fseek(file, tabl->items[str_tabl_i].sh_offset, SEEK_SET);
    fread(str_buff->items, sizeof(*str_buff->items), buffer_size, file);
    for (size_t i = 0; i < tabl->len; i++) {
        Elf32_Word element = tabl->items[i].sh_name;
        size_t slice_len = fs_len(str_buff, element);
        fs_t* slice  = fs_slice(str_buff, element, element + slice_len);
        printf("Section %d name at %d: %s\n", (int)i, element, slice->items);
        fs_free(slice);
    }
    fs_free(str_buff);
}


size_t read_phdr(Elf32_Phdr* phdr, FILE* file)
{
    size_t n = 0;
    n += fread(&phdr->p_type,  sizeof(&phdr->p_type), 1, file);
    n += fread(&phdr->p_offset,sizeof(&phdr->p_offset), 1, file);
    n += fread(&phdr->p_vaddr, sizeof(&phdr->p_vaddr), 1, file);
    n += fread(&phdr->p_paddr, sizeof(&phdr->p_paddr), 1, file);
    n += fread(&phdr->p_filesz,sizeof(&phdr->p_filesz), 1, file);
    n += fread(&phdr->p_memsz, sizeof(&phdr->p_memsz), 1, file);
    n += fread(&phdr->p_flags, sizeof(&phdr->p_flags), 1, file);
    n += fread(&phdr->p_align, sizeof(&phdr->p_align), 1, file);
    return n;
}
void dump_phdr(Elf32_Phdr* phdr)
{
    assert(phdr != NULL && "[ERROR] hdr is null...");
    printf("\n");
    printf("P_TYPE      |P_OFFSET    |P_VADDR     |P_PADDR     |P_FILESZ    |P_MEMSZ     |P_FLAGS     |\n");
    printf("------------|------------|------------|------------|------------|------------|------------|\n");
    printf("%-12u|", phdr->p_type);
    printf("%-12u|", phdr->p_offset);
    printf("%-12x|", phdr->p_vaddr);
    printf("%-12x|", phdr->p_paddr);
    printf("%-12u|", phdr->p_filesz);
    printf("%-12u|", phdr->p_memsz);
    printf("%-12u|\n", phdr->p_flags);

    printf("------------|------------|------------|------------|------------|------------|------------|\n");
    printf("P_ALIGN     |            |            |\n");
    printf("------------|------------|------------|------------|------------|------------|------------|\n");
    printf("%-12u|", phdr->p_align);
    printf("------------|------------|------------|------------|------------|------------|------------|\n");
}

int main(int argc, char** argv)
{
    char* file_name;
    FILE* file;
    size_t n;
    shdr_tabl* arr;
    size_t prev_file_loc;

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
    arr = shdr_tabl_init(hdr.e_shnum);
    prev_file_loc = ftell(file);
    fseek(file, hdr.e_shoff, SEEK_SET);
    read_shdr_tabl(arr, hdr.e_shnum, file);
    fseek(file, prev_file_loc, SEEK_SET);

    prev_file_loc = ftell(file);
    print_section_names(arr, hdr.e_shstrndx, file);
    fseek(file, prev_file_loc, SEEK_SET);

    shdr_tabl_free(arr);
    fclose(file);
    return 0;

fail:
    fclose(file);
    return 1;
}
