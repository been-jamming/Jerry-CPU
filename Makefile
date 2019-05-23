CC = gcc
CFLAGS = -O3

default: dictionary.c dictionary.h assembler.c
	$(CC) $(CFLAGS) assembler.c dictionary.c -o jasm

clean:
	del jasm.exe
