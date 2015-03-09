/**
 * @file:   usr_proc.h
 * @brief:  Two user processes header file
 * @author: GG (yes re), Reesey, RayMak, and LJ
 * @date:   2015/02/01
 */
 
#ifndef USR_PROC_H_
#define USR_PROC_H

/* ----- Extern ----- */
extern int getMSP (void);
extern Queue **ready_qs;
extern PCB **gp_pcbs;
extern Queue* getReadyQ(int priority);
extern void pushToReadyQ (int priority, Element* p_pcb_old);
extern void printReadyQ (char* tag);
extern void printBlockedQ (char* tag);
extern Queue* getBlockedResourceQ(int priority);
extern PCB *scheduler(void);
extern PCB *scheduler(void);
extern void test1(void);
extern void test2(void);
extern void test3(void);
extern void test4(void);
extern void test5(void);
extern void A(void);
extern void B(void);
extern void C(void);

#define PID_P2 2

/* ----- Functions ----- */
void set_test_procs(void);
void endTest(int failed, int test);
void testHandler(void);


#endif /* USR_PROC_H_ */
