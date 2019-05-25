#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "emulator.h"

unsigned int num_instructions;
instruction *instructions;
unsigned char carry_flag;
unsigned char overflow_flag;
unsigned char last_operation;
unsigned char last_result;
unsigned int program_counter = 0;

void free_instructions(){
	free(instructions);
}

unsigned int get_program_counter(){
	return program_counter;
}

char *execute_instruction(unsigned char (*read_input)(), void (*write_output)(unsigned char, unsigned char)){
	unsigned char destination;
	unsigned char source1;
	unsigned char source2;
	unsigned char immediate_low;
	
	unsigned char immediate;
	unsigned char next_carry;
	unsigned char do_branch;
	unsigned int address;
	unsigned int result;

	if(program_counter >= num_instructions){
		return "Program counter out of range\n";
	}
	
	destination = instructions[program_counter].destination;
	source1 = instructions[program_counter].source1;
	source2 = instructions[program_counter].source2;
	immediate_low = instructions[program_counter].immediate_low;

	if(destination > 15 || source1 > 15 || source2 > 15 || immediate_low > 15){
		return "Instruction operand out of range\n";
	}

	immediate = source2<<4 | immediate_low;
	address = ((unsigned int) source1)<<8 | source2<<4 | immediate_low;
	
	if(last_operation == SUB && instructions[program_counter].operation != SUB){
		carry_flag = 0;
	}

	switch(instructions[program_counter].operation){
		case ADDI:
			result = ((unsigned int) registers[source1]) + immediate;
			next_carry = (result&0x100)>>8;
			if((((registers[source1]&0x7F) + (immediate&0x7F) + carry_flag)&0x80)>>7 != next_carry){
				overflow_flag = 1;
			} else {
				overflow_flag = 0;
			}
			carry_flag = next_carry;
			registers[destination] = result&0xFF;
			break;
		case ADDC:
			result = ((unsigned int) registers[source1]) + registers[source2] + carry_flag;
			next_carry = (result&0x100)>>8;
			if((((registers[source1]&0x7F) + (registers[source2]&0x7F) + carry_flag)&0x80)>>7 != next_carry){
				overflow_flag = 1;
			} else {
				overflow_flag = 0;
			}
			carry_flag = next_carry;
			registers[destination] = result&0xFF;
			break;
		case SUB:
			result = ((unsigned int) registers[source1]) + (~registers[source2]);
			if(last_operation == SUB){
				result += carry_flag;
			} else {
				result++;
			}
			next_carry = (result&0x100)>>8;
			if(last_operation == SUB && (((registers[source1]&0x7F) + ((~registers[source2])&0x7F) + carry_flag)&0x80)>>7 != next_carry){
				overflow_flag = 1;
			} else if(last_operation != SUB && (((registers[source1]&0x7F) + ((~registers[source2])&0x7F) + 1)&0x80)>>7 != next_carry){
				overflow_flag = 1;
			} else {
				overflow_flag = 0;
			}
			carry_flag = next_carry;
			registers[destination] = result&0xFF;
			break;
		case AND:
			registers[destination] = registers[source1]&registers[source2];
			break;
		case OR:
			registers[destination] = registers[source1] | registers[source2];
			break;
		case XOR:
			registers[destination] = registers[source1]^registers[source2];
			break;
		case ANDI:
			registers[destination] = registers[source1]&immediate;
			break;
		case LOADI:
			registers[destination] = immediate;
			break;
		case XORI:
			registers[destination] = registers[source1]^immediate;
			break;
		case RSHFT:
			next_carry = registers[source1]&1;
			registers[destination] = registers[source1]>>1 | carry_flag<<7;
			carry_flag = next_carry;
			break;
		case RROLL:
			if(registers[source1]&1){
				registers[destination] = registers[source1]>>1 | 0x80;
				carry_flag = 1;
			} else {
				registers[destination] = registers[source1]>>1;
				carry_flag = 0;
			}
			break;
		case BRNCH:
			if(!destination){
				program_counter = address - 1;
			} else {
				do_branch = 0;
				if((destination&0x8) && !last_result){
					do_branch = 1;
				}
				if((destination&0x4) && (last_result&0x80)){
					do_branch = 1;
				}
				if((destination&0x2) && carry_flag){
					do_branch = 1;
				}
				if((destination&0x1) && overflow_flag){
					do_branch = 1;
				}

				if(do_branch){
					program_counter = address - 1;
				}
			}
			break;
		case JMPD:
			program_counter = (((unsigned int) registers[source1])<<8 | registers[source2]) - 1;
			break;
		case WRITE:
			write_output(registers[source1], registers[source2]);
			break;
		case READ:
			registers[destination] = read_input();
			break;
		case ADD:
			result = ((unsigned int) registers[source1]) + registers[source2];
			next_carry = (result&0x100)>>8;
			if((((registers[source1]&0x7F) + (registers[source2]&0x7F))&0x80)>>7 != next_carry){
				overflow_flag = 1;
			} else {
				overflow_flag = 0;
			}
			carry_flag = next_carry;
			registers[destination] = result&0xFF;
			break;
	}

	last_result = registers[destination];
	last_operation = instructions[program_counter].operation;
	
	if(last_operation != ADDI && last_operation != ADDC && last_operation != SUB && last_operation != RSHFT && last_operation != RROLL && last_operation != ADD){
		carry_flag = 0;
	}
	
	program_counter++;

	return NULL;
}

