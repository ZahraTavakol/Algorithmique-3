/*-----------------------------------------------------------------*/
/*
 Licence Informatique - Structures de données
 Mathias Paulin (Mathias.Paulin@irit.fr)
 
 Implantation du TAD List vu en cours.
 */
/*Zahra TAVAKOL 
Numero Etudiant:22300070*/
/*-----------------------------------------------------------------*/

/*Zahra TAVAKOL 
Numero Etudiant:22300070*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "list.h"
// SubList definition



typedef struct s_LinkedElement {
	int value;
	struct s_LinkedElement* previous;
	struct s_LinkedElement* next;
} LinkedElement;



/* Use of a sentinel for implementing the list :
 The sentinel is a LinkedElement* whose next pointer refer always to the head of the list and previous pointer to the tail of the list
 */
struct s_List {
	LinkedElement* sentinel;
	int size;
};
typedef struct {
    LinkedElement* head;
    LinkedElement* tail;
    
} SubList;


static void list_split(SubList l, SubList* left, SubList* right);
static SubList list_merge(SubList left, SubList right, OrderFunctor f);
static SubList list_mergesort(SubList l, OrderFunctor f);

/*-----------------------------------------------------------------*/

List* list_create(void) {//Because i have two malloc because 
    List* l = (List*)malloc(sizeof(List));
    if (!l) return NULL;

    l->sentinel = (LinkedElement*)malloc(sizeof(LinkedElement));
    if (!l->sentinel) {
        free(l);
        return NULL;
    }

    l->sentinel->next = l->sentinel;
    l->sentinel->previous = l->sentinel;
    l->size = 0;

    return l;
}

/*-----------------------------------------------------------------*/

List* list_push_back(List* l, int v) {
    if (!l) return NULL;

    LinkedElement* new_elem = (LinkedElement*)malloc(sizeof(LinkedElement));
    if (!new_elem) return l;

    new_elem->value = v;

    LinkedElement* last = l->sentinel->previous;
    new_elem->next = l->sentinel;
    new_elem->previous = last;

    last->next = new_elem;
    l->sentinel->previous = new_elem;

    l->size++;
    return l;
}


/*-----------------------------------------------------------------*/

void list_delete(ptrList* l) {
    //if (!l || !*l) return;

    LinkedElement* current = (*l)->sentinel->next;
    while (current != (*l)->sentinel) {
        LinkedElement* temp = current;
        current = current->next;
        free(temp);
    }

    free((*l)->sentinel);
    free(*l);
    *l = NULL;
}


/*-----------------------------------------------------------------*/

List* list_push_front(List* l, int v) {
    if (!l) return NULL;

    LinkedElement* new_elem = (LinkedElement*)malloc(sizeof(LinkedElement));
    if (!new_elem) return l;

    new_elem->value = v;

    LinkedElement* first = l->sentinel->next;
    new_elem->next = first;
    new_elem->previous = l->sentinel;

    l->sentinel->next = new_elem;
    first->previous = new_elem;

    l->size++;
    return l;
}


/*-----------------------------------------------------------------*/

int list_front(const List* l) {
    assert(l && l->sentinel->next != l->sentinel);  // Ensure the list is valid and not empty
    return l->sentinel->next->value;  // Return the value of the first element
}


/*-----------------------------------------------------------------*/

int list_back(const List* l) {
    assert(l && l->sentinel->previous != l->sentinel);  // Ensure the list is valid and not empty
    return l->sentinel->previous->value;  // Return the value of the last element
}

/*-----------------------------------------------------------------*/

List* list_pop_front(List* l) {
    if (!l || l->sentinel->next == l->sentinel) return l;  

    LinkedElement* first = l->sentinel->next;  
    l->sentinel->next = first->next;  
    first->next->previous = l->sentinel;  

    free(first);  
    l->size--;  
    return l;
}


/*-----------------------------------------------------------------*/
List* list_pop_back(List* l) {
    if (!l || l->sentinel->previous == l->sentinel) return l;  

    LinkedElement* last = l->sentinel->previous;  
    l->sentinel->previous = last->previous;  
    last->previous->next = l->sentinel;  

    free(last);  
    l->size--;  
    return l;
}

/*-----------------------------------------------------------------*/

List* list_insert_at(List* l, int p, int v) {
    if (!l || p < 0 || p > l->size) return l; 

    LinkedElement* new_elem = (LinkedElement*)malloc(sizeof(LinkedElement));
    if (!new_elem) return l; 
    new_elem->value = v;

    LinkedElement* current = l->sentinel;
    for (int i = 0; i < p; i++) {
        current = current->next; 
    }

    new_elem->next = current->next;
    new_elem->previous = current;
    current->next->previous = new_elem;
    current->next = new_elem;

    l->size++; 
    return l;
}

/*-----------------------------------------------------------------*/
List* list_remove_at(List* l, int p) {
    if (!l || p < 0 || p >= l->size) return l; 

    LinkedElement* current = l->sentinel->next;
    for (int i = 0; i < p; i++) {
        current = current->next;
    }

    current->previous->next = current->next;
    current->next->previous = current->previous;
    free(current);

    l->size--; 
    return l;
}


