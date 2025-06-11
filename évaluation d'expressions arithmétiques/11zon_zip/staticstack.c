/*-----------------------------------------------------------------*/
/*
 Licence Informatique - Structures de données
 Mathias Paulin (Mathias.Paulin@irit.fr)
 
 Implantation du TAD Stack étudié en cours.
 
 */
/*-----------------------------------------------------------------*/
#include "stack.h"
#include <assert.h>
#include <stdlib.h>
#define STACK_SIZE 4

/* Full definition of the s_stack structure */
struct s_stack {
	int top;
	int capacity;
	const void** stack; // array of const void *
};

Stack* create_stack(int max_size) {
	Stack* s;
	size_t capacity = (max_size > 0 ? max_size : STACK_SIZE);
	s = malloc(sizeof(struct s_stack) + sizeof(void *) * capacity);
	s->stack = (const void**)(s+1);
	s->capacity = capacity;
	s->top=-1;
	return (s);
}

void delete_stack(ptrStack* s) {
	free (*s);
	*s = NULL;
}

Stack* stack_push(Stack* s, const void* e) {
	assert(!stack_overflow(s));
	s->stack[++(s->top)] = e;
	return(s);
}

bool stack_empty(const Stack* s) {
	return (s->top == -1);
}

Stack* stack_pop(Stack* s) {
	assert(!stack_empty(s));
	--(s->top);
	return(s);
}

const void* stack_top(const Stack* s) {
	assert(!stack_empty(s));
	return (s->stack[s->top]);
}

unsigned int stack_size(const Stack* s) {
	return s->top + 1;
}

bool stack_overflow(const Stack* s){
	return s->top + 1 == s->capacity;
}

void stack_map(const Stack* s, StackMapOperator f, void* user_param) {
	for (int i=s->top; i>=0; --i)
		f(s->stack[i], user_param);
}

