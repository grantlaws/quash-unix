quash: main.o cmdline.o utilities.o
	gcc -o quash main.o utilities.o cmdline.o

main.o: main.c cmdline.h
	gcc -c main.c

cmdline.o: cmdline.c utilities.h
	gcc -c cmdline.c

utilities.o: utilities.c
	gcc -c utilities.c

clean:
	rm *.o quash
