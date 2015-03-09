/**
 * @file:   k_memory.h
 * @brief:  kernel memory managment header file
 * @author: GG (yes re), Reesey, RayMak, and LJ
 * @date:   2015/02/01
 */
 
#ifndef K_MEM_H_
#define K_MEM_H_

#include "k_rtx.h"
#include "queue.h"

/* ----- Definitions ----- */
#define RAM_END_ADDR 0x10008000
#define BLOCK_SIZE 128

/* ----- Type Definitions Variables ----- */
typedef struct Block Block;
struct Block { //fixed size, defined above
	Block* next;
	int pid;
} ;


/* ----- Variables ----- */
/* This symbol is defined in the scatter file (see RVCT Linker User Guide) */  
extern unsigned int Image$$RW_IRAM1$$ZI$$Limit; 

extern PCB **gp_pcbs;
extern PCB *gp_current_process;
extern PROC_INIT g_proc_table[NUM_PROCS];
extern Queue **ready_qs;								/* ready queue*/
extern Queue **blocked_resource_qs;			/* blocked resources queue*/


/* ----- Functions ------ */
int k_release_processor(void);
void memory_init(void);
U32 *alloc_stack(U32 size_b);
void *k_request_memory_block(void);
void *k_request_element(void);
int k_release_memory_block(void *);
int getMSP (void);
int getTotalFreeMemory(void);
int k_get_total_num_blocks(void);
int k_release_element_block (void *);

extern Element* pop(Queue* self);
extern void push(Queue* self, Element* element);
extern void setReadyQ(int priority, Queue* q);
extern void setBlockedResourceQ(int priority, Queue* q);


#endif /* ! K_MEM_H_ */
