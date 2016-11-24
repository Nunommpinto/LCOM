#ifndef MOUSE_H
#define MOUSE_H

typedef enum {INIT, DRAW, COMP} state_t;
typedef enum {LDOWN, LUP, MOVE} ev_type_t;
typedef struct
{
	ev_type_t type;
	short xdelta;
	short ydelta;
} event_t;

typedef struct
{
	state_t state;
	short x;
	short y;
} state_machine;

void mouse_print_packet(unsigned char *packet);

void mouse_print_config(unsigned char *config);

void mouse_get_event(event_t *event, unsigned char *packet);

short mouse_get_xdelta(unsigned char *packet);

short mouse_get_ydelta(unsigned char *packet);

void check_hor_line(state_machine *status, event_t *event, short length, unsigned short tolerance);

#endif // MOUSE_H
