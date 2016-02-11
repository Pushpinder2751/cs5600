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


void final_calculation(int i);
void compute_by_select(int n, int fd[][2]);
void set_mechanism_flag(char flag[]);
int max_array(int p[][2], int num_elements);
// have declared the variables here as if I declare them
// inside main, throws an error of 'Use them first'
fd_set fds;

// flags for wait_mechanism
bool sequential, select_flag, poll_flag,epoll_flag;
// I want to know the reason why i have to keep it here instead of main
// as I get the bus error otherwise.
int worker_path;


// this is the structure for select();
struct timeval tv = {.tv_sec = 5, .tv_usec = 0};

double e_x=0;
char my_value[8];

int main(int argc, char *argv[])
{
        int x, n;

        //sequential = true;
        FD_ZERO(&fds);

        printf("No. of arguements : %d\n", argc);
        // pulling out essentials form argv, ideally need to use getopt(),
        // but don't know how to do that.
        for(int i = 1;i < argc; i++)
        {
            if(strcmp(argv[i],"-x") == 0)
                x = atoi(argv[i+1]);
            else if(strcmp(argv[i],"-n") == 0)
                n = atoi(argv[i+1]);
            else if(strcmp(argv[i], "--wait_mechanism" ) == 0)
            {
                 set_mechanism_flag(argv[i+1]);
                 printf("wait_mechanism : %s\n", argv[i+1]);
            }
            else if(strcmp("--worker_path",argv[i]) == 0)
            {
                worker_path = i + 1;
                printf("path of worker: %s\n", argv[i+1]);
            }
        }
        // this needs to be here, after the n gets it value
        // this is for poll
        struct pollfd poll_number[(int)n];
        // this is for epoll
        struct epoll_event event;
        int epollfd = epoll_create(n);
        if(epollfd == -1)
        {
            perror("epoll_create");
            exit(EXIT_FAILURE);
        }

        printf("x = %d , n = %d \n", x, n);
        char char_x[24] = {0};
	    char char_n[24] = {0};

        sprintf(char_x, "%d", x);

        //fd for pid
        // 10 will have to be have to be changed according to n
        int child_no = n; // just for testing on few children
        int fd[child_no][2];

        // child_no is to be updated to the value of n
        pid_t childpid[child_no];

        for(int i = 0;i < child_no; i++)
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
                        execl(argv[worker_path],"worker","-n", char_n, "-x", char_x, NULL);
                        // this will not execute if execl is used.
                         printf("excel did not work \n");


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

                                printf("I read value from my child no, %d: %s \n",i,my_value);
                                final_calculation(i);
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

                            event.data.fd = fd[i][0];
                            event.events = EPOLLIN;

                            if(epoll_ctl(epollfd, EPOLL_CTL_ADD, fd[i][0], &event) == -1)
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

                for(int i =0;i < n; i++)
                {
                        read(poll_number[i].fd,my_value, sizeof(my_value) );
                        final_calculation(i);

                }
            }
        }

        else if(epoll_flag)
        {
            struct epoll_event events[n];
            int child_process = n;
            for(;;)
            {
                printf("child process : %d\n", child_process);
                fflush(stdout);
                if(child_process <= 0)
                    break;

                int e = epoll_wait(epollfd, events, n+1, 1);
                if(e == -1)
                {
                    perror("epoll_pwait");
                    exit(EXIT_FAILURE);
                }

                for(int i =0;i < e; i++)
                {
                    if((events[i].events & EPOLLIN) != 0)
                    {

                            if(read(events[i].data.fd, my_value, sizeof(my_value) ) == -1)
                            {
                                perror("while reading");
                                //child_process--;
                            }
                            else
                            {
                                printf("reading for worker : %d\n", i);
                                final_calculation(i);

                                epoll_ctl(epollfd, EPOLL_CTL_DEL, events[i].data.fd, NULL);


                                //close(ev[i].data.fd);
                                child_process--;
                            }

                    }

                }
            }
        }
return 0;


}
// not perfect ; need to understand select again
void compute_by_select(int n, int fd[][2])
{
        printf("Usnig select\n");
        //int select_no = n;
        int child_process = n;
        //printf()
        while(1)
        {
            printf("child_process %d\n", child_process);
            fflush(stdout);
            if(child_process <= 0)
                break;

            FD_ZERO(&fds);
            for(int i = 0; i < n; i++)
                FD_SET(fd[i][0], &fds);

            // the first arguement should be the max +1 of the highest
            // file descriptors
            int max_fd = max_array(fd, n)+1;
            printf("max_fd : %d\n", max_fd);
            int select_no = select(max_fd, &fds, NULL, NULL, &tv);
            //printf("select_no : %d\n", select_no);

                // how to know what file descriptors are ready
                printf("Number of Fds ready1 : %d \n ", select_no);

                // this works but I still do not understand select properly

                    int sd;
                    printf("n: %d\n", n);
                    for(int i = 0;i <  n; i++)
                    {
                        sd = fd[i][0];
                        if(sd == -1)
                        {}
                        else if((FD_ISSET(sd, &fds)))
                        {
                            printf("i is :%d \n", i);
                            read(sd, &my_value, sizeof(my_value));
                            final_calculation(i);

                            fd[i][0] = -1;

                            // Is there a point of doing this?
                            //FD_CLR(sd, &fds);
                            child_process--;
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

void final_calculation(int i)
{
    double x = atof(my_value);
    e_x += x;
    printf("Worker: %d, My Float : %.4lf function %.4lf\n, my_value %s\n", i, x, e_x, my_value);
}

int max_array(int p[][2], int num_elements)
{
   int i, max=-32000;
   for (i=0; i<num_elements; i++)
   {
	 if (p[i][0]>max)
	 {
	    max=p[i][0];
	 }
   }
   return(max);
}
