// Need this to use the getline C function on Linux. Works without this on MacOs. Not tested on Windows.
#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "token.h"
#include "queue.h"
#include "stack.h"
/*-----------------------------------------------------------------------------------------------------------------------------------------*/

/* Cette fonction retourne un booléen indiquant si un caractère est un symbole ou une parenthèse */
bool isSymbol(char c){
	return c == '+' || c =='-' || c == '*' ||
	 c == '/' || c == '^' || c == '(' || c == ')';
}
/*-----------------------------------------------------------------------------------------------------------------------------------------*/
/* Cette fonction prend une chaîne de caractères représentant une expression mathématique.
   Elle retourne un pointeur vers une file (Queue) contenant les Tokens. */
Queue *stringToTokenQueue(const char*expression){
	Queue *token_queue =create_queue();
	const char *curpos = expression; // Initialise le pointeur de caractère courant pour pointer vers le début de la chaîne.
	int lengTok = 1; // Taille du token courant

	while (*(curpos)!= '\0') // La boucle continue jusqu'à la fin de la chaîne
	{
		while (*(curpos)=='\n' || *(curpos)==' ' ) // On ignore les espaces et retours à la ligne
		{
			++curpos;
		}
		if (*(curpos)!= '\0'){
			const char * char_num_actual = curpos +1; // Pointeur vers le caractère suivant
			bool A;
			// Si le caractère actuel et le suivant ne sont pas des symboles, c'est un nombre
			A = !isSymbol(*(curpos)) && !isSymbol(*(char_num_actual));
			if (A){
				// Tant qu'on n'est pas à la fin de la chaîne et qu'on ne rencontre pas de symbole, on agrandit le token
				while (! isSymbol( *(char_num_actual)) && *(char_num_actual) != '\0')
				{
					++lengTok;
					++char_num_actual;
				}
			}
			// Création du token et ajout à la file
			Token *token_actual = create_token_from_string(curpos, lengTok);
			queue_push(token_queue,token_actual);
			curpos +=lengTok;
			lengTok=1; // Réinitialisation de la taille du prochain token
		}
	}
	return token_queue;
}
/*-----------------------------------------------------------------------------------------------------------------------------------------*/
Queue *shutingYard(Queue* infix){
	Queue *resQueue = create_queue();//This is the output Queue that contains the postfixed tokens
	Stack *operatorStack = create_stack(30); //here we are creating the stack for containing the operatiors and parantheses
	while (! queue_empty(infix)) 

	{
		Token *t =(Token *)queue_top(infix);
		queue_pop(infix);
		/*if our token is a number, then it is directly put into the 
		file de sortie*/
		if (token_is_number(t))
		{
			queue_push(resQueue,t);
		}else if (stack_empty(operatorStack))
		{
			stack_push(operatorStack, t);
		}
		/* sinon, on traite les Tokens en fonction de leur type */
		else{
			if (token_is_operator(t))
			{
				if (!stack_empty(operatorStack))
				/*si la pile d'opérateurs n'est pas vide 
				 on compare la priorité de l'opérateur courant avec celle de l'opérateur en haut de pile, en fonction de leur associativité */
				{
					bool compare_operator = (!stack_empty(operatorStack))
					&& (!token_is_parenthesis(stack_top(operatorStack)) 
					&& (token_operator_priority(stack_top(operatorStack))>= token_operator_priority(t))
					&& token_operator_leftAssociative(t));
					/* tant que l'opérateur en haut de pile a une priorité supérieure ou égale 
					 on dépile l'opérateur et on le met dans la file de sortie  */
					while (compare_operator)
					{
						queue_push(resQueue, stack_top(operatorStack));
						stack_pop(operatorStack);
					    compare_operator = (!stack_empty(operatorStack))
					    && (!token_is_parenthesis(stack_top(operatorStack)) 
					    && (token_operator_priority(stack_top(operatorStack))>= token_operator_priority(t))
					    && token_operator_leftAssociative(t));
					}
					stack_push(operatorStack, t);
					
				}
				
			}
			/* si le Token est une parenthèse ouvrante on empile la parenthèse ouvrante */
			if (token_is_parenthesis(t) && token_parenthesis(t) == '(')
			{
				stack_push(operatorStack,t);
			}
			/* si le Token est une parenthèse fermante  on regarde le Token en haut de pile
			si c'est une parenthèse, on enregistre son symbole et si c'est un opérateur, on enregistre son symbole  */
			if(token_is_parenthesis(t)&& token_parenthesis(t)== ')'){
				char valueToken;
				if (token_is_parenthesis(stack_top(operatorStack)))
				{
					valueToken = token_parenthesis(stack_top(operatorStack));
				}else{
					valueToken= token_operator(stack_top(operatorStack));
				}
				/*si le Token est une une parenthèse ouvrante,
				la fonction entre dans cette boucle qui continue tant que le premier élément de operatorStack
				n'est pas une parenthèse fermante. À chaque itération de la boucle, la fonction extrait le premier élément de operatorStack et l'ajoute sur outputQueue.À chaque itération de la boucle
				, la fonction extrait le premier élément de operatorStack et l'ajoute sur resQueue. */
				while (valueToken != '(')
				{
					queue_push(resQueue,stack_top(operatorStack));
					stack_pop(operatorStack);
					if (stack_empty(operatorStack))
					{
						/*Si operatorStack devient vide avant qu'une parenthèse fermante ne soit trouvée, 
						la fonction imprime un message d'erreur*/
						fprintf(stderr,"Parenthese incompatible\n");
						exit(1);
					}
					if (token_is_parenthesis(stack_top(operatorStack)))
					{
						valueToken = token_parenthesis(stack_top(operatorStack));
					}else if(token_is_operator(stack_top(operatorStack))){
						valueToken = token_operator(stack_top(operatorStack));
					}
					
					
				}
				Token* parenthesis_error = (Token*)stack_top(operatorStack);
                delete_token(&parenthesis_error);

				
				delete_token(&t);
				stack_pop(operatorStack);
				
			}
			
		}
		
		
	}
	/*Après avoir traité tous les Token de la file infix,
	 la fonction entre dans une autre boucle qui continue tant que operatorStack n'est pas vide.
	vérifier que l'opérateur en haut n'est pas une parenthèse. */
	while (!stack_empty(operatorStack))
	{
		
		if (token_is_parenthesis(stack_top(operatorStack))){
			fprintf(stderr, "parenthese incompatible \n");
			exit(2);
		}
		queue_push(resQueue, stack_top(operatorStack));
		stack_pop(operatorStack);
	}
	/*on supprime le operatorStack t on renvoie le resQueue, 
	qui doit contenir l'expression postfixée équivalente à l'expression infixe d'origine*/
	delete_stack(&operatorStack);
	return resQueue;
	
	
}
/*-----------------------------------------------------------------------------------------------------------------------------------------*/
/*La fonction récupère l'opérateur à appliquer sur les deux opérandes.
La fonction récupère la valeur numérique de chaque opérande à l'aide de la fonction tokenGetValue.
 we have a switch for different cases : Si l'opérateur est une addition (+), la fonction ajoute les deux opérandes.
 Si l'opérateur est une soustraction (-), la fonction soustrait le deuxième opérande du premier.
 Si l'opérateur est une multiplication (*), la fonction multiplie les deux opérandes.
 Si l'opérateur est une division (/), la fonction divise le premier opérande par le deuxième.
 Si l'opérateur est une puissance (^), la fonction calcule la première opérande à la puissance de la deuxième opérande.*/
