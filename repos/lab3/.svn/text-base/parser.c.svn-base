#include "parser.h"

unsigned long parse_ulong(char *str, int base)
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
		printf("parser: parse_ulong: no digits were found in %s \n", str);
		return ULONG_MAX;
	}

	/* Successful conversion */
	return val;
}
