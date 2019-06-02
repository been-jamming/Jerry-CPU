#include <stdlib.h>
#include "linked_list.h"

linked_list *create_linked_list(void *value){
	linked_list *output;

	output = malloc(sizeof(linked_list));
	output->value = value;
	output->next = NULL;
	output->previous = NULL;
	return output;
}

void free_linked_list(linked_list *list, void (*free_value)(void *)){
	linked_list *next_list;

	while(list){
		next_list = list->next;
		free_value(list->value);
		free(list);
		list = next_list;
	}
}

void add_linked_list(linked_list **list, linked_list *element){
	(*list)->next = element;
	element->previous = *list;
	*list = element;
}

