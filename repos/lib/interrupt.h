#ifndef INTERRUPT_H
#define INTERRUPT_H

typedef struct
{
	int hook_id;
	int line;
	int policy;
	int set;
} Interrupt;

int interrupt_subscribe(Interrupt *irq);

int interrupt_unsubscribe(Interrupt *irq);


#endif // INTERRUPT_H
