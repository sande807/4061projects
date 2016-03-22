#include <stdio.h>
#include <string.h>
#include "util.h"
/* CSci4061 S2016 Assignment 2
section: 007 (Eric)
section: 002 (Nick)
date: mm/dd/yy
name: Eric Sande, Nick Orf
id: sande807, orfxx012 */

/*
 * Print shell prompt. Pass the username as argument.
 */
void print_prompt(char *name)
{
	printf("%s >> ", name);
}

/*
 * Checks if the first string starts with the second. Return 1 if true.
 */
int starts_with(const char *a, const char *b)
{
	if (strncmp(a, b, strlen(b)) == 0)
		return 1;
	else
		return 0;
}
