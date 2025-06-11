#include "bstree.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "queue.h"


static BinarySearchTree* fixredblack_insert(ptrBinarySearchTree* root, BinarySearchTree* x);
static BinarySearchTree* fixredblack_insert_case1(ptrBinarySearchTree* root, BinarySearchTree* x);
static BinarySearchTree* fixredblack_insert_case2(ptrBinarySearchTree* root, BinarySearchTree* x);
static BinarySearchTree* fixredblack_insert_case2_left(ptrBinarySearchTree* root, BinarySearchTree* x);
static BinarySearchTree* fixredblack_insert_case2_right(ptrBinarySearchTree* root, BinarySearchTree* x);


static BinarySearchTree* fixredblack_remove(ptrBinarySearchTree* root, BinarySearchTree* parent, BinarySearchTree* x);
static BinarySearchTree* fixredblack_remove_case1(ptrBinarySearchTree* root, BinarySearchTree* parent, BinarySearchTree* x);
static BinarySearchTree* fixredblack_remove_case1_left(ptrBinarySearchTree* root, BinarySearchTree* parent);
static BinarySearchTree* fixredblack_remove_case1_right(ptrBinarySearchTree* root, BinarySearchTree* parent);
static BinarySearchTree* fixredblack_remove_case2(ptrBinarySearchTree* root, BinarySearchTree* parent, BinarySearchTree* x);
static BinarySearchTree* fixredblack_remove_case2_left(ptrBinarySearchTree* root, BinarySearchTree* parent);
static BinarySearchTree* fixredblack_remove_case2_right(ptrBinarySearchTree* root, BinarySearchTree* parent);


typedef enum { red, black } NodeColor;
/*------------------------  BSTreeType  -----------------------------*/

struct _bstree {
    BinarySearchTree* parent;
    BinarySearchTree* left;
    BinarySearchTree* right;
    int key;
    NodeColor color; // New field for node color
};

// Type d'une fonction d'accès à un sous-arbre
typedef BinarySearchTree* (*AccessFunction)(const BinarySearchTree*);


// Structure regroupant les fonctions d'accès aux sous-arbres nécessaires
typedef struct {
    AccessFunction get_child;
    AccessFunction get_parent_child;
} ChildAccessors;


/*------------------------  BaseBSTree  -----------------------------*/
void fix_red_black_delete(ptrBinarySearchTree* root, BinarySearchTree* x);

void fix_red_black_insert(ptrBinarySearchTree* root, BinarySearchTree* x);


BinarySearchTree* bstree_create(void) {
    return NULL;
}

/* This constructor is private so that we can maintain the oredring invariant on
 * nodes. The only way to add nodes to the tree is with the bstree_add function
 * that ensures the invariant.
 */
BinarySearchTree* bstree_cons(BinarySearchTree* left, BinarySearchTree* right, int key) {
    BinarySearchTree* t = malloc(sizeof(struct _bstree));
    t->parent = NULL;
    t->left = left;
    t->right = right;
    if (t->left != NULL)
        t->left->parent = t;
    if (t->right != NULL)
        t->right->parent = t;
    t->key = key;
    t->color = red; // Initialize as red
    return t;
}


/**/
// Finds the node with the given key in the BST (returns NULL if not found)
ptrBinarySearchTree bstree_find(ptrBinarySearchTree t, int key) {
    while (t != NULL) {
        if (key < t->key)
            t = t->left;
        else if (key > t->key)
            t = t->right;
        else
            return t;
    }
    return NULL;
}


static const BinarySearchTree* find_next(const BinarySearchTree* x, ChildAccessors access) {
    if (x == NULL) return NULL;

    const BinarySearchTree* node = access.get_child(x);
    if (node != NULL) {
        while (access.get_parent_child(node) != NULL) {
            node = access.get_parent_child(node);
        }
        return node;
    } else {
        node = x;
        const BinarySearchTree* parent = node->parent;
        while (parent != NULL && node == access.get_child(parent)) {
            node = parent;
            parent = parent->parent;
        }
        return parent;
    }
}


