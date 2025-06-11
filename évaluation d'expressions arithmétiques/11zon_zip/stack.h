/*-----------------------------------------------------------------*/
/*
 Licence Informatique - Structures de données
 Mathias Paulin (Mathias.Paulin@irit.fr)
 
 Implantation du TAD Stack étudié en cours.
 
 */
/*-----------------------------------------------------------------*/
#ifndef __STACK_H__
#define __STACK_H__
#include <stdio.h>
#include <stdbool.h>

/** Opaque definition of type Stack */
typedef struct s_stack Stack;
typedef Stack* ptrStack;

/** Create and initialize an empty stack.
 * @param max_size : max number of element that the stack will contains.
 * This operator creates an empty stack of the required capacity.
 * @note If max_size is 0, the capacity of the stack will be fixed by the implementation and is a least 32.
 */
Stack* create_stack(int max_size);

/** Delete the stack.
 */
void delete_stack(ptrStack *s);

/** Push a value on the stack.
 * @param s : the Stack to push on.
 * @param e : the value to push on the stack.
 * @return the modified stack.
 * @note implemented using side effect on the stack. After execution, s is the same than the returned stack.
 */
Stack* stack_push(Stack* s, const void * e);

/** Test if a stack is empty.
 * @param s : the Stack to test.
 * @return : true if the stack is empty, else false.
 */
bool stack_empty(const Stack* s);

/** Pop a value from the stack.
 * @pre pop(s) defined if \f$\neg\f$ empty(s)
 * @param s : the Stack to pop from.
 * @return the modified stack.
 * @note implemented using side effect on the stack. After execution, s is the same than the returned stack.
 */
Stack* stack_pop(Stack* s);

/** Return the top of a stack.
 * @pre top(s) defined if \f$\neg\f$ empty(s)
 * @param s : the Stack to examine.
 * @return the value at the top of the stack.
 */
const void* stack_top(const Stack* s);

/** Return the number of element (the size) in the stack.
 * @param s : the Stack to examine.
 * @return the number of element in the stack.
*/
unsigned int stack_size(const Stack* s);

/** Return true if the stack will overflow on the next push.
 * @param s : the Stack to examine.
 * @return true if the number of element in the stack is equal to the stack capacity, else false.
 */
bool stack_overflow(const Stack* s);

/** Function type for mappable operators */
typedef void (*StackMapOperator)(const void* elem, void* user_param);

/** Maps an operator on each element of the stack, with user defined global parameters
 */
void stack_map(const Stack* s, StackMapOperator f, void* user_param);

#endif

