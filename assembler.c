#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "dictionary.h"

char *error_message;

unsigned char get_instruction_register(char *buffer, unsigned char *dest, unsigned char *source){
	int source_int;
	int dest_int;

	if(sscanf(buffer, "R%d R%d\n", &dest_int, &source_int) == 2){
		if(source_int >= 0 && source_int <= 15 && dest_int >= 0 && dest_int <= 15){
			*source = source_int;
			*dest = dest_int;
		} else {
			error_message = "Register value out of range";
			return 0;
		}
	} else {
		error_message = "Invalid syntax";
		return 0;
	}

	return 1;
}

unsigned char get_instruction_no_output(char *buffer, unsigned char *source_1, unsigned char *source_2){
	int source_1_int;
	int source_2_int;

	if(sscanf(buffer, "R%d R%d\n", &source_1_int, &source_2_int) == 2){
		if(source_1_int >= 0 && source_1_int <= 15 && source_2_int >= 0 && source_2_int <= 15){
			*source_1 = source_1_int;
			*source_2 = source_2_int;
		} else {
			error_message = "Register value out of range";
			return 0;
		}
	} else {
		error_message = "Invalid syntax";
		return 0;
	}

	return 1;
}

unsigned char get_instruction(char *buffer, unsigned char *dest, unsigned char *source_1, unsigned char *source_2){
	int dest_int;
	int source_1_int;
	int source_2_int;
	
	if(sscanf(buffer, "R%d R%d R%d\n", &dest_int, &source_1_int, &source_2_int) == 3){
		if(dest_int >= 0 && dest_int <= 15 && source_1_int >= 0 && source_1_int <= 15 && source_2_int >= 0 && source_2_int <= 15){
			*dest = dest_int;
			*source_1 = source_1_int;
			*source_2 = source_2_int;
		} else {
			error_message = "Register value out of range";
			return 0;
		}
	} else {
		error_message = "Invalid syntax";
		return 0;
	}

	return 1;
}

unsigned char get_instruction_only_output(char *buffer, unsigned char *dest){
	int dest_int;

	if(sscanf(buffer, "R%d\n", &dest_int) == 1){
		if(dest_int >= 0 && dest_int <= 15){
			*dest = dest_int;
		} else {
			error_message = "Register value out of range";
			return 0;
		}
	} else {
		error_message = "Invalid syntax";
		return 0;
	}

	return 1;
}

unsigned char get_instruction_output_immediate(char *buffer, unsigned char *dest, unsigned char *immediate){
	int dest_int;
	int immediate_int;

	if(sscanf(buffer, "R%d %d\n", &dest_int, &immediate_int) == 2){
		if(dest_int >= 0 && dest_int <= 15){
			if(immediate_int < 0 && immediate_int >= -128){
				*immediate = (~((unsigned char) -immediate_int)) + 1;
			} else if(immediate_int >= 0 && immediate_int <= 255){
				*immediate = immediate_int;
			} else {
				error_message = "Immediate value out of range";
				return 0;
			}
			
			*dest = dest_int;
		} else {
			error_message = "Register value out of range";
			return 0;
		}
	} else {
		error_message = "Invalid syntax";
		return 0;
	}

	return 1;
}

unsigned char get_instruction_immediate(char *buffer, unsigned char *dest, unsigned char *source_1, unsigned char *immediate){
	int dest_int;
	int source_int;
	int immediate_int;

	if(sscanf(buffer, "R%d R%d %d\n", &dest_int, &source_int, &immediate_int) == 3){
		if(dest_int >= 0 && dest_int <= 15 && source_int >= 0 && source_int <= 15){
			if(immediate_int < 0 && immediate_int >= -128){
				*immediate = (~((unsigned char) -immediate_int)) + 1;
			} else if(immediate_int >= 0 && immediate_int <= 255){
				*immediate = immediate_int;
			} else {
				error_message = "Immediate value out of range";
				return 0;
			}
			*dest = dest_int;
			*source_1 = source_int;
		} else {
			error_message = "Register value out of range";
			return 0;
		}
	} else {
		error_message = "Invalid syntax";
		return 0;
	}

	return 1;
}

