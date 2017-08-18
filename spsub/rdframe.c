/*---------------------------------------------------------------------
 * %Z%%M% Version %I% by IKEDA Mikio %E%
 * Description
 *   read frame datas
 * ----------------------------------
 * rdframe(length, shift, is, stream)
 * ----------------------------------
 * arguments
 *  int   length : frame length
 *  int   shift  : frame shift ( <= length )
 *  short is[]   : signal data (short integer)
 *  FILE  *stream : file pointer
 * return value
 *  number of read data (normal return == shift) 
 *---------------------------------------------------------------------
 */

#include "ana.h"

int
rdframe(int length, int shift, short *is, FILE *stream)
{
    int  i, nread, leneff;

    if (length < shift) shift = length;
    leneff = length - shift;
    for (i = 0; i < leneff; ++i) is[i] = is[i + shift];
    nread = fread(&is[leneff], sizeof(*is), shift, stream);
    for (i = nread; i < shift; ++i) is[i+leneff] = 0;
    return(nread);
}
