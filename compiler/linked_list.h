typedef struct linked_list linked_list;

struct linked_list{
	linked_list *next;
	linked_list *previous;
	void *value;
};

linked_list *create_linked_list(void *value);

void free_linked_list(linked_list *list, void (*free_value)(void *));

void add_linked_list(linked_list **list, linked_list *element);

