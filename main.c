/* DOBRE Maria-Adina - 312CD */
#include "Tree.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_LINE_LENGTH 100
//.....................................................
//.....................................................
//.....................................................
int main(int argc, char const *argv[]) {
  if (argc < 1) {
    return 0;
  }
  int threshold = 0; // tolerance for image compression
  FILE *file;        // pointers to files used

  FILE *out;
  FILE *bout;
  FILE *fisierin;
  FILE *fisierout;
  // for tasks one and two...
  if (!strcmp(argv[1], "-c1") || !strcmp(argv[1], "-c2")) {
    file = fopen(argv[3], "rb");
    threshold = atoi(argv[2]); // convert string to int
  }
  if (!strcmp(argv[1], "-c1") || !strcmp(argv[1], "-c2")) {
    int width = 0, height = 0;
    int p255 = 0;
    // read header
    fscanf(file, "P6\n%d %d\n%d", &width, &height, &p255);

    // allocate memory for the grid
    Grid **grid = (Grid **)malloc(height * sizeof(Grid *));

    for (int i = 0; i < height; i++) {
      grid[i] = (Grid *)malloc(width * sizeof(Grid));
    }

    fseek(file, 1, SEEK_CUR);

    // Read the pixels from the file and store them in the grid
    for (int i = 0; i < height; i++) {
      for (int j = 0; j < width; j++) {
        fread(&(grid[i][j].valoarea_pentru_rosu), sizeof(unsigned char), 1,
              file);
        fread(&(grid[i][j].valoarea_pentru_verde), sizeof(unsigned char), 1,
              file);
        fread(&(grid[i][j].valoarea_pentru_albastru), sizeof(unsigned char), 1,
              file);
      }
    }

    // create the initial image using the grid of pixels
    Tree *imagine = insert(0, 0, height, threshold, grid);

    if (!strcmp(argv[1], "-c1")) {
      out = fopen(argv[4], "wt");
      int levels = countLevel(imagine);
      fprintf(out, "%d\n", levels);
      int nrLeaves = countFrunze(imagine);
      fprintf(out, "%d\n", nrLeaves);
      unsigned int dimens = (int)floor(maxLeafSize(imagine));
      fprintf(out, "%d\n", dimens);
      fclose(out);
    }
    if (!strcmp(argv[1], "-c2")) {
      bout = fopen(argv[4], "wb");
      fwrite(&width, 1, sizeof(unsigned int), bout);
      writeOnLevel(bout, imagine);
      fclose(bout);
    }

    fclose(file);
    // free initial image
    for (int i = 0; i < imagine->dimensiune_imagine; i++) {

      free(grid[i]);
    }
    free(grid);

    freeQuadtree(imagine);
  }
  // task 3...
  if (!strcmp(argv[1], "-d")) {
    fisierin = fopen(argv[2], "rb");
    fisierout = fopen(argv[3], "wb");
    Tree *imagine = readFromLevel(fisierin);
    Grid **grid = NULL; // this will be returned as final image
    grid = getGrid(imagine, imagine->dimensiune_imagine);
    toPpm(fisierout, grid, imagine->dimensiune_imagine);
    // free the grid
    for (int i = 0; i < imagine->dimensiune_imagine; i++) {
      free(grid[i]);
    }
    free(grid);
    freeQuadtree(imagine);
    fclose(fisierin);
    fclose(fisierout);
  }

  return 0;
}