char *load_file(char *file_name){
	FILE *finput;
	int current_pos = -2;
	unsigned int current_instruction = 0;
	char char_buffer[256];
	unsigned char i;

	finput = fopen(file_name, "r");
	if(!finput){
		return "Could not open input file\n";
	}	

	do{
		current_pos--;
		fseek(finput, current_pos, SEEK_END);
	} while(fgetc(finput) != '\n');

	fgets(char_buffer, 256, finput);

	if(sscanf(char_buffer, "%d:", &num_instructions) != 1){
		return "Could not read input file\n";
	}

	num_instructions++;
	instructions = malloc(sizeof(instruction)*num_instructions);

	rewind(finput);
	while(!feof(finput)){
		current_pos = 0;
		fgets(char_buffer, 256, finput);
		while(char_buffer[current_pos] != ' '){
			current_pos++;
		}
		current_pos++;

		instructions[current_instruction].operation = 0;
		for(i = 0; i < 4; i++){
			instructions[current_instruction].operation <<= 1;
			if(char_buffer[current_pos] == '1'){
				instructions[current_instruction].operation++;
			} else if(char_buffer[current_pos] != '0'){
				free(instructions);
				return "Expected binary opcode\n";
			}
			current_pos++;
		}
		
		if(char_buffer[current_pos] == '|'){
			current_pos++;
		}

		instructions[current_instruction].destination = 0;
		for(i = 0; i < 4; i++){
			instructions[current_instruction].destination <<= 1;
			if(char_buffer[current_pos] == '1'){
				instructions[current_instruction].destination++;
			} else if(char_buffer[current_pos] != '0'){
				free(instructions);
				return "Expected binary destination operand\n";
			}
			current_pos++;
		}

		if(char_buffer[current_pos] == '|'){
			current_pos++;
		}

		instructions[current_instruction].source1 = 0;
		for(i = 0; i < 4; i++){
			instructions[current_instruction].source1 <<= 1;
			if(char_buffer[current_pos] == '1'){
				instructions[current_instruction].source1++;
			} else if(char_buffer[current_pos] != '0'){
				free(instructions);
				return "Expected binary source operand\n";
			}
			current_pos++;
		}

		if(char_buffer[current_pos] == '|'){
			current_pos++;
		}

		instructions[current_instruction].source2 = 0;
		for(i = 0; i < 4; i++){
			instructions[current_instruction].source2 <<= 1;
			if(char_buffer[current_pos] == '1'){
				instructions[current_instruction].source2++;
			} else if(char_buffer[current_pos] != '0'){
				free(instructions);
				return "Expected binary source operand\n";
			}
			current_pos++;
		}
		
		if(char_buffer[current_pos] == '|'){
			current_pos++;
		}

		instructions[current_instruction].immediate_low = 0;
		for(i = 0; i < 4; i++){
			instructions[current_instruction].immediate_low <<= 1;
			if(char_buffer[current_pos] == '1'){
				instructions[current_instruction].immediate_low++;
			} else if(char_buffer[current_pos] != '0'){
				free(instructions);
				return "Expected binary immediate operand\n";
			}
			current_pos++;
		}
		
		current_instruction++;
	}

	fclose(finput);
	return NULL;
}
