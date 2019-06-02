#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "parse.h"
#include "linked_list.h"

linked_list *token_list_begin;
linked_list *token_list_current;

const char control_characters[] = {'(', ')', '{', '}', '[', ']', ',', ';'};

const char operator_characters[] = {'+', '-', '*', '/'};

unsigned char is_control_character(char c){
	unsigned int i;

	for(i = 0; i < sizeof(control_characters); i++){
		if(c == control_characters[i]){
			return 1;
		}
	}

	return 0;
}

unsigned char is_operator_character(char c){
	unsigned int i;

	for(i = 0; i < sizeof(operator_characters); i++){
		if(c == operator_characters[i]){
			return 1;
		}
	}

	return 0;
}

unsigned char is_alpha(char c){
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

unsigned char is_digit(char c){
	return c >= '0' && c <= '9';
}

char *get_next_separator(char *c){
	while(is_alpha(*c) || is_digit(*c) || *c == '_'){
		c++;
	}

	return c;
}

int get_integer(char *c, size_t num_digits){
	int result;

	result = 0;
	while(num_digits--){
		result = result*10 + *c - '0';
	}

	return result;
}

void free_token(void *p){
	token *tp;

	tp = p;
	if(tp->type == IDENTIFIER){
		free(tp->identifier);
	}

	free(tp);
}

token get_token(char **c){
	char *token_string;
	size_t token_size;
	token output;
	
	if(is_alpha(**c)){
		token_size = get_next_separator(*c) - *c;
		token_string = malloc(sizeof(char)*(token_size + 1));
		token_string[token_size] = (char) 0;
		memcpy(token_string, *c, token_size);
		if(!strcmp(token_string, "if")){
			output.type = KEYWORD;
			output.keyword_type = IF;
			*c += token_size;
			free(token_string);
			return output;
		} else if(!strcmp(token_string, "while")){
			output.type = KEYWORD;
			output.keyword_type = WHILE;
			*c += token_size;
			free(token_string);
			return output;
		} else if(!strcmp(token_string, "else")){
			output.type = KEYWORD;
			output.keyword_type = ELSE;
			*c += token_size;
			free(token_string);
			return output;
		} else if(!strcmp(token_string, "for")){
			output.type = KEYWORD;
			output.keyword_type = FOR;
			*c += token_size;
			free(token_string);
			return output;
		} else if(!strcmp(token_string, "int")){
			output.type = TYPE;
			output.dtype = INT;
			*c += token_size;
			free(token_string);
			return output;
		} else if(!strcmp(token_string, "char")){
			output.type = TYPE;
			output.dtype = CHAR;
			*c += token_size;
			free(token_string);
			return output;
		} else if(!strcmp(token_string, "void")){
			output.type = TYPE;
			output.dtype = VOID;
			*c += token_size;
			free(token_string);
			return output;
		} else {
			output.type = IDENTIFIER;
			output.identifier = token_string;
			*c += token_size;
			return output;
		}
	} else if(is_digit(**c)){
		token_size = get_next_separator(*c) - *c;
		output.type = LITERAL;
		output.ltype = INTEGER;
		output.int_value = get_integer(*c, token_size);
		*c += token_size;
		return output;
	} else if(is_control_character(**c)){
		output.type = CONTROL;
		output.control_character = **c;
		++*c;
		return output;
	} else if(is_operator_character(**c)){
		output.type = OPERATOR;
		output.operator_character = **c;
		++*c;
		return output;
	} else {
		output.type = UNKNOWN;
		output.unknown_character = **c;
		++*c;
		return output;
	}
}

int main(){
	char *input_text = "this is a test";
	token *current_token;
	token next_token;

	current_token = malloc(sizeof(token));
	*current_token = get_token(&input_text);
	token_list_begin = create_linked_list(current_token);
	token_list_current = token_list_begin;
	next_token = get_token(&input_text);
	while(next_token.type != UNKNOWN || next_token.unknown_character == ' '){
		current_token = malloc(sizeof(token));
		*current_token = next_token;
		add_linked_list(&token_list_current, create_linked_list(current_token));
		next_token = get_token(&input_text);
	}

	token_list_current = token_list_begin;
	while(token_list_current){
		if(((token *) token_list_current->value)->type == IDENTIFIER){
			printf("%s ", ((token *) token_list_current->value)->identifier);
		}
		token_list_current = token_list_current->next;
	}
	
	free_linked_list(token_list_begin, free_token);
	return 0;
}
