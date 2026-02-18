CC = gcc 
FLAGS = -Wall -Wextra -Werror -ggdb -std=c99
OUTPUT = build/elf
src = elf.c

all: build main 

main:
	${CC} $(src) ${FLAGS} -o ${OUTPUT}

build:
	mkdir build/

clean:
	rm -rf build/
