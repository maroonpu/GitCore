/*---------------------------------------------------------------------
 * %Z%%M% Version %I% by IKEDA Mikio %E%
 * Description
 *   multiply integer and real to real
 * ----------------------------
 * multirr(length,id,win,frame)
 * ----------------------------
 * Arguments
 *   int   length  : data length
 *   short id[]    : integer*2 data
 *   double win[]   : real multiples
 *   double frame[] : result
 *---------------------------------------------------------------------
 */
#include "ana.h"
void
multirr(int length, short *id, double *win, double *frame)
{
    while(--length >= 0) *frame++ = (double)*id++ * *win++;
}
