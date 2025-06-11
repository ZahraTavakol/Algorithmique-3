/*-----------------------------------------------------------------*/
/*
 Licence Informatique - Structures de données
 Mathias Paulin (Mathias.Paulin@irit.fr)
 
 TAD de gestion d'éléments syntaxique pour les expressions arithmétiques
 
 */
/*-----------------------------------------------------------------*/
#ifndef __TOKEN_H__
#define __TOKEN_H__

#include <stdio.h>
#include <stdbool.h>

/** Opaque definition of type Token and ptrToken*/
typedef struct s_Token Token;
typedef Token* ptrToken;

/** Create a Token from the string designed by s, taking only the lg first chars of the string */
Token* create_token_from_string(const char* s, int lg);

Token* create_token_from_value(float v);

/** Delete the give token.
 Free the memory used by the token and set the pointer to NULL.
*/
void delete_token(ptrToken* t);

/** Test if a token represents a number.
 @param t : the token to test
 @return true if the given token represent a number, else false
 */
bool token_is_number(const Token* t);

/** Test if a token represents a binary operator.
 @param t : the token to test
 @return true if the given token represent a binary operator, else false
 @note Currently supported binary operators are +,-,*,/,^.
 */
bool token_is_operator(const Token* t);

/** Test if a token represents a parenthesis.
 @param t : the token to test
 @return true if the given token represent a '(' or ')', else false
 */
bool token_is_parenthesis(const Token* t);

/** get the value of a number token.
 @param t : the token to examine
 @return the value stored in the token
 @pre token_is_number(t) == true
*/
float token_value(const Token* t);

/** Get the operator symbol of a binary operator token.
 @param t : the token to examine
 @return the operator symbol (@see tokenIsOperator)
 @pre token_is_operator(t) == true
 */
char token_operator(const Token* t);

/** Get the parenthesis symbol of a  token.
 @param t : the token to examine
 @return the parenthesis symbol
 @pre token_is_parenthesis(t) == true
 */
char token_parenthesis(const Token* t);

/** Get the priority of a binaray operator token.
 @param t : the token to examine
 @return 1 if operator is + or -, 2 if operator is * or /, 3 if operator is ^.
 @pre token_is_operator(t) == true
 @see token_is_operator
 */
int token_operator_priority(const Token* t);

/** Is the operator left associative.
 @param t : the token to examine
 @return true if operator is left associative.
 @pre token_is_operator(t) == true
 @see token_is_operator
 */
bool token_operator_leftAssociative(const Token* t);

/** Dump the token to the given file */
void token_dump(FILE* f, const Token* t);
#endif