void freenode(const BinarySearchTree* t, void* userData) {
    (void)userData; // Unused parameter
    free((BinarySearchTree*)t); // Free the node
}
void bstree_delete(ptrBinarySearchTree* t) {
    if (t == NULL || *t == NULL) return;

    // Use the existing post-order traversal function
    bstree_depth_postfix(*t, freenode, NULL);

    // Set the root pointer to NULL to mark the tree as empty
    *t = NULL;
}


/* Rotate left around node x */
void leftrotate(BinarySearchTree *x) {
    assert(x != NULL && x->right != NULL);

    BinarySearchTree *y = x->right;

    x->right = y->left;
    if (y->left != NULL) {
        y->left->parent = x;
    }

    y->parent = x->parent;

    if (x->parent == NULL) {
        // Root update must be handled outside this function
    } else if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }

    y->left = x;
    x->parent = y;
}

void rightrotate(BinarySearchTree *y) {
    assert(y != NULL && y->left != NULL);

    BinarySearchTree *x = y->left;

    y->left = x->right;
    if (x->right != NULL) {
        x->right->parent = y;
    }

    x->parent = y->parent;

    if (y->parent == NULL) {
        // Root update must be handled outside this function
    } else if (y == y->parent->left) {
        y->parent->left = x;
    } else {
        y->parent->right = x;
    }

    x->right = y;
    y->parent = x;
}

void testrotateleft(BinarySearchTree *t) {
    leftrotate(t);
}

void testrotateright(BinarySearchTree *t) {
    rightrotate(t);
}



bool bstree_empty(const BinarySearchTree* t) {
    return t == NULL;
}

int bstree_key(const BinarySearchTree* t) {
    assert(!bstree_empty(t));
    return t->key;
}

BinarySearchTree* bstree_left(const BinarySearchTree* t) {
    assert(!bstree_empty(t));
    return t->left;
}

BinarySearchTree* bstree_right(const BinarySearchTree* t) {
    assert(!bstree_empty(t));
    return t->right;
}

BinarySearchTree* bstree_parent(const BinarySearchTree* t) {
    assert(!bstree_empty(t));
    return t->parent;
}

/*------------------------  BSTreeDictionary  -----------------------------*/

/* Obligation de passer l'arbre par référence pour pouvoir le modifier */
void bstree_add(ptrBinarySearchTree* t, int v) {
    BinarySearchTree* current = *t;
    BinarySearchTree* parent = NULL;

    while (current != NULL) {
        parent = current;
        if (v < bstree_key(current)) {
            current = bstree_left(current);
        } else if (v > bstree_key(current)) {
            current = bstree_right(current);
        } else {
            
            return; // Duplicate value
        }
    }

    BinarySearchTree* new_node = bstree_cons(NULL, NULL, v);
    new_node->parent = parent;

    if (parent == NULL) {
        *t = new_node;
        
    } else if (v < bstree_key(parent)) {
        parent->left = new_node;
        
    } else {
        parent->right = new_node;
        
    }

    // Call fixup only once
    fixredblack_insert(t, new_node);
    

    // Now move root pointer to the true root (no parent)
    while ((*t)->parent != NULL) {
        *t = (*t)->parent;
    }

    // Ensure root is black
    (*t)->color = black;
    
}


/*----------------------------------------------------------------------------------------------------------------*/
void bstree_node_to_dot(const BinarySearchTree* t, void* stream) {
    FILE* file = (FILE*) stream;
    const char* fillcolor = (t->color == red) ? "red" : "grey";

    fprintf(file, "\tn%d [label=\"{%d|{<left>|<right>}}\", fillcolor=%s, style=filled, fontcolor=white];\n",
            bstree_key(t), bstree_key(t), fillcolor);

    if (bstree_left(t)) {
        fprintf(file, "\tn%d:left:c -> n%d:n [headclip=false, tailclip=false]\n",
                bstree_key(t), bstree_key(bstree_left(t)));
    } else {
        fprintf(file, "\tlnil%d [style=filled, fillcolor=grey, label=\"NIL\"];\n", bstree_key(t));
        fprintf(file, "\tn%d:left:c -> lnil%d:n [headclip=false, tailclip=false]\n",
                bstree_key(t), bstree_key(t));
    }
    if (bstree_right(t)) {
        fprintf(file, "\tn%d:right:c -> n%d:n [headclip=false, tailclip=false]\n",
                bstree_key(t), bstree_key(bstree_right(t)));
    } else {
        fprintf(file, "\trnil%d [style=filled, fillcolor=grey, label=\"NIL\"];\n", bstree_key(t));
        fprintf(file, "\tn%d:right:c -> rnil%d:n [headclip=false, tailclip=false]\n",
                bstree_key(t), bstree_key(t));
    }
}

