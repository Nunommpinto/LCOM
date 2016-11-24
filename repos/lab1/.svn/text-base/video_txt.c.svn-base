#include <minix/drivers.h>
#include <sys/video.h>
#include <sys/mman.h>

#include <assert.h>

#include "vt_info.h"

#include "video_txt.h"

/* Private global variables */

static char *video_mem; /* Address to which VRAM is mapped */

static unsigned scr_width; /* Width of screen in columns */
static unsigned scr_lines; /* Height of screen in lines */

void vt_fill(char ch, char attr)
{
	char *vptr;

	for (vptr = video_mem; vptr < video_mem + 2 * scr_width * scr_lines; vptr +=
			2)
	{
		*vptr = ch;
		*(vptr + 1) = attr;
	}
}

void vt_blank()
{
	vt_fill(0x00, 0x00);
}

int vt_print_char(char ch, char attr, int r, int c)
{
	if (c < 0 || c >= scr_width || r < 0 || r >= scr_lines)
		return -1;

	char *vptr = video_mem + 2 * (r * scr_width + c);
	*vptr = ch;
	*(vptr + 1) = attr;

	return 0;
}

int vt_print_string(char *str, char attr, int r, int c)
{
	while (*str != '\0')
	{
		if (vt_print_char(*str, attr, r, c) == -1)
			return -1;

		++str;
		++c;

		if (c == scr_width)
		{
			c = 0;
			++r;
		}

		if (r == scr_lines)
		{
			r = 0;
		}
	}

	return 0;
}

int vt_print_int(int num, char attr, int r, int c)
{
	int num_digits = 1;
	int sign = 0;

	if (num < 0)
	{
		sign = 1;
		num = -num;
	}

	int temp_num = num;

	while (temp_num > 9)
	{
		temp_num /= 10;
		++num_digits;
	}

	char str[sign + num_digits + 1];

	str[num_digits + sign] = '\0';

	while (num_digits-- > 0)
	{
		str[num_digits + sign] = num % 10 + '0';
		num /= 10;
	}

	if (sign != 0)
		str[0] = '-';

	return vt_print_string(str, attr, r, c);
}

int vt_draw_frame(int width, int height, char attr, int r, int c)
{
	if (width <= 1 || height <= 1 ||
		c < 0 || c + width >= scr_width ||
		r < 0 || r + height >= scr_lines)
		return -1;

	int max_c = c + width - 1;
	int max_r = r + height - 1;
	int temp;

	// Place corners
	if (vt_print_char(UL_CORNER, attr, r, c) == -1 ||
		vt_print_char(UR_CORNER, attr, r, max_c) == -1 ||
		vt_print_char(LL_CORNER, attr, max_r, c) == -1 ||
		vt_print_char(LR_CORNER, attr, max_r, max_c) == -1)
		return -1;

	// Horizontal lines
	for (temp = c + 1; temp < max_c; ++temp)
	{
		if (vt_print_char(HOR_BAR, attr, r, temp) == -1 ||
		    vt_print_char(HOR_BAR, attr, max_r, temp) == -1)
			return -1;
	}

	// Vertical lines
	for (temp = r + 1; temp < max_r; ++temp)
	{
		if (vt_print_char(VERT_BAR, attr, temp, c) == -1 ||
			vt_print_char(VERT_BAR, attr, temp, max_c) == -1)
			return -1;
	}

	return 0;
}

/*
 * THIS FUNCTION IS FINALIZED, do NOT touch it
 */

char *vt_init(vt_info_t *vi_p)
{
	int r;
	struct mem_range mr;

	/* Allow memory mapping */

	mr.mr_base = (phys_bytes)(vi_p->vram_base);
	mr.mr_limit = mr.mr_base + vi_p->vram_size;

	if (OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
		panic("video_txt: sys_privctl (ADD_MEM) failed: %d\n", r);

	/* Map memory */

	video_mem = vm_map_phys(SELF, (void *) mr.mr_base, vi_p->vram_size);

	if (video_mem == MAP_FAILED)
		panic("video_txt couldn't map video memory");

	/* Save text mode resolution */

	scr_lines = vi_p->scr_lines;
	scr_width = vi_p->scr_width;

	return video_mem;
}
