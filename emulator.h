#ifndef EMULATOR_INCLUDED
#define EMULATOR_INCLUDED
typedef enum opcode opcode;

enum opcode{
	ADDI = 0x0,
	ADDC = 0x1,
	SUB = 0x2,
	AND = 0x3,
	OR = 0x4,
	XOR = 0x5,
	ANDI = 0x6,
	LOADI = 0x7,
	XORI = 0x8,
	RSHFT = 0x9,
	RROLL = 0xA,
	BRNCH = 0xB,
	JMPD = 0xC,
	WRITE = 0xD,
	READ = 0xE,
	ADD = 0xF
};

typedef struct instruction instruction;

struct instruction{
	opcode operation;
	unsigned char destination;
	unsigned char source1;
	unsigned char source2;
	unsigned char immediate_low;
};

void free_instructions();

char *load_file(char *file_name);

char *execute_instruction(unsigned char (*read_input)(), void (*write_output)(unsigned char, unsigned char));

unsigned int get_program_counter();

unsigned char registers[16];
#endif

