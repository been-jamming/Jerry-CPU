CC = gcc
CFLAGS = -O3 -Wall

default: dictionary.c dictionary.h assembler.c emulator.h emulator.c emulator_test.c
	$(CC) $(CFLAGS) assembler.c dictionary.c -o jasm
	$(CC) $(CFLAGS) emulator.c emulator_test.c -o emulator

clean:
	del jasm.exe
	del emulator.exe
