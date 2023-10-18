/* DOBRE Maria-Adina - 312CD */
#include "Tree.h"
#include <math.h>
#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) > (b)) ? (a) : (b))

//...............................................................
//...............................................................
//...............................................................

Tree *initNode(int size) { // allocate memory for a tree node
  Tree *img = (Tree *)malloc(sizeof(Tree));
  img->dimensiune_imagine = size;    // size of image to compress
  img->type = 0;                     // initialize type as non-leaf node
  img->valoarea_pentru_albastru = 0; // imitialize pixel colors
  img->valoarea_pentru_rosu = 0;
  img->valoarea_pentru_verde = 0;
  img->square_1 = NULL;
  img->square_1 = NULL;
  img->square_2 = NULL;
  img->square_3 = NULL;
  img->square_4 = NULL;

  return img;
}

// this function will build the quadtree
Tree *insert(int x, int y, int size, int threshold, Grid **grid) {
  // initialize the node
  Tree *root = initNode(size);
  // sums for calculatig mean_color
  unsigned long long s1 = 0, s2 = 0, s3 = 0;
  unsigned long long n = size * size;

  for (int i = x; i < x + size; i++) {
    for (int j = y; j < y + size; j++) {
      s1 += (unsigned long long)grid[i][j].valoarea_pentru_rosu;
      s2 += (unsigned long long)grid[i][j].valoarea_pentru_verde;
      s3 += (unsigned long long)grid[i][j].valoarea_pentru_albastru;
    }
  }

  s1 = 1.0 * s1 / n; // these should be integers
  s2 = 1.0 * s2 / n;
  s3 = 1.0 * s3 / n;
  // we need to turn them to unsigned chars to form a pixel for mean_color
  unsigned char s1_c = s1, s2_c = s2, s3_c = s3;
  unsigned long long score = 0;

  // score using the mean color
  for (int i = x; i < x + size; i++) {
    for (int j = y; j < y + size; j++) {
      unsigned long long d1 = (s1 - grid[i][j].valoarea_pentru_rosu);
      unsigned long long d2 = (s2 - grid[i][j].valoarea_pentru_verde);
      unsigned long long d3 = (s3 - grid[i][j].valoarea_pentru_albastru);
      score += (d1 * d1) + (d2 * d2) + (d3 * d3);
    }
  }
  // score must be integer
  int mean = (score / (3 * n));

  // ? divide the image further
  if (mean <= threshold) { // we dont need to  so we initialize the mean_color
                           // of the leaf
    (root)->valoarea_pentru_albastru = s3_c;
    (root)->valoarea_pentru_rosu = s1_c;
    (root)->valoarea_pentru_verde = s2_c;
    (root)->type = 1; // set node as leaf
  } else {            // created new nodes for tree
    // the width and height will be half because it is a quadtree
    int halfSize = size / 2;
    // Divide the block into four smaller parts
    root->square_1 = insert(x, y, halfSize, threshold, grid);
    root->square_2 = insert(x, y + halfSize, halfSize, threshold, grid);
    root->square_3 = insert(x + halfSize, y, halfSize, threshold, grid);
    root->square_4 =
        insert(x + halfSize, y + halfSize, halfSize, threshold, grid);
  }
  return root;
}

// how many levels does the quadtree have
int countLevel(Tree *root) {
  if (root == NULL) {
    return 0;
  }
  int max = 0; // to store the maximum level
  if (root->square_1 != NULL) {
    max = max(max, countLevel(root->square_1));
  }
  if (root->square_2 != NULL) {
    max = max(max, countLevel(root->square_2));
  }
  if (root->square_3 != NULL) {
    max = max(max, countLevel(root->square_3));
  }
  if (root->square_4 != NULL) {
    max = max(max, countLevel(root->square_4));
  }
  return max + 1;
}

int countFrunze(Tree *root) {
  int count = 0; // counter for leaves
  if (root == NULL)
    return 0;
  // If the node has no children, it is a leaf
  if (root->square_1 == NULL && root->square_2 == NULL &&
      root->square_3 == NULL && root->square_4 == NULL) {
    return 1;
  }

  count += countFrunze(root->square_1);
  count += countFrunze(root->square_2);
  count += countFrunze(root->square_3);
  count += countFrunze(root->square_4);

  return count;
}

