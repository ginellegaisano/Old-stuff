/**
 * @file:   queue.h
 * @brief:  Queue structure
 * @author: GG (yes re), Reesey, RayMak, and LJ
 * @date:   2015/03/05
 */
 
#ifndef QUEUE_H_
#define QUEUE_H_

#include "k_rtx.h"

/* ----- Functions ------ */
//Element* pop(Queue* self);
Element* removeFromQ(Queue* self, int id);
//int push(Queue* self, Element* element);
void pushToReadyQ (int priority, Element* p_pcb_old);
Element* popFromReadyQ (int priority);
Queue* getReadyQ(int priority);
void setReadyQ(int priority, Queue* q);
Queue* getBlockedResourceQ(int priority);
void setBlockedResourceQ(int priority, Queue* q);
Queue* getBlockedReceiveQ(int priority);
void setBlockedReceiveQ(int priority, Queue* q);
Queue* getTimedQ(void);
void setTimedQ(Queue* q);
void printReadyQ (char* tag);
void printBlockedQ (char* tag);
void printBlockedReceiveQ(char* tag);

#endif
