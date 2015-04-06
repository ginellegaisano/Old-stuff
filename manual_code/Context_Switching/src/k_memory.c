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
#include "msg.h"

/* ----- Global Variables ----- */
U32 *gp_stack; 
int total_mem_blocks;
int free_blocks;
/* The last allocated stack low address. 8 bytes aligned */
/* The first stack starts at the RAM high address */
/* stack grows down. Fully decremental stack */



Block* MSP;
Block* ElementBlock;


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
 * @brief getMSP()
 * @return returns address of MSP
 */
int getMSP (void) {
	return (int)MSP;
}

int getTotalFreeMemory(void){
	/*Block *iterator = MSP;
	int counter = 0;
	while(iterator != NULL){
		counter ++;
		iterator = iterator->next;
	}
	return counter;*/
	return free_blocks;
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
	Queue* q4;
	Element* currElement;
	
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
	//setting blocked Resources Q
	for (i = 0; i < NUM_PRIORITIES; i++) {
		Queue* q1 = (Queue *)p_end;
		p_end += sizeof(Queue);
		q1->first = NULL;
		q1->last = NULL;
		setBlockedResourceQ(i, q1);
	}
	

	
	//setting reading Q
	for ( i = 0; i < NUM_PRIORITIES; i++) {
		Queue* q2 = (Queue *)p_end;
		p_end += sizeof(Queue);
		q2->first = NULL;
		q2->last = NULL;
		
		setReadyQ(i, q2);
	}
	
	//setting the blocked_resources_qs
	for (i = 0; i < NUM_PRIORITIES; i++) {
		Queue* q3 = (Queue *)p_end;
		p_end += sizeof(Queue);
		q3->first = NULL;
		q3->last = NULL;
		setBlockedReceiveQ(i, q3);
	}
	
	for ( i = 0; i < NUM_PROCS; i++) {
		Queue* q3 = (Queue *)p_end;
		p_end += sizeof(Queue);
		q3->first = NULL;
		q3->last = NULL;
		
		gp_pcbs[i]->mailbox = q3;
	}
	
	q4 = (Queue *)p_end;
	p_end += sizeof(Queue);
	q4->first = NULL;
	q4->last = NULL;
	
	setTimedQ(q4);

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
	
	ElementBlock = (Block*)((int)k_request_memory_block()-4);
	total_mem_blocks --;
	free_blocks = total_mem_blocks;
	currElement = (Element*)((int) ElementBlock + sizeof(int*));
	for (i = (int)currElement; i < (int)ElementBlock + BLOCK_SIZE - sizeof(Element); i+= 3*sizeof(int*)) {
		((Element*)(i))->next = NULL;
	  ((Element*)(i))->data = NULL;
		((Element*)(i))->block = ElementBlock;
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
 * @brief:
 * @return: 
 * POST:  
 */
void *k_request_element(void) {
		Block* currBlock;
		int i;
  	Element* currElement;

		__disable_irq();
		currBlock = ElementBlock;
	  currElement = (Element*)((int) currBlock + sizeof(int) + sizeof(Block*));;

		while (currElement->data != NULL) {
			if ((int)currElement + 3*sizeof(int*) > (int)currBlock + BLOCK_SIZE - 3*sizeof(int*)) {
				if (currBlock->next == NULL) {
					currBlock->next = (Block*)((int)k_request_memory_block()-4);
					total_mem_blocks --;
					for (i = (int)currBlock->next + 4 + sizeof(Block*); i <= (int)currBlock->next + BLOCK_SIZE - 3*sizeof(int*); i+= 3*sizeof(int*)) {
						((Element*)(i))->next = NULL;
					  ((Element*)(i))->data = NULL;
						((Element*)(i))->block = currBlock->next;
					}

					currBlock->next->next = NULL;
				}
				currBlock = currBlock->next;
				currElement = (Element*)((int) currBlock + 4 + sizeof(Block*));
			} else {
				currElement = (Element*)((int)currElement + 3*sizeof(int*));
			}
		}
		__enable_irq();
		return currElement;
}
int k_release_element_block(void * released){
	Element *element = (Element *)released;
	Block * elementBlock = element->block;
	Block* iterator = ElementBlock;
	int i;
	bool empty = true;
	element->next = NULL;
	element->data = NULL;
	__disable_irq();
	for (i = (int)elementBlock  + 4 + sizeof(Block*); i <= (int)elementBlock + BLOCK_SIZE - 3*sizeof(int*); i += 3*sizeof(int*)){
		if (((Element*)(i))->data != NULL) empty = false;
	}
	if(empty){
		while (iterator->next != NULL && iterator->next != elementBlock) {
			iterator = iterator->next;
		}
		if (iterator->next == elementBlock) {
			iterator->next = elementBlock->next;
			elementBlock -> next = MSP;
			elementBlock -> pid = NULL;
			MSP = elementBlock;
			total_mem_blocks++;
			free_blocks++;
		}
	}
	__enable_irq(); //released the memory block.
	//printf("+1 memory block released\n\r");

	return RTX_OK; //something to say that i didnt release the memory block because it had something in it.
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
	Element* element;

	while (MSP == NULL || (free_blocks == 2 && gp_current_process->m_pid < 10)) {
		//get the priority of the current process by looking up pid in process table
		priority = g_proc_table[gp_current_process->m_pid].m_priority;
		//push PCB of current process on blocked_resource_qs; << here we are pushing a PCB. <<
		//element = k_request_element();
		//element->data = gp_current_process;
		push(getBlockedResourceQ(priority), gp_current_element);
		//update PCB of current process' state
		gp_current_process->m_state = BLOCKED_ON_RESOURCE;
		__enable_irq();
		k_release_processor(); 
	}

	__disable_irq();
	a = (Block *)MSP;
	// increment MSP

	MSP = MSP->next;
	//assign process 1 mem block (need to make process table and memory table to assign shit too
	a->next = NULL;
	a->pid = gp_current_process->m_pid;
	free_blocks--;
	//atomic(off);
	 __enable_irq();
	//printf("-1 memory block requested\n\r");

	return (void *) ((int)a+4);
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
	Block* released = (Block*)((int)p_mem_blk-4);
	Element *element;
	int i;
	msgbuf *msg = (msgbuf *) p_mem_blk;
	PCB * pcb;

	if (released == NULL) {
		return RTX_ERR;
	}
	if (released->pid != gp_current_process->m_pid) { //check if current process own memory block
			return RTX_ERR;
	}
		__disable_irq();


	released -> next = MSP;
	released -> pid = NULL;
	MSP = released;
	free_blocks++;
	 __enable_irq();

	 //unblocking resources
	for (i = 0; i < NUM_PRIORITIES; i++) {
		element = pop(getBlockedResourceQ(i));
		if (element != NULL) {
			pcb = ((PCB*)(element->data));
			pcb->m_state = RDY;
			pushToReadyQ(i,element);
			if(((PCB*)(element->data))->m_priority < gp_current_process->m_priority) {
				k_release_processor();
			}
		}
	}
	
	return RTX_OK;
}
/**
 * @brief: k_get_total_num_blocks()- 
 * @return: total_mem_blocks, total number of memory blocks
 */
int k_get_total_num_blocks(void){
	return total_mem_blocks;
}
