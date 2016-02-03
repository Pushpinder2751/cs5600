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
                x = atoi(argv[i+1]);
            else if(strcmp(argv[i],"-n") == 0)
                n = atoi(argv[i+1]);
        }
        printf("x = %d , n = %d \n", x, n);
        char char_x[24] = {0x0}, char_n[24] = {0x0};
        sprintf(char_x, "%d", x);

        double e_x=0;

        //fd for pid
        // 10 will have to be have to be changed according to n
        int child_no = n; // just for testing on few children
        int fd[child_no][2];
        char my_value[6];
        // child_no is to be updated to the value of n
        pid_t childpid[child_no];

        for(int i = 0;i <= child_no; i++)
            {

                // check if pipe was created successfully
                if( pipe(fd[i]) == -1)
                {
                        printf("child no : %d \n", i);
                        perror("pipe");
                        exit(EXIT_FAILURE);
                }


                printf("hellow world, Parent : (pid:%d)\n", (int)getpid());
                // forking for each child
                childpid[i] = fork();

                if(childpid[i] < 0) // fork failed; exit
                {
                        printf("child no : %d \n", i);
                        perror("fork");
                        exit(1);
                }
                else if (childpid[i] == 0) // child process
                {
                        // child closes the read/input side of pipe
                        close(fd[i][0]);
                        printf("Hello, I am child no. %d and (pid:%d)\n",i, (int)getpid());

                        // duplicating the std output of child to fd[1]
                        dup2(fd[i][1], 1);

                        sprintf(char_n,"%d", i);
                        // How to use exec()
                        execl("worker", "worker","-n", char_n, "-x", char_x, NULL);
                        // this will not execute if execl is used.
                        // prinf("excel did not work \n");
                        //my_value = 10;
                        //write to the pipe
                        //write(fd[1], &my_value, sizeof(my_value ));
                        close(fd[i][1]); // closing write/output in pipe as
                        exit(0);
                }
                else  // parent goes down this path
                {
                        //closing write/output side of pipe
                        close(fd[i][1]);

                        // waiting for the child to finish
                        // This is the sequential method.
                        int wc = wait(NULL);

                        printf("Hello, I am parent of %d (wc: %d) (pid:%d)\n", childpid[i],wc, (int)getpid());
                        //reading value fromthe executed process
                        read(fd[i][0], &my_value, sizeof(my_value));

                        printf("I read value from my child as : %s \n",my_value);

                        double x = atof(my_value);
                        printf("My Float : %.4f\n", x);
                        e_x += x;
                        printf("updated e_x functin %.4lf \n", e_x);
                        //closing the read/input side of pipe as well
                        close(fd[i][0]);
                }
        } // closing for
return 0;
}
