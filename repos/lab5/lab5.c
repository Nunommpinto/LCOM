#include <stdio.h>
#include <stdlib.h>
#include <minix/drivers.h>
#include "test5.h"
#include "pixmap.h"
#include <string.h>
#include "vbe.h"
#include "lmlib.h"

#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>


static int proc_args(int argc, char *argv[]);
static unsigned long parse_ulong(char *str, int base);
static void print_usage(char *argv[]);
static long parse_long(char *str, int base);

int main(int argc, char *argv[])
{

	sef_startup();

	if (argc == 1)
	{
		print_usage(argv);
		return 0;
	}
	else
	{
		proc_args(argc, argv);
	}

	return 0;
}

static void print_usage(char *argv[])
{
	printf(
			"Usage: one of the following:\n"
					"\t service run %s -args \"init <mode> <delay>\" \n"
					"\t service run %s -args \"square <x> <y> <size> <color>\" \n"
					"\t service run %s -args \"line <xi> <yi> <xf> <yf> <color>\" \n"
					"\t service run %s -args \"xpm <xi> <yi> <xpm>\" \n"
					"\t service run %s -args \"move <xi> <yi> <xpm> <hor> <delta> <time>\" \n"
					"\t service run %s -args \"controller\" \n", argv[0],
			argv[0], argv[0], argv[0], argv[0], argv[0]);
}