void write_register(unsigned char reg, FILE *foutput){
	unsigned char i;

	for(i = 0; i < 4; i++){
		if(reg&0x8){
			fprintf(foutput, "1");
		} else {
			fprintf(foutput, "0");
		}
		reg <<= 1;
	}
	fprintf(foutput, "|");
}

void write_immediate(unsigned char immediate, FILE *foutput){
	unsigned char i;

	for(i = 0; i < 8; i++){
		if(immediate&0x80){
			fprintf(foutput, "1");
		} else {
			fprintf(foutput, "0");
		}
		immediate <<= 1;
		if(i == 3){
			fprintf(foutput, "|");
		}
	}
	fprintf(foutput, "|");
}

void write_address(unsigned int address, FILE *foutput){
	unsigned char i;

	for(i = 0; i < 12; i++){
		if(address&0x800){
			fprintf(foutput, "1");
		} else {
			fprintf(foutput, "0");
		}
		address <<= 1;
		
		if(!((i + 1)%4)){
			fprintf(foutput, "|");
		}
	}
}

unsigned char write_instruction_register(char *buffer, FILE *foutput){
	unsigned char source;
	unsigned char dest;

	if(!get_instruction_register(buffer, &dest, &source)){
		return 0;
	}

	write_register(dest, foutput);
	write_register(source, foutput);
	fprintf(foutput, "0000|0000|");
}

unsigned char write_instruction_immediate(char *buffer, FILE *foutput){
	unsigned char dest;
	unsigned char source;
	unsigned char immediate;

	if(!get_instruction_immediate(buffer, &dest, &source, &immediate)){
		return 0;
	}

	write_register(dest, foutput);
	write_register(source, foutput);
	write_immediate(immediate, foutput);

	return 1;
}

unsigned char write_instruction(char *buffer, FILE *foutput){
	unsigned char dest;
	unsigned char source_1;
	unsigned char source_2;

	if(!get_instruction(buffer, &dest, &source_1, &source_2)){
		return 0;
	}

	write_register(dest, foutput);
	write_register(source_1, foutput);
	write_register(source_2, foutput);
	fprintf(foutput, "0000|");

	return 1;
}

unsigned char write_instruction_no_output(char *buffer, FILE *foutput){
	unsigned char source_1;
	unsigned char source_2;

	if(!get_instruction_no_output(buffer, &source_1, &source_2)){
		return 0;
	}

	fprintf(foutput, "0000|");
	write_register(source_1, foutput);
	write_register(source_2, foutput);
	fprintf(foutput, "0000|");

	return 1;
}

unsigned char write_instruction_output_immediate(char *buffer, FILE *foutput){
	unsigned char dest;
	unsigned char immediate;

	if(!get_instruction_output_immediate(buffer, &dest, &immediate)){
		return 0;
	}

	write_register(dest, foutput);
	fprintf(foutput, "0000|");
	write_immediate(immediate, foutput);
}

unsigned char write_instruction_only_output(char *buffer, FILE *foutput){
	unsigned char dest;

	if(!get_instruction_only_output(buffer, &dest)){
		return 0;
	}
	
	write_register(dest, foutput);
	fprintf(foutput, "0000|0000|0000|");
}

