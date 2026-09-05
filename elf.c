#define ELF_IMPLEMENTATION
#include "elf.h"

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
    dump_segments(f->seg_table);
    free_elf(f);
    return 0;
}
