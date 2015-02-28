/**
 * @file:   k_memory.c
 * @brief:  kernel memory managment routines
 * @author: GG (yes re), Reesey, RayMak, and LJ
 * @date:   2015/02/01
 */

#include "k_memory.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* ! DEBUG_0 */

/* ----- Global Variables ----- */
U32 *gp_stack; 
int total_mem_blocks;
/* The last allocated stack low address. 8 bytes aligned */
/* The first stack starts at the RAM high address */
/* stack grows down. Fully decremental stack */
/*git test*/

/* ----- Type Definitions Variables ----- */
typedef struct Block Block;
struct Block { //fixed size, defined above
	Block* next;
	int pid;
} ;

PCB* pop(Queue* self) {
	PCB* element;
	
	if (self == NULL || self->first == NULL) {
		return NULL;
	} else if (self->first->next == NULL) { //queue only has 1 element
		element = self->first;
		element->next = NULL;
		self->first = NULL;
		self->last = NULL;
		return element;
	}
	element = self->first;
	self->first = self->first->next;
	element->next = NULL;
	return element;
};

int push(Queue* self, PCB* pcb) {
	pcb->next = NULL;
	
	if (self->first == NULL) { //queue was formerly empty
			self->first = pcb;
			self->last = pcb;
	} else {
		self->last->next = pcb;
		self->last = pcb;
	}
	self->last->next = NULL;
	
	return RTX_OK;
};

Block* MSP;
//array of queues, organized by priority
Queue* blocked_resource_qs[NUM_PRIORITIES]; 
Queue* ready_qs[NUM_PRIORITIES];

/**
 * @brief: Initialize RAM as follows:

0x10008000+---------------------------+ High Address
          |    Proc 1 STACK           |
          |---------------------------|
          |    Proc 2 STACK           |
          |---------------------------|<--- gp_stack
          |                           |
          |        HEAP               |
          |                           |
          |---------------------------|
          |        PCB 2              |
          |---------------------------|
          |        PCB 1              |
          |---------------------------|
          |        PCB pointers       |
          |---------------------------|<--- gp_pcbs
          |        Padding            |
          |---------------------------|  
          |Image$$RW_IRAM1$$ZI$$Limit |
          |...........................|          
          |       RTX  Image          |
          |                           |
0x10000000+---------------------------+ Low Address

*/

/**
 * @brief pushToReadyQ(). 
 * @param priority of the pcb to push onto the ready Queue
 * @param pointer to the pcb
 * 
 * POST: add pcb to ready queue at index [priority]
 */
void pushToReadyQ (int priority, PCB* p_pcb_old) {
	push(ready_qs[priority], p_pcb_old);
}

/**
 * @brief popFromReadyQ(). 
 * @param priority
 * @return first pcb at priority
 *
 * POST: ready_qs is updated. first pcb of index priority is removed
 */
PCB* popFromReadyQ (int priority) {
	return pop(ready_qs[priority]);
}

/**
 * @brief getReadyQ().
 * @param priority
 * @return returns *Queue stored in ready_qs at index[priority] 
 */
Queue* getReadyQ(int priority) {
	return ready_qs[priority];
}

/**
 * @brief getBlockedResourceQ().
 * @param priority
 * @return returns *Queue stored in blocked_resource_qs at index[priority] 
 */
Queue* getBlockedResourceQ(int priority) {
	return blocked_resource_qs[priority];
}
	
/**
 * @brief getMSP()
 * @return returns address of MSP
 */
int getMSP (void) {
	return (int)MSP;
}

/**
 * @brief memory_init()
 * 
 *PRE:memory is uninitialized
 *POST: allocates memory for heap, 
 *			links memory into blocks of size BLOCK_SIZE
 *			initializes blocked and ready queues
 */