/*----------------------------------------------------------------------------------------------------------------*/
const BinarySearchTree* bstree_search(const BinarySearchTree* t, int v) {
    while (t != NULL) {
        if (v < bstree_key(t)) {
            t = bstree_left(t);
        } else if (v > bstree_key(t)) {
            t = bstree_right(t);
        } else {
            return t; // Key found
        }
    }
    return NULL; // Key not found
}


const BinarySearchTree* bstree_successor(const BinarySearchTree* x) {
    ChildAccessors access = {
        .get_child = bstree_right,
        .get_parent_child = bstree_left
    };
    return find_next(x, access);
}

const BinarySearchTree* bstree_predecessor(const BinarySearchTree* x) {
    ChildAccessors access = {
        .get_child = bstree_left,
        .get_parent_child = bstree_right
    };
    return find_next(x, access);
}


void bstree_swap_nodes(ptrBinarySearchTree* tree, ptrBinarySearchTree from, ptrBinarySearchTree to) {
    assert(!bstree_empty(*tree) && !bstree_empty(from) && !bstree_empty(to));
    
     if (from == to)
        return;

    ptrBinarySearchTree fromLeft = from->left;
    ptrBinarySearchTree fromRight = from->right;
    ptrBinarySearchTree toLeft = to->left;
    ptrBinarySearchTree toRight = to->right;

    if (fromLeft) fromLeft->parent = to;
    if (fromRight) fromRight->parent = to;
    if(toLeft) toLeft->parent = from;
    if(toRight) toRight->parent = from;

    to->left = fromLeft;
    to->right = fromRight;
    from->left = toLeft;
    from->right = toRight;
    
    if (from->parent) {
        if (from->parent->left == from) {
            from->parent->left = to;
        } else {
            from->parent->right = to;
        }
    } else {
        *tree = to;
    }

    if (to->parent) {
        if (to->parent->left == to) {
            to->parent->left = from;
        } else {
            to->parent->right = from;
        }
    } else {
        *tree = from;
    }

    ptrBinarySearchTree temp = from->parent;
    from->parent = to->parent;
    to->parent = temp;
}

// t -> the tree to remove from, current -> the node to remove
void bstree_remove_node(ptrBinarySearchTree* t, ptrBinarySearchTree current) {
    assert(!bstree_empty(*t) && !bstree_empty(current));

    BinarySearchTree* replacement;
    BinarySearchTree* fix_target;

    if (current->left == NULL || current->right == NULL) {
        replacement = (current->left) ? current->left : current->right;

        // Replace current node
        if (replacement) replacement->parent = current->parent;

        if (current->parent == NULL) {
            *t = replacement;
        } else if (current == current->parent->left) {
            current->parent->left = replacement;
        } else {
            current->parent->right = replacement;
        }

        fix_target = replacement ? replacement : current->parent;

        if (current->color == black) {
            if (fix_target != NULL || current->parent != NULL) { // Defensive: Only call if at least one exists
                fixredblack_remove(t, (current->parent ? current->parent : NULL), fix_target);
            }
        }
        
        

        free(current);
    } else {
        // Find the successor
        BinarySearchTree* successor = (BinarySearchTree*)bstree_successor(current);
        bstree_swap_nodes(t, current, successor);
        bstree_remove_node(t, current);
    }
}


void bstree_remove(ptrBinarySearchTree* t, int v) {

   ptrBinarySearchTree current = *t;
    while (current && current->key != v)
        current = (current->key > v) ? current->left : current->right;
    if (current){

        bstree_remove_node(t, current);
    }
    
}



/*------------------------  BSTreeVisitors  -----------------------------*/

