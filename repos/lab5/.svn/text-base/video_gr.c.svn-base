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
#include "video_gr.h"
#include "bitmanip.h"

/* Constants for VBE 0x105 mode */

/* The physical address may vary from VM to VM.
 * At one time it was 0xD0000000
 *  #define VRAM_PHYS_ADDR    0xD0000000 
 * Currently on lab B107 is 0xF0000000
 * Better run my version of lab5 as follows:
 *     service run `pwd`/lab5 -args "mode 0x105"
 */
#define VRAM_PHYS_ADDR		0xF0000000
#define H_RES             	1024
#define V_RES		  		768
#define BITS_PER_PIXEL		8

/* Private global variables */

static char *video_mem;

static unsigned h_res; /* Horizontal screen resolution in pixels */
static unsigned v_res; /* Vertical screen resolution in pixels */
static unsigned bits_per_pixel; /* Number of VRAM bits per pixel */

void *vg_init(unsigned short mode)
{
	vbe_mode_info_t vmi;
	if (vbe_get_mode_info(mode, &vmi) != OK)
		return NULL;

	if (!(vmi.ModeAttributes && SUPPORTED))
	{
		printf("Mode 0x%04X not supported\n", mode);
		return NULL;
	}

	struct reg86u reg86;

	reg86.u.b.intno = VIDEO_CARD; /* BIOS video services */

	reg86.u.b.ah = CALL_VBE_FUNC; /* VBE function call */
	reg86.u.b.al = SET_VBE_MODE; /* Set VBE mode*/
	reg86.u.w.bx = mode | LINEAR_FRAME_BUFFER; /* mode to be set */

	if (sys_int86(&reg86) != OK)
	{
		printf("\tvg_init(): sys_int86() failed \n");
		return NULL;
	}

	int r;
	struct mem_range mr;

	/* Allow memory mapping */

	mr.mr_base = vmi.PhysBasePtr;
	mr.mr_limit = mr.mr_base
			+ vmi.BitsPerPixel * vmi.XResolution * vmi.YResolution / 8;

	if (OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
		panic("video_txt: sys_privctl (ADD_MEM) failed: %d\n", r);

	/* Map memory */

	video_mem = vm_map_phys(SELF, (void *) mr.mr_base,
			vmi.BitsPerPixel * vmi.XResolution * vmi.YResolution / 8);

	if (video_mem == MAP_FAILED)
		panic("video_txt couldn't map video memory");

	h_res = vmi.XResolution;
	v_res = vmi.YResolution;
	bits_per_pixel = vmi.BitsPerPixel;

	return video_mem;
}

int vg_exit()
{
	struct reg86u reg86;

	reg86.u.b.intno = VIDEO_CARD; /* BIOS video services */

	reg86.u.b.ah = 0x00; /* Set Video Mode function */
	reg86.u.b.al = 0x03; /* 80x25 text mode*/

	if (sys_int86(&reg86) != OK)
	{
		printf("\tvg_exit(): sys_int86() failed \n");
		return 1;
	}
	else
		return 0;
}

void vg_set_pixel(unsigned int x, unsigned int y, unsigned long color)
{
	if (x >= h_res || y >= v_res)
		return;
	video_mem[(y * h_res + x) * bits_per_pixel / 8] = color;
}

void vg_fill(unsigned int x, unsigned int y, unsigned int width,
		unsigned int height, unsigned long color)
{
	unsigned int i, j;
	for (i = 0; i < height; ++i)
		for (j = 0; j < width; ++j)
			vg_set_pixel(x + i, y + j, color);
}

void vg_line(unsigned int xi, unsigned int yi, unsigned int xf, unsigned int yf,
		unsigned long color)
{
	int i, dx, dy, sdx, sdy, dxabs, dyabs, x, y, px, py;

	dx = xf - xi; /* the horizontal distance of the line */
	dy = yf - yi; /* the vertical distance of the line */
	dxabs = abs(dx);
	dyabs = abs(dy);
	sdx = dx >= 0 ? 1 : -1;
	sdy = dy >= dy ? 1 : -1;
	x = dyabs >> 1;
	y = dxabs >> 1;
	px = xi;
	py = yi;

	if (dxabs >= dyabs) /* the line is more horizontal than vertical */
	{
		for (i = 0; i < dxabs; i++)
		{
			y += dyabs;
			if (y >= dxabs)
			{
				y -= dxabs;
				py += sdy;
			}
			px += sdx;
			vg_set_pixel(px, py, color);
		}
	}
	else /* the line is more vertical than horizontal */
	{
		for (i = 0; i < dyabs; i++)
		{
			x += dxabs;
			if (x >= dyabs)
			{
				x -= dyabs;
				px += sdx;
			}
			py += sdy;
			vg_set_pixel(px, py, color);
		}
	}
}

void vg_animate_sprite(Sprite* sp)
{
	if (sp == NULL)
		return;

	if (sp->x + sp->width + sp->xspeed > h_res)
	{
		sp->x = h_res - sp->width;
		draw_sprite(sp);
		sp->xspeed = 0;
	}
	else if (sp->x + sp->xspeed < 0)
	{
		sp->x = 0;
		draw_sprite(sp);
		sp->xspeed = 0;
	}
	else if (sp->y + sp->height + sp->yspeed > v_res)
	{
		sp->y = v_res - sp->height;
		draw_sprite(sp);
		sp->yspeed = 0;
	}
	else if (sp->y + sp->yspeed < 0)
	{
		sp->y = 0;
		draw_sprite(sp);
		sp->yspeed = 0;
	}
	else
	{
		sp->x += sp->xspeed;
		sp->y += sp->yspeed;

		draw_sprite(sp);
	}
}

