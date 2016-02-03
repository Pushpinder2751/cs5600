#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>

int main(int argc, char *argv[])
{
        int x, n;
        for(int i = 0;i < argc; i++)
        {
            if(strcmp(argv[i],"-x") == 0)
                x = i+1;
            else if(strcmp(argv[i],"-n") == 0)
                n = i+1;
        }
        //printf("x = %.4f , n = %.4f \n", x, n);

        //fd for pid
        // 10 will have to be have to be changed according to n
        int fd[2];
        char my_value[6];

        pid_t childpid;

        // check if pipe was created successfully
        if( pipe(fd) == -1)
        {
                perror("pipe");
                exit(EXIT_FAILURE);
        }


        printf("hellow world (pid:%d)\n", (int)getpid());
        childpid = fork();

        if(childpid <0) // fork failed; exit
        {
                perror("fork");
                exit(1);
        }
        else if (childpid == 0) // child process
        {
                // child closes the read/input side of pipe
                close(fd[0]);
                printf("Hello, I am child (pid:%d)\n", (int)getpid());

                // duplicating the std output of child to fd[1]
                dup2(fd[1], 1);
                // How to use exec()
                execl("worker", "worker","-n", argv[n], "-x", argv[x], NULL);
                // this will not execute if execl is used.
                // prinf("excel did not work \n");
                //my_value = 10;
                //write to the pipe
                //write(fd[1], &my_value, sizeof(my_value ));
                close(fd[1]); // closing write/output in pipe as
                exit(0);
        }
        else  // parent goes down this path
        {
                //closing write/output side of pipe
                close(fd[1]);

                // waiting for the child to finish
                int wc = wait(NULL);

                printf("Hello, I am parent of %d (wc: %d) (pid:%d)\n", childpid, wc, (int)getpid());
                //reading value fromthe executed process
                read(fd[0], &my_value, sizeof(my_value));

                printf("I read value from my child as : %s \n",my_value);

                float x = atof(my_value);
                printf("My Float : %.4f\n", x);
                //closing the read/input side of pipe as well
                close(fd[0]);
        }
    return 0;
}
