

build: 
	gcc -Wall 2048.c -lncurses -lpanel -o 2048

run:
	./2048

clean:
	rm -f 2048
