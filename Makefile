quash: main.o utilities.o
	gcc -o quash main.o utilities.o

main.o: main.c utilities.h
	gcc -c main.c

utilities.o: utilities.c
	gcc -c utilities.c

clean:
	rm *.o quash
