/*---------------------------------------------------------------------
 * %Z%%M% Version %I% by IKEDA mikio %E% 
 * Description
 *   FFT for real sequence
 * -----------
 * rfft(m0, x)
 * -----------
 * Arguments
 *   int    m0: FFT order (signal length = 2^m0)
 *   double *x: input real sequence/output conjugate symmteric spectra
 * Notes
 *   size of x[] must be greater of equal to 2^m0 + 2
 *   but there is 2 zeros
 *   Nth harmonic is
 *	F(n)  = x(2n) + j x(2n+1)
 *	F(-n) = x(2n) - j x(2n+1)
 *	(n = 0, 1, ... , 2^(m0-1))
 *   case of 0th and 2^(m0-1)th (or -2^(m0-1)th) harmonics
 *	F(0) = x(0) + j0
 *	F(2^(m0-1)) = F(-2^(m0-1)) = x(2^m0) + j0
 *---------------------------------------------------------------------
 */

#include <math.h>
#include "ana.h"

void
rfft(int m0, double *x)
{
    int nn, nn2, i, j;
    double d, ti0, tr0, ti1, tr1, ac, as;
    double sstep, cstep, s, c, ww;

    nn = 1 << m0;
    nn2 = nn/2;

    cfftall(m0-1, x, 1.0);
    d   = PI * 2.0 / (double)nn;
    cstep = cos(d);
    sstep = sin(d);

    c = cstep;
    s = sstep;

    for (i = 2; i < nn2; i += 2) {
    	j = nn - i;
        tr0 = (x[i]   + x[j])   * 0.5;
        ti0 = (x[i+1] - x[j+1]) * 0.5;
        tr1 = (x[i+1] + x[j+1]) * 0.5;
        ti1 = (x[j]   - x[i])   * 0.5;

        ac = tr1 * c - ti1 * s;
        as = ti1 * c + tr1 * s;

        x[j]   =  tr0 - ac;
        x[j+1] = -ti0 + as;
        x[i]   =  tr0 + ac;
        x[i+1] =  ti0 + as;

        ww = c * cstep - s * sstep;
        s  = s * cstep + c * sstep;
        c  = ww;
    }
    tr0     = x[0];
    tr1     = x[1];
    x[0]    = tr0 + tr1;
    x[1]    = 0.0;
    x[nn]   = tr0 - tr1;
    x[nn+1] = 0.0;
}
