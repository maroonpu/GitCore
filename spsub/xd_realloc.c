/*
*/

#include <stdlib.h>
#include "ana.h"

double *
xd_realloc(double *ptr, unsigned nitems)
{
    double *tmp;

    if (ptr == NULL)
	tmp = (double *)malloc(nitems * sizeof(*ptr));
    else
	tmp = (double *)realloc(ptr, nitems * sizeof(*ptr));
    if (NULL == tmp) {
	fprintf(stderr, "xd_realloc failed !!\n");
	exit(0);
    }
    return tmp;
}

void *
xx_realloc(char *ptr, unsigned size)
{
    void *tmp;
    if (ptr == NULL)
	tmp = (void *)malloc(size * sizeof(*ptr));
    else
	tmp = (void *)realloc(ptr, size * sizeof(*ptr));
    if (NULL == tmp) {
	fprintf(stderr, "xx_realloc failed !!\n");
	exit(0);
    }
    return tmp;
}