void bstree_depth_prefix(const BinarySearchTree* t, OperateFunctor f, void* environment) {
    if (t == NULL) return;
    f(t, environment); // Visit root
    bstree_depth_prefix(bstree_left(t), f, environment); // Traverse left subtree
    bstree_depth_prefix(bstree_right(t), f, environment); // Traverse right subtree
}

void bstree_depth_infix(const BinarySearchTree* t, OperateFunctor f, void* environment) {
    if (t == NULL) return;
    bstree_depth_infix(bstree_left(t), f, environment); // Traverse left subtree
    f(t, environment); // Visit root
    bstree_depth_infix(bstree_right(t), f, environment); // Traverse right subtree
}

void bstree_depth_postfix(const BinarySearchTree* t, OperateFunctor f, void* environment) {
    if (t == NULL) return;
    bstree_depth_postfix(bstree_left(t), f, environment); // Traverse left subtree
    bstree_depth_postfix(bstree_right(t), f, environment); // Traverse right subtree
    f(t, environment); // Visit root
}


void bstree_iterative_breadth(const BinarySearchTree* t, OperateFunctor f, void* environment) {
    if (t == NULL) return;

    Queue* q = create_queue();
    queue_push(q, t);

    while (!queue_empty(q)) {
        const BinarySearchTree* current = queue_top(q);
        queue_pop(q);

        f(current, environment); // Visit the node

        if (bstree_left(current)) {
            queue_push(q, bstree_left(current)); // Enqueue left child
        }
        if (bstree_right(current)) {
            queue_push(q, bstree_right(current)); // Enqueue right child
        }
    }

    delete_queue(&q); // Clean up
}


void bstree_iterative_depth_infix(const BinarySearchTree* t, OperateFunctor f, void* environment) {
    // If the tree is empty, return immediately
    if (t == NULL) return;

    // Create a stack to simulate recursion
    const BinarySearchTree* stack[100]; // Adjust size if needed for larger trees
    int top = -1;

    const BinarySearchTree* current = t;

    while (current != NULL || top >= 0) {
        // Traverse to the leftmost node
        while (current != NULL) {
            stack[++top] = current; // Push the current node onto the stack
            current = bstree_left(current);
        }

        // Pop a node from the stack
        current = stack[top--];

        // Process the node
        f(current, environment);

        // Move to the right subtree
        current = bstree_right(current);
    }
}


/*------------------------  BSTreeIterator  -----------------------------*/

struct _BSTreeIterator {
    /* the collection the iterator is attached to */
    const BinarySearchTree* collection;
    /* the first element according to the iterator direction */
    const BinarySearchTree* (*begin)(const BinarySearchTree* );
    /* the current element pointed by the iterator */
    const BinarySearchTree* current;
    /* function that goes to the next element according to the iterator direction */
    const BinarySearchTree* (*next)(const BinarySearchTree* );
};

const BinarySearchTree* goto_min(const BinarySearchTree* e) {
    if (e == NULL) return NULL;
    while (bstree_left(e) != NULL) {
        e = bstree_left(e);
    }
    return e;
}

const BinarySearchTree* goto_max(const BinarySearchTree* e) {
    if (e == NULL) return NULL;
    while (bstree_right(e) != NULL) {
        e = bstree_right(e);
    }
    return e;
}


/* constructor */
BSTreeIterator* bstree_iterator_create(const BinarySearchTree* collection, IteratorDirection direction) {
    BSTreeIterator* iterator = malloc(sizeof(BSTreeIterator));
    iterator->collection = collection;
    if (direction == forward) {
        iterator->begin = goto_min;
        iterator->next = bstree_successor;
    } else {
        iterator->begin = goto_max;
        iterator->next = bstree_predecessor;
    }
    iterator->current = NULL; // Initialize to NULL
    return iterator;
}


/* destructor */
void bstree_iterator_delete(ptrBSTreeIterator* i) {
    free(*i);
    *i = NULL;
}

BSTreeIterator* bstree_iterator_begin(BSTreeIterator* i) {
    i->current = i->begin(i->collection);
    return i;
}


bool bstree_iterator_end(const BSTreeIterator* i) {
    return i->current == NULL;
}


BSTreeIterator* bstree_iterator_next(BSTreeIterator* i) {
    if (i->current != NULL) {
        i->current = i->next(i->current);
    }
    return i;
}


