/* DOBRE Maria-Adina - 312CD */
#ifndef TREE_HEADER
#define TREE_HEADER

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//.............................................................
//.............................................................
//.............................................................
typedef struct gri { // structure to define the initial matrix of pixels
  unsigned char valoarea_pentru_rosu;
  unsigned char valoarea_pentru_albastru;
  unsigned char valoarea_pentru_verde;
} Grid;

typedef struct tee {                  // quadtree
  int dimensiune_imagine;             // size of image for compression
  unsigned char type;                 // leafnode = 1, not a leaf = 0
  unsigned char valoarea_pentru_rosu; // pixel colors
  unsigned char valoarea_pentru_albastru;
  unsigned char valoarea_pentru_verde;
  struct tee *square_1; // pointer to upper left quarter of devided image
  struct tee *square_2; // pointer to upper right quarter of devided image
  struct tee *square_3; // pointer to lower left quarter of devided image
  struct tee *square_4; // pointer to lower right quarter of devided image
} Tree;

typedef struct Node { // node in queue
  Tree *data;         // pointer to a node in tree
  struct Node *next;
} Node;

typedef struct Queue { // queue
  // queue is used to perform a level-order traversal of the tree.
  Node *front;
  Node *rear;
  int size; // size of the queue
} Queue;

//........................................................................
//........................FUNCTIONS.......................................
//........................................................................

// recursive helper function that traverses the quadtree, and for each leaf
// node, sets the color of the corresponding pixels in the grid matrix.
extern void getGridAdd(Tree *root, Grid **grid, int x, int y, int size);

// reates a 2D grid of pixels and initializes it to default values, and then
// traverses the quadtree and fills the corresponding pixels in the grid matrix
// based on the colors stored in the leaf nodes of the quadtree
extern Grid **getGrid(Tree *root, int size);

// Creates a new empty queue and returns a pointer to it.
extern Queue *createQueue();

// returns 1 if the queue is empty, 0 otherwise.
extern int isEmpty(Queue *q);

// inserts a new element to the back of the queue.
extern void enqueue(Queue *q, Tree *data);

// deteles first element in the queue
extern void dequeue(Queue *q);

// returns the number of elements in the queue.
extern int gSize(Queue *q);

// deallocates the memory of all Nodes in the queue and the queue itself.
extern void destroyQueue(Queue *q);

// this function will return the quadtree node ( that includes a pixel, size of
// image, type) that is associated to the node in the front of the queue
extern Tree *getHead(Queue *q);

// this function will read information from the compressed file and will created
// the quadtree
extern Tree *readFromLevel(FILE *fp);

// this function is for decompression. it will write in the ppm file all the
// pixels
extern void toPpm(FILE *fp, Grid **grid, unsigned int size);

// this function will calculate the maximum size of a leaf
extern int maxLeafSize(Tree *root);

// this functio will allocate memory and initialize elements in quadtree
extern Tree *initNode(int size);

// this function will return the number of levels in quadtree
extern int countLevel(Tree *root);

// this function will build the quadtree
extern Tree *insert(int x, int y, int size, int threshold, Grid **grid);

// this function will free the memory used for nodes in quadtree
extern void freeQuadtree(Tree *node);

// this function returns the number of leaves in the quadtree
extern int countFrunze(Tree *root);

// this function is for compression. It will write information from the
// compression quadtree
extern void writeOnLevel(FILE *fp, Tree *root);

#endif // TREE_HEADER