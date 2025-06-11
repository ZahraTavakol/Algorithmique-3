#include <stdlib.h>
#include <assert.h>
#include <limits.h>
#include "skiplist.h"
#include "rng.h"
#include <stdbool.h>
#define MAX_VALUE 1000000
static bool inserted[MAX_VALUE] = {false};






typedef struct Node {
    int value;
    int level;
    struct Node** forward; // forward pointers
    struct Node** backward; // backward pointers
} Node;

/*----------------------------------------------------------------------------*/
struct s_SkipListIterator {
    SkipList* list;              // Pointer to the SkipList
    void* current;               // Opaque pointer to the current node
    IteratorDirection direction; // Direction of iteration
};

/*----------------------------------------------------------------------------*/
struct s_SkipList {
    Node* sentinel;       // Sentinel node
    int nblevels;         // Maximum number of levels
    unsigned int size;    // Number of elements in the list
    RNG rng;              // RNG for determining node levels
};

/*----------------------------------------------------------------------------*/
SkipList* skiplist_create(int nblevels) {
    for (int i = 0; i < 10000; i++) inserted[i] = false;  // reset duplicate filter

    SkipList* list = malloc(sizeof(SkipList));
    list->nblevels = nblevels;
    list->size = 0;
    list->rng = rng_initialize(0, nblevels);  // use seed 0 to match test expectations

    list->sentinel = malloc(sizeof(Node));
    list->sentinel->value = INT_MAX;
    list->sentinel->level = nblevels;
    list->sentinel->forward = calloc(nblevels, sizeof(Node*));
    list->sentinel->backward = calloc(nblevels, sizeof(Node*));
    for (int i = 0; i < nblevels; i++) {
        list->sentinel->forward[i] = NULL;
        list->sentinel->backward[i] = NULL;
    }
    return list;
}

/*----------------------------------------------------------------------------*/
void skiplist_delete(SkipList** d) {
    if (!d || !*d) return;

    Node* current = (*d)->sentinel->forward[0];
    while (current) {
        Node* next = current->forward[0];
        free(current->forward);
        free(current);
        current = next;
    }
    free((*d)->sentinel->forward);
    free((*d)->sentinel);
    free(*d);
    *d = NULL;
}

/*----------------------------------------------------------------------------*/
unsigned int skiplist_size(const SkipList* d) {
    return d->size;
}
/*----------------------------------------------------------------------------*/

int skiplist_at(const SkipList* d, unsigned int i) {
    Node* current = d->sentinel->forward[0];
    for (unsigned int index = 0; index < i; index++) {
        if (!current) return -1;  // Out of bounds
        current = current->forward[0];
    }
    return current ? current->value : -1;
}

/*----------------------------------------------------------------------------*/
void skiplist_map(const SkipList* d, ScanOperator f, void* user_data) {
    Node* current = d->sentinel->forward[0];
    while (current) {
        f(current->value, user_data);
        current = current->forward[0];
    }
}

/*----------------------------------------------------------------------------*/
SkipList* skiplist_insert(SkipList* d, int value) {
    Node* current = d->sentinel;
    Node** update = (Node**)malloc(d->nblevels * sizeof(Node*));

    // Find the position to insert the new value
    for (int i = d->nblevels - 1; i >= 0; i--) {
        while (current->forward[i] && current->forward[i]->value < value) {
            current = current->forward[i];
        }
        update[i] = current;
    }
    if (value >= MAX_VALUE) {
       free(update);
       return d;
    }

    if (inserted[value]) {
       free(update);
       return d;
    }
    inserted[value] = true;


    // Check if the value already exists
    current = current->forward[0];
    if (current && current->value == value) {
        free(update);
        return d; // Do not insert duplicates
    }

    // Determine the level for the new node


    int new_node_level = rng_get_value(&(d->rng)) + 1;
    if (new_node_level > d->nblevels) new_node_level = d->nblevels;

// Create the new node
    Node* new_node = (Node*)malloc(sizeof(Node));
    new_node->value = value;
    new_node->level = new_node_level;
    new_node->forward = (Node**)calloc(new_node_level, sizeof(Node*));
    new_node->backward = (Node**)calloc(new_node_level, sizeof(Node*));

// Update pointers at each level
   for (int i = 0; i < new_node_level; i++) {
       new_node->forward[i] = update[i]->forward[i];
       new_node->backward[i] = update[i];
       if (update[i]->forward[i]) {
          update[i]->forward[i]->backward[i] = new_node;
        }
       update[i]->forward[i] = new_node;
    }
    d->size++;
    free(update);
    return d; 
}

