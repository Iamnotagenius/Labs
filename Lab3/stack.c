#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct node;
typedef struct node {
	void *current;
	struct node *prev;
} stack_node;

typedef struct {
	size_t size;
	stack_node *last;
	void (*free_func)(void *);
} stack;

stack *create_stack(size_t size, void (*free_func)(void *)) {
	stack *new = malloc(sizeof(stack));
	new->size = size;
	new->last = NULL;
	new->free_func = free_func;
	return new;
}

void delete_stack(stack *s) {
	while (s->last != NULL) {
		stack_node *tmp = s->last;
		s->last = s->last->prev;

		if (s->free_func != NULL)
			s->free_func(tmp->current);
		free(tmp->current);
		free(tmp);
	}
}

void push(stack *s, void *element) {
	stack_node *new = malloc(sizeof(stack_node));
	new->current = malloc(s->size);
	memcpy(new->current, element, s->size);
	new->prev = s->last;
	s->last = new;
}

int pop(stack *s, void *buf) {
	if (s->last == NULL) 
		return 0;

	memcpy(buf, s->last->current, s->size);
	stack_node *tmp = s->last;
	s->last = s->last->prev;

	if (s->free_func != NULL)
		s->free_func(tmp->current);
	free(tmp->current);
	free(tmp);
	return 1;
}
