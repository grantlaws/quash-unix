quash: main.o redirec.o cmdline.o
	gcc -o quash main.o cmdline.o redirec.o

main.o: main.c redirec.h cmdline.h
	gcc -c main.c

redirec.o: redirec.c
	gcc -c redirec.c
	
cmdline.o: cmdline.c
	gcc -c cmdline.c
	
clean:
	rm *.o quash
