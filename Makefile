quash: main.o redirec.o cmdline.o utilities.o
	gcc -o quash main.o cmdline.o redirec.o utilities.o -lreadline

main.o: main.c redirec.h cmdline.h utilities.h
	gcc -c main.c

redirec.o: redirec.c
	gcc -c redirec.c

cmdline.o: cmdline.c
	gcc -c cmdline.c

utilities.o: utilities.c
	gcc -c utilities.c

clean:
	rm *.o quash
