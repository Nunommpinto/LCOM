#include "mouse.h"
#include "i8042.h"
#include "bitmanip.h"

short mouse_get_xdelta(unsigned char *packet)
{
	short xdelta = 0x00;
	if (packet[0] & MOUSE_XOVF)
		xdelta |= 0xFF;
	else
		xdelta |= packet[1];

	if(packet[0] & MOUSE_XSIGN)
		xdelta |= SET_BYTE(1);

	return xdelta;
}

short mouse_get_ydelta(unsigned char *packet)
{
	short ydelta = 0x00;
	if (packet[0] & MOUSE_YOVF)
		ydelta |= 0xFF;
	else
		ydelta |= packet[2];

	if(packet[0] & MOUSE_YSIGN)
		ydelta |= SET_BYTE(1);

	return ydelta;
}

void mouse_print_packet(unsigned char *packet)
{
	printf("B1=0x%02x B2=0x%02x B3=0x%02x LB=%d MB=%d RB=%d XOV=%d YOV=%d X=%4d Y=%4d\n",
			packet[0], packet[1], packet[2],
			(packet[0] & MOUSE_LB) ? 1 : 0,
			(packet[0] & MOUSE_MB) ? 1 : 0,
			(packet[0] & MOUSE_RB) ? 1 : 0,
			(packet[0] & MOUSE_XOVF) ? 1 : 0,
			(packet[0] & MOUSE_YOVF) ? 1 : 0,
			(mouse_get_xdelta(packet)),
			(mouse_get_ydelta(packet)));
}

void mouse_print_config(unsigned char *config)
{
	printf("Right Button: %s\n", (config[0] & MOUSE_RB_PRESS) ? "Pressed" : "Released");
	printf("Middle Button: %s\n", (config[0] & MOUSE_MB_PRESS) ? "Pressed" : "Released");
	printf("Left Button: %s\n", (config[0] & MOUSE_LB_PRESS) ? "Pressed" : "Released");
	printf("Scaling Set To: %s\n", (config[0] & MOUSE_SCALE_2_1) ? "2.1" : "1.1");
	printf("State: %s\n", (config[0] & MOUSE_ENABLE_DATA) ? "Enable" : "Disable");
	printf("Mode: %s\n", (config[0] & MOUSE_MODE) ? "Remote" : "Stream");
	printf("Resolution: ");
	switch (config[1])
	{
	case 0:	printf("1"); break;
	case 1: printf("2"); break;
	case 2: printf("4"); break;
	case 3: printf("8"); break;
	}
	printf(" units per mm\n");
	printf("Sample Rate: %d Hz\n", config[2]);
}


void mouse_get_event(event_t *event, unsigned char *packet)
{
	if (packet[1] != 0 || packet[2] != 0)
	{
		event->type = MOVE;
		event->xdelta = mouse_get_xdelta(packet);
		event->ydelta = mouse_get_ydelta(packet);
	}
	else if (packet[0] & MOUSE_LB)
		event->type = LDOWN;
	else
		event->type = LUP;
}

void check_hor_line(state_machine *status, event_t *event, short length, unsigned short tolerance)
{
	switch(status->state)
	{
	case INIT:
		if (event->type == LDOWN)
		{
			status->state = DRAW;
			status->x = 0;
			status->y = 0;
		}
		break;
	case DRAW:
		if (event->type == LUP)
		{
			status->state = INIT;
			status->x = 0;
			status->y = 0;
		}
		else if (event->type == MOVE)
		{
			if ((length > 0 && event->xdelta > 0) ||(length < 0 && event->xdelta < 0))
			{
				status->x += event->xdelta;
				status->y += event->ydelta;

				if ((length > 0 && status->x > length) || (length < 0 && status->x < length))
					status->state = COMP;

				if (status->y > tolerance || status->y < -tolerance)
				{
					status->x = 0;
					status->y = 0;
				}
			}
			else
			{
				status->x = 0;
				status->y = 0;
			}
		}
	}
}