const BinarySearchTree* bstree_iterator_value(const BSTreeIterator* i) {
    return i->current;
}


BinarySearchTree* grandparent(BinarySearchTree* n) {
    if (n && n->parent) {
        return n->parent->parent;
    }
    return NULL;
}

BinarySearchTree* uncle(BinarySearchTree* n) {
    BinarySearchTree* g = grandparent(n);
    if (!g) return NULL;
    if (n->parent == g->left) {
        return g->right;
    } else {
        return g->left;
    }
}


// Replace your fix_red_black_insert with:
static BinarySearchTree* fixredblack_insert(ptrBinarySearchTree* root, BinarySearchTree* x) {
    

    if (x->parent == NULL) {
        x->color = black;
        *root = x;
        return x;
    }
    if (x->parent->color == black) return x;

    BinarySearchTree* r = fixredblack_insert_case1(root, x);

    // --- Force the root to be black at the end! ---
    if (*root) (*root)->color = black;

    

    return r;
}



static BinarySearchTree* fixredblack_insert_case1(ptrBinarySearchTree* root, BinarySearchTree* x) {
   

    BinarySearchTree* u = uncle(x);
    BinarySearchTree* p = x->parent;
    BinarySearchTree* g = grandparent(x);

    if (u && u->color == red) {
        p->color = black;
        u->color = black;
        if (g) g->color = red;
        // Only recurse up if grandparent is NOT root
        if (g && g->parent != NULL) {
            

            return fixredblack_insert_case1(root, g);
        } else {
            if (g && g->parent == NULL) {
                g->color = black;
                *root = g;
            }
            return g;
        }
    } else {
        

        return fixredblack_insert_case2(root, x);
    }
}

static BinarySearchTree* fixredblack_insert_case2(ptrBinarySearchTree* root, BinarySearchTree* x) {
    BinarySearchTree* p = x->parent;
    BinarySearchTree* g = grandparent(x);
    if (p == NULL || g == NULL) return x;

    if (g == NULL) return x; // Defensive: do nothing if grandparent is NULL

    if (p == g->left)
        return fixredblack_insert_case2_left(root, x);
    else
        return fixredblack_insert_case2_right(root, x);
}


static BinarySearchTree* fixredblack_insert_case2_left(ptrBinarySearchTree* root, BinarySearchTree* x) {
   

    BinarySearchTree* p = x->parent;
    BinarySearchTree* g = grandparent(x);

    if (x == p->right) {
        leftrotate(p);
        x = p;
        p = x->parent;
        g = grandparent(x);
    }
    p->color = black;
    if (g) {
        g->color = red;
        rightrotate(g);
        // No need to set *root again: handled by rotation!
    }
    if (p->parent == NULL) {
        *root = p;
        p->color = black;
    }
  

    return p;
}

static BinarySearchTree* fixredblack_insert_case2_right(ptrBinarySearchTree* root, BinarySearchTree* x) {
    

    BinarySearchTree* p = x->parent;
    BinarySearchTree* g = grandparent(x);

    if (x == p->left) {
        rightrotate(p);
        x = p;
        p = x->parent;
        g = grandparent(x);
    }
    p->color = black;
    if (g) {
        g->color = red;
        leftrotate(g);
        // No need to set *root again: handled by rotation!
    }
    if (p->parent == NULL) {
        *root = p;
        p->color = black;
    }
   

    return p;
}

// Master entry point, called from remove_node
static BinarySearchTree* fixredblack_remove(ptrBinarySearchTree* root, BinarySearchTree* parent, BinarySearchTree* x) {
    if (parent == NULL) {
        if (x) x->color = black;
        return x;
    }
    BinarySearchTree* sibling = (x == parent->left) ? parent->right : parent->left;
    if (sibling == NULL) return parent; // Shouldn't happen in a valid RBT

    if (sibling->color == black) {
        return fixredblack_remove_case1(root, parent, x);
    } else {
        return fixredblack_remove_case2(root, parent, x);
    }
}