Token *evaluateOperator(Token *arg1,Token *op , Token *arg2){

	char char_op = token_operator(op);
	float op1 = token_value(arg1);
	float op2 = token_value(arg2);
	float float_res =0;
	switch(char_op){
		case '+': /*Si l'opérateur est une addition (+), la fonction ajoute les deux opérandes. */
			float_res = op1+ op2;
			break;
			case '-': /*Si l'opérateur est une soustraction (-), la fonction soustrait le deuxième opérande du premier.*/
				float_res = op1- op2;
				break;
			case '*': /*Si l'opérateur est une multiplication (*), la fonction multiplie les deux opérandes. */
				float_res = op1* op2;
				break;
			case '/': /*Si l'opérateur est une division (/), la fonction divise le premier opérande par le deuxième. */
				float_res = op1 / op2;
				break;
			case '^': /*Si l'opérateur est une puissance (^), la fonction calcule la première opérande à la puissance de la deuxième opérande. */
				float_res = pow(op1,op2);
				break;
	}
	return create_token_from_value(float_res);

}

/*-----------------------------------------------------------------------------------------------------------------------------------------*/

float evaluateExpression(Queue* postfix){
	/*Creation d'une pile vide de taille 1000 qui sera utilisée
	 pour stocker les opérandes et les résultats intermédiaires de l'évaluation.*/
	Stack* stackEval  = create_stack(1000); 
	while (! queue_empty(postfix)) { 
		Token * t = (Token*)queue_top(postfix);
		queue_pop(postfix);
		Token * arg1 = NULL;
		Token * arg2 = NULL;
		Token * res = NULL;
		/*Si l'élément retiré est un opérateur, 
		la fonction calcule le résultat de l'opération en utilisant 
		les deux opérandes précédemment stockés dans la pile stackEval. */
		if(token_is_operator(t)){ 
			/*Récupère les deux opérandes qui ont été précédemment ajoutés sur la pile stackEvaluation 
			et puis les supprime de la pile. */
			arg2 = (Token*)stack_top(stackEval); 
			stack_pop(stackEval); 
			arg1 =(Token*) stack_top(stackEval);
			stack_pop(stackEval);
			res = evaluateOperator(arg1, t, arg2);
 
            delete_token(&arg1);
            delete_token(&arg2);
            delete_token(&t);

			stack_push(stackEval, res); 
			/*Si l'élément retiré est un nombre, 
			la fonction stocke simplement le Token contenant le nombre dans la pile stackEval. */
		}else if(token_is_number(t)){ 
				stack_push(stackEval,t);
		}
	} 
	 /*Après avoir parcouru tous les éléments de la file postfix, 
	 la fonction récupère le résultat final,Le Token en haut de la pile stackEvaluation est ensuite supprimé 
	 et la pile stackEvaluation est supprimée aussi. */
	float res_float = token_value(stack_top(stackEval));
	Token * last_token_to_delete = (Token*)stack_top(stackEval);
	delete_token(&last_token_to_delete);

	stack_pop(stackEval); 
	delete_stack(&stackEval); 
	return res_float; 
}
/*-----------------------------------------------------------------------------------------------------------------------------------------*/
/** 
 * Utilities function to print the token queues
 */
