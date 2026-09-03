CC = gcc 
FLAGS = -Wall -Wextra -ggdb -std=c99
OUTPUT = build/elf
src = elf.c

all: clean build main 

main:
	${CC} $(src) ${FLAGS} -o ${OUTPUT}

build:
	mkdir build/

clean:
	rm -rf build/
