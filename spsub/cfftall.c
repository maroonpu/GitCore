/*---------------------------------------------------------------------
 * @(#)cfftall.c Version 1.1 by IKEDA Mikio 91/03/11
 * Comblex fft basic function
 * --------------------
 * cfftall(m0, x, ainv)
 * --------------------
 * arguments
 *   int   m0   : 2**m0 = length
 *   double x[]  : complex signal --> spectrum
 *   double ainv : inverse switch (inverse = -1.0)
 *---------------------------------------------------------------------
 */

#include <math.h>
#include "ana.h"

void cfftall(int m0, double *x, double ainv)
{
    int    i, j, lm, li, k, lmx, lmx2, np, lix;
    double  temp1, temp2;
    double  c, s, csave, sstep, cstep;
    double  c0, s0, c1, s1;

    lmx = 1 << m0;

    csave = PI * 2.0 / (double)lmx;
    cstep = cos(csave);
    sstep = sin(csave);

    lmx += lmx;
    np   = lmx;

/*----- fft butterfly numeration */
    for (i = 3; i <= m0; ++i) {
	lix = lmx;
	lmx >>= 1;
	lmx2 = lmx >> 1;
	c = cstep;
	s = sstep;
	s0 = ainv * s;
	c1 = -s;
	s1 = ainv * c;
	for (li = 0; li < np; li += lix ) {
	    j = li;
	    k = j + lmx;
	    temp1  = x[j] - x[k];
	    x[j]  += x[k];
	    x[k]   = temp1;
	    temp2  = x[++j] - x[++k];
	    x[j]  += x[k];
	    x[k]   = temp2;

	    temp1  = x[++j] - x[++k];
	    x[j]  += x[k];
	    temp2  = x[++j] - x[++k];
	    x[j]  += x[k];
	    x[k-1] = c * temp1 - s0 * temp2;
	    x[k]   = s0 * temp1 + c * temp2;

	    j = li + lmx2;
	    k = j + lmx;
	    temp1  = x[j] - x[k];
	    x[j]  += x[k];
	    temp2  = x[++j] - x[++k];
	    x[j]  += x[k];
	    x[k-1] = -ainv * temp2;
	    x[k]   =  ainv * temp1;

	    temp1  = x[++j] - x[++k];
	    x[j]  += x[k];
	    temp2  = x[++j] - x[++k];
	    x[j]  += x[k];
	    x[k-1] = c1 * temp1 - s1 * temp2;
	    x[k]   = s1 * temp1 + c1 * temp2;

	}
	for (lm = 4; lm < lmx2; lm += 2) {
	    csave = c;
	    c = cstep * c - sstep * s;
            s = sstep * csave + cstep * s;

	    s0 = ainv * s;
	    c1 = -s;
	    s1 = ainv * c;

	    for (li = 0; li < np; li += lix ) {
		j = li + lm;
		k = j + lmx;
		temp1  = x[j] - x[k];
	        x[j]  += x[k];
		temp2  = x[++j] - x[++k];
		x[j]  += x[k];
	        x[k-1] = c * temp1 - s0 * temp2;
	        x[k]   = s0 * temp1 + c * temp2;

		j = li + lm + lmx2;
		k = j + lmx;
		temp1  = x[j] - x[k];
	        x[j]  += x[k];
		temp2  = x[++j] - x[++k];
		x[j]  += x[k];
	        x[k-1] = c1 * temp1 - s1 * temp2;
	        x[k]   = s1 * temp1 + c1 * temp2;
	    }
	}
	csave = cstep;
	cstep = 2.0 * cstep * cstep - 1.0;
	sstep = 2.0 * sstep * csave;
    }
    if (m0 >= 2)
	for (li = 0; li < np; li += 8) {
	    j = li;
	    k = j + 4;
	    temp1 = x[j] - x[k];
	    x[j] += x[k];
	    temp2 = x[++j] - x[++k];
	    x[j] += x[k];
	    x[k-1] = temp1;
	    x[k]   = temp2;
	    temp1  = x[++j] - x[++k];
	    x[j]  += x[k];
	    temp2  = x[++j] - x[++k];
	    x[j]  += x[k];
	    x[k-1] = -ainv * temp2;
	    x[k]   =  ainv * temp1;
	}
    for (li = 0; li < np; li += 4) {
	j = li;
	k = j + 2;
	temp1 = x[j] - x[k];
	x[j]  += x[k];
	x[k]   = temp1;
	temp2  = x[++j] - x[++k];
	x[j]  += x[k];
	x[k]   = temp2;
    }

/*----- fft bit reversal */
    lmx = np / 2;
    j = 0;
    for (i = 2; i < np - 2; i += 2) {
	k = lmx;
	while(k <= j) {
	    j -= k;
	    k >>= 1;
	}
	j += k;
        if ( i < j ) {
	    temp1 = x[j];
	    x[j] = x[i];
	    x[i] = temp1;
	    lm = j + 1;
	    li = i + 1;
	    temp1 = x[lm];
	    x[lm] = x[li];
	    x[li] = temp1;
	}
    }
    if (ainv == 1.0) return;

    temp1 = 1.0 / (double)lmx;
    for (i = 0; i < np; ++i) *x++ *= temp1;
    return;
}
