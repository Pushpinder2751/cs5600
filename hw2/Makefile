build: worker master

worker: worker.c
	gcc worker.c -o worker

master:	master1.c
	gcc -g master1.c -o master1

check: master1

master1:	master
	./master1 -x 2 -n 3 --worker_path worker --wait_mechanism sequential
	./master1 -x 2 -n 3 --worker_path worker --wait_mechanism select
	./master1 -x 2 -n 3 --worker_path worker --wait_mechanism poll
	./master1 -x 2 -n 3 --worker_path worker --wait_mechanism epoll
clean:
	rm -rf worker master1
