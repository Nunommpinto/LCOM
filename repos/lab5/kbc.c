#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/sysutil.h>
#include "kbc.h"
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
		{
			//printf("Sending 0x%02x to port 0x%02x\n", data, KBC_IN_BUF);
			return sys_outb(KBC_IN_BUF, data);
		}
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
	//		if (status & (PARITY_EVEN | TRANS_TMOUT))
 	//			return 1;
			if (sys_inb(KBC_OUT_BUF, (unsigned long *) data) != OK)
				return 1;
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
		{
			//printf("Sending 0x%02x to port 0x%02x\n", command, KBC_CMD_PORT);
			return sys_outb(KBC_CMD_PORT, command);
		}
		tickdelay(micros_to_ticks(DELAY_US));
	}
}

/* Arguments declared as volatile because arg was being set to 0x00 by unknown reasons*/
int kbc_send_command_with_arg(volatile unsigned char command, volatile unsigned char arg)
{
	//printf("kbc(commnand = 0x%02x, arg = 0x%02x)\n", command, arg);

//	printf("kbc send command: 0x%02x\n", command);

	if (kbc_send_command(command) != OK)
		return 1;

//	printf("kbc send arg: 0x%02x\n", arg);
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

int kbd_send_command_with_arg(unsigned char command, unsigned char arg)
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

int mouse_send_command(unsigned char command)
{
	static unsigned char answer;
	do
	{
		if (kbc_send_command_with_arg(WRITE_BYTE_MOUSE, command) != OK)
			return 1;
		do
		{
			if (kbc_read_out_buf(&answer) != OK)
				return 1;
			//printf("%02x\n", answer);
		} while (answer != KBD_ACK && answer != KBD_NOT_ACK && answer != KBD_ERROR);
	} while (answer != KBD_ACK);

	return OK;
}

int mouse_send_command_with_arg(unsigned char command, unsigned char arg)
{

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