static int proc_args(int argc, char *argv[])
{
	// check the function to test: if the first characters match, accept it
	if (strncmp(argv[1], "init", strlen("init")) == 0)
	{
		if (argc != 4)
		{
			printf(
					"v_graph: wrong number of arguments for test of test_init() \n");
			return 1;
		}
		else
		{
			unsigned short mode = parse_ulong(argv[2], 16);

			unsigned short delay = parse_ulong(argv[3], 10);

			if (delay == USHRT_MAX)
				return 1;

			if (test_init(mode, delay) == NULL)
			{
				printf("v_graph: invalid test of test_init() \n");
				return 1;
			}

			return 0;
		}
	}
	else if (strncmp(argv[1], "square", strlen("square")) == 0)
	{
		if (argc != 6)
		{
			printf(
					"v_graph: wrong number of arguments for test of test_square() \n");
			return 1;
		}
		else
		{
			unsigned short x = parse_ulong(argv[2], 10);
			unsigned short y = parse_ulong(argv[3], 10);
			unsigned short size = parse_ulong(argv[4], 10);
			unsigned long color = parse_ulong(argv[5], 10);

			if (x == USHRT_MAX)
			{
				printf("v_graph: incorrect value for x coordinate\n");
				return 1;
			}
			else if (y == USHRT_MAX)
			{
				printf("v_graph: incorrect value for y coordinate\n");
				return 1;
			}
			else if (size == USHRT_MAX)
			{
				printf("v_graph: incorrect value for size parameter\n");
				return 1;
			}
			else if (color == ULONG_MAX)
			{
				printf("v_graph: incorrect value for color parameter\n");
				return 1;
			}
			if (test_square(x, y, size, color) != 0)
			{
				printf("v_graph: invalid test of test_square() \n");
				return 1;
			}
		}
	}
	else if (strncmp(argv[1], "line", strlen("line")) == 0)
	{
		if (argc != 7)
		{
			printf(
					"v_graph: wrong number of arguments for test of test_line() \n");
			return 1;
		}
		else
		{
			unsigned short xi = parse_ulong(argv[2], 10);
			unsigned short yi = parse_ulong(argv[3], 10);
			unsigned short xf = parse_ulong(argv[4], 10);
			unsigned short yf = parse_ulong(argv[5], 10);
			unsigned long color = parse_ulong(argv[6], 10);

			if (xi == USHRT_MAX)
			{
				printf("v_graph: incorrect value for initial x coordinate\n");
				return 1;
			}
			else if (yi == USHRT_MAX)
			{
				printf("v_graph: incorrect value for x coordinate\n");
				return 1;
			}
			else if (xf == USHRT_MAX)
			{
				printf("v_graph: incorrect value for final x coordinate\n");
				return 1;
			}
			else if (yf == USHRT_MAX)
			{
				printf("v_graph: incorrect value for final y coordinate\n");
				return 1;
			}
			else if (color == ULONG_MAX)
			{
				printf("v_graph: incorrect value for color parameter\n");
				return 1;
			}
			else if (test_line(xi, yi, xf, yf, color) != 0)
			{
				printf("v_graph: invalid test of test_line() \n");
				return 1;
			}
		}
	}
	else if (strncmp(argv[1], "xpm", strlen("xpm")) == 0)
	{
		if (argc != 5)
		{
			printf(
					"v_graph: wrong number of arguments for test of test_xpm() \n");
			return 1;
		}
		else
		{
			unsigned short xi = parse_ulong(argv[2], 10);
			unsigned short yi = parse_ulong(argv[3], 10);
			unsigned short xpm = parse_ulong(argv[4], 10);

			if (xi == USHRT_MAX)
			{
				printf("v_graph: incorrect value for initial x\n");
				return 1;
			}
			else if (yi == USHRT_MAX)
			{
				printf("v_graph: incorrect value for initial y\n");
				return 1;
			}
			else
			{
				if (xpm == 1)
				{
					if (test_xpm(xi, yi, pic1))
					{
						printf("v_graph: invalid test of test_xpm() \n");
						return 1;
					}
				}
				else if (xpm == 2)
				{
					if (test_xpm(xi, yi, pic2))
					{
						printf("v_graph: invalid test of test_xpm() \n");
						return 1;
					}
				}
				else if (xpm == 3)
				{
					if (test_xpm(xi, yi, cross))
					{
						printf("v_graph: invalid test of test_xpm() \n");
						return 1;
					}
				}
				else if (xpm == 4)
				{
					if (test_xpm(xi, yi, pic3))
					{
						printf("v_graph: invalid test of test_xpm() \n");
						return 1;
					}
				}
				else if (xpm == 5)
				{
					if (test_xpm(xi, yi, penguin))
					{
						printf("v_graph: invalid test of test_xpm() \n");
						return 1;
					}
				}
				else
				{
					printf("v_graph: invalid test of test_xpm() \n");
					return 1;
				}
			}
		}
	}
	else if (strncmp(argv[1], "move", strlen("move")) == 0)
	{
		if (argc != 8)
		{
			printf(
					"v_graph: wrong number of arguments for test of test_move() \n");
			return 1;
		}
		else
		{
			unsigned short xi = parse_ulong(argv[2], 10);
			unsigned short yi = parse_ulong(argv[3], 10);
			unsigned short xpm = parse_ulong(argv[4], 10);
			unsigned short hor = parse_ulong(argv[5], 10);
			unsigned short delta = parse_long(argv[6], 10);
			unsigned short time = parse_ulong(argv[7], 10);

			if (xi == USHRT_MAX)
			{
				printf("v_graph: incorrect value for initial x\n");
				return 1;
			}
			else if (yi == USHRT_MAX)
			{
				printf("v_graph: incorrect value for initial y\n");
				return 1;
			}
			else if (hor != 0 && hor != 1)
			{
				printf("v_graph: incorrect value for movement direction\n");
				return 1;
			}
			else if (delta == SHRT_MAX)
			{
				printf("v_graph: incorrect value for delta component\n");
				return 1;
			}
			else if (time == USHRT_MAX)
			{
				printf("v_graph: incorrect value for required time\n");
				return 1;
			}
			else
			{
				if (xpm == 1)
				{
					if (test_move(xi, yi, pic1, hor, delta, time))
					{
						printf("v_graph: invalid test of test_move() \n");
						return 1;
					}
				}
				else if (xpm == 2)
				{
					if (test_move(xi, yi, pic2, hor, delta, time))
					{
						printf("v_graph: invalid test of test_move() \n");
						return 1;
					}
				}
				else if (xpm == 3)
				{
					if (test_move(xi, yi, cross, hor, delta, time))
					{
						printf("v_graph: invalid test of test_move() \n");
						return 1;
					}
				}
				else if (xpm == 4)
				{
					if (test_move(xi, yi, pic3, hor, delta, time))
					{
						printf("v_graph: invalid test of test_move() \n");
						return 1;
					}
				}
				else if (xpm == 5)
				{
					if (test_move(xi, yi, penguin, hor, delta, time))
					{
						printf("v_graph: invalid test of test_move() \n");
						return 1;
					}
				}
				else
				{
					printf("v_graph: invalid pixmap \n");
					return 1;
				}

			}
		}
	}
	else if (strncmp(argv[1], "controller", strlen("controller")) == 0)
	{
		if (argc != 2)
		{
			printf("v_graph: wrong number of arguments for test of test_controller() \n");
			return 1;
		}

		if (test_controller() != OK)
		{
			return 1;
		}
	}

	return 0;
}

static unsigned long parse_ulong(char *str, int base)
{
	char *endptr;
	unsigned long val;

	val = strtoul(str, &endptr, base);

	if (str[0] == '-')
	{
		printf("Invalid negative arguments\n");
		return ULONG_MAX;
	}

	if ((errno == ERANGE && val == ULONG_MAX) || (errno != 0 && val == 0))
	{
		perror("strtol");
		return ULONG_MAX;
	}

	if (endptr == str)
	{
		printf("mouse: parse_ulong: no digits were found in %s \n", str);
		return ULONG_MAX;
	}

	/* Successful conversion */
	return val;
}

static long parse_long(char *str, int base)
{
	char *endptr;
	unsigned long val;

	val = strtoul(str, &endptr, base);

	if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))
			|| (errno != 0 && val == 0))
	{
		perror("strtol");
		return LONG_MAX;
	}

	if (endptr == str)
	{
		printf("mouse: parse_ulong: no digits were found in %s \n", str);
		return LONG_MAX;
	}

	/* Successful conversion */
	return val;
}

