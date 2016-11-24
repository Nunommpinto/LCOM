#include <stdio.h>
#include <minix/syslib.h>
#include <minix/drivers.h>
#include "test3.h"
#include "i8042.h"
#include "i8254.h"
#include "keyboard.h"
#include "interrupt.h"

extern unsigned char key;
static unsigned long timer_ticks;

static void kbd_int_handler()
{
	unsigned char answer;
	if (kbc_read_out_buf(&answer) == OK)
		key = answer;
}

static void timer0_int_handler()
{
	++timer_ticks;
}

int kbd_test_scan(unsigned short ass)
{
	int ipc_status;
	message msg;
	int r;
	Interrupt kbd_irq;
	kbd_irq.line = KBD_IRQ_LINE;
	kbd_irq.policy = IRQ_REENABLE | IRQ_EXCLUSIVE;
	irq_subscribe(&kbd_irq);
	key = 0x00;

	while (key != (KBD_KEY_ESC | KBD_KEY_BREAK))
	{
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != OK)
		{
			printf("driver_receive failed with status: %d\n", r);
			continue;
		}

		if (is_ipc_notify(ipc_status))
		{
			switch (_ENDPOINT_P(msg.m_source))
			{
			case HARDWARE:
				if (msg.NOTIFY_ARG & kbd_irq.set)
				{
					if (key == KBD_KEY_2BYTE)
					{
						if (!ass)
							kbd_int_handler();
						else
							kbd_int_handler_asm();
						if (key & (KBD_KEY_BREAK))
							printf("Breakcode: ");
						else
							printf("Makecode: ");
						printf("0x%02x%02x\n", KBD_KEY_2BYTE, key);
					}
					else
					{
						if (!ass)
							kbd_int_handler();
						else
							kbd_int_handler_asm();
						if (key != KBD_KEY_2BYTE)
						{
							if (key & (KBD_KEY_BREAK))
								printf("Breakcode: ");
							else
								printf("Makecode: ");
							printf("0x%02x\n", key);
						}
					}

				}
				break;
			default:
				break;
			}
		}
	}

	return irq_unsubscribe(&kbd_irq);
}

int kbd_test_leds(volatile unsigned short n, unsigned short *leds)
{
	if (n == 0)
		return 1;

	int ipc_status;
	message msg;
	int r;

	Interrupt timer0_irq;
	timer0_irq.line = TIMER0_IRQ_LINE;
	timer0_irq.policy = IRQ_REENABLE;
	irq_subscribe(&timer0_irq);
	timer_ticks = 0;

	Interrupt kbd_irq;
	kbd_irq.line = KBD_IRQ_LINE;
	kbd_irq.policy = IRQ_REENABLE | IRQ_EXCLUSIVE;
	irq_subscribe(&kbd_irq);

	if (kbd_clear_leds() != OK)
		return 1;

	int led_count = 0;
	if (kbd_toggle_led(leds[led_count]) != OK)
		return 1;

	while (led_count < n)
	{
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != OK)
		{
			printf("driver_receive failed with status: %d\n", r);
			continue;
		}

		if (is_ipc_notify(ipc_status))
		{
			switch (_ENDPOINT_P(msg.m_source))
			{
			case HARDWARE:
				if (msg.NOTIFY_ARG & timer0_irq.set)
				{
					timer0_int_handler();
					if (timer_ticks == 60)
					{
						++led_count;
						if (led_count == n)
							break;
						if (kbd_toggle_led(leds[led_count]) != OK)
							return 1;
						timer_ticks = 0;
					}
				}
				break;
			default:
				break;
			}
		}
	}

	if (kbd_clear_leds() != OK)
		return 1;

	if (irq_unsubscribe(&kbd_irq) != OK)
			return 1;

	if (irq_unsubscribe(&timer0_irq) != OK)
		return 1;

	return OK;
}

int kbd_test_timed_scan(unsigned short n)
{
	int ipc_status;
	message msg;
	int r;

	Interrupt kbd_irq;
	kbd_irq.line = KBD_IRQ_LINE;
	kbd_irq.policy = IRQ_REENABLE | IRQ_EXCLUSIVE;
	irq_subscribe(&kbd_irq);
	key = 0x00;

	Interrupt timer0_irq;
	timer0_irq.line = TIMER0_IRQ_LINE;
	timer0_irq.policy = IRQ_REENABLE;
	irq_subscribe(&timer0_irq);
	timer_ticks = 0;

	while ((key != (KBD_KEY_ESC | KBD_KEY_BREAK)) && (timer_ticks < n * 60))
	{
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != OK)
		{
			printf("driver_receive failed with status: %d\n", r);
			continue;
		}

		if (is_ipc_notify(ipc_status))
		{
			switch (_ENDPOINT_P(msg.m_source))
			{
			case HARDWARE:
				if (msg.NOTIFY_ARG & kbd_irq.set)
				{
					timer_ticks = 0;
					if (key == KBD_KEY_2BYTE)
					{
						kbd_int_handler();
						if (key & (KBD_KEY_BREAK))
							printf("Breakcode: ");
						else
							printf("Makecode: ");
						printf("0x%02x%02x\n", KBD_KEY_2BYTE, key);
					}
					else
					{
						kbd_int_handler();
						if (key != KBD_KEY_2BYTE)
						{
							if (key & (KBD_KEY_BREAK))
								printf("Breakcode: ");
							else
								printf("Makecode: ");
							printf("0x%02x\n", key);
						}
					}
				}
				else if (msg.NOTIFY_ARG & timer0_irq.set)
				{
					timer0_int_handler();
				}
				break;
			default:
				break;
			}
		}
	}

	if (irq_unsubscribe(&kbd_irq) != OK)
		return 1;
	if (irq_unsubscribe(&timer0_irq) != OK)
		return 1;

	return OK;
}