void memory_init(void)
{
	U8 *p_end = (U8 *)&Image$$RW_IRAM1$$ZI$$Limit;
	int i;
	
	/* 4 bytes padding */
	p_end += 4;

	/* allocate memory for pcb pointers   */
	gp_pcbs = (PCB **)p_end;
	p_end += NUM_PROCS * sizeof(PCB *);
	for ( i = 0; i < NUM_PROCS; i++ ) {
		gp_pcbs[i] = (PCB *)p_end;
		p_end += sizeof(PCB); 
	}
	
	/* initializing blocked and ready queues (array of queues, organized by PRIORITY) 
	Currently 1D for blocked, will need to eventually be 2D when we have multiple events */
	for (i = 0; i < NUM_PRIORITIES; i++) {
		Queue* q1 = (Queue *)p_end;
		p_end += sizeof(Queue);
		q1->first = NULL;
		q1->last = NULL;
		/*for (q = 0; i < NUM_TEST_PROCS; q++) {
			BlockedElement *element = (BlockedElement *)p_end;
			p_end += sizeof(BlockedElement);
			if (q1->first == NULL) {
				q1->first = element;
			}
			element->process = NULL;
			element->next = q1->last;
			q1->last = element;
		}
		
		q1->last = q1->first;*/
		blocked_resource_qs[i] = q1;
	}
	
	for ( i = 0; i < NUM_PRIORITIES; i++) {
		Queue* q2 = (Queue *)p_end;
		p_end += sizeof(Queue);
		q2->first = NULL;
		q2->last = NULL;
		
		ready_qs[i] = q2;
	}

	/* prepare for alloc_stack() to allocate memory for stacks */
	gp_stack = (U32 *)RAM_END_ADDR;
	if ((U32)gp_stack & 0x04) { /* 8 bytes alignment */
		--gp_stack; 
	}
	MSP = (Block *)(int)(p_end+4);
	MSP->pid = NULL;
	MSP->next = NULL;
	
	/* allocate memory for heap*/	
	
	for ( i =(int)( p_end + 4 + BLOCK_SIZE); i < (int)(gp_stack - NUM_PROCS * USR_SZ_STACK); i+= BLOCK_SIZE ) {
		Block* current = (Block*)i;
		current->pid = NULL;
		current->next = MSP;
		MSP = current;
		total_mem_blocks ++;
	}
}

/**
 * @brief: allocate stack for a process, align to 8 bytes boundary
 * @param: size, stack size in bytes
 * @return: The top of the stack (i.e. high address)
 * POST:  gp_stack is updated.
 */

U32 *alloc_stack(U32 size_b) 
{
	U32 *sp;
	sp = gp_stack; /* gp_stack is always 8 bytes aligned */
	
	/* update gp_stack */
	gp_stack = (U32 *)((U8 *)sp - size_b);

	/* 8 bytes alignement adjustment to exception stack frame */
	if ((U32)gp_stack & 0x04) {
		--gp_stack; 
	}
	return sp;
}

/**
 * @brief: k_request_memory_block()- 
 *				 removes memory block from free memory lists. returns pointer to free memory block.
 * @return: void *, a pointer to the memory block 
 * POST:  returns pointer to memory block. updates MSP to MSP->next. 
 */
void *k_request_memory_block(void) {
//	atomic(on);
	Block * a;
	int priority;

	while (MSP == NULL) {
		//get the priority of the current process by looking up pid in process table
		priority = g_proc_table[gp_current_process->m_pid].m_priority;
		//push PCB of current process on blocked_resource_qs;
		push(blocked_resource_qs[priority], gp_current_process);
		//update PCB of current process' state
		gp_current_process->m_state = BLOCKED_ON_RESOURCE;
		k_release_processor(); 
	}

	__disable_irq();
	a = (Block *)MSP;
	// increment MSP
	MSP = MSP->next;
	//assign process 1 mem block (need to make process table and memory table to assign shit too
	a->next = NULL;
	a->pid = gp_current_process->m_pid;
	//atomic(off);
	 __enable_irq();
	return (void *) a;
}

/**
 * @brief: k_release_memory_block()- 
 *				 given a memory block, checks that current process owns the memory block. 
 *				 if so, adds memory block to the free memory heap. 
 * @return: RTX_OK upon success
 *         RTX_ERR upon failure 
 * POST:  updates free memory heap. updates MSP.
 */
int k_release_memory_block(void *p_mem_blk) {
	Block* released = (Block*)p_mem_blk;
	PCB *element;
	int i;
	//atomic(on);

	if (released == NULL) {
		return RTX_ERR;
	} else if (released->pid != gp_current_process->m_pid) { //check if current process own memory block
			return RTX_ERR;
	}
		__disable_irq();

	released -> next = MSP -> next;
	released -> pid = NULL;
	MSP = released;
	
	 //unblocking resources
	for (i = 0; i < NUM_PRIORITIES; i++) {
		element = pop(blocked_resource_qs[i]);
		if (element != NULL) {
			element->m_state = RDY;
			pushToReadyQ(i,element);
			break;
		}
	}
	
	//atomic(off);
	 __enable_irq();
	return RTX_OK;
}
/**
 * @brief: k_get_total_num_blocks()- 
 * @return: total_mem_blocks, total number of memory blocks
 */
int k_get_total_num_blocks(void){
	return total_mem_blocks;
}
