#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include "mymem.h"

void check_head();

//  MODELS
typedef struct ram
{
    struct ram *prev;
    struct ram *next;
    int size;            
    char alloc;          
    char *ptr;           
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
	myMemory = malloc(sz);

    ram *base = malloc(sizeof(ram));
    base->size  = sz;
    base->prev  = NULL;
    base->next  = NULL;
    base->alloc = 0;
    base->ptr   = 0;
    head = base;
    largestFree = base;
}

//  ALLOCATE MEMORYBLOCKS
void *mymalloc(size_t requested)
{
    ram *new; 
    mem_largest_free(); //Sætter den største ledige som global
    if (largestFree)
    {
        if(largestFree->size > requested)
        {
            new = malloc(sizeof(ram));
            //PTR
            new->ptr = largestFree->ptr;
            largestFree->ptr += requested;
            //SIZE
            largestFree->size -= requested;
            new->size = requested;
            // SET BLOCK TO HEAD IF FIRST
            if(!new->ptr)
                head = new;
            else
                largestFree->prev->next = new;
            //NEXT/PREV
            new->next = largestFree;
            new->prev = largestFree->prev;
            largestFree->prev = new;
        }
        else if (largestFree->size == requested)
        {
            new = largestFree;
        }

        //ALLOCATE 
        new->alloc = 1;

        //RETURN POINTER
        return new->ptr;
    }
    return NULL;
}

//  FREE MEMORYBLOCKS
void myfree(void* block)
{
    char* ptr = block;
    ram *tmp = head;
    int f = 0;
    while(tmp)
    {
        if(tmp->alloc)
        {
        printf("PTR:\t%i\tAGAINST\t%i\n",ptr,tmp->ptr);
            if(tmp->ptr == ptr)
                break;
            else if(tmp->next == NULL)
                return;
        }
        tmp = tmp->next;
    }

    tmp->alloc = 0;

    if(tmp->next)
        if(!tmp->next->alloc)
        {
            myfree(tmp->next->ptr);
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
    largestFree = NULL;
    ram *tmp = head;
    while(tmp)
    {
        if(!tmp->alloc)
        {
            if(!largestFree)
                largestFree = tmp;
            else if(tmp->size > largestFree->size)
                largestFree = tmp;
        }
        tmp = tmp->next;
    }
    if (largestFree)
        return largestFree->size;
    else 
        return 0;
}

int mem_small_free(int size)
{
    int x = 0;
    ram *tmp = head;
    while(tmp)
    {
        if(tmp->size <= size && !tmp->alloc)
            x++;
        tmp = tmp->next;
    }
	return x;
}       

char mem_is_alloc(void *ptr)
{
    ram *tmp = head;
    while(tmp)
    {
        if(tmp->ptr == ptr)
        {
            if(tmp->alloc)
                return 1;
            else
                return 0;
        }
        tmp = tmp->next;
    }
    return 0;
}
void *mem_pool()
{
	return 0;
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

void try_mymem(int argc, char **argv) 
{
    return;
}

void check_head(ram *tmp)
{
    printf("PTR:\t%i\t HAS\t%i\t SIZE AND IS%s ALLOCATED\n",
            tmp->ptr,
            tmp->size,
            tmp->alloc ? "" : " NOT");

    if(tmp->next)
        check_head(tmp->next);
}
