# Single purpose header only elf file reader.
> [!WARNING]
> Only 32 bit executables are supported..

This is a header only library to load elf files specifically for getting the executable segments. Upon
successful read of the elf file, you will get back an elf file struct:
```cpp
typedef struct {
    byte_arr* contents; // file contents
    Elf32_Ehdr* hdr; // elf header
    shdr_tabl* shdr_table; // section header table
    phdr_tabl* phdr_table; // program header table
    seg_tabl* seg_table; // segments table
} elf_file;
```
that contains mostly all the information you need to get the data. It is composed of a list
of segments that each hold their own array of bytes.

## Usage
It is a stb style header only library, include it in your project using:
```cpp
#define ELF_IMPLEMENTATION
#include "elf.h"
```

## Build Linux example
```console
cd elf-files/
make && ./build/main <file>
```