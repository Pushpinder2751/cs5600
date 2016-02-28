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

void* malloc(size_t size)
{
    if(size <= 8)
    {
        if(!bin_8) // first call for 8 bytes
        {
            if(debug_flag)
            {
                printf("First call for 8 bytes\n");
            }
            bin_8 = request_space_from_heap(bin_8, 8);

        }
        void *space = (char*)bin_8 + META_SIZE;
        bin_8->size = 8;
        bin_8 = bin_8->next;
        return space;
    }
    else if(size <=64)
    {
        if(!bin_64) // first call for 8 bytes
        {
            if(debug_flag)
            {
                printf("First call for 64 bytes\n");
            }
            bin_64 = request_space_from_heap(bin_64, 64);

        }
        void *space = (char*)bin_64 + META_SIZE;
        bin_64->size = 64;
        bin_64 = bin_64->next;
        return space;
    }
    else if(size <=512)
    {
        if(!bin_512) // first call for 8 bytes
        {
            if(debug_flag)
            {
                printf("First call for 512 bytes\n");
            }
            bin_512 = request_space_from_heap(bin_512, 512);

        }
        void *space = (char*)bin_512 + META_SIZE;
        bin_512->size = 512;
        bin_512 = bin_512->next;
        return space;
    }
}

// since we need this at multiple places, a function is defined
node_f* get_node_pointer(void* ptr)
{
    return (node_f*)((char*)ptr - META_SIZE);
}

void free(void *ptr)
{
    if(!ptr)
        return;

    // getting position of pointer
    node_f* position_of_ptr = get_node_pointer(ptr);
    // adding the pointer to the front
    if(position_of_ptr->size == 8)
    {
        if(debug_flag)
        {
            printf("freeing 8 bytes of memory\n");
        }
        position_of_ptr->next = bin_8;
        bin_8 = position_of_ptr;
    }
    else if(position_of_ptr->size == 64)
    {
        if(debug_flag)
        {
            printf("freeing 64 bytes of memory\n");
        }
        position_of_ptr->next = bin_64;
        bin_64 = position_of_ptr;
    }
    else if(position_of_ptr->size == 512)
    {
        if(debug_flag)
        {
            printf("freeing 512 bytes of memory\n");
        }
        position_of_ptr->next = bin_512;
        bin_512 = position_of_ptr;
    }
}


void main(){


    printf("META_SIZE: %d\n", META_SIZE);
    void* x = malloc(500);
    free(x);
    //void *x = request_space_from_heap(bin_64, 64);
}
