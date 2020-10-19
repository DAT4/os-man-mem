#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include "mymem.h"

void check_head();
void print_blk();

//  MODELS
typedef struct ram
{
    struct ram *prev;
    struct ram *next;
    int size;            
    char alloc;          
    void *ptr;           
} ram;

//  GLOBALS
strategies myStrategy = Worst;    
size_t mySize;
void *myMemory = NULL;
static ram *head; 
static ram *largestFree; 

//  INITIATE BASE MEMORY
void initmem(strategies strategy, size_t sz)
{
	myStrategy  = strategy;
	mySize      = sz;
	if (myMemory != NULL) free(myMemory); 
    ram *base = malloc(sizeof(ram));

    base->size  = sz;
    base->prev  = NULL;
    base->next  = NULL;
    base->alloc = 0;
    base->ptr   = 0;
    head = base;
    largestFree = base;

	myMemory = malloc(sz);
}

//  ALLOCATE MEMORYBLOCKS
void *mymalloc(size_t requested)
{

    mem_largest_free();

    ram *new = malloc(sizeof(ram));

    //PTR
    new->ptr = largestFree->ptr;
    largestFree->ptr += requested;

    //SIZE
    largestFree->size -= requested;
    new->size = requested;

    new->alloc = 1;

    // SET BLOCK TO HEAD IF FIRST
    if(new->ptr == 0)
        head = new;
    else
        largestFree->prev->next = new;

    //NEXT/PREV
    new->next = largestFree;
    new->prev = largestFree->prev;
    largestFree->prev = new;

    return new;
}

//  FREE MEMORYBLOCKS
void myfree(void* block)
{
    ram *tmp = block;
    tmp->alloc = 0;

    if(tmp->next)
        if(!tmp->next->alloc)
        {
            myfree(tmp->next);
        }
    if(tmp->prev)
         if(!tmp->prev->alloc)
         {
             tmp->prev->next = tmp->next;
             tmp->next->prev = tmp->prev;
             tmp->prev->size += tmp->size;
             free(tmp);
         }
}

//  FIND HOW MANY FREE MEMORYBLOCKS IN BETWEEN
int mem_holes()
{
    int x = 0;
    ram *tmp = head;
    while(tmp)
    {
        if(!tmp->alloc)
            x++;
        tmp = tmp->next;
    }
	return x;
}

//  FIND HOW MUCH ALLOCATED MEMORY
int mem_allocated()
{
    int x = 0;
    ram *tmp = head;
    while(tmp)
    {
        if(tmp->alloc)
            x += tmp->size;
        tmp = tmp->next;
    }
	return x;
}

int mem_free()
{
    int x = 0;
    ram *tmp = head;
    while(tmp)
    {
        if(!tmp->alloc)
            x += tmp->size;
        tmp = tmp->next;
    }
	return x;
}

int mem_largest_free()
{
    ram *tmp = head;
    while(tmp)
    {
        if(tmp->size > largestFree->size && !tmp->alloc)
            largestFree = tmp;
        tmp = tmp->next;
    }
	return largestFree->size;
}

int mem_small_free(int size)
{
    ram *small = head;
    ram *tmp = head;
    while(tmp)
    {
        if(tmp->size < small->size)
            small = tmp;
        tmp = tmp->next;
    }
	return small->size;
}       

char mem_is_alloc(void *ptr)
{
    ram *tmp = head;
    while(tmp)
        if(tmp->ptr == ptr)
            if(tmp->alloc)
                return 1;
            else
                return 0;
    return -1;
}
void *mem_pool()
{
	return myMemory;
}
int mem_total()
{
	return mySize;
}
char *strategy_name(strategies strategy)
{
    return "worst";
}
strategies strategyFromString(char * strategy)
{
	return Worst;
}
void print_memory()
{
    check_head(head);
	return;
}
 
//  SUMMARIZE THE CURRENT STATUS OF THE MEMORY
void print_memory_status()
{
	printf("%d out of %d bytes allocated.\n",mem_allocated(),mem_total());
	printf("%d bytes are free in %d holes; maximum allocatable block is %d bytes.\n",mem_free(),mem_holes(),mem_largest_free());
	printf("Average hole size is %f.\n\n",((float)mem_free())/mem_holes());
}

//  TEST THE SYSTEM 
void try_mymem(int argc, char **argv) 
{
	ram *a, *b, *c, *d, *e;
	initmem(Worst,500);

	a = mymalloc(100);
	b = mymalloc(100);
	c = mymalloc(100);
	myfree(b);
	d = mymalloc(50);
	myfree(a);
	e = mymalloc(25);

	print_memory();
	print_memory_status();
}

void check_head(ram *tmp)
{
    print_blk(tmp);
    if(tmp->next)
        check_head(tmp->next);
}

void print_blk(ram *x)
{
    printf("PTR:\t0x%x\t HAS\t%i\t SIZE AND IS%s ALLOCATED\n",
            x->ptr,
            x->size,
            x->alloc ? "" : " NOT");
}
