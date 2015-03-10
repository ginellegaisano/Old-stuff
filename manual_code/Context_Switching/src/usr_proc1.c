/**
 * @file:   usr_proc.c
 * @brief:  6 user tests and 2 random procs
 * @author: GG (yes re), Reesey, RayMak, and LJ
 * @date:   2015/02/01
 * NOTE: Each process is in an infinite loop. Processes never terminate.
 */

#include "k_rtx.h"
#include "rtx.h"
#include "k_memory.h"
#include "usr_proc.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* DEBUG_0 */
extern int FAILED;

extern int a_count;
extern int b_count;
extern int c_count;

extern int test1_count;
extern int test2_count;
extern int test3_count;
extern int test4_count;
extern int test5_count;



/* initialization table item */
void * test3_mem = NULL;

/**
 * @brief: Empty Procedure
 */
void A(void) //pid = 7
{
		/** TEST1 code */
	a_count ++;
	set_process_priority(7,MEDIUM);
	
	while (1) {
			release_processor();
	}
}

/**
 * @brief: Empty Procedure
 */
void B(void) //pid = 8
{
//	int i;
	//int *sender = k_request_memory_block();
//	msgbuf *message;
	
	
	/* TEST3 code */
	b_count = release_memory_block(test3_mem);
	release_processor();

	while(1) {
		release_processor();
	}
}

/**
 * @brief: Empty Procedure
 */
void C(void) //pid == 9
{
	
	/*TEST4 code */
	
	//fills up the memory block array. Also requests ALL memory.
	int number_mem_blocks = getTotalFreeMemory(); //101
	void * mem_blocks[500];
	int i = 0;
	
	for (i = 0; i < number_mem_blocks; i ++){
		mem_blocks[i] = request_memory_block();
	}
	i = 0;
	//release all memory
	while(mem_blocks[i] != NULL) {
		release_memory_block(mem_blocks[i]);
		i++;
	}
		
	release_processor();
	
	//release process
	while(1) {
		release_processor();
	}
}


/**
 * @brief: a process that tests getting and setting priority as well as setting priority to an illegal value.
 */
void test1(void){
	
	int failed = 0;
	int initial = 0;
	int final = 0; 
	Element* iterator;
	
	set_process_priority(2, MEDIUM);


	//Check that process A is put on the correct ready queue
	initial = (int)get_process_priority(7);
	iterator = getReadyQ(initial)->first;

	while (iterator != NULL && ((PCB*)(iterator->data))->m_pid != 7) {
		iterator = iterator->next;
	}
	if (iterator == NULL) {
		failed = failed + 1;
	}
	
	
	//Check that setting priority to medium preempts and updates queue correctly
	set_process_priority(7,HIGH);
	final = get_process_priority(7);
	
	//check that A popped from lowest ready queue
	iterator = getReadyQ(initial)->first;
	while (iterator != NULL && ((PCB*)(iterator->data))->m_pid != 7) {
		iterator = iterator->next;
	}
	if (iterator != NULL) {
		failed = failed + 1;
	}
	
	//check that A premepted
	if(a_count != 1) {
		failed = failed + 1;
	}
	
	//Get and set priorities correctly changed priorities
	if(initial != LOWEST || final != MEDIUM) {
		failed = failed + 1;
	}
	
	//Set priority to invalid value
	final = set_process_priority(7,10);
	if (final == RTX_OK) {
		failed = failed + 1;
	}
	

	endTest(failed + test1_count, 1);
	set_process_priority(2, LOWEST);
	
	while(1) {
		release_processor();
	}
	
}

/**
 * @brief: a process that tests the allocation and deallocation of a memory block
	*  			 and when trying to free a memory block twice, returns an error
 */
void test2(void){
	int failed = 0;
	int ret_code;
	void * requested;
	int initial;
	int final;

	set_process_priority(3, MEDIUM);
	initial = getMSP();

	requested = request_memory_block();

	final = getMSP();
	
	if(initial - final != 128) {
		failed = failed + 1;
	}
	release_memory_block(requested);
	if(initial != getMSP()) {
		failed = failed + 1;
	}

	requested = request_memory_block();
	ret_code = release_memory_block(requested);
	if (ret_code != RTX_OK) {
		failed++;
	}

	ret_code = release_memory_block(requested);

	if (ret_code != RTX_ERR) {
		failed++;
	}

	endTest(failed + test2_count, 2);
	set_process_priority(3, LOWEST);
	
	while(1) {
		release_processor();
	}
	
}
/**
 * @brief: a process that tests memory ownership 
 */
void test3(void){
	int failed = 0; 
	void * requested;
	requested = request_memory_block();
	test3_mem = requested;
	
	set_process_priority(4, MEDIUM);

	//Test that memory was allocated
	if (requested == NULL) {
		failed = failed + 1;
	}

	//shove B to medium queue, B will attempt to free test3_mem
	set_process_priority(8,MEDIUM);
	release_processor();

	if(b_count != -1) {
		failed ++;
	}
	
	release_memory_block(requested);

	endTest(failed + test3_count, 3);
	set_process_priority(4, LOWEST);
	
		while(1) {
		release_processor();
	}
	
}
/**
 * @brief: a process that tests the out of memory exception + tests the blocked queue size
 */
void test4(void){
	int failed = 0;
	int initialFree = getTotalFreeMemory();
	int finalFree = getTotalFreeMemory();

	void *test4_mem;
	
	set_process_priority(5,MEDIUM);
	
	//Take one memory block
	test4_mem = request_memory_block();
	finalFree = getTotalFreeMemory();

	//Have process C try consume all memory
	set_process_priority(9,MEDIUM);
	release_processor();

	// check that C was blocked
	if(getBlockedResourceQ(MEDIUM) == NULL &&
		getBlockedResourceQ(MEDIUM)->first == NULL ){
		failed ++;
	}
	
	//Check that releasing memory unblocks C
	release_memory_block(test4_mem);
	release_processor();
	finalFree = getTotalFreeMemory();

	if(getBlockedResourceQ(MEDIUM) != NULL && getBlockedResourceQ(MEDIUM)->first != NULL ){	
		failed ++;
	}

	finalFree = getTotalFreeMemory();
	if(finalFree != initialFree ){
		failed ++;
	}
	
	 
	endTest(failed + test4_count, 4);
	set_process_priority(5, LOWEST);

	while(1) {
		release_processor();
	}
	
}
/**
 * @brief: a process that tests message passing
 */
void test5(void){	
	int failed = 0;
	int initial_total = 0;
	int final_total;
	Block* initial_elementBlock;
	Block* final_elementBlock;
	
	endTest(failed + test5_count, 5);
	set_process_priority(6, LOWEST);

	while(1) {
		release_processor();
	}
	
}