void write_output(FILE *finput, FILE *foutput, dictionary labels){
	char char_buffer[256];
	size_t line_length;
	size_t current_character;
	unsigned int current_address = 0;
	unsigned char dest;
	unsigned char condition;
	unsigned char source_1;
	unsigned char source_2;
	unsigned int dest_address;
	unsigned char immediate;
	unsigned char no_error;
	unsigned char commenting;
	unsigned int *address_pointer;

	while(!feof(finput)){
		fgets(char_buffer, 256, finput);
		if(feof(finput)){
			break;
		}

		if(char_buffer[0] == '`'){
			continue;
		}

		line_length = strlen(char_buffer);
		if(line_length != 0){
			if(char_buffer[line_length - 1] != '\n' && !feof(finput)){
				printf("Error: line %d too long\n", current_address + 1);
				fclose(finput);
				fclose(foutput);
				free_dictionary(labels, free);
				exit(1);
			}
			if(line_length > 1){
				current_character = line_length - 2;
				commenting = 0;
				while(current_character && (char_buffer[current_character] == ' ' || char_buffer[current_character] == '\t' || char_buffer[current_character] == '`' || commenting)){
					if(char_buffer[current_character] == '`'){
						commenting = !commenting;
					}
					current_character--;
				}
				if(char_buffer[current_character] == ':'){
					continue;
				}
			} else if(line_length == 1 && char_buffer[0] == '\n' || char_buffer[0] == ' ' || char_buffer[0] == '\t'){
				continue;
			}
		}

		current_character = 0;
		while(char_buffer[current_character] != ' ' && char_buffer[current_character] != '\t' && char_buffer[current_character]){
			current_character++;
		}

		if(!char_buffer[current_character]){
			printf("Error: Invalid syntax (line %d)\n", current_address + 2);
			free_dictionary(labels, free);
			fclose(finput);
			fclose(foutput);
			exit(1);
		}		
	
		fprintf(foutput, "%d: ", current_address);
		char_buffer[current_character] = (char) 0;
		no_error = 1;
		if(!strcmp(char_buffer, "addi")){
			fprintf(foutput, "0000|");
			no_error = write_instruction_immediate(char_buffer + current_character + 1, foutput);
			fprintf(foutput, "\n");
		} else if(!strcmp(char_buffer, "addc")){
			fprintf(foutput, "0001|");
			no_error = write_instruction(char_buffer + current_character + 1, foutput);
			fprintf(foutput, "\n");
		} else if(!strcmp(char_buffer, "sub")){
			fprintf(foutput, "0010|");
			no_error = write_instruction(char_buffer + current_character + 1, foutput);
			fprintf(foutput, "\n");
		} else if(!strcmp(char_buffer, "and")){
			fprintf(foutput, "0011|");
			no_error = write_instruction(char_buffer + current_character + 1, foutput);
			fprintf(foutput, "\n");
		} else if(!strcmp(char_buffer, "or")){
			fprintf(foutput, "0100|");
			no_error = write_instruction(char_buffer + current_character + 1, foutput);
			fprintf(foutput, "\n");
		} else if(!strcmp(char_buffer, "xor")){
			fprintf(foutput, "0101|");
			no_error = write_instruction(char_buffer + current_character + 1, foutput);
			fprintf(foutput, "\n");
		} else if(!strcmp(char_buffer, "andi")){
			fprintf(foutput, "0110|");
			no_error = write_instruction_immediate(char_buffer + current_character + 1, foutput);
			fprintf(foutput, "\n");
		} else if(!strcmp(char_buffer, "loadi")){
			fprintf(foutput, "0111|");
			no_error = write_instruction_output_immediate(char_buffer + current_character + 1, foutput);
			fprintf(foutput, "\n");
		} else if(!strcmp(char_buffer, "xori")){
			fprintf(foutput, "1000|");
			no_error = write_instruction_immediate(char_buffer + current_character + 1, foutput);
			fprintf(foutput, "\n");
		} else if(!strcmp(char_buffer, "rshft")){
			fprintf(foutput, "1001|");
			no_error = write_instruction_register(char_buffer + current_character + 1, foutput);
			fprintf(foutput, "\n");
		} else if(!strcmp(char_buffer, "rroll")){
			fprintf(foutput, "1010|");
			no_error = write_instruction_register(char_buffer + current_character + 1, foutput);
			fprintf(foutput, "\n");
		} else if(!strcmp(char_buffer, "bz") || !strcmp(char_buffer, "bn") || !strcmp(char_buffer, "bc") || !strcmp(char_buffer, "bv") || !strcmp(char_buffer, "jmp")){
			line_length = strlen(char_buffer + current_character + 1);
			
			commenting = 0;
			while(line_length && (char_buffer[current_character + line_length] == ' ' || char_buffer[current_character + line_length] == '\t' || char_buffer[current_character + line_length] == '\n' || char_buffer[current_character + line_length] == '`' || commenting)){
				if(char_buffer[current_character + line_length] == '`'){
					commenting = !commenting;
				}

				char_buffer[current_character + line_length] = (char) 0;
				line_length--;
			}

			address_pointer = read_dictionary(labels, char_buffer + current_character + 1, 0);
			if(!address_pointer){
				printf("Error: Unresolved label (line %d)\n", current_address + 1);
				free_dictionary(labels, free);
				fclose(finput);
				fclose(foutput);
				exit(1);
			}

			fprintf(foutput, "1011|");
			if(!strcmp(char_buffer, "bz")){
				fprintf(foutput, "1000|");
			} else if(!strcmp(char_buffer, "bn")){
				fprintf(foutput, "0100|");
			} else if(!strcmp(char_buffer, "bc")){
				fprintf(foutput, "0010|");
			} else if(!strcmp(char_buffer, "bv")){
				fprintf(foutput, "0001|");
			} else if(!strcmp(char_buffer, "jmp")){
				fprintf(foutput, "0000|");
			}

			write_address(*address_pointer, foutput);
			fprintf(foutput, "\n");
		} else if(!strcmp(char_buffer, "jmpd")){
			fprintf(foutput, "1100|");
			no_error = write_instruction_no_output(char_buffer + current_character + 1, foutput);
			fprintf(foutput, "\n");
		} else if(!strcmp(char_buffer, "write")){
			fprintf(foutput, "1101|");
			no_error = write_instruction_no_output(char_buffer + current_character + 1, foutput);
			fprintf(foutput, "\n");
		} else if(!strcmp(char_buffer, "read")){
			fprintf(foutput, "1110|");
			no_error = write_instruction_only_output(char_buffer + current_character + 1, foutput);
			fprintf(foutput, "\n");
		} else if(!strcmp(char_buffer, "add")){
			fprintf(foutput, "1111|");
			no_error = write_instruction(char_buffer + current_character + 1, foutput);
			fprintf(foutput, "\n");
		} else {
			printf("Error: Unrecognized operation (line %d)\n", current_address + 2);
			free_dictionary(labels, free);
			fclose(finput);
			fclose(foutput);
			exit(1);
		}

		if(!no_error){
			printf("Error: %s (line %d)\n", error_message, current_address + 1);
			free_dictionary(labels, free);
			fclose(finput);
			fclose(foutput);
			exit(1);
		}

		current_address++;
	}
}

