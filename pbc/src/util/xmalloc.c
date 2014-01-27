#include <stdio.h>
#include <stdlib.h>
#include "xmalloc.h"

extern void *xmalloc(size_t size)
{
	void *p = NULL;
	if (!(p = malloc(size))) {
		fprintf(stderr, "malloc failed.\n");
		exit(1);
	}
	return p;
}

extern void xfree(void *p)
{
	if(!p) {
		free(p);
		p = NULL;
	}
}

extern char *xstrdup(char *str)
{
	void *p = NULL;
	if (!(p = strdup(str))) {
		fprintf(stderr, "strdup failed\n");
		exit(1);
	}
	return p;
}

