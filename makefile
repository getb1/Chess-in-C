all:
	gcc -o  main misc.c board.c main.c
	./main

	rm main