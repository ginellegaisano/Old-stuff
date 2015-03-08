/**
 * @file:   msg.h
 * @brief:  Message Send and Receive header
 * @author: GG (yes re), Reesey, RayMak, and LJ
 * @date:   2015/03/10
 */
 
#ifndef MSG_H_
#define MSG_H

#include "k_memory.h"

/* ----- Definitions ----- */
#define DEFAULT 0
#define KCD_REG 1

#define KCD_INTERRUPT_ID 15


typedef struct msgbuf msgbuf;
struct msgbuf {
	int mtype;
	char mtext[(BLOCK_SIZE - sizeof(int))/sizeof(char)];
};

typedef struct Envelope Envelope;
struct Envelope{
	int sender_id;
	int destination_id;
	int time;
	int delay;
	msgbuf *message;
};


void setMessageText(msgbuf*, char text[], int);
int checkMessageText(msgbuf*, char text[]);

int push_mailbox(Envelope *);
Envelope *pop_mailbox(int);
msgbuf *allocate_message(int, char text[], int length);
int deallocate_message(msgbuf *);
msgbuf *k_allocate_message(int, char text[], int length);
int k_deallocate_message(msgbuf *);

void *k_receive_message(int *);
int k_send_message(int, void *);
int k_delayed_send(int, void *, int);
int empty_mailbox(void);


#endif
