
#include "bstree.h"
#include <stdio.h>
#include <stdlib.h>


/* Define this for solving the exercice 1 - coloring the tree. */
#define EXERCICE_1
/* Define this for solving the exercice 2. - test rotations on nodes */
#define EXERCICE_2
/* Define this for solving the exercice 3. - fix rb property after add*/
#define EXERCICE_3
/* Define this for solving the exercice 4. - nothing to do, just to verify that search is still operational */
#define EXERCICE_4
/* Define this for solving the exercice 5. - fix rb property after remove */
#define EXERCICE_5


#ifdef EXERCICE_3
#undef EXERCICE_2
#endif

/**
 * Print the value of a node.
 * @param t the tree node to output
 * @param userData unused pointer.
 */
void print_tree(const BinarySearchTree *t, void *userData) {
    (void) userData;
    printf("%d ", bstree_key(t));
}

/** This function reads an int from a file with result checking */
int read_int(FILE* input) {
  int v;
  int r = fscanf(input,"%d", &v);
  if (r == 1) {
    return v;
  }
  perror("Unable to read int from input file\n");
  abort();
}


#ifndef EXERCICE_1
/**
 * This function output one node using the <a href="https://www.graphviz.org/documentation/">dot</a> syntax.
 * A node must defined its shape and its links to the left and righet subtrees. If one of this subtree is NULL,
 * a grey box with the label NIL is drawn.
 * @dot
 * digraph node_example {
 *      node [shape = record];
 *      treeroot [label="{root|{<left>|<right>}}"];
 *      left [label="bstree_left()"];
 *      right [label="bstree_right()"];
 *      treeroot:left:c -> left:n [headclip=false, tailclip=false]
 *      treeroot:right:c -> right:n [headclip=false, tailclip=false]
 * }
 * @enddot
 * @param t the tree node to draw.
 * @param stream the file to output the dot commands. Concretely, this parameter will be of type FILE * when the functor is called.
 */
void node_to_dot(const BinarySearchTree *t, void *stream) {
    FILE *file = (FILE *) stream;

    printf("%d ", bstree_key(t));
    fprintf(file, "\tn%d [label=\"{%d|{<left>|<right>}}\"];\n",
            bstree_key(t), bstree_key(t));

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
#endif
/**
 * Exports the tree as a graphviz file using the dot language
 */
void export_dot(BinarySearchTree* t, FILE* stream) {
    fprintf(stream, "digraph RedBlackTree {\n\tgraph [ranksep=0.5];\n\tnode [shape = record];\n\n");
    #ifdef EXERCICE_1
    bstree_iterative_depth_infix(t, bstree_node_to_dot, stream);
    #else
    bstree_iterative_depth_infix(t, node_to_dot, stream);
    #endif
    fprintf(stream, "\n}\n");
}

/** Main function for testing the Tree implementation.
 * The main function expects one parameter that is the file where values added to the tree, searched into the
 * tree and removed from the tree are to be read.
 *
 * This file must contain the following informations :
 * - on the first line, the number of values to be added to the tree,
 * - on the second line, the values to be added, separated by a space (or tab).
 * - on the third line, the number of values to be searched into the tree.
 * - on the fourth line, the values to be searched, separated by a space (or tab).
 * - on the fifth line, the number of values to be removed from the tree.
 * - on the sixth line, the values to be removed, separated by a space (or tab).
 *
 * The values will be added, searched and remove in the order they are read from the file.
 */
int main(int argc, char **argv) {

    if (argc < 2) {
        fprintf(stderr, "usage : %s filename\n", argv[0]);
        return 1;
    }

    FILE *input = fopen(argv[1], "r");

    if (!input) {
        perror(argv[1]);
        return 1;
    }


    BinarySearchTree *theTree = bstree_create();

    /* Exercice 1 : add values to the BinarySearchTree */
    printf("Adding values to the tree.\n\t");
    int n = read_int(input);

    for (int i = 0; i < n; ++i) {
        int v = read_int(input);
        printf("%d ", v);
        bstree_add(&theTree, v);
    }
    printf("\nDone.\n");
    

#ifdef EXERCICE_1
    /* Exercice 1 : exporting the colored tree */
    printf("Exporting the tree.\n\t");
    FILE *output = fopen("redblacktree_0.dot", "w");
    export_dot(theTree, output);
    fclose(output);
    printf("\nDone.\n");

#ifdef EXERCICE_2
    /* Exercice 2 : rotate left */
    printf("Rotating the tree left around %d.\n\t", bstree_key(theTree));
    testrotateleft(theTree);
    theTree = bstree_parent(theTree);
    output = fopen("redblacktree_0_leftrotateroot.dot", "w");
    export_dot(theTree, output);
    fclose(output);
    printf("Done.\n");

    printf("Rotating the tree right around %d.\n\t", bstree_key(bstree_left(theTree)));
    testrotateright(bstree_left(theTree));
    output = fopen("redblacktree_0_rightrotateleftrotatedroot.dot", "w");
    export_dot(theTree, output);
    fclose(output);
    printf("\nDone.\n");
#endif

#ifdef EXERCICE_4
    /* Exercice 4 : search for values on the tree */
    printf("Searching into the tree.");
    n = read_int(input);
    for (int i = 0; i < n; ++i) {
        int v = read_int(input);
        printf("\n\tSearching for value %d in the tree : %s", v, bstree_search(theTree, v) ? "true" : "false");
    }
    printf("\nDone.\n");

#ifdef EXERCICE_5
    /* Exercice 5 : remove a value from the tree */
    printf("Removing from the tree."); 
    n = read_int(input);
    for (int i = 0; i < n; ++i) {
        int v = read_int(input);
        printf("\n\tRemoving the value %d from the tree : \t", v);
        bstree_remove(&theTree, v);

        char filename[256];
        sprintf(filename, "thetree-%d.dot", i + 1);
        output = fopen(filename, "w");
        export_dot(theTree, output);
        fclose(output);
    }
    printf("\nDone.\n");
#endif
#endif
#endif

    bstree_delete(&theTree);
    fclose(input);
    return 0;
}