/*-----------------------------------------------------------------*/
int list_at(const List* l, int p) {
    assert(l && p >= 0 && p < l->size); 

    LinkedElement* current = l->sentinel->next;
    for (int i = 0; i < p; i++) {
        current = current->next;
    }

    return current->value; 
}


/*-----------------------------------------------------------------*/

bool list_is_empty(const List* l) {
    return l->sentinel->next == l->sentinel;
}


/*-----------------------------------------------------------------*/

int list_size(const List* l) {
    return l->size;
}


/*-----------------------------------------------------------------*/
List* list_map(List* l, ListFunctor f, void* environment) {
    

    LinkedElement* current = l->sentinel->next;
    while (current != l->sentinel) {
        current->value = f(current->value, environment);
        current = current->next;
    }

    return l;
}


/*-----------------------------------------------------------------*/

List* list_sort(List* l, OrderFunctor f) {
    if (list_is_empty(l)) {
        return l;
    }
    // Création d’une sous-liste sans la sentinelle pour appliquer le tri

    SubList sublist;
    sublist.head = l->sentinel->next;
    sublist.tail = l->sentinel->previous;

    // Déconnexion de la sentinelle avant le tri
    sublist.head->previous = NULL;
    sublist.tail->next = NULL;

    // Tri fusion récursif de la sous-liste
    sublist = list_mergesort(sublist, f);

    // Reconnexion de la liste triée avec la sentinelle
    if (sublist.head && sublist.tail) {
        l->sentinel->next = sublist.head;
        l->sentinel->previous = sublist.tail;
        sublist.head->previous = l->sentinel;
        sublist.tail->next = l->sentinel;
    }

    return l;
}




/*-----------------------------------------------------------------*/
static void list_split(SubList l, SubList* left, SubList* right) {
    LinkedElement* slow = l.head;
    LinkedElement* fast = l.head;

// Utilisation de la technique "tortue-lievre" pour trouver le milieu
    while (fast != l.tail && fast->next != l.tail) {
        slow = slow->next;
        fast = fast->next->next;
    }

    // Création de la sous-liste gauche
    *left = (SubList){ .head = l.head, .tail = slow };
    // Création de la sous-liste droite
    *right = (SubList){ .head = slow->next, .tail = NULL };
    // Déconnexion entre les deux sous-listes
    if (right->head != NULL) {
        right->head->previous = NULL;
        slow->next = NULL;

        // Recherche de la vraie fin de la sous-liste droite
        LinkedElement* cur = right->head;
        while (cur->next != NULL) {
            cur = cur->next;
        }
        right->tail = cur;
    } else {
        right->tail = NULL;
    }
}



/*-----------------------------------------------------------------*/
static SubList list_merge(SubList leftlist, SubList rightlist, OrderFunctor f) {
    LinkedElement* left = leftlist.head;
    LinkedElement* right = rightlist.head;
    LinkedElement* mergedHead = NULL;
    LinkedElement* mergedTail = NULL;

    // Fusion des deux listes en respectant l'ordre donné par f
    while (left && right) {
        LinkedElement* next;

        if (f(left->value, right->value)) {
            next = left;
            left = left->next;
        } else {
            next = right;
            right = right->next;
        }
        // Mise à jour des pointeurs next/previous
        next->previous = mergedTail;
        next->next = NULL;

        if (mergedTail) {
            mergedTail->next = next;
        } else {
            mergedHead = next;
        }

        mergedTail = next;
    }

    // Ajout des éléments restants de la liste gauche
    while (left) {
        LinkedElement* next = left;
        left = left->next;

        next->previous = mergedTail;
        next->next = NULL;

        if (mergedTail) {
            mergedTail->next = next;
        } else {
            mergedHead = next;
        }

        mergedTail = next;
    }

   // Ajout des éléments restants de la liste droite
    while (right) {
        LinkedElement* next = right;
        right = right->next;

        next->previous = mergedTail;
        next->next = NULL;

        if (mergedTail) {
            mergedTail->next = next;
        } else {
            mergedHead = next;
        }

        mergedTail = next;
    }
    // Retour de la nouvelle sous-liste fusionnée
    SubList merged = { .head = mergedHead, .tail = mergedTail };
    return merged;
}


/*-----------------------------------------------------------------*/
static SubList list_mergesort(SubList l, OrderFunctor f) {
    // Cas de base : liste vide ou à un seul élément (déjà triée)
    if (l.head == NULL || l.head->next == NULL) {
        return l;
    }

    SubList left, right;
    // Division de la sous-liste en deux moitiés
    list_split(l, &left, &right);

    // Appel récursif sur chaque moitié
    left = list_mergesort(left, f);
    right = list_mergesort(right, f);

    // Fusion des deux moitiés triées
    return list_merge(left, right, f);
}

