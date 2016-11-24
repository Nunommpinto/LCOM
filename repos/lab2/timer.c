#include <minix/syslib.h>
#include <minix/drivers.h>
#include "timer.h"
#include "i8254.h"

#define IRQ_SET 3

static unsigned int hook_id;
static unsigned int timer_counter;

/**
 * @brief Converts from binary to packed binary coded decimal
 *
 * @param binary	Number to be converted
 * @return Returns number in packed binary coded decimal
 */
unsigned short binary_to_packed_bcd(unsigned short binary)
{
	unsigned short bcd = 0;

	if (binary > 9999)
		binary = 9999;

	int i;
	for (i = 1000; i > 0; i /= 10)
	{
		bcd <<= 4;
		bcd |= binary / i % 10;
	}

	return bcd;
}

int timer_set_square(unsigned long timer, unsigned long freq)
{
	if (freq == 0)
		return 1;

	unsigned short count = TIMER_FREQ / freq;
	unsigned char conf;
	unsigned char timer_sel;
	unsigned long timer_port;

	switch (timer)
	{
	case 0:
		timer_sel = TIMER_SEL0;
		timer_port = TIMER_0;
		break;
	case 1:
		timer_sel = TIMER_SEL1;
		timer_port = TIMER_1;
		break;
	case 2:
		timer_sel = TIMER_SEL2;
		timer_port = TIMER_2;
		break;
	default:
		return 1;
	}

	if (timer_get_conf(timer, &conf) != 0)
		return 1;

	count = (conf & TIMER_CONF_COUNTING) ? binary_to_packed_bcd(count) : count;

	if (sys_outb(TIMER_CTRL,
					timer_sel | TIMER_LSB_MSB | TIMER_SQR_WAVE
							| (conf & BIT(0))) != 0
			|| sys_outb(timer_port, (char) count) != 0
			|| sys_outb(timer_port, (char) (count >> 8)) != 0)
		return 1;

	return 0;
}

int timer_subscribe_int(void)
{
	hook_id = IRQ_SET;

	if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id) != OK
			|| sys_irqenable(&hook_id) != OK)
		return -1;

	return BIT(IRQ_SET);
}

int timer_unsubscribe_int()
{
	if (sys_irqdisable(&hook_id) != OK || sys_irqrmpolicy(&hook_id) != OK)
		return 1;
}

void timer_int_handler()
{
	++timer_counter;
}

int timer_get_conf(unsigned long timer, unsigned char *st)
{
	unsigned long timer_port;
	switch (timer)
	{
	case 0:
		timer_port = TIMER_0;
		break;
	case 1:
		timer_port = TIMER_1;
		break;
	case 2:
		timer_port = TIMER_1;
		break;
	default:
		return 1;
	}

	if (sys_outb(TIMER_CTRL,
	TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer)) != 0
			|| sys_inb(timer_port, (unsigned long *) st) != 0)
		return 1;

	return 0;
}

int timer_display_conf(unsigned char conf)
{
	printf("Output pin: %d\n", (conf & TIMER_CONF_OUT) ? 1 : 0);

	printf("Count: %s\n", (conf & TIMER_CONF_COUNT) ? "null" : "available for reading");

	printf("Type of access: ");
	switch (conf & TIMER_CONF_ACCESS)
	{
	case TIMER_LSB:
		printf("LSB");
		break;
	case TIMER_MSB:
		printf("MSB");
		break;
	case TIMER_LSB_MSB:
		printf("LSB followed by MSB");
		break;
	case TIMER_LATCH:
		printf("counter latch command");
		break;
	}
	printf("\n");

	printf("Operating mode: ");
	switch (conf & TIMER_CONF_MODE))
	{
	case TIMER_INT_TERM_CNT:
		printf("interrupt on terminal count");
		break;
	case TIMER_HW_RETRIG_1S:
		printf("hardware retriggerable one-shot");
		break;
	case TIMER_RATE_GEN:
	case (TIMER_RATE_GEN | TIMER_INTEL_COMP):
		printf("rate generator");
		break;
	case TIMER_SQR_WAVE:
	case (TIMER_SQR_WAVE | TIMER_INTEL_COMP):
		printf("square wave generator");
		break;
	case TIMER_SW_TRIG_STROBE:
		printf("software triggered strobe");
		break;
	case TIMER_HW_TRIG_STROBE:
		printf("hardware triggered strobe (retriggerable)");
		break;
	}
	printf("\n");

	printf("Counting mode: %s\n",
			(conf & TIMER_CONF_COUNTING) ? "count in BCD" : "count in binary");

	return 0;
}

int timer_test_square(unsigned long freq)
{
	return timer_set_square(0, freq);
}

int timer_test_int(unsigned long time)
{
	timer_counter = 0;

	int irq_set = timer_subscribe_int();
	int ipc_status;
	message msg;
	int r;

	while (time > 0)
	{
		if ((r =driver_receive(ANY, &msg, &ipc_status)) != OK)
		{
			printf("driver_receive failed with status: %d\n", r);
			continue;
		}

		if (is_ipc_notify(ipc_status))
		{
			switch (_ENDPOINT_P(msg.m_source))
			{
			case HARDWARE:
				if (msg.NOTIFY_ARG & irq_set)
				{
					timer_int_handler();

					if (timer_counter == 60)
					{
						--time;
						timer_counter = 0;

						printf("Interrupt on TIMER0\n");
					}
				}
				break;
			default:
				break;
			}
		}
	}

	return timer_unsubscribe_int();
}

int timer_test_config(unsigned long timer)
{

	char config;

	if (timer_get_conf(timer, &config) != 0)
		return 1;

	printf("Configurations for timer %d\n", timer);

	if (timer_display_conf(config) != 0)
		return 1;

	return 0;
}