int maxLeafSize(Tree *root) {
  int max_size = 0; // counter for size of leaves

  if (root->square_1 == NULL && root->square_2 == NULL &&
      root->square_3 == NULL && root->square_4 == NULL) {
    // base case: return leaf size
    return root->dimensiune_imagine;
  }
  // if the root node has children, recursively check each child node
  int size1 = maxLeafSize(root->square_1);
  int size2 = maxLeafSize(root->square_2);
  int size3 = maxLeafSize(root->square_3);
  int size4 = maxLeafSize(root->square_4);
  // find the maximum of the leaf sizes
  max_size = max(size1, max(size2, max(size3, size4)));
  return max_size;
}

void writeOnLevel(FILE *fp, Tree *root) {
  // rites the elements of the tree in level order to the file using a queue to
  // keep track of the nodes
  if (root == NULL) {
    return;
  }
  // put the nodes in the tree in the queue
  Queue *q = createQueue();
  enqueue(q, root);
  while (!isEmpty(q)) {        // as long as we have elements in the queue
    int levelSize = gSize(q);  // take the size
    while (levelSize--) {      // decreases lvl and returns old value so it gets
                               // inside the loop
      Tree *curr = getHead(q); // takes the head of the queue out
      dequeue(q);
      if (curr->type == 1) { // if the head is node tree type 1 aka a leaf
        fwrite(&curr->type, 1, sizeof(unsigned char),
               fp); // we write it in the compression file
        fwrite(&curr->valoarea_pentru_rosu, 1, sizeof(unsigned char), fp);
        fwrite(&curr->valoarea_pentru_verde, 1, sizeof(unsigned char), fp);
        fwrite(&curr->valoarea_pentru_albastru, 1, sizeof(unsigned char), fp);
      } else { // write 0 if it is not a leaf
        fwrite(&curr->type, 1, sizeof(unsigned char), fp);
      }
      if (curr->square_1) // put the nodes on the next level in the queue
        enqueue(q, curr->square_1);
      if (curr->square_2)
        enqueue(q, curr->square_2);
      if (curr->square_4)
        enqueue(q, curr->square_4);
      if (curr->square_3)
        enqueue(q, curr->square_3);
    }
  }
  destroyQueue(q);
}

//.....................................................................
//.....................function for queue..............................
//.....................................................................
Queue *createQueue() {
  Queue *q = (Queue *)malloc(sizeof(Queue));
  q->front = q->rear = NULL;
  q->size = 0;
  return q;
}

int isEmpty(Queue *q) { return q->front == NULL; }

void enqueue(Queue *q, Tree *data) {
  Node *newNode = (Node *)malloc(sizeof(Node));
  newNode->data = data;
  newNode->next = NULL;
  if (isEmpty(q)) {
    q->front = q->rear = newNode;
  } else {
    q->rear->next = newNode;
    q->rear = newNode;
  }
  q->size++;
}

void dequeue(Queue *q) {
  if (isEmpty(q)) {
    return;
  }
  Node *temp = q->front;
  q->front = q->front->next;
  free(temp);
  q->size--;
}

int gSize(Queue *q) { return q->size; }

void destroyQueue(Queue *q) {
  while (!isEmpty(q)) {
    dequeue(q);
  }
  free(q);
}
//.....................................................................
//.....................................................................
// this function will build the quadtree from a compressed file
Tree *readFromLevel(FILE *fp) {
  if (fp == NULL) {
    printf("Error\n");
    return NULL;
  }
  // use a queue to take count of the levels
  Queue *q = createQueue();
  unsigned int size; // size of image
  if (!fread(&size, sizeof(unsigned int), 1, fp)) {
    fprintf(stderr, "Error\n");
    return NULL;
  }
  Tree *root = initNode(size); // create the root node of the quadtree
  enqueue(q, root);            // enqueue the root node into the queue
  int levelSize = 1;           // now queue has size 1
  while (!isEmpty(q)) {
    levelSize = gSize(q); // get how many elements are on current lvl
    for (int i = 0; i < levelSize; i++) {
      Tree *node = getHead(q); // dequeue the first node from the queue
      dequeue(q);
      if (fread(&(node->type), sizeof(unsigned char), 1, fp) != 1) {
        fprintf(stderr, "Error reading node type from file.\n");
        destroyQueue(q);
        return NULL;
      }
      // check type of node
      if (node->type == 1) {
        if (fread(&(node->valoarea_pentru_rosu), sizeof(unsigned char), 1,
                  fp) != 1 ||
            fread(&(node->valoarea_pentru_verde), sizeof(unsigned char), 1,
                  fp) != 1 ||
            fread(&(node->valoarea_pentru_albastru), sizeof(unsigned char), 1,
                  fp) != 1) {
          fprintf(stderr, "Error reading pixel values from file.\n");
          destroyQueue(q);
          return NULL;
        }
      }

      if (node->type == 0) { // internal node
        int childSize = (node->dimensiune_imagine / 2);
        Tree *tl = initNode(
            childSize); // make children for it in case it is not a leaf
        Tree *tr = initNode(childSize); // children must be in order
        Tree *bl = initNode(childSize);
        Tree *br = initNode(childSize);
        if (!tl || !tr || !bl || !br) {
          fprintf(stderr, "Error\n");
          destroyQueue(q);
          return NULL;
        }
        // pur the newly created nodes in the quadtree
        node->square_1 = tl;
        node->square_2 = tr;
        node->square_4 = br;
        node->square_3 = bl;
        enqueue(q, tl); // put them in the queue
        enqueue(q, tr);
        enqueue(q, br);
        enqueue(q, bl);
      }
    }
  }

  destroyQueue(q);
  return root; // return the root node of the quadtree
}