// Case 1: sibling is black
static BinarySearchTree* fixredblack_remove_case1(ptrBinarySearchTree* root, BinarySearchTree* parent, BinarySearchTree* x) {
    if (x == parent->left) {
        return fixredblack_remove_case1_left(root, parent);
    } else {
        return fixredblack_remove_case1_right(root, parent);
    }
}

// Case 1 LEFT: x is left child
static BinarySearchTree* fixredblack_remove_case1_left(ptrBinarySearchTree* root, BinarySearchTree* parent) {
    BinarySearchTree* sibling = parent->right;
    if (sibling == NULL) return parent; 

    // Both sibling's children are black or null
    bool left_black = (sibling->left == NULL) || (sibling->left->color == black);
    bool right_black = (sibling->right == NULL) || (sibling->right->color == black);

    if (left_black && right_black) {
        sibling->color = red;
        if (parent->color == red) {
            parent->color = black;
            return parent;
        } else {
            // propagate double-black upward
            return fixredblack_remove(root, parent->parent, parent);
        }
    } else if (right_black == 0) {
        // Sibling's right child is red (case 1d in some texts)
        sibling->color = parent->color;
        parent->color = black;
        if (sibling->right) sibling->right->color = black;
        leftrotate(parent);
        if (parent == *root) *root = sibling;
        return sibling;
    } else if (left_black == 0) {
        // Sibling's left child is red and right child is black
        if (sibling->left) sibling->left->color = black;
        sibling->color = red;
        rightrotate(sibling);
        sibling = parent->right; // update sibling after rotation
        // now sibling's right child is red, so do as above
        sibling->color = parent->color;
        parent->color = black;
        if (sibling->right) sibling->right->color = black;
        leftrotate(parent);
        if (parent == *root) *root = sibling;
        return sibling;
    }
    return NULL; // Should not reach here
}

// Case 1 RIGHT: x is right child
static BinarySearchTree* fixredblack_remove_case1_right(ptrBinarySearchTree* root, BinarySearchTree* parent) {
    BinarySearchTree* sibling = parent->left;
    if (sibling == NULL) return parent;
    bool left_black = (sibling->left == NULL) || (sibling->left->color == black);
    bool right_black = (sibling->right == NULL) || (sibling->right->color == black);

    if (left_black && right_black) {
        sibling->color = red;
        if (parent->color == red) {
            parent->color = black;
            return parent;
        } else {
            return fixredblack_remove(root, parent->parent, parent);
        }
    } else if (left_black == 0) {
        sibling->color = parent->color;
        parent->color = black;
        if (sibling->left) sibling->left->color = black;
        rightrotate(parent);
        if (parent == *root) *root = sibling;
        return sibling;
    } else if (right_black == 0) {
        if (sibling->right) sibling->right->color = black;
        sibling->color = red;
        leftrotate(sibling);
        sibling = parent->left;
        sibling->color = parent->color;
        parent->color = black;
        if (sibling->left) sibling->left->color = black;
        rightrotate(parent);
        if (parent == *root) *root = sibling;
        return sibling;
    }
    return NULL;
}

// Case 2: sibling is red
static BinarySearchTree* fixredblack_remove_case2(ptrBinarySearchTree* root, BinarySearchTree* parent, BinarySearchTree* x) {
    if (x == parent->left) {
        return fixredblack_remove_case2_left(root, parent);
    } else {
        return fixredblack_remove_case2_right(root, parent);
    }
}

static BinarySearchTree* fixredblack_remove_case2_left(ptrBinarySearchTree* root, BinarySearchTree* parent) {
    BinarySearchTree* sibling = parent->right;
    if (sibling == NULL) return parent;
    sibling->color = black;
    parent->color = red;
    leftrotate(parent);
    // After rotation, new sibling is parent's right child
    sibling = parent->right;
    // Continue with case 1 (now sibling is black)
    return fixredblack_remove_case1_left(root, parent);
}

static BinarySearchTree* fixredblack_remove_case2_right(ptrBinarySearchTree* root, BinarySearchTree* parent) {
    BinarySearchTree* sibling = parent->left;
    if (sibling == NULL) return parent;
    sibling->color = black;
    parent->color = red;
    rightrotate(parent);
    sibling = parent->left;
    return fixredblack_remove_case1_right(root, parent);
}
