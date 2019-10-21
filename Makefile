quash: main.o redirec.o cmdline.o utilities.o
	gcc -o quash main.o utilities.o redirec.o cmdline.o

main.o: main.c redirec.h cmdline.h
	gcc -c main.c

redirec.o: redirec.c utilities.h
	gcc -c redirec.c

cmdline.o: cmdline.c utilities.h
	gcc -c cmdline.c

utilities.o: utilities.c
	gcc -c utilities.c

clean:
	rm *.o quash
