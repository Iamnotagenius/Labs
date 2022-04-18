#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"
 
queue *create_queue(size_t size) {
    queue *new = malloc(sizeof(queue));
    new->size = size;
    new->head = NULL;
    new->tail = NULL;
	new->length = 0;
    return new;
}
 
void delete_queue(queue *s) {
    while (s->tail != NULL) {
        queue_node *tmp = s->tail;
        s->tail = s->tail->next;   
        free(tmp->item);
        free(tmp);
    }
}
 
void push_q(queue *s, void *element) {
    queue_node *new = malloc(sizeof(queue_node));
    new->item = malloc(s->size);
    memcpy(new->item, element, s->size);
    new->next = NULL;
	new->previous = s->head;
 
    if (s->head == NULL) {
        s->tail = new;
        s->head = new;
    }
    else {
        s->head->next = new;
        s->head = new;
    }
	s->length++;
}
 
int pop_q(queue *s, void *buf) {
	if (s->tail == NULL) 
		return 0;

	if (buf != NULL)
		memcpy(buf, s->tail->item, s->size);
    queue_node *tmp = s->tail;
    s->tail = s->tail->next;
 
    if (s->tail == NULL)
        s->head = NULL;
 
    free(tmp->item);
    free(tmp);
	s->length--;
    return 1;
}

void *head_q(queue *q) {
	return q->head->item;
}

void *tail_q(queue *q) {
	return q->tail->item;
}
