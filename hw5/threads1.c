#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>

#define NUM_THREADS 1

double factorial(double n);
double power(double x, double y);
void final_calculation(int i, double result);
int global_x, global_n;
double e_x = 0;

// pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

//int worker_path;


// need to execute my worker instead of this
void *do_work(void *threadid)
{
    //pthread_mutex_lock( &mutex1);
    long tid;
    tid = (long)threadid;
    //printf("%s\n", worker_path_string);
    // this wont work for now. I am not sure why.
    // execl(worker_path_string,"worker","-n", char_n, "-x", char_x, NULL);
    double* result = (double*) malloc(sizeof(double));
    *result = (power(global_x,global_n)) / (factorial(global_n));

    printf("Hello World! It's me, thread #%ld!\n", tid);
    printf("result : %lf\n", *result);
    //pthread_mutex_unlock( &mutex1 );
    //return result;

    pthread_exit((void*)result);

}


int main(int argc, char* argv[]){

    // to extract the essentials from argv
    int x,n, no_of_threads;

    printf("No. of arguements : %d\n", argc);
    // pulling out essentials form argv, ideally need to use getopt(),
    // but don't know how to do that.
    for(int i = 1;i < argc; i++)
    {
        if(strcmp(argv[i],"-x") == 0)
            x = atoi(argv[i+1]);
        else if(strcmp(argv[i],"-n") == 0)
            n = atoi(argv[i+1]);
    }

    printf("x : %d \nn : %d \n", x, n);
    no_of_threads = n;
//  don't think I need global_x
    global_x = x;
    global_n = n;



    pthread_t threads[no_of_threads];
    int rc;
    long t;
    void *result_ptr;
    for(t = 0; t < no_of_threads; t++){
        printf("In main: creating thread %ld\n", t);
        global_n = t;
        rc = pthread_create(&threads[t], NULL, do_work, (void *)t);
        printf("rc for this thread was : %d\n", rc);
        if(rc){
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
        printf("thread NO: %ld\n", t);
        pthread_join(threads[t], &result_ptr );
        double result_main = *(double*) result_ptr;
        final_calculation(t, result_main);


        free(result_ptr);
    }
    printf("ans: %lf\n", e_x);
    pthread_exit(NULL);
}



double factorial(double n)
{
    if(n == 0)
        return 1;
    else
        return n*factorial(n-1);
}

double power(double x, double y)
{
    double temp;
    if( y == 0)
       return 1;
    temp = power(x, y/2);
    if ((int)y%2 == 0)
        return temp*temp;
    else
    {
        if(y > 0)
            return x*temp*temp;
        else
            return (temp*temp)/x;
    }
}


// does the final additions
void final_calculation(int i, double result)
{
    double x = result;
    e_x += x;
    printf("Worker: %d, My Float : %.4lf function %.4lf\n", i, x, e_x);
}
