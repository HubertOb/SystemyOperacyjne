
all: main.o static shared dynamic clean

main.o: main.c
	gcc -Wall -c main.c

static:
	gcc -Wall main.o ../zad1/library.a -o program

shared:
	gcc -Wall main.c -o program_shared -L ../zad1 -l library
	
dynamic:
	gcc -c -fPIC -L. -o program_dynamic main_dynamic.c -ldl -D DYNAMIC
	
clean:
	rm -f *.o
	


