#include <minix/drivers.h>
#include "i8254.h"
#include "timer.h"

static int proc_args(int argc, char *argv[]);
static unsigned long parse_ulong(char *str, int base);
static void print_usage(char *argv[]);

int main(int argc, char **argv)
{
	/* Initialize service */

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
	printf("Usage: one of the following:\n"
			"\t service run %s -args \"config <timer>\" \n"
			"\t service run %s -args \"square <frequency>\" \n"
			"\t service run %s -args \"int <time>\" \n", argv[0], argv[0],
			argv[0]);
}

static int proc_args(int argc, char *argv[])
{
	// check the function to test: if the first characters match, accept it
	if (strncmp(argv[1], "config", strlen("config")) == 0)
	{
		if (argc != 3)
		{
			printf(
					"timer: wrong no of arguments for test of timer_test_config() \n");
			return 1;
		}
		else
		{
			unsigned long timer = parse_ulong(argv[2], 10);

			if (timer_test_config(timer) != 0)
			{
				printf(
						"timer: invalid timer selected for test of timer_test_config() \n");
				return 1;
			}

		}

		return 0;
	}
	else if (strncmp(argv[1], "square", strlen("square")) == 0)
	{
		if (argc != 3)
		{
			printf(
					"timer: wrong no of arguments for test of timer_test_square() \n");
			return 1;
		}
		else
		{
			unsigned long frequency = parse_ulong(argv[2], 10);

			if (timer_test_square(frequency) != 0)
			{
				printf(
						"timer: invalid frequency selected for test of timer_test_square() \n");
				return 1;
			}
		}
	}
	else if (strncmp(argv[1], "int", strlen("int")) == 0)
	{
		if (argc != 3)
		{
			printf(
					"timer: wrong no of arguments for test of timer_test_int() \n");
			return 1;
		}
		else
		{
			unsigned long time = parse_ulong(argv[2], 10);

			if (timer_test_int(time) != 0)
			{
				printf(
						"timer: invalid number of time selected for test of timer_test_int() \n");
				return 1;
			}
		}
	}
	else
	{
		printf("timer: non valid function \"%s\" to test\n", argv[1]);
		return 1;
	}
}

static unsigned long parse_ulong(char *str, int base)
{
	char *endptr;
	unsigned long val;

	val = strtoul(str, &endptr, base);

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
