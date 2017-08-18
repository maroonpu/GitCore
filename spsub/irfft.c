/*---------------------------------------------------------------------
 * %Z%%M% Version %I% by IKEDA mikio %E%
 * Description
 *   inverse Fourier transformation for complex conjufgate spectra
 * ------------
 * irfft(m0, x)
 * ------------
 * Arguments
 *   int    m0 :
 *   double *x : input spectra / output sequence
 *---------------------------------------------------------------------
 */

#include <math.h>
#include "ana.h"

void
irfft(int m0, double *x)
{
    int nn, i, j;
    double d, ti0, tr0, ti1, tr1, ac, as;
    double sstep, cstep, s, c, ww;

    nn = 1 << m0;

    d   = PI * 2.0 / (double)nn;
    cstep = cos(d);
    sstep = sin(d);

    c = cstep;
    s = sstep;

    for (i = 2; i < (j = nn - i); i += 2) {
        tr0 = (x[i]   + x[j])* 0.5;
        ti0 = (x[i+1] - x[j+1]) * 0.5;
        as  = (x[i+1] + x[j+1]) * 0.5;
        ac  = (x[i]   - x[j]) * 0.5;

        tr1 = ac * c + as * s;
        ti1 = as * c - ac * s;

        x[i]   =  tr0 - ti1;
        x[i+1] =  ti0 + tr1;
        x[j]   =  tr0 + ti1;
        x[j+1] =  tr1 - ti0;

        ww = c * cstep - s * sstep;
        s  = s * cstep + c * sstep;
        c  = ww;
    }
    tr0  = x[0] + x[nn];
    tr1  = x[0] - x[nn];
    x[0] = tr0 * 0.5;
    x[1] = tr1 * 0.5;
    cfftall(m0-1, x, -1.0);
}
