#include <stdio.h>
#include <stdlib.h>
#include <minix/drivers.h>
#include "test3.h"

static int proc_args(int argc, char *argv[]);
static unsigned long parse_ulong(char *str, int base);
static void print_usage(char *argv[]);

int main(int argc, char **argv)
{
	/* Initialize service */
	sef_startup();

	/* Enable IO-sensitive operations for ourselves */
	sys_enable_iop(SELF);

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
	printf("Usage: one of the following:\n"
			"\t service run %s -args \"scan <assembly>\" \n"
			"\t service run %s -args \"led <leds>\" \n"
			"\t service run %s -args \"timed_scan <time>\" \n", argv[0], argv[0], argv[0]);
}

static int proc_args(int argc, char *argv[])
{
	// check the function to test: if the first characters match, accept it
	if (strncmp(argv[1], "scan", strlen("scan")) == 0)
	{
		if (argc != 3)
		{
			printf(
					"keyboard: wrong no of arguments for test of kbd_test_scan() \n");
			return 1;
		}
		else
		{
			unsigned short assembly = parse_ulong(argv[2], 10);

			if (assembly == ULONG_MAX)
				return 1;

			if (kbd_test_scan(assembly) != 0)
			{
				printf(
						"keyboard: invalid mode selected for test of kbd_test_scan() \n");
				return 1;
			}

		}

		return 0;
	}
	else if (strncmp(argv[1], "led", strlen("led")) == 0)
	{
		if (argc < 3)
		{
			printf("keyboard: wrong no of arguments for test of kbd_test_leds() \n");
			return 1;
		}

		unsigned num_leds = argc - 2;
		unsigned short leds[num_leds];
		int i;

		for (i = 0; i < num_leds; i++)
		{
			unsigned long temp;
			temp = parse_ulong(argv[2 + i], 10);
			if (temp == ULONG_MAX)
				return 1;
			leds[i] = (unsigned short) temp;
			if (leds[i] > 2)
			{
				printf("keyboard: leds for test of kbd_test_leds() must be 0-2\n");
				return 1;
			}
		}

		if (kbd_test_leds(num_leds, leds) != 0)
		{
			printf("keyboard: wrong arguments for test of kbd_test_leds()\n");
			return 1;
		}
		return 0;
	}
	else if (strncmp(argv[1], "timed_scan", strlen("timed_scan")) == 0)
	{
		if (argc != 3)
		{
			printf("keyboard: wrong no of arguments for test of kbd_test_timed_scan() \n");
			return 1;
		}
		else
		{
			unsigned long time = parse_ulong(argv[2], 10);

			if (time == ULONG_MAX)
				return 1;

			if (kbd_test_timed_scan(time) != 0)
			{
				printf("keyboard: invalid time for test of kbd_test_timed_scan() \n");
				return 1;
			}
		}
	}
	else
	{
		printf("keyboard: non valid function \"%s\" to test\n", argv[1]);
		return 1;
	}
}

static unsigned long parse_ulong(char *str, int base)
{
	char *endptr;
	unsigned long val;

	val = strtoul(str, &endptr, base);

	if(str[0] == '-')
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
		printf("timer: parse_ulong: no digits were found in %s \n", str);
		return ULONG_MAX;
	}

	/* Successful conversion */
	return val;
}
