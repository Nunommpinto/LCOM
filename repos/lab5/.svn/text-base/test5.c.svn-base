#include <stdio.h>
#include <minix/syslib.h>
#include <minix/drivers.h>
#include "test5.h"
#include "video_gr.h"
#include "read_xpm.h"
#include "interrupt.h"
#include "kbc.h"
#include "i8254.h"
#include "i8042.h"
#include "pixmap.h"
#include "sprite.h"
#include "vbe.h"

static unsigned long timer_ticks;
static unsigned char key;

static void timer0_int_handler()

{
	++timer_ticks;
}

static void kbd_int_handler()
{
	unsigned char answer;
	if (kbc_read_out_buf(&answer) == OK)
		key = answer;
}

void *test_init(unsigned short mode, unsigned short delay)
{

	int ipc_status;
	message msg;
	int r;

	void *video_mem = vg_init(mode);
	if (video_mem == NULL)
		return NULL;

	Interrupt timer0_irq;
	timer0_irq.line = TIMER0_IRQ_LINE;
	timer0_irq.policy = IRQ_REENABLE;
	timer_ticks = 0;

	if (irq_subscribe(&timer0_irq) != OK)
		return NULL;

	while (timer_ticks < delay * 60)
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
				}
				break;
			default:
				break;
			}
		}
	}

	if (vg_exit() != OK)
		return NULL;

	if (irq_unsubscribe(&timer0_irq) != OK)
		return NULL;

	return video_mem;
}

int test_square(unsigned short x, unsigned short y, unsigned short size,
		unsigned long color)
{
	int ipc_status;
	message msg;
	int r;

	static Interrupt kbd_irq;
	kbd_irq.line = KBD_IRQ_LINE;
	kbd_irq.policy = IRQ_REENABLE | IRQ_EXCLUSIVE;
	if (irq_subscribe(&kbd_irq) != OK)
		return 1;

	if (vg_init(0x105) == NULL)
		return 1;

	vg_fill(x, y, size, size, color);
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
					kbd_int_handler();
				}
				break;
			default:
				break;
			}
		}
	}

	if (vg_exit() != OK)
		return 1;

	if (irq_unsubscribe(&kbd_irq) != OK)
		return 1;

	return 0;
}

int test_line(unsigned short xi, unsigned short yi, unsigned short xf,
		unsigned short yf, unsigned long color)
{

	int ipc_status;
	message msg;
	int r;

	static Interrupt kbd_irq;
	kbd_irq.line = KBD_IRQ_LINE;
	kbd_irq.policy = IRQ_REENABLE | IRQ_EXCLUSIVE;
	if (irq_subscribe(&kbd_irq) != OK)
		return 1;

	if (vg_init(0x105) == NULL)
		return 1;

	vg_line(xi, yi, xf, yf, color);

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
					kbd_int_handler();
				}
				break;
			default:
				break;
			}
		}
	}

	if (vg_exit() != OK)
		return 1;

	if (irq_unsubscribe(&kbd_irq) != OK)
		return 1;

	return 0;
}

int test_xpm(unsigned short xi, unsigned short yi, char *xpm[])
{

	int ipc_status;
	message msg;
	int r;

	static Interrupt kbd_irq;
	kbd_irq.line = KBD_IRQ_LINE;
	kbd_irq.policy = IRQ_REENABLE | IRQ_EXCLUSIVE;
	if (irq_subscribe(&kbd_irq) != OK)
		return 1;

	if (vg_init(0x105) == NULL)
		return 1;

	print_xpm(xi, yi, xpm);

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
					kbd_int_handler();

				}
				break;
			default:
				break;
			}
		}
	}

	if (vg_exit() != OK)
		return 1;

	if (irq_unsubscribe(&kbd_irq) != OK)
		return 1;

	return 0;

}

int test_move(unsigned short xi, unsigned short yi, char *xpm[],
		unsigned short hor, short delta, unsigned short time)
{
	int ipc_status;
	message msg;
	int r;

	Sprite* sp = create_sprite(xpm, xi, yi, hor, delta, time);

	Interrupt timer0_irq;
	timer0_irq.line = TIMER0_IRQ_LINE;
	timer0_irq.policy = IRQ_REENABLE;
	irq_subscribe(&timer0_irq);
	timer_ticks = 0;

	static Interrupt kbd_irq;
	kbd_irq.line = KBD_IRQ_LINE;
	kbd_irq.policy = IRQ_REENABLE | IRQ_EXCLUSIVE;
	if (irq_subscribe(&kbd_irq) != OK)
		return 1;

	if (vg_init(0x105) == NULL)
		return 1;

	vg_animate_sprite(sp);

	while ((key != (KBD_KEY_ESC | KBD_KEY_BREAK)))
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
					kbd_int_handler();
				}
				else if (msg.NOTIFY_ARG & timer0_irq.set)
				{
					if (timer_ticks < time * 60)
					{
						vg_animate_sprite(sp);
						timer0_int_handler();
					}
				}
				break;
			default:
				break;
			}
		}
	}

	destroy_sprite(sp);

	if (vg_exit() != OK)
		return 1;

	if (irq_unsubscribe(&kbd_irq) != OK)
		return 1;

	if (irq_unsubscribe(&timer0_irq) != OK)
		return 1;

	return 0;

}


int test_controller()
{
	vbe_info_block_t vib;
	if (vbe_get_info_block(&vib) != OK)
		return 1;

	vbe_info_display(&vib);

	return OK;
}
