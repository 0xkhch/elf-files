#include "elf.h"
#define FIXED_STR_IMPLEMENTATION
#include "fixed_str.h"

typedef struct {
    Elf32_Shdr* items;
    size_t len;
} shdr_tabl;

typedef struct {
    Elf32_Phdr* items;
    size_t len;
} phdr_tabl;

typedef struct {
    uint8_t* items;
    uint64_t cap;
    uint64_t len;
} byte_arr;

byte_arr* init_arr(uint64_t len)
{
    byte_arr* arr = calloc(1, sizeof(byte_arr));
    arr->items = calloc(len, sizeof(uint8_t));
    arr->len = len;
    return arr;
}

void free_arr(byte_arr* arr)
{
    free(arr->items);
    free(arr);
}

void free_hdr(Elf32_Ehdr* arr)
{
    free(arr);
}

void free_shdr_tabl(shdr_tabl* arr)
{
    free(arr->items);
    free(arr);
}

void free_phdr_tabl(phdr_tabl* arr)
{
    free(arr->items);
    free(arr);
}

Elf32_Ehdr* read_hdr(const byte_arr* arr)
{
    Elf32_Ehdr* hdr = calloc(1, sizeof(Elf32_Ehdr));
    memcpy(hdr, arr->items, sizeof(Elf32_Ehdr));
    return hdr;
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

void dump_shdr(Elf32_Shdr* shdr)
{
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

    Elf32_Word buffer_size = tabl->items[hdr->e_shstrndx].sh_size;
    char* buf = calloc(1, buffer_size + 1);
    // hdr->e_shstrndx is the section that holds the info for string table
    size_t offset = tabl->items[hdr->e_shstrndx].sh_offset; 
    memcpy(buf, arr->items + offset, buffer_size);
    for (size_t i = 0; i < tabl->len; i++) {
        Elf32_Word name_index = tabl->items[i].sh_name;
        printf("\nSection %d name: %s at %d\n", (int) i, buf + name_index, name_index);
        dump_shdr(&tabl->items[i]);
    }
    free(buf);
    return tabl;
}

Elf32_Phdr read_phdr(const byte_arr* arr, uint64_t offset)
{
    
    Elf32_Phdr phdr = {0};
    memcpy(&phdr, arr->items + offset, sizeof(Elf32_Phdr));
    return phdr;
}
void dump_phdr(Elf32_Phdr* phdr)
{
    printf("P_TYPE      |P_OFFSET    |P_VADDR     |P_PADDR     |P_FILESZ    |P_MEMSZ     |P_FLAGS     |\n");
    printf("------------|------------|------------|------------|------------|------------|------------|\n");
    printf("%-12x|", phdr->p_type);
    printf("%-12u|", phdr->p_offset);
    printf("%-12x|", phdr->p_vaddr);
    printf("%-12x|", phdr->p_paddr);
    printf("%-12u|", phdr->p_filesz);
    printf("%-12u|", phdr->p_memsz);
    printf("%-12u|\n", phdr->p_flags);

    printf("------------|------------|------------|------------|------------|------------|------------|\n");
    printf("P_ALIGN     |            |            |\n");
    printf("------------|------------|------------|------------|------------|------------|------------|\n");
    printf("%-12u|\n", phdr->p_align);
    printf("------------|------------|------------|------------|------------|------------|------------|\n");
}

uint64_t is_ascii(int c) {
    // without control codes
    return c > 31 && c < 127;
}

void dump_seg(Elf32_Phdr* segment, const byte_arr* arr)
{
    #define col_num 8
    uint64_t addr   = segment->p_paddr;
    uint64_t offset = segment->p_offset;
    uint8_t buffer[col_num] = {0};

    for (uint64_t i = 0; i < segment->p_filesz; i++) {
        if (i % col_num == 0) {
            printf("%08x: ", (unsigned)addr);
        }

        uint8_t byte = arr->items[offset + i];
        buffer[i % col_num] = byte;
        printf("%04x ", byte);
        addr++;

        if ((i + 1) % col_num == 0) {
            printf("|");
            for (uint64_t j = 0; j < col_num; j++) {
                printf("%c", is_ascii(buffer[j]) == 1 ? buffer[j] : '.');
            }
            printf("|\n");
        }
    }
    printf("\n");
}

phdr_tabl* read_seg_tabl(Elf32_Ehdr* hdr, const byte_arr* arr)
{
    phdr_tabl* tabl = (phdr_tabl*)calloc(1, sizeof(phdr_tabl));
    tabl->items = (Elf32_Phdr*)calloc(hdr->e_phnum, sizeof(Elf32_Phdr));
    tabl->len = hdr->e_phnum;
    uint64_t phdr_offset = hdr->e_phoff;
    for (size_t i = 0; i < tabl->len; i++) {
        tabl->items[i] = read_phdr(arr, phdr_offset + (sizeof(Elf32_Phdr) * i));
        printf("\nSegment %d\n", (int) i);
        dump_phdr(&tabl->items[i]);
    }
    return tabl;
}

void dump_segments(phdr_tabl* tabl, const byte_arr* arr)
{
    assert(tabl != NULL && "[ERROR] tabl is null");

    for (size_t i = 0; i < tabl->len; i++) {
        // if (tabl->items[i].p_type != PT_LOAD) continue;
        printf("\nSegment %d\n", (int) i);
        dump_seg(&tabl->items[i], arr);
    }
}

byte_arr* read_entire_file(char* path)
{
    FILE* f = fopen(path, "rb");
    if (f == NULL) {
        printf("File not found...\n");
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

int main(int argc, char** argv)
{
    char* name = argv[argc - 1];
    byte_arr* arr = read_entire_file(name);
    Elf32_Ehdr* hdr = read_hdr(arr); dump_hdr(hdr);
    if (strncmp((const char*)hdr->e_ident, "\x7f" "ELF", EI_NIDENT) == 0) {
        fprintf(stderr, "[ERROR] Magic numbers do not match up..\n");
        return -1;
    }
    if (hdr->e_type != ET_EXEC) {
        fprintf(stderr, "[ERROR] not an executable\n");
    }
    if (hdr->e_machine != EM_RISCV) {
        fprintf(stderr, "[ERROR] only riscv executable\n");
    }
    if (hdr->e_version != EV_CURRENT) {
        fprintf(stderr, "[ERROR] incorrect elf version\n");
    }
    shdr_tabl* sh_tabl = read_shdr_tabl(hdr, arr);
    phdr_tabl* segment_tabl = read_seg_tabl(hdr, arr);
    dump_segments(segment_tabl, arr);

    free_phdr_tabl(segment_tabl);
    free_shdr_tabl(sh_tabl);
    free_hdr(hdr);
    free_arr(arr);
    return 0;
}
