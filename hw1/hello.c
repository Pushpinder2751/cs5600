#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
	int x = getpid();
	printf("%d \n", x);
	int i = 0;
	while(1)
	{
		printf("%d \t", i);
		i++;
		fflush(stdout);
		//delay in seconds
		sleep(1);
	}
}
