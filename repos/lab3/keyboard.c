#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/sysutil.h>
#include "keyboard.h"
#include "i8042.h"
#include "bitmanip.h"

static unsigned char leds_set = 0x00;

int kbc_read_status_buf(unsigned char *status)
{
	return sys_inb(KBC_STATUS_PORT, (unsigned long *) status);
}

int kbc_write_in_buf(unsigned char data)
{
	unsigned char status;
	while(1)
	{
		if (kbc_read_status_buf(&status) != OK)
			return 1;

		if (!(status & IN_BUF_FULL))
			return sys_outb(KBC_IN_BUF, data);
		tickdelay(micros_to_ticks(DELAY_US));
	}
}

int kbc_read_out_buf(unsigned char *data)
{
	unsigned char status;
	while(1)
	{
		if (kbc_read_status_buf(&status) != OK)
			return 1;

		if (status & OUT_BUF_FULL)
		{
			if ((status & (PARITY_EVEN | TRANS_TMOUT)) != 0)
				return 1;
			sys_inb(KBC_OUT_BUF, (unsigned long *) data);
			return 0;
		}
		tickdelay(micros_to_ticks(DELAY_US));
	}
}

int kbc_send_command(unsigned char command)
{
	unsigned char status;
	while(1)
	{
		if (kbc_read_status_buf(&status) != OK)
			return 1;

		if (!(status & IN_BUF_FULL))
			return sys_outb(KBC_CMD_PORT, command);
		tickdelay(micros_to_ticks(DELAY_US));
	}
}

int kbc_send_command_with_arg(unsigned char command, unsigned char arg)
{
	if (kbc_send_command(command) != OK)
		return 1;

	return kbc_write_in_buf(arg);
}

int kdb_send_command(unsigned char command)
{
	unsigned char answer;
	while (1)
	{
		if (kbc_write_in_buf(command) != OK)
			return 1;
		while (1)
		{
			if (kbc_read_out_buf(&answer) != OK)
				return 1;
			if (answer == KBD_ACK || answer == KBD_ERROR || answer == KBD_NOT_ACK)
				break;
		}
		if (answer == KBD_ACK)
			break;
	}

	return OK;
}

int kbd_send_command_with_arg(volatile unsigned char command, volatile unsigned char arg)
{
	static unsigned char answer;
	while (1)
	{
		if (kbc_write_in_buf(command) != OK)
			return 1;
		while (1)
		{
			if (kbc_read_out_buf(&answer) != OK)
				return 1;
			if (answer == KBD_ACK || answer == KBD_ERROR || answer == KBD_NOT_ACK)
				break;
		}

		if (answer == KBD_NOT_ACK || answer == KBD_ERROR)
			continue;
		while (1)
		{
			if (kbc_write_in_buf(arg) != OK)
				return 1;
			while (1)
			{
				if (kbc_read_out_buf(&answer) != OK)
					return 1;
				if (answer == KBD_ACK || answer == KBD_NOT_ACK || answer == KBD_ERROR)
					break;
			}
			if (answer == KBD_ACK || answer == KBD_ERROR)
				break;
		}

		if (answer == KBD_ACK)
			break;
	}

	return OK;
}

int kbd_toggle_led(unsigned short led)
{
	if (!(0 <= led && led <= 2))
		return 1;
	leds_set = TOGGLE_BIT(leds_set, led);

	return kbd_send_command_with_arg(KBD_LED_CMD, leds_set);
}

int kbd_clear_leds()
{
	leds_set = 0x00;
	return kbd_send_command_with_arg(KBD_LED_CMD, leds_set);
}
