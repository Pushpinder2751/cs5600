#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <ucontext.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/mman.h>

#define BUFFERSIZE 1024



int main(int argc, char *argv[])
{
    // code to change the location of the stack pointer
    // need to change it so that it does not conflict with the process

    void *change_stack_pointer = mmap((void *)(0x5300000),0x10000, PROT_READ|PROT_WRITE|PROT_EXEC, MAP_PRIVATE|MAP_FIXED|MAP_ANONYMOUS | MAP_POPULATE,-1,0);
    asm volatile ("mov %0,%%rsp;" : : "g" ((void *)0x5301000):"memory");
    //restore_memory();

    //I am skipping this for now. Use it if you are not getting any further.
    /*
    int input_fd = open("/proc/self/maps", O_RDONLY);
    printf("This is the file descriptor for input file: %d \n", input_fd);
    if(input_fd == -1)
    {
        // to print relatively user friendly error
        // message. Use man 3 perror for more.
        perror ("open");
        //printf("Something is wrong! \n", );
    }

*/
    //Question: Is the 6th line in /proc/pid/maps always going to be stack?
    // Answer : NO!
    int image_fd = open(argv[1], O_RDWR);
    printf("This is the file descriptor for image file: %d \n", image_fd);
    if(image_fd == -1)
    {
        // to print relatively user friendly error
        // message. Use man 3 perror for more.
        perror ("open");
        //printf("Something is wrong! \n", );
    }

    // to get the register values

    int reg_output_fd = open(argv[2], O_RDWR | O_CREAT | O_APPEND, 0777 );
    printf("This is the file descriptor for output file: %d \n", reg_output_fd);
    if(reg_output_fd == -1)
    {
        // to print relatively user friendly error
        // message. Use man 3 perror for more.
        perror ("open");
        //printf("Something is wrong! \n", );
    }

    ucontext_t context;
    read(reg_output_fd, &context, sizeof(ucontext_t));


    struct data
    {
        long long start_pt;
        long long end_pt;
        char permissions[4];
    };
    struct data line1,line2,line3,line4,line5,line6,line7,line8;
    char buffer[1000];


    void extract_addresses_data(struct data line)
    {
        int read_image_fd =  read(image_fd, &line, sizeof(struct data));
        printf("image_fd:%d\n",read_image_fd);
        //perror("error read from file");
        printf("%llx \n", line.start_pt);
        printf("%llx \n", line.end_pt);
        //printf("bug3\n");
        printf("%llx \n", (line.end_pt - line.start_pt));

        munmap((void *)line.start_pt, (line.end_pt - line.start_pt));
        printf("bug1\n");

        void *p = mmap((void *)line.start_pt, (line.end_pt - line.start_pt), PROT_READ|PROT_WRITE|PROT_EXEC, MAP_PRIVATE|MAP_FIXED|MAP_ANONYMOUS | MAP_POPULATE,-1,0);
        printf("bug2 \n");
        int data = read(image_fd, (void *)line.start_pt, (ssize_t)(line.end_pt - line.start_pt));
        if(data == -1)
            perror("read");

        printf("data for this address :  %d \n", data);
    }

    extract_addresses_data(line1);
    extract_addresses_data(line2);
    extract_addresses_data(line3);
    extract_addresses_data(line4);
    extract_addresses_data(line5);
    extract_addresses_data(line6);
    extract_addresses_data(line7);
    extract_addresses_data(line8);
    setcontext(&context);
}