Tree *getHead(Queue *q) {
  if (!isEmpty(q)) {
    return q->front->data;
  } // this function will return a Tree node associated with the head node in
    // the queue
  return NULL;
}

Grid **getGrid(Tree *root, int size) {
  // returns a 2D array of type Grid **, which represents the grid corresponding
  // to the quadtree
  // allocate memory
  Grid **grid = malloc(size * sizeof(Grid *));
  for (int i = 0; i < size; i++) {
    grid[i] = malloc(size * sizeof(Grid));
  }

  // initialize it with default values
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      grid[i][j].valoarea_pentru_rosu = 0;
      grid[i][j].valoarea_pentru_verde = 0;
      grid[i][j].valoarea_pentru_albastru = 0;
    }
  }

  // traverse the quadtree and fill the corresponding pixels in the grid matrix
  getGridAdd(root, grid, 0, 0, size);

  return grid;
}

void getGridAdd(Tree *root, Grid **grid, int x, int y, int size) {
  // main role of this function is to initialize the pixels and reconstruct the
  // image
  // - x, y: the top-left coordinates of the current sub-matrix being traversed
  if (root == NULL) {
    return;
  }

  if (root->type == 1) {
    // Leaf node, set the corresponding pixels in the grid matrix to the color
    // value
    for (int i = x; i < x + size; i++) {
      for (int j = y; j < y + size; j++) {
        grid[i][j].valoarea_pentru_rosu = root->valoarea_pentru_rosu;
        grid[i][j].valoarea_pentru_verde = root->valoarea_pentru_verde;
        grid[i][j].valoarea_pentru_albastru = root->valoarea_pentru_albastru;
      }
    }
  } else {
    // non-leaf node, recursively traverse the four sub-matrixes
    int halfSize = size / 2;
    getGridAdd(root->square_1, grid, x, y, halfSize);
    getGridAdd(root->square_2, grid, x, y + halfSize, halfSize);
    getGridAdd(root->square_4, grid, x + halfSize, y + halfSize, halfSize);
    getGridAdd(root->square_3, grid, x + halfSize, y, halfSize);
  }
}

void toPpm(FILE *fp, Grid **grid, unsigned int size) {
  // writes the data in the grid to a PPM file format
  int max_color = 255;
  fprintf(fp, "P6\n%d %d\n%d\n", size, size, max_color);

  for (int i = 0; i < (int)size; i++) {
    for (int j = 0; j < (int)size; j++) {
      fwrite(&grid[i][j].valoarea_pentru_rosu, 1, sizeof(unsigned char), fp);
      fwrite(&grid[i][j].valoarea_pentru_verde, 1, sizeof(unsigned char), fp);
      fwrite(&grid[i][j].valoarea_pentru_albastru, 1, sizeof(unsigned char),
             fp);
    }
  }
}

void freeQuadtree(Tree *node) {
  if (node == NULL) {
    return;
  }

  freeQuadtree(node->square_1);
  freeQuadtree(node->square_2);
  freeQuadtree(node->square_3);
  freeQuadtree(node->square_4);

  free(node);
}
