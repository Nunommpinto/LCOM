#include <stdio.h>
#include <stdlib.h>
#include <minix/drivers.h>
#include "test4.h"

static int proc_args(int argc, char *argv[]);
static unsigned long parse_ulong(char *str, int base);
static void print_usage(char *argv[]);
static long parse_long(char *str, int base);

int main(int argc, char *argv[])
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
			"\t service run %s -args \"packet <packets>\" \n"
			"\t service run %s -args \"async <time>\" \n"
			"\t service run %s -args \"config\" \n"
			"\t service run %s -args \"gesture <length> <tolerance>\" \n", argv[0],
			argv[0], argv[0], argv[0]);
}

static int proc_args(int argc, char *argv[])
{
	// check the function to test: if the first characters match, accept it
	if (strncmp(argv[1], "packet", strlen("packet")) == 0)
	{
		if (argc != 3)
		{
			printf("mouse: wrong no of arguments for test of test_packet() \n");
			return 1;
		}

		unsigned long cnt = parse_ulong(argv[2], 10);

		if (cnt > USHRT_MAX)
		{
			printf("mouse: argument out of range for test of test_packet() \n");

			return 1;
		}

		if (test_packet(cnt) != 0)
		{
			printf("mouse: wrong arguments for test of test_packet() \n");
			return 1;
		}

		return 0;
	}
	else if (strncmp(argv[1], "async", strlen("async")) == 0)
	{
		unsigned long n;

		if (argc != 3)
		{
			printf("mouse: wrong no of arguments for test of test_async() \n");
			return 1;
		}
		else
		{
			unsigned long time = parse_ulong(argv[2], 10);

			if (time == ULONG_MAX)
				return 1;

			if (test_async(time) != 0)
			{
				printf(
						"keyboard: invalid time for test of test_async() \n");
				return 1;
			}

			return 0;
		}
	}
	else if (strncmp(argv[1], "config", strlen("config")) == 0)
	{
		if (argc != 2)
		{
			printf(
					"mouse: wrong no of arguments for test of test_config() \n");
			return 1;
		}
		else
		{
			if (test_config() != 0)
			{
				printf(
						"mouse: invalid call for test of test_config() \n");
				return 1;
			}

			return 0;
		}
	}
	else if (strncmp(argv[1], "gesture", strlen("gesture")) == 0)
	{
		if (argc != 4)
		{
			printf("mouse: wrong no of arguments for test of test_gesture() \n");

			return 1;
		}

		long length = parse_long(argv[2],10);

		if(length > USHRT_MAX)
		{
			printf("mouse: argument length out of range for test_gesture()\n");

			return 1;
		}

		unsigned long tolerance = parse_ulong(argv[3],10);

		if(tolerance > SHRT_MAX)
		{
			printf("mouse: argument tolerance out of range for test_gesture()\n");

			return 1;
		}

		if(test_gesture(length,tolerance) != 0)
		{
			printf("mouse: wrong arguments for test_gesture()\n");

			return 1;
		}

		return 0;
	}
	else
	{
		printf("mouse: non valid function \"%s\" to test\n", argv[1]);
		return 1;
	}
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

	if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN)) || (errno != 0 && val == 0))
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

