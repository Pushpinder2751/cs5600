#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <math.h>

double factorial(double n);
double power(double x, double y);

int main(int argc, char *argv[])
{
     double x= 0;
     double n= 0;
    int opt;
    while((opt = getopt(argc, argv, "x:n:")) != -1)
    {
        switch(opt)
        {
            case 'x':
                x = atol(optarg);
                break;
            case 'n':
                n = atol(optarg);
                break;
            default:
                printf("wrong arguments");
        }
    }
    //printf("x = %.4f , n = %.4f \n", x, n);
    double result = (power(x,n)) / (factorial(n));
    printf("%lf", result);


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
