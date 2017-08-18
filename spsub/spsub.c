/*---------------------------------------------------------------------
 * Enhancing Speech Degraded by spectral subtruction
 *				by IKEDA Mikio
 *---------------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "ana.h"

#define DEFAULT_LENGTH    256
#define DEFAULT_SHIFT     32
#define DEFAULT_MULTIPLE  1.0
#define DEFAULT_SMOOTHING 0.95

#define p_err(x,y,z) fprintf(stderr, x, y, z)
#define CONFLICT(x, y) p_err("Error from %s : %c switch conflict.\n", x, y)
#define ILL(x,y) p_err("Error from %s : illegal specification of -%c switch.\n", x, y)

main(int argc, char **argv)
{
    short *is, *ix;
    int  m0, i, j;
    int  length = 0;
    int  shift = 0, lhalf, lpow2, half_pow2;
    int  nread, noffile = 0;
    double smul = 0.0;
    double lambda = 0.0;
    double ar, ai, power;
    double *win, *frame, *noise, *pre;
    double *rev_win;
    char *argin;
    FILE *srcfd, *dstfd;
    
    double kk;

    if (argc <= 2) {
	usage(argv[0]);
    }
    for (i = 1; i < argc; ++i) {
	argin = argv[i];
	if (*argin == '-') {
	    switch (*++argin) {
	      case 'm' :
		if (smul != 0.0) {
		    CONFLICT(argv[0], 'm');
		    exit(0);
		}
		smul = strtod(++argin, NULL);
		if (smul <= 0.0) {
		    ILL(argv[0], 'm');
		    exit(0);
		}
		break;
	      case 'l':
		if (length != 0) {
		    CONFLICT(argv[0], 'l');
		    exit(0);
		}
		length = strtol(++argin, NULL, 10);
		if (length <= 0 || (shift != 0 && length < shift + shift)) {
		    ILL(argv[0], 'l');
		    exit(0);
		}
		break;
	      case 's':
		if (shift != 0) {
		    CONFLICT(argv[0], 's');
		    exit(0);
		}
		length = strtol(++argin, NULL, 10);
		if (shift <= 0 || (length != 0 && length < shift + shift)) {
		    ILL(argv[0], 's');
		    exit(0);
		}
		break;
	      case 'r':
		if (lambda != 0.0) {
		    CONFLICT(argv[0], 'r');
		    exit(0);
		}
		lambda = strtod(++argin, NULL);
		if (lambda <= 0.0) {
		    ILL(argv[0], 'r');
		    exit(0);
		}
		break;
	      default:
		p_err("Error from %s : such switch -%c does not exist.\n",
		      argv[0], *argin);
		exit(0);
		break;
	    }
	}else{
	    ++noffile;
	    if (noffile == 1) {
		if (NULL == (srcfd = fopen(argv[i], "r"))) {
		    p_err("Error from %s: File '%s' open failed.\n",
			  argv[0], argv[i]);
		    exit(2);
		}
	    }else if (noffile == 2) {
		if (NULL == (dstfd = fopen(argv[i], "w"))) {
		    p_err("Error from %s: File '%s' open failed.\n",
			  argv[0], argv[i]);
		    exit(2);
		}
	    }else{
		fprintf(stderr, "Error from %s: too many arguments.\n",
			argv[0]);
		exit(3);
	    }
	}
    }
    if (noffile < 2) {
	fprintf(stderr, "Error from %s : too fewer arguments.\n", argv[0]);
	exit(3);
    }

    /*----------------------- trim variables ----------------------*/
    if (0   == length) length = DEFAULT_LENGTH;
    if (0   == shift)  shift  = DEFAULT_SHIFT;
    if (0.0 == smul)   smul = DEFAULT_MULTIPLE;
    if (0.0 == lambda) lambda = DEFAULT_SMOOTHING;

    m0 = 0;
    lpow2 = 1;
    lhalf = ((length + 1)/2);
    length = lhalf + lhalf;    /* rounding */
    while(lpow2 < length) {
	lpow2 += lpow2;
	++m0;
    }
    half_pow2 = lpow2/2;

    /*------------------ allocate memory ------------------*/
    frame = xd_realloc(NULL, lpow2+2);
    noise = xd_realloc(NULL, half_pow2+1);
    win   = xd_realloc(NULL, length);
    rev_win = xd_realloc(NULL, shift + shift);
    pre     = xd_realloc(NULL, shift);

    is = (short *)xx_realloc(NULL, length * sizeof(short));
    ix = (short *)xx_realloc(NULL, shift * sizeof(short));

    /*----------------- setup windows ------------------*/
    for (i = 0; i < length; ++i)
      win[i] = 0.5 - 0.5 * cos(PI * 2.0 * (double)i / (double)length);
    for (i = -shift; i < shift; ++i)
      rev_win[i+shift]
	= (0.5 + 0.5 * cos(PI * (double)i/(double)shift)) / win[i+lhalf];

    /*----------------- initialize noise spectrum ------------*/
    nread = fread(is, sizeof(*is), length, srcfd);
    multirr(length, is, win, frame);
    for (i = length; i < lpow2; ++i) frame[i] = 0.0;
    rfft(m0, frame);
    for (i = j = 0; j <= lpow2; ++i, j += 2) {
      noise[i] = sqrt(frame[j] * frame[j] + frame[j+1] * frame[j+1]);
    }
    for (i = 0; i < shift; ++i)
	pre[i] = 0.0;

    nread = getfirst(length, lhalf-shift, is, srcfd);
    while(0 != nread) {
	multirr(length, is, win, frame);
	for (i = length; i < lpow2; ++i)
	  frame[i] = 0.0;
	rfft(m0, frame);
	for (i = j = 0; i <= lpow2; i += 2, ++j) {
	    ar = frame[i];
	    ai = frame[i+1];
	    power   = sqrt(ar * ar + ai * ai + 1.0e-30);
	    ar   /= power;
	    ai   /= power;
#if 0	    
	    if (power < noise[j] * smul) {
		noise[j] = noise[j] * lambda + (1.0 - lambda) * power;
		power = 0.0;
	    }else{
		power -= noise[j] * smul;
	    }
#else	//改为公式9
		kk = pow(power, 0.4) - 0.9 * pow(noise[j], 0.4);
		if (kk < 0) kk = 0;
		kk = pow(kk, (1/0.4));
		power = kk;	
#endif	    
	    frame[i]   = ar * power * DEFAULT_AGCKK;
	    frame[i+1] = ai * power * DEFAULT_AGCKK;
	}
	irfft(m0, frame);
	for (i = 0; i < shift; ++i) {
	    ar = pre[i] + frame[i+lhalf-shift] * rev_win[i];
	    ix[i] = (short)ar;
	}
	for (i = 0; i < shift; ++i) {
	    pre[i] = frame[i+lhalf] * rev_win[i+shift];
	}
	fwrite(ix, sizeof(*ix), shift, dstfd);
	nread = rdframe(length, shift, is, srcfd);
    }

    fclose(srcfd);
    fclose(dstfd);
}

usage(char *name)
{
    fprintf(stderr,"\nSpeech enhancing by spectral subtruction.\n");
    fprintf(stderr,"Usage:\n\t%s -m[a] -l[n] -s[k] -r[a]", name);
    fprintf(stderr,"<source degraded speech file> <destination ehnanced speech file>\n");
    fprintf(stderr,"Switches:\n");
    fprintf(stderr,"\t-m[a] : set subtruct mutiple = a (default = %f).\n",
	    DEFAULT_MULTIPLE);
    fprintf(stderr,"\t-l[n] : set frame length = n (default = %d).\n",
	    DEFAULT_LENGTH);
    fprintf(stderr,"\t-s[k] : set frame shift = k (default = %d).\n",
	    DEFAULT_SHIFT);
    fprintf(stderr,"\t-r[a] : set noise spectra learning factor (default = %f).\n",
	    DEFAULT_SMOOTHING);
    fprintf(stderr,"Note:\n");
    fprintf(stderr,"\tInitial noise spectrum is estimated from the first frame.\n");
    fprintf(stderr,"(Assuming that the first consists of noise only.)\n");
    exit(0);
}
