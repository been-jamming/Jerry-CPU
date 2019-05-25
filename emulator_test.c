#include <stdlib.h>
#include <stdio.h>
#include "emulator.h"

unsigned char read_input(){
	int input;

	printf("Requesting input: ");
	scanf("%d", &input);

	return input;
}

void write_output(unsigned char a, unsigned char b){}

int main(int argc, char **argv){
	char *error_message;
	int last_program_counter = -1;
	unsigned char i;
	
	if(argc != 2){
		printf("Error: Expected single input file\n");
		exit(1);
	}

	error_message = load_file(argv[1]);
	if(error_message){
		printf("Error: %s", error_message);
		exit(1);
	}

	while(last_program_counter != get_program_counter()){
		last_program_counter = get_program_counter();
		error_message = execute_instruction(read_input, write_output);
		if(error_message){
			printf("Error: %s", error_message);
			free_instructions();
			exit(1);
		}
	}

	for(i = 0; i < 16; i++){
		printf("%d: %d\n", (int) i, (int) registers[i]);
	}

	return 0;
}
