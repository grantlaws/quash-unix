quash: main.o
	gcc -o quash main.o

main.o: main.c
	gcc -c main.c

clean:
	rm *.o quash