#include <stdio.h>
#include <minix/syslib.h>
#include <minix/drivers.h>
#include "test4.h"
#include "i8042.h"
#include "i8254.h"
#include "interrupt.h"
#include "mouse.h"

unsigned char packet[3];
static unsigned int counter;
static unsigned long timer_ticks;

static void mouse_int_handler() {
	unsigned char data;
	if (kbc_read_out_buf(&data) == OK) {
		switch (counter) {
		case 0:
			if (data & MOUSE_CHECK) {
				packet[0] = data;
				++counter;
			}
			break;
		case 1:
			packet[1] = data;
			++counter;
			break;
		case 2:
			packet[2] = data;
			counter = 0;
			break;
		}
	}
}

static void timer0_int_handler() {
	timer_ticks++;
}

static void mouse_config_int_handler() {
	unsigned char data;
	if (kbc_read_out_buf(&data) == OK) {
		switch (counter) {
		case 0:
			packet[0] = data;
			++counter;
			break;
		case 1:
			packet[1] = data;
			++counter;
			break;
		case 2:
			packet[2] = data;
			counter = 0;
			break;
		}
	}
}

int test_packet(volatile unsigned short cnt) {
	int ipc_status;
	message msg;
	int r;

	Interrupt mouse_irq;
	mouse_irq.line = MOUSE_IRQ_LINE;
	mouse_irq.policy = IRQ_REENABLE | IRQ_EXCLUSIVE;
	irq_subscribe(&mouse_irq);

	if (kbc_send_command(ENABLE_MOUSE) != OK
			|| mouse_send_command(SET_STREAM) != OK
			|| mouse_send_command(ENABLE_SEND_PACKET) != OK)
		return 1;

	while (cnt > 0) {
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != OK) {
			printf("driver_receive failed with status: %d\n", r);
			continue;
		}

		if (is_ipc_notify(ipc_status)) {
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE:
				if (msg.NOTIFY_ARG & mouse_irq.set) {
					if (counter < 2)
						mouse_int_handler();
					else {
						mouse_int_handler();
						mouse_print_packet(packet);
						--cnt;
					}
				}
				break;
			default:
				break;
			}
		}
	}

	if (mouse_send_command(DISABLE_STREAM) != OK
			|| irq_unsubscribe(&mouse_irq) != OK)
		return 1;

	return OK;
}

int test_async(volatile unsigned short idle_time) {
	int ipc_status;
	message msg;
	int r;

	Interrupt mouse_irq;
	mouse_irq.line = MOUSE_IRQ_LINE;
	mouse_irq.policy = IRQ_REENABLE | IRQ_EXCLUSIVE;
	irq_subscribe(&mouse_irq);
	counter = 0;

	Interrupt timer0_irq;
	timer0_irq.line = TIMER0_IRQ_LINE;
	timer0_irq.policy = IRQ_REENABLE;
	irq_subscribe(&timer0_irq);
	timer_ticks = 0;

	if (kbc_send_command(ENABLE_MOUSE) != OK
			|| mouse_send_command(SET_STREAM) != OK
			|| mouse_send_command(ENABLE_SEND_PACKET) != OK)
		return 1;

	while (timer_ticks < idle_time * 60) {
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != OK) {
			printf("driver_receive failed with status: %d\n", r);
			continue;
		}

		if (is_ipc_notify(ipc_status)) {
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE:
				if (msg.NOTIFY_ARG & mouse_irq.set) {
					timer_ticks = 0;

					if (counter < 2)
						mouse_int_handler();
					else {
						mouse_int_handler();
						mouse_print_packet(packet);
					}

				} else if (msg.NOTIFY_ARG & timer0_irq.set) {
					timer0_int_handler();
				}
				break;
			default:
				break;
			}
		}
	}

	if (mouse_send_command(DISABLE_STREAM) != OK
			|| irq_unsubscribe(&mouse_irq) != OK
			|| irq_unsubscribe(&timer0_irq) != OK)
		return 1;

	return OK;
}

int test_config(void) {
	int ipc_status;
	message msg;
	int r;

	Interrupt mouse_irq;
	mouse_irq.line = MOUSE_IRQ_LINE;
	mouse_irq.policy = IRQ_REENABLE | IRQ_EXCLUSIVE;
	irq_subscribe(&mouse_irq);
	counter = 0;

	if (mouse_send_command(DISABLE_STREAM) != OK)
		return 1;

	if (mouse_send_command(STATUS_REQUEST) != OK)
		return 1;

	volatile int config_read = 0;
	while (config_read != 1) {
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != OK) {
			printf("driver_receive failed with status: %d\n", r);
			continue;
		}

		if (is_ipc_notify(ipc_status)) {
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE:
				if (msg.NOTIFY_ARG & mouse_irq.set) {
					if (counter < 2)
						mouse_config_int_handler();
					else {
						mouse_config_int_handler();
						mouse_print_config(packet);
						config_read = 1;
					}
				}
				break;
			default:
				break;
			}
		}
	}
	return irq_unsubscribe(&mouse_irq);
}

int test_gesture(volatile short length, unsigned short tolerance) {
	int ipc_status;
	message msg;
	int r;

	Interrupt mouse_irq;
	mouse_irq.line = MOUSE_IRQ_LINE;
	mouse_irq.policy = IRQ_REENABLE | IRQ_EXCLUSIVE;
	irq_subscribe(&mouse_irq);

	if (kbc_send_command(ENABLE_MOUSE) != OK
			|| mouse_send_command(SET_STREAM) != OK
			|| mouse_send_command(ENABLE_SEND_PACKET) != OK)
		return 1;

	state_machine status;
	status.state = INIT;
	status.x = 0;
	status.y = 0;
	event_t event;

	printf("Draw an horizontal line from %s\n", length > 0 ? "left to right" : "right to left");
	while (status.state != COMP) {
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != OK) {
			printf("driver_receive failed with status: %d\n", r);
			continue;
		}

		if (is_ipc_notify(ipc_status)) {
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE:
				if (msg.NOTIFY_ARG & mouse_irq.set) {
					if (counter < 2)
						mouse_int_handler();
					else {
						mouse_int_handler();
						mouse_get_event(&event, packet);
						check_hor_line(&status, &event, length, tolerance);
					}
				}
				break;
			default:
				break;
			}
		}
	}
	printf("Horizontal line drawn\n");

	if (mouse_send_command(DISABLE_STREAM) != OK
			|| irq_unsubscribe(&mouse_irq) != OK)
		return 1;

	return OK;
}
