#include "token.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>


/* Enum type that defines the token type */
typedef enum t_Token {number, binary_operator, parenthesis} TokenType;

struct s_Token {
	TokenType type;
	union {
		float number;
		char symbol;
	} value;
};

Token* create_token_from_string(const char* s, int lg) {
	(void)lg;
	Token* t = malloc(sizeof(Token));
	if (isdigit(*s) || *s == '.') {
		t->type = number;
		t->value.number = strtof(s, NULL);
	} else if (*s == '(' || *s == ')') {
		t->type = parenthesis;
		t->value.symbol = *s;
	} else {
		t->type = binary_operator;
		t->value.symbol = *s;
	}
	return t;
}

Token* create_token_from_value(float v) {
	Token* t = malloc(sizeof(Token));
	t->type = number;
	t->value.number = v;
	return t;
}

void delete_token(ptrToken* t) {
	free (*t);
	*t = NULL;
}

bool token_is_number(const Token* t) {
	return t->type == number;
}

bool token_is_operator(const Token* t) {
	return t->type == binary_operator;
}

bool token_is_parenthesis(const Token* t) {
	return t->type == parenthesis;
}

float token_value(const Token* t) {
	assert( token_is_number(t) );
	return t->value.number;
}

char token_operator(const Token* t) {
	assert(token_is_operator(t) );
	return t->value.symbol;
}

char token_parenthesis(const Token* t) {
	assert( token_is_parenthesis(t) );
	return t->value.symbol;
}

int token_operator_priority(const Token* t){
	assert( token_is_operator(t) );
	switch( t->value.symbol ) {
		case '+':
		case '-':
			return 1;
		case '*':
		case '/':
			return 2;
		case '^':
			return 3;
		default:
			return -1;
	}
}

bool token_operator_leftAssociative(const Token* t){
	assert( token_is_operator(t) );
	switch( t->value.symbol ) {
		case '+':
		case '-':
		case '*':
		case '/':
			return true;
		case '^':
			return false;
		default:
			return false;
	}
}

void token_dump(FILE* f, const Token* t) {
	if (t->type == number)
		fprintf(f, "%f ", t->value.number);
	else
		fprintf(f, "%c ", t->value.symbol);
}

