build:	hello myrestart

libckpt:
	gcc -c -fno-stack-protector -o myckpt.o myckpt.c && ar rcs libckpt.a myckpt.o

hello:	hello.c	libckpt
	gcc -g -fno-stack-protector -static -L`pwd` -lckpt -Wl,-u,myconstructor hello.c -o hello

myrestart:	myrestart.c
	gcc myrestart.c -o myrestart -g -O0 -static -Wl,-Ttext=5000000 -Wl,-Tdata=5100000 -Wl,-Tbss=5200000

restart:	myrestart
	./myrestart myckpt reg_myckpt

check: clean	hello
	(sleep 2 && kill -12 `pgrep -n hello` && sleep 2 && pkill -9 -n hello) &
	./hello
	make restart

clean:
	rm -rf myckpt reg_myckpt myrestart hello
