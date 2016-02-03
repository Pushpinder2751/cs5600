#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <math.h>

int factorial(int n);
float power(float x, int y);

int main(int argc, char *argv[])
{
    float x= 0;
    float n= 0;
    int opt;
    while((opt = getopt(argc, argv, "x:n:")) != -1)
    {
        switch(opt)
        {
            case 'x':
                x = atof(optarg);
                break;
            case 'n':
                n = atof(optarg);
                break;
            default:
                printf("wrong arguments");
        }
    }
    //printf("x = %.4f , n = %.4f \n", x, n);
    float result = power(x,n)/factorial((int)n);
    printf("%f", result);


}

int factorial(int n)
{
    if(n != 1)
        return n*factorial(n-1);
}

float power(float x, int y)
{
    float temp;
    if( y == 0)
       return 1;
    temp = power(x, y/2);
    if (y%2 == 0)
        return temp*temp;
    else
    {
        if(y > 0)
            return x*temp*temp;
        else
            return (temp*temp)/x;
    }
}
