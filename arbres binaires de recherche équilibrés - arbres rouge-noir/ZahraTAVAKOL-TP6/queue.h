/*-----------------------------------------------------------------*/
/*
 Licence Informatique - Structures de données
 Mathias Paulin (Mathias.Paulin@irit.fr)
 
 Implantation du TAD Queue étudié en cours.
 
 */
/*-----------------------------------------------------------------*/
#ifndef __QUEUE__H__
#define __QUEUE__H__
#include <stdio.h>
#include <stdbool.h>

/* Opaque definition iof the type Queue */
typedef struct s_queue Queue;
typedef Queue* ptrQueue;

/** Constructor : build an empty queue 
	queue : -> Queue
*/
Queue* create_queue(void);

/** Delete the queue.
 */
void delete_queue(ptrQueue* q);

/** Constructor : add an element to the queue 
	queue_push : Queue x int -> Queue
	@note : side effect on the queue q
*/
Queue* queue_push(Queue* q, const void* v);

/** Operator : pop an element from the queue 
	queue_pop : Queue -> Queue
	@pre !queue_empty(q)
*/
Queue* queue_pop(Queue* q);

/** Operator : acces to the frist element of the queue
	queue_top : Queue -> int
	@pre !queue_empty(q)
*/
const void *queue_top(const Queue* q);

/** Operator : is the queue empty ?
	queue_empty : Queue -> boolean
*/
bool queue_empty(const Queue* q);

/** Operator : size of the queue ?
 size : Queue -> int
 */
unsigned int queue_size(const Queue* q);

/** Function type for mappable operators */
typedef void (*QueueMapOperator)(const void* elem, void* user_param);

/** Maps an operator on each element of the queue, with user defined global parameters
 */
void queue_map(const Queue* q, QueueMapOperator f, void* user_param);

#endif

