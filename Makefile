#compile with gcc with following flags
CC = gcc
CFLAGS = -Wall -g -Wextra
#for math.h
LIBS = -lm
#src files
SRC = main.c Tree.c
OBJ = $(SRC:.c=.o)
#name of exec
EXEC = quadtree 
threshold = 2000
#build rule for exec
build: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(EXEC) $(LDFLAGS) -lm

main.o: main.c Tree.h
	$(CC) $(CFLAGS) -c  main.c

Tree.o: Tree.c Tree.h
	$(CC) $(CFLAGS) -c Tree.c
#clean rule
clean:
	rm -f $(OBJ) $(EXEC)
#run rule
run:
	./$(EXEC) -d test0_c2.out output.ppm
#make valgrind => checks memory 
valgrind:
	valgrind --leak-check=full --show-leak-kinds=all ./$(EXEC) -d test0_c2.out output.ppm
#WARNING this makefile works only for the arguments I gave