build: threads


threads: threads1.c
	gcc -g threads1.c -o threads1 -pthread

check: threads1

threads1:	threads
	./threads1 -x 2 -n 3
	./threads1 -x 5 -n 30
clean:
	rm -rf threads1
