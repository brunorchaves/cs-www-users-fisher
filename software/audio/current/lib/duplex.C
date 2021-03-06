/* Audio library for MIPS   AJF	  December 1996 */

#include <stdio.h>	//

#include "fishaudio.h"
#include "private.h"


void audio::setduplex(int d)
  { fprintf(stderr, "[%d: ", d); //
    control(AU_SETIFILL, 1);					/* set input and output non-blocking */
    control(AU_SETOFILL, MAXSAMPLES);
    int ni, no, nh;
    gdelay(ni, no, nh);
    until (ni < no && ni+no+nh == d)				/* aim for Tx full, Rx empty */
      { until (ni < no && ni+no+nh == d)
	  { if (ni >= no || ni+no+nh < d) { write(0); no++; }	/* delay too small, prime output buffer */
	    if (ni >= no || ni+no+nh > d) { read(); ni--; }	/* delay too big, discard input */
	  }
	gdelay(ni, no, nh);
      }
    control(AU_SETIFILL, d/2);					/* set blocking points to half-way */
    control(AU_SETOFILL, d/2);
    fprintf(stderr, "]\r\n"); //
  }

#define XX 0

static uchar hwdtab[8] =
  { /* table of h/w delays at various sample rates */
    0, 90, 0, 66, 60, 0, 0, 0	/* 24000, 12000, 9600 */
  };

void audio::gdelay(int &ni, int &no, int &nh)	/* private */
  { int ni1, ni2, no1, no2;
    ni2 = icount();
    no2 = ocount();
    do
      { ni1 = ni2; no1 = no2;
	ni2 = icount();
	no2 = ocount();
      }
    until (ni1 == ni2 && no1 == no2);
    int hwd = hwdtab[(srate >> 4) & 7];
    int tot = ni1 + no1 + hwd;
    fprintf(stderr, "%d+%d+%d=%d ", ni1, no1, hwd, tot); //
    ni = ni1; no = no1; nh = hwd;
  }

