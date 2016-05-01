#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <ucontext.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

//#include <sys/types/h>
#define BUFFERSIZE 1024

bool x = true;

void interrupt_handler(int sig_num);

	// This code hijacks the control from the main program
	// for us to save it as a checkpoint
	// this function is used to call functoins before the execution of main
	__attribute__ ((constructor))
	void myconstructor()
	{
		//signal(SIGINT,interrupt_handler);
		signal(SIGUSR2,interrupt_handler);
	}

	// This is the function we are going to use to save
	void interrupt_handler(int sig_num)
	{
		printf("\n Signal number : %d",sig_num);
		printf("\n");

		int input_fd = open("/proc/self/maps", O_RDONLY);
		printf("This is the file descriptor for input file: %d \n", input_fd);
		if(input_fd == -1)
		{
			// to print relatively user friendly error
			// message. Use man 3 perror for more.
			perror ("open");
			//printf("Something is wrong! \n", );
		}
		// an image file to write everything into.
		// myckpt is the name of the image as asked in the assignment
		int output_fd = open("myckpt", O_RDWR | O_CREAT | O_APPEND, 0777 );
		printf("This is the file descriptor for output file: %d \n", output_fd);
		if(output_fd == -1)
		{
			// to print relatively user friendly error
			// message. Use man 3 perror for more.
			perror ("open");
			//printf("Something is wrong! \n", );
		}

		// This is a separate image for the registers as it is easier to
		// extract
		int reg_output_fd = open("reg_myckpt", O_RDWR | O_CREAT | O_APPEND, 0777 );
		printf("This is the file descriptor for output file: %d \n", reg_output_fd);
		if(reg_output_fd == -1)
		{
			// to print relatively user friendly error
			// message. Use man 3 perror for more.
			perror ("open");
			//printf("Something is wrong! \n", );
		}




		//ssize_t is an unsigned type, cannot be negative
		//ssize_t in,out; not needed
		char buffer[1000];
		int read_fd = read(input_fd, &buffer, sizeof(buffer));
		printf("read_fd : %d \n", read_fd);
		// to check for error
		if(read_fd == -1)
			perror("read");

		// code to read data form th buffer and save it in a string
		char read_string[BUFFERSIZE];

		int i = 0;
		while(i < read_fd)
		{
			read_string[i] = buffer[i];
			//if(buffer[i] == '\n')
			//{
			//	read_string[i] = '\0';
			//}
			if(i>BUFFERSIZE)
			{
				printf("Max length reached, create bigger string");
			}
			printf("%c",read_string[i]);
			i++;
		}

		// code to interpret the string to usable addresses
		printf("\n");

		// structure to save each line
		struct data
		{
			long long start_pt;
			long long end_pt;
			char permissions[4];
		};
		struct data line1,line2,line3,line4,line5,line6,line7,line8;

		int index = 0;
		void extract_addresses(struct data line)
		{
			char start[64];
			char end[64];
			for(int i = 0; read_string[index] != '-'; index++,i++)
			{
				start[i]=read_string[index];
				printf("%c",start[i]);
			}

			//start[index+1] = '\0';
			printf("\n");
			line.start_pt = strtoll(start,NULL,16);
			printf("%llx", line.start_pt);
			printf("\n");

			index++;// to get past '-'
			//memset(start,0,sizeof(start));
			// ask professer why did this not work!
			for(int i = 0;read_string[index] != ' ' ;index++,i++)
			{
				end[i]=read_string[index];
				printf("%c",end[i]);
			}

			printf("\n");

			line.end_pt = strtoll(end,NULL,16);
			printf("%llx", line.end_pt);
			//printf("%llx", line.end_pt);
			printf("\n");

			//printf("I am here \n");

			index++;//to get past " "
			//memset(start,0,sizeof(start));
			for(int i = 0;read_string[index] != ' ';index++)
			{
				line.permissions[i]=read_string[index];
				printf("%c",line.permissions[i]);
				i++;
			}
			printf("\n");

			//to get to the next line
			while(read_string[index] != '\n'){
				index++;
			}
			index++;//to get past the '\n'

			// writing to output file here
			// writing the address
			write(output_fd, &line, sizeof(struct data));

			// writing the data
			printf("testing : (%llx, %llx, %llx)" , line.start_pt,line.end_pt, (line.end_pt - line.start_pt));
			int data = write(output_fd, (void *)line.start_pt, (line.end_pt - line.start_pt));
			printf("data for this address :  %d \n", data);
			if(data == -1)
				perror("read");

		}
		// to write register data
		ucontext_t context;
		getcontext(&context);
		printf("\nDone with get context\n");
		printf("My x : %d\n",x);
		if(x == true)
		{
			printf("I am in here!\n" );
			write(reg_output_fd, &context, sizeof(ucontext_t));
			x = false;
			extract_addresses(line1);
			extract_addresses(line2);
			extract_addresses(line3);
			extract_addresses(line4);
			extract_addresses(line5);
			extract_addresses(line6);
			extract_addresses(line7);
			extract_addresses(line8);
			exit(0);
		}
		printf("I am done restoring!\n");


	}
