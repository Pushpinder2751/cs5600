This is the Readme for HW3. The assignment wants us to create a custom malloc
implementation which is also thread safe with proper locking.

Design Overview: In this progrom I have used separate bins for 8, 64, 512 bytes of memory.
Memory requests greater than 500bytes are requested using mmap system call.
The bins are basically linked lists which contain the address and meta data for each memory
segment. On a free call, the memory is put back to the respective lists again. The program passes
the t-test1 provided by the professor successfully. 

Make file includes
all : performs check
libmalloc.so : creates shared objects file from malloc.o
t-test1 : builds the test file given provided by the teacher
check : runs the test file with the shared object
clean: removes the executables