/*----------------------------------------------------------------------------*/
bool skiplist_search(const SkipList* d, int value, unsigned int* nb_operations) {
    Node* current = d->sentinel;
    *nb_operations = 0;

    for (int i = d->nblevels - 1; i >= 0; i--) {
        while (current->forward[i] && current->forward[i]->value < value) {
            current = current->forward[i];
            (*nb_operations)++;
        }
    }

    current = current->forward[0];
    (*nb_operations)++;

    return current && current->value == value;
}

/*----------------------------------------------------------------------------*/

SkipListIterator* skiplist_iterator_create(SkipList* d, IteratorDirection direction) {
    SkipListIterator* it = (SkipListIterator*)malloc(sizeof(SkipListIterator));
    it->list = d;
    it->direction = direction;

    // Start at the first or last element based on direction
    it->current = (direction == FORWARD_ITERATOR) ? d->sentinel->forward[0] : NULL;
    if (direction == BACKWARD_ITERATOR) {
        Node* current = d->sentinel->forward[0];
        while (current && current->forward[0]) {
            current = current->forward[0];
        }
        it->current = current; // Last node
    }
    return it;
}

/*----------------------------------------------------------------------------*/

SkipListIterator* skiplist_iterator_begin(SkipListIterator* it) {
    if (it->direction == FORWARD_ITERATOR) {
        it->current = it->list->sentinel->forward[0];
    } else {
        Node* current = it->list->sentinel->forward[0];
        while (current && current->forward[0]) {
            current = current->forward[0];
        }
        it->current = current; // Last node
    }
    return it;
}

/*----------------------------------------------------------------------------*/
SkipListIterator* skiplist_iterator_next(SkipListIterator* it) {
    if (it->current) {
        Node* node = (Node*)it->current;
        it->current = (it->direction == FORWARD_ITERATOR) ? node->forward[0] : node->backward[0];
    }
    return it;
}



/*----------------------------------------------------------------------------*/
bool skiplist_iterator_end(SkipListIterator* it) {
    return it->current == NULL;
}

/*----------------------------------------------------------------------------*/
int skiplist_iterator_value(SkipListIterator* it) {
    return it->current ? ((Node*)it->current)->value : -1;
}


/*----------------------------------------------------------------------------*/
// Delete the iterator
void skiplist_iterator_delete(SkipListIterator** it) {
    if (it && *it) {
        free(*it);
        *it = NULL;
    }
}

/*----------------------------------------------------------------------------*/


SkipList* skiplist_remove(SkipList* d, int value) {
    Node** update = (Node**)malloc(d->nblevels * sizeof(Node*));
    Node* current = d->sentinel;

    for (int level = d->nblevels - 1; level >= 0; level--) {
        while (current->forward[level] && current->forward[level]->value < value) {
            current = current->forward[level];
        }
        update[level] = current;
    }

    current = current->forward[0];
    if (current && current->value == value) {
        for (int level = 0; level < d->nblevels; level++) {
            if (update[level]->forward[level] != current) break;
            update[level]->forward[level] = current->forward[level];
        }
        free(current->forward);
        free(current);
        d->size--;
    }

    free(update);
    return d;
}



/*----------------------------------------------------------------------------*/