/*-----------------------------------------------------------------*/
/*
 Licence Informatique - Structures de données
 Mathias Paulin (Mathias.Paulin@irit.fr)
 
 Implantation du TAD Queue étudié en cours.
 
 */
/*-----------------------------------------------------------------*/
#include "queue.h"
#include <assert.h>
#include <stdlib.h>

/* Full definition of the queue structure */
typedef struct s_internalQueue {
	const void* value;
	struct s_internalQueue* next;
} InternalQueue;

struct s_queue{
	InternalQueue* head;
	InternalQueue* tail;
	unsigned int size;
};

Queue* create_queue(void){
	Queue* q = calloc(1, sizeof(Queue));
	return(q);
}

void delete_queue(ptrQueue *q) {
	InternalQueue* toDelete = (*q)->head;
	while (toDelete) {
		InternalQueue* f = toDelete;
		toDelete = toDelete->next;
		free (f);
	}
	free(*q);
	*q = NULL;
}

Queue* queue_push(Queue* q, const void* v){
	InternalQueue* new = calloc(1, sizeof(InternalQueue));
	new->value = v;
	InternalQueue** insert_at = (q->size ? &(q->tail->next) : &(q->head));
	*insert_at = new;
	q->tail = new;
	++(q->size);
	return (q);
}

Queue* queue_pop(Queue* q){
	assert (!queue_empty(q));
	InternalQueue* old = q->head;
	q->head = q->head->next;
	--(q->size);
	free (old);
	return (q);
}

const void* queue_top(const Queue* q){
	assert (!queue_empty(q));
	return (q->head->value);
}

bool queue_empty(const Queue* q){
	return (queue_size(q) == 0);
}

unsigned int queue_size(const Queue* q) {
	return q->size;
}

void queue_map(const Queue* q, QueueMapOperator f, void* user_param) {
	for (InternalQueue *c = q->head; c ; c = c->next)
		f(c->value, user_param);
}

