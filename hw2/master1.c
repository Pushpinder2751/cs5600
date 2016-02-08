#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include <stdbool.h>
#include <sys/select.h>
#include <sys/poll.h>
#include <sys/epoll.h>



double final_calculation();
void compute_by_select(int n, int fd[][2]);
void set_mechanism_flag(char flag[]);
// have declared the variables here as if I declare them
// inside main, throws an error of 'Use them first'
fd_set fds;

// flags for wait_mechanism
bool sequential, select_flag, poll_flag,epoll_flag;




// this is the structure for select();
struct timeval tv = {.tv_sec = 5, .tv_usec = 0};

double e_x=0;
char my_value[6];

int main(int argc, char *argv[])
{
        epoll_flag = true;
        FD_ZERO(&fds);
        int x, n, worker_path;
        struct pollfd poll_number[(int)n];

        struct epoll_event ev[(int)n];
        int epollfd;
        epollfd = epoll_create((int)n);
        if(epollfd == -1)
        {
            perror("epoll_create");
            exit(EXIT_FAILURE);
        }



        // pulling out essentials form argv, ideally need to use getopt(),
        // but don't know how to do that.
        for(int i = 1;i < argc; i++)
        {
            if(strcmp(argv[i],"-x") == 0)
                x = atoi(argv[i+1]);
            else if(strcmp(argv[i],"-n") == 0)
                n = atoi(argv[i+1]);
            //else if(strcmp("--worker_path",argv[i]) == 0)
            //    worker_path = i + 1;
            //else if(strcmp(argv[i], "--wait_mechanism" ) == 0)
            //    set_mechanism_flag(argv[i+1]);

        }
        printf("x = %d , n = %d \n", x, n);
        char char_x[24] = {0x0}, char_n[24] = {0x0};
        sprintf(char_x, "%d", x);




        int return_value;




        //fd for pid
        // 10 will have to be have to be changed according to n
        int child_no = n; // just for testing on few children
        int fd[child_no][2];

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

                printf("i is : %d\n", i);
                printf("hello world, Parent : (pid:%d)\n", (int)getpid());
                fflush(stdout);
                // forking for each child
                childpid[i] = fork();
                printf("forked! \n");
                fflush(stdout);
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
                        fflush(stdout);

                        // duplicating the std output of child to fd[1]
                        dup2(fd[i][1], 1);

                        sprintf(char_n,"%d", i);
                        // How to use exec()
                        execl("worker","worker","-n", char_n, "-x", char_x, NULL);
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


                        if (sequential)
                            {
                                printf("sequential\n");
                                // waiting for the child to finish
                                // This is the sequential method.
                                int wc = wait(NULL);
                                printf("Hello, I am parent of %d (wc: %d) (pid:%d)\n", childpid[i],wc, (int)getpid());

                                //reading value fromthe executed process
                                read(fd[i][0], &my_value, sizeof(my_value));

                                printf("I read value from my child as : %s \n",my_value);
                                final_calculation();
                                //closing the read/input side of pipe as well
                                close(fd[i][0]);
                            }
                        else if(select_flag)
                        {
                            printf("Adding pipe no : %d to the fds\n", i);
                            fflush(stdout);
                            // add fds in fd_set here.
                            printf("fd : %d\n", fd[i][0]);
                            FD_SET(fd[i][0], &fds);

                        }
                        else if(poll_flag)
                        {
                            printf("Polling!\n");
                            poll_number[i].fd = fd[i][0];
                            poll_number[i].events = POLLIN | POLLPRI ;

                        }
                        else if(epoll_flag)
                        {
                            printf("E_Polling!\n");

                            ev[i].events = EPOLLIN;
                            ev[i].data.fd = fd[i][0];
                            if(epoll_ctl(epollfd, EPOLL_CTL_ADD, fd[i][0], &ev[i]) == -1)
                            {
                                perror("epoll_ctl");
                                exit(EXIT_FAILURE);
                            }

                        }



                } // closing parent else
        } // closing for
        if(select_flag)
        {
            printf("Bug1 \n");
            compute_by_select(n, fd);
        }
        else if (poll_flag)
        {
            int p = poll(poll_number, n, 5000);
            if(p == -1)
            {
                perror("poll");
            }
            else if(p == 0)
            {
                perror("timeout!");

            }
            else
            {

                for(int i =0;i <= n; i++)
                {
                        read(poll_number[i].fd,my_value, sizeof(my_value) );
                        final_calculation();

                }
            }
        }

        else if(epoll_flag)
        {
            int child_process = n;
            for(;;)
            {
                printf("child process : %d\n", child_process);
                if(!child_process)
                    break;

                int e = epoll_wait(epollfd, ev, n+1, 0);
                if(e == -1)
                {
                    perror("epoll_pwait");
                    exit(EXIT_FAILURE);
                }

                for(int i =0;i <= n; i++)
                {
                    if(ev[i].events & EPOLLIN != 0)
                    {
                        if(read(ev[i].data.fd, my_value, sizeof(my_value) ) == -1)
                            perror("while reading");
                        else
                        {
                            final_calculation();
                            ev[i].data.fd = -1;
                            child_process--;
                        }
                    }

            }
        }

return 0;
    }

}

void compute_by_select(int n, int fd[][2])
{
        printf("Usnig select\n");
        //int select_no = n;
        int child_process = n+1;
        //printf()
        while(1)
        {
            printf("child_process %d\n", child_process);
            fflush(stdout);
            if(!child_process)
                break;


            //for(int i= 0;i<=n;i++)
            //{
            //        FD_SET(fd[i][0], &fds);
            //}
            int select_no = select(FD_SETSIZE, &fds, NULL, NULL, &tv);
            //printf("select_no : %d\n", select_no);

                // how to know what file descriptors are ready
                printf("Number of Fds ready1 : %d \n ", select_no);

                // this works but I still do not understand select properly

                    int sd;
                    for(int i =0;i<= n; i++)
                    {
                        sd = fd[i][0];
                        if((FD_ISSET(sd, &fds)))
                        {   printf("fd is set%d \n", sd);
                            read(sd, &my_value, sizeof(my_value));
                            final_calculation();
                            FD_CLR(sd, &fds);
                            child_process--;
                        }else{   printf("fd is  not set%d \n", sd);
                                FD_SET(sd, &fds);
                            }

                    }


            }
}

void set_mechanism_flag(char flag[])
{
    if(strcmp(flag, "sequential") == 0)
        sequential = true;
    else if (strcmp(flag, "select") == 0)
        select_flag = true;
    else if(strcmp(flag, "poll")== 0)
        poll_flag = true;
    else if(strcmp(flag, "epoll") == 0)
        epoll_flag = true;
    else
        {
            printf("Choose correct wait_mechanism! \n");
            exit(EXIT_FAILURE);
        }
    printf("flag : %s\n", flag);
}

double final_calculation()
{
    double x = atof(my_value);
    printf("My Float : %.4f\n", x);
    e_x += x;
    printf("updated e_x functin %.4lf \n", e_x);
}
