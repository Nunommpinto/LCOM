#include <minix/syslib.h>
#include "bitmanip.h"
#include "interrupt.h"

static unsigned int irq_set_available = 0xFFFFFFFF;

static int get_irq_set(Interrupt *irq)
{
	int i = 0;
	for (i = 0; i < 32; ++i)
		if (irq_set_available & BIT(i))
		{
			irq->set = BIT(i);
			irq->hook_id = i;
			irq_set_available |= BIT(i);
			return OK;
		}

	return -1;
}

int interrupt_subscribe(Interrupt *irq)
{
	if (get_irq_set(irq) != OK)
		return -1;

	if (sys_irqsetpolicy(irq->line, irq->policy, &(irq->hook_id)) != OK
			|| sys_irqenable(&(irq->hook_id)) != OK)
		return -1;

	return 0;
}

int interrupt_unsubscribe(Interrupt *irq)
{
	if (sys_irqdisable(&(irq->hook_id)) != OK || sys_irqrmpolicy(&(irq->hook_id)) != OK)
		return -1;
	return 0;
}
