
all: library.a library.so clean

library.o: library.c library.h
	gcc -c -Wall library.c -o library.o

library.a: library.o
	ar rcs library.a library.o
	
library2.o: library.c library.h
	gcc -Wall -fPIC -c library.c -o library2.o
	
library.so: library2.o
	gcc -shared -o liblibrary.so library2.o

clean:
	rm -f *.o
