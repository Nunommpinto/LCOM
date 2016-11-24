#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vbe.h"
#include "lmlib.h"

#define BIT(x)	(0x1 << x)
#define PB2BASE(x) (((x) >> 4) & 0x0F000)
#define PB2OFF(x) ((x) & 0x0FFFF)

static void *lm_p = NULL;

void *lm_start()
{
	if (lm_p == NULL)
		lm_p = lm_init();

	return lm_p;
}

int vbe_get_mode_info(unsigned short mode, volatile vbe_mode_info_t *vmi_p)
{
	struct reg86u reg86;
	mmap_t map;

	if (lm_start() == NULL)
			return 1;

	lm_alloc(sizeof(vbe_mode_info_t), &map);

	reg86.u.b.intno = VIDEO_CARD;
	reg86.u.b.ah = CALL_VBE_FUNC;
	reg86.u.b.al = RET_VBE_MODE_INFO;	/* Return VBE mode information */
	reg86.u.w.cx = mode;				/* mode number */
	reg86.u.w.es = PB2BASE(map.phys);	/* VBE mode info base pointer */
	reg86.u.w.di = PB2OFF(map.phys); 	/* VBE mode info offset */

	if (sys_int86(&reg86) != OK)
	{
		printf("\tvbe_get_mode_info(): sys_int86() failed \n");
		return 1;
	}

	*vmi_p = *((vbe_mode_info_t *)(map.virtual));
	lm_free(&map);
  
	return OK;
}

int vbe_get_info_block(vbe_info_block_t *vib_p)
{
	struct reg86u reg86;
	mmap_t map;

	if (lm_start() == NULL)
			return 1;

	lm_alloc(sizeof(vbe_info_block_t), &map);

	memcpy(((vbe_info_block_t *)(map.virtual))->VbeSignature, "VBE2", 4);
	((vbe_info_block_t *)(map.virtual))->VbeVersion = 0X200;

	reg86.u.b.intno = VIDEO_CARD;
	reg86.u.b.ah = CALL_VBE_FUNC;
	reg86.u.b.al = RET_VBE_CTRL_INFO;	/* Return VBE controller information */
	reg86.u.w.es = PB2BASE(map.phys);
	reg86.u.w.di = PB2OFF(map.phys);

	if (sys_int86(&reg86) != OK)
	{
		printf("\tvbe_get_info_block(): sys_int86() failed \n");
		return 1;
	}

	*vib_p = *((vbe_info_block_t *)(map.virtual));

	uint16_t *reserved = ((uint32_t)(lm_p)) + (vib_p->VideoModePtr_Seg << 4 | vib_p->VideoModePtr_Off);
	int i;
	for (i = 0; i < 111; ++i)
		vib_p->Reserved[i] = reserved[i];

	lm_free(&map);

	return OK;
}

int get_supported_modes(uint16_t *modes, uint16_t **supported, unsigned *supported_size)
{
	uint16_t temp[111];
	uint16_t *temp_ptr = temp;

	while (*modes != 0xFFFF)
	{
		vbe_mode_info_t vmi;

		if (vbe_get_mode_info(*modes, &vmi) != OK)
			return 1;

		if (vmi.ModeAttributes && BIT(0))
			*(temp_ptr++) = *modes;

		++modes;
	}

	*supported_size = (temp_ptr - temp);
	*supported = malloc(sizeof(uint16_t) * (*supported_size));

	if (supported == NULL)
		return 1;

	int i;
	for (i = 0; i < *supported_size; ++i)
		(*supported)[i] = temp[i];

	return OK;
}


void vbe_info_display(vbe_info_block_t *vib_p)
{
	uint16_t *supported;
	unsigned supported_size;
	get_supported_modes(vib_p->Reserved, &supported, &supported_size);

	printf("Capabilities:\n");
	{
		if (vib_p->Capabilities[0] && BIT(0))
			printf("\tDAC width is switchable to 8 bits per primary color\n");
		else
			printf("\tDAC is fixed width, with 6 bits per primary color\n");
		if (vib_p->Capabilities[0] && BIT(1))
			printf("\tController is not VGA compatible\n");
		else
			printf("\tController is VGA compatible\n");

		if (vib_p->Capabilities[0] && BIT(2))
			printf("\tWhen programming large blocks of information to the RAMDAC,"
					" use the blank bit in Function 09h\n");
		else
			printf("\tNormal RAMDAC operation\n");
	}

	printf("Supported modes:");
	int i;
	for (i = 0; i < supported_size; ++i)
		printf("%s0x%04X", i % 10 == 0 ? "\n\t" : " ", supported[i]);
	printf("\n");
	printf("VRAM size: %ld KB\n", vib_p->TotalMemory * 64);
}
