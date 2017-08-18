#ifndef _STDIO_H
#include <stdio.h>
#endif
		/* interpolation formula for pitch detection */
#define getrt(a,b,c)   0.5 * ((a) - (c)) / ((a) + (c) - (b) - (b) )
#define getpk(a,b,c,x) (b) - 0.25 * ((a) - (c)) * (x)
#define round(x)	floor((x) + 0.5)

		/* fft rountine conversions */

#define cfft(m0, s)		cfftall((m0), (s), 1.0)
#define icfft(m0, s)		cfftall((m0), (s), -1.0)

		/* compatibility for elder version and variable types */
#ifndef PI
#define PI 3.1415926535897932384626434
#endif
#define DPI	0.31830988618379067154 /* 1.0 / PI */
#define SQRT2   1.41421356237309504880 /* sqrt(2.0) */
#define SQRTD2	0.70710678118654752440 /* sqrt(0.5) */
#define E	2.7182818284590452354  /* base of natural log */
#define LOG10E  0.43429448190325182765 /* log10(e) */
#define LN10	2.30258509299404568402 /* ln(10.0) */

#define  TRUE  1
#define  FALSE 0

extern void   cfftall(int, double *, double);
extern void   irfft(int, double *);
extern void   multirr(int, short *, double *, double *);
extern void   rfft(int, double *);

/* */
extern int    getfirst(int, int, short *, FILE *);
extern int    rdframe(int, int, short *, FILE *);
extern double *xd_realloc(double *, unsigned);
extern void   *xx_realloc(char *, unsigned);
