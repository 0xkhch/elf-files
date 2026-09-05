#define ELF_IMPLEMENTATION
#include "elf.h"

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

void dump_shdr_tabl(shdr_tabl* tabl)
{
    for (size_t i = 0; i < tabl->len; i++) {
        Elf32_Word name_index = tabl->items[i].sh_name;
        printf("\nSection %d name: %s at index %d\n", (int) i, tabl->names + name_index, name_index);
        dump_shdr(&tabl->items[i]);
    }
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
void dump_phdr_tabl(phdr_tabl* tabl)
{
    for (size_t i = 0; i < tabl->len; i++) {
        printf("\nProgram header %d\n", (int) i);
        dump_phdr(&tabl->items[i]);
    }
}

uint64_t is_ascii(int c) {
    return c > 31 && c < 127;
}

void dump_seg(segment* seg)
{
    #define col_num 8
    uint64_t addr   = seg->addr;
    uint8_t buffer[col_num] = {0};

    for (uint64_t i = 0; i < seg->len; i++) {
        if (i % col_num == 0) {
            printf("%08x: ", (unsigned)addr);
        }

        uint8_t byte = seg->items[i];
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

void dump_segments(seg_tabl* tabl)
{
    for (size_t i = 0; i < tabl->len; i++) {
        printf("\nSegment %d\n", (int) i);
        dump_seg(tabl->items[i]);
    }
}

int main(int argc, char** argv)
{
    char* name = argv[argc - 1];
    elf_file* f = read_elf(name);
    if (f == NULL) {
        return -1;
    }
    dump_shdr_tabl(f->shdr_table);
    dump_phdr_tabl(f->phdr_table);
    dump_segments(f->seg_table);
    free_elf(f);
    return 0;
}
