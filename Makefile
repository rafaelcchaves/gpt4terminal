all: main
	gcc main.o -o main 
main:
	gcc -c main.c -o main.o