void populate_labels(dictionary *labels, FILE *fp){
	unsigned int current_address = 0;
	char char_buffer[256];
	size_t line_length;
	size_t current_character;
	unsigned char commenting;
	unsigned int *new_address;

	while(!feof(fp)){
		fgets(char_buffer, 256, fp);
		line_length = strlen(char_buffer);
		if(line_length != 0){
			if(char_buffer[line_length - 1] != '\n' && !feof(fp)){
				printf("Error: line %d too long\n", current_address + 1);
				fclose(fp);
				free_dictionary(*labels, free);
				exit(1);
			}
			if(line_length > 1){
				current_character = line_length - 2;
				commenting = 0;
				while(current_character && (char_buffer[current_character] == ' ' || char_buffer[current_character] == '\t' || char_buffer[current_character] == '`' || commenting)){
					if(char_buffer[current_character] == '`'){
						commenting = !commenting;
					}
					current_character--;
				}
				if(char_buffer[current_character] == ':'){
					char_buffer[current_character] = (char) 0;
					new_address = malloc(sizeof(unsigned int));
					*new_address = current_address;
					write_dictionary(labels, char_buffer, new_address, 0);
				}
			}
		}
		current_address++;
	}
}

int main(int argc, char **argv){
	FILE *finput;
	FILE *foutput;
	char *output_file_name;
	dictionary labels;

	if(argc < 2){
		printf("Error: expected input file\n");
		exit(1);
	} else if(argc > 3){
		printf("Error: too many arguments\n");
	}

	finput = fopen(argv[1], "r");
	if(!finput){
		printf("Error: cannot open input file %s\n", argv[1]);
		exit(1);
	}

	labels = create_dictionary(NULL);
	populate_labels(&labels, finput);

	if(argc == 2){
		output_file_name = "a.o";
	} else {
		output_file_name = argv[2];
	}

	foutput = fopen(output_file_name, "w");
	rewind(finput);

	write_output(finput, foutput, labels);
	
	fclose(finput);
	fclose(foutput);
	free_dictionary(labels, free);

	return 0;
}
