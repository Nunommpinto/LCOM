#ifndef INTERRUPT_H
#define INTERRUPT_H

typedef struct
{
	int hook_id;
	int line;
	int policy;
	int set;
} Interrupt;

int irq_subscribe(Interrupt *irq);

int irq_unsubscribe(Interrupt *irq);


#endif // INTERRUPT_H
