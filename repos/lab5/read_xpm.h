#ifndef __READ_XPM_H
#define __READ_XPM_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


char *read_xpm(char *map[], int *wd, int *ht);

void print_xpm(unsigned short xi, unsigned short yi, char *xpm[]);

#endif
