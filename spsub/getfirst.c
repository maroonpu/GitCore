/*---------------------------------------------------------------------
 * %Z%%M% Version %I% by IKEDA Mikio %E%
 * Description
 *   read first frame of speech  subroutine
 * ----------------------------------
 * getfirst(length, offset, is, stream)
 * ----------------------------------
 * arguments
 *  int   length : frame length
 *  int   offset : read start point (normally length / 2)
 *  short is[]   : signal data ( short integer )
 *  FILE  *stream: file pointer
 * return value 
 *  number of read data (normal return = length )
 *---------------------------------------------------------------------
 */

#include "ana.h"

int
getfirst(int length, int offset, short *is, FILE *stream)
{
    int nread;

    rewind(stream);
    nread = length - offset;
    if (nread < 0) return nread;
    while(offset--) *is++ = 0;
    nread = fread(is, sizeof(*is), nread, stream);
    return nread;
}
