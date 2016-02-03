1)To test the code, use make check.It includes building binaries, running the process,
killing it after 2 seconds, restarting the process from the images, and then cleaning the binaries again.
2)make build compliles the the process file i.e hello.c, myrestart.c
3) make clean cleans removes all the binaries.
4)make hello builds the process.
5)make myrestart makes myrestart.c


Limitations:

I have not cleared the stack pointer for the myrestart.c as I have not run into
any problems with it so far, but it is one of the limitations 