void print_token(const void* e, void* user_param);
void print_queue(FILE* f, Queue* q);

/*-----------------------------------------------------------------------------------------------------------------------------------------*/
/** 
 * Function to be written by students
 */
void computeExpressions(FILE* input) {
    char *buffer = NULL;
    size_t size = 0;
 /*On parcourt le fichier ligne par ligne jusqu'à ce qu'il n'y ait plus de lignes à lire et
  on lit une ligne du fichier et stocke le résultat dans buffer.
   La fonction renvoie -1 si la fin du fichier est atteinte.*/
    while (getline(&buffer, &size, input) != -1) {
        // Skip empty or whitespace-only lines
        bool is_blank = true;
        for (size_t i = 0; buffer[i] != '\0'; i++) {
            if (buffer[i] != ' ' && buffer[i] != '\n' && buffer[i] != '\t') {
                is_blank = false;
                break;
            }
        }
        if (is_blank) continue;

        printf("\nInput: %s", buffer);

        Queue *infix_queue = stringToTokenQueue(buffer);
        printf("Tokens: ");
        print_queue(stdout, infix_queue);
        printf("\n");

        printf("Postfix  : ");
        Queue *postfix_queue = shutingYard(infix_queue);
        print_queue(stdout, postfix_queue);
        printf("\n");

        printf("Evaluate : ");
        float evaluate = evaluateExpression(postfix_queue);
        printf("%f\n", evaluate);

        delete_queue(&postfix_queue);
        delete_queue(&infix_queue);
    }

    free(buffer);
}

/*-----------------------------------------------------------------------------------------------------------------------------------------*/
/** Main function for testing.
 * The main function expects one parameter that is the file where expressions to translate are
 * to be read.
 *
 * This file must contain a valid expression on each line
 *
 */
int main(int argc, char** argv){
	if (argc<2) {
		fprintf(stderr,"usage : %s filename\n", argv[0]);
		return 1;
	}
	
	FILE* input = fopen(argv[1], "r");

	if ( !input ) {
		perror(argv[1]);
		return 1;
	}

	computeExpressions(input);

	fclose(input);
	return 0;
}
 
void print_token(const void* e, void* user_param) {
	FILE* f = (FILE*)user_param;
	Token* t = (Token*)e;
	token_dump(f, t);
}

void print_queue(FILE* f, Queue* q) {
	fprintf(f, "(%d) --  ", queue_size(q));
	queue_map(q, print_token, f);
}