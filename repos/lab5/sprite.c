#include "sprite.h"
#include "video_gr.h"
#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vbe.h"
#include "lmlib.h"
#include <math.h>

/** Creates a new sprite with pixmap "pic", random speeds
 *   (not zero) and position (within the screen limits), and
 *	draws it in memory whose address is "base";
 *	Returns NULL on invalid pixmap.
 */

Sprite *create_sprite(char *pic[], unsigned short xi, unsigned short yi,
		unsigned short hor, short delta, unsigned short time)
{ //allocate space for the "object"
	Sprite* sp = (Sprite*) malloc(sizeof(Sprite));

	if (sp == NULL)
		return NULL;

// read the sprite pixmap
	sp->map = read_xpm(pic, &(sp->width), &(sp->height));

	if (sp->map == NULL)
	{
		free(sp);
		return NULL;
	}

	float vel = (float) delta / ((double) time * 60);

	if (hor == 1)
	{
		sp->xspeed = vel;
		sp->yspeed = 0;
	}
	else if (hor == 0)
	{
		sp->xspeed = 0;
		sp->yspeed = vel;
	}

	sp->x = xi;
	sp->y = yi;

	return sp;
}

void destroy_sprite(Sprite* sp)
{
	if (sp == NULL)
		return;

	free(sp->map);

	free(sp);

	sp = NULL; // hopeless: pointer is passed by value
}

void draw_sprite(Sprite* sp)
{
	int i, j;

	if (sp == NULL)
		return;

	char* mapc = sp->map;

	clear_sprite(sp);

	for (i = 0; i < sp->height; i++)
	{
		for (j = 0; j < sp->width; j++)
		{
			vg_set_pixel(sp->x + j, sp->y + i, *mapc);
			mapc++;
		}
	}
}

void clear_sprite(Sprite *sp)
{
	int i, j;

	if (sp->xspeed > 0)
	{
		for (i = 0; i < sp->height; i++)
			for (j = 0; j <= (int) sp->xspeed; j++)
				vg_set_pixel((int) (sp->x - sp->xspeed) + j, (int) sp->y + i,
						0);
	}
	else if (sp->xspeed < 0)
	{
		for (i = 0; i < sp->height; i++)
			for (j = 0; j <= (int) abs(sp->xspeed); j++)
				vg_set_pixel(sp->x + sp->width + j, sp->y + i, 0);
	}
	else if (sp->yspeed > 0)
	{
		for (i = 0; i <= sp->yspeed; i++)
			for (j = 0; j < (int) sp->width; j++)
				vg_set_pixel((int) sp->x + j, (int) (sp->y - sp->yspeed) + i,
						0);
	}
	else if (sp->yspeed < 0)
	{
		for (i = 0; i <= (int) abs(sp->yspeed); i++)
			for (j = 0; j < (int) sp->width; j++)
				vg_set_pixel((int) sp->x + j, (int) (sp->y) + sp->height + i,
						0);
	}
}
