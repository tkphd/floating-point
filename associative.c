/**
   \file   associative.c
   \brief  Explore floating-point rounding errors using float and MPFR data types

   For built-in floating point representation, compile and run using

   $ gcc -Wall associative.c -o associative && ./associative
   |  a             b             c           |  d             l             r           |
   |  1.000000000   1.000000000  -1.000000000 |  1.000000000   1.000000000   1.000000000 |
   |  0.250000000   1.000000000  -1.000000000 |  0.250000000   0.250000000   0.250000000 |
   |  0.062500000   1.000000000  -1.000000000 |  0.062500000   0.062500000   0.062500000 |
   |  0.015625000   1.000000000  -1.000000000 |  0.015625000   0.015625000   0.015625000 |
   |  0.003906250   1.000000000  -1.000000000 |  0.003906250   0.003906250   0.003906250 |
   |  0.000976562   1.000000000  -1.000000000 |  0.000976562   0.000976562   0.000976562 |
   |  0.000244141   1.000000000  -1.000000000 |  0.000244141   0.000244141   0.000244141 |
   |  0.000061035   1.000000000  -1.000000000 |  0.000061035   0.000061035   0.000061035 |
   |  0.000015259   1.000000000  -1.000000000 |  0.000015259   0.000015259   0.000015259 |
   |  0.000003815   1.000000000  -1.000000000 |  0.000003815   0.000003815   0.000003815 |

   For GNU MPFR floating-point representation, compile and run using

   $ gcc -Wall -include "mpfr.h" associative.c -o associative -lmpfr && ./associative
   |  a             b             c           |  d             l             r           |
   |  1.000000000   1.000000000  -1.000000000 |  1.000000000   1.000000000   1.000000000 |
   |  0.250000000   1.000000000  -1.000000000 |  0.250000000   0.250000000   0.250000000 |
   |  0.062500000   1.000000000  -1.000000000 |  0.062500000   0.062500000   0.062500000 |
   |  0.015625000   1.000000000  -1.000000000 |  0.015625000   0.015625000   0.015625000 |
   |  0.003906250   1.000000000  -1.000000000 |  0.003906250   0.003906250   0.003906250 |
   |  0.000976562   1.000000000  -1.000000000 |  0.000976562   0.000976562   0.000976562 |
   |  0.000244141   1.000000000  -1.000000000 |  0.000244141   0.000244141   0.000244141 |
   |  0.000061035   1.000000000  -1.000000000 |  0.000061035   0.000061035   0.000061035 |
   |  0.000015259   1.000000000  -1.000000000 | -0.000000000  -0.000000000   0.000015259 |
   associative: associative.c:96: main: Assertion `mpfr_get_flt(l, MPFR_RNDD) == mpfr_get_flt(r, MPFR_RNDD)' failed.
   Aborted
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

int main()
{
    int x, check;

    const char i[12] = {'a', '\0',
                        'b', '\0',
                        'c', '\0',
                        'd', '\0',
                        'l', '\0',
                        'r', '\0'};

    printf("|  %-12s  %-12s  %-12s|  %-12s  %-12s  %-12s| equal |\n",
               i+0,    i+2,  i+4,    i+6,   i+8,   i+10);

    for (x = 2; x < 33; x += 2) {
        check = 0;

        #ifdef __GNU_MP__
        mpfr_set_default_prec(4*sizeof(float)); /* expect 7 digits of precision */

        /* initialize variables */
        mpfr_t a, b, c, d, l, r;
        mpfr_init_set_d(a, 1./x, MPFR_RNDD);
        mpfr_init_set_d(b, 1, MPFR_RNDD);
        mpfr_init_set_d(c,-1, MPFR_RNDD);
        mpfr_init_set_d(d, 0, MPFR_RNDD);
        mpfr_init_set_d(l, 0, MPFR_RNDD);
        mpfr_init_set_d(r, 0, MPFR_RNDD);

        /* compute associative expressions */
        mpfr_add(d, a, b, MPFR_RNDD);
        mpfr_add(d, d, c, MPFR_RNDD);

        mpfr_add(l, a, b, MPFR_RNDD);
        mpfr_add(l, l, c, MPFR_RNDD);

        mpfr_add(r, b, c, MPFR_RNDD);
        mpfr_add(r, r, a, MPFR_RNDD);

        /* report values */
        if (mpfr_get_flt(d, MPFR_RNDD) == mpfr_get_flt(l, MPFR_RNDD) &&
            mpfr_get_flt(l, MPFR_RNDD) == mpfr_get_flt(r, MPFR_RNDD) ) {
            check = 1;
        }

        printf("| %12.9f  %12.9f  %12.9f | %12.9f  %12.9f  %12.9f | %-5d |\n",
               mpfr_get_flt(a, MPFR_RNDD), mpfr_get_flt(b, MPFR_RNDD),
               mpfr_get_flt(c, MPFR_RNDD), mpfr_get_flt(d, MPFR_RNDD),
               mpfr_get_flt(l, MPFR_RNDD), mpfr_get_flt(r, MPFR_RNDD),
               check);

        /*
          assert(mpfr_get_flt(d, MPFR_RNDD) == mpfr_get_flt(l, MPFR_RNDD));
          assert(mpfr_get_flt(l, MPFR_RNDD) == mpfr_get_flt(r, MPFR_RNDD));
        */

        /* clean up */
        mpfr_clear(a);
        mpfr_clear(b);
        mpfr_clear(c);
        mpfr_clear(d);
        mpfr_clear(l);
        mpfr_clear(r);

        #else
        /* initialize variables */
        float *a, *b, *c, *d, *l, *r;
        a = (float*)malloc(sizeof(float));
        b = (float*)malloc(sizeof(float));
        c = (float*)malloc(sizeof(float));
        d = (float*)malloc(sizeof(float));
        l = (float*)malloc(sizeof(float));
        r = (float*)malloc(sizeof(float));

        *a = 1./x;
        *b = 1;
        *c =-1;

        /* compute associative expressions */
        *d =  *a + *b + *c;
        *l = (*a + *b)+ *c;
        *r =  *a +(*b + *c);

        /* report values */
		if (*d == *l && *l == *r) {
            check = 1;
        }
        printf(     "| %12.9f  %12.9f  %12.9f | %12.9f  %12.9f  %12.9f | %-5d |\n",
                    *a,     *b,     *c,      *d,     *l,     *r, check);

        /*
          assert(*d == *l);
          assert(*l == *r);
        */

        /* clean up */
        free(a);
        free(b);
        free(c);
        free(d);
        free(l);
        free(r);

        #endif
    }

	#ifdef __GNU_MP__
    mpfr_free_cache();
    #endif

    return 0;
}
