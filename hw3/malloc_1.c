#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

//set the debug_flag on or off to print the debug statements
bool debug_flag = true;



typedef struct node
{
    size_t size;
    struct node* next;
    int debug;
    //int magic; // For debugging, for now.
} node_f;
// to be removed later
//#define PAGESIZE 500

int META_SIZE =  sizeof(struct node);

//initailly the bins are initalized to null
node_f* bin_8 = NULL;
node_f*  bin_64 = NULL;
node_f*  bin_512 = NULL;

// all calls above 512 bytes are to be done using mmap
node_f* request_space_from_heap(node_f *head_node, size_t bin_size)
{
    size_t PAGESIZE = sysconf(_SC_PAGESIZE);
    node_f* block;
    block = sbrk(0); // current location of program break
    void *request = sbrk(PAGESIZE);
    assert((void*)block == request);
    if(request == (void*) -1)
    {
        return NULL; //sbrk failed
    }
    // if the node is NULL of first request
    // I will try remove this as I am only going to request
    // when the lists are empty
    if(!head_node)
    {
        head_node = block;
    }
    int size_of_blocks = META_SIZE+bin_size;
    int no_of_blocks = PAGESIZE/size_of_blocks;
    if(debug_flag)
    {
        printf("no_of_blocks : %d\n", no_of_blocks);
    }
    node_f* current_node = block;
    current_node -> debug = 1;


    for(int i = 2;i <= no_of_blocks; i++)
    {

        // This is really important!!
        // change current_node to char* so that we can do addition byte wise
        node_f* temp = (node_f*)((char*)current_node + size_of_blocks);

        current_node->next = temp;
        current_node = temp;
        current_node -> debug = i;
        if(debug_flag)
        {
            printf("debug : %d\n", current_node->debug);
        }

    }
    current_node -> next = NULL;



    // point it back to original place
    return head_node;
}


void main(){


    printf("META_SIZE: %d\n", META_SIZE);

    void *x = request_space_from_heap(bin_64, 64);
}
