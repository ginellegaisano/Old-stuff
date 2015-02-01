/**
 * @file:   usr_proc.h
 * @brief:  Two user processes header file
 * @author: Yiqing Huang
 * @date:   2014/01/17
 */
 
#ifndef USR_PROC_H_
#define USR_PROC_H

extern int getMSP (void);
extern Queue **ready_qs;
extern PCB **gp_pcbs;
extern Queue* getReadyQ(int priority);
extern Queue* getBlockedResourceQ(int priority);
extern PCB *scheduler(void);

void set_test_procs(void);
void null_process(void);
void proc1(void);
void proc2(void);
void test1(void);
void test2(void);
void test3(void);
void test4(void);
void test5(void);
void test6(void);

#endif /* USR_PROC_H_ */
